/*
 * Copyright (C) 2005-2006 Atmel Corporation
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
#ifndef __DRIVERS_MACB_H__
#define __DRIVERS_MACB_H__

/* MACB register offsets */
#define MACB_NCR				0x0000
#define MACB_NCFGR				0x0004
#define MACB_NSR				0x0008
#define MACB_TSR				0x0014
#define MACB_RBQP				0x0018
#define MACB_TBQP				0x001c
#define MACB_RSR				0x0020
#define MACB_ISR				0x0024
#define MACB_IER				0x0028
#define MACB_IDR				0x002c
#define MACB_IMR				0x0030
#define MACB_MAN				0x0034
#define MACB_PTR				0x0038
#define MACB_PFR				0x003c
#define MACB_FTO				0x0040
#define MACB_SCF				0x0044
#define MACB_MCF				0x0048
#define MACB_FRO				0x004c
#define MACB_FCSE				0x0050
#define MACB_ALE				0x0054
#define MACB_DTF				0x0058
#define MACB_LCOL				0x005c
#define MACB_EXCOL				0x0060
#define MACB_TUND				0x0064
#define MACB_CSE				0x0068
#define MACB_RRE				0x006c
#define MACB_ROVR				0x0070
#define MACB_RSE				0x0074
#define MACB_ELE				0x0078
#define MACB_RJA				0x007c
#define MACB_USF				0x0080
#define MACB_STE				0x0084
#define MACB_RLE				0x0088
#define MACB_TPF				0x008c
#define MACB_HRB				0x0090
#define MACB_HRT				0x0094
#define MACB_SA1B				0x0098
#define MACB_SA1T				0x009c
#define MACB_SA2B				0x00a0
#define MACB_SA2T				0x00a4
#define MACB_SA3B				0x00a8
#define MACB_SA3T				0x00ac
#define MACB_SA4B				0x00b0
#define MACB_SA4T				0x00b4
#define MACB_TID				0x00b8
#define MACB_TPQ				0x00bc
#define MACB_USRIO				0x00c0
#define MACB_WOL				0x00c4

/* Bitfields in NCR */
#define MACB_LB_OFFSET				0
#define MACB_LB_SIZE				1
#define MACB_LLB_OFFSET				1
#define MACB_LLB_SIZE				1
#define MACB_RE_OFFSET				2
#define MACB_RE_SIZE				1
#define MACB_TE_OFFSET				3
#define MACB_TE_SIZE				1
#define MACB_MPE_OFFSET				4
#define MACB_MPE_SIZE				1
#define MACB_CLRSTAT_OFFSET			5
#define MACB_CLRSTAT_SIZE			1
#define MACB_INCSTAT_OFFSET			6
#define MACB_INCSTAT_SIZE			1
#define MACB_WESTAT_OFFSET			7
#define MACB_WESTAT_SIZE			1
#define MACB_BP_OFFSET				8
#define MACB_BP_SIZE				1
#define MACB_TSTART_OFFSET			9
#define MACB_TSTART_SIZE			1
#define MACB_THALT_OFFSET			10
#define MACB_THALT_SIZE				1
#define MACB_NCR_TPF_OFFSET			11
#define MACB_NCR_TPF_SIZE			1
#define MACB_TZQ_OFFSET				12
#define MACB_TZQ_SIZE				1

/* Bitfields in NCFGR */
#define MACB_SPD_OFFSET				0
#define MACB_SPD_SIZE				1
#define MACB_FD_OFFSET				1
#define MACB_FD_SIZE				1
#define MACB_BIT_RATE_OFFSET			2
#define MACB_BIT_RATE_SIZE			1
#define MACB_JFRAME_OFFSET			3
#define MACB_JFRAME_SIZE			1
#define MACB_CAF_OFFSET				4
#define MACB_CAF_SIZE				1
#define MACB_NBC_OFFSET				5
#define MACB_NBC_SIZE				1
#define MACB_NCFGR_MTI_OFFSET			6
#define MACB_NCFGR_MTI_SIZE			1
#define MACB_UNI_OFFSET				7
#define MACB_UNI_SIZE				1
#define MACB_BIG_OFFSET				8
#define MACB_BIG_SIZE				1
#define MACB_EAE_OFFSET				9
#define MACB_EAE_SIZE				1
#define MACB_CLK_OFFSET				10
#define MACB_CLK_SIZE				2
#define MACB_RTY_OFFSET				12
#define MACB_RTY_SIZE				1
#define MACB_PAE_OFFSET				13
#define MACB_PAE_SIZE				1
#define MACB_RBOF_OFFSET			14
#define MACB_RBOF_SIZE				2
#define MACB_RLCE_OFFSET			16
#define MACB_RLCE_SIZE				1
#define MACB_DRFCS_OFFSET			17
#define MACB_DRFCS_SIZE				1
#define MACB_EFRHD_OFFSET			18
#define MACB_EFRHD_SIZE				1
#define MACB_IRXFCS_OFFSET			19
#define MACB_IRXFCS_SIZE			1

/* Bitfields in NSR */
#define MACB_NSR_LINK_OFFSET			0
#define MACB_NSR_LINK_SIZE			1
#define MACB_MDIO_OFFSET			1
#define MACB_MDIO_SIZE				1
#define MACB_IDLE_OFFSET			2
#define MACB_IDLE_SIZE				1

/* Bitfields in TSR */
#define MACB_UBR_OFFSET				0
#define MACB_UBR_SIZE				1
#define MACB_COL_OFFSET				1
#define MACB_COL_SIZE				1
#define MACB_TSR_RLE_OFFSET			2
#define MACB_TSR_RLE_SIZE			1
#define MACB_TGO_OFFSET				3
#define MACB_TGO_SIZE				1
#define MACB_BEX_OFFSET				4
#define MACB_BEX_SIZE				1
#define MACB_COMP_OFFSET			5
#define MACB_COMP_SIZE				1
#define MACB_UND_OFFSET				6
#define MACB_UND_SIZE				1

/* Bitfields in RSR */
#define MACB_BNA_OFFSET				0
#define MACB_BNA_SIZE				1
#define MACB_REC_OFFSET				1
#define MACB_REC_SIZE				1
#define MACB_OVR_OFFSET				2
#define MACB_OVR_SIZE				1

/* Bitfields in ISR/IER/IDR/IMR */
#define MACB_MFD_OFFSET				0
#define MACB_MFD_SIZE				1
#define MACB_RCOMP_OFFSET			1
#define MACB_RCOMP_SIZE				1
#define MACB_RXUBR_OFFSET			2
#define MACB_RXUBR_SIZE				1
#define MACB_TXUBR_OFFSET			3
#define MACB_TXUBR_SIZE				1
#define MACB_ISR_TUND_OFFSET			4
#define MACB_ISR_TUND_SIZE			1
#define MACB_ISR_RLE_OFFSET			5
#define MACB_ISR_RLE_SIZE			1
#define MACB_TXERR_OFFSET			6
#define MACB_TXERR_SIZE				1
#define MACB_TCOMP_OFFSET			7
#define MACB_TCOMP_SIZE				1
#define MACB_ISR_LINK_OFFSET			9
#define MACB_ISR_LINK_SIZE			1
#define MACB_ISR_ROVR_OFFSET			10
#define MACB_ISR_ROVR_SIZE			1
#define MACB_HRESP_OFFSET			11
#define MACB_HRESP_SIZE				1
#define MACB_PFR_OFFSET				12
#define MACB_PFR_SIZE				1
#define MACB_PTZ_OFFSET				13
#define MACB_PTZ_SIZE				1

/* Bitfields in MAN */
#define MACB_DATA_OFFSET			0
#define MACB_DATA_SIZE				16
#define MACB_CODE_OFFSET			16
#define MACB_CODE_SIZE				2
#define MACB_REGA_OFFSET			18
#define MACB_REGA_SIZE				5
#define MACB_PHYA_OFFSET			23
#define MACB_PHYA_SIZE				5
#define MACB_RW_OFFSET				28
#define MACB_RW_SIZE				2
#define MACB_SOF_OFFSET				30
#define MACB_SOF_SIZE				2

/* Bitfields in USRIO */
#define MACB_MII_OFFSET				0
#define MACB_MII_SIZE				1
#define MACB_EAM_OFFSET				1
#define MACB_EAM_SIZE				1
#define MACB_TX_PAUSE_OFFSET			2
#define MACB_TX_PAUSE_SIZE			1
#define MACB_TX_PAUSE_ZERO_OFFSET		3
#define MACB_TX_PAUSE_ZERO_SIZE			1

/* Bitfields in USRIO (AT91) */
#define MACB_RMII_OFFSET			0
#define MACB_RMII_SIZE				1
#define MACB_CLKEN_OFFSET			1
#define MACB_CLKEN_SIZE				1

/* Bitfields in WOL */
#define MACB_IP_OFFSET				0
#define MACB_IP_SIZE				16
#define MACB_MAG_OFFSET				16
#define MACB_MAG_SIZE				1
#define MACB_ARP_OFFSET				17
#define MACB_ARP_SIZE				1
#define MACB_SA1_OFFSET				18
#define MACB_SA1_SIZE				1
#define MACB_WOL_MTI_OFFSET			19
#define MACB_WOL_MTI_SIZE			1

/* Constants for CLK */
#define MACB_CLK_DIV8				0
#define MACB_CLK_DIV16				1
#define MACB_CLK_DIV32				2
#define MACB_CLK_DIV64				3

/* Constants for MAN register */
#define MACB_MAN_SOF				1
#define MACB_MAN_WRITE				1
#define MACB_MAN_READ				2
#define MACB_MAN_CODE				2

/* Bit manipulation macros */
#define MACB_BIT(name)					\
	(1 << MACB_##name##_OFFSET)
#define MACB_BF(name,value)				\
	(((value) & ((1 << MACB_##name##_SIZE) - 1))	\
	 << MACB_##name##_OFFSET)
#define MACB_BFEXT(name,value)\
	(((value) >> MACB_##name##_OFFSET)		\
	 & ((1 << MACB_##name##_SIZE) - 1))
#define MACB_BFINS(name,value,old)			\
	(((old) & ~(((1 << MACB_##name##_SIZE) - 1)	\
		    << MACB_##name##_OFFSET))		\
	 | MACB_BF(name,value))

/* Register access macros */
#define macb_readl(port,reg)				\
	readl((port)->regs + MACB_##reg)
#define macb_writel(port,reg,value)			\
	writel((value), (port)->regs + MACB_##reg)

#endif /* __DRIVERS_MACB_H__ */
