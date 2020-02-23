#include "pulse.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

void *pulse_init(void)
{
	pPULSE_INFO pulse_info = NULL;

	pulse_info = (pPULSE_INFO)malloc(sizeof(tpPULSE_INFO));
	if (!pulse_info)
		goto MALLOC_ERR;
	memset(pulse_info, 0, sizeof(tpPULSE_INFO));
	pulse_info->fd = open(PULSE_PATH, O_RDWR);
	if (0 > pulse_info->fd)
	{
		printf("open %s err\n", PULSE_PATH);
		goto OPEN_ERR;
	}printf("%p\n", pulse_info);
	return pulse_info;
OPEN_ERR:
	free(pulse_info);
MALLOC_ERR:
	return NULL;
}

void pulse_release(void *hdl)
{
	pPULSE_INFO pulse_info = (pPULSE_INFO)hdl;

	if (!hdl)
		return;
	if (pulse_info->fd)
		close(pulse_info->fd);
	free(hdl);
}

int pulse_irq_start(void *hdl)
{
	int ret = 0;
	pPULSE_INFO pulse_info = (pPULSE_INFO)hdl;

	ret = ioctl(pulse_info->fd, CMD_PULSE_START, 0);
	if (ret)
	{
		printf("error:%p, %d, %x\n", pulse_info, pulse_info->fd, CMD_PULSE_START);
		perror("pulse irq");
	}

	return ret;
}

int pulse_irq_stop(void *hdl)
{
	int ret = 0;
	pPULSE_INFO pulse_info = (pPULSE_INFO)hdl;

	ret = ioctl(pulse_info->fd, CMD_PULSE_STOP, NULL);

	return ret;	
}


