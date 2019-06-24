#
# Copyright (C) 2016 OpenWrt.org
# Copyright (C) 2017 LEDE project
# Copyright (C) 2019 Ycarus (Yannick Chabanois) ycarus@zugaina.org for OpenMPTCProuter project
#

ARCH:=aarch64
SUBTARGET:=bcm2711
BOARDNAME:=BCM2711 64 bit based boards
CPU_TYPE:=cortex-a72

define Target/Description
	Build firmware image for Broadcom BCM2711 64 bit SoC devices.
endef
