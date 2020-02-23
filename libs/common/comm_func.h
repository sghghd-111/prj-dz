#ifndef _COMM_FUNC_H_
#define _COMM_FUNC_H_

#if 0
typedef enum
{
	LOG_NOFITY = 0
	, LOG_WARNING
	, LOG_ERROR
	, LOG_EMGERNCY
}LOG_LEVEL;

#define PL_LOG_I(format, ...) log_wr("INFO", format, ## __VA_ARGS__)
#define PL_LOG_W(format, ...) log_wr("WARN", format, ## __VA_ARGS__)
#define PL_LOG_E(format, ...) log_wr("ERRO", format, ## __VA_ARGS__)
#define PL_LOG_EE(format, ...) log_wr("EMGE", format, ## __VA_ARGS__)

extern int log_info_init(void);
extern int log_wr(char *level, char *format, ...);
#endif

extern int app_ms_sleep(int ms);

#endif

