#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "udp_func.h"
#include <pthread.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include "plat_include.h"

typedef struct _UDP_info_
{
	int local_port;
	int fd;
	char *net_dev;
}tpUDP_INFO, *pUDP_INFO;


static int bind_eth_net(int fd, char *net_id)
{
	int ret = 0;
	struct ifreq interface;

	strncpy(interface.ifr_ifrn.ifrn_name, net_id, strlen(net_id)+1);
	ret = setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interface, sizeof(interface));
	if (ret < 0) 
	{
		LOG_E("SO_BINDTODEVICE failed");
	}
	return ret;
}

void *udp_init(int port, char *net_dev)
{
	pUDP_INFO pu = NULL;
	struct sockaddr_in servaddr;
	int tmp = 1;

	pu = (pUDP_INFO)malloc(sizeof(tpUDP_INFO));
	if (!pu)
	{
		LOG_E("%s malloc err\n",  __FUNCTION__);
		goto MALLOC_ERR;
	}
	memset(pu, 0, sizeof(tpUDP_INFO));
	pu->local_port = port;
	pu->net_dev = net_dev;
	pu->fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (0 > pu->fd)
	{
       		LOG_E("socket error\n");
		goto SOCK_ERR;
	}
	if (bind_eth_net(pu->fd, pu->net_dev))
	{
		LOG_E("bind eth net err\n");
		goto BIND_LOCAL_DEV_ERR;
	}
	setsockopt(pu->fd, SOL_SOCKET, SO_BROADCAST, &tmp, sizeof(tmp));
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(pu->local_port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	LOG_I("listen net[%s], port[%d]\n", pu->net_dev, pu->local_port);
	if (0 > bind(pu->fd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
	{
		LOG_E("bind error\n");
		goto BIND_ERR;
	}
	return pu;
BIND_ERR:
BIND_LOCAL_DEV_ERR:
	close(pu->fd);
SOCK_ERR:
	free(pu);
MALLOC_ERR:
	return NULL;
}

int udp_write(void *hdl, pREM_INFO pr, char *buff, int len)
{
	int ret = 0;
	fd_set tfds;
	struct timeval tv;
	pUDP_INFO pu = (pUDP_INFO)hdl;

	FD_ZERO(&tfds);
	FD_SET(pu->fd, &tfds);

	tv.tv_sec = 1;
	tv.tv_usec = 100000;

	ret = select(pu->fd+1, NULL,  &tfds, NULL, &tv);

	if (0 > ret)
		return 0;
	else if (0 < ret)
	{
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(pr->rem_port);
		addr.sin_addr.s_addr = inet_addr(pr->rem_ip);

		ret = sendto(pu->fd, buff, len, 0, (struct sockaddr *)&addr, sizeof(addr));
		LOG_I("sendto %s, %d, ret %d\n", pr->rem_ip, pr->rem_port, ret);
//		perror("udp send:");
	}
//	else
//	printf("send wait timeout.\n");

	return ret;
}

int udp_read(void *hdl, pREM_INFO pr, char *buff, int len)
{
	int ret = 0;
	fd_set rfds;
	struct timeval tv;
	pUDP_INFO pu = (pUDP_INFO)hdl;

	FD_ZERO(&rfds);
	FD_SET(pu->fd, &rfds);

	tv.tv_sec = 1;
	tv.tv_usec = 100000;

	ret = select(pu->fd+1, &rfds, NULL, NULL, &tv);

	if (0 > ret)
		perror("select()");
	else if (0 < ret)
	{
		struct sockaddr_in addr;
		socklen_t addrlen;

		addrlen = sizeof(addr);
		ret = recvfrom(pu->fd, buff, len, 0,
		             (struct sockaddr *)&addr, &addrlen);
		if (0 < ret)
		{
			sprintf(pr->rem_ip, "%s", (char *)inet_ntoa(addr.sin_addr));
			pr->rem_port = htons(addr.sin_port);
			LOG_I("IP:%s\n", (char *)inet_ntoa(addr.sin_addr));
			LOG_I("Port:%d\n", htons(addr.sin_port));
		//	pr->rem_port = addr.
		}
	}
//	else
//		printf("recv wait timeout.\n");
	
	return ret;
}

int udp_block_read(void *hdl, pREM_INFO pr, char *buff, int len)
{
	int ret = 0;
	pUDP_INFO pu = (pUDP_INFO)hdl;
	struct sockaddr_in addr;
	socklen_t addrlen;

	addrlen = sizeof(addr);
	ret = recvfrom(pu->fd, buff, len, 0,
	             (struct sockaddr *)&addr, &addrlen);
	if (0 < ret)
	{
		sprintf(pr->rem_ip, "%s", (char *)inet_ntoa(addr.sin_addr));
		pr->rem_port = htons(addr.sin_port);
		LOG_I("IP:%s\n", (char *)inet_ntoa(addr.sin_addr));
		LOG_I("Port:%d\n", htons(addr.sin_port));
	}

	return ret;
}

int udp_get_fd(void *hdl)
{
	pUDP_INFO pu = (pUDP_INFO)hdl;

	return pu->fd;
}

void udp_release(void *hdl)
{
	pUDP_INFO pu = (pUDP_INFO)hdl;
	
	if (!hdl)
		return;
	if (pu->fd)
		close(pu->fd);
	free(hdl);
}




