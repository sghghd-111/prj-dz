#include "plat_log.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

void plat_log_init(char *tip)
{
	openlog(tip, LOG_CONS, LOG_USER);
}

void plat_log(int priority, const char *format, ...)
{
	va_list args;
	char buf[256];
	int len = 0;

	va_start(args, format);
	len = vsnprintf(buf, 256, format, args);
	va_end(args);

	if (0 < len)
		syslog(priority, buf);
}

void plat_log_release(void)
{
	closelog();
}




