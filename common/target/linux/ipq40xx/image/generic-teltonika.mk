# Teltonika RUTX (DSA), included from image/Makefile by build.sh.
# factory.ubi can be flashed from the Teltonika web interface.

define Device/teltonika_rutx-common
	$(call Device/FitImage)
	$(call Device/UbiFit)
	DEVICE_VENDOR := Teltonika
	SOC := qcom-ipq4018
	DEVICE_DTS_CONFIG := config@5
	KERNEL_INSTALL := 1
	BLOCKSIZE := 128k
	PAGESIZE := 2048
	FILESYSTEMS := squashfs
	SUPPORTED_TELTONIKA_DEVICES := teltonika,rutx
	SUPPORTED_TELTONIKA_HW_MODS := W25N02KV NAND_GD5F2GXX EG060K RUTX_V12
	IMAGE/factory.ubi := append-ubi | qsdk-ipq-factory-nand | append-teltonika-metadata
	DEVICE_PACKAGES := kmod-gpio-nxp-74hc164 kmod-spi-gpio
endef

define Device/teltonika_rutx-modem
	$(call Device/teltonika_rutx-common)
	DEVICE_PACKAGES += kmod-usb-net-qmi-wwan kmod-usb-serial-option uqmi
endef

define Device/teltonika_rutx08
	$(call Device/teltonika_rutx-common)
	DEVICE_MODEL := RUTX08
endef
TARGET_DEVICES += teltonika_rutx08

define Device/teltonika_rutx09
	$(call Device/teltonika_rutx-modem)
	DEVICE_MODEL := RUTX09
endef
TARGET_DEVICES += teltonika_rutx09

define Device/teltonika_rutx09-stm32
	$(call Device/teltonika_rutx09)
	DEVICE_VARIANT := STM32
	DEVICE_PACKAGES += kmod-r2ec
endef
TARGET_DEVICES += teltonika_rutx09-stm32

# Replaces the upstream definition (RUTX10 was not built upstream: no DSA)
define Device/teltonika_rutx10
	$(call Device/teltonika_rutx-common)
	DEVICE_MODEL := RUTX10
	DEVICE_PACKAGES += kmod-btusb
endef
TARGET_DEVICES += teltonika_rutx10

define Device/teltonika_rutx10-stm32
	$(call Device/teltonika_rutx10)
	DEVICE_VARIANT := STM32
	DEVICE_PACKAGES += kmod-r2ec
endef
TARGET_DEVICES += teltonika_rutx10-stm32

define Device/teltonika_rutx11
	$(call Device/teltonika_rutx-modem)
	DEVICE_MODEL := RUTX11
	DEVICE_PACKAGES += kmod-btusb
endef
TARGET_DEVICES += teltonika_rutx11

define Device/teltonika_rutx11-stm32
	$(call Device/teltonika_rutx11)
	DEVICE_VARIANT := STM32
	DEVICE_PACKAGES += kmod-r2ec
endef
TARGET_DEVICES += teltonika_rutx11-stm32

define Device/teltonika_rutx12
	$(call Device/teltonika_rutx-modem)
	DEVICE_MODEL := RUTX12
	DEVICE_PACKAGES += kmod-btusb
endef
TARGET_DEVICES += teltonika_rutx12

define Device/teltonika_rutx12-stm32
	$(call Device/teltonika_rutx12)
	DEVICE_VARIANT := STM32
	DEVICE_PACKAGES += kmod-r2ec
endef
TARGET_DEVICES += teltonika_rutx12-stm32

define Device/teltonika_rutx14
	$(call Device/teltonika_rutx-modem)
	DEVICE_MODEL := RUTX14
endef
TARGET_DEVICES += teltonika_rutx14

# Hardware version 4.00 and later, older boards use the upstream teltonika_rutx50
define Device/teltonika_rutx50-v4
	$(call Device/teltonika_rutx-modem)
	DEVICE_MODEL := RUTX50
	DEVICE_VARIANT := v4
endef
TARGET_DEVICES += teltonika_rutx50-v4

define Device/teltonika_rutx14-stm32
	$(call Device/teltonika_rutx14)
	DEVICE_VARIANT := STM32
	DEVICE_PACKAGES += kmod-r2ec
endef
TARGET_DEVICES += teltonika_rutx14-stm32
