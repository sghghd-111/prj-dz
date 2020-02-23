#include <stdio.h>
#include <pthread.h>
#include "lib_msg.h"
#include <string.h>
#include <unistd.h>
#include "comm_func.h"
#include "command_line.h"
#include "msg_def.h"


int main(int argc, char *argv[])
{
	int j = 0;
	int len = 0;
	char buf[MSG_SIZE];

	do
	{
		len = snprintf(&buf[len], MSG_SIZE, "%s,", argv[j]);
		if (MSG_SIZE == len)
			return -1;
		j++;
	}while(j<argc);

	return ipc_msg_send(NULL, "aaa", IPC_MSG_CMDLINE, buf, len);
}

