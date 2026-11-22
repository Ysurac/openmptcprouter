#
# This is free software, licensed under the GNU General Public License v2.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=r2ec
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/r2ec
  SUBMENU:=GPIO support
  TITLE:=Teltonika RUTX STM32 (R2EC) I2C GPIO expander
  DEPENDS:=@TARGET_ipq40xx
  FILES:=$(PKG_BUILD_DIR)/r2ec.ko
  AUTOLOAD:=$(call AutoLoad,30,r2ec,1)
  KCONFIG:=
endef

define KernelPackage/r2ec/description
 LEDs, modem power/reset and SIM select of the Teltonika RUTX boards
 that use an STM32 instead of 74HC595 shift registers (-stm32 images).
endef

define Build/Compile
	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" modules
endef

$(eval $(call KernelPackage,r2ec))
