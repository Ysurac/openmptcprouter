
ARCH:=arm
SUBTARGET:=ipq60xx_32
BOARDNAME:=QTI IPQ60xx(32bit) based boards
CPU_TYPE:=cortex-a7

DEFAULT_PACKAGES += \
	uboot-2016-ipq6018 uboot-2016-ipq6018_tiny lk-ipq6018 \
	fwupgrade-tools

define Target/Description
	Build firmware image for IPQ60xx SoC devices.
endef
