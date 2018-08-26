ifneq  ($(KERNELRELEASE),)
obj-m:=hello.o
$(info "2nd")
else
KDIR := /usr/src/linux-4.4.128
PWD:=$(shell pwd)
all:
	$(info "1st")
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm -f *.ko *.o *.symvers *.mod.c *.mod.o *.order
endif
