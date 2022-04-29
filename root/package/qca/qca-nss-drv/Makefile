include $(TOPDIR)/rules.mk

PKG_NAME:=qca-nss-drv
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2021-11-12
PKG_SOURCE_URL:=https://source.codeaurora.org/quic/qsdk/oss/lklm/nss-drv
PKG_SOURCE_VERSION:=5d47e4261bae9bc343ce50402011ee64729684f9
PKG_MIRROR_HASH:=832a6f3af2d37f3e38231a3df41d5d7d9c7abc4975c0445f962146e8950a9433

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/qca-nss-drv
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Devices
  DEPENDS:=@TARGET_ipq60xx +kmod-qca-nss-dp
  TITLE:=Kernel driver for NSS (core driver)
  FILES:=$(PKG_BUILD_DIR)/qca-nss-drv.ko
  AUTOLOAD:=$(call AutoLoad,32,qca-nss-drv)
endef

define KernelPackage/qca-nss-drv/install
	$(INSTALL_DIR) $(1)/lib/debug
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/etc/sysctl.d
	$(INSTALL_DIR) $(1)/etc/config

	$(INSTALL_BIN) ./files/qca-nss-drv.debug $(1)/lib/debug/qca-nss-drv
	$(INSTALL_BIN) ./files/qca-nss-drv.init $(1)/etc/init.d/qca-nss-drv
	$(INSTALL_BIN) ./files/qca-nss-drv.sysctl $(1)/etc/sysctl.d/qca-nss-drv.conf
	$(INSTALL_BIN) ./files/qca-nss-drv.conf $(1)/etc/config/nss
endef

define KernelPackage/qca-nss-drv/Description
This package contains a NSS driver for QCA chipset
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include/qca-nss-drv
	$(CP) $(PKG_BUILD_DIR)/exports/* $(1)/usr/include/qca-nss-drv/
endef

EXTRA_CFLAGS+= -Wno-unused-variable \
	-I$(STAGING_DIR)/usr/include/qca-nss-dp \
	-I$(STAGING_DIR)/usr/include/qca-ssdk

ifeq ($(CONFIG_KERNEL_IPQ_MEM_PROFILE),256)
EXTRA_CFLAGS+= -DNSS_MEM_PROFILE_LOW
endif

ifeq ($(CONFIG_KERNEL_IPQ_MEM_PROFILE),512)
EXTRA_CFLAGS+= -DNSS_MEM_PROFILE_MEDIUM
endif

ifeq ($(CONFIG_KERNEL_SKB_FIXED_SIZE_2K),y)
EXTRA_CFLAGS+= -DNSS_SKB_FIXED_SIZE_2K
endif

DRV_MAKE_OPTS:=
ifeq ($(CONFIG_KERNEL_IPQ_MEM_PROFILE),256)
DRV_MAKE_OPTS+= \
        NSS_DRV_C2C_ENABLE=n \
        NSS_DRV_CAPWAP_ENABLE=n \
        NSS_DRV_CLMAP_ENABLE=n \
        NSS_DRV_CRYPTO_ENABLE=n \
        NSS_DRV_DTLS_ENABLE=n \
        NSS_DRV_GRE_ENABLE=n \
        NSS_DRV_GRE_REDIR_ENABLE=n \
        NSS_DRV_GRE_TUNNEL_ENABLE=n \
        NSS_DRV_IGS_ENABLE=n \
        NSS_DRV_IPSEC_ENABLE=n \
        NSS_DRV_LAG_ENABLE=n \
        NSS_DRV_L2TP_ENABLE=n \
        NSS_DRV_MAPT_ENABLE=n \
        NSS_DRV_OAM_ENABLE=n \
        NSS_DRV_PPTP_ENABLE=n \
        NSS_DRV_PORTID_ENABLE=n \
        NSS_DRV_PVXLAN_ENABLE=n \
        NSS_DRV_QRFS_ENABLE=n \
        NSS_DRV_QVPN_ENABLE=n \
        NSS_DRV_RMNET_ENABLE=n \
        NSS_DRV_SHAPER_ENABLE=n \
        NSS_DRV_SJACK_ENABLE=n \
        NSS_DRV_TLS_ENABLE=n \
        NSS_DRV_TRUSTSEC_ENABLE=n \
        NSS_DRV_TSTAMP_ENABLE=n \
        NSS_DRV_TUN6RD_ENABLE=n \
        NSS_DRV_TUNIPIP6_ENABLE=n \
        NSS_DRV_VXLAN_ENABLE=n
endif

define Build/Configure
	$(LN) arch/nss_$(BOARD)_64.h $(PKG_BUILD_DIR)/exports/nss_arch.h
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" $(strip $(DRV_MAKE_OPTS)) \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		$(KERNEL_MAKE_FLAGS) \
		M="$(PKG_BUILD_DIR)" \
		SoC="$(BOARD)_64" \
		modules
endef

$(eval $(call KernelPackage,qca-nss-drv))
