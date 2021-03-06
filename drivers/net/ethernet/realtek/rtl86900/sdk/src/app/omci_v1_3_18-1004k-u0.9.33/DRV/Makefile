#########################################################
#	Build Realtek OMCI modules		#
#########################################################
ifneq ($(OMCI_X86),y)
include $(APOLLODIR)/Compiler_Flag
endif

MODULE_NAME=omcidrv

PWD=$(shell pwd)
SDKDIR=$(APOLLODIR)/sdk
SDK_INCLUDE   = $(SDKDIR)/include
SYS_INCLUDE   = $(SDKDIR)/system/include
DRV_INCLUDE   = $(SDKDIR)/system/linux
OMCIDIR=$(OMCIROOT_DIR)
OMCISRCDIR=$(OMCIDIR)/OMCI/src
OMCI_API_INCLUDE = $(OMCIDIR)/API
MODULEDIR=$(SDKDIR)/src/module
PLATFORM_DIR=platform
FEATURES_DIR=features

EXTRA_CFLAGS += -Wall
EXTRA_CFLAGS += -I$(SDK_INCLUDE) \
		-I$(DRV_INCLUDE) \
		-I$(SYS_INCLUDE) \
		-I$(MODULEDIR) \
		-I$(OMCIDIR) \
		-I$(OMCISRCDIR) \
		-I$(OMCI_API_INCLUDE) \
		-I$(NETDIR) \
		-DCONFIG_SDK_KERNEL_LINUX

obj-m := $(MODULE_NAME).o

$(MODULE_NAME)-objs:= omci_drv.o omci_drv_event.o


ifeq ($(OMCI_X86),y)
KVERSION=$(shell uname -r)
KERNEL_DIR=/usr/src/linux-headers-$(KVERSION)/

EXTRA_CFLAGS += -DOMCI_X86

all:
	@echo "   BUILD module $(MODULE_NAME)"
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules || exit 1

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean || exit 1

else
all:
	@echo "   BUILD module $(MODULE_NAME)"
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) ARCH=rlx CROSS_COMPILE=$(CROSS_COMPILE) || exit 1
	CROSS_COMPILE=$(CROSS_COMPILE)
	for subdir in $(shell ls $(PWD)/$(PLATFORM_DIR)); do	\
		if [ -f $(PWD)/$(PLATFORM_DIR)/$$subdir/Makefile ]; then \
			$(MAKE) -C $(PWD)/$(PLATFORM_DIR)/$$subdir all || exit 1; \
		fi \
	done
	for subdir in $(shell ls $(PWD)/$(FEATURES_DIR)); do	\
		if [ -f $(PWD)/$(FEATURES_DIR)/$$subdir/Makefile ]; then \
			$(MAKE) -C $(PWD)/$(FEATURES_DIR)/$$subdir all || exit 1; \
		fi \
	done

install:
	@if [ ! -d $(ROMFSDIR)/lib/modules ]; \
        then \
                mkdir $(ROMFSDIR)/lib/modules ; \
    fi
	cp -af $(MODULE_NAME).ko $(ROMFSDIR)/lib/modules/
	for subdir in $(shell ls $(PWD)/$(PLATFORM_DIR)); do    \
		if [ -f $(PWD)/$(PLATFORM_DIR)/$$subdir/Makefile ]; then \
			 $(MAKE) -C $(PWD)/$(PLATFORM_DIR)/$$subdir install; \
		fi \
	done
	for subdir in $(shell ls $(PWD)/$(FEATURES_DIR)); do    \
		if [ -f $(PWD)/$(FEATURES_DIR)/$$subdir/Makefile ]; then \
			 $(MAKE) -C $(PWD)/$(FEATURES_DIR)/$$subdir install; \
		fi \
	done

clean:
	rm -rf *.o *~ core .depend .*.cmd *.mod.c .tmp_versions *.symvers *.ko modules.order
	for subdir in $(shell ls $(PWD)/$(PLATFORM_DIR)); do    \
		if [ -f $(PWD)/$(PLATFORM_DIR)/$$subdir/Makefile ]; then \
			 $(MAKE) -C $(PWD)/$(PLATFORM_DIR)/$$subdir clean; \
		fi \
	done
	for subdir in $(shell ls $(PWD)/$(FEATURES_DIR)); do    \
		if [ -f $(PWD)/$(FEATURES_DIR)/$$subdir/Makefile ]; then \
			 $(MAKE) -C $(PWD)/$(FEATURES_DIR)/$$subdir clean; \
		fi \
	done
endif
