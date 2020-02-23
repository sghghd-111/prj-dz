#include <stdio.h>
#include <pthread.h>
#include "ipc_msg.h"
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
//#include "comm_func.h"
#include "plat_include.h"
#include "command_line.h"
#include "msg_def.h"

static int help(void *hdl, int argc, char *argv[])
{
	printf("%s argc %d\n", __FUNCTION__, argc);

	return 0;
}

tpCMD_NODE cmd_node[] = 
{
	{"help", "help", help},
//	{"help", "help", help},
};

void *func1(void *hdl)
{
	char *str = "i am func1";
	int msg_id = 10;
//	LOG_W();
	while (1)
	{
		ipc_msg_send(hdl, "aaa", msg_id, str, strlen(str));
		sleep(1);
	}
}

void *func2(void *hdl)
{
	char *str = "i am func2";
	int msg_id = 12;

	LOG_I("func2\n");
	while (1)
	{
		ipc_msg_send(hdl, "aaa", msg_id, str, strlen(str));
		usleep(500000);
	}
}

static void cmd_parse(pCMD_INFO cmd_info)
{
	int j = 0;

	for (j=0; j<cmd_info->cmd_num; j++)
		LOG_I("%d.[%s]\n", j, cmd_info->cmds[j]);
}

void msg_hdl(pMSG_INFO pmsg, void *usr)
{
	LOG_I("msg id %d\n", pmsg->msg_id);
	LOG_I("msg from %s \n", pmsg->src);
	switch (pmsg->msg_id)
	{
	case 10:
		LOG_I("ten...\n");
		break;
	case IPC_MSG_CMDLINE:
		LOG_I("enenenenenenenen\n");
		cmd_parse(pmsg->msg_comm);
		break;
	default:
		LOG_I("%d...\n", pmsg->msg_id);
		break;
	}
}

tpCMD_PARA cmd_par;

void pro_out(int sig)
{
	printf("aaaaaaaaaaaa %d\n", sig);
	exit(0);
}

int main(int argc, char * argv[])
{
	char *tip = NULL;
	pthread_t thread_id;
	void *hdl = NULL; 
	
	signal(SIGINT, pro_out);
	signal(SIGTERM, pro_out);

	plat_conf_parse(argc, argv);
	if (plat_daem())
		plat_daemonize();

	tip = plat_get_process_name(argv[0]);
	plat_log_init(tip);

	hdl = ipc_msg_init(tip, msg_hdl, NULL);
	if (!hdl)
	{
		LOG_E("ipc msg init err\n");
		return 0;
	}

//	cmdline_init(&cmd_par, cmd_node, sizeof(cmd_node)/sizeof(tpCMD_NODE), NULL);
//	pthread_create(&thread_id, NULL, func1, hdl);
//	pthread_create(&thread_id, NULL, func2, hdl);
	ipc_msg_listen(hdl);
	return 0;
}

