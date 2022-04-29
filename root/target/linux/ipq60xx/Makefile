include $(TOPDIR)/rules.mk

ARCH:=aarch64
BOARD:=ipq60xx
BOARDNAME:=Qualcomm Atheros IPQ60xx
FEATURES:=squashfs fpu ramdisk nand
CPU_TYPE:=cortex-a53
SUBTARGETS:=generic

KERNEL_PATCHVER:=5.4

KERNELNAME:=Image dtbs

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += \
	kmod-leds-gpio kmod-gpio-button-hotplug swconfig \
	kmod-qca-nss-dp kmod-qca-nss-drv kmod-qca-ssdk \
	kmod-ath11k-ahb qca-ssdk-shell wpad-openssl \
	ath11k-firmware-ipq6018 nss-firmware-ipq6018 \
	kmod-qca-nss-drv-pppoe uboot-envtools

$(eval $(call BuildTarget))
