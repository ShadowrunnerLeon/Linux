ccflags-y := -std=gnu11
obj-m += rcu_module.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean	
test:
	sudo dmesg -C
	sudo insmod rcu_module.ko
	sudo rmmod rcu_module.ko
	dmesg
