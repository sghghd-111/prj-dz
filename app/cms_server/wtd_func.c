#include "wtd_func.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <linux/watchdog.h>
#include "plat_include.h"

tpWTD_INFO wtd;

static int wtd_feed_dog(pWTD_INFO wtd_info)
{
	int ret = 0;

	ret = ioctl(wtd_info->fd, WDIOC_KEEPALIVE, 0);
	if (!ret)
		wtd_info->feed_cnt++;

	return ret;
}

void *wtd_thread(void *para)
{
	int cnt = 0;
	int flag = 0;
	pWTD_INFO wtd_info = (pWTD_INFO)para;

	LOG_I("watchdog thread running now...\n");
	flag = WDIOS_ENABLECARD;// WDIOS_DISABLECARD;
	ioctl(wtd_info->fd, WDIOC_SETOPTIONS, &flag);

	while (wtd_info->thread_run)
	{
		cnt++;
		if (wtd_info->feed_set < cnt)
		{
			wtd_feed_dog(wtd_info);
			cnt = 0;
		}
		app_ms_sleep(1000);
	}

	LOG_W("watchdog thread end...\n");

	return NULL;
}

int wtd_init(void)
{
//	int ret = 0;
	int tout = WTD_MAX_TOUT;

	wtd.fd = open(WTD_DEV_PATH, O_RDWR);
	if (0 > wtd.fd)
	{
		LOG_E("%s open err\n", WTD_DEV_PATH);
		goto OPEN_ERR;
	}

	if (ioctl(wtd.fd, WDIOC_SETTIMEOUT, &tout))
	{
		LOG_E("watchdog set tout fail\n");
		goto SET_TOUT_FAIL;
	}

	wtd.tout_set = tout;
	wtd.feed_set = wtd.tout_set/2;

	wtd.thread_run = 1;
	if (pthread_create(&wtd.thread_id, NULL, wtd_thread, &wtd))
	{
		LOG_E("watchdog thread create fail...\n");
		goto THREAD_CREATE_FAIL;
	}
	return 0;
THREAD_CREATE_FAIL:
SET_TOUT_FAIL:
	close(wtd.fd);
OPEN_ERR:
	return -1;
}

int wtd_get_tout(int *tout)
{
	if (ioctl(wtd.fd, WDIOC_GETTIMEOUT, tout))
		return -1;

	return 0;
}

int wtd_set_tout(int tout)
{
	if ((WTD_MAX_TOUT<tout) || (WTD_MIN_TOUT>tout))
		return -1;

	wtd_feed_dog(&wtd);
	if (ioctl(wtd.fd, WDIOC_SETTIMEOUT, &tout))
		return -2;

	wtd.tout_set = tout;
	wtd.feed_set = tout/2;

	return 0;
}

void wtd_stop(void)
{
	void *status;
	int ret = 0;
	int flag = WDIOS_DISABLECARD;

	wtd.thread_run = 0;

	ret = pthread_join(wtd.thread_id, &status);
	if (ret)
		LOG_W("wait watchdog thread out fail...\n");
	ioctl(wtd.fd, WDIOC_SETOPTIONS, &flag);
}

void wtd_stat(void)
{
	LOG_I("watchdog hdl[%d], is-run[%d], feed-seq[%d], feed-cnt[%d]\n", 
		wtd.fd, wtd.thread_run, wtd.tout_set, wtd.feed_cnt);
}

void wtd_release(void)
{
	wtd_stop();
	close(wtd.fd);
}


