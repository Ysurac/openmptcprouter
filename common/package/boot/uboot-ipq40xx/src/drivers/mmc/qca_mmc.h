/*
 * Copyright (c) 2014 The Linux Foundation. All rights reserved.

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

#ifndef ___QCA_MMC_H_
#define ___QCA_MMC_H_

#define MMCIPOWER	0x000
#define MCI_PWR_OFF	0x00
#define MCI_PWR_UP	0x02
#define MCI_PWR_ON	0x03
#define MCI_SW_RST	(1 << 7)

#define MMCICLOCK		0x004
#define MCI_CLK_ENABLE		(1 << 8)
#define MCI_CLK_PWRSAVE		(1 << 9)
#define MCI_CLK_FLOWENA		(1 << 12)
#define MCI_CLK_SELECTIN	(1 << 15)
#define MCI_CLK_WIDEBUS_1	(0 << 10)
#define MCI_CLK_WIDEBUS_4	(2 << 10)
#define MCI_CLK_WIDEBUS_8	(3 << 10)


#define MMCIARGUMENT		0x008
#define MMCICOMMAND		0x00c
#define MCI_CPSM_RESPONSE	(1 << 6)
#define MCI_CPSM_LONGRSP	(1 << 7)
#define MCI_CPSM_ENABLE		(1 << 10)
#define MCI_CPSM_PROGENA	(1 << 11)

#define MMCIRESPCMD		0x010
#define MMCIRESPONSE0		0x014
#define MMCIRESPONSE1		0x018
#define MMCIRESPONSE2		0x01c
#define MMCIRESPONSE3		0x020
#define MMCIDATATIMER		0x024
#define MMCIDATALENGTH		0x028

#define MMCIDATACTRL		0x02c
#define MCI_DPSM_ENABLE		(1 << 0)
#define MCI_DPSM_DIRECTION	(1 << 1)
#define MCI_RX_DATA_PEND	(1 << 20)

#define MMCIDATACNT		0x030
#define MMCISTATUS		0x034
#define MCI_CMDCRCFAIL		(1 << 0)
#define MCI_DATACRCFAIL		(1 << 1)
#define MCI_CMDTIMEOUT		(1 << 2)
#define MCI_DATATIMEOUT		(1 << 3)
#define MCI_TXUNDERRUN		(1 << 4)
#define MCI_RXOVERRUN		(1 << 5)
#define MCI_CMDRESPEND		(1 << 6)
#define MCI_CMDSENT		(1 << 7)
#define MCI_DATAEND		(1 << 8)
#define MCI_DATABLOCKEND	(1 << 10)
#define MCI_CMDACTIVE		(1 << 11)
#define MCI_TXACTIVE		(1 << 12)
#define MCI_RXACTIVE		(1 << 13)
#define MCI_TXFIFOHALFEMPTY	(1 << 14)
#define MCI_RXFIFOHALFFULL	(1 << 15)
#define MCI_TXFIFOFULL		(1 << 16)
#define MCI_RXFIFOFULL		(1 << 17)
#define MCI_TXFIFOEMPTY		(1 << 18)
#define MCI_RXFIFOEMPTY		(1 << 19)
#define MCI_TXDATAAVLBL		(1 << 20)
#define MCI_RXDATAAVLBL		(1 << 21)

#define MMCICLEAR		0x038
#define MCI_CMDCRCFAILCLR	(1 << 0)
#define MCI_DATACRCFAILCLR	(1 << 1)
#define MCI_CMDTIMEOUTCLR	(1 << 2)
#define MCI_DATATIMEOUTCLR	(1 << 3)
#define MCI_TXUNDERRUNCLR	(1 << 4)
#define MCI_RXOVERRUNCLR	(1 << 5)
#define MCI_CMDRESPENDCLR	(1 << 6)
#define MCI_CMDSENTCLR		(1 << 7)
#define MCI_DATAENDCLR		(1 << 8)
#define MCI_STARTBITERRCLR	(1 << 9)
#define MCI_DATABLOCKENDCLR	(1 << 10)

#define MCI_SDIOINTRCLR		(1 << 22)
#define MCI_PROGDONECLR		(1 << 23)
#define MCI_ATACMDCOMPLCLR 	(1 << 24)
#define MCI_SDIOINTROPECLR	(1 << 25)
#define MCI_CCSTIMEOUTCLR	(1 << 26)

#define MCI_CLEAR_STATIC_MASK   \
	(MCI_CMDCRCFAILCLR|MCI_DATACRCFAILCLR|MCI_CMDTIMEOUTCLR|\
	MCI_DATATIMEOUTCLR|MCI_TXUNDERRUNCLR|MCI_RXOVERRUNCLR|  \
	MCI_CMDRESPENDCLR|MCI_CMDSENTCLR|MCI_DATAENDCLR|        \
	MCI_STARTBITERRCLR|MCI_DATABLOCKENDCLR|MCI_SDIOINTRCLR| \
	MCI_SDIOINTROPECLR|MCI_PROGDONECLR|MCI_ATACMDCOMPLCLR|  \
	MCI_CCSTIMEOUTCLR)

#define MCI_STATUS2		0x06C
#define MCI_MCLK_REG_WR_ACTIVE	(1 << 0)

#define MMCIFIFO		0x080 /* to 0x0bc */
/*
 * The size of the FIFO in bytes.
 */
#define MCI_FIFOSIZE		(16*4)

#define MCI_FIFOHALFSIZE	(MCI_FIFOSIZE / 2)

#define MCI_FIFODEPTH		16
#define MCI_HFIFO_COUNT		(MCI_FIFODEPTH / 2)

#endif  /*  __QCA_MMC_H_ */
