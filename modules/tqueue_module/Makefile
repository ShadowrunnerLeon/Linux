ccflags-y := -std=gnu11
obj-m += tqueue_module.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean	
test:
	sudo dmesg -C
	sudo insmod tqueue_module.ko
	sudo rmmod tqueue_module.ko
	dmesg
