include $(TOPDIR)/rules.mk

PKG_NAME:=qca-nss-ecm
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2021-11-09
PKG_SOURCE_URL:=https://source.codeaurora.org/quic/qsdk/oss/lklm/qca-nss-ecm
PKG_SOURCE_VERSION:=33122aebdd54803b5817065060289d6af5dfd3ba
PKG_MIRROR_HASH:=a857db70804e19a20efef2d1ad519fe26bce3a45decd6ae3bb4a26d53f464a8d

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/qca-nss-ecm
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Support
  DEPENDS:=@TARGET_ipq60xx +iptables-mod-extra +iptables-mod-physdev \
	+kmod-ipt-conntrack +kmod-ipt-physdev +kmod-pppoe +kmod-qca-nss-drv
  TITLE:=QCA NSS Enhanced Connection Manager (ECM)
  FILES:=$(PKG_BUILD_DIR)/*.ko
  KCONFIG:= \
	CONFIG_BRIDGE_NETFILTER=y \
	CONFIG_NF_CONNTRACK_EVENTS=y \
	CONFIG_NF_CONNTRACK_CHAIN_EVENTS=y \
	CONFIG_NF_CONNTRACK_DSCPREMARK_EXT=n
endef

define KernelPackage/qca-nss-ecm/Description
This package contains the QCA NSS Enhanced Connection Manager
endef

define KernelPackage/qca-nss-ecm/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/qca-nss-ecm.uci $(1)/etc/config/ecm
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/qca-nss-ecm.init $(1)/etc/init.d/qca-nss-ecm
	$(INSTALL_DIR) $(1)/etc/firewall.d
	$(INSTALL_DATA) ./files/qca-nss-ecm.firewall $(1)/etc/firewall.d/qca-nss-ecm
	$(INSTALL_DIR) $(1)/etc/sysctl.d
	$(INSTALL_BIN) ./files/qca-nss-ecm.sysctl $(1)/etc/sysctl.d/qca-nss-ecm.conf
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_DATA) ./files/qca-nss-ecm.defaults $(1)/etc/uci-defaults/99-qca-nss-ecm
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) ./files/ecm_dump.sh $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/lib/netifd/offload
	$(INSTALL_BIN) ./files/on-demand-down $(1)/lib/netifd/offload/on-demand-down
endef

EXTRA_CFLAGS+=-I$(STAGING_DIR)/usr/include/qca-nss-drv

ifneq (, $(findstring $(BOARD), ipq60xx ipq807x))
ECM_MAKE_OPTS+= \
		ECM_BAND_STEERING_ENABLE=n \
		ECM_CLASSIFIER_DSCP_ENABLE=n \
		ECM_CLASSIFIER_HYFI_ENABLE=n \
		ECM_CLASSIFIER_MARK_ENABLE=n \
		ECM_CLASSIFIER_PCC_ENABLE=n \
		ECM_FRONT_END_NSS_ENABLE=y \
		ECM_INTERFACE_BOND_ENABLE=n \
		ECM_INTERFACE_GRE_TAP_ENABLE=n \
		ECM_INTERFACE_GRE_TUN_ENABLE=n \
		ECM_INTERFACE_IPSEC_ENABLE=n \
		ECM_INTERFACE_L2TPV2_ENABLE=n \
		ECM_INTERFACE_PPPOE_ENABLE=y \
		ECM_INTERFACE_PPTP_ENABLE=n \
		ECM_INTERFACE_RAWIP_ENABLE=n \
		ECM_INTERFACE_SIT_ENABLE=n \
		ECM_INTERFACE_TUNIPIP6_ENABLE=n \
		ECM_INTERFACE_VLAN_ENABLE=n \
		ECM_MULTICAST_ENABLE=n
endif

define Build/InstallDev
	mkdir -p $(1)/usr/include/qca-nss-ecm
	$(CP) $(PKG_BUILD_DIR)/exports/* $(1)/usr/include/qca-nss-ecm
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" $(strip $(ECM_MAKE_OPTS)) \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		$(KERNEL_MAKE_FLAGS) \
		M="$(PKG_BUILD_DIR)" \
		SoC="$(BOARD)_64" \
		modules
endef

$(eval $(call KernelPackage,qca-nss-ecm))
