# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Device/Default
	PROFILES := Default
	KERNEL_LOADADDR := 0x41080000
	KERNEL_DEPENDS = $$(wildcard $(DTS_DIR)/$$(DEVICE_DTS).dts)
	KERNEL_INITRAMFS_PREFIX := $$(IMG_PREFIX)-$(1)-initramfs
	KERNEL_PREFIX := $$(IMAGE_PREFIX)
	SUPPORTED_DEVICES := $(subst _,$(comma),$(1))
	IMAGE/sysupgrade.bin = sysupgrade-tar | append-metadata
	IMAGE/sysupgrade.bin/squashfs :=
endef

define Device/LegacyImage
	KERNEL_SUFFIX := -uImage
	KERNEL = kernel-bin | append-dtb | uImage none
	KERNEL_NAME := zImage
endef

define Device/FitImage
	KERNEL_SUFFIX := -fit-uImage.itb
	KERNEL = kernel-bin | gzip | fit gzip $$(DTS_DIR)/$$(DEVICE_DTS).dtb
	KERNEL_NAME := Image
endef

define Device/FitImageLzma
	KERNEL_SUFFIX := -fit-uImage.itb
	KERNEL = kernel-bin | lzma | fit lzma $$(DTS_DIR)/$$(DEVICE_DTS).dtb
	KERNEL_NAME := Image
endef

define Device/UbiFit
	KERNEL_IN_UBI := 1
	IMAGES := nand-factory.bin nand-sysupgrade.bin
	IMAGE/nand-factory.bin := append-ubi
	IMAGE/nand-sysupgrade.bin := sysupgrade-tar | append-metadata
endef

define Device/pangu_l6018
	$(call Device/FitImage)
	$(call Device/UbiFit)        
	DEVICE_DTS := ipq6018-l6018
	DEVICE_DTS_CONFIG := config@cp01-c3
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	BOARD_NAME := ap-cp01-c3
	DEVICE_TITLE := PANGU L6018
endef
TARGET_DEVICES += pangu_l6018

define Device/wf_hr6001
	$(call Device/FitImage)
	$(call Device/UbiFit)        
	KERNEL_LOADADDR := 0x41080000
	DEVICE_DTS := ipq6018-hr6001
	DEVICE_DTS_CONFIG := config@cp01-c1
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	BOARD_NAME := ap-cp01-c1
	DEVICE_TITLE := WF HR6001        
endef
TARGET_DEVICES += wf_hr6001

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

$(eval $(call BuildImage))
