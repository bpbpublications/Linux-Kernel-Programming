obj-m := procfs.o
KDIR := /lib/modules/$(shell uname -r)/build

all:
	make M=$(shell pwd) -C $(KDIR) modules
	modinfo ./procfs.ko

info:
	modinfo ./procfs.ko

load:
	insmod ./procfs.ko
	dmesg

unload:
	rmmod ./param.ko
	dmesg

install:
	make M=$(shell pwd) -C $(KDIR) modules_install
	/sbin/depmod -a

clean:
	make M=$(shell pwd) -C $(KDIR) clean
