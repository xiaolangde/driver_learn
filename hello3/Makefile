#ifneq ($(KERNELRELASE),)

obj-m := hello3.o
TARGET_F := hello3.ko
#else

KERNELDIR ?= ~/works/linux3.10

PWD       := $(shell pwd)
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD) modules
	$(MAKE) install
#endif
install:
	cp $(TARGET_F) /tftpboot/filesys_nk/root/

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions
