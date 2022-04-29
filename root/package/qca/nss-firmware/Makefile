#
# Copyright (C) 2021 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nss-firmware
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2021-09-14
PKG_SOURCE_URL:=https://github.com/quic/qca-sdk-nss-fw.git
PKG_SOURCE_VERSION:=b06171cb0d14360c90a4b8f9f835dc1c4647ea07
PKG_MIRROR_HASH:=e851d4f6af8aaccbc34a05bc007991a88287913f17c5d9f0172a00745aed6e48

PKG_LICENSE_FILES:=LICENSE.md

PKG_MAINTAINER:=Robert Marko <robimarko@gmail.com>

include $(INCLUDE_DIR)/package.mk

VERSION_PATH=$(PKG_BUILD_DIR)/QCA_Networking_2021.SPF_11.4/CS

define Package/nss-firmware-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=$(PKG_SOURCE_URL)
endef

define Package/nss-firmware-ipq6018
$(Package/nss-firmware-default)
  DEPENDS:=@TARGET_ipq60xx
  TITLE:=NSS firmware for IPQ6018 devices
  NSS_ARCHIVE:=$(VERSION_PATH)/IPQ6018.ATH.11.4/BIN-NSS.CP.11.4.0.5-5-R.tar.bz2
endef

define Package/nss-firmware-ipq8074
$(Package/nss-firmware-default)
  DEPENDS:=@TARGET_ipq807x
  TITLE:=NSS firmware for IPQ8074 devices
  NSS_ARCHIVE:=$(VERSION_PATH)/IPQ8074.ATH.11.4/BIN-NSS.HK.11.4.0.5-5-R.tar.bz2
endef

define Build/Compile
	true
endef

define Package/nss-firmware-ipq6018/install
	mkdir -p $(PKG_BUILD_DIR)/IPQ6018
	$(TAR) -C $(PKG_BUILD_DIR)/IPQ6018 -xf $(NSS_ARCHIVE) --strip-components=1
	$(INSTALL_DIR) $(1)/lib/firmware/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/IPQ6018/retail_router0.bin \
		$(1)/lib/firmware/qca-nss0.bin
endef

define Package/nss-firmware-ipq8074/install
	mkdir -p $(PKG_BUILD_DIR)/IPQ8074
	$(TAR) -C $(PKG_BUILD_DIR)/IPQ8074 -xf $(NSS_ARCHIVE) --strip-components=1
	$(INSTALL_DIR) $(1)/lib/firmware/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/IPQ8074/retail_router0.bin \
		$(1)/lib/firmware/qca-nss0.bin
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/IPQ8074/retail_router1.bin \
		$(1)/lib/firmware/qca-nss1.bin
endef

$(eval $(call BuildPackage,nss-firmware-ipq6018))
$(eval $(call BuildPackage,nss-firmware-ipq8074))
