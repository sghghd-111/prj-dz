#ifndef _IOCTL_CMD_H_
#define _IOCTL_CMD_H_
#include <linux/ioctl.h>

#define PULSE_MAGIC				'd'
#define CMD_PULSE_START			_IO(PULSE_MAGIC, 0x1)
#define CMD_PULSE_STOP			_IO(PULSE_MAGIC, 0x2)
#define CMD_PULSE_PR			_IO(PULSE_MAGIC, 0x3)
#define CMD_PULSE_STAT_GET		_IOR(PULSE_MAGIC, 0x4, void *)

#endif
