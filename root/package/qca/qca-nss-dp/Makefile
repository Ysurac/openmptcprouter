include $(TOPDIR)/rules.mk

PKG_NAME:=qca-nss-dp
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2021-03-26
PKG_SOURCE_URL:=https://source.codeaurora.org/quic/qsdk/oss/lklm/nss-dp
PKG_SOURCE_VERSION:=e0c89348d5ad99559ce2fbe15d37b3b5bc66aa03
PKG_MIRROR_HASH:=f369f0c3b33b5f4ad6d0a6ad6ac5495f63c9ecaf94e4e7fa345169f3e44fcf45

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/qca-nss-dp
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=@TARGET_ipq60xx +kmod-qca-ssdk
  TITLE:=Kernel driver for NSS data plane
  FILES:=$(PKG_BUILD_DIR)/qca-nss-dp.ko
  AUTOLOAD:=$(call AutoLoad,31,qca-nss-dp)
endef

define KernelPackage/qca-nss-dp/Description
This package contains a NSS data plane driver for QCA chipset
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include/qca-nss-dp
	$(CP) $(PKG_BUILD_DIR)/exports/* $(1)/usr/include/qca-nss-dp/
endef

EXTRA_CFLAGS+= \
	-I$(STAGING_DIR)/usr/include/qca-ssdk

NSS_DP_HAL_DIR:=$(PKG_BUILD_DIR)/hal

define Build/Configure
	$(LN) $(NSS_DP_HAL_DIR)/arch/$(BOARD)/nss_$(BOARD).h \
		$(PKG_BUILD_DIR)/exports/nss_dp_arch.h
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		$(KERNEL_MAKE_FLAGS) \
		M="$(PKG_BUILD_DIR)" \
		SoC="$(BOARD)_64" \
		modules
endef

$(eval $(call KernelPackage,qca-nss-dp))
