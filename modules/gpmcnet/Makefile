
ifneq ($(KERNELRELEASE),)

obj-m := gpmc_net.o
#pulse-objs := pulse.o

else
	
PWD	:= $(shell pwd)
KDIR := /home/cms/code/linux-3.2.0
all:
	make -C $(KDIR) -Wall M=$(PWD)  modules ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
clean:
	rm -rf *.ko *.o *.mod.o *.mod.c .*.cmd .tmp_versions *.symvers *.order
install:
	cp *.ko ../../install_dir/module/

endif
