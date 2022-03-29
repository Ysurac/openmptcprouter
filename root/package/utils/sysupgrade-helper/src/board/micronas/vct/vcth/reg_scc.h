/*
 * (C) Copyright 2009 Stefan Roese <sr@denx.de>, DENX Software Engineering
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

#ifndef _REG_SCC_PREMIUM_H_
#define _REG_SCC_PREMIUM_H_

#define SCC0_BASE		0x00110000
#define SCC1_BASE		0x00110080
#define SCC2_BASE		0x00110100
#define SCC3_BASE		0x00110180
#define SCC4_BASE		0x00110200
#define SCC5_BASE		0x00110280
#define SCC6_BASE		0x00110300
#define SCC7_BASE		0x00110380
#define SCC8_BASE		0x00110400
#define SCC9_BASE		0x00110480
#define SCC10_BASE		0x00110500
#define SCC11_BASE		0x00110580
#define SCC12_BASE		0x00110600
#define SCC13_BASE		0x00110680
#define SCC14_BASE		0x00110700
#define SCC15_BASE		0x00110780
#define SCC16_BASE		0x00110800
#define SCC17_BASE		0x00110880
#define SCC18_BASE		0x00110900
#define SCC19_BASE		0x00110980
#define SCC20_BASE		0x00110a00
#define SCC21_BASE		0x00110a80
#define SCC22_BASE		0x00110b00
#define SCC23_BASE		0x00110b80
#define SCC24_BASE		0x00110c00
#define SCC25_BASE		0x00110c80
#define SCC26_BASE		0x00110d00
#define SCC27_BASE		0x00110d80
#define SCC28_BASE		0x00110e00
#define SCC29_BASE		0x00110e80
#define SCC30_BASE		0x00110f00
#define SCC31_BASE		0x00110f80
#define SCC32_BASE		0x00111000
#define SCC33_BASE		0x00111080
#define SCC34_BASE		0x00111100
#define SCC35_BASE		0x00111180
#define SCC36_BASE		0x00111200
#define SCC37_BASE		0x00111280
#define SCC38_BASE		0x00111300
#define SCC39_BASE		0x00111380
#define SCC40_BASE		0x00111400

/*  Relative offsets of the register adresses */

#define SCC_ENABLE_OFFS		0x00000000
#define SCC_ENABLE(base)	((base) + SCC_ENABLE_OFFS)
#define SCC_RESET_OFFS		0x00000004
#define SCC_RESET(base)		((base) + SCC_RESET_OFFS)
#define SCC_VCID_OFFS		0x00000008
#define SCC_VCID(base)		((base) + SCC_VCID_OFFS)
#define SCC_MCI_CFG_OFFS	0x0000000C
#define SCC_MCI_CFG(base)	((base) + SCC_MCI_CFG_OFFS)
#define SCC_PACKET_CFG1_OFFS	0x00000010
#define SCC_PACKET_CFG1(base)	((base) + SCC_PACKET_CFG1_OFFS)
#define SCC_PACKET_CFG2_OFFS	0x00000014
#define SCC_PACKET_CFG2(base)	((base) + SCC_PACKET_CFG2_OFFS)
#define SCC_PACKET_CFG3_OFFS	0x00000018
#define SCC_PACKET_CFG3(base)	((base) + SCC_PACKET_CFG3_OFFS)
#define SCC_DMA_CFG_OFFS	0x0000001C
#define SCC_DMA_CFG(base)	((base) + SCC_DMA_CFG_OFFS)
#define SCC_CMD_OFFS		0x00000020
#define SCC_CMD(base)		((base) + SCC_CMD_OFFS)
#define SCC_PRIO_OFFS		0x00000024
#define SCC_PRIO(base)		((base) + SCC_PRIO_OFFS)
#define SCC_DEBUG_OFFS		0x00000028
#define SCC_DEBUG(base)		((base) + SCC_DEBUG_OFFS)
#define SCC_STATUS_OFFS		0x0000002C
#define SCC_STATUS(base)	((base) + SCC_STATUS_OFFS)
#define SCC_IMR_OFFS		0x00000030
#define SCC_IMR(base)		((base) + SCC_IMR_OFFS)
#define SCC_ISR_OFFS		0x00000034
#define SCC_ISR(base)		((base) + SCC_ISR_OFFS)
#define SCC_DMA_OFFSET_OFFS	0x00000038
#define SCC_DMA_OFFSET(base)	((base) + SCC_DMA_OFFSET_OFFS)
#define SCC_RS_CTLSTS_OFFS	0x0000003C
#define SCC_RS_CTLSTS(base)	((base) + SCC_RS_CTLSTS_OFFS)

#endif
