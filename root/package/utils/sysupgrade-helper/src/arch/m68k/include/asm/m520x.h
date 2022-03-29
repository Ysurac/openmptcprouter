/*
 * m520x.h -- Definitions for Freescale Coldfire 520x
 *
 * Copyright (C) 2004-2009 Freescale Semiconductor, Inc.
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

#ifndef __M520X__
#define __M520X__

/* *** System Control Module (SCM) *** */
#define SCM_MPR_MPROT0(x)		(((x) & 0x0F) << 28)
#define SCM_MPR_MPROT1(x)		(((x) & 0x0F) << 24)
#define SCM_MPR_MPROT2(x)		(((x) & 0x0F) << 20)
#define MPROT_MTR			4
#define MPROT_MTW			2
#define MPROT_MPL			1

#define SCM_PACRA_PACR0(x)		(((x) & 0x0F) << 28)
#define SCM_PACRA_PACR1(x)		(((x) & 0x0F) << 24)
#define SCM_PACRA_PACR2(x)		(((x) & 0x0F) << 20)

#define SCM_PACRB_PACR12(x)		(((x) & 0x0F) << 12)

#define SCM_PACRC_PACR16(x)		(((x) & 0x0F) << 28)
#define SCM_PACRC_PACR17(x)		(((x) & 0x0F) << 24)
#define SCM_PACRC_PACR18(x)		(((x) & 0x0F) << 20)
#define SCM_PACRC_PACR21(x)		(((x) & 0x0F) << 8)
#define SCM_PACRC_PACR22(x)		(((x) & 0x0F) << 4)
#define SCM_PACRC_PACR23(x)		((x) & 0x0F)

#define SCM_PACRD_PACR24(x)		(((x) & 0x0F) << 28)
#define SCM_PACRD_PACR25(x)		(((x) & 0x0F) << 24)
#define SCM_PACRD_PACR26(x)		(((x) & 0x0F) << 20)
#define SCM_PACRD_PACR28(x)		(((x) & 0x0F) << 12)
#define SCM_PACRD_PACR29(x)		(((x) & 0x0F) << 8)
#define SCM_PACRD_PACR30(x)		(((x) & 0x0F) << 4)
#define SCM_PACRD_PACR31(x)		((x) & 0x0F)

#define SCM_PACRE_PACR32(x)		(((x) & 0x0F) << 28)
#define SCM_PACRE_PACR33(x)		(((x) & 0x0F) << 24)
#define SCM_PACRE_PACR34(x)		(((x) & 0x0F) << 20)
#define SCM_PACRE_PACR35(x)		(((x) & 0x0F) << 16)
#define SCM_PACRE_PACR36(x)		(((x) & 0x0F) << 12)

#define SCM_PACRF_PACR40(x)		(((x) & 0x0F) << 28)
#define SCM_PACRF_PACR41(x)		(((x) & 0x0F) << 24)
#define SCM_PACRF_PACR42(x)		(((x) & 0x0F) << 20)

#define PACR_SP	4
#define PACR_WP	2
#define PACR_TP	1

#define SCM_BMT_BME			(0x00000008)
#define SCM_BMT_BMT(x)			((x) & 0x07)
#define SCM_BMT_BMT1024			(0x0000)
#define SCM_BMT_BMT512			(0x0001)
#define SCM_BMT_BMT256			(0x0002)
#define SCM_BMT_BMT128			(0x0003)
#define SCM_BMT_BMT64			(0x0004)
#define SCM_BMT_BMT32			(0x0005)
#define SCM_BMT_BMT16			(0x0006)
#define SCM_BMT_BMT8			(0x0007)

#define SCM_CWCR_RO			(0x8000)
#define SCM_CWCR_CWR_WH			(0x0100)
#define SCM_CWCR_CWE			(0x0080)
#define SCM_CWRI_WINDOW			(0x0060)
#define SCM_CWRI_RESET			(0x0040)
#define SCM_CWRI_INT_RESET		(0x0020)
#define SCM_CWRI_INT			(0x0000)
#define SCM_CWCR_CWT(x)			(((x) & 0x001F))

#define SCM_ISR_CFEI			(0x02)
#define SCM_ISR_CWIC			(0x01)

#define SCM_CFIER_ECFEI			(0x01)

#define SCM_CFLOC_LOC			(0x80)

#define SCM_CFATR_WRITE			(0x80)
#define SCM_CFATR_SZ32			(0x20)
#define SCM_CFATR_SZ16			(0x10)
#define SCM_CFATR_SZ08			(0x00)
#define SCM_CFATR_CACHE			(0x08)
#define SCM_CFATR_MODE			(0x02)
#define SCM_CFATR_TYPE			(0x01)

/* *** Interrupt Controller (INTC) *** */
#define INT0_LO_RSVD0			(0)
#define INT0_LO_EPORT_F1		(1)
#define INT0_LO_EPORT_F4		(2)
#define INT0_LO_EPORT_F7		(3)
#define INT1_LO_PIT0			(4)
#define INT1_LO_PIT1			(5)
/* 6 - 7 rsvd */
#define INT0_LO_EDMA_00			(8)
#define INT0_LO_EDMA_01			(9)
#define INT0_LO_EDMA_02			(10)
#define INT0_LO_EDMA_03			(11)
#define INT0_LO_EDMA_04			(12)
#define INT0_LO_EDMA_05			(13)
#define INT0_LO_EDMA_06			(14)
#define INT0_LO_EDMA_07			(15)
#define INT0_LO_EDMA_08			(16)
#define INT0_LO_EDMA_09			(17)
#define INT0_LO_EDMA_10			(18)
#define INT0_LO_EDMA_11			(19)
#define INT0_LO_EDMA_12			(20)
#define INT0_LO_EDMA_13			(21)
#define INT0_LO_EDMA_14			(22)
#define INT0_LO_EDMA_15			(23)
#define INT0_LO_EDMA_ERR		(24)
#define INT0_LO_SCM_CWIC		(25)
#define INT0_LO_UART0			(26)
#define INT0_LO_UART1			(27)
#define INT0_LO_UART2			(28)
/* 29 rsvd */
#define INT0_LO_I2C			(30)
#define INT0_LO_QSPI			(31)

#define INT0_HI_DTMR0			(32)
#define INT0_HI_DTMR1			(33)
#define INT0_HI_DTMR2			(34)
#define INT0_HI_DTMR3			(35)
#define INT0_HI_FEC0_TXF		(36)
#define INT0_HI_FEC0_TXB		(37)
#define INT0_HI_FEC0_UN			(38)
#define INT0_HI_FEC0_RL			(39)
#define INT0_HI_FEC0_RXF		(40)
#define INT0_HI_FEC0_RXB		(41)
#define INT0_HI_FEC0_MII		(42)
#define INT0_HI_FEC0_LC			(43)
#define INT0_HI_FEC0_HBERR		(44)
#define INT0_HI_FEC0_GRA		(45)
#define INT0_HI_FEC0_EBERR		(46)
#define INT0_HI_FEC0_BABT		(47)
#define INT0_HI_FEC0_BABR		(48)
/* 49 - 61 rsvd */
#define INT0_HI_SCMISR_CFEI		(62)

/* *** Reset Controller Module (RCM) *** */
#define RCM_RCR_SOFTRST			(0x80)
#define RCM_RCR_FRCRSTOUT		(0x40)

#define RCM_RSR_SOFT			(0x20)
#define RCM_RSR_WDOG			(0x10)
#define RCM_RSR_POR			(0x08)
#define RCM_RSR_EXT			(0x04)
#define RCM_RSR_WDR_CORE		(0x02)
#define RCM_RSR_LOL			(0x01)

/* *** Chip Configuration Module (CCM) *** */
#define CCM_CCR_CSC			(0x0200)
#define CCM_CCR_OSCFREQ			(0x0080)
#define CCM_CCR_LIMP			(0x0040)
#define CCM_CCR_LOAD			(0x0020)
#define CCM_CCR_BOOTPS(x)		(((x) & 0x0003) << 3)
#define CCM_CCR_OSC_MODE		(0x0004)
#define CCM_CCR_PLL_MODE		(0x0002)
#define CCM_CCR_RESERVED		(0x0001)

#define CCM_CIR_PIN(x)			(((x) & 0xFFC0) >> 6)
#define CCM_CIR_PRN(x)			((x) & 0x003F)

/* *** General Purpose I/O (GPIO) *** */
#define GPIO_PDR_BUSCTL(x)		((x) & 0x0F)
#define GPIO_PDR_BE(x)			((x) & 0x0F)
#define GPIO_PDR_CS(x)			(((x) & 0x07) << 1)
#define GPIO_PDR_FECI2C(x)		((x) & 0x0F)
#define GPIO_PDR_QSPI(x)		((x) & 0x0F)
#define GPIO_PDR_TIMER(x)		((x) & 0x0F)
#define GPIO_PDR_UART(x)		((x) & 0xFF)
#define GPIO_PDR_FECH(x)		((x) & 0xFF)
#define GPIO_PDR_FECL(x)		((x) & 0xFF)

#define GPIO_PAR_FBCTL_OE		(0x10)
#define GPIO_PAR_FBCTL_TA		(0x08)
#define GPIO_PAR_FBCTL_RWB		(0x04)
#define GPIO_PAR_FBCTL_TS_UNMASK	(0xFC)
#define GPIO_PAR_FBCTL_TS_TS		(0x03)
#define GPIO_PAR_FBCTL_TS_DMA		(0x02)

#define GPIO_PAR_BE3			(0x08)
#define GPIO_PAR_BE2			(0x04)
#define GPIO_PAR_BE1			(0x02)
#define GPIO_PAR_BE0			(0x01)

#define GPIO_PAR_CS3			(0x08)
#define GPIO_PAR_CS2			(0x04)
#define GPIO_PAR_CS1_UNMASK		(0xFC)
#define GPIO_PAR_CS1_CS1		(0x03)
#define GPIO_PAR_CS1_SDCS1		(0x02)

#define GPIO_PAR_FECI2C_RMII_UNMASK	(0x0F)
#define GPIO_PAR_FECI2C_MDC_UNMASK	(0x3F)
#define GPIO_PAR_FECI2C_MDC_MDC		(0xC0)
#define GPIO_PAR_FECI2C_MDC_SCL		(0x80)
#define GPIO_PAR_FECI2C_MDC_U2TXD	(0x40)
#define GPIO_PAR_FECI2C_MDIO_UNMASK	(0xCF)
#define GPIO_PAR_FECI2C_MDIO_MDIO	(0x30)
#define GPIO_PAR_FECI2C_MDIO_SDA	(0x20)
#define GPIO_PAR_FECI2C_MDIO_U2RXD	(0x10)
#define GPIO_PAR_FECI2C_I2C_UNMASK	(0xF0)
#define GPIO_PAR_FECI2C_SCL_UNMASK	(0xF3)
#define GPIO_PAR_FECI2C_SCL_SCL		(0x0C)
#define GPIO_PAR_FECI2C_SCL_U2RXD	(0x04)
#define GPIO_PAR_FECI2C_SDA_UNMASK	(0xFC)
#define GPIO_PAR_FECI2C_SDA_SDA		(0x03)
#define GPIO_PAR_FECI2C_SDA_U2TXD	(0x01)

#define GPIO_PAR_QSPI_PCS2_UNMASK	(0x3F)
#define GPIO_PAR_QSPI_PCS2_PCS2		(0xC0)
#define GPIO_PAR_QSPI_PCS2_DACK0	(0x80)
#define GPIO_PAR_QSPI_PCS2_U2RTS	(0x40)
#define GPIO_PAR_QSPI_DIN_UNMASK	(0xCF)
#define GPIO_PAR_QSPI_DIN_DIN		(0x30)
#define GPIO_PAR_QSPI_DIN_DREQ0		(0x20)
#define GPIO_PAR_QSPI_DIN_U2CTS		(0x10)
#define GPIO_PAR_QSPI_DOUT_UNMASK	(0xF3)
#define GPIO_PAR_QSPI_DOUT_DOUT		(0x0C)
#define GPIO_PAR_QSPI_DOUT_SDA		(0x08)
#define GPIO_PAR_QSPI_SCK_UNMASK	(0xFC)
#define GPIO_PAR_QSPI_SCK_SCK		(0x03)
#define GPIO_PAR_QSPI_SCK_SCL		(0x02)

#define GPIO_PAR_TMR_TIN3(x)		(((x) & 0x03) << 6)
#define GPIO_PAR_TMR_TIN2(x)		(((x) & 0x03) << 4)
#define GPIO_PAR_TMR_TIN1(x)		(((x) & 0x03) << 2)
#define GPIO_PAR_TMR_TIN0(x)		((x) & 0x03)
#define GPIO_PAR_TMR_TIN3_UNMASK	(0x3F)
#define GPIO_PAR_TMR_TIN3_TIN3		(0xC0)
#define GPIO_PAR_TMR_TIN3_TOUT3		(0x80)
#define GPIO_PAR_TMR_TIN3_U2CTS		(0x40)
#define GPIO_PAR_TMR_TIN2_UNMASK	(0xCF)
#define GPIO_PAR_TMR_TIN2_TIN2		(0x30)
#define GPIO_PAR_TMR_TIN2_TOUT2		(0x20)
#define GPIO_PAR_TMR_TIN2_U2RTS		(0x10)
#define GPIO_PAR_TMR_TIN1_UNMASK	(0xF3)
#define GPIO_PAR_TMR_TIN1_TIN1		(0x0C)
#define GPIO_PAR_TMR_TIN1_TOUT1		(0x08)
#define GPIO_PAR_TMR_TIN1_U2RXD		(0x04)
#define GPIO_PAR_TMR_TIN0_UNMASK	(0xFC)
#define GPIO_PAR_TMR_TIN0_TIN0		(0x03)
#define GPIO_PAR_TMR_TIN0_TOUT0		(0x02)
#define GPIO_PAR_TMR_TIN0_U2TXD		(0x01)

#define GPIO_PAR_UART1_UNMASK		(0xF03F)
#define GPIO_PAR_UART0_UNMASK		(0xFFC0)
#define GPIO_PAR_UART_U1CTS_UNMASK	(0xF3FF)
#define GPIO_PAR_UART_U1CTS_U1CTS	(0x0C00)
#define GPIO_PAR_UART_U1CTS_TIN1	(0x0800)
#define GPIO_PAR_UART_U1CTS_PCS1	(0x0400)
#define GPIO_PAR_UART_U1RTS_UNMASK	(0xFCFF)
#define GPIO_PAR_UART_U1RTS_U1RTS	(0x0300)
#define GPIO_PAR_UART_U1RTS_TOUT1	(0x0200)
#define GPIO_PAR_UART_U1RTS_PCS1	(0x0100)
#define GPIO_PAR_UART_U1TXD		(0x0080)
#define GPIO_PAR_UART_U1RXD		(0x0040)
#define GPIO_PAR_UART_U0CTS_UNMASK	(0xFFCF)
#define GPIO_PAR_UART_U0CTS_U0CTS	(0x0030)
#define GPIO_PAR_UART_U0CTS_TIN0	(0x0020)
#define GPIO_PAR_UART_U0CTS_PCS0	(0x0010)
#define GPIO_PAR_UART_U0RTS_UNMASK	(0xFFF3)
#define GPIO_PAR_UART_U0RTS_U0RTS	(0x000C)
#define GPIO_PAR_UART_U0RTS_TOUT0	(0x0008)
#define GPIO_PAR_UART_U0RTS_PCS0	(0x0004)
#define GPIO_PAR_UART_U0TXD		(0x0002)
#define GPIO_PAR_UART_U0RXD		(0x0001)

#define GPIO_PAR_FEC_7W_UNMASK		(0xF3)
#define GPIO_PAR_FEC_7W_FEC		(0x0C)
#define GPIO_PAR_FEC_7W_U1RTS		(0x04)
#define GPIO_PAR_FEC_MII_UNMASK		(0xFC)
#define GPIO_PAR_FEC_MII_FEC		(0x03)
#define GPIO_PAR_FEC_MII_UnCTS		(0x01)

#define GPIO_PAR_IRQ_IRQ4		(0x01)

#define GPIO_MSCR_FB_FBCLK(x)		(((x) & 0x03) << 6)
#define GPIO_MSCR_FB_DUP(x)		(((x) & 0x03) << 4)
#define GPIO_MSCR_FB_DLO(x)		(((x) & 0x03) << 2)
#define GPIO_MSCR_FB_ADRCTL(x)		((x) & 0x03)
#define GPIO_MSCR_FB_FBCLK_UNMASK	(0x3F)
#define GPIO_MSCR_FB_DUP_UNMASK		(0xCF)
#define GPIO_MSCR_FB_DLO_UNMASK		(0xF3)
#define GPIO_MSCR_FB_ADRCTL_UNMASK	(0xFC)

#define GPIO_MSCR_SDR_SDCLKB(x)		(((x) & 0x03) << 4)
#define GPIO_MSCR_SDR_SDCLK(x)		(((x) & 0x03) << 2)
#define GPIO_MSCR_SDR_SDRAM(x)		((x) & 0x03)
#define GPIO_MSCR_SDR_SDCLKB_UNMASK	(0xCF)
#define GPIO_MSCR_SDR_SDCLK_UNMASK	(0xF3)
#define GPIO_MSCR_SDR_SDRAM_UNMASK	(0xFC)

#define MSCR_25VDDR			(0x03)
#define MSCR_18VDDR_FULL		(0x02)
#define MSCR_OPENDRAIN			(0x01)
#define MSCR_18VDDR_HALF		(0x00)

#define GPIO_DSCR_I2C(x)		((x) & 0x03)
#define GPIO_DSCR_I2C_UNMASK		(0xFC)

#define GPIO_DSCR_MISC_DBG(x)		(((x) & 0x03) << 4)
#define GPIO_DSCR_MISC_DBG_UNMASK	(0xCF)
#define GPIO_DSCR_MISC_RSTOUT(x)	(((x) & 0x03) << 2)
#define GPIO_DSCR_MISC_RSTOUT_UNMASK	(0xF3)
#define GPIO_DSCR_MISC_TIMER(x)		((x) & 0x03)
#define GPIO_DSCR_MISC_TIMER_UNMASK	(0xFC)

#define GPIO_DSCR_FEC(x)		((x) & 0x03)
#define GPIO_DSCR_FEC_UNMASK		(0xFC)

#define GPIO_DSCR_UART_UART1(x)		(((x) & 0x03) << 4)
#define GPIO_DSCR_UART_UART1_UNMASK	(0xCF)
#define GPIO_DSCR_UART_UART0(x)		(((x) & 0x03) << 2)
#define GPIO_DSCR_UART_UART0_UNMASK	(0xF3)
#define GPIO_DSCR_UART_IRQ(x)		((x) & 0x03)
#define GPIO_DSCR_UART_IRQ_UNMASK	(0xFC)

#define GPIO_DSCR_QSPI(x)		((x) & 0x03)
#define GPIO_DSCR_QSPI_UNMASK		(0xFC)

#define DSCR_50PF			(0x03)
#define DSCR_30PF			(0x02)
#define DSCR_20PF			(0x01)
#define DSCR_10PF			(0x00)

/* *** Phase Locked Loop (PLL) *** */
#define PLL_PODR_CPUDIV(x)		(((x) & 0x0F) << 4)
#define PLL_PODR_CPUDIV_UNMASK		(0x0F)
#define PLL_PODR_BUSDIV(x)		((x) & 0x0F)
#define PLL_PODR_BUSDIV_UNMASK		(0xF0)

#define PLL_PCR_DITHEN			(0x80)
#define PLL_PCR_DITHDEV(x)		((x) & 0x07)
#define PLL_PCR_DITHDEV_UNMASK		(0xF8)

#endif				/* __M520X__ */
