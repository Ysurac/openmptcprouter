#
# (C) Copyright 2000-2002
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

CROSS_COMPILE ?= bfin-uclinux-

CONFIG_STANDALONE_LOAD_ADDR ?= 0x1000 -m elf32bfin

ifeq ($(CONFIG_BFIN_CPU),)
CONFIG_BFIN_CPU := \
	$(shell awk '$$2 == "CONFIG_BFIN_CPU" { print $$3 }' \
		$(src)include/configs/$(BOARD).h)
else
CONFIG_BFIN_CPU := $(strip $(subst ",,$(CONFIG_BFIN_CPU)))
endif
CONFIG_BFIN_BOOT_MODE := $(strip $(subst ",,$(CONFIG_BFIN_BOOT_MODE)))

PLATFORM_RELFLAGS += -ffixed-P3 -fomit-frame-pointer -mno-fdpic
PLATFORM_CPPFLAGS += -DCONFIG_BLACKFIN

LDFLAGS_FINAL += --gc-sections
LDFLAGS += -m elf32bfin
PLATFORM_RELFLAGS += -ffunction-sections -fdata-sections

PLATFORM_RELFLAGS += -mcpu=$(CONFIG_BFIN_CPU)

ifneq ($(CONFIG_BFIN_BOOT_MODE),BFIN_BOOT_BYPASS)
ALL-y += $(obj)u-boot.ldr
endif
ifeq ($(CONFIG_ENV_IS_EMBEDDED_IN_LDR),y)
CREATE_LDR_ENV = $(obj)tools/envcrc --binary > $(obj)env-ldr.o
HOSTCFLAGS_NOPED_ADSP := \
	$(shell $(CPP) -dD - -mcpu=$(CONFIG_BFIN_CPU) </dev/null \
		| awk '$$2 ~ /ADSP/ { print "-D" $$2 }')
HOSTCFLAGS_NOPED += $(HOSTCFLAGS_NOPED_ADSP)
else
CREATE_LDR_ENV =
endif

SYM_PREFIX = _

LDR_FLAGS-y :=
LDR_FLAGS-$(CONFIG_BFIN_BOOTROM_USES_EVT1) += -J

LDR_FLAGS += --bmode $(subst BFIN_BOOT_,,$(CONFIG_BFIN_BOOT_MODE))
LDR_FLAGS += --use-vmas
LDR_FLAGS += --initcode $(obj)$(CPUDIR)/initcode.o
ifneq ($(CONFIG_BFIN_BOOT_MODE),BFIN_BOOT_UART)
LDR_FLAGS-$(CONFIG_ENV_IS_EMBEDDED_IN_LDR) += \
	--punchit $$(($(CONFIG_ENV_OFFSET))):$$(($(CONFIG_ENV_SIZE))):$(obj)env-ldr.o
endif
ifneq (,$(findstring s,$(MAKEFLAGS)))
LDR_FLAGS += --quiet
endif

LDR_FLAGS += $(LDR_FLAGS-y)

# Set some default LDR flags based on boot mode.
LDR_FLAGS += $(LDR_FLAGS-$(CONFIG_BFIN_BOOT_MODE))

ifneq ($(CONFIG_SYS_TEXT_BASE),)
$(error do not set CONFIG_SYS_TEXT_BASE for Blackfin boards)
endif
