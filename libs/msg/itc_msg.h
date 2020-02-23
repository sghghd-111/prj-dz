#ifndef _ITC_MSG_H_
#define _ITC_MSG_H_
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#define ITC_MSG_SIZE			128

typedef struct _itc_msg_
{
	int msg_id;
	char msg[ITC_MSG_SIZE];
}tpITC_MSG, *pITC_MSG;

typedef struct _itc_info_
{
	sem_t lock;
	int pipe_fd[2];						//0:read 1:write
}tpITC_INFO, *pITC_INFO;

#endif

