#########################################################
#	Build Realtek OMCI modules		#
#########################################################
include $(APOLLODIR)/Compiler_Flag
ifeq ($(shell [ $(CONFIG_SDK_LUNA_G3) == y ] || [ $(CONFIG_SDK_CA8279) == y ] && echo true),true)

MODULE_NAME=pf_ca8279
PWD=$(shell pwd)
SDKDIR=$(APOLLODIR)/sdk
SDK_INCLUDE   = $(SDKDIR)/include
SYS_INCLUDE   = $(SDKDIR)/system/include
DRV_INCLUDE   = $(SDKDIR)/system/linux
OMCIDIR=$(OMCIROOT_DIR)
OMCISRCDIR=$(OMCIDIR)/OMCI/src
OMCIDRVDIR=$(OMCIDIR)/DRV
OMCI_API_INCLUDE = $(OMCIDIR)/API
MODULEDIR=$(SDKDIR)/src/module
RG_INCLUDE = $(APOLLODIR)/romeDriver
ROOT_INCLUDE = $(ROOTDIR)/include

KBUILD_EXTRA_SYMBOLS += $(OMCIDRVDIR)/Module.symvers
EXTRA_CFLAGS += -Wall -Wframe-larger-than=2048
EXTRA_CFLAGS += -I$(SDK_INCLUDE) \
		-I$(DRV_INCLUDE) \
		-I$(SYS_INCLUDE) \
		-I$(MODULEDIR) \
		-I$(OMCIDIR) \
		-I$(OMCISRCDIR) \
		-I$(OMCIDRVDIR) \
		-I$(OMCI_API_INCLUDE) \
		-I$(RG_INCLUDE) \
		-I$(NETDIR)  \
		-I$(ROOT_INCLUDE) \
		-I$(ROOTDIR)/ca_packages/ca-network-engine/1.0-r0/ca-network-engine-1.0 \
		-I$(ROOTDIR)/ca_packages/ca-pon-mac/1.0-r0/ca-pon-mac-1.0 \
		-I$(ROOTDIR)/ca_packages/ca-plat/1.0-r0/ca-plat-1.0/log \
		-I$(ROOTDIR)/ca_packages/ca-scfg/1.0-r0/ca-scfg-1.0 \
		-DCONFIG_SDK_KERNEL_LINUX $(PON_CFLAGS)

ifdef RGDRV_RELEASE_VERSION
EXTRA_CFLAGS += -DCONFIG_DRV_RELEASE_VERSION="$(RGDRV_RELEASE_VERSION)"
endif

obj-m := $(MODULE_NAME).o

$(MODULE_NAME)-objs:= omci_pf_ca8279.o omci_pf_ca8279_gpon.o

all:
	@echo "   BUILD module $(MODULE_NAME)"
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules || exit 1

install:
	@if [ ! -d $(ROMFSDIR)/lib/modules ]; \
        then \
                mkdir $(ROMFSDIR)/lib/modules ; \
        fi
	$(STRIP) $(MODULE_NAME).ko
	cp -af $(MODULE_NAME).ko $(ROMFSDIR)/lib/modules/

clean:
	rm -rf *.o *~ core .depend .*.cmd *.mod.c .tmp_versions *.symvers *.ko modules.order
else
all: 
	@echo " done "
endif
