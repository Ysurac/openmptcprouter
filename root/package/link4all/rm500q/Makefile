#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=rm500q
PKG_RELEASE:=3
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/rm500q
  CATEGORY:=LINK4ALL
  DEPENDS:=+kmod-usb-net +kmod-usb-wdm
  TITLE:=rm500q driver
  FILES:=$(PKG_BUILD_DIR)/rm500q.ko
  AUTOLOAD:=$(call AutoLoad,81,rm500q,1)
  KCONFIG:=
endef

define KernelPackage/rm500q/description
 rm500q driver
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

$(eval $(call KernelPackage,rm500q))
