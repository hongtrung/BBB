# Hello world kernel module

EXTRA_CFLAGS = -Wall
obj-m = hello.o

KDIR := /lib/modules/`uname -r`/build

all:
	make -C $(KDIR) M=`pwd` modules
	gcc -o app test_app.c
	sudo insmod hello.ko
	sudo chmod 0777 /dev/m_device
clean:
	make -C $(KDIR) M=`pwd` clean
	rm -rf app
	sudo rmmod hello

