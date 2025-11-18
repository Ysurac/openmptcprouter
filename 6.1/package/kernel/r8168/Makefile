include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=r8168
PKG_VERSION:=8.055.00
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/mtorromeo/r8168/archive/refs/tags/$(PKG_VERSION).tar.gz
PKG_MIRROR_HASH:=skip

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=

include $(INCLUDE_DIR)/package.mk

define KernelPackage/r8168
  SUBMENU:=Network Devices
  TITLE:=Driver for Realtek RTL8111/8168/8411 PCIe GbE network cards
  DEPENDS:=@PCI_SUPPORT +kmod-libphy
  FILES:=$(PKG_BUILD_DIR)/src/r8168.ko
  AUTOLOAD:=$(call AutoProbe,r8168)
  CONFLICTS:=kmod-r8169
endef

define KernelPackage/r8168/description
 This package contains an improved driver for Realtek RTL8111/8168/8411
 PCI Express Gigabit Ethernet network cards.

 Supported devices:
 - RTL8168B/8111B
 - RTL8168C/8111C
 - RTL8168CP/8111CP
 - RTL8168D/8111D
 - RTL8168DP/8111DP
 - RTL8168E/8111E
 - RTL8168EP/8111EP
 - RTL8168F/8111F
 - RTL8168FP/8111FP
 - RTL8168G/8111G
 - RTL8168GU/8111GU
 - RTL8168H/8111H
 - RTL8411
 - RTL8411B

 This driver provides better performance and stability compared to the
 in-kernel r8169 driver.
endef

R8168_MAKEOPTS= -C $(PKG_BUILD_DIR)/src \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	KSRC="$(LINUX_DIR)" \
	M="$(PKG_BUILD_DIR)/src" \
	ENABLE_RSS_SUPPORT=y

define Build/Compile
	+$(MAKE) $(PKG_JOBS) $(R8168_MAKEOPTS) modules
endef

$(eval $(call KernelPackage,r8168))
