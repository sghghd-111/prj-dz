#ifndef _BC_FUNC_H_
#define _BC_FUNC_H_
#include "udp_func.h"
#include <pthread.h>

#define ETH_DEV_FILE				"/proc/net/dev"
#define ETH_NET_NUM				4

typedef struct _net_dev_info_
{
	char net_dev[ETH_NET_NAME_SIZE];
	char ip_addr[IPADDR_SIZE];
	void *hdl;
	int fd;
}tpNET_DEV_INFO, *pNET_DEV_INFO; 

typedef struct _bc_info_
{
	tpNET_DEV_INFO devs[ETH_NET_NUM];
	int dev_num;
	int bind_port;
	int thread_flag;
	pthread_t thread_id;
}tpBC_INFO, *pBC_INFO;

extern void *bc_func_init(int bind_port);
extern void bc_func_release(void *hdl);
#endif

