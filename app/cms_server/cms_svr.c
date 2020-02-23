#include "cms_svr.h"
//#include "comm_func.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include "wtd_func.h"
#include "plat_include.h"
#include "gpio_key_wtd.h"

void *main_hdl = NULL;


int app_ms_sleep(int ms)
{
	int ret = 0;
	struct timeval tv;

	tv.tv_sec = ms/1000;
	tv.tv_usec = (ms%1000)*1000;

	ret = select(0, NULL, NULL, NULL, &tv);
	if (!ret)
		return -1;
	return 0;
}

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

int main(int argc, char *argv[])
{
	char *tip = NULL;
	void *hdl = NULL;
	pSERVER_INFO svr_info = NULL;

	signal(SIGINT, pro_out);
	signal(SIGTERM, pro_out);

	plat_conf_parse(argc, argv);
	if (plat_daem())
		plat_daemonize();

	tip = plat_get_process_name(argv[0]);
	plat_log_init(tip);

	hdl = ipc_msg_init(tip, msg_hdl, NULL);

	svr_info = (pSERVER_INFO)malloc(sizeof(tpSERVER_INFO));
	if (!svr_info)
	{
		LOG_E("%s, malloc err\n", __FUNCTION__);
		goto MALLOC_ERR;
	}
	memset(svr_info, 0, sizeof(tpSERVER_INFO));
	svr_info->pulse_hdl = pulse_init();
	if (!svr_info->pulse_hdl)
	{
		LOG_E("pulse init err\n");
		goto PULSE_INIT;
	}

	main_hdl = svr_info;
	if (pulse_irq_start(svr_info->pulse_hdl))
	{
		LOG_E("IRQ START ER\n");
		goto PULSE_IRQ_ERR;
	}
	if (wtd_init())
	{
		LOG_E("watch dog init err\n");
		goto WTD_INIT_ERR;
	}
#if 1
	svr_info->bc_hdl = bc_func_init(10888);
	if (!svr_info)
	{
		LOG_E("broadcast func init err\n");
		goto BC_FUNC_INIT_ERR;
	}
#endif
	kw_init();
	ipc_msg_listen(hdl);

	return 0;
BC_FUNC_INIT_ERR:
	wtd_release();
WTD_INIT_ERR:
	
PULSE_IRQ_ERR:
	pulse_release(svr_info->pulse_hdl);
PULSE_INIT:
	free(svr_info);
MALLOC_ERR:
	return -1;
}

