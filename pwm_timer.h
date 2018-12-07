#ifndef PWM_TIMER_H
#define PWM_TIMER_H
#include <linux/types.h>
#include <linux/ioctl.h>


#define IOC_MAGIC 'p'
#define DEBUG (1)
#define DEFAULT_TIMEOUT_MS (10)
#define DEFAULT_TIMEOUT_TICKS (5000)
#define DEFAULT_TIMEOUT_JIFFIES msecs_to_jiffies(DEFAULT_TIMEOUT_MS)
#define DEBOUNCE_TIME (200)

/* P8_12 */
#define PWM_GPIO (44)

/* P9_27 */
#define BUTTON_GPIO (115)

#define TRUE (1)
#define FALSE (0)
#define LED_ON (1)
#define LED_OFF (0)
#define UP (1)
#define DOWN (0)
#define MAJOR_NUM (503)
#define DEFAULT_DUTY_CYCLE (50)
#define DEBOUNCE_TIME (200)
#define DEFAULT_UP_CYCLES (5)
#define DEFAULT_DOWN_CYCLES (18)

#define IOCTL_PWM_LED_DUTY_CYCLE_WR _IOW(IOC_MAGIC, 1, __u8)
#define IOCTL_PWM_LED_DUTY_CYCLE_RD _IOR(IOC_MAGIC, 1, __u8)
#define IOCTL_TIMER_TICK_DURATION_WR _IOW(IOC_MAGIC, 2, __u8)
#define IOCTL_TIMER_TICK_DURATION_RD _IOR(IOC_MAGIC, 2, __u8)


#endif
