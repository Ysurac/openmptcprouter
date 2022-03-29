/*
 * Copyright (C) 2004 Arabella Software Ltd.
 * Yuli Barcohen <yuli@arabellasw.com>
 *
 * Support for Analogue&Micro Rattler boards family.
 * Tested on Rattler8248.
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
#include <mpc8260.h>
#include <ioports.h>

/*
 * I/O Port configuration table
 *
 * if conf is 1, then that port pin will be configured at boot time
 * according to the five values podr/pdir/ppar/psor/pdat for that entry
 */

#define CONFIG_SYS_FCC1 (CONFIG_ETHER_INDEX == 1)
#define CONFIG_SYS_FCC2 (CONFIG_ETHER_INDEX == 2)

const iop_conf_t iop_conf_tab[4][32] = {

    /* Port A */
    {	/*	      conf      ppar psor pdir podr pdat */
	/* PA31 */ { CONFIG_SYS_FCC1,   1,   1,   0,   0,   0 }, /* FCC1 MII COL    */
	/* PA30 */ { CONFIG_SYS_FCC1,   1,   1,   0,   0,   0 }, /* FCC1 MII CRS    */
	/* PA29 */ { CONFIG_SYS_FCC1,   1,   1,   1,   0,   0 }, /* FCC1 MII TX_ER  */
	/* PA28 */ { CONFIG_SYS_FCC1,   1,   1,   1,   0,   0 }, /* FCC1 MII TX_EN  */
	/* PA27 */ { CONFIG_SYS_FCC1,   1,   1,   0,   0,   0 }, /* FCC1 MII RX_DV  */
	/* PA26 */ { CONFIG_SYS_FCC1,   1,   1,   0,   0,   0 }, /* FCC1 MII RX_ER  */
	/* PA25 */ { 0,          0,   0,   0,   0,   0 }, /* PA25            */
	/* PA24 */ { 0,          0,   0,   0,   0,   0 }, /* PA24            */
	/* PA23 */ { 0,          0,   0,   0,   0,   0 }, /* PA23            */
	/* PA22 */ { 1,          0,   0,   1,   0,   1 }, /* Eth PHYs reset  */
	/* PA21 */ { CONFIG_SYS_FCC1,   1,   0,   1,   0,   0 }, /* FCC1 MII TxD[3] */
	/* PA20 */ { CONFIG_SYS_FCC1,   1,   0,   1,   0,   0 }, /* FCC1 MII TxD[2] */
	/* PA19 */ { CONFIG_SYS_FCC1,   1,   0,   1,   0,   0 }, /* FCC1 MII TxD[1] */
	/* PA18 */ { CONFIG_SYS_FCC1,   1,   0,   1,   0,   0 }, /* FCC1 MII TxD[0] */
	/* PA17 */ { CONFIG_SYS_FCC1,   1,   0,   0,   0,   0 }, /* FCC1 MII RxD[0] */
	/* PA16 */ { CONFIG_SYS_FCC1,   1,   0,   0,   0,   0 }, /* FCC1 MII RxD[1] */
	/* PA15 */ { CONFIG_SYS_FCC1,   1,   0,   0,   0,   0 }, /* FCC1 MII RxD[2] */
	/* PA14 */ { CONFIG_SYS_FCC1,   1,   0,   0,   0,   0 }, /* FCC1 MII RxD[3] */
	/* PA13 */ { 0,          0,   0,   0,   0,   0 }, /* PA13            */
	/* PA12 */ { 0,          0,   0,   0,   0,   0 }, /* PA12            */
	/* PA11 */ { 0,          0,   0,   0,   0,   0 }, /* PA11            */
	/* PA10 */ { 0,          0,   0,   0,   0,   0 }, /* PA10            */
	/* PA9  */ { 0,          1,   0,   1,   0,   0 }, /* SMC2 TxD        */
	/* PA8  */ { 0,          1,   0,   0,   0,   0 }, /* SMC2 RxD        */
	/* PA7  */ { 0,          0,   0,   0,   0,   0 }, /* PA7             */
	/* PA6  */ { 0,          0,   0,   0,   0,   0 }, /* PA6             */
	/* PA5  */ { 0,          0,   0,   0,   0,   0 }, /* PA5             */
	/* PA4  */ { 0,          0,   0,   0,   0,   0 }, /* PA4             */
	/* PA3  */ { 0,          0,   0,   0,   0,   0 }, /* PA3             */
	/* PA2  */ { 0,          0,   0,   0,   0,   0 }, /* PA2             */
	/* PA1  */ { 0,          0,   0,   0,   0,   0 }, /* PA1             */
	/* PA0  */ { 0,          0,   0,   0,   0,   0 }  /* PA0             */
    },

    /* Port B */
    {   /*	      conf      ppar psor pdir podr pdat */
	/* PB31 */ { CONFIG_SYS_FCC2,   1,   0,   1,   0,   0 }, /* FCC2 MII TX_ER  */
	/* PB30 */ { CONFIG_SYS_FCC2,   1,   0,   0,   0,   0 }, /* FCC2 MII RX_DV  */
	/* PB29 */ { CONFIG_SYS_FCC2,   1,   1,   1,   0,   0 }, /* FCC2 MII TX_EN  */
	/* PB28 */ { CONFIG_SYS_FCC2,   1,   0,   0,   0,   0 }, /* FCC2 MII RX_ER  */
	/* PB27 */ { CONFIG_SYS_FCC2,   1,   0,   0,   0,   0 }, /* FCC2 MII COL    */
	/* PB26 */ { CONFIG_SYS_FCC2,   1,   0,   0,   0,   0 }, /* FCC2 MII CRS    */
	/* PB25 */ { CONFIG_SYS_FCC2,   1,   0,   1,   0,   0 }, /* FCC2 MII TxD[3] */
	/* PB24 */ { CONFIG_SYS_FCC2,   1,   0,   1,   0,   0 }, /* FCC2 MII TxD[2] */
	/* PB23 */ { CONFIG_SYS_FCC2,   1,   0,   1,   0,   0 }, /* FCC2 MII TxD[1] */
	/* PB22 */ { CONFIG_SYS_FCC2,   1,   0,   1,   0,   0 }, /* FCC2 MII TxD[0] */
	/* PB21 */ { CONFIG_SYS_FCC2,   1,   0,   0,   0,   0 }, /* FCC2 MII RxD[0] */
	/* PB20 */ { CONFIG_SYS_FCC2,   1,   0,   0,   0,   0 }, /* FCC2 MII RxD[1] */
	/* PB19 */ { CONFIG_SYS_FCC2,   1,   0,   0,   0,   0 }, /* FCC2 MII RxD[2] */
	/* PB18 */ { CONFIG_SYS_FCC2,   1,   0,   0,   0,   0 }, /* FCC2 MII RxD[3] */
	/* PB17 */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PB16 */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PB15 */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PB14 */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PB13 */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PB12 */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PB11 */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PB10 */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PB9  */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PB8  */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PB7  */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PB6  */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PB5  */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PB4  */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PB3  */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PB2  */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PB1  */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PB0  */ { 0,          0,   0,   0,   0,   0 }  /* non-existent    */
    },

    /* Port C */
    {   /*	      conf      ppar psor pdir podr pdat */
	/* PC31 */ { 0,          0,   0,   0,   0,   0 }, /* PC31            */
	/* PC30 */ { 0,          0,   0,   0,   0,   0 }, /* PC30            */
	/* PC29 */ { 0,          0,   0,   0,   0,   0 }, /* PC29            */
	/* PC28 */ { 0,          0,   0,   0,   0,   0 }, /* PC28            */
	/* PC27 */ { 0,          0,   0,   0,   0,   0 }, /* PC27            */
	/* PC26 */ { 0,          0,   0,   0,   0,   0 }, /* PC26            */
	/* PC25 */ { 0,          0,   0,   0,   0,   0 }, /* PC25            */
	/* PC24 */ { 0,          0,   0,   0,   0,   0 }, /* PC24            */
	/* PC23 */ { 0,          0,   0,   0,   0,   0 }, /* PC23            */
	/* PC22 */ { CONFIG_SYS_FCC1,   1,   0,   0,   0,   0 }, /* FCC1 TxClk (CLK10) */
	/* PC21 */ { CONFIG_SYS_FCC1,   1,   0,   0,   0,   0 }, /* FCC1 RxClk (CLK11) */
	/* PC20 */ { 0,          0,   0,   0,   0,   0 }, /* PC20            */
	/* PC19 */ { 0,          0,   0,   0,   0,   0 }, /* PC19            */
	/* PC18 */ { CONFIG_SYS_FCC2,   1,   0,   0,   0,   0 }, /* FCC2 TxClk (CLK14) */
	/* PC17 */ { CONFIG_SYS_FCC2,   1,   0,   0,   0,   0 }, /* FCC2 RxClk (CLK15) */
	/* PC16 */ { 0,          0,   0,   0,   0,   0 }, /* PC16            */
	/* PC15 */ { 0,          0,   0,   0,   0,   0 }, /* PC15            */
	/* PC14 */ { 0,          0,   0,   0,   0,   0 }, /* PC14            */
	/* PC13 */ { 0,          0,   0,   0,   0,   0 }, /* PC13            */
	/* PC12 */ { 0,          0,   0,   0,   0,   0 }, /* PC12            */
	/* PC11 */ { 0,          0,   0,   0,   0,   0 }, /* PC11            */
	/* PC10 */ { 0,          0,   0,   0,   0,   0 }, /* PC10            */
	/* PC9  */ { 1,          0,   0,   1,   0,   1 }, /* MDIO            */
	/* PC8  */ { 1,          0,   0,   1,   0,   1 }, /* MDC             */
	/* PC7  */ { 0,          0,   0,   0,   0,   0 }, /* PC7             */
	/* PC6  */ { 0,          0,   0,   0,   0,   0 }, /* PC6             */
	/* PC5  */ { 1,          1,   0,   1,   0,   0 }, /* SMC1 TxD        */
	/* PC4  */ { 1,          1,   0,   0,   0,   0 }, /* SMC1 RxD        */
	/* PC3  */ { 0,          0,   0,   0,   0,   0 }, /* PC3             */
	/* PC2  */ { 0,          0,   0,   0,   0,   0 }, /* PC2             */
	/* PC1  */ { 0,          0,   0,   0,   0,   0 }, /* PC1             */
	/* PC0  */ { 0,          0,   0,   0,   0,   0 }, /* PC0             */
    },

    /* Port D */
    {   /*	      conf      ppar psor pdir podr pdat */
	/* PD31 */ { 1,          1,   0,   0,   0,   0 }, /* SCC1 RxD        */
	/* PD30 */ { 1,          1,   1,   1,   0,   0 }, /* SCC1 TxD        */
	/* PD29 */ { 0,          0,   0,   0,   0,   0 }, /* PD29            */
	/* PD28 */ { 0,          0,   0,   0,   0,   0 }, /* PD28            */
	/* PD27 */ { 0,          0,   0,   0,   0,   0 }, /* PD27            */
	/* PD26 */ { 0,          0,   0,   0,   0,   0 }, /* PD26            */
	/* PD25 */ { 0,          0,   0,   0,   0,   0 }, /* PD25            */
	/* PD24 */ { 0,          0,   0,   0,   0,   0 }, /* PD24            */
	/* PD23 */ { 0,          0,   0,   0,   0,   0 }, /* PD23            */
	/* PD22 */ { 0,          0,   0,   0,   0,   0 }, /* PD22            */
	/* PD21 */ { 0,          0,   0,   0,   0,   0 }, /* PD21            */
	/* PD20 */ { 0,          0,   0,   0,   0,   0 }, /* PD20            */
	/* PD19 */ { 0,          0,   0,   0,   0,   0 }, /* PD19            */
	/* PD18 */ { 0,          0,   0,   0,   0,   0 }, /* PD18            */
	/* PD17 */ { 0,          0,   0,   0,   0,   0 }, /* PD17            */
	/* PD16 */ { 0,          0,   0,   0,   0,   0 }, /* PD16            */
	/* PD15 */ { 0,          0,   0,   0,   0,   0 }, /* PD15            */
	/* PD14 */ { 0,          0,   0,   0,   0,   0 }, /* PD14            */
	/* PD13 */ { 0,          0,   0,   0,   0,   0 }, /* PD13            */
	/* PD12 */ { 0,          0,   0,   0,   0,   0 }, /* PD12            */
	/* PD11 */ { 0,          0,   0,   0,   0,   0 }, /* PD11            */
	/* PD10 */ { 0,          0,   0,   0,   0,   0 }, /* PD10            */
	/* PD9  */ { 0,          0,   0,   0,   0,   0 }, /* PD9             */
	/* PD8  */ { 0,          0,   0,   0,   0,   0 }, /* PD8             */
	/* PD7  */ { 0,          0,   0,   0,   0,   0 }, /* PD7             */
	/* PD6  */ { 0,          0,   0,   0,   0,   0 }, /* PD6             */
	/* PD5  */ { 0,          0,   0,   0,   0,   0 }, /* PD5             */
	/* PD4  */ { 0,          0,   0,   0,   0,   0 }, /* PD4             */
	/* PD3  */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PD2  */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PD1  */ { 0,          0,   0,   0,   0,   0 }, /* non-existent    */
	/* PD0  */ { 0,          0,   0,   0,   0,   0 }  /* non-existent    */
    }
};

phys_size_t initdram(int board_type)
{
	long int msize = CONFIG_SYS_SDRAM_SIZE;

#ifndef CONFIG_SYS_RAMBOOT
	volatile immap_t *immap = (immap_t *)CONFIG_SYS_IMMR;
	volatile memctl8260_t *memctl = &immap->im_memctl;
	vu_char *ramaddr = (vu_char *)CONFIG_SYS_SDRAM_BASE;
	uchar c = 0xFF;
	uint psdmr = CONFIG_SYS_PSDMR;
	int i;

	immap->im_siu_conf.sc_ppc_acr  = 0x02;
	immap->im_siu_conf.sc_ppc_alrh = 0x30126745;
	immap->im_siu_conf.sc_tescr1   = 0x00004000;

	memctl->memc_mptpr = CONFIG_SYS_MPTPR;

	/* Initialise 60x bus SDRAM */
	memctl->memc_psrt = CONFIG_SYS_PSRT;
	memctl->memc_or1  = CONFIG_SYS_SDRAM_OR;
	memctl->memc_br1  = CONFIG_SYS_SDRAM_BR;
	memctl->memc_psdmr = psdmr | PSDMR_OP_PREA; /* Precharge all banks */
	*ramaddr = c;
	memctl->memc_psdmr = psdmr | PSDMR_OP_CBRR; /* CBR refresh */
	for (i = 0; i < 8; i++)
		*ramaddr = c;
	memctl->memc_psdmr = psdmr | PSDMR_OP_MRW;  /* Mode Register write */
	*ramaddr = c;
	memctl->memc_psdmr = psdmr | PSDMR_RFEN;    /* Refresh enable */
	*ramaddr = c;
#endif /* !CONFIG_SYS_RAMBOOT */

	/* Return total 60x bus SDRAM size */
	return msize * 1024 * 1024;
}

int checkboard(void)
{
	vu_char *bcsr = (vu_char *)CONFIG_SYS_BCSR;

	printf("Board: Rattler Rev. %c\n", bcsr[0x20] + 0x40);
	return 0;
}
