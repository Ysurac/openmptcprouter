include $(TOPDIR)/rules.mk

PKG_NAME:=mt76
PKG_RELEASE:=1

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=

PKG_SOURCE_URL:=https://github.com/openwrt/mt76
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2025-11-12
PKG_SOURCE_VERSION:=f473d66ab595d5f37aedc868bfe407da0a1245c8
PKG_MIRROR_HASH:=skip

PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>
PKG_BUILD_PARALLEL:=1

STAMP_CONFIGURED_DEPENDS := $(STAGING_DIR)/usr/include/mac80211-backport/backport/autoconf.h

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/mt76-default
  SUBMENU:=Wireless Drivers
  DEPENDS:= \
	+kmod-mac80211 \
	+@DRIVER_11AC_SUPPORT +@DRIVER_11AX_SUPPORT +@DRIVER_11BE_SUPPORT \
	@PCI_SUPPORT||USB_SUPPORT||TARGET_ramips_mt7620||TARGET_ramips_mt76x8||TARGET_ramips_mt7621||TARGET_mediatek_filogic||TARGET_mediatek_mt7622||TARGET_mediatek_mt7623||TARGET_mediatek_mt7629
endef

define KernelPackage/mt76
  SUBMENU:=Wireless Drivers
  TITLE:=MediaTek MT76x2/MT7603 wireless driver (metapackage)
  DEPENDS:= \
	+kmod-mt76-core +kmod-mt76-usb +kmod-mt76x02-common +kmod-mt76x02-usb \
	+kmod-mt76x2 +kmod-mt76x2u +kmod-mt7603 +kmod-mt7615-common \
	+kmod-mt7615e +kmod-mt7663-usb-sdio +kmod-mt7663u \
	+kmod-mt7915e +kmod-mt7921-common +kmod-mt7921e +kmod-mt7921u \
	+kmod-mt7921s +kmod-mt7996e +kmod-mt792x-usb
endef

define KernelPackage/mt76/description
 MediaTek MT76x2/MT7603 wireless driver (metapackage)
endef

define KernelPackage/mt76-core
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT76xx wireless driver
  FILES:=$(PKG_BUILD_DIR)/mt76.ko
endef

define KernelPackage/mt76-usb
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT76xx wireless driver (USB support)
  DEPENDS += +kmod-usb-core +kmod-mt76-core
  FILES:=$(PKG_BUILD_DIR)/mt76-usb.ko
endef

define KernelPackage/mt76x02-usb
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT76x0/MT76x2 wireless driver (USB support)
  DEPENDS += +kmod-mt76-usb +kmod-mt76x02-common
  FILES:=$(PKG_BUILD_DIR)/mt76x02-usb.ko
endef

define KernelPackage/mt76x02-common
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT76x0/MT76x2 wireless driver (common code)
  DEPENDS += +kmod-mt76-core
  FILES:=$(PKG_BUILD_DIR)/mt76x02-lib.ko
endef

define KernelPackage/mt76x0-common
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT76x0 wireless driver (common code)
  DEPENDS += +kmod-mt76x02-common
  FILES:=$(PKG_BUILD_DIR)/mt76x0/mt76x0-common.ko
endef

define KernelPackage/mt76x0e
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT76x0E wireless driver
  DEPENDS += +kmod-mt76x0-common
  FILES:=$(PKG_BUILD_DIR)/mt76x0/mt76x0e.ko
  AUTOLOAD:=$(call AutoProbe,mt76x0e)
endef

define KernelPackage/mt76x0u
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT76x0U wireless driver
  DEPENDS += +kmod-mt76x0-common +kmod-mt76x02-usb
  FILES:=$(PKG_BUILD_DIR)/mt76x0/mt76x0u.ko
  AUTOLOAD:=$(call AutoProbe,mt76x0u)
endef

define KernelPackage/mt76x2-common
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT76x2 wireless driver (common code)
  DEPENDS += +kmod-mt76-core +kmod-mt76x02-common
  FILES:=$(PKG_BUILD_DIR)/mt76x2/mt76x2-common.ko
endef

define KernelPackage/mt76x2u
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT76x2U wireless driver
  DEPENDS += +kmod-mt76x2-common +kmod-mt76x02-usb
  FILES:=$(PKG_BUILD_DIR)/mt76x2/mt76x2u.ko
  AUTOLOAD:=$(call AutoProbe,mt76x2u)
endef

define KernelPackage/mt76x2
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT76x2 wireless driver
  DEPENDS += +kmod-mt76x2-common
  FILES:=$(PKG_BUILD_DIR)/mt76x2/mt76x2e.ko
  AUTOLOAD:=$(call AutoProbe,mt76x2e)
endef

define KernelPackage/mt7603
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7603 wireless driver
  DEPENDS += +kmod-mt76-core
  FILES:=$(PKG_BUILD_DIR)/mt7603/mt7603.ko
  AUTOLOAD:=$(call AutoProbe,mt7603)
endef

define KernelPackage/mt7615-common
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7615 wireless driver (common code)
  DEPENDS += +kmod-mt76-core
  FILES:=$(PKG_BUILD_DIR)/mt7615/mt7615-common.ko
endef

define KernelPackage/mt7615-firmware
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7615e firmware
  DEPENDS += +kmod-mt7615-common
endef

define KernelPackage/mt7615e
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7615e wireless driver
  DEPENDS += +kmod-mt7615-common
  FILES:=$(PKG_BUILD_DIR)/mt7615/mt7615e.ko
  AUTOLOAD:=$(call AutoProbe,mt7615e)
endef

define KernelPackage/mt7663-firmware-ap
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7663e firmware (AP)
  DEPENDS += +kmod-mt7615-common
endef

define KernelPackage/mt7663-firmware-sta
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7663e firmware (STA)
  DEPENDS += +kmod-mt7615-common
endef

define KernelPackage/mt7663-usb-sdio
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7663 USB/SDIO shared code
  DEPENDS += +kmod-mt7615-common
  FILES:=$(PKG_BUILD_DIR)/mt7615/mt7663-usb-sdio-common.ko
endef

define KernelPackage/mt7663u
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7663u wireless driver
  DEPENDS += +kmod-mt76-usb +kmod-mt7663-usb-sdio
  FILES:=$(PKG_BUILD_DIR)/mt7615/mt7663u.ko
  AUTOLOAD:=$(call AutoProbe,mt7663u)
endef

define KernelPackage/mt7663s
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7663s wireless driver
  DEPENDS += +kmod-mt7663-usb-sdio
  FILES:=$(PKG_BUILD_DIR)/mt7615/mt7663s.ko
  AUTOLOAD:=$(call AutoProbe,mt7663s)
endef

define KernelPackage/mt7915-firmware
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7915e firmware
endef

define KernelPackage/mt7915e
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7915e wireless driver
  DEPENDS += +kmod-mt76-core +kmod-mt7915-firmware +kmod-hwmon-core
  FILES:=$(PKG_BUILD_DIR)/mt7915/mt7915e.ko
  AUTOLOAD:=$(call AutoProbe,mt7915e)
endef

define KernelPackage/mt7921-common
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7921 wireless driver (common code)
  DEPENDS += +kmod-mt76-core
  FILES:=$(PKG_BUILD_DIR)/mt7921/mt7921-common.ko
endef

define KernelPackage/mt7921-firmware
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7921 firmware
  DEPENDS += +kmod-mt7921-common
endef

define KernelPackage/mt7921e
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7921e wireless driver
  DEPENDS += +kmod-mt7921-common
  FILES:=$(PKG_BUILD_DIR)/mt7921/mt7921e.ko
  AUTOLOAD:=$(call AutoProbe,mt7921e)
endef

define KernelPackage/mt7921s
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7921s wireless driver
  DEPENDS += +kmod-mt7921-common
  FILES:=$(PKG_BUILD_DIR)/mt7921/mt7921s.ko
  AUTOLOAD:=$(call AutoProbe,mt7921s)
endef

define KernelPackage/mt792x-usb
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT792x USB wireless driver
  DEPENDS += +kmod-mt76-usb +kmod-mt7921-common
  FILES:=$(PKG_BUILD_DIR)/mt7921/mt792x-usb.ko
endef

define KernelPackage/mt7921u
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7921u wireless driver
  DEPENDS += +kmod-mt792x-usb
  FILES:=$(PKG_BUILD_DIR)/mt7921/mt7921u.ko
  AUTOLOAD:=$(call AutoProbe,mt7921u)
endef

define KernelPackage/mt7925-common
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7925 wireless driver (common code)
  DEPENDS += +kmod-mt76-core
  FILES:=$(PKG_BUILD_DIR)/mt7925/mt7925-common.ko
endef

define KernelPackage/mt7925-firmware
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7925 firmware
  DEPENDS += +kmod-mt7925-common
endef

define KernelPackage/mt7925e
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7925e wireless driver
  DEPENDS += +kmod-mt7925-common
  FILES:=$(PKG_BUILD_DIR)/mt7925/mt7925e.ko
  AUTOLOAD:=$(call AutoProbe,mt7925e)
endef

define KernelPackage/mt7925u
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7925u wireless driver
  DEPENDS += +kmod-mt792x-usb +kmod-mt7925-common
  FILES:=$(PKG_BUILD_DIR)/mt7925/mt7925u.ko
  AUTOLOAD:=$(call AutoProbe,mt7925u)
endef

define KernelPackage/mt7996-firmware
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7996 firmware
endef

define KernelPackage/mt7996e
  $(KernelPackage/mt76-default)
  TITLE:=MediaTek MT7996e wireless driver
  DEPENDS += +kmod-mt76-core +kmod-mt7996-firmware +kmod-hwmon-core
  FILES:=$(PKG_BUILD_DIR)/mt7996/mt7996e.ko
  AUTOLOAD:=$(call AutoProbe,mt7996e)
endef

NOSTDINC_FLAGS += \
	$(KERNEL_NOSTDINC_FLAGS) \
	-I$(PKG_BUILD_DIR) \
	-I$(STAGING_DIR)/usr/include/mac80211-backport/uapi \
	-I$(STAGING_DIR)/usr/include/mac80211-backport \
	-I$(STAGING_DIR)/usr/include/mac80211/uapi \
	-I$(STAGING_DIR)/usr/include/mac80211 \
	-include backport/autoconf.h \
	-include backport/backport.h

ifdef CONFIG_PACKAGE_MAC80211_MESH
  NOSTDINC_FLAGS += -DCONFIG_MAC80211_MESH
endif

ifdef CONFIG_PACKAGE_MAC80211_DEBUGFS
  NOSTDINC_FLAGS += -DCONFIG_MAC80211_DEBUGFS
  PKG_MAKE_FLAGS += CONFIG_MAC80211_DEBUGFS=y
endif

ifdef CONFIG_PACKAGE_kmod-mt76-usb
  PKG_MAKE_FLAGS += CONFIG_MT76_USB=m
endif
ifdef CONFIG_PACKAGE_kmod-mt76x02-common
  PKG_MAKE_FLAGS += CONFIG_MT76x02_LIB=m
endif
ifdef CONFIG_PACKAGE_kmod-mt76x02-usb
  PKG_MAKE_FLAGS += CONFIG_MT76x02_USB=m
endif
ifdef CONFIG_PACKAGE_kmod-mt76x0-common
  PKG_MAKE_FLAGS += CONFIG_MT76x0_COMMON=m
endif
ifdef CONFIG_PACKAGE_kmod-mt76x0e
  PKG_MAKE_FLAGS += CONFIG_MT76x0E=m
endif
ifdef CONFIG_PACKAGE_kmod-mt76x0u
  PKG_MAKE_FLAGS += CONFIG_MT76x0U=m
endif
ifdef CONFIG_PACKAGE_kmod-mt76x2-common
  PKG_MAKE_FLAGS += CONFIG_MT76x2_COMMON=m
endif
ifdef CONFIG_PACKAGE_kmod-mt76x2
  PKG_MAKE_FLAGS += CONFIG_MT76x2E=m
endif
ifdef CONFIG_PACKAGE_kmod-mt76x2u
  PKG_MAKE_FLAGS += CONFIG_MT76x2U=m
endif
ifdef CONFIG_PACKAGE_kmod-mt7603
  PKG_MAKE_FLAGS += CONFIG_MT7603E=m
endif
ifdef CONFIG_PACKAGE_kmod-mt7615-common
  PKG_MAKE_FLAGS += CONFIG_MT7615_COMMON=m
endif
ifdef CONFIG_PACKAGE_kmod-mt7615e
  PKG_MAKE_FLAGS += CONFIG_MT7615E=m
endif
ifdef CONFIG_PACKAGE_kmod-mt7663-usb-sdio
  PKG_MAKE_FLAGS += CONFIG_MT7663_USB_SDIO_COMMON=m
endif
ifdef CONFIG_PACKAGE_kmod-mt7663u
  PKG_MAKE_FLAGS += CONFIG_MT7663U=m
endif
ifdef CONFIG_PACKAGE_kmod-mt7663s
  PKG_MAKE_FLAGS += CONFIG_MT7663S=m
endif
ifdef CONFIG_PACKAGE_kmod-mt7915e
  PKG_MAKE_FLAGS += CONFIG_MT7915E=m
endif
ifdef CONFIG_PACKAGE_kmod-mt7921-common
  PKG_MAKE_FLAGS += CONFIG_MT7921_COMMON=m
endif
ifdef CONFIG_PACKAGE_kmod-mt7921e
  PKG_MAKE_FLAGS += CONFIG_MT7921E=m
endif
ifdef CONFIG_PACKAGE_kmod-mt7921s
  PKG_MAKE_FLAGS += CONFIG_MT7921S=m
endif
ifdef CONFIG_PACKAGE_kmod-mt792x-usb
  PKG_MAKE_FLAGS += CONFIG_MT792x_USB=m
endif
ifdef CONFIG_PACKAGE_kmod-mt7921u
  PKG_MAKE_FLAGS += CONFIG_MT7921U=m
endif
ifdef CONFIG_PACKAGE_kmod-mt7925-common
  PKG_MAKE_FLAGS += CONFIG_MT7925_COMMON=m
endif
ifdef CONFIG_PACKAGE_kmod-mt7925e
  PKG_MAKE_FLAGS += CONFIG_MT7925E=m
endif
ifdef CONFIG_PACKAGE_kmod-mt7925u
  PKG_MAKE_FLAGS += CONFIG_MT7925U=m
endif
ifdef CONFIG_PACKAGE_kmod-mt7996e
  PKG_MAKE_FLAGS += CONFIG_MT7996E=m
endif

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C "$(LINUX_DIR)" \
		$(KERNEL_MAKE_FLAGS) \
		$(PKG_MAKE_FLAGS) \
		M="$(PKG_BUILD_DIR)" \
		NOSTDINC_FLAGS="$(NOSTDINC_FLAGS)" \
		modules
endef

define Package/mt7615-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/mediatek
	cp \
		$(PKG_BUILD_DIR)/firmware/mt7615_cr4.bin \
		$(PKG_BUILD_DIR)/firmware/mt7615_n9.bin \
		$(PKG_BUILD_DIR)/firmware/mt7615_rom_patch.bin \
		$(1)/lib/firmware/mediatek
endef

define Package/mt7915-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/mediatek/mt7915
	cp \
		$(PKG_BUILD_DIR)/firmware/mt7915_wa.bin \
		$(PKG_BUILD_DIR)/firmware/mt7915_wm.bin \
		$(PKG_BUILD_DIR)/firmware/mt7915_rom_patch.bin \
		$(1)/lib/firmware/mediatek/mt7915
endef

define Package/mt7921-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/mediatek
	cp \
		$(PKG_BUILD_DIR)/firmware/mt7921_wa.bin \
		$(PKG_BUILD_DIR)/firmware/mt7921_wm.bin \
		$(PKG_BUILD_DIR)/firmware/mt7921_rom_patch.bin \
		$(1)/lib/firmware/mediatek
endef

define Package/mt7925-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/mediatek/mt7925
	cp \
		$(PKG_BUILD_DIR)/firmware/mt7925_wa.bin \
		$(PKG_BUILD_DIR)/firmware/mt7925_wm.bin \
		$(PKG_BUILD_DIR)/firmware/mt7925_rom_patch.bin \
		$(1)/lib/firmware/mediatek/mt7925
endef

define Package/mt7996-firmware/install
	$(INSTALL_DIR) $(1)/lib/firmware/mediatek/mt7996
	cp \
		$(PKG_BUILD_DIR)/firmware/mt7996_wa.bin \
		$(PKG_BUILD_DIR)/firmware/mt7996_wm.bin \
		$(PKG_BUILD_DIR)/firmware/mt7996_rom_patch.bin \
		$(PKG_BUILD_DIR)/firmware/mt7996_dsp.bin \
		$(1)/lib/firmware/mediatek/mt7996
endef

define Package/mt7663-firmware-ap/install
	$(INSTALL_DIR) $(1)/lib/firmware/mediatek
	ln -sf mt7615_n9.bin $(1)/lib/firmware/mediatek/mt7663_n9_v3.bin
	ln -sf mt7615_cr4.bin $(1)/lib/firmware/mediatek/mt7663pr2h.bin
	ln -sf mt7615_rom_patch.bin $(1)/lib/firmware/mediatek/mt7663pr2h_rebb.bin
endef

define Package/mt7663-firmware-sta/install
	$(INSTALL_DIR) $(1)/lib/firmware/mediatek
	ln -sf mt7615_n9.bin $(1)/lib/firmware/mediatek/mt7663_n9_rebb.bin
	ln -sf mt7615_cr4.bin $(1)/lib/firmware/mediatek/mt7663pr2h_rebb.bin
	ln -sf mt7615_rom_patch.bin $(1)/lib/firmware/mediatek/mt7663_rom_patch.bin
endef

$(eval $(call KernelPackage,mt76-core))
$(eval $(call KernelPackage,mt76-usb))
$(eval $(call KernelPackage,mt76x02-usb))
$(eval $(call KernelPackage,mt76x02-common))
$(eval $(call KernelPackage,mt76x0-common))
$(eval $(call KernelPackage,mt76x0e))
$(eval $(call KernelPackage,mt76x0u))
$(eval $(call KernelPackage,mt76x2-common))
$(eval $(call KernelPackage,mt76x2u))
$(eval $(call KernelPackage,mt76x2))
$(eval $(call KernelPackage,mt7603))
$(eval $(call KernelPackage,mt7615-common))
$(eval $(call KernelPackage,mt7615-firmware))
$(eval $(call KernelPackage,mt7615e))
$(eval $(call KernelPackage,mt7663-firmware-ap))
$(eval $(call KernelPackage,mt7663-firmware-sta))
$(eval $(call KernelPackage,mt7663-usb-sdio))
$(eval $(call KernelPackage,mt7663u))
$(eval $(call KernelPackage,mt7663s))
$(eval $(call KernelPackage,mt7915-firmware))
$(eval $(call KernelPackage,mt7915e))
$(eval $(call KernelPackage,mt7921-common))
$(eval $(call KernelPackage,mt7921-firmware))
$(eval $(call KernelPackage,mt7921e))
$(eval $(call KernelPackage,mt7921s))
$(eval $(call KernelPackage,mt792x-usb))
$(eval $(call KernelPackage,mt7921u))
$(eval $(call KernelPackage,mt7925-common))
$(eval $(call KernelPackage,mt7925-firmware))
$(eval $(call KernelPackage,mt7925e))
$(eval $(call KernelPackage,mt7925u))
$(eval $(call KernelPackage,mt7996-firmware))
$(eval $(call KernelPackage,mt7996e))
$(eval $(call KernelPackage,mt76))
