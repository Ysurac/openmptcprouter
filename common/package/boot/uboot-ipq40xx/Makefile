#
# Copyright (c) 2014 2015 The Linux Foundation. All rights reserved.
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=uboot

#PKG_SOURCE_VERSION:=e9af48b5da6b8b9ddce90bef0097cadbab257c30

DEVICE_VENDOR='teltonika'
PKG_VERSION:=teltonika

include $(INCLUDE_DIR)/package.mk

define Package/uboot-ipq40xx
  SECTION:=boot
  CATEGORY:=Boot Loaders
  TITLE:=U-boot for ipq40xx based platforms
  URL:=http://www.denx.de/wiki/U-Boot
endef

define Build/Configure
	$(MAKE) -C $(PKG_BUILD_DIR) ipq40xx_cdp_config
endef

define Build/Compile
	VERSION_CODE='$(VERSION_NICK) $(VERSION_NUMBER)' \
	REVISION='$(REVISION)' DEVICE_VENDOR='$(DEVICE_VENDOR)' \
	$(MAKE) -C $(PKG_BUILD_DIR) HOSTCC=$(TARGET_CC) \
		HOSTSTRIP=true CROSS_COMPILE=$(TARGET_CROSS) \
		QSDK_CFLAGS="$(TARGET_CFLAGS)" \
		QSDK_LDFLAGS="$(TARGET_LDFLAGS)"

endef

define Package/uboot-ipq40xx/install
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_BUILD_DIR)/u-boot $(BIN_DIR)/openwrt-ipq40xx-u-boot.elf
	$(CP) $(PKG_BUILD_DIR)/striped-u-boot.elf $(BIN_DIR)/openwrt-ipq40xx-u-boot-stripped.elf
	$(CP) $(PKG_BUILD_DIR)/u-boot.bin $(BIN_DIR)/openwrt-ipq40xx-u-boot.img
endef

$(eval $(call BuildPackage,uboot-ipq40xx))
