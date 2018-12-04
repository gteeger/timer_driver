#include <linux/ioctl.h>

#ifndef MY_TIMER_H
#define MY_TIMER_H

#define IOC_MAGIC 'k'
#define DEFAULT_TIMEOUT_MS (100)
#define DEFAULT_TIMEOUT_TICKS (5000)
#define DEFAULT_TIMEOUT_JIFFIES msecs_to_jiffies(DEFAULT_TIMEOUT_MS)
#define DEBOUNCE_TIME (200)

/* P8_12 */
#define PWM_GPIO (44)
#define TRUE (1)
#define FALSE (0)
#define LED_ON (1)
#define LED_OFF (0)
#define MAJOR_NUM (503)
#define DEFAULT_DUTY_CYCLE (10)

#define DUTY_CYCLE_WR _IOW(IOC_MAGIC, 1, __u8)
#define DUTY_CYCLE_RD _IOR(IOC_MAGIC, 1, __u8)
#define TICK_DURATION_WR _IOW(IOC_MAGIC, 2, __u8)
#define TICK_DURATION_RD _IOR(IOC_MAGIC, 2, __u8)


#endif