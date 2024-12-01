# Hello world kernel module

EXTRA_CFLAGS = -Wall
obj-m = hello.o

KDIR = /lib/modules/`uname -r`/build

all:
	make -C $(KDIR) M=`pwd` modules
	sudo insmod hello.ko

clean:
	make -C $(KDIR) M=`pwd` clean
	sudo rmmod hello
