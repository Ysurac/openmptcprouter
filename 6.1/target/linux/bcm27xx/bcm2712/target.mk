#
# Copyright (C) 2019 OpenWrt.org
# Copyright (C) 2023 Yannick Chabanois (Ycarus) for OpenMPTCProuter
#

ARCH:=aarch64
SUBTARGET:=bcm2712
BOARDNAME:=BCM2712 boards (64 bit)
CPU_TYPE:=cortex-a76

define Target/Description
	Build firmware image for BCM2712 devices.
	This firmware features a 64 bit kernel.
endef
