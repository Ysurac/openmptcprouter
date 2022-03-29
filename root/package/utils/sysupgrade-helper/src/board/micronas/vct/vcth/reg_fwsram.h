/*
 * (C) Copyright 2008 Stefan Roese <sr@denx.de>, DENX Software Engineering
 *
 * Copyright (C) 2006 Micronas GmbH
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

/*
 * Premium & Platinum register addresses/definitions seem to be
 * identical, so we only need to use one file for both platforms.
 */

#ifndef _REG_FWSRAM_H_
#define _REG_FWSRAM_H_

#define FWSRAM_BASE			0x00030000

/*  Relative offsets of the register adresses */

#define FWSRAM_SR_ADDR_OFFSET_OFFS	0x00002000
#define FWSRAM_SR_ADDR_OFFSET(base)	((base) + FWSRAM_SR_ADDR_OFFSET_OFFS)
#define FWSRAM_TOP_BOOT_LOG_OFFS	0x00002004
#define FWSRAM_TOP_BOOT_LOG(base)	((base) + FWSRAM_TOP_BOOT_LOG_OFFS)
#define FWSRAM_TOP_ROM_KBIST_OFFS	0x00002008
#define FWSRAM_TOP_ROM_KBIST(base)	((base) + FWSRAM_TOP_ROM_KBIST_OFFS)
#define FWSRAM_TOP_CID1_H_OFFS		0x0000200C
#define FWSRAM_TOP_CID1_H(base)		((base) + FWSRAM_TOP_CID1_H_OFFS)
#define FWSRAM_TOP_CID1_L_OFFS		0x00002010
#define FWSRAM_TOP_CID1_L(base)		((base) + FWSRAM_TOP_CID1_L_OFFS)
#define FWSRAM_TOP_CID2_H_OFFS		0x00002014
#define FWSRAM_TOP_CID2_H(base)		((base) + FWSRAM_TOP_CID2_H_OFFS)
#define FWSRAM_TOP_CID2_L_OFFS		0x00002018
#define FWSRAM_TOP_CID2_L(base)		((base) + FWSRAM_TOP_CID2_L_OFFS)
#define FWSRAM_TOP_TDO_CFG_OFFS		0x0000203C
#define FWSRAM_TOP_TDO_CFG(base)	((base) + FWSRAM_TOP_TDO_CFG_OFFS)
#define FWSRAM_TOP_GPIO2_0_CFG_OFFS	0x00002040
#define FWSRAM_TOP_GPIO2_0_CFG(base)	((base) + FWSRAM_TOP_GPIO2_0_CFG_OFFS)
#define FWSRAM_TOP_GPIO2_1_CFG_OFFS	0x00002044
#define FWSRAM_TOP_GPIO2_1_CFG(base)	((base) + FWSRAM_TOP_GPIO2_1_CFG_OFFS)
#define FWSRAM_TOP_GPIO2_2_CFG_OFFS	0x00002048
#define FWSRAM_TOP_GPIO2_2_CFG(base)	((base) + FWSRAM_TOP_GPIO2_2_CFG_OFFS)
#define FWSRAM_TOP_GPIO2_3_CFG_OFFS	0x0000204C
#define FWSRAM_TOP_GPIO2_3_CFG(base)	((base) + FWSRAM_TOP_GPIO2_3_CFG_OFFS)
#define FWSRAM_TOP_GPIO2_4_CFG_OFFS	0x00002050
#define FWSRAM_TOP_GPIO2_4_CFG(base)	((base) + FWSRAM_TOP_GPIO2_4_CFG_OFFS)
#define FWSRAM_TOP_GPIO2_5_CFG_OFFS	0x00002054
#define FWSRAM_TOP_GPIO2_5_CFG(base)	((base) + FWSRAM_TOP_GPIO2_5_CFG_OFFS)
#define FWSRAM_TOP_GPIO2_6_CFG_OFFS	0x00002058
#define FWSRAM_TOP_GPIO2_6_CFG(base)	((base) + FWSRAM_TOP_GPIO2_6_CFG_OFFS)
#define FWSRAM_TOP_GPIO2_7_CFG_OFFS	0x0000205C
#define FWSRAM_TOP_GPIO2_7_CFG(base)	((base) + FWSRAM_TOP_GPIO2_7_CFG_OFFS)
#define FWSRAM_TOP_SCL_CFG_OFFS		0x00002060
#define FWSRAM_TOP_SCL_CFG(base)	((base) + FWSRAM_TOP_SCL_CFG_OFFS)
#define FWSRAM_TOP_SDA_CFG_OFFS		0x00002064
#define FWSRAM_TOP_SDA_CFG(base)	((base) + FWSRAM_TOP_SDA_CFG_OFFS)
#define FWSRAM_NO_MCM_FLASH_OFFS	0x00002068
#define FWSRAM_NO_MCM_FLASH(base)	((base) + FWSRAM_NO_MCM_FLASH_OFFS)

#endif
