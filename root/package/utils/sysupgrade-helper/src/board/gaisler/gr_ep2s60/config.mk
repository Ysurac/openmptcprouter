#
# (C) Copyright 2008
# Daniel Hellstrom, Gaisler Research, daniel@gaisler.com.
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

#
# Altera NIOS delopment board Stratix II edition, FPGA device EP2S60,
# with GRLIB Template design (GPL Open Source SPARC/LEON3)
#

# U-BOOT IN FLASH
CONFIG_SYS_TEXT_BASE = 0x00000000

# U-BOOT IN SDRAM
#CONFIG_SYS_TEXT_BASE = 0x40000000

PLATFORM_CPPFLAGS += -DCONFIG_SYS_TEXT_BASE=$(CONFIG_SYS_TEXT_BASE) \
	-I$(TOPDIR)/board
