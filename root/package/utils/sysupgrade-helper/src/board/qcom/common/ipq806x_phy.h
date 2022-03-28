/*
 * Copyright (c) 2013 The Linux Foundation. All rights reserved.
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
#ifndef _IPQ806X_PHY_H
#define _IPQ806X_PHY_H

#include <config.h>

extern u16 ATH_GMAC_read_phy_reg(u32 *RegBase, u32 PhyBase, u32 RegOffset);
extern s32 GMAC_write_phy_reg(u32 *RegBase, u32 PhyBase, u32 RegOffset, u16 *data);

#define phy_reg_read(base, addr, reg)  \
	ATH_GMAC_read_phy_reg(0x37000000, addr, reg)

#define phy_reg_write(base, addr, reg, data)   \
	GMAC_write_phy_reg(0x37000000, addr, reg, data)

#endif

