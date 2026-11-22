# Teltonika RUTM models missing upstream, included from mt7621.mk by build.sh.
# They reuse the upstream Device/teltonika_rutm_common: factory.bin can be
# flashed from the Teltonika web interface.

RUTM_MODEM_PACKAGES := kmod-usb-net-qmi-wwan kmod-usb-net-cdc-mbim kmod-usb-net-cdc-ncm

define Device/teltonika_rutm-nowifi
  $(Device/teltonika_rutm_common)
  DEVICE_PACKAGES := $$(filter-out kmod-mt7615-firmware,$$(DEVICE_PACKAGES)) -wpad-basic-mbedtls
endef

define Device/teltonika_rutm08
  $(Device/teltonika_rutm-nowifi)
  DEVICE_MODEL := RUTM08
endef
TARGET_DEVICES += teltonika_rutm08

define Device/teltonika_rutm09
  $(Device/teltonika_rutm-nowifi)
  DEVICE_MODEL := RUTM09
  DEVICE_PACKAGES += $(RUTM_MODEM_PACKAGES)
endef
TARGET_DEVICES += teltonika_rutm09

define Device/teltonika_rutm10
  $(Device/teltonika_rutm_common)
  DEVICE_MODEL := RUTM10
endef
TARGET_DEVICES += teltonika_rutm10

define Device/teltonika_rutm12
  $(Device/teltonika_rutm_common)
  DEVICE_MODEL := RUTM12
  DEVICE_PACKAGES += $(RUTM_MODEM_PACKAGES)
endef
TARGET_DEVICES += teltonika_rutm12

define Device/teltonika_rutm13
  $(Device/teltonika_rutm-nowifi)
  DEVICE_MODEL := RUTM13
  DEVICE_PACKAGES += $(RUTM_MODEM_PACKAGES)
endef
TARGET_DEVICES += teltonika_rutm13

define Device/teltonika_rutm16
  $(Device/teltonika_rutm_common)
  DEVICE_MODEL := RUTM16
  DEVICE_PACKAGES += $(RUTM_MODEM_PACKAGES)
endef
TARGET_DEVICES += teltonika_rutm16

define Device/teltonika_rutm20
  $(Device/teltonika_rutm_common)
  DEVICE_MODEL := RUTM20
  DEVICE_PACKAGES += $(RUTM_MODEM_PACKAGES)
endef
TARGET_DEVICES += teltonika_rutm20

# Current RUTM51 boards, the upstream RUTM50 image only fits the first ones
define Device/teltonika_rutm51
  $(Device/teltonika_rutm_common)
  DEVICE_MODEL := RUTM51
  DEVICE_PACKAGES += $(RUTM_MODEM_PACKAGES)
endef
TARGET_DEVICES += teltonika_rutm51

define Device/teltonika_rutm52
  $(Device/teltonika_rutm_common)
  DEVICE_MODEL := RUTM52
  DEVICE_PACKAGES += $(RUTM_MODEM_PACKAGES)
  SUPPORTED_TELTONIKA_HW_MODS += RUTM52_TUSB8020B
endef
TARGET_DEVICES += teltonika_rutm52

define Device/teltonika_rutm54
  $(Device/teltonika_rutm_common)
  DEVICE_MODEL := RUTM54
  DEVICE_PACKAGES += $(RUTM_MODEM_PACKAGES)
endef
TARGET_DEVICES += teltonika_rutm54

# Hardware version 4.00 and later
define Device/teltonika_rutm54-v4
  $(Device/teltonika_rutm54)
  DEVICE_VARIANT := v4
endef
TARGET_DEVICES += teltonika_rutm54-v4

define Device/teltonika_rutm55
  $(Device/teltonika_rutm-nowifi)
  DEVICE_MODEL := RUTM55
  DEVICE_PACKAGES += $(RUTM_MODEM_PACKAGES)
endef
TARGET_DEVICES += teltonika_rutm55

define Device/teltonika_rutm56
  $(Device/teltonika_rutm_common)
  DEVICE_MODEL := RUTM56
  DEVICE_PACKAGES += $(RUTM_MODEM_PACKAGES)
endef
TARGET_DEVICES += teltonika_rutm56

define Device/teltonika_rutm59
  $(Device/teltonika_rutm-nowifi)
  DEVICE_MODEL := RUTM59
  DEVICE_PACKAGES += $(RUTM_MODEM_PACKAGES)
endef
TARGET_DEVICES += teltonika_rutm59
