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

tpUDP_INFO udp_info;

static int get_ipaddr_by_dev_name(char *dev, char *ipaddr)
{
	int fd = -1;
	struct sockaddr_in *sin;
	struct ifreq ifr_ip;
	int ret = -1;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > fd)
		return -1;
	memset(&ifr_ip, 0, sizeof(struct ifreq));
//	strncpy(ifr_ip.ifr_name, dev, strlen(dev));
	strcpy(ifr_ip.ifr_name, dev);

	ret = ioctl(fd, SIOCGIFADDR, &ifr_ip);
	if (0 > ret)
		goto IOCTL_ERR;
	sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;
	strcpy(ipaddr, (char *)inet_ntoa(sin->sin_addr));
	printf("dev[%s], ipaddr[%s]\n", dev, ipaddr);
IOCTL_ERR:
	close(fd);
	return -1;
}

static void devs_ipaddr_refresh(pUDP_INFO pui)
{
//	int ret = -1;
	int j = 0;

	for (j=0; j<pui->dev_num; j++)
	{
		if (get_ipaddr_by_dev_name(pui->devs[j].net_dev, pui->devs[j].ip_addr))
			continue;
	}
}

static int bind_eth_net(int fd, char *net_id)
{
	int ret = 0;
	struct ifreq interface;

	strncpy(interface.ifr_ifrn.ifrn_name, net_id, strlen(net_id)+1);
	ret = setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interface, sizeof(interface));
	if (ret < 0) 
	{
		perror("SO_BINDTODEVICE failed");
	}
	return ret;
}

#define NETDEV_STOP_CHAR		':'
#define NETDEV_TRIM_CHAR		' '

static int netdev_line_parse(char *str, char *out)
{
	char *pt = strrchr(str, NETDEV_STOP_CHAR);
	
	if (!pt)
		return -1;
	*pt = '\0';
	pt = strrchr(str, NETDEV_TRIM_CHAR);
	if (!pt)
		return -2;
	strcpy(out, pt+1);
	return 0;
}

static void netdev_file_parse(pUDP_INFO pui, char *path)
{
	FILE *fp;
	char buff[1024];
	char buf_out[32];

	fp = fopen(ETH_DEV_FILE, "rb");
	if (!fp)
	{
		perror("file open:");
		return;
	}
	while (fgets(buff, 1024, fp))
	{
		if (!netdev_line_parse(buff, buf_out))
		{
			strcpy(pui->devs[pui->dev_num].net_dev, buf_out);
			pui->dev_num++;
			printf("netdev: [%s]\n", buf_out);
		}
		else
			printf("fail: [%s]\n", buff);
	}
}

void *udp_init(int port)
{
	pUDP_INFO pu = NULL;
	struct sockaddr_in servaddr;
	int tmp = 1;

	pu = (pUDP_INFO)malloc(sizeof(tpUDP_INFO));
	if (!pu)
	{
		printf("%s malloc err\n",  __FUNCTION__);
		goto MALLOC_ERR;
	}
	memset(pu, 0, sizeof(tpUDP_INFO));
	pu->local.local_port = port;

	pu->local.fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (0 > pu->local.fd)
	{
       		printf("socket error\n");
		goto SOCK_ERR;
	}
	if (bind_eth_net(pu->local.fd, "eth0"))
		printf("\n");
	setsockopt(pu->local.fd, SOL_SOCKET, SO_BROADCAST, &tmp, sizeof(tmp));
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(pu->local.local_port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	printf("监听%d端口\n", pu->local.local_port);
	if (0 > bind(pu->local.fd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
	{
		printf("bind error\n");
		goto BIND_ERR;
	}
	return pu;
BIND_ERR:
	close(pu->local.fd);
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
	FD_SET(pu->local.fd, &tfds);

	tv.tv_sec = 1;
	tv.tv_usec = 100000;

	ret = select(pu->local.fd+1, NULL,  &tfds, NULL, &tv);

	if (0 > ret)
		return 0;
	else if (0 < ret)
	{
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(pr->rem_port);
		addr.sin_addr.s_addr = inet_addr(pr->rem_ip);

		ret = sendto(pu->local.fd, buff, len, 0, (struct sockaddr *)&addr, sizeof(addr));
		printf("sendto %s, %d, ret %d\n", pr->rem_ip, pr->rem_port, ret);
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
	FD_SET(pu->local.fd, &rfds);

	tv.tv_sec = 1;
	tv.tv_usec = 100000;

	ret = select(pu->local.fd+1, &rfds, NULL, NULL, &tv);

	if (0 > ret)
		perror("select()");
	else if (0 < ret)
	{
		struct sockaddr_in addr;
		socklen_t addrlen;

		addrlen = sizeof(addr);
		ret = recvfrom(pu->local.fd, buff, len, 0,
		             (struct sockaddr *)&addr, &addrlen);
		if (0 < ret)
		{
			sprintf(pr->rem_ip, "%s", (char *)inet_ntoa(addr.sin_addr));
			pr->rem_port = htons(addr.sin_port);
			printf("IP:%s\n", (char *)inet_ntoa(addr.sin_addr));
			printf("Port:%d\n", htons(addr.sin_port));
		//	pr->rem_port = addr.
		}
	}
//	else
//		printf("recv wait timeout.\n");
	
	return ret;
}

static int build_ipaddr_string(pUDP_INFO pui, char *buff)
{
	int j = 0;
	int ret= 0;

	for (j=0; j<pui->dev_num; j++)
	{
		ret += sprintf(&buff[ret], "%s\n", pui->devs[j].ip_addr);
	}
	return ret;
}

void *recv_func(void *hdl)
{
	tpREM_INFO rem;
	char buff[512];
	char buff_out[512];
	int len;

	while (1)
	{
		len = udp_read(hdl, &rem, buff, 512);
		if (0 < len)
		{
			buff[len] = 0;
			printf("recv:%s, len=%d\n", buff, len);
			if (!strcmp(buff, "ipaddr get"))
			{
				len = build_ipaddr_string(&udp_info, buff_out);
				buff_out[len] = 0;
				printf("bo %d %s\n", len, buff_out);
				sprintf(rem.rem_ip, "%s", "255.255.255.255");
//				rem.rem_port = 10999;
				udp_write(hdl, &rem, buff_out, len);
			}
			else
				printf("err string??!!!\n");
		}
	}
}

int main(int argc, char *argv[])
{
	void *hdl = NULL;
	pthread_t thread_id;
	tpREM_INFO rem;
	char buff[128];
	int ret = 0;
	int role = 0;
	int local_port = 10999;

	if (!strcmp(argv[1], "1"))
		role = 1;//server

	if (role)
		local_port = 11000;
	else
	{
		sprintf(rem.rem_ip, "%s", "255.255.255.255");
		rem.rem_port = 11000;
	}
	memset(&udp_info, 0, sizeof(tpUDP_INFO));
	netdev_file_parse(&udp_info, NULL);
	devs_ipaddr_refresh(&udp_info);
	ret = sprintf(buff, "ipaddr get");
	hdl = udp_init(local_port);
	pthread_create(&thread_id, 0, recv_func, hdl);

	while (1)
	{
		if (role)
		{}
		else
		{
			ret = udp_write(hdl, &rem, buff, ret);
			if (0 >= ret)
				printf("ai......\n");
		}
		sleep(5);
	}
	return 0;
}

