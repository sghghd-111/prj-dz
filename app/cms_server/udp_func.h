#ifndef _UDP_FUNC_H_
#define _UDP_FUNC_H_

#define ETH_NET_TMP_SIZE			8

#define ETH_NET_NAME_SIZE			16

#define IPADDR_SIZE				16

typedef struct _rem_info_
{
	char rem_ip[16];
	int rem_port;
}tpREM_INFO, *pREM_INFO;

extern void *udp_init(int port, char *net_dev);
extern int udp_write(void *hdl, pREM_INFO pr, char *buff, int len);
extern int udp_read(void *hdl, pREM_INFO pr, char *buff, int len);
extern int udp_block_read(void *hdl, pREM_INFO pr, char *buff, int len);
extern int udp_get_fd(void *hdl);
extern void udp_release(void *hdl);

#endif

