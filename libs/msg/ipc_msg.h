#ifndef _IPC_MSG_H_
#define _IPC_MSG_H_

#define MSG_SIZE			512

#define PROCESS_NAME_SIZE	32

typedef struct _msg_
{
	unsigned int msg_id;
	char src[PROCESS_NAME_SIZE];
	char msg_comm[MSG_SIZE];
}tpMSG_INFO, *pMSG_INFO;


//#define PROCESS_NAME_SIZE	64
#define PRO_FILE_PATH		"/dev/shm/"

typedef void (*MSG_CB)(pMSG_INFO, void *);

typedef struct _process_info_
{
	int listen_fd;
	int recv_fd;
	char name[PROCESS_NAME_SIZE];
	tpMSG_INFO msg;
	MSG_CB cb;
	void *usr;
}tpPROCESS_INFO, *pPROCESS_INFO;

extern void *ipc_msg_init(char *name, MSG_CB cb, void *usr);
extern int ipc_msg_send(void *hdl, char *target,  int msg_id, void *msg, int msg_len);
extern int ipc_msg_listen(void *hdl);
extern void ipc_msg_release(void *hdl);

#endif

