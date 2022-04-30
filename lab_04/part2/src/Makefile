KVERSION = $(shell uname -r)
KDIR = /lib/modules/$(KVERSION)/build
PWD = $(shell pwd)
TARGET1 = fortune
TARGET2 = seqfile
#obj-m := $(TARGET1).o $(TARGET2).o
obj-m := $(TARGET1).o

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean

disclean: clean
	@rm -f *.ko *.symvers *.mod