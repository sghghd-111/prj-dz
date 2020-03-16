#include "led_run.h"
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "comm_func.h"

int led_run_ctl(int stat)
{
	int fd;
	char buff;

	fd = open(LED_RUN_DEV, O_RDWR);
	if (0 > fd)
		return -1;
	if (stat)
		buff = '1';
	else
		buff = '0';

	write(fd, &buff, 1);

	close(fd);
	return 0;
}

static int led_run_flag = 0;
pthread_t led_th_id;

static void *led_run_func(void *param)
{
	int stat = 0;

	while (led_run_flag)
	{
		led_run_ctl(stat);
		stat = !stat;
		app_ms_sleep(1000);
	}
	return NULL;
}

int led_run_start(void)
{
	int ret = 0;

	led_run_flag = 1;
	ret = pthread_create(&led_th_id, NULL, led_run_func, NULL);

	return ret;
}

void led_run_stop(void)
{
	void *ret;

	led_run_flag = 0;
	pthread_join(led_th_id, &ret);
}
