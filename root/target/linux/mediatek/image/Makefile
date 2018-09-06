#
# Copyright (C) 2012-2015 OpenWrt.org
# Copyright (C) 2016-2017 LEDE project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

# for arm
KERNEL_LOADADDR := 0x80008000

# for arm64
ifeq ($(SUBTARGET),mt7622)
KERNEL_LOADADDR = 0x41080000
endif

# build dtb
define Build/dtb
  $(call Image/BuildDTB,$(DEVICE_DTS_DIR)/$(DEVICE_DTS).dts,$(DEVICE_DTS_DIR)/$(DEVICE_DTS).dtb)
  $(CP) $(DEVICE_DTS_DIR)/$(DEVICE_DTS).dtb $(BIN_DIR)/
endef

define Build/sysupgrade-emmc
  rm -f $@.recovery
  mkfs.fat -C $@.recovery 3070

  dd bs="512" of="$@" if="$(IMAGE_KERNEL)"
  dd bs="512" of="$@" if="$@.recovery" seek="67072"
  dd bs="512" of="$@" if="$(IMAGE_ROOTFS)" seek="87552"
  dd if=/dev/zero of="$@" bs=128k count=1 oflag=append conv=notrunc
endef

define Build/sysupgrade-bpi-r2-sd
  rm -f $@.recovery
  mkfs.fat -C $@.recovery 3070
  dd bs="1024" if="$(STAGING_DIR_IMAGE)/mtk-bpi-r2-preloader-sd.bin" of="$@" seek="0"
  dd bs="1024" if="$(STAGING_DIR_IMAGE)/mtk-bpi-r2-uboot.bin" of="$@" seek="320"
  dd bs="1024" if="$(IMAGE_KERNEL)" of="$@" seek="2048"
  dd bs="1024" if="$@.recovery" of="$@" seek="33792"
  dd bs="1024" if="$(IMAGE_ROOTFS)" of="$@" seek="67584"
endef
define Build/sysupgrade-bpi-r2-emmc
  dd bs="1024" if="$(STAGING_DIR_IMAGE)/mtk-bpi-r2-preloader-emmc.bin" of="$@" seek="0"
  dd bs="1024" if="$(STAGING_DIR_IMAGE)/mtk-bpi-r2-uboot.bin" of="$@" seek="320"
  dd bs="1024" if="$(IMAGE_KERNEL)" of="$@" seek="2048"
  dd bs="1024" if="$(IMAGE_ROOTFS)" of="$@" seek="67584"
endef

# default all platform image(fit) build 
define Device/Default
  PROFILES = Default $$(DEVICE_NAME)
  KERNEL_NAME := zImage
#  FILESYSTEMS := squashfs
  DEVICE_DTS_DIR := $(DTS_DIR)
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | append-rootfs | pad-rootfs | append-metadata 
ifeq ($(SUBTARGET),mt7623)
  KERNEL_NAME := zImage
  KERNEL := dtb | kernel-bin | append-dtb | uImage none
  KERNEL_INITRAMFS := dtb | kernel-bin | append-dtb | uImage none
endif
ifeq ($(SUBTARGET),mt7622)
  KERNEL_NAME := Image
  KERNEL = dtb | kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
  KERNEL_INITRAMFS = dtb | kernel-bin | lzma | fit lzma $$(KDIR)/image-$$(firstword $$(DEVICE_DTS)).dtb
endif
endef

ifeq ($(SUBTARGET),mt7622)
include mt7622.mk
endif

ifeq ($(SUBTARGET),mt7623)
include mt7623.mk
endif

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

$(eval $(call BuildImage))

