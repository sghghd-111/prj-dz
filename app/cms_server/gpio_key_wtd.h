#ifndef _GPIO_KEY_WTD_H_
#define _GPIO_KEY_WTD_H_
#include <pthread.h>
#include <stdio.h>

#define GPIO_WTD				"/dev/wdi"
#define RECOVER_EVENT			"/dev/input/event1"

#define RECOVER_SHELL_PATH		"/opt/platform/shell/param-disk-format.sh"

typedef struct _io_info_
{
	int wdi_fd;
	int key_fd;
	pthread_t peri_wtd_id;
	pthread_t key_id;
	int run_flag;
}tpIO_INFO, *pIO_INFO;

extern int kw_init(void);
extern void kw_release(void);
#endif

