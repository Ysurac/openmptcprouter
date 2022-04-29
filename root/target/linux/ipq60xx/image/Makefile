include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Device/Default
	PROFILES := Default
	KERNEL_DEPENDS = $$(wildcard $(DTS_DIR)/$$(DEVICE_DTS).dts)
	KERNEL_INITRAMFS_PREFIX := $$(IMG_PREFIX)-$(1)-initramfs
	KERNEL_PREFIX := $$(IMAGE_PREFIX)
	KERNEL_LOADADDR := 0x41080000
	DEVICE_DTS_DIR := $(DTS_DIR)/qcom
	DEVICE_DTS = $$(SOC)-$(lastword $(subst _, ,$(1)))
	IMAGE/sysupgrade.bin = sysupgrade-tar | append-metadata
	IMAGE/sysupgrade.bin/squashfs :=
endef

define Device/FitImage
	KERNEL_SUFFIX := -fit-uImage.itb
	KERNEL = kernel-bin | gzip | fit gzip $$(DEVICE_DTS_DIR)/$$(DEVICE_DTS).dtb
	KERNEL_NAME := Image
endef

define Device/FitImageLzma
	KERNEL_SUFFIX := -fit-uImage.itb
	KERNEL = kernel-bin | lzma | fit lzma $$(DEVICE_DTS_DIR)/$$(DEVICE_DTS).dtb
	KERNEL_NAME := Image
endef

define Device/UbiFit
	KERNEL_IN_UBI := 1
	IMAGES := nand-factory.ubi nand-sysupgrade.bin
	IMAGE/nand-factory.ubi := append-ubi
	IMAGE/nand-sysupgrade.bin := sysupgrade-tar | append-metadata
endef

define Device/ylx_q60
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := YLX
	DEVICE_MODEL := Q60
	DEVICE_DTS_CONFIG := config@cp03-c1
	SOC := ipq6018
	BLOCKSIZE := 128k
	PAGESIZE := 2048
endef
TARGET_DEVICES += ylx_q60

define Device/ylx_x5
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := YLX
	DEVICE_MODEL := X5
	DEVICE_DTS_CONFIG := config@cp01-c1
	SOC := ipq6018
	BLOCKSIZE := 128k
	PAGESIZE := 2048
endef
TARGET_DEVICES += ylx_x5

define Device/ylx_x8
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := YLX
	DEVICE_MODEL := X8
	DEVICE_DTS_CONFIG := config@cp01-c3
	SOC := ipq6018
	BLOCKSIZE := 128k
	PAGESIZE := 2048
endef
TARGET_DEVICES += ylx_x8

define Device/ylx_x511
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := YLX
	DEVICE_MODEL := X511
	DEVICE_DTS_CONFIG := config@cp03-c1
	SOC := ipq6018
	BLOCKSIZE := 128k
	PAGESIZE := 2048
endef
TARGET_DEVICES += ylx_x511

$(eval $(call BuildImage))
