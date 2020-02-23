#ifndef _MSG_DEF_H_
#define _MSG_DEF_H_

typedef enum
{
	IPC_MSG_MIN = 100,
	IPC_MSG_CMDLINE,
	IPC_MSG_MAX,
}IPC_MSG;

//IPC_MSG_CMDLINE
#define CMD_CELL_SIZE				16
#define CMD_CELL_LENGTH				16

typedef struct _cmd_info_
{
	char cmds[CMD_CELL_SIZE][CMD_CELL_LENGTH];
	int cmd_num;
}tpCMD_INFO, *pCMD_INFO;

#endif


