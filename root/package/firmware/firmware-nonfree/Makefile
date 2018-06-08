include $(TOPDIR)/rules.mk

PKG_NAME:=firmware-nonfree
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/RPi-Distro/firmware-nonfree.git
PKG_SOURCE_DATE:=2018-05-27
PKG_SOURCE_VERSION:=86e88fbf0345da49555d0ec34c80b4fbae7d0cd3
PKG_MIRROR_HASH:=bef9efe173b87e7afc37bb856f9727c4ff64a1ee1cbe6640d22bae79601553bb

SCAN_DEPS = *.mk

include $(INCLUDE_DIR)/package.mk

define Package/firmware-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=https://github.com/RPi-Distro/firmware-nonfree.git
  TITLE:=$(1)
endef

define Build/Compile

endef

include $(wildcard ./*.mk)
#$(eval $(call BuildPackage,firmware-nonfree))
