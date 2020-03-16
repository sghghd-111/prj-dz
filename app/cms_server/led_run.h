#ifndef _LED_RUN_H_
#define _LED_RUN_H_

#define LED_RUN_DEV		"/dev/led-run"

extern int led_run_start(void);
extern void led_run_stop(void);
extern int led_run_ctl(int stat);
#endif

