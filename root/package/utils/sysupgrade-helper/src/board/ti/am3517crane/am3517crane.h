/*
 * am3517crane.h - Header file for the AM3517 CraneBoard.
 *
 * Author: Srinath R <srinath@mistralsolutions.com>
 *
 * Based on logicpd/am3517evm/am3517evm.h
 *
 * Copyright (C) 2011 Mistral Solutions Pvt Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _AM3517CRANE_H_
#define _AM3517CRANE_H_

const omap3_sysinfo sysinfo = {
	DDR_DISCRETE,
	"CraneBoard",
	"NAND",
};

/*
 * IEN  - Input Enable
 * IDIS - Input Disable
 * PTD  - Pull type Down
 * PTU  - Pull type Up
 * DIS  - Pull type selection is inactive
 * EN   - Pull type selection is active
 * M0   - Mode 0
 * The commented string gives the final mux configuration for that pin
 */
#define MUX_AM3517CRANE()\
	/*SDRC*/\
	MUX_VAL(CP(SDRC_D0),    (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D1),    (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D2),    (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D3),    (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D4),    (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D5),    (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D6),    (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D7),    (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D8),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D9),    (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D10),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D11),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D12),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D13),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D14),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D15),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D16),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D17),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D18),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D19),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D20),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D21),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D22),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D23),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D24),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D25),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D26),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D27),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D28),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D29),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D30),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_D31),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_CLK),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_DQS0),  (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_CKE0),  (M0))\
	MUX_VAL(CP(SDRC_DQS1),  (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_DQS2),  (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_DQS3),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SDRC_DQS0N), (IEN  | PTD | EN  | M0))\
	MUX_VAL(CP(SDRC_DQS1N), (IEN  | PTD | EN  | M0))\
	MUX_VAL(CP(SDRC_DQS2N),	(IEN  | PTD | EN  | M0))\
	MUX_VAL(CP(SDRC_DQS3N),	(IEN  | PTD | EN  | M0))\
	MUX_VAL(CP(SDRC_CKE0),  (M0))\
	MUX_VAL(CP(SDRC_CKE1),	(M0))\
	/*sdrc_strben_dly0*/\
	MUX_VAL(CP(STRBEN_DLY0),    (IEN  | PTD | EN  | M0))\
	/*sdrc_strben_dly1*/\
	MUX_VAL(CP(STRBEN_DLY1),	(IEN  | PTD | EN  | M0))\
	/*GPMC*/\
	MUX_VAL(CP(GPMC_A1),    (M7))\
	MUX_VAL(CP(GPMC_A2),    (IDIS | PTU | DIS | M4))\
	MUX_VAL(CP(GPMC_A3),    (IDIS | PTU | EN  | M4))\
	MUX_VAL(CP(GPMC_A4),    (IDIS | PTU | EN  | M4))\
	MUX_VAL(CP(GPMC_A5),    (IDIS | PTU | EN  | M4))\
	MUX_VAL(CP(GPMC_A6),    (M7))\
	MUX_VAL(CP(GPMC_A7),    (IDIS | PTU | EN  | M4))\
	MUX_VAL(CP(GPMC_A8),    (IEN  | PTU | EN  | M4))\
	MUX_VAL(CP(GPMC_A9),	(M7))\
	MUX_VAL(CP(GPMC_A10),	(M7))\
	MUX_VAL(CP(GPMC_D0),    (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(GPMC_D1),    (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(GPMC_D2),    (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(GPMC_D3),    (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(GPMC_D4),    (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(GPMC_D5),    (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(GPMC_D6),    (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(GPMC_D7),    (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(GPMC_D8),    (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(GPMC_D9),	(IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(GPMC_D10),   (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(GPMC_D11),   (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(GPMC_D12),   (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(GPMC_D13),   (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(GPMC_D14),   (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(GPMC_D15),   (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(GPMC_NCS0),  (IDIS | PTU | EN  | M0))\
	MUX_VAL(CP(GPMC_NCS1),  (IDIS | PTU | EN  | M4))\
	MUX_VAL(CP(GPMC_NCS2),  (M7))\
	MUX_VAL(CP(GPMC_NCS3),  (M7))\
	MUX_VAL(CP(GPMC_NCS4),  (M7))\
	MUX_VAL(CP(GPMC_NCS5),  (M7))\
	MUX_VAL(CP(GPMC_NCS6),  (M7))\
	MUX_VAL(CP(GPMC_NCS7),	(M7))\
	MUX_VAL(CP(GPMC_CLK),	(IDIS | PTU | EN  | M0))/*TP*/\
	MUX_VAL(CP(GPMC_NADV_ALE),	(IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(GPMC_NOE),   (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(GPMC_NWE),	(IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(GPMC_NBE0_CLE),	(IDIS | PTU | EN  | M0))\
	MUX_VAL(CP(GPMC_NBE1),  (M7))\
	MUX_VAL(CP(GPMC_NWP),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(GPMC_WAIT0), (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(GPMC_WAIT1), (M7))\
	MUX_VAL(CP(GPMC_WAIT2), (M7))\
	MUX_VAL(CP(GPMC_WAIT3), (IDIS | PTU | EN  | M4))/*GPIO_65*/\
	/*DSS*/\
	MUX_VAL(CP(DSS_PCLK),	(IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_HSYNC),  (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_VSYNC),  (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_ACBIAS),	(IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA0),  (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA1),  (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA2),  (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA3),  (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA4),  (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA5),  (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA6),  (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA7),  (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA8),  (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA9),  (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA10), (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA11), (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA12), (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA13), (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA14), (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA15), (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA16), (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA17), (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA18), (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA19), (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA20), (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA21), (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA22), (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(DSS_DATA23),	(IDIS | PTD | DIS | M0))\
	/*MMC1*/\
	MUX_VAL(CP(MMC1_CLK),   (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(MMC1_CMD),   (IEN  | PTU | DIS | M0))\
	MUX_VAL(CP(MMC1_DAT0),  (IEN  | PTU | DIS | M0))\
	MUX_VAL(CP(MMC1_DAT1),  (IEN  | PTU | DIS | M0))\
	MUX_VAL(CP(MMC1_DAT2),  (IEN  | PTU | DIS | M0))\
	MUX_VAL(CP(MMC1_DAT3),  (IEN  | PTU | DIS | M0))\
	MUX_VAL(CP(MMC1_DAT4),  (IEN  | PTU | DIS | M0))\
	MUX_VAL(CP(MMC1_DAT5),  (IEN  | PTU | DIS | M0))\
	MUX_VAL(CP(MMC1_DAT6),  (IEN  | PTU | DIS | M0))\
	MUX_VAL(CP(MMC1_DAT7),  (IEN  | PTU | DIS | M0))\
	/*MMC2*/\
	MUX_VAL(CP(MMC2_CLK),   (IEN  | PTD | EN  | M0))\
	MUX_VAL(CP(MMC2_CMD),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(MMC2_DAT0),  (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(MMC2_DAT1),  (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(MMC2_DAT2),  (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(MMC2_DAT3),  (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(MMC2_DAT4),  (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(MMC2_DAT5),  (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(MMC2_DAT6),  (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(MMC2_DAT7),	(IEN  | PTD | DIS | M0))\
	/*McBSP*/\
	MUX_VAL(CP(MCBSP_CLKS),	(IEN  | PTU | DIS | M0))\
	MUX_VAL(CP(MCBSP1_CLKR),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(MCBSP1_FSR),	(IDIS | PTU | EN  | M0))\
	MUX_VAL(CP(MCBSP1_DX),  (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(MCBSP1_DR),  (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(MCBSP1_FSX),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(MCBSP1_CLKX),	(IEN  | PTD | DIS | M0))\
	\
	MUX_VAL(CP(MCBSP2_FSX),	(M7))\
	MUX_VAL(CP(MCBSP2_CLKX),	(M7))\
	MUX_VAL(CP(MCBSP2_DR),  (M7))\
	MUX_VAL(CP(MCBSP2_DX),  (M7))\
	\
	MUX_VAL(CP(MCBSP3_DX),  (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(MCBSP3_DR),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(MCBSP3_CLKX),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(MCBSP3_FSX),	(IEN  | PTD | DIS | M0))\
	\
	MUX_VAL(CP(MCBSP4_CLKX),	(M7))\
	MUX_VAL(CP(MCBSP4_DR),  (M7))\
	MUX_VAL(CP(MCBSP4_DX),  (M7))\
	MUX_VAL(CP(MCBSP4_FSX),	(M7))\
	/*UART*/\
	MUX_VAL(CP(UART1_TX),	(M7))\
	MUX_VAL(CP(UART1_RTS),  (M7))\
	MUX_VAL(CP(UART1_CTS),  (M7))\
	MUX_VAL(CP(UART1_RX),   (M7))\
	\
	MUX_VAL(CP(UART2_CTS),  (M7))\
	MUX_VAL(CP(UART2_RTS),	(M7))\
	MUX_VAL(CP(UART2_TX),   (M7))\
	MUX_VAL(CP(UART2_RX),	(M7))\
	\
	MUX_VAL(CP(UART3_CTS_RCTX),	(IEN  | PTU | DIS | M0))\
	MUX_VAL(CP(UART3_RTS_SD),	(IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(UART3_RX_IRRX),  (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(UART3_TX_IRTX),	(IDIS | PTD | DIS | M0))\
	/*I2C 1, 2, 3*/\
	MUX_VAL(CP(I2C1_SCL),   (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(I2C1_SDA),   (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(I2C2_SCL),   (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(I2C2_SDA),   (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(I2C3_SCL),   (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(I2C3_SDA),	(IEN  | PTU | EN  | M0))\
	/*McSPI*/\
	MUX_VAL(CP(MCSPI1_CLK),	(IEN  | PTU | EN  | M4))/*GPIO_171 TP*/\
	MUX_VAL(CP(MCSPI1_SIMO),    (IEN  | PTU | EN  | M4))/*GPIO_172 TP*/\
	MUX_VAL(CP(MCSPI1_SOMI),	(IEN  | PTU | EN  | M4))/*GPIO_173 TP*/\
	MUX_VAL(CP(MCSPI1_CS0), (IEN  | PTU | EN  | M4))/*GPIO_174 TP*/\
	MUX_VAL(CP(MCSPI1_CS1), (IEN  | PTU | EN  | M4))/*GPIO_175 TP*/\
	MUX_VAL(CP(MCSPI1_CS2), (IEN  | PTU | EN  | M4))/*GPIO_176 TP*/\
	MUX_VAL(CP(MCSPI1_CS3),	(IEN  | PTD | EN  | M4))/*GPIO_176 TP*/\
	\
	MUX_VAL(CP(MCSPI2_CLK),	(M7))\
	MUX_VAL(CP(MCSPI2_SIMO),    (M7))\
	MUX_VAL(CP(MCSPI2_SOMI),	(M7))\
	MUX_VAL(CP(MCSPI2_CS0), (M7))\
	MUX_VAL(CP(MCSPI2_CS1), (M7))\
	/*CCDC*/\
	MUX_VAL(CP(CCDC_PCLK),  (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(CCDC_FIELD),	(IEN  | PTD | DIS | M1))/*CCDC_DATA8*/\
	MUX_VAL(CP(CCDC_HD),    (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(CCDC_VD),	(IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(CCDC_WEN),	(IEN  | PTD | DIS | M1))/*CCDC_DATA9 */\
	MUX_VAL(CP(CCDC_DATA0), (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(CCDC_DATA1), (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(CCDC_DATA2), (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(CCDC_DATA3), (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(CCDC_DATA4), (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(CCDC_DATA5), (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(CCDC_DATA6),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(CCDC_DATA7),	(IEN  | PTD | DIS | M0))\
	/*RMII*/\
	MUX_VAL(CP(RMII_MDIO_DATA),	(IEN  |  M0))\
	MUX_VAL(CP(RMII_MDIO_CLK),	(M0))\
	MUX_VAL(CP(RMII_RXD0),  (IEN  | PTD | M0))\
	MUX_VAL(CP(RMII_RXD1),	(IEN  | PTD | M0))\
	MUX_VAL(CP(RMII_CRS_DV),	(IEN  | PTD | M0))\
	MUX_VAL(CP(RMII_RXER),  (PTD  | M0))\
	MUX_VAL(CP(RMII_TXD0),  (PTD  | M0))\
	MUX_VAL(CP(RMII_TXD1),  (PTD  | M0))\
	MUX_VAL(CP(RMII_TXEN),	(PTD  | M0))\
	MUX_VAL(CP(RMII_50MHZ_CLK),	(IEN  | PTD | EN  | M0))\
	/*HECC*/\
	MUX_VAL(CP(HECC1_TXD),  (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(HECC1_RXD),	(IEN  | PTU | EN  | M0))\
	/*HSUSB*/\
	MUX_VAL(CP(USB0_DRVBUS),	(IEN  | PTD | EN  | M0))\
	/*HDQ*/\
	MUX_VAL(CP(HDQ_SIO),	(IEN  | PTU | EN  | M4))\
	/*Control and debug*/\
	MUX_VAL(CP(SYS_32K),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SYS_CLKREQ),	(IEN  | PTD | DIS | M4))/*GPIO_1 TPS_SLEEP*/\
	MUX_VAL(CP(SYS_NIRQ),	(IEN  | PTU | EN  | M0))\
	/*SYS_nRESWARM*/\
	MUX_VAL(CP(SYS_NRESWARM),	(IEN | PTU | EN | M0))/*GPIO_30 ToExp*/\
	MUX_VAL(CP(SYS_BOOT1),  (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SYS_BOOT2),  (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SYS_BOOT3),  (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SYS_BOOT4),  (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SYS_BOOT5),  (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(SYS_BOOT6),  (IDIS | PTD | DIS | M0))\
	MUX_VAL(CP(SYS_BOOT7),  (IEN  | PTD | EN  | M0))\
	MUX_VAL(CP(SYS_BOOT8),	(IEN  | PTD | EN  | M0))\
	MUX_VAL(CP(SYS_CLKOUT1),    (IEN  | PTD | DIS | M4))/*GPIO_10 TP*/\
	MUX_VAL(CP(SYS_CLKOUT2),	(IEN  | PTU | EN  | M0))\
	/*JTAG*/\
	MUX_VAL(CP(JTAG_nTRST),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(JTAG_TCK),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(JTAG_TMS),   (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(JTAG_TDI),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(JTAG_EMU0),  (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(JTAG_EMU1),	(IEN  | PTD | DIS | M0))\
	/*ETK (ES2 onwards)*/\
	MUX_VAL(CP(ETK_CLK_ES2),    (IEN  | PTU | EN  | M3))\
	MUX_VAL(CP(ETK_CTL_ES2),	(IDIS | PTD | DIS | M3))\
	MUX_VAL(CP(ETK_D0_ES2), (IEN  | PTD | DIS | M3))\
	MUX_VAL(CP(ETK_D1_ES2), (IEN  | PTD | DIS | M3))\
	MUX_VAL(CP(ETK_D2_ES2),	(IEN  | PTD | EN  | M3))\
	MUX_VAL(CP(ETK_D3_ES2), (IEN  | PTD | DIS | M3))\
	MUX_VAL(CP(ETK_D4_ES2),	(IEN  | PTD | DIS | M3))\
	MUX_VAL(CP(ETK_D5_ES2), (IEN  | PTD | DIS | M3))\
	MUX_VAL(CP(ETK_D6_ES2), (IEN  | PTD | DIS | M3))\
	MUX_VAL(CP(ETK_D7_ES2), (IEN  | PTD | DIS | M3))\
	MUX_VAL(CP(ETK_D8_ES2), (IEN  | PTD | DIS | M3))\
	MUX_VAL(CP(ETK_D9_ES2),	(IEN  | PTD | DIS | M3))\
	MUX_VAL(CP(ETK_D10_ES2),    (M7))\
	MUX_VAL(CP(ETK_D11_ES2),    (M7))\
	MUX_VAL(CP(ETK_D12_ES2),    (M7))\
	MUX_VAL(CP(ETK_D13_ES2),    (M7))\
	MUX_VAL(CP(ETK_D14_ES2),    (M7))\
	MUX_VAL(CP(ETK_D15_ES2),	(M7))\
	/*Die to Die*/\
	MUX_VAL(CP(D2D_MCAD34), (IEN  | PTD | EN  | M0))\
	MUX_VAL(CP(D2D_MCAD35), (IEN  | PTD | EN  | M0))\
	MUX_VAL(CP(D2D_MCAD36),	(IEN  | PTD | EN  | M0))\
	MUX_VAL(CP(D2D_CLK26MI),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(D2D_NRESPWRON),  (IEN  | PTD | EN  | M0))\
	MUX_VAL(CP(D2D_NRESWARM),   (IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(D2D_ARM9NIRQ),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(D2D_UMA2P6FIQ),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(D2D_SPINT),	(IEN  | PTD | EN  | M0))\
	MUX_VAL(CP(D2D_FRINT),	(IEN  | PTD | EN  | M0))\
	MUX_VAL(CP(D2D_DMAREQ0),    (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(D2D_DMAREQ1),    (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(D2D_DMAREQ2),    (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(D2D_DMAREQ3),    (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(D2D_N3GTRST),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(D2D_N3GTDI),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(D2D_N3GTDO),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(D2D_N3GTMS),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(D2D_N3GTCK),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(D2D_N3GRTCK),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(D2D_MSTDBY),	(IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(D2D_SWAKEUP),    (IEN  | PTD | EN  | M0))\
	MUX_VAL(CP(D2D_IDLEREQ),    (IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(D2D_IDLEACK),	(IEN  | PTU | EN  | M0))\
	MUX_VAL(CP(D2D_MWRITE),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(D2D_SWRITE),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(D2D_MREAD),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(D2D_SREAD),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(D2D_MBUSFLAG),	(IEN  | PTD | DIS | M0))\
	MUX_VAL(CP(D2D_SBUSFLAG),	(IEN  | PTD | DIS | M0))\

#endif /* _AM3517CRANE_H_ */
