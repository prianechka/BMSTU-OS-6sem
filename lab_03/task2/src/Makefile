KVERSION = $(shell uname -r)
KDIR = /lib/modules/$(KVERSION)/build
PWD = $(shell pwd)
TARGET1 = md1
TARGET2 = md2
TARGET3 = md3
obj-m := $(TARGET1).o $(TARGET2).o $(TARGET3).o

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean


disclean: clean
	@rm -f *.ko *.symvers *.mod