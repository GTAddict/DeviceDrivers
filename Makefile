obj-m :=parlelport.o 
KDIR  := /lib/modules/2.6.28-11-generic/build
PWD   := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
