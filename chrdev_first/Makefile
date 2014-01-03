#ifneq ($(KERNELRELASE),)
obj-m := chrdev_first.o

#else

KERNELDIR ?= ~/works/linux3.10

PWD       := $(shell pwd)
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD) modules

#endif

install:
	cp chrdev_first.ko /tftpboot/filesys_nk/root/

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions
