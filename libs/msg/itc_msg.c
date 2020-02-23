#include "itc_msg.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "plat_log.h"
//#include "plat_include.h"

void *itc_msg_init(void)
{
	pITC_INFO pii = NULL;

	pii = (pITC_INFO)malloc(sizeof(tpITC_INFO));
	if (!pii)
	{
		LOG_E("%s malloc err\n", __FUNCTION__);
		goto MALLOC_ERR;
	}
	memset(pii, 0, sizeof(tpITC_INFO));
	if (pipe(pii->pipe_fd))
	{
		LOG_E("%s pipe err\n", __FUNCTION__);
		goto PIPE_ERR;
	}
	if (sem_init(&pii->lock, 0, 1))
	{
		LOG_E("%s sem init err\n", __FUNCTION__);
		goto SEM_INIT_ERR;
	}

	return pii;
SEM_INIT_ERR:
	close(pii->pipe_fd[0]);
	close(pii->pipe_fd[1]);
PIPE_ERR:
	free(pii);
MALLOC_ERR:
	return NULL;
}

int itc_msg_send(void *hdl, int msg_id, char *msg_comm, int len)
{
	int ret = 0;
	tpITC_MSG msg;
	pITC_INFO pii = (pITC_INFO)hdl;

	if (ITC_MSG_SIZE < len)
		return ret;

	sem_wait(&pii->lock);
	msg.msg_id = msg_id;
	memcpy(msg.msg, msg_comm, len);
	ret = write(pii->pipe_fd[1], (void*)&msg, sizeof(tpITC_MSG));
	sem_post(&pii->lock);
	
	return ret;
}

int itc_msg_recv(void *hdl, pITC_MSG msg_info)
{
	int ret = 0;
	pITC_INFO pii = (pITC_INFO)hdl;

	ret = read(pii->pipe_fd[0], (void*)msg_info, sizeof(tpITC_MSG));
	return ret;
}

void itc_msg_release(void *hdl)
{
	pITC_INFO pii = (pITC_INFO)hdl;

	if (!pii)
		return;
	sem_close(&pii->lock);
	close(pii->pipe_fd[0]);
	close(pii->pipe_fd[1]);
	free(hdl);
}

