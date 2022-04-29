include $(TOPDIR)/rules.mk

PKG_NAME:=nss-eip-firmware
PKG_VERSION=2.5.7
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/nss-eip-firmware
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=NSS EIP-197 firmware
  DEPENDS:=@(TARGET_ipq60xx||TARGET_ipq807x)
endef

define Build/Compile
	true
endef

define Package/nss-eip-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/
	$(CP) ./files/$(BOARD)/* $(1)/lib/firmware/
endef

$(eval $(call BuildPackage,nss-eip-firmware))
