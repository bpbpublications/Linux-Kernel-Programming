obj-m := panicb.o
KDIR := /lib/modules/$(shell uname -r)/build

all:
	make M=$(shell pwd) -C $(KDIR) modules
	modinfo ./panicb.ko
	gcc panicb_test.c -o panicb_test

info:
	modinfo ./panicb.ko

load:
	insmod ./panicb.ko
	dmesg

unload:
	rmmod ./panicb.ko
	dmesg

install:
	make M=$(shell pwd) -C $(KDIR) modules_install
	/sbin/panicb -a

clean:
	make M=$(shell pwd) -C $(KDIR) clean
