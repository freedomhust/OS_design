	obj-m := mydev.o                                                     
	PWD := $(shell pwd)                                                     
	KERNELDIR := /usr/src/linux-4.4.128                                      
all:
	make -C $(KERNELDIR) M=$(PWD) modules                                   


.PHONY: clean                                                             
clean:
	rm -f *.o .*.cmd *.ko *.mod.c modules.order Module.symvers  