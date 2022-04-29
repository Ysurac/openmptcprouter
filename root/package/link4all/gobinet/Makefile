#
# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=GobiNet
PKG_VERSION:=2011-07-29-1026
PKG_RELEASE:=1


PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/usb-gobi-net
  CATEGORY:=LINK4ALL
  DEPENDS:=+kmod-usb-net
  TITLE:=Kernel USB driver for GobiNet
  FILES:= $(PKG_BUILD_DIR)/GobiNet.ko
  AUTOLOAD:=$(call AutoLoad,81,GobiNet)
endef

define KernelPackage/GobiNet/Description
This package contains a USB driver for GobiNet3000
endef

define Build/Prepare
	$(CP) src/* $(PKG_BUILD_DIR)
	$(call Build/Prepare/Default)
endef

# define Build/Compile
# 	$(MAKE) -C "$(LINUX_DIR)" \
# 		CROSS_COMPILE="$(TARGET_CROSS)" \
# 		ARCH="$(LINUX_KARCH)" \
# 		SUBDIRS="$(PKG_BUILD_DIR)" \
# 		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
# 		modules
# endef

MAKE_OPTS:= \
	$(KERNEL_MAKE_FLAGS) \
	M="$(PKG_BUILD_DIR)"

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,usb-gobi-net))

