ifneq ($(KERNELRELEASE),)
	obj-m := md1.o
else
	KVERSION = $(shell uname -r)
	KDIR = /lib/modules/$(KVERSION)/build
	PWD = $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean

disclean: clean
	@rm *.ko *.symvers

endif