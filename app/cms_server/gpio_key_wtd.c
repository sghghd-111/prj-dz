#include "gpio_key_wtd.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <linux/input.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "plat_log.h"

tpIO_INFO io_info;

void *peri_wtd_thread(void *param)
{
	pIO_INFO io = (pIO_INFO)param;
	char buf[8];
	int len = 0;
	int stat = 0;

printf("%s, %d\n", __FUNCTION__, __LINE__);
	while (io->run_flag)
	{
		len = sprintf(buf, "%d", stat);
		write(io->wdi_fd, buf, 1);
		sync();
		stat = !stat;
//		printf("%s, write %s to wtd\n", __FUNCTION__, buf);
		sleep(2);
	}
	return NULL;
}

static int peri_wtd_init(pIO_INFO io)
{
	int ret = 0;

	io->wdi_fd = open(GPIO_WTD, O_WRONLY);
	if (0 > io->wdi_fd)
	{
		LOG_E("%s open fail\n", GPIO_WTD);
		ret = -1;
	}
	pthread_create(&io->peri_wtd_id, NULL, peri_wtd_thread, io);
	return ret;
}

static void peri_wtd_release(pIO_INFO io)
{
	pthread_join(io->peri_wtd_id, NULL);
	close(io->wdi_fd);
}

static void *key_func(void *param)
{
	pIO_INFO io = (pIO_INFO)param;
	struct input_event ev_key;
	int ret;

	while (io->run_flag)
	{
		ret = read(io->key_fd, &ev_key, sizeof(struct input_event));
		if (sizeof(struct input_event) != ret)
		{
			LOG_W("read ret len err %d, %d\n", ret, sizeof(struct input_event));
			continue;
		}
		printf("type:%d, code:%d, value:%d \n", ev_key.type,ev_key.code,ev_key.value);
		if (EV_KEY != ev_key.type)
		{
//			LOG_W("ev-key type is not EV-KEY\n");
			continue;
		}
		if (KEY_UNDO != ev_key.code)
			continue;
		if (!ev_key.value)
			continue;
		LOG_I("recover setting...\n");
		system(RECOVER_SHELL_PATH);
	}
	return NULL;
}

static int key_init(pIO_INFO io)
{
	int ret = 0;

	io->key_fd = open(RECOVER_EVENT, O_RDWR);
	if (!io->key_fd)
	{
		LOG_E("%s open fail\n", RECOVER_EVENT);
		return -1;
	}
	pthread_create(&io->key_id, NULL, key_func, io);

	return ret;
}

int kw_init(void)
{
	io_info.run_flag = 1;

	peri_wtd_init(&io_info);
	key_init(&io_info);

	return 0;
}

void kw_release(void)
{
	
}

