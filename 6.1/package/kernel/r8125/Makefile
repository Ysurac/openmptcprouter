include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=r8125
PKG_VERSION:=9.016.01
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/heri16/r8125/releases/download/v$(PKG_VERSION)/
PKG_MIRROR_HASH:=skip

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=

include $(INCLUDE_DIR)/package.mk

define KernelPackage/r8125
  SUBMENU:=Network Devices
  TITLE:=Driver for Realtek RTL8125 2.5GbE/5GbE network cards
  DEPENDS:=@PCI_SUPPORT +kmod-libphy
  FILES:=$(PKG_BUILD_DIR)/src/r8125.ko
  AUTOLOAD:=$(call AutoProbe,r8125)
  CONFLICTS:=kmod-r8169
endef

define KernelPackage/r8125/description
 This package contains a driver for Realtek RTL8125 2.5 Gigabit and
 5 Gigabit Ethernet network cards.

 Supported devices:
 - RTL8125A
 - RTL8125B
 - RTL8125BG
 - RTL8126A

 This driver provides better performance and stability compared to the
 in-kernel r8169 driver for RTL8125 devices.
endef

R8125_MAKEOPTS= -C $(PKG_BUILD_DIR)/src \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	KSRC="$(LINUX_DIR)" \
	M="$(PKG_BUILD_DIR)/src" \
	ENABLE_RSS_SUPPORT=y \
	CONFIG_SOC_LAN=n

define Build/Compile
	+$(MAKE) $(PKG_JOBS) $(R8125_MAKEOPTS) modules
endef

$(eval $(call KernelPackage,r8125))
