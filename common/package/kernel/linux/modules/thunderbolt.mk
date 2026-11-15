define KernelPackage/usb4
  SUBMENU:=$(OTHER_MENU)
  TITLE:=USB4 and Thunderbolt support
  # 7.3+ builds configfs.o into thunderbolt.ko when CONFIGFS_FS=m
  DEPENDS:=@PCI_SUPPORT +kmod-fs-configfs
  # Tunnelled PCIe devices (Thunderbolt NICs) only show up through pciehp
  KCONFIG:= \
	CONFIG_USB4 \
	CONFIG_HOTPLUG_PCI_PCIE=y \
	CONFIG_USB4_NET=n \
	CONFIG_USB4_DEBUGFS_WRITE=n \
	CONFIG_USB4_KUNIT_TEST=n \
	CONFIG_USB4_DMA_TEST=n \
	CONFIG_USB4_STREAM=n \
	CONFIG_DRM_I915_DP_TUNNEL=n \
	CONFIG_DRM_XE_DP_TUNNEL=n
  FILES:=$(LINUX_DIR)/drivers/thunderbolt/thunderbolt.ko
  AUTOLOAD:=$(call AutoProbe,thunderbolt)
endef

define KernelPackage/usb4/description
 USB4 and Thunderbolt host controller driver, so PCIe devices behind a
 Thunderbolt link (10G NICs...) work without BIOS pre-boot support.
 Devices are approved automatically: there is no boltd on OpenWrt.
endef

define KernelPackage/usb4/install
	$(INSTALL_DIR) $(1)/etc/hotplug.d/thunderbolt
	$(INSTALL_DATA) ./files/hotplug-usb4-authorize.sh \
		$(1)/etc/hotplug.d/thunderbolt/10-authorize
endef

$(eval $(call KernelPackage,usb4))
