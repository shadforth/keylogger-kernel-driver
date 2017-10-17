#CONFIG_MODULE_SIG=n

obj-m += S3332777Keylogger.o

KERNELDIR = /usr/src/linux-headers-$(shell uname -r)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.symvers *.order Module.*
