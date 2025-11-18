include $(TOPDIR)/rules.mk

PKG_NAME:=rtl88x2cu
PKG_RELEASE=1

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=

PKG_SOURCE_URL:=https://github.com/morrownr/8821cu.git
PKG_MIRROR_HASH:=skip
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2024-11-23
PKG_SOURCE_VERSION:=HEAD

PKG_MAINTAINER:=morrownr
PKG_BUILD_PARALLEL:=1

STAMP_CONFIGURED_DEPENDS := $(STAGING_DIR)/usr/include/mac80211-backport/backport/autoconf.h

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/rtl88x2cu
  SUBMENU:=Wireless Drivers
  TITLE:=Driver for Realtek 8812CU/8822CU devices
  DEPENDS:=+kmod-cfg80211 +kmod-usb-core +@DRIVER_11AC_SUPPORT
  FILES:=\
	$(PKG_BUILD_DIR)/8821cu.ko
  AUTOLOAD:=$(call AutoProbe,8821cu)
  PROVIDES:=kmod-rtl88x2cu
endef

define KernelPackage/rtl88x2cu/description
 This package contains a driver for Realtek 8812CU, 8822CU and 8821CU chipsets.
 This driver supports USB WiFi adapters with the following chipsets:
 - RTL8811CU
 - RTL8812CU
 - RTL8821CU
 - RTL8822CU
 - RTL8831CU

 This is an updated driver with support for modern kernels.
 Features:
 - Supports kernel 5.4 through 6.12+
 - Monitor mode support
 - Power saving features
 - AC speeds
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
	CONFIG_RTL8821CU=m \
	USER_MODULE_NAME=8821cu

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,rtl88x2cu))
