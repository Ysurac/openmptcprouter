# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2024 OpenWrt.org

define KernelPackage/crypto-hw-rockchip
    SUBMENU:=$(CRYPTO_MENU)
    TITLE:=Rockchip Cryptographic Engine driver
    DEPENDS:= \
	@LINUX_6_6 \
	@TARGET_rockchip \
	+kmod-crypto-cbc \
	+kmod-crypto-des \
	+kmod-crypto-ecb \
	+kmod-crypto-hash \
	+kmod-crypto-kpp \
	+kmod-crypto-md5 \
	+kmod-crypto-rsa \
	+kmod-crypto-sha1 \
	+kmod-crypto-sha256
    KCONFIG:= \
	CONFIG_CRYPTO_AES \
	CONFIG_CRYPTO_ENGINE \
	CONFIG_CRYPTO_HW=y \
	CONFIG_CRYPTO_DEV_ROCKCHIP \
	CONFIG_CRYPTO_DEV_ROCKCHIP_DEBUG=n \
	CONFIG_CRYPTO_LIB_DES \
	CRYPTO_SKCIPHER
    FILES:= \
	$(LINUX_DIR)/crypto/crypto_engine.ko \
	$(LINUX_DIR)/drivers/crypto/rockchip/rk_crypto.ko
    AUTOLOAD:=$(call AutoLoad,09,crypto_engine rk_crypto)
    $(call AddDepends/crypto)
endef

define KernelPackage/crypto-hw-rockchip/description
 This driver interfaces with the hardware crypto accelerator.
 Supporting cbc/ecb chainmode, and aes/des/des3_ede cipher mode.
endef

$(eval $(call KernelPackage,crypto-hw-rockchip))
