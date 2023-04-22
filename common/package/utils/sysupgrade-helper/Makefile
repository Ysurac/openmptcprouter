include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/version.mk

PKG_DISTNAME:=sysupgrade-helper
PKG_NAME:=u-boot-ipq86xx
PKG_VERSION:=1.0
PKG_RELEASE:=1

#PKG_SOURCE_DATE:=2021-03-03
#PKG_SOURCE_VERSION:=23cc677bb2dc41f0077a1bef660649070bf4b8c9

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_DISTNAME)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/sysupgrade-helper
  SECTION:=boot
  CATEGORY:=Boot Loaders
  TITLE:=U-boot images tools (dumpimage, mkimage)
endef

define Build/Configure
	$(MAKE) -C $(PKG_BUILD_DIR) ipq806x_cdp_config
endef

define Build/Compile
	cp config $(PKG_BUILD_DIR)/.config
	$(MAKE) -C $(PKG_BUILD_DIR) HOSTCC=$(TARGET_CC) \
		HOSTSTRIP=true CROSS_COMPILE=$(TARGET_CROSS) \
		QSDK_CFLAGS="$(TARGET_CFLAGS)" \
		QSDK_LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/sysupgrade-helper/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tools/mkimage $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tools/dumpimage $(1)/usr/bin
endef

$(eval $(call BuildPackage,sysupgrade-helper))
