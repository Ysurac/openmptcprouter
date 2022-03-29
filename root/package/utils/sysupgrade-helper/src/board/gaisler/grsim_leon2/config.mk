#
# (C) Copyright 2007
# Daniel Hellstrom, Gaisler Research, daniel@gaisler.com
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
# GRSIM simulating a LEON2 board
#

# RUN U-BOOT FROM PROM
CONFIG_SYS_TEXT_BASE = 0x00000000

# RUN U-BOOT FROM RAM
#CONFIG_SYS_TEXT_BASE = 0x40000000

PLATFORM_CPPFLAGS += -DCONFIG_SYS_TEXT_BASE=$(CONFIG_SYS_TEXT_BASE) \
	-I$(TOPDIR)/board
