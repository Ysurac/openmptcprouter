#
# Copyright (C) 2012-2016 OpenWrt.org
# Copyright (C) 2017 LEDE project
# Copyright (C) 2019 Ycarus (Yannick Chabanois) <ycarus@zugaina.org> for OpenMPTCProuter project
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=brcm2708-gpu-fw
PKG_VERSION:=2019-07-05
PKG_RELEASE:=cbf712782411e563be8c5c3d514f3ff10a5cc8d3

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)/rpi-firmware-$(PKG_RELEASE)

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

RPI_FIRMWARE_URL:=@GITHUB/raspberrypi/firmware/$(PKG_RELEASE)/boot/
RPI_FIRMWARE_FILE:=rpi-firmware-$(PKG_RELEASE)

define Download/LICENCE_broadcom
  FILE:=$(RPI_FIRMWARE_FILE)-LICENCE.broadcom
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=LICENCE.broadcom
  HASH:=ba76edfc10a248166d965b8eaf320771c44f4f432d4fce2fd31fd272e7038add
endef
$(eval $(call Download,LICENCE_broadcom))

define Download/bootcode_bin
  FILE:=$(RPI_FIRMWARE_FILE)-bootcode.bin
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=bootcode.bin
  HASH:=6948c07f60f93489bbbc78747a1dc09ff0b1de42035c915fbd6453cdbd0e6475
endef
$(eval $(call Download,bootcode_bin))

define Download/fixup_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup.dat
  HASH:=295d09bf528e13a85e46d00d65e26c61458c9d1574660fab8a230f9bd27143f3
endef
$(eval $(call Download,fixup_dat))

define Download/fixup_cd_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup_cd.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup_cd.dat
  HASH:=021fe0db47e17199c3dcf092203b0ed7cc84ff3297ad8033c9477e02ab665344
endef
$(eval $(call Download,fixup_cd_dat))

define Download/fixup_x_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup_x.dat
  URL:=$(RPI_FIRMWARE_URL)
  HASH:=ea6f1163e3f903632da2cd7a3b0aade6ee01df870a70c65b1436b8c144f775ab
  URL_FILE:=fixup_x.dat
endef
$(eval $(call Download,fixup_x_dat))

define Download/start_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start.elf
  HASH:=6631618bcd38286e78b5c0c1f7e340f7d084ea30b48d5b1fbefa77158b48e7d4
endef
$(eval $(call Download,start_elf))

define Download/start_cd_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start_cd.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start_cd.elf
  HASH:=2b66f5f4431560a8a65a7cd7061f23762d639e3f8ff63b01f0ef380f52fd59d8
endef
$(eval $(call Download,start_cd_elf))

define Download/start_x_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start_x.elf
  URL:=$(RPI_FIRMWARE_URL)
  HASH:=23025c91e69eb0ec61ff1344448b0b61cca2c5bb273b30716bfb0c408b924897
  URL_FILE:=start_x.elf
endef
$(eval $(call Download,start_x_elf))

define Download/fixup4_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup4.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup4.dat
  HASH:=b28e24af11e1e04cd28a8f1a85216dd318a076aeb0b041d94de3232173557d3c
endef
$(eval $(call Download,fixup4_dat))

define Download/fixup4cd_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup4cd.dat
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=fixup4cd.dat
  HASH:=d62976dc8b0653c796910e2fe3f07cce84592b36e2b8f068710d371069a84ce7
endef
$(eval $(call Download,fixup4cd_dat))

define Download/fixup4x_dat
  FILE:=$(RPI_FIRMWARE_FILE)-fixup4x.dat
  URL:=$(RPI_FIRMWARE_URL)
  HASH:=9fe9bc97ae797e3fe7a0afed025d84a20cbd06462942b0a5a92d9cc1cd306f6b
  URL_FILE:=fixup4x.dat
endef
$(eval $(call Download,fixup4x_dat))

define Download/start4elf
  FILE:=$(RPI_FIRMWARE_FILE)-start4.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start4.elf
  HASH:=9b69679ba3e8576449488170975b857e0b5c231cec75b613f26a53a6c18648b3
endef
$(eval $(call Download,start4elf))

define Download/start4cd_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start4cd.elf
  URL:=$(RPI_FIRMWARE_URL)
  URL_FILE:=start4cd.elf
  HASH:=8b0a764e2896a9e43af859544f0e5702185702a46149d6dbf09a6da3310395e3
endef
$(eval $(call Download,start4cd_elf))

define Download/start4x_elf
  FILE:=$(RPI_FIRMWARE_FILE)-start4x.elf
  URL:=$(RPI_FIRMWARE_URL)
  HASH:=cb13f91356df1b5310061af07a9a8dd7cb5dcb1fcc73ff0e86d424a4de35006e
  URL_FILE:=start4x.elf
endef
$(eval $(call Download,start4x_elf))

define Package/brcm2708-gpu-fw
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_brcm2708
  TITLE:=brcm2708-gpu-fw
  DEFAULT:=y if TARGET_brcm2708
endef

define Package/brcm2708-gpu-fw/description
 GPU and kernel boot firmware for brcm2708. 
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	cp $(DL_DIR)/$(RPI_FIRMWARE_FILE)-LICENCE.broadcom $(PKG_BUILD_DIR)/LICENCE.broadcom
	cp $(DL_DIR)/$(RPI_FIRMWARE_FILE)-bootcode.bin $(PKG_BUILD_DIR)/bootcode.bin
	cp $(DL_DIR)/$(RPI_FIRMWARE_FILE)-fixup.dat $(PKG_BUILD_DIR)/fixup.dat
	cp $(DL_DIR)/$(RPI_FIRMWARE_FILE)-fixup_cd.dat $(PKG_BUILD_DIR)/fixup_cd.dat
	cp $(DL_DIR)/$(RPI_FIRMWARE_FILE)-fixup_x.dat $(PKG_BUILD_DIR)/fixup_x.dat
	cp $(DL_DIR)/$(RPI_FIRMWARE_FILE)-start.elf $(PKG_BUILD_DIR)/start.elf
	cp $(DL_DIR)/$(RPI_FIRMWARE_FILE)-start_cd.elf $(PKG_BUILD_DIR)/start_cd.elf
	cp $(DL_DIR)/$(RPI_FIRMWARE_FILE)-start_x.elf $(PKG_BUILD_DIR)/start_x.elf
	cp $(DL_DIR)/$(RPI_FIRMWARE_FILE)-fixup4.dat $(PKG_BUILD_DIR)/fixup4.dat
	cp $(DL_DIR)/$(RPI_FIRMWARE_FILE)-fixup4cd.dat $(PKG_BUILD_DIR)/fixup4cd.dat
	cp $(DL_DIR)/$(RPI_FIRMWARE_FILE)-fixup4x.dat $(PKG_BUILD_DIR)/fixup4x.dat
	cp $(DL_DIR)/$(RPI_FIRMWARE_FILE)-start4.elf $(PKG_BUILD_DIR)/start4.elf
	cp $(DL_DIR)/$(RPI_FIRMWARE_FILE)-start4cd.elf $(PKG_BUILD_DIR)/start4cd.elf
	cp $(DL_DIR)/$(RPI_FIRMWARE_FILE)-start4x.elf $(PKG_BUILD_DIR)/start4x.elf
endef

define Build/Compile
	true
endef

define Package/brcm2708-gpu-fw/install
	true
endef

define Build/InstallDev
	$(CP) $(PKG_BUILD_DIR)/bootcode.bin $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/LICENCE.broadcom $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/start.elf $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/start_cd.elf $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/start_x.elf $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/fixup.dat $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/fixup_cd.dat $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/fixup_x.dat $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/start4.elf $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/start4cd.elf $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/start4x.elf $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/fixup4.dat $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/fixup4cd.dat $(KERNEL_BUILD_DIR)
	$(CP) $(PKG_BUILD_DIR)/fixup4x.dat $(KERNEL_BUILD_DIR)
endef

$(eval $(call BuildPackage,brcm2708-gpu-fw))
