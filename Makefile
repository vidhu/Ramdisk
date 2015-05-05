ccflags-y := -std=gnu99 -Wno-declaration-after-statement

obj-m += kdisk.o

all: user
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
user: ioctl_test.c
	gcc -std=gnu99 -m32 ioctl_test.c -o ioctl_test

.PHONY: in
in: kdisk.ko
	insmod kdisk.ko

test: 
	make all
	make in
	./ioctl_test
	dmesg
	make clean


clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(shell rmmod kdisk)