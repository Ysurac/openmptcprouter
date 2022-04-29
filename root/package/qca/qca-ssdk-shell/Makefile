include $(TOPDIR)/rules.mk

PKG_NAME:=qca-ssdk-shell
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2021-12-01
PKG_SOURCE_URL:=https://source.codeaurora.org/quic/qsdk/oss/ssdk-shell
PKG_SOURCE_VERSION:=c443bebf58de61a148b35d307bf331bb349f9b8b
PKG_MIRROR_HASH:=b48291620a90b1cfec949447bb2225da8a35fe48338412f1c50af43d9656c7c8

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define Package/qca-ssdk-shell
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Shell application for QCA SSDK
  DEPENDS:=@TARGET_ipq60xx
endef

define Package/qca-ssdk-shell/Description
  This package contains a qca-ssdk shell application for QCA chipset
endef

ifndef CONFIG_TOOLCHAIN_BIN_PATH
CONFIG_TOOLCHAIN_BIN_PATH=$(TOOLCHAIN_DIR)/bin
endif

QCASSDK_CONFIG_OPTS+= \
	ARCH=$(LINUX_KARCH) \
	KVER=$(LINUX_VERSION) \
	SYS_PATH=$(LINUX_DIR) \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	TOOLPREFIX=$(TARGET_CROSS) \
	TOOL_PATH=$(CONFIG_TOOLCHAIN_BIN_PATH)

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) $(strip $(QCASSDK_CONFIG_OPTS))
endef

define Package/qca-ssdk-shell/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/bin/ssdk_sh $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,qca-ssdk-shell))
