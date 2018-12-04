#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/gpio.h>
#include <linux/poll.h>
#include <linux/ioctl.h>
#include "my_timer.h"


static struct timer_list timer;
static uint32_t timer_timeout;
static uint32_t master_timer;
static uint32_t master_timeout;
static uint8_t duty_cycle;
static bool pwm_state;

static void timeout(struct timer_list *t);

static int dev_open(struct inode *inode, struct file *filp)
{
    printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
    return mod_timer(&timer, jiffies);
}

static long dev_ioctl(struct file *filp, unsigned int cmd,
		      unsigned long arg)
{
    u8 tmp;
    long retval = 0;
    printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);

    switch (cmd) {

    case DUTY_CYCLE_WR:
	retval = get_user(tmp, (__u32 __user *) arg);
	duty_cycle = tmp;
	break;

    case DUTY_CYCLE_RD:
	retval = put_user(duty_cycle, (__u8 __user *) arg);
	break;

    case TICK_DURATION_WR:
	retval = get_user(tmp, (__u32 __user *) arg);
	timer_timeout = tmp;
	break;

    case TICK_DURATION_RD:
	retval = put_user(timer_timeout, (__u8 __user *) arg);
	break;
    default:
	break;
    }
    return retval;
}


static ssize_t dev_read(struct file *filp, char __user * buf,
			size_t count, loff_t * f_pos)
{
    ssize_t retval = 0;
    printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
    retval = copy_to_user(buf, &master_timer, count);
    return retval;
}

static ssize_t dev_write(struct file *filp, const char __user * buf,
			 size_t count, loff_t * f_pos)
{
    ssize_t retval;
    printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
    retval = copy_from_user(&master_timeout, buf, count);
    return retval;
}

static int dev_release(struct inode *inode, struct file *filp)
{
    printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
    return 0;
}


static void timeout(struct timer_list *t)
{
    gpio_set_value(PWM_GPIO, pwm_state);
    pwm_state = !pwm_state;
    if (pwm_state == LED_ON)
	mod_timer(&timer,
		  jiffies + ((timer_timeout) -
			     (timer_timeout) * (duty_cycle / 100)));

    else
	mod_timer(&timer,
		  jiffies + ((timer_timeout) * (duty_cycle / 100)));
    master_timer++;
    if (master_timer == master_timeout) {
	printk(KERN_ALERT "master timeout!\n");
	master_timer = 0;
	return;
    }

}



/***********************************************************************/


static const struct file_operations dev_fops = {
    .owner = THIS_MODULE,
    .write = dev_write,
    .read = dev_read,

    /*
     * unlocked_ioctl lets the writer chose what lock to use instead
     * of the BKL (big kernel lock )
     */
    .unlocked_ioctl = dev_ioctl,
    .open = dev_open,

    /* 
     * The release method is called when the last reference to the device is removed  
     * All device structures registed with the core must have a release method
     * LDD3-pg 382
     */
    .release = dev_release,
};


static int __init my_timer_init(void)
{
    int status;
    printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
    master_timer = 0;
    duty_cycle = DEFAULT_DUTY_CYCLE;
    master_timeout = DEFAULT_TIMEOUT_TICKS;
    timer_timeout = DEFAULT_TIMEOUT_JIFFIES;
    timer_setup(&timer, timeout, LED_OFF);
    if (gpio_request(PWM_GPIO, "pwm_gpio") ||
	gpio_direction_output(PWM_GPIO, LED_OFF)) {
	return -1;
    }
    status = register_chrdev(MAJOR_NUM, "timer_dev", &dev_fops);
    return 0;
}

static void __exit my_timer_exit(void)
{
    printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
    unregister_chrdev(MAJOR_NUM, "timer_dev");
    del_timer(&timer);
    gpio_free(PWM_GPIO);
}

module_init(my_timer_init);
module_exit(my_timer_exit);

MODULE_AUTHOR("gt");
MODULE_LICENSE("GPL");
