include $(TOPDIR)/rules.mk

PKG_NAME:=qca-nss-crypto
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2021-03-22
PKG_SOURCE_URL:=https://source.codeaurora.org/quic/qsdk/oss/lklm/nss-crypto
PKG_SOURCE_VERSION:=2271a3a66f7e8284d42a9e787ddec6f24a1d2e15
PKG_MIRROR_HASH:=7e70ffa2cbddc3830376f25047575db8867f2027b2ae3a7276d87ead3e95eb80

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

# v1.0 is for Akronite
# v2.0 is for Hawkeye/Cypress/Maple
ifneq (, $(findstring $(BOARD), ipq50xx ipq60xx ipq807x))
NSS_CRYPTO_DIR:=v2.0
else
NSS_CRYPTO_DIR:=v1.0
endif

define KernelPackage/qca-nss-crypto
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  TITLE:=Kernel driver for NSS crypto driver
  FILES:= \
	$(PKG_BUILD_DIR)/$(NSS_CRYPTO_DIR)/src/qca-nss-crypto.ko \
	$(PKG_BUILD_DIR)/$(NSS_CRYPTO_DIR)/tool/qca-nss-crypto-tool.ko
  DEPENDS:=@TARGET_ipq60xx +kmod-qca-nss-drv
  AUTOLOAD:=$(call AutoLoad,52,qca-nss-crypto)
endef

define KernelPackage/qca-nss-crypto/Description
This package contains a NSS crypto driver for QCA chipset
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/qca-nss-crypto
	$(CP) $(PKG_BUILD_DIR)/$(NSS_CRYPTO_DIR)/include/* $(1)/usr/include/qca-nss-crypto
endef

EXTRA_CFLAGS+= \
	-DCONFIG_NSS_DEBUG_LEVEL=4 \
	-I$(STAGING_DIR)/usr/include/qca-nss-crypto \
	-I$(STAGING_DIR)/usr/include/qca-nss-drv \
	-I$(PKG_BUILD_DIR)/$(NSS_CRYPTO_DIR)/include \
	-I$(PKG_BUILD_DIR)/$(NSS_CRYPTO_DIR)/src

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		NSS_CRYPTO_DIR=$(NSS_CRYPTO_DIR) \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		$(KERNEL_MAKE_FLAGS) \
		M="$(PKG_BUILD_DIR)" \
		SoC="$(BOARD)_64" \
		modules
endef

$(eval $(call KernelPackage,qca-nss-crypto))
