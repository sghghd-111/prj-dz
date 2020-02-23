#ifndef _UDP_FUNC_H_
#define _UDP_FUNC_H_

#define ETH_NET_TMP_SIZE			8

#define ETH_NET_NAME_SIZE			16

#define ETH_DEV_FILE				"/proc/net/dev"

#define ETH_NET_NUM				4

#define IPADDR_SIZE					16

typedef struct _net_dev_info_
{
	char net_dev[ETH_NET_NAME_SIZE];
	char ip_addr[IPADDR_SIZE];
}tpNET_DEV_INFO, *pNET_DEV_INFO;

typedef struct _local_info_
{
	int local_port;
	int fd;
//	char *
}tpLOCAL_INFO, *pLOCAL_INFO;

typedef struct _rem_info_
{
	char rem_ip[16];
	int rem_port;
}tpREM_INFO, *pREM_INFO;

typedef struct _udp_info_
{
	tpLOCAL_INFO local;
	tpNET_DEV_INFO devs[ETH_NET_NUM];
	int dev_num;
}tpUDP_INFO, *pUDP_INFO;


#endif

