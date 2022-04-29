#
# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=usb-serial-ch9344
PKG_VERSION:=2022-03-01-1026
PKG_RELEASE:=1


PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/usb-serial-ch9344
  SUBMENU:=USB Support
  DEPENDS:=+kmod-usb-serial
  TITLE:=Kernel USB driver for ch342/3
  FILES:= $(PKG_BUILD_DIR)/ch9344.ko
  AUTOLOAD:=$(call AutoLoad,81,ch9344)
endef

define KernelPackage/usb-serial-ch9344/Description
This package contains a USB driver for ch9344
endef

define Build/Prepare
	$(CP) src/* $(PKG_BUILD_DIR)
	$(call Build/Prepare/Default)
endef

MAKE_OPTS:= \
	$(KERNEL_MAKE_FLAGS) \
	M="$(PKG_BUILD_DIR)"

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,usb-serial-ch9344))

