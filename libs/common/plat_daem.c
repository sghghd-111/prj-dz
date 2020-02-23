#include "plat_daem.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void plat_daemonize(void)
{
	pid_t pid;

	umask(0);

	pid = fork();
	if (0 > pid)
	{
		printf("can not fork...\n");
		exit(1);
	}
	else if (0 < pid)
		exit(0);

	setsid();
	pid = fork();
	if (0 > pid)
	{
		printf("t2, can not fork...\n");
		exit(1);
	}
	else if (0 < pid)
		exit(0);
	if (0 > chdir("/"))
	{
		printf("chdir...\n");
		exit(1);
	}
}


