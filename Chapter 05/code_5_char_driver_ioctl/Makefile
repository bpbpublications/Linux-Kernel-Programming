obj-m := chardrv.o
KDIR := /lib/modules/$(shell uname -r)/build

all:
	make M=$(shell pwd) -C $(KDIR) modules
	modinfo ./chardrv.ko
	gcc chardrv_ioctl_userspace.c -o chardrv_ioctl_userspace
	gcc test_ioctl.c -o test_ioctl

info:
	modinfo ./chardrv.ko

load:
	insmod ./chardrv.ko
	dmesg

unload:
	rmmod ./chardrv.ko
	dmesg

install:
	make M=$(shell pwd) -C $(KDIR) modules_install
	/sbin/depmod -a

clean:
	make M=$(shell pwd) -C $(KDIR) clean
