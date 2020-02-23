#ifndef _PULSE_H_
#define _PULSE_H_
#include "ioctl_cmd.h"


#define PULSE_PATH				"/dev/pulse"

typedef struct _pulse_info_
{
	int fd;
}tpPULSE_INFO, *pPULSE_INFO;

extern void *pulse_init(void);
extern int pulse_irq_start(void *hdl);
extern int pulse_irq_stop(void *hdl);
extern void pulse_release(void *hdl);

#endif
