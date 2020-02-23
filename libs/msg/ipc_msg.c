#include "ipc_msg.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "plat_log.h"

static int sock_init(pPROCESS_INFO pi)
{
	int ret;
	struct sockaddr_un srv_addr;

	pi->listen_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	if(0 > pi->listen_fd)
	{
		LOG_E("cannot create communication socket");
		goto SOCK_ERR;
	}
	//set server addr_param
	srv_addr.sun_family = AF_UNIX;
	sprintf(srv_addr.sun_path, "%s%s", PRO_FILE_PATH, pi->name);
//	strcpy(srv_addr.sun_path, pi->name);
	unlink(srv_addr.sun_path);
	//bind sockfd & addr
	ret = bind(pi->listen_fd, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
	if(0 > ret)
	{
		LOG_E("cannot bind server socket");
		goto BIND_ERR;
	}
	//listen sockfd
	ret = listen(pi->listen_fd,1);
	if(0 > ret)
	{
		LOG_E("cannot listen the client connect request");
		goto LISTEN_ERR;
	}
	return 0;
LISTEN_ERR:
BIND_ERR:
	close(pi->listen_fd);
	unlink(pi->name);
SOCK_ERR:
	return -1;
}

static int sock_recv(pPROCESS_INFO pi, char *pmsg, int msg_len)
{
//	socklen_t clt_addr_len;
//	int ret;
	int len;
	struct sockaddr_un clt_addr;

	len = sizeof(struct sockaddr_un);
	pi->recv_fd = accept(pi->listen_fd, (struct sockaddr*)&clt_addr, (socklen_t*)&len);
	if(pi->recv_fd < 0)
	{
		LOG_E("cannot accept client connect request");
//		close(pi->listen_fd);
		return 0;
	}
	len = read(pi->recv_fd, pmsg, msg_len);
	return len;
}

int sock_send(pPROCESS_INFO pi,  char *target, char *pmsg, int msg_len)
{
//	int len = 0;
	int ret = 0;
	int sockfd;
	struct sockaddr_un addr;

	sockfd = socket(PF_UNIX, SOCK_STREAM, 0);
	if(0 > sockfd)
	{
		LOG_E("cannot create communication socket");
		return -1;
	}
	/*len =*/ sprintf(addr.sun_path, "%s%s", PRO_FILE_PATH, target);
	addr.sun_family = AF_UNIX;  

	ret = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
	if (ret)
	{
		LOG_E("connect err\n");
		return -1;
	}
	ret = write(sockfd, pmsg, msg_len);
	close(sockfd);
	return ret;
}

void *ipc_msg_init(char *name, MSG_CB cb, void *usr)
{
//	int len = 0;
	pPROCESS_INFO pi = NULL;

	pi = (pPROCESS_INFO)malloc(sizeof(tpPROCESS_INFO));
	if (!pi)
		goto MALLOC_ERR;
	memset(pi, 0, sizeof(tpPROCESS_INFO));
	pi->cb = cb;
	pi->usr = usr;
//	len = sprintf(pi->name, "%s%s", PRO_FILE_PATH, name);
	strcpy(pi->name, name);
	if (sock_init(pi))
	{
		LOG_E("ipc msg init socket err\n");
		goto SOCK_INIT;
	}
	return pi;
SOCK_INIT:
	free(pi);
MALLOC_ERR:
	return NULL;
}

int ipc_msg_send(void *hdl, char *target,  int msg_id, void *msg, int msg_len)
{
	pPROCESS_INFO pi = (pPROCESS_INFO)hdl;
	tpMSG_INFO msg_tmp;

	msg_tmp.msg_id = msg_id;
	strcpy(msg_tmp.src, pi->name);
	memcpy(msg_tmp.msg_comm, msg, msg_len);
	return sock_send(pi, target, (void *)&msg_tmp, sizeof(tpMSG_INFO));
}

int ipc_msg_listen(void *hdl)
{
	int ret = 0;
	pPROCESS_INFO pi = (pPROCESS_INFO)hdl;

	do
	{
		ret = sock_recv(pi, (char *)&pi->msg, sizeof(tpMSG_INFO));
		if (ret && pi->cb)
			pi->cb(&pi->msg, pi->usr);
		close(pi->recv_fd);
	}while (1);
	
	return 0;
}

void ipc_msg_release(void *hdl)
{
//	if ()
}

