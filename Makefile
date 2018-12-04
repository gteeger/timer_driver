export ARCH:=arm
export CROSS_COMPILE:=arm-linux-gnueabihf-
export CC:=arm-linux-gnueabihf-gcc
obj-m := my_timer.o
KDIR := /opt/ti-processor-sdk-linux-am335x-evm-05.01.00.11/board-support/linux-4.14.67+gitAUTOINC+d315a9bb00-gd315a9bb00
#KDIR := /home/gt/ti-processor-sdk-linux-am335x-evm-05.01.00.11/board-support/linux-4.14.67+gitAUTOINC+d315a9bb00-gd315a9bb00
PWD := $(shell pwd)
mod:
		$(MAKE) -C $(KDIR) M=$(PWD) modules

app:
		$(CC) timer_test_app.c -o test

clean:
		$(MAKE) -C $(KDIR) M=$(PWD) clean
		rm -f test