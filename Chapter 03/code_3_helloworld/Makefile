obj-m := helloworld.o
KDIR := /lib/modules/$(shell uname -r)/build

all:
	make M=$(shell pwd) -C $(KDIR) modules
	modinfo ./helloworld.ko

load:
	insmod ./helloworld.ko
	dmesg

unload:
	rmmod ./helloworld.ko
	dmesg

install:
	make M=$(shell pwd) -C $(KDIR) modules_install
	/sbin/depmod -a

clean:
	make M=$(shell pwd) -C $(KDIR) clean
