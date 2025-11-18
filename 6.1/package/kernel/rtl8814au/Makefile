include $(TOPDIR)/rules.mk

PKG_NAME:=rtl8814au
PKG_RELEASE=1

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=

PKG_SOURCE_URL:=https://github.com/morrownr/8814au.git
PKG_MIRROR_HASH:=skip
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2025-09-03
PKG_SOURCE_VERSION:=8d82854

PKG_MAINTAINER:=morrownr
PKG_BUILD_PARALLEL:=1

STAMP_CONFIGURED_DEPENDS := $(STAGING_DIR)/usr/include/mac80211-backport/backport/autoconf.h

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/rtl8814au
  SUBMENU:=Wireless Drivers
  TITLE:=Driver for Realtek 8814AU devices (AC1900)
  DEPENDS:=+kmod-cfg80211 +kmod-usb-core +@DRIVER_11AC_SUPPORT
  FILES:=\
	$(PKG_BUILD_DIR)/8814au.ko
  AUTOLOAD:=$(call AutoProbe,8814au)
  PROVIDES:=kmod-rtl8814au
endef

define KernelPackage/rtl8814au/description
 This package contains a driver for Realtek 8814AU chipset.
 This driver supports USB WiFi adapters with the RTL8814AU chipset (AC1900).

 Features:
 - Supports kernel 5.4 through 6.12+
 - Monitor mode and frame injection support
 - Concurrent mode support
 - AC1900 speeds (1300 Mbps @ 5 GHz + 600 Mbps @ 2.4 GHz)
 - MU-MIMO support
endef

NOSTDINC_FLAGS := \
	$(KERNEL_NOSTDINC_FLAGS) \
	-I$(PKG_BUILD_DIR) \
	-I$(PKG_BUILD_DIR)/include \
	-I$(STAGING_DIR)/usr/include/mac80211-backport \
	-I$(STAGING_DIR)/usr/include/mac80211-backport/uapi \
	-I$(STAGING_DIR)/usr/include/mac80211 \
	-I$(STAGING_DIR)/usr/include/mac80211/uapi \
	-include backport/backport.h \
	-Wno-error=address \
	-Wno-error=stringop-overread \
	-Wno-error=stringop-overflow

NOSTDINC_FLAGS+=-DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT -DBUILD_OPENWRT

MAKE_OPTS:= \
	$(KERNEL_MAKE_FLAGS) \
	M="$(PKG_BUILD_DIR)" \
	NOSTDINC_FLAGS="$(NOSTDINC_FLAGS)" \
	CONFIG_RTL8814AU=m \
	USER_MODULE_NAME=8814au

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,rtl8814au))
