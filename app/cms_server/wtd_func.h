#ifndef _WTD_FUNC_H_
#define _WTD_FUNC_H_
#include <pthread.h>

#define WTD_DEV_PATH				"/dev/watchdog"

#define WTD_MAX_TOUT				60
#define WTD_MIN_TOUT				10

typedef struct _wtd_info_
{
	int tout_set;
	int feed_set;
	int fd;
	int thread_run;
	pthread_t thread_id;
	
	unsigned int feed_cnt;
}tpWTD_INFO, *pWTD_INFO;

extern int wtd_init(void);
extern void wtd_release(void);

#endif

