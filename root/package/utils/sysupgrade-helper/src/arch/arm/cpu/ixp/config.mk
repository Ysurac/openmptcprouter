#
# (C) Copyright 2002
# Sysgo Real-Time Solutions, GmbH <www.elinos.com>
# Marius Groeger <mgroeger@sysgo.de>
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

BIG_ENDIAN = y

PLATFORM_RELFLAGS += -fno-common -ffixed-r8 -msoft-float -mbig-endian

PLATFORM_CPPFLAGS += -mbig-endian -march=armv5te -mtune=strongarm1100

PLATFORM_LDFLAGS += -EB
USE_PRIVATE_LIBGCC = yes

# -fdata-sections triggers "section .bss overlaps section .rel.dyn" linker error
PLATFORM_RELFLAGS += -ffunction-sections
LDFLAGS_u-boot += --gc-sections

# =========================================================================
#
# Supply options according to compiler version
#
# =========================================================================
PF_RELFLAGS_SLB_AT := $(call cc-option,-mshort-load-bytes,$(call cc-option,-malignment-traps,))
PLATFORM_RELFLAGS += $(PF_RELFLAGS_SLB_AT)
