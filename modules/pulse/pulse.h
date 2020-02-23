#ifndef _PULSE_H_
#define _PULSE_H_
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/kfifo.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/gpio.h>

#define PULSE_GPO_NAME				"pulse-gpo"

typedef struct _gpo_info_
{
	unsigned int gpo_no;
	unsigned int cnt;
	unsigned int val;

	unsigned int set_fail_cnt;
}tpGPO_INFO, *pGPO_INFO;

#define TIMER_OUT_CNT				10				//500ms*10 = 5s now for test

typedef struct _timer_info_
{
	int ms;
	struct hrtimer mytimer;
	ktime_t kt;
	int keepalive_cnt;
}tpTIMER_INFO, *pTIMER_INFO;

typedef struct _irq_info_
{
	unsigned int irq_gpi_no;
}tpIRQ_INFO, *pIRQ_INFO;

typedef struct _pulse_dev_
{
	struct cdev cdev;
	struct class *pulse_class;
	struct device *pulse_dev;
	dev_t devno;
	int opened;
	tpGPO_INFO gpo;
	tpTIMER_INFO timer;
	tpIRQ_INFO irq_info;
}tpPULSE_DEV, *pPULSE_DEV;

#endif


