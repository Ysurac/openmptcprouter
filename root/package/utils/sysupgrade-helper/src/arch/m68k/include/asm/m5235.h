/*
 * mcf5329.h -- Definitions for Freescale Coldfire 5329
 *
 * Copyright (C) 2004-2007 Freescale Semiconductor, Inc.
 * TsiChung Liew (Tsi-Chung.Liew@freescale.com)
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

#ifndef mcf5235_h
#define mcf5235_h
/****************************************************************************/

/*********************************************************************
* System Control Module (SCM)
*********************************************************************/

/* Bit definition and macros for SCM_IPSBAR */
#define SCM_IPSBAR_BA(x)		(((x)&0x03)<<30)
#define SCM_IPSBAR_V			(0x00000001)

/* Bit definition and macros for SCM_RAMBAR */
#define SCM_RAMBAR_BA(x)		(((x)&0xFFFF)<<16)
#define SCM_RAMBAR_BDE			(0x00000200)

/* Bit definition and macros for SCM_CRSR */
#define SCM_CRSR_EXT			(0x80)

/* Bit definitions and macros for SCM_CWCR */
#define SCM_CWCR_CWE			(0x80)
#define SCM_CWCR_CWRI			(0x40)
#define SCM_CWCR_CWT(x)			(((x)&0x07)<<3)
#define SCM_CWCR_CWTA			(0x04)
#define SCM_CWCR_CWTAVAL		(0x02)
#define SCM_CWCR_CWTIC			(0x01)

/* Bit definitions and macros for SCM_LPICR */
#define SCM_LPICR_ENBSTOP		(0x80)
#define SCM_LPICR_XLPM_IPL(x)		(((x)&0x07)<<4)
#define SCM_LPICR_XLPM_IPL_ANY		(0x00)
#define SCM_LPICR_XLPM_IPL_L2_7		(0x10)
#define SCM_LPICR_XLPM_IPL_L3_7		(0x20)
#define SCM_LPICR_XLPM_IPL_L4_7		(0x30)
#define SCM_LPICR_XLPM_IPL_L5_7		(0x40)
#define SCM_LPICR_XLPM_IPL_L6_7		(0x50)
#define SCM_LPICR_XLPM_IPL_L7		(0x70)

/* Bit definitions and macros for SCM_DMAREQC */
#define SCM_DMAREQC_EXT(x)		(((x)&0x0F)<<16)
#define SCM_DMAREQC_EXT_ETPU		(0x00080000)
#define SCM_DMAREQC_EXT_EXTDREQ2	(0x00040000)
#define SCM_DMAREQC_EXT_EXTDREQ1	(0x00020000)
#define SCM_DMAREQC_EXT_EXTDREQ0	(0x00010000)
#define SCM_DMAREQC_DMAC3(x)		(((x)&0x0F)<<12)
#define SCM_DMAREQC_DMAC2(x)		(((x)&0x0F)<<8)
#define SCM_DMAREQC_DMAC1(x)		(((x)&0x0F)<<4)
#define SCM_DMAREQC_DMAC0(x)		(((x)&0x0F))
#define SCM_DMAREQC_DMACn_DTMR0		(0x04)
#define SCM_DMAREQC_DMACn_DTMR1		(0x05)
#define SCM_DMAREQC_DMACn_DTMR2		(0x06)
#define SCM_DMAREQC_DMACn_DTMR3		(0x07)
#define SCM_DMAREQC_DMACn_UART0RX	(0x08)
#define SCM_DMAREQC_DMACn_UART1RX	(0x09)
#define SCM_DMAREQC_DMACn_UART2RX	(0x0A)
#define SCM_DMAREQC_DMACn_UART0TX	(0x0C)
#define SCM_DMAREQC_DMACn_UART1TX	(0x0D)
#define SCM_DMAREQC_DMACn_UART3TX	(0x0E)

/* Bit definitions and macros for SCM_MPARK */
#define SCM_MPARK_M2_P_EN		(0x02000000)
#define SCM_MPARK_M3_PRTY_MSK		(0x00C00000)
#define SCM_MPARK_M3_PRTY_4TH		(0x00000000)
#define SCM_MPARK_M3_PRTY_3RD		(0x00400000)
#define SCM_MPARK_M3_PRTY_2ND		(0x00800000)
#define SCM_MPARK_M3_PRTY_1ST		(0x00C00000)
#define SCM_MPARK_M2_PRTY_MSK		(0x00300000)
#define SCM_MPARK_M2_PRTY_4TH		(0x00000000)
#define SCM_MPARK_M2_PRTY_3RD		(0x00100000)
#define SCM_MPARK_M2_PRTY_2ND		(0x00200000)
#define SCM_MPARK_M2_PRTY_1ST		(0x00300000)
#define SCM_MPARK_M0_PRTY_MSK		(0x000C0000)
#define SCM_MPARK_M0_PRTY_4TH		(0x00000000)
#define SCM_MPARK_M0_PRTY_3RD		(0x00040000)
#define SCM_MPARK_M0_PRTY_2ND		(0x00080000)
#define SCM_MPARK_M0_PRTY_1ST		(0x000C0000)
#define SCM_MPARK_FIXED			(0x00004000)
#define SCM_MPARK_TIMEOUT		(0x00002000)
#define SCM_MPARK_PRKLAST		(0x00001000)
#define SCM_MPARK_LCKOUT_TIME(x)	(((x)&0x0F)<<8)

/* Bit definitions and macros for SCM_MPR */
#define SCM_MPR_MPR3			(0x08)
#define SCM_MPR_MPR2			(0x04)
#define SCM_MPR_MPR1			(0x02)
#define SCM_MPR_MPR0			(0x01)

/* Bit definitions and macros for SCM_PACRn */
#define SCM_PACRn_LOCK1			(0x80)
#define SCM_PACRn_ACCESSCTRL1(x)	(((x)&0x07)<<4)
#define SCM_PACRn_LOCK0			(0x08)
#define SCM_PACRn_ACCESSCTRL0(x)	(((x)&0x07))

/* Bit definitions and macros for SCM_GPACR */
#define SCM_PACRn_LOCK			(0x80)
#define SCM_PACRn_ACCESSCTRL0(x)	(((x)&0x07))

/*********************************************************************
* SDRAM Controller (SDRAMC)
*********************************************************************/
/* Bit definitions and macros for SDRAMC_DCR */
#define SDRAMC_DCR_NAM			(0x2000)
#define SDRAMC_DCR_COC			(0x1000)
#define SDRAMC_DCR_IS			(0x0800)
#define SDRAMC_DCR_RTIM_MASK		(0x0C00)
#define SDRAMC_DCR_RTIM_3CLKS		(0x0000)
#define SDRAMC_DCR_RTIM_6CLKS		(0x0200)
#define SDRAMC_DCR_RTIM_9CLKS		(0x0400)
#define SDRAMC_DCR_RC(x)		(((x)&0xFF)<<8)

/* Bit definitions and macros for SDRAMC_DARCn */
#define SDRAMC_DARCn_BA(x)		(((x)&0xFFFC)<<18)
#define SDRAMC_DARCn_RE			(0x00008000)
#define SDRAMC_DARCn_CASL_MASK		(0x00003000)
#define SDRAMC_DARCn_CASL_C0		(0x00000000)
#define SDRAMC_DARCn_CASL_C1		(0x00001000)
#define SDRAMC_DARCn_CASL_C2		(0x00002000)
#define SDRAMC_DARCn_CASL_C3		(0x00003000)
#define SDRAMC_DARCn_CBM_MASK		(0x00000700)
#define SDRAMC_DARCn_CBM_CMD17		(0x00000000)
#define SDRAMC_DARCn_CBM_CMD18		(0x00000100)
#define SDRAMC_DARCn_CBM_CMD19		(0x00000200)
#define SDRAMC_DARCn_CBM_CMD20		(0x00000300)
#define SDRAMC_DARCn_CBM_CMD21		(0x00000400)
#define SDRAMC_DARCn_CBM_CMD22		(0x00000500)
#define SDRAMC_DARCn_CBM_CMD23		(0x00000600)
#define SDRAMC_DARCn_CBM_CMD24		(0x00000700)
#define SDRAMC_DARCn_IMRS		(0x00000040)
#define SDRAMC_DARCn_PS_MASK		(0x00000030)
#define SDRAMC_DARCn_PS_32		(0x00000000)
#define SDRAMC_DARCn_PS_16		(0x00000010)
#define SDRAMC_DARCn_PS_8		(0x00000020)
#define SDRAMC_DARCn_IP			(0x00000008)

/* Bit definitions and macros for SDRAMC_DMRn */
#define SDRAMC_DMRn_BAM(x)		(((x)&0x3FFF)<<18)
#define SDRAMC_DMRn_WP			(0x00000100)
#define SDRAMC_DMRn_V			(0x00000001)

/*********************************************************************
* Interrupt Controller (INTC)
*********************************************************************/
#define INT0_LO_RSVD0			(0)
#define INT0_LO_EPORT1			(1)
#define INT0_LO_EPORT2			(2)
#define INT0_LO_EPORT3			(3)
#define INT0_LO_EPORT4			(4)
#define INT0_LO_EPORT5			(5)
#define INT0_LO_EPORT6			(6)
#define INT0_LO_EPORT7			(7)
#define INT0_LO_SCM			(8)
#define INT0_LO_DMA0			(9)
#define INT0_LO_DMA1			(10)
#define INT0_LO_DMA2			(11)
#define INT0_LO_DMA3			(12)
#define INT0_LO_UART0			(13)
#define INT0_LO_UART1			(14)
#define INT0_LO_UART2			(15)
#define INT0_LO_RSVD1			(16)
#define INT0_LO_I2C			(17)
#define INT0_LO_QSPI			(18)
#define INT0_LO_DTMR0			(19)
#define INT0_LO_DTMR1			(20)
#define INT0_LO_DTMR2			(21)
#define INT0_LO_DTMR3			(22)
#define INT0_LO_FEC_TXF			(23)
#define INT0_LO_FEC_TXB			(24)
#define INT0_LO_FEC_UN			(25)
#define INT0_LO_FEC_RL			(26)
#define INT0_LO_FEC_RXF			(27)
#define INT0_LO_FEC_RXB			(28)
#define INT0_LO_FEC_MII			(29)
#define INT0_LO_FEC_LC			(30)
#define INT0_LO_FEC_HBERR		(31)
#define INT0_HI_FEC_GRA			(32)
#define INT0_HI_FEC_EBERR		(33)
#define INT0_HI_FEC_BABT		(34)
#define INT0_HI_FEC_BABR		(35)
#define INT0_HI_PIT0			(36)
#define INT0_HI_PIT1			(37)
#define INT0_HI_PIT2			(38)
#define INT0_HI_PIT3			(39)
#define INT0_HI_RNG			(40)
#define INT0_HI_SKHA			(41)
#define INT0_HI_MDHA			(42)
#define INT0_HI_CAN1_BUF0I		(43)
#define INT0_HI_CAN1_BUF1I		(44)
#define INT0_HI_CAN1_BUF2I		(45)
#define INT0_HI_CAN1_BUF3I		(46)
#define INT0_HI_CAN1_BUF4I		(47)
#define INT0_HI_CAN1_BUF5I		(48)
#define INT0_HI_CAN1_BUF6I		(49)
#define INT0_HI_CAN1_BUF7I		(50)
#define INT0_HI_CAN1_BUF8I		(51)
#define INT0_HI_CAN1_BUF9I		(52)
#define INT0_HI_CAN1_BUF10I		(53)
#define INT0_HI_CAN1_BUF11I		(54)
#define INT0_HI_CAN1_BUF12I		(55)
#define INT0_HI_CAN1_BUF13I		(56)
#define INT0_HI_CAN1_BUF14I		(57)
#define INT0_HI_CAN1_BUF15I		(58)
#define INT0_HI_CAN1_ERRINT		(59)
#define INT0_HI_CAN1_BOFFINT		(60)
/* 60-63 Reserved */

/* 0 - 7 Reserved */
#define INT1_LO_CAN1_BUF0I		(8)
#define INT1_LO_CAN1_BUF1I		(9)
#define INT1_LO_CAN1_BUF2I		(10)
#define INT1_LO_CAN1_BUF3I		(11)
#define INT1_LO_CAN1_BUF4I		(12)
#define INT1_LO_CAN1_BUF5I		(13)
#define INT1_LO_CAN1_BUF6I		(14)
#define INT1_LO_CAN1_BUF7I		(15)
#define INT1_LO_CAN1_BUF8I		(16)
#define INT1_LO_CAN1_BUF9I		(17)
#define INT1_LO_CAN1_BUF10I		(18)
#define INT1_LO_CAN1_BUF11I		(19)
#define INT1_LO_CAN1_BUF12I		(20)
#define INT1_LO_CAN1_BUF13I		(21)
#define INT1_LO_CAN1_BUF14I		(22)
#define INT1_LO_CAN1_BUF15I		(23)
#define INT1_LO_CAN1_ERRINT		(24)
#define INT1_LO_CAN1_BOFFINT		(25)
/* 26 Reserved */
#define INT1_LO_ETPU_TC0F		(27)
#define INT1_LO_ETPU_TC1F		(28)
#define INT1_LO_ETPU_TC2F		(29)
#define INT1_LO_ETPU_TC3F		(30)
#define INT1_LO_ETPU_TC4F		(31)
#define INT1_HI_ETPU_TC5F		(32)
#define INT1_HI_ETPU_TC6F		(33)
#define INT1_HI_ETPU_TC7F		(34)
#define INT1_HI_ETPU_TC8F		(35)
#define INT1_HI_ETPU_TC9F		(36)
#define INT1_HI_ETPU_TC10F		(37)
#define INT1_HI_ETPU_TC11F		(38)
#define INT1_HI_ETPU_TC12F		(39)
#define INT1_HI_ETPU_TC13F		(40)
#define INT1_HI_ETPU_TC14F		(41)
#define INT1_HI_ETPU_TC15F		(42)
#define INT1_HI_ETPU_TC16F		(43)
#define INT1_HI_ETPU_TC17F		(44)
#define INT1_HI_ETPU_TC18F		(45)
#define INT1_HI_ETPU_TC19F		(46)
#define INT1_HI_ETPU_TC20F		(47)
#define INT1_HI_ETPU_TC21F		(48)
#define INT1_HI_ETPU_TC22F		(49)
#define INT1_HI_ETPU_TC23F		(50)
#define INT1_HI_ETPU_TC24F		(51)
#define INT1_HI_ETPU_TC25F		(52)
#define INT1_HI_ETPU_TC26F		(53)
#define INT1_HI_ETPU_TC27F		(54)
#define INT1_HI_ETPU_TC28F		(55)
#define INT1_HI_ETPU_TC29F		(56)
#define INT1_HI_ETPU_TC30F		(57)
#define INT1_HI_ETPU_TC31F		(58)
#define INT1_HI_ETPU_TGIF		(59)

/*********************************************************************
* General Purpose I/O (GPIO)
*********************************************************************/
/* Bit definitions and macros for GPIO_PODR */
#define GPIO_PODR_ADDR(x)		(((x)&0x07)<<5)
#define GPIO_PODR_ADDR_MASK		(0xE0)
#define GPIO_PODR_BS(x)			((x)&0x0F)
#define GPIO_PODR_BS_MASK		(0x0F)
#define GPIO_PODR_CS(x)			(((x)&0x7F)<<1)
#define GPIO_PODR_CS_MASK		(0xFE)
#define GPIO_PODR_SDRAM(X)		((x)&0x3F)
#define GPIO_PODR_SDRAM_MASK		(0x3F)
#define GPIO_PODR_FECI2C(x)		GPIO_PODR_BS(x)
#define GPIO_PODR_FECI2C_MASK		GPIO_PODR_BS_MASK
#define GPIO_PODR_UARTH(x)		((x)&0x03)
#define GPIO_PODR_UARTH_MASK		(0x03)
#define GPIO_PODR_QSPI(x)		((x)&0x1F)
#define GPIO_PODR_QSPI_MASK		(0x1F)
#define GPIO_PODR_ETPU(x)		((x)&0x07)
#define GPIO_PODR_ETPU_MASK		(0x07)

/* Bit definitions and macros for GPIO_PDDR */
#define GPIO_PDDR_ADDR(x)		GPIO_PODR_ADDR(x)
#define GPIO_PDDR_ADDR_MASK		GPIO_PODR_ADDR_MASK
#define GPIO_PDDR_BS(x)			GPIO_PODR_BS(x)
#define GPIO_PDDR_BS_MASK		GPIO_PODR_BS_MASK
#define GPIO_PDDR_CS(x)			GPIO_PODR_CS(x)
#define GPIO_PDDR_CS_MASK		GPIO_PODR_CS_MASK
#define GPIO_PDDR_SDRAM(X)		GPIO_PODR_SDRAM(X)
#define GPIO_PDDR_SDRAM_MASK		GPIO_PODR_SDRAM_MASK
#define GPIO_PDDR_FECI2C(x)		GPIO_PDDR_BS(x)
#define GPIO_PDDR_FECI2C_MASK		GPIO_PDDR_BS_MASK
#define GPIO_PDDR_UARTH(x)		GPIO_PODR_UARTH(x)
#define GPIO_PDDR_UARTH_MASK		GPIO_PODR_UARTH_MASK
#define GPIO_PDDR_QSPI(x)		GPIO_PODR_QSPI(x)
#define GPIO_PDDR_QSPI_MASK		GPIO_PODR_QSPI_MASK
#define GPIO_PDDR_ETPU(x)		GPIO_PODR_ETPU(x)
#define GPIO_PDDR_ETPU_MASK		GPIO_PODR_ETPU_MASK

/* Bit definitions and macros for GPIO_PPDSDR */
#define GPIO_PPDSDR_ADDR(x)		GPIO_PODR_ADDR(x)
#define GPIO_PPDSDR_ADDR_MASK		GPIO_PODR_ADDR_MASK
#define GPIO_PPDSDR_BS(x)		GPIO_PODR_BS(x)
#define GPIO_PPDSDR_BS_MASK		GPIO_PODR_BS_MASK
#define GPIO_PPDSDR_CS(x)		GPIO_PODR_CS(x)
#define GPIO_PPDSDR_CS_MASK		GPIO_PODR_CS_MASK
#define GPIO_PPDSDR_SDRAM(X)		GPIO_PODR_SDRAM(X)
#define GPIO_PPDSDR_SDRAM_MASK		GPIO_PODR_SDRAM_MASK
#define GPIO_PPDSDR_FECI2C(x)		GPIO_PPDSDR_BS(x)
#define GPIO_PPDSDR_FECI2C_MASK		GPIO_PPDSDR_BS_MASK
#define GPIO_PPDSDR_UARTH(x)		GPIO_PODR_UARTH(x)
#define GPIO_PPDSDR_UARTH_MASK		GPIO_PODR_UARTH_MASK
#define GPIO_PPDSDR_QSPI(x)		GPIO_PODR_QSPI(x)
#define GPIO_PPDSDR_QSPI_MASK		GPIO_PODR_QSPI_MASK
#define GPIO_PPDSDR_ETPU(x)		GPIO_PODR_ETPU(x)
#define GPIO_PPDSDR_ETPU_MASK		GPIO_PODR_ETPU_MASK

/* Bit definitions and macros for GPIO_PCLRR */
#define GPIO_PCLRR_ADDR(x)		GPIO_PODR_ADDR(x)
#define GPIO_PCLRR_ADDR_MASK		GPIO_PODR_ADDR_MASK
#define GPIO_PCLRR_BS(x)		GPIO_PODR_BS(x)
#define GPIO_PCLRR_BS_MASK		GPIO_PODR_BS_MASK
#define GPIO_PCLRR_CS(x)		GPIO_PODR_CS(x)
#define GPIO_PCLRR_CS_MASK		GPIO_PODR_CS_MASK
#define GPIO_PCLRR_SDRAM(X)		GPIO_PODR_SDRAM(X)
#define GPIO_PCLRR_SDRAM_MASK		GPIO_PODR_SDRAM_MASK
#define GPIO_PCLRR_FECI2C(x)		GPIO_PCLRR_BS(x)
#define GPIO_PCLRR_FECI2C_MASK		GPIO_PCLRR_BS_MASK
#define GPIO_PCLRR_UARTH(x)		GPIO_PODR_UARTH(x)
#define GPIO_PCLRR_UARTH_MASK		GPIO_PODR_UARTH_MASK
#define GPIO_PCLRR_QSPI(x)		GPIO_PODR_QSPI(x)
#define GPIO_PCLRR_QSPI_MASK		GPIO_PODR_QSPI_MASK
#define GPIO_PCLRR_ETPU(x)		GPIO_PODR_ETPU(x)
#define GPIO_PCLRR_ETPU_MASK		GPIO_PODR_ETPU_MASK

/* Bit definitions and macros for GPIO_PAR */
#define GPIO_PAR_AD_ADDR23		(0x80)
#define GPIO_PAR_AD_ADDR22		(0x40)
#define GPIO_PAR_AD_ADDR21		(0x20)
#define GPIO_PAR_AD_DATAL		(0x01)
#define GPIO_PAR_BUSCTL_OE		(0x4000)
#define GPIO_PAR_BUSCTL_TA		(0x1000)
#define GPIO_PAR_BUSCTL_TEA(x)		(((x)&0x03)<<10)
#define GPIO_PAR_BUSCTL_TEA_MASK	(0x0C00)
#define GPIO_PAR_BUSCTL_TEA_GPIO	(0x0400)
#define GPIO_PAR_BUSCTL_TEA_DREQ1	(0x0800)
#define GPIO_PAR_BUSCTL_TEA_EXTBUS	(0x0C00)
#define GPIO_PAR_BUSCTL_RWB		(0x0100)
#define GPIO_PAR_BUSCTL_TSIZ1		(0x0040)
#define GPIO_PAR_BUSCTL_TSIZ0		(0x0010)
#define GPIO_PAR_BUSCTL_TS(x)		(((x)&0x03)<<2)
#define GPIO_PAR_BUSCTL_TS_MASK		(0x0C)
#define GPIO_PAR_BUSCTL_TS_GPIO		(0x04)
#define GPIO_PAR_BUSCTL_TS_DACK2	(0x08)
#define GPIO_PAR_BUSCTL_TS_EXTBUS	(0x0C)
#define GPIO_PAR_BUSCTL_TIP(x)		((x)&0x03)
#define GPIO_PAR_BUSCTL_TIP_MASK	(0x03)
#define GPIO_PAR_BUSCTL_TIP_GPIO	(0x01)
#define GPIO_PAR_BUSCTL_TIP_DREQ0	(0x02)
#define GPIO_PAR_BUSCTL_TIP_EXTBUS	(0x03)
#define GPIO_PAR_BS(x)			((x)&0x0F)
#define GPIO_PAR_BS_MASK		(0x0F)
#define GPIO_PAR_CS(x)			(((x)&0x7F)<<1)
#define GPIO_PAR_CS_MASK		(0xFE)
#define GPIO_PAR_CS_CS7			(0x80)
#define GPIO_PAR_CS_CS6			(0x40)
#define GPIO_PAR_CS_CS5			(0x20)
#define GPIO_PAR_CS_CS4			(0x10)
#define GPIO_PAR_CS_CS3			(0x08)
#define GPIO_PAR_CS_CS2			(0x04)
#define GPIO_PAR_CS_CS1			(0x02)
#define GPIO_PAR_CS_SD3			GPIO_PAR_CS_CS3
#define GPIO_PAR_CS_SD2			GPIO_PAR_CS_CS2
#define GPIO_PAR_SDRAM_CSSDCS(x)	(((x)&0x03)<<6)
#define GPIO_PAR_SDRAM_CSSDCS_MASK	(0xC0)
#define GPIO_PAR_SDRAM_SDWE		(0x20)
#define GPIO_PAR_SDRAM_SCAS		(0x10)
#define GPIO_PAR_SDRAM_SRAS		(0x08)
#define GPIO_PAR_SDRAM_SCKE		(0x04)
#define GPIO_PAR_SDRAM_SDCS(x)		((x)&0x03)
#define GPIO_PAR_SDRAM_SDCS_MASK	(0x03)
#define GPIO_PAR_FECI2C_EMDC(x)		(((x)&0x03)<<6)
#define GPIO_PAR_FECI2C_EMDC_MASK	(0xC0)
#define GPIO_PAR_FECI2C_EMDC_U2TXD	(0x40)
#define GPIO_PAR_FECI2C_EMDC_I2CSCL	(0x80)
#define GPIO_PAR_FECI2C_EMDC_FECEMDC	(0xC0)
#define GPIO_PAR_FECI2C_EMDIO(x)	(((x)&0x03)<<4)
#define GPIO_PAR_FECI2C_EMDIO_MASK	(0x30)
#define GPIO_PAR_FECI2C_EMDIO_U2RXD	(0x10)
#define GPIO_PAR_FECI2C_EMDIO_I2CSDA	(0x20)
#define GPIO_PAR_FECI2C_EMDIO_FECEMDIO	(0x30)
#define GPIO_PAR_FECI2C_SCL(x)		(((x)&0x03)<<2)
#define GPIO_PAR_FECI2C_SCL_MASK	(0x0C)
#define GPIO_PAR_FECI2C_SCL_CAN0RX	(0x08)
#define GPIO_PAR_FECI2C_SCL_I2CSCL	(0x0C)
#define GPIO_PAR_FECI2C_SDA(x)		((x)&0x03)
#define GPIO_PAR_FECI2C_SDA_MASK	(0x03)
#define GPIO_PAR_FECI2C_SDA_CAN0TX	(0x02)
#define GPIO_PAR_FECI2C_SDA_I2CSDA	(0x03)
#define GPIO_PAR_UART_DREQ2		(0x8000)
#define GPIO_PAR_UART_CAN1EN		(0x4000)
#define GPIO_PAR_UART_U2RXD		(0x2000)
#define GPIO_PAR_UART_U2TXD		(0x1000)
#define GPIO_PAR_UART_U1RXD(x)		(((x)&0x03)<<10)
#define GPIO_PAR_UART_U1RXD_MASK	(0x0C00)
#define GPIO_PAR_UART_U1RXD_CAN0RX	(0x0800)
#define GPIO_PAR_UART_U1RXD_U1RXD	(0x0C00)
#define GPIO_PAR_UART_U1TXD(x)		(((x)&0x03)<<8)
#define GPIO_PAR_UART_U1TXD_MASK	(0x0300)
#define GPIO_PAR_UART_U1TXD_CAN0TX	(0x0200)
#define GPIO_PAR_UART_U1TXD_U1TXD	(0x0300)
#define GPIO_PAR_UART_U1CTS(x)		(((x)&0x03)<<6)
#define GPIO_PAR_UART_U1CTS_MASK	(0x00C0)
#define GPIO_PAR_UART_U1CTS_U2CTS	(0x0080)
#define GPIO_PAR_UART_U1CTS_U1CTS	(0x00C0)
#define GPIO_PAR_UART_U1RTS(x)		(((x)&0x03)<<4)
#define GPIO_PAR_UART_U1RTS_MASK	(0x0030)
#define GPIO_PAR_UART_U1RTS_U2RTS	(0x0020)
#define GPIO_PAR_UART_U1RTS_U1RTS	(0x0030)
#define GPIO_PAR_UART_U0RXD		(0x0008)
#define GPIO_PAR_UART_U0TXD		(0x0004)
#define GPIO_PAR_UART_U0CTS		(0x0002)
#define GPIO_PAR_UART_U0RTS		(0x0001)
#define GPIO_PAR_QSPI_CS1(x)		(((x)&0x03)<<6)
#define GPIO_PAR_QSPI_CS1_MASK		(0xC0)
#define GPIO_PAR_QSPI_CS1_SDRAMSCKE	(0x80)
#define GPIO_PAR_QSPI_CS1_QSPICS1	(0xC0)
#define GPIO_PAR_QSPI_CS0		(0x20)
#define GPIO_PAR_QSPI_DIN(x)		(((x)&0x03)<<3)
#define GPIO_PAR_QSPI_DIN_MASK		(0x18)
#define GPIO_PAR_QSPI_DIN_I2CSDA	(0x10)
#define GPIO_PAR_QSPI_DIN_QSPIDIN	(0x18)
#define GPIO_PAR_QSPI_DOUT		(0x04)
#define GPIO_PAR_QSPI_SCK(x)		((x)&0x03)
#define GPIO_PAR_QSPI_SCK_MASK		(0x03)
#define GPIO_PAR_QSPI_SCK_I2CSCL	(0x02)
#define GPIO_PAR_QSPI_SCK_QSPISCK	(0x03)
#define GPIO_PAR_DT3IN(x)		(((x)&0x03)<<14)
#define GPIO_PAR_DT3IN_MASK		(0xC000)
#define GPIO_PAR_DT3IN_QSPICS2		(0x4000)
#define GPIO_PAR_DT3IN_U2CTS		(0x8000)
#define GPIO_PAR_DT3IN_DT3IN		(0xC000)
#define GPIO_PAR_DT2IN(x)		(((x)&0x03)<<12)
#define GPIO_PAR_DT2IN_MASK		(0x3000)
#define GPIO_PAR_DT2IN_DT2OUT		(0x1000)
#define GPIO_PAR_DT2IN_DREQ2		(0x2000)
#define GPIO_PAR_DT2IN_DT2IN		(0x3000)
#define GPIO_PAR_DT1IN(x)		(((x)&0x03)<<10)
#define GPIO_PAR_DT1IN_MASK		(0x0C00)
#define GPIO_PAR_DT1IN_DT1OUT		(0x0400)
#define GPIO_PAR_DT1IN_DREQ1		(0x0800)
#define GPIO_PAR_DT1IN_DT1IN		(0x0C00)
#define GPIO_PAR_DT0IN(x)		(((x)&0x03)<<8)
#define GPIO_PAR_DT0IN_MASK		(0x0300)
#define GPIO_PAR_DT0IN_DREQ0		(0x0200)
#define GPIO_PAR_DT0IN_DT0IN		(0x0300)
#define GPIO_PAR_DT3OUT(x)		(((x)&0x03)<<6)
#define GPIO_PAR_DT3OUT_MASK		(0x00C0)
#define GPIO_PAR_DT3OUT_QSPICS3		(0x0040)
#define GPIO_PAR_DT3OUT_U2RTS		(0x0080)
#define GPIO_PAR_DT3OUT_DT3OUT		(0x00C0)
#define GPIO_PAR_DT2OUT(x)		(((x)&0x03)<<4)
#define GPIO_PAR_DT2OUT_MASK		(0x0030)
#define GPIO_PAR_DT2OUT_DACK2		(0x0020)
#define GPIO_PAR_DT2OUT_DT2OUT		(0x0030)
#define GPIO_PAR_DT1OUT(x)		(((x)&0x03)<<2)
#define GPIO_PAR_DT1OUT_MASK		(0x000C)
#define GPIO_PAR_DT1OUT_DACK1		(0x0008)
#define GPIO_PAR_DT1OUT_DT1OUT		(0x000C)
#define GPIO_PAR_DT0OUT(x)		((x)&0x03)
#define GPIO_PAR_DT0OUT_MASK		(0x0003)
#define GPIO_PAR_DT0OUT_DACK0		(0x0002)
#define GPIO_PAR_DT0OUT_DT0OUT		(0x0003)
#define GPIO_PAR_ETPU_TCRCLK		(0x04)
#define GPIO_PAR_ETPU_UTPU_ODIS		(0x02)
#define GPIO_PAR_ETPU_LTPU_ODIS		(0x01)

/* Bit definitions and macros for GPIO_DSCR */
#define GPIO_DSCR_EIM_EIM1		(0x10)
#define GPIO_DSCR_EIM_EIM0		(0x01)
#define GPIO_DSCR_ETPU_ETPU31_24	(0x40)
#define GPIO_DSCR_ETPU_ETPU23_16	(0x10)
#define GPIO_DSCR_ETPU_ETPU15_8		(0x04)
#define GPIO_DSCR_ETPU_ETPU7_0		(0x01)
#define GPIO_DSCR_FECI2C_FEC		(0x10)
#define GPIO_DSCR_FECI2C_I2C		(0x01)
#define GPIO_DSCR_UART_IRQ		(0x40)
#define GPIO_DSCR_UART_UART2		(0x10)
#define GPIO_DSCR_UART_UART1		(0x04)
#define GPIO_DSCR_UART_UART0		(0x01)
#define GPIO_DSCR_QSPI_QSPI		(0x01)
#define GPIO_DSCR_TIMER			(0x01)

/*********************************************************************
* Chip Configuration Module (CCM)
*********************************************************************/
/* Bit definitions and macros for CCM_RCR */
#define CCM_RCR_SOFTRST			(0x80)
#define CCM_RCR_FRCRSTOUT		(0x40)

/* Bit definitions and macros for CCM_RSR */
#define CCM_RSR_SOFT			(0x20)
#define CCM_RSR_WDR			(0x10)
#define CCM_RSR_POR			(0x08)
#define CCM_RSR_EXT			(0x04)
#define CCM_RSR_LOC			(0x02)
#define CCM_RSR_LOL			(0x01)

/* Bit definitions and macros for CCM_CCR */
#define CCM_CCR_LOAD			(0x8000)
#define CCM_CCR_SZEN			(0x0040)
#define CCM_CCR_PSTEN			(0x0020)
#define CCM_CCR_BME			(0x0008)
#define CCM_CCR_BMT(x)			((x)&0x07)
#define CCM_CCR_BMT_MASK		(0x0007)
#define CCM_CCR_BMT_64K			(0x0000)
#define CCM_CCR_BMT_32K			(0x0001)
#define CCM_CCR_BMT_16K			(0x0002)
#define CCM_CCR_BMT_8K			(0x0003)
#define CCM_CCR_BMT_4K			(0x0004)
#define CCM_CCR_BMT_2K			(0x0005)
#define CCM_CCR_BMT_1K			(0x0006)
#define CCM_CCR_BMT_512			(0x0007)

/* Bit definitions and macros for CCM_RCON */
#define CCM_RCON_RCSC(x)		(((x)&0x0003)<<8)
#define CCM_RCON_RLOAD			(0x0020)
#define CCM_RCON_BOOTPS(x)		(((x)&0x0003)<<3)
#define CCM_RCON_BOOTPS_MASK		(0x0018)
#define CCM_RCON_BOOTPS_32		(0x0018)
#define CCM_RCON_BOOTPS_16		(0x0008)
#define CCM_RCON_BOOTPS_8		(0x0010)
#define CCM_RCON_MODE			(0x0001)

/* Bit definitions and macros for CCM_CIR */
#define CCM_CIR_PIN(x)			(((x) & 0xFFC0) >> 6)
#define CCM_CIR_PRN(x)			((x) & 0x003F)

/*********************************************************************
* PLL Clock Module
*********************************************************************/
/* Bit definitions and macros for PLL_SYNCR */
#define PLL_SYNCR_MFD(x)		(((x)&0x07)<<24)
#define PLL_SYNCR_MFD_MASK		(0x07000000)
#define PLL_SYNCR_RFC(x)		(((x)&0x07)<<19)
#define PLL_SYNCR_RFC_MASK		(0x00380000)
#define PLL_SYNCR_LOCEN			(0x00040000)
#define PLL_SYNCR_LOLRE			(0x00020000)
#define PLL_SYNCR_LOCRE			(0x00010000)
#define PLL_SYNCR_DISCLK		(0x00008000)
#define PLL_SYNCR_LOLIRQ		(0x00004000)
#define PLL_SYNCR_LOCIRQ		(0x00002000)
#define PLL_SYNCR_RATE			(0x00001000)
#define PLL_SYNCR_DEPTH(x)		(((x)&0x03)<<10)
#define PLL_SYNCR_EXP(x)		((x)&0x03FF)

/* Bit definitions and macros for PLL_SYNSR */
#define PLL_SYNSR_LOLF			(0x00000200)
#define PLL_SYNSR_LOC			(0x00000100)
#define PLL_SYNSR_MODE			(0x00000080)
#define PLL_SYNSR_PLLSEL		(0x00000040)
#define PLL_SYNSR_PLLREF		(0x00000020)
#define PLL_SYNSR_LOCKS			(0x00000010)
#define PLL_SYNSR_LOCK			(0x00000008)
#define PLL_SYNSR_LOCF			(0x00000004)
#define PLL_SYNSR_CALDONE		(0x00000002)
#define PLL_SYNSR_CALPASS		(0x00000001)

/*********************************************************************
* Watchdog Timer Modules (WTM)
*********************************************************************/
/* Bit definitions and macros for WTM_WCR */
#define WTM_WCR_WAIT			(0x0008)
#define WTM_WCR_DOZE			(0x0004)
#define WTM_WCR_HALTED			(0x0002)
#define WTM_WCR_EN			(0x0001)

#endif				/* mcf5235_h */
