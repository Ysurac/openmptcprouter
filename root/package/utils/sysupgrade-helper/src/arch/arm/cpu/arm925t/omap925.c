/*
 * (C) Copyright 2003
 * Texas Instruments <www.ti.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <arm925t.h>

#define MIF_CONFIG_REG 0xFFFECC0C
#define FLASH_GLOBAL_CTRL_NWP 1

void archflashwp (void *archdata, int wp)
{
	ulong *fgc = (ulong *) MIF_CONFIG_REG;

	if (wp == 1)
		*fgc &= ~FLASH_GLOBAL_CTRL_NWP;
	else
		*fgc |= FLASH_GLOBAL_CTRL_NWP;
}
