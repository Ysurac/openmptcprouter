include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=enhanced-driver-support
PKG_VERSION:=1.0
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/enhanced-driver-support
  SUBMENU:=Network Support
  TITLE:=Enhanced driver support for modern hardware
  DEPENDS:=@TARGET_x86||TARGET_x86_64 +kmod-usb-net +kmod-usb-net-cdc-ether +kmod-usb-net-rndis
  KCONFIG:= \
	CONFIG_USB_NET_QMI_WWAN=m \
	CONFIG_USB_SERIAL_OPTION=m \
	CONFIG_USB_NET_CDC_NCM=m \
	CONFIG_USB_NET_CDC_MBIM=m \
	CONFIG_USB_ACM=m \
	CONFIG_PHYLIB=y \
	CONFIG_AQUANTIA_PHY=m \
	CONFIG_REALTEK_PHY=m \
	CONFIG_MARVELL_PHY=m \
	CONFIG_MICROCHIP_PHY=m \
	CONFIG_INTEL_PHY=m \
	CONFIG_AT803X_PHY=m \
	CONFIG_BCM87XX_PHY=m \
	CONFIG_BCM7XXX_PHY=m \
	CONFIG_DP83822_PHY=m \
	CONFIG_DP83848_PHY=m \
	CONFIG_DP83867_PHY=m \
	CONFIG_DP83869_PHY=m \
	CONFIG_DP83TD510_PHY=m \
	CONFIG_MICROSEMI_PHY=m \
	CONFIG_NXP_TJA11XX_PHY=m \
	CONFIG_USB_XHCI_HCD=y \
	CONFIG_USB_XHCI_PCI=y \
	CONFIG_USB_XHCI_PLATFORM=y \
	CONFIG_USB_EHCI_HCD=y \
	CONFIG_USB_OHCI_HCD=y \
	CONFIG_PCIEPORTBUS=y \
	CONFIG_HOTPLUG_PCI_PCIE=y
  FILES:= \
	$(LINUX_DIR)/drivers/net/usb/qmi_wwan.ko \
	$(LINUX_DIR)/drivers/usb/serial/usb_wwan.ko \
	$(LINUX_DIR)/drivers/usb/serial/option.ko
  AUTOLOAD:=$(call AutoProbe,qmi_wwan option usb_wwan)
endef

define KernelPackage/enhanced-driver-support/description
  Enhanced driver support for modern network hardware including:
  - Extended PHY support (Aquantia, Realtek 2.5G/5G, Marvell, Intel)
  - Modern USB modem support (Quectel RM5xx, Fibocom, Sierra Wireless)
  - Improved USB 3.x/4.0 controller support
  - Better PCIe device support
endef

$(eval $(call KernelPackage,enhanced-driver-support))
