#
#
#
KERNEL-BUILDDIR	= /opt/arm9g25/linux-3.14.37
CC-PREFIX	= /opt/arm9g25/usr/bin/arm-linux-

obj-m	+= basic.o
obj-m	+= pdd.o
obj-m	+= procfs.o

KOBJS	= $(obj-m:%.o=%.ko)

all::
	make -C $(KERNEL-BUILDDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=$(CC-PREFIX) modules

clean::
	make -C $(KERNEL-BUILDDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=$(CC-PREFIX) clean
	@rm -f *~

.DEFAULT:
	make -C $(KERNEL-BUILDDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=$(CC-PREFIX) $@

