#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <asm/types.h>
#include <sys/ioctl.h>
#include "pwm_timer.h"



static uint8_t duty_cycle;
static uint8_t max_ticks;
static uint8_t tick_duration;



static void do_read(int fd)
{
    uint8_t buf = 0;
    int status = 0;
    status = read(fd, &buf, 1);
    if (status < 0) {
	perror("read");
	return;
    }

    printf("\n");
    printf("Timer master value: %02d", buf);
    printf("\n");
}

static void do_write(int fd, uint8_t message)
{
    unsigned char buf;
    int status;
    buf = message;
    status = write(fd, &buf, 1);
    printf("\n");
    printf("write(%2d): %02x", status, buf);
    printf("\n");
    return;
}


int main(int argc, char *argv[])
{
    int c = 0;
    int fd = 0;
    int write_msg = 0;
    int ret = 0;
    duty_cycle = 50;
    const char *name = "/dev/TIMER_DEV";
    const char *optstring = "w:rd:t:";
    fd = open(name, O_RDWR);

    while ((c = getopt(argc, argv, optstring)) != EOF) {

	switch (c) {

	case 'w':
	    write_msg = (uint8_t) (atoi(optarg));
	    do_write(fd, write_msg);
	    continue;
	case 'r':
	    do_read(fd);
	    continue;
	case 'd':
	    duty_cycle = (uint8_t) (atoi(optarg));
	    ret = ioctl(fd, IOCTL_PWM_LED_DUTY_CYCLE_WR, &duty_cycle);
	    if (ret == -1)
		perror("can't set DUTY_CYCLE");

	    ret = ioctl(fd, IOCTL_PWM_LED_DUTY_CYCLE_RD, &duty_cycle);
	    if (ret == -1)
		perror("can't get DUTY_CYCLE");
	    continue;
	case 't':
	    tick_duration = (uint8_t) (atoi(optarg));
	    ret = ioctl(fd, IOCTL_TIMER_TICK_DURATION_WR, &tick_duration);
	    if (ret == -1)
		perror("can't set TICK_DURATION");

	    ret = ioctl(fd, IOCTL_TIMER_TICK_DURATION_RD, &tick_duration);
	    if (ret == -1)
		perror("can't get TICK_DURATION");
	    continue;
	case '?':
	  err:
	    printf
		("\nusage: [-w N] [-r] [-d N] [-t N]\n -w: write [N: message]\n");
	    printf
		("-r: read current timer value\n-d: Duty cycle change [N: percentage] \n");
	    printf("Tick duration change [N : #ms] \n");
	    printf("Duty_cycle (default = 50) \n");
	    printf("Tick duration (default = 100ms)  \n\n");

	    return 1;
	}

    }

    close(fd);
    return 0;


}
