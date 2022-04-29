include $(TOPDIR)/rules.mk

PKG_NAME:=qca-nss-cfi
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2021-03-22
PKG_SOURCE_URL:=https://source.codeaurora.org/quic/qsdk/oss/lklm/nss-cfi
PKG_SOURCE_VERSION:=73f2e5f5bf93cc244036ce5641faf9f859692cdf
PKG_MIRROR_HASH:=8c8edc9b8d8c68fdd14640152245013a93c123957b6b338c06f266f6c9db8cbd

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

# v1.0 is for Akronite
# v2.0 is for Hawkeye/Cypress/Maple
ifneq (, $(findstring $(BOARD), ipq50xx ipq60xx ipq807x))
  CFI_OCF_DIR:=ocf/v2.0
  CFI_CRYPTOAPI_DIR:=cryptoapi/v2.0
else
  CFI_CRYPTOAPI_DIR:=cryptoapi/v1.1
  CFI_OCF_DIR:=ocf/v1.0
  CFI_IPSEC_DIR:=ipsec/v1.0
endif

define KernelPackage/qca-nss-cfi-cryptoapi
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=@TARGET_ipq60xx +kmod-crypto-authenc +kmod-qca-nss-crypto
  TITLE:=Kernel driver for NSS cfi
  FILES:=$(PKG_BUILD_DIR)/$(CFI_CRYPTOAPI_DIR)/qca-nss-cfi-cryptoapi.ko
  AUTOLOAD:=$(call AutoLoad,59,qca-nss-cfi-cryptoapi)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/qca-nss-cfi
	$(CP) $(PKG_BUILD_DIR)/$(CFI_CRYPTOAPI_DIR)/../exports/* $(1)/usr/include/qca-nss-cfi
	$(CP) $(PKG_BUILD_DIR)/include/* $(1)/usr/include/qca-nss-cfi
endef

define KernelPackage/qca-nss-cfi/Description
This package contains a NSS cfi driver for QCA chipset
endef

EXTRA_CFLAGS+= \
	-DCONFIG_NSS_DEBUG_LEVEL=4 \
	-I$(LINUX_DIR)/crypto/ocf \
	-I$(STAGING_DIR)/usr/include/qca-nss-crypto \
	-I$(STAGING_DIR)/usr/include/crypto \
	-I$(STAGING_DIR)/usr/include/qca-nss-drv

ifneq (, $(findstring $(BOARD), ipq50xx ipq60xx ipq807x))
EXTRA_CFLAGS+= -I$(STAGING_DIR)/usr/include/qca-nss-clients
endif

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		CFI_CRYPTOAPI_DIR=$(CFI_CRYPTOAPI_DIR) \
		CFI_IPSEC_DIR=$(CFI_IPSEC_DIR) \
		CFI_OCF_DIR=$(CFI_OCF_DIR) \
		$(KERNEL_MAKE_FLAGS) \
		M="$(PKG_BUILD_DIR)" \
		SoC="$(BOARD)_64" \
		"cryptoapi=y" \
		modules
endef

$(eval $(call KernelPackage,qca-nss-cfi-cryptoapi))
