#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/gpio.h>
#include <linux/poll.h>
#include <linux/ioctl.h>
#include <linux/interrupt.h>
#include "pwm_timer.h"


static struct timer_list timer;
static uint32_t timer_timeout;
static uint32_t master_timer;
static uint32_t master_timeout;
static int duty_cycle;
static bool pwm_state;
static bool button_state;
static int up_cycles, down_cycles;
static int irq_number;

static void timeout(struct timer_list *t);
static irqreturn_t irq_handler(unsigned int irq, void *dev_id,
                   struct pt_regs *regs);


static int dev_open(struct inode *inode, struct file *filp)
{
#ifdef DEBUG
    printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
#endif
    mod_timer(&timer, jiffies + timer_timeout);
    return 0;
}

static long dev_ioctl(struct file *filp, unsigned int cmd,
                      unsigned long arg)
{
    u32 tmp;
    long retval = 0;
#ifdef DEBUG
    printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
#endif
    switch (cmd) {

    case IOCTL_PWM_LED_DUTY_CYCLE_WR:
        retval = get_user(tmp, (__u32 __user *) arg);
        duty_cycle = (int) tmp;
        mod_timer(&timer, jiffies + timer_timeout);
        break;

    case IOCTL_PWM_LED_DUTY_CYCLE_RD:
        retval = put_user(duty_cycle, (__u8 __user *) arg);
        break;

    case IOCTL_TIMER_TICK_DURATION_WR:
        retval = get_user(tmp, (__u32 __user *) arg);
        timer_timeout = tmp;
        break;

    case IOCTL_TIMER_TICK_DURATION_RD:
        retval = put_user(timer_timeout, (__u8 __user *) arg);
        break;
    default:
        break;
    }
    return 0;
}


static ssize_t dev_read(struct file *filp, char __user * buf,
                        size_t count, loff_t * f_pos)
{
    ssize_t retval = 0;
#ifdef DEBUG
    printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
#endif
    retval = copy_to_user(buf, &master_timer, count);
    return retval;
}

static ssize_t dev_write(struct file *filp, const char __user * buf,
                         size_t count, loff_t * f_pos)
{
    ssize_t retval;
#ifdef DEBUG
    printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
#endif
    retval = copy_from_user(&master_timeout, buf, count);
    mod_timer(&timer, jiffies + DEFAULT_TIMEOUT_JIFFIES);
    return retval;
}

static int dev_release(struct inode *inode, struct file *filp)
{
    printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
    return 0;
}


static void timeout(struct timer_list *t)
{

    uint32_t x, y;

    if (master_timer == master_timeout) {
        master_timer = 0;
        gpio_set_value(PWM_GPIO, LED_OFF);
        return;
    }
    x = duty_cycle * timer_timeout * 10;
    y = abs(timer_timeout * 1000 - x);

    printk(KERN_ALERT "x = %u, y = %u\n", x, y);

    x = usecs_to_jiffies(x);
    y = usecs_to_jiffies(y);



    if (pwm_state == LED_ON)
        mod_timer(&timer, jiffies + x);
    else
        mod_timer(&timer, jiffies + y);
    gpio_set_value(PWM_GPIO, pwm_state);
    pwm_state = !pwm_state;
    master_timer++;


}

static irqreturn_t irq_handler(unsigned int irq, void *dev_id,
                   struct pt_regs *regs){

    /* This edge is falling */
    if(button_state == UP){
        master_timeout = down_cycles;

        /* timeout right away */
        mod_timer(&timer, jiffies);
    }
        /* This edge is rising */
    if(button_state == DOWN){
        master_timeout = up_cycles;

        /* timeout right away */
        mod_timer(&timer, jiffies);
    }

    button_state = !button_state;

    return IRQ_HANDLED;
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
    .release = dev_release,
};


static int __init timer_init(void)
{
    int status;
    int irq_number;
    unsigned long IRQflags = IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING;
#ifdef DEBUG
    printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
#endif
    master_timer = 0;
    button_state = DOWN;
    duty_cycle = DEFAULT_DUTY_CYCLE;
    master_timeout = DEFAULT_TIMEOUT_TICKS;
    timer_timeout = DEFAULT_TIMEOUT_MS;
    up_cycles = DEFAULT_UP_CYCLES;
    down_cycles = DEFAULT_DOWN_CYCLES;

    timer_setup(&timer, timeout, 0);
    if (gpio_request(PWM_GPIO, "pwm_gpio") ||
            gpio_direction_output(PWM_GPIO, LED_OFF)) {
        return -1;
    }
    if (gpio_request(BUTTON_GPIO, "button_gpio") ||
            gpio_direction_input(BUTTON_GPIO)||
            gpio_set_debounce(BUTTON_GPIO, DEBOUNCE_TIME))  {
        return -1;
    }
    irq_number = gpio_to_irq(BUTTON_GPIO);

    if (irq_number < 0){
        printk(KERN_ALERT "Unable to map IRQ");
        return irq_number;
    }
#ifdef DEBUG
    printk(KERN_ALERT "gpio mapped to irq %d", irq_number);
#endif
    status = request_irq(irq_number,
             (irq_handler_t) irq_handler,
             IRQflags, "button_handler", NULL);
    if(status < 0){
        printk(KERN_ALERT "Unable to request IRQ");
        return status;
    }
    status = register_chrdev(MAJOR_NUM, "timer_dev", &dev_fops);
    return status;
}



static void __exit timer_exit(void)
{
#ifdef DEBUG
    printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
#endif
    unregister_chrdev(MAJOR_NUM, "timer_dev");
    del_timer(&timer);
    gpio_free(PWM_GPIO);
    free_irq(irq_number, NULL);
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_AUTHOR("gt");
MODULE_LICENSE("GPL");
