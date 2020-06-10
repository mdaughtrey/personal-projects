OVCAM_KVER := $(shell uname -r)
OVCAM_MAJMIN := $(shell echo $(OVCAM_KVER) | cut -d . -f 1-2)

# -------------------------- 2.4 KERNEL SUPPORT -------------------------------
ifeq ($(OVCAM_MAJMIN),2.4)

#################################### OPTIONS ##################################
# Change this to /usr/include if you get header file errors
INCLUDEDIR = /lib/modules/`uname -r`/build/include

CFLAGS = -D__KERNEL__ -DMODULE -DOUTSIDE_KERNEL -O2 -Wall -Wstrict-prototypes \
	-fomit-frame-pointer -I$(INCLUDEDIR)

CC = gcc

MODULES = ov51x.o ov511_decomp.o ov518_decomp.o

INSTALL_PATH_2.2   = /lib/modules/`uname -r`/usb/
INSTALL_PATH_2.4   = /lib/modules/`uname -r`/kernel/drivers/usb/
INSTALL_PATH_2.4RH = /lib/modules/`uname -r`/kernel/drivers/usb/ov511/
INSTALL_PATH_2.5   = /lib/modules/`uname -r`/kernel/drivers/usb/media/

#################################### TARGETS ##################################

# Make with standard options for cameras and video capture. No tuner support.
all: $(MODULES)

install:
	@echo 'You must specify an appropriate install command for your kernel:'
	@echo '   make install-2.2'
	@echo '   make install-2.4'
	@echo '   make install-2.4rh  (for RedHat kernels 2.4.9-31 thru 2.4.18)'
	@echo '   make install-2.5'

# Install on a 2.2 system
install-2.2: all
	$(MAKE) _do_install _INSTALL_PATH_="$(INSTALL_PATH_2.2)"

# Install on a 2.4 system
install-2.4: all
	$(MAKE) _do_install _INSTALL_PATH_="$(INSTALL_PATH_2.4)"

# Install on a RedHat 2.4.9-31 - 2.4.18 system
install-2.4rh: all
	$(MAKE) _do_install _INSTALL_PATH_="$(INSTALL_PATH_2.4RH)"

# Install on a 2.5 system
install-2.5: all
	$(MAKE) _do_install _INSTALL_PATH_="$(INSTALL_PATH_2.5)"

clean:
	rm -f *.o *~ core *.i

#################################### RULES ####################################

_do_install:
	install $(MODULES) $(_INSTALL_PATH_)
	/sbin/depmod -ae

ov51x.o: ov51x.c ov51x.h
	$(CC) -c $(CFLAGS) -DEXPORT_SYMTAB ov51x.c

ov511_decomp.o: ov511_decomp.c ov51x.h
	$(CC) -c $(CFLAGS) ov511_decomp.c

ov518_decomp.o: ov518_decomp.c ov51x.h
	$(CC) -c $(CFLAGS) ov518_decomp.c

endif  # End kernel version test
# -------------------------- 2.6 KERNEL SUPPORT -------------------------------
ifeq ($(OVCAM_MAJMIN),2.6)

ifneq ($(KERNELRELEASE),)
#
# Make rules for use from within 2.6 kbuild system
#
obj-m	+= ov51x.o ov511_decomp.o ov518_decomp.o

else  # We were called from command line

KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

install: all
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install

clean:
	rm -f *.mod.c *.ko *.o *~ core *.i
endif

endif  # End kernel version test
