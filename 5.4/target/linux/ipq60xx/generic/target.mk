
SUBTARGET:=generic
BOARDNAME:=QTI IPQ60xx(64bit) based boards
CPU_TYPE:=cortex-a53
KERNELNAME:=Image dtbs

DEFAULT_PACKAGES += \
	sysupgrade-helper

define Target/Description
	Build firmware image for IPQ60xx 64 bit system.
endef
