#include "bc_func.h"
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
#include "plat_log.h"

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

static void devs_ipaddr_refresh(pBC_INFO pbc)
{
//	int ret = -1;
	int j = 0;

	for (j=0; j<pbc->dev_num; j++)
	{
		if (get_ipaddr_by_dev_name(pbc->devs[j].net_dev, pbc->devs[j].ip_addr))
			continue;
	}
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

static void netdev_file_parse(pBC_INFO pbc, char *path)
{
	FILE *fp;
	char buff[1024];
	char buf_out[32];

	fp = fopen(ETH_DEV_FILE, "rb");
	if (!fp)
	{
		LOG_E("file open:");
		return;
	}
	while (fgets(buff, 1024, fp))
	{
		if (!netdev_line_parse(buff, buf_out))
		{
			if (!strcmp(buf_out, "lo"))
				continue;
			if (!strcmp(buf_out, "usb0"))
				continue;
			strcpy(pbc->devs[pbc->dev_num].net_dev, buf_out);
			pbc->dev_num++;
			LOG_E("netdev: [%s]\n", buf_out);
		}
		else
			LOG_E("fail: [%s]\n", buff);
	}
}

static int build_ipaddr_string(pBC_INFO pbc, char *buff)
{
	int j = 0;
	int ret= 0;

	for (j=0; j<pbc->dev_num; j++)
	{
		ret += sprintf(&buff[ret], "%s\n", pbc->devs[j].ip_addr);
	}
	return ret;
}

static int comm_read(pBC_INFO pbc, pREM_INFO pr, char *buff, int len, int *seq)
{
	int ret = 0;
	int j = 0;
	int maxfd = 0;
	fd_set rfds;
	struct timeval tv;
//	struct sockaddr_in addr;
//	socklen_t addrlen;

	FD_ZERO(&rfds);
	for (j=0; j<pbc->dev_num; j++)
	{
		FD_SET(pbc->devs[j].fd, &rfds);
		if (pbc->devs[j].fd > maxfd)
			maxfd = pbc->devs[j].fd;
	}

	tv.tv_sec = 1;
	tv.tv_usec = 100000;

	ret = select(maxfd+1, &rfds, NULL, NULL, &tv);

	if (0 > ret)
		perror("select()");
	else if (0 < ret)
	{
		for (j=0; j<pbc->dev_num; j++)
		{
			if (!FD_ISSET(pbc->devs[j].fd, &rfds))
				continue;
//			addrlen = sizeof(addr);
			ret = udp_block_read(pbc->devs[j].hdl, pr, buff, len);
			*seq = j;
			break;
		}
	}
//	else
//		printf("recv wait timeout.\n");
	
	return ret;
}

void *recv_func(void *hdl)
{
	pBC_INFO pbc = (pBC_INFO)hdl;
	int seq = -1;
	tpREM_INFO rem;
	char buff[512];
	char buff_out[512];
	int len;
//	fd_set rfds;
//	struct timeval tv;

	while (pbc->thread_flag)
	{
		len = comm_read(pbc, &rem, buff, 512, &seq);
		if (0 < len)
		{
			buff[len] = 0;
			LOG_I("recv:%s, len=%d\n", buff, len);
			if (!strcmp(buff, "ipaddr get"))
			{
				len = build_ipaddr_string(pbc, buff_out);
				buff_out[len] = 0;
				LOG_I("bo %d %s\n", len, buff_out);
				sprintf(rem.rem_ip, "%s", "255.255.255.255");
//				rem.rem_port = 10999;
				len = udp_write(pbc->devs[seq].hdl, &rem, buff_out, len);
			}
			else
				LOG_I("err string!!!\n");
		}
#if 0
		else if (0 == len)
		{
			rem.rem_port = 10888;
			udp_write(pbc->devs[2].hdl, &rem, "ipaddr get", strlen("ipaddr get")+1);
		}
#endif
	}

	return NULL;
}

static void net_dev_release(pBC_INFO pbc)
{
	int j = 0;

	for (j=0; j<pbc->dev_num; j++)
	{
		if (pbc->devs[j].hdl)
			udp_release(pbc->devs[j].hdl);
	}
}

static int net_dev_init(pBC_INFO pbc)
{
	int j = 0;
	int ret = 0;

	for (j=0; j<pbc->dev_num; j++)
	{
		pbc->devs[j].hdl = udp_init(pbc->bind_port, pbc->devs[j].net_dev);
		if (!pbc->devs[j].hdl)
		{
			net_dev_release(pbc);
			ret = 1;
			LOG_E("%s udp init err\n", pbc->devs[j].net_dev);
			break;
		}
		pbc->devs[j].fd = udp_get_fd(pbc->devs[j].hdl);
	}
	return ret;
}

void *bc_func_init(int bind_port)
{
	pBC_INFO pbc = NULL;

	pbc = (pBC_INFO)malloc(sizeof(tpBC_INFO));
	if (!pbc)
	{
		LOG_E("%s, malloc err\n", __FUNCTION__);
		goto BC_MALLOC_ERR;
	}
	pbc->bind_port = bind_port;
	netdev_file_parse(pbc, NULL);
	devs_ipaddr_refresh(pbc);
	if (net_dev_init(pbc))
	{
//		LOG_E("");
		goto NET_UDP_INIT_ERR;
	}
	pbc->thread_flag = 1;
	if (pthread_create(&pbc->thread_id, 0, recv_func, pbc))
	{
		LOG_E("thread init err\n");
		goto THREAD_INIT_ERR;
	}

	return pbc;
THREAD_INIT_ERR:
	net_dev_release(pbc);
NET_UDP_INIT_ERR:
	free(pbc);
BC_MALLOC_ERR:
	return NULL;
}

void bc_func_release(void *hdl)
{
	void *stat;
	pBC_INFO pbc = (pBC_INFO)hdl;

	if (!pbc)
		return;
	pbc->thread_flag = 0;
	pthread_join(pbc->thread_id, &stat);
	net_dev_release(pbc);
	free(pbc);
}

/*
int main(int argc, char *argv[])
{
	void *hdl = NULL;

	hdl = bc_func_init(10999);
	if (!hdl)
		return 0;

	while (1)
	{
		sleep(5);
	}
	return 0;
}
*/


