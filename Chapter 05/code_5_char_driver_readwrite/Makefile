obj-m := chardrv.o
KDIR := /lib/modules/$(shell uname -r)/build

all:
	make M=$(shell pwd) -C $(KDIR) modules
	modinfo ./chardrv.ko
	gcc chardrv_userspace.c -o chardrv_userspace

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
	/sbin/chardrv -a

clean:
	make M=$(shell pwd) -C $(KDIR) clean
