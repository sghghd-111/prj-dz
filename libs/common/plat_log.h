#ifndef _PLAT_LOG_H_
#define _PLAT_LOG_H_
#include <syslog.h>

extern void plat_log_init(char *tip);
extern void plat_log(int priority, const char *format, ...);
extern void plat_log_release(void);

#define LOG_I(format, ...) plat_log(LOG_INFO, format, ## __VA_ARGS__)
#define LOG_W(format, ...) plat_log(LOG_WARNING, format, ## __VA_ARGS__)
#define LOG_E(format, ...) plat_log(LOG_ERR, format, ## __VA_ARGS__)
//#define LOG_EE(format, ...) log_wr("EMGE", format, ## __VA_ARGS__)

#endif
