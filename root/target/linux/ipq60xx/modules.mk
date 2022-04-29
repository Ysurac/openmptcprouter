define KernelPackage/usb-phy-msm
  TITLE:=DWC3 USB QCOM PHY driver for IPQ60xx and IPQ807x
  DEPENDS:=@TARGET_ipq60xx +kmod-usb-dwc3-qcom
  KCONFIG:= \
	CONFIG_PHY_QCOM_QMP \
	CONFIG_PHY_QCOM_QUSB2
  FILES:= \
	$(LINUX_DIR)/drivers/phy/qualcomm/phy-qcom-qmp.ko \
	$(LINUX_DIR)/drivers/phy/qualcomm/phy-qcom-qusb2.ko
  AUTOLOAD:=$(call AutoLoad,45,phy-qcom-qmp phy-qcom-qusb2,1)
  $(call AddDepends/usb)
endef

define KernelPackage/usb-phy-msm/description
 This driver provides support for the USB PHY drivers
 within the IPQ60xx and IPQ807x SoCs.
endef

$(eval $(call KernelPackage,usb-phy-msm))
