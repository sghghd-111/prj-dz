#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

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

