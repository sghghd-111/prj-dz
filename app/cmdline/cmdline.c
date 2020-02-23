#include "cmdline.h"
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "plat_include.h"

static tpCMD_INFO cmds;

void pro_out(int sig)
{
	exit(0);
}

void msg_hdl(pMSG_INFO pmsg, void *usr)
{
	LOG_I("msg id %d\n", pmsg->msg_id);
	LOG_I("msg comm \"%s\"\n", pmsg->msg_comm);
	switch (pmsg->msg_id)
	{
	case 10:
		LOG_I("ten...\n");
		break;
//	case IPC_MSG_CMDLINE:
		
//		break;
	default:
		LOG_I("%d...\n", pmsg->msg_id);
		break;
	}
}

static int cmd_save(int argc, char *argv[], pCMD_INFO cmd_info)
{
	int j = 0;

	for (j=0; j<argc; j++)
	{
		if (CMD_CELL_LENGTH < strlen(argv[j]))
		{
			printf("argv %d[%s], string length more than %d\n", j, argv[j], CMD_CELL_LENGTH);
			return -1;
		}
		strcpy(cmd_info->cmds[j], argv[j]);
	}
	cmd_info->cmd_num = argc;
	return 0;
}

int main(int argc, char *argv[])
{
	char *tip = NULL;
	char *target = NULL;
	void *hdl = NULL;
	int ret = 0;

	if ((CMD_CELL_SIZE<(argc-1)) || (2 > argc))
	{
		printf("argc %d, more than %d, or less than 2\n", argc, CMD_CELL_SIZE);
		goto ARGC_TOO_MUCH;
	}
	
	target = argv[1];
	tip = plat_get_process_name(argv[0]);

	hdl = ipc_msg_init(tip, msg_hdl, NULL);

	cmd_save(argc-2, &argv[2], &cmds);
	ret = ipc_msg_send(hdl, target, IPC_MSG_CMDLINE, &cmds, sizeof(cmds));
	printf("%s send to %s, ret %d\n", tip, target, ret);
//	ipc_msg_listen(hdl);
	ipc_msg_release(hdl);

	return 0;
ARGC_TOO_MUCH:
	return -1;
}

