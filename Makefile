ccflags-y := -g -DDEBUG -std=gnu99 -Wno-declaration-after-statement

obj-m += kdisk.o

all: user
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
user: ioctl_test.c
	gcc -std=gnu99 -m32 ioctl_test.c -o ioctl_test
	gcc -std=gnu99 -m32 api.c test.c -o test


.PHONY: in
in: kdisk.ko
	insmod kdisk.ko

.PHONY: test
test: 
	make all
	make in
	./ioctl_test
	dmesg
	make clean


clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm ioctl_test
	$(shell rmmod kdisk)