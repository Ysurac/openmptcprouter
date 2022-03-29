/*
 * Copyright 2004 Freescale Semiconductor.
 * (C) Copyright 2003,Motorola Inc.
 * Xianghua Xiao, (X.Xiao@motorola.com)
 *
 * (C) Copyright 2002 Scott McNutt <smcnutt@artesyncp.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */


#include <common.h>
#include <pci.h>
#include <asm/processor.h>
#include <asm/mmu.h>
#include <asm/immap_85xx.h>
#include <asm/fsl_ddr_sdram.h>
#include <ioports.h>
#include <spd_sdram.h>
#include <miiphy.h>
#include <libfdt.h>
#include <fdt_support.h>
#include <asm/fsl_lbc.h>

#if defined(CONFIG_DDR_ECC) && !defined(CONFIG_ECC_INIT_VIA_DDRCONTROLLER)
extern void ddr_enable_ecc(unsigned int dram_size);
#endif


void local_bus_init(void);


/*
 * I/O Port configuration table
 *
 * if conf is 1, then that port pin will be configured at boot time
 * according to the five values podr/pdir/ppar/psor/pdat for that entry
 */

const iop_conf_t iop_conf_tab[4][32] = {

    /* Port A configuration */
    {   /*            conf ppar psor pdir podr pdat */
	/* PA31 */ {   0,   1,   0,   1,   0,   0   }, /* FCC1 TxENB */
	/* PA30 */ {   0,   1,   0,   0,   0,   0   }, /* FCC1 TxClav   */
	/* PA29 */ {   0,   1,   0,   1,   0,   0   }, /* FCC1 TxSOC  */
	/* PA28 */ {   0,   1,   0,   1,   0,   0   }, /* FCC1 RxENB */
	/* PA27 */ {   0,   1,   0,   0,   0,   0   }, /* FCC1 RxSOC */
	/* PA26 */ {   0,   1,   0,   0,   0,   0   }, /* FCC1 RxClav */
	/* PA25 */ {   0,   1,   0,   1,   0,   0   }, /* FCC1 ATMTXD[0] */
	/* PA24 */ {   0,   1,   0,   1,   0,   0   }, /* FCC1 ATMTXD[1] */
	/* PA23 */ {   0,   1,   0,   1,   0,   0   }, /* FCC1 ATMTXD[2] */
	/* PA22 */ {   0,   1,   0,   1,   0,   0   }, /* FCC1 ATMTXD[3] */
	/* PA21 */ {   0,   1,   0,   1,   0,   0   }, /* FCC1 ATMTXD[4] */
	/* PA20 */ {   0,   1,   0,   1,   0,   0   }, /* FCC1 ATMTXD[5] */
	/* PA19 */ {   0,   1,   0,   1,   0,   0   }, /* FCC1 ATMTXD[6] */
	/* PA18 */ {   0,   1,   0,   1,   0,   0   }, /* FCC1 ATMTXD[7] */
	/* PA17 */ {   0,   1,   0,   0,   0,   0   }, /* FCC1 ATMRXD[7] */
	/* PA16 */ {   0,   1,   0,   0,   0,   0   }, /* FCC1 ATMRXD[6] */
	/* PA15 */ {   0,   1,   0,   0,   0,   0   }, /* FCC1 ATMRXD[5] */
	/* PA14 */ {   0,   1,   0,   0,   0,   0   }, /* FCC1 ATMRXD[4] */
	/* PA13 */ {   0,   1,   0,   0,   0,   0   }, /* FCC1 ATMRXD[3] */
	/* PA12 */ {   0,   1,   0,   0,   0,   0   }, /* FCC1 ATMRXD[2] */
	/* PA11 */ {   0,   1,   0,   0,   0,   0   }, /* FCC1 ATMRXD[1] */
	/* PA10 */ {   0,   1,   0,   0,   0,   0   }, /* FCC1 ATMRXD[0] */
	/* PA9  */ {   0,   1,   1,   1,   0,   0   }, /* FCC1 L1TXD */
	/* PA8  */ {   0,   1,   1,   0,   0,   0   }, /* FCC1 L1RXD */
	/* PA7  */ {   0,   0,   0,   1,   0,   0   }, /* PA7 */
	/* PA6  */ {   0,   1,   1,   1,   0,   0   }, /* TDM A1 L1RSYNC */
	/* PA5  */ {   0,   0,   0,   1,   0,   0   }, /* PA5 */
	/* PA4  */ {   0,   0,   0,   1,   0,   0   }, /* PA4 */
	/* PA3  */ {   0,   0,   0,   1,   0,   0   }, /* PA3 */
	/* PA2  */ {   0,   0,   0,   1,   0,   0   }, /* PA2 */
	/* PA1  */ {   1,   0,   0,   0,   0,   0   }, /* FREERUN */
	/* PA0  */ {   0,   0,   0,   1,   0,   0   }  /* PA0 */
    },

    /* Port B configuration */
    {   /*            conf ppar psor pdir podr pdat */
	/* PB31 */ {   1,   1,   0,   1,   0,   0   }, /* FCC2 MII TX_ER */
	/* PB30 */ {   1,   1,   0,   0,   0,   0   }, /* FCC2 MII RX_DV */
	/* PB29 */ {   1,   1,   1,   1,   0,   0   }, /* FCC2 MII TX_EN */
	/* PB28 */ {   1,   1,   0,   0,   0,   0   }, /* FCC2 MII RX_ER */
	/* PB27 */ {   1,   1,   0,   0,   0,   0   }, /* FCC2 MII COL */
	/* PB26 */ {   1,   1,   0,   0,   0,   0   }, /* FCC2 MII CRS */
	/* PB25 */ {   1,   1,   0,   1,   0,   0   }, /* FCC2 MII TxD[3] */
	/* PB24 */ {   1,   1,   0,   1,   0,   0   }, /* FCC2 MII TxD[2] */
	/* PB23 */ {   1,   1,   0,   1,   0,   0   }, /* FCC2 MII TxD[1] */
	/* PB22 */ {   1,   1,   0,   1,   0,   0   }, /* FCC2 MII TxD[0] */
	/* PB21 */ {   1,   1,   0,   0,   0,   0   }, /* FCC2 MII RxD[0] */
	/* PB20 */ {   1,   1,   0,   0,   0,   0   }, /* FCC2 MII RxD[1] */
	/* PB19 */ {   1,   1,   0,   0,   0,   0   }, /* FCC2 MII RxD[2] */
	/* PB18 */ {   1,   1,   0,   0,   0,   0   }, /* FCC2 MII RxD[3] */
	/* PB17 */ {   0,   1,   0,   0,   0,   0   }, /* FCC3:RX_DIV */
	/* PB16 */ {   0,   1,   0,   0,   0,   0   }, /* FCC3:RX_ERR */
	/* PB15 */ {   0,   1,   0,   1,   0,   0   }, /* FCC3:TX_ERR */
	/* PB14 */ {   0,   1,   0,   1,   0,   0   }, /* FCC3:TX_EN */
	/* PB13 */ {   0,   1,   0,   0,   0,   0   }, /* FCC3:COL */
	/* PB12 */ {   0,   1,   0,   0,   0,   0   }, /* FCC3:CRS */
	/* PB11 */ {   0,   1,   0,   0,   0,   0   }, /* FCC3:RXD */
	/* PB10 */ {   0,   1,   0,   0,   0,   0   }, /* FCC3:RXD */
	/* PB9  */ {   0,   1,   0,   0,   0,   0   }, /* FCC3:RXD */
	/* PB8  */ {   0,   1,   0,   0,   0,   0   }, /* FCC3:RXD */
	/* PB7  */ {   0,   1,   0,   1,   0,   0   }, /* FCC3:TXD */
	/* PB6  */ {   0,   1,   0,   1,   0,   0   }, /* FCC3:TXD */
	/* PB5  */ {   0,   1,   0,   1,   0,   0   }, /* FCC3:TXD */
	/* PB4  */ {   0,   1,   0,   1,   0,   0   }, /* FCC3:TXD */
	/* PB3  */ {   0,   0,   0,   0,   0,   0   }, /* pin doesn't exist */
	/* PB2  */ {   0,   0,   0,   0,   0,   0   }, /* pin doesn't exist */
	/* PB1  */ {   0,   0,   0,   0,   0,   0   }, /* pin doesn't exist */
	/* PB0  */ {   0,   0,   0,   0,   0,   0   }  /* pin doesn't exist */
    },

    /* Port C */
    {   /*            conf ppar psor pdir podr pdat */
	/* PC31 */ {   0,   0,   0,   1,   0,   0   }, /* PC31 */
	/* PC30 */ {   0,   0,   0,   1,   0,   0   }, /* PC30 */
	/* PC29 */ {   0,   1,   1,   0,   0,   0   }, /* SCC1 EN *CLSN */
	/* PC28 */ {   0,   0,   0,   1,   0,   0   }, /* PC28 */
	/* PC27 */ {   0,   0,   0,   1,   0,   0   }, /* UART Clock in */
	/* PC26 */ {   0,   0,   0,   1,   0,   0   }, /* PC26 */
	/* PC25 */ {   0,   0,   0,   1,   0,   0   }, /* PC25 */
	/* PC24 */ {   0,   0,   0,   1,   0,   0   }, /* PC24 */
	/* PC23 */ {   0,   1,   0,   1,   0,   0   }, /* ATMTFCLK */
	/* PC22 */ {   0,   1,   0,   0,   0,   0   }, /* ATMRFCLK */
	/* PC21 */ {   0,   1,   0,   0,   0,   0   }, /* SCC1 EN RXCLK */
	/* PC20 */ {   0,   1,   0,   0,   0,   0   }, /* SCC1 EN TXCLK */
	/* PC19 */ {   1,   1,   0,   0,   0,   0   }, /* FCC2 MII RX_CLK CLK13 */
	/* PC18 */ {   1,   1,   0,   0,   0,   0   }, /* FCC Tx Clock (CLK14) */
	/* PC17 */ {   0,   0,   0,   1,   0,   0   }, /* PC17 */
	/* PC16 */ {   0,   1,   0,   0,   0,   0   }, /* FCC Tx Clock (CLK16) */
	/* PC15 */ {   1,   1,   0,   0,   0,   0   }, /* PC15 */
	/* PC14 */ {   0,   1,   0,   0,   0,   0   }, /* SCC1 EN *CD */
	/* PC13 */ {   0,   0,   0,   1,   0,   0   }, /* PC13 */
	/* PC12 */ {   0,   1,   0,   1,   0,   0   }, /* PC12 */
	/* PC11 */ {   0,   0,   0,   1,   0,   0   }, /* LXT971 transmit control */
	/* PC10 */ {   1,   0,   0,   1,   0,   0   }, /* FETHMDC */
	/* PC9  */ {   1,   0,   0,   0,   0,   0   }, /* FETHMDIO */
	/* PC8  */ {   0,   0,   0,   1,   0,   0   }, /* PC8 */
	/* PC7  */ {   0,   0,   0,   1,   0,   0   }, /* PC7 */
	/* PC6  */ {   0,   0,   0,   1,   0,   0   }, /* PC6 */
	/* PC5  */ {   0,   0,   0,   1,   0,   0   }, /* PC5 */
	/* PC4  */ {   0,   0,   0,   1,   0,   0   }, /* PC4 */
	/* PC3  */ {   0,   0,   0,   1,   0,   0   }, /* PC3 */
	/* PC2  */ {   0,   0,   0,   1,   0,   1   }, /* ENET FDE */
	/* PC1  */ {   0,   0,   0,   1,   0,   0   }, /* ENET DSQE */
	/* PC0  */ {   0,   0,   0,   1,   0,   0   }, /* ENET LBK */
    },

    /* Port D */
    {   /*            conf ppar psor pdir podr pdat */
	/* PD31 */ {   1,   1,   0,   0,   0,   0   }, /* SCC1 EN RxD */
	/* PD30 */ {   1,   1,   1,   1,   0,   0   }, /* SCC1 EN TxD */
	/* PD29 */ {   1,   1,   0,   1,   0,   0   }, /* SCC1 EN TENA */
	/* PD28 */ {   0,   1,   0,   0,   0,   0   }, /* PD28 */
	/* PD27 */ {   0,   1,   1,   1,   0,   0   }, /* PD27 */
	/* PD26 */ {   0,   0,   0,   1,   0,   0   }, /* PD26 */
	/* PD25 */ {   0,   0,   0,   1,   0,   0   }, /* PD25 */
	/* PD24 */ {   0,   0,   0,   1,   0,   0   }, /* PD24 */
	/* PD23 */ {   0,   0,   0,   1,   0,   0   }, /* PD23 */
	/* PD22 */ {   0,   0,   0,   1,   0,   0   }, /* PD22 */
	/* PD21 */ {   0,   0,   0,   1,   0,   0   }, /* PD21 */
	/* PD20 */ {   0,   0,   0,   1,   0,   0   }, /* PD20 */
	/* PD19 */ {   0,   0,   0,   1,   0,   0   }, /* PD19 */
	/* PD18 */ {   0,   0,   0,   1,   0,   0   }, /* PD18 */
	/* PD17 */ {   0,   1,   0,   0,   0,   0   }, /* FCC1 ATMRXPRTY */
	/* PD16 */ {   0,   1,   0,   1,   0,   0   }, /* FCC1 ATMTXPRTY */
	/* PD15 */ {   0,   1,   1,   0,   1,   0   }, /* I2C SDA */
	/* PD14 */ {   0,   0,   0,   1,   0,   0   }, /* LED */
	/* PD13 */ {   0,   0,   0,   0,   0,   0   }, /* PD13 */
	/* PD12 */ {   0,   0,   0,   0,   0,   0   }, /* PD12 */
	/* PD11 */ {   0,   0,   0,   0,   0,   0   }, /* PD11 */
	/* PD10 */ {   0,   0,   0,   0,   0,   0   }, /* PD10 */
	/* PD9  */ {   0,   1,   0,   1,   0,   0   }, /* SMC1 TXD */
	/* PD8  */ {   0,   1,   0,   0,   0,   0   }, /* SMC1 RXD */
	/* PD7  */ {   0,   0,   0,   1,   0,   1   }, /* PD7 */
	/* PD6  */ {   0,   0,   0,   1,   0,   1   }, /* PD6 */
	/* PD5  */ {   0,   0,   0,   1,   0,   1   }, /* PD5 */
	/* PD4  */ {   0,   0,   0,   1,   0,   1   }, /* PD4 */
	/* PD3  */ {   0,   0,   0,   0,   0,   0   }, /* pin doesn't exist */
	/* PD2  */ {   0,   0,   0,   0,   0,   0   }, /* pin doesn't exist */
	/* PD1  */ {   0,   0,   0,   0,   0,   0   }, /* pin doesn't exist */
	/* PD0  */ {   0,   0,   0,   0,   0,   0   }  /* pin doesn't exist */
    }
};


/*
 * MPC8560ADS Board Status & Control Registers
 */
typedef struct bcsr_ {
	volatile unsigned char bcsr0;
	volatile unsigned char bcsr1;
	volatile unsigned char bcsr2;
	volatile unsigned char bcsr3;
	volatile unsigned char bcsr4;
	volatile unsigned char bcsr5;
} bcsr_t;

void reset_phy (void)
{
#if defined(CONFIG_ETHER_ON_FCC) /* avoid compile warnings for now */
	volatile bcsr_t *bcsr = (bcsr_t *) CONFIG_SYS_BCSR;
#endif
	/* reset Giga bit Ethernet port if needed here */

	/* reset the CPM FEC port */
#if (CONFIG_ETHER_INDEX == 2)
	bcsr->bcsr2 &= ~FETH2_RST;
	udelay(2);
	bcsr->bcsr2 |=  FETH2_RST;
	udelay(1000);
#elif (CONFIG_ETHER_INDEX == 3)
	bcsr->bcsr3 &= ~FETH3_RST;
	udelay(2);
	bcsr->bcsr3 |=  FETH3_RST;
	udelay(1000);
#endif
#if defined(CONFIG_MII) && defined(CONFIG_ETHER_ON_FCC)
	/* reset PHY */
	miiphy_reset("FCC1", 0x0);

	/* change PHY address to 0x02 */
	bb_miiphy_write(NULL, 0, MII_MIPSCR, 0xf028);

	bb_miiphy_write(NULL, 0x02, MII_BMCR,
			BMCR_ANENABLE | BMCR_ANRESTART);
#endif /* CONFIG_MII */
}


int checkboard (void)
{
	puts("Board: ADS\n");

#ifdef CONFIG_PCI
	printf("PCI1: 32 bit, %d MHz (compiled)\n",
	       CONFIG_SYS_CLK_FREQ / 1000000);
#else
	printf("PCI1: disabled\n");
#endif

	/*
	 * Initialize local bus.
	 */
	local_bus_init();

	return 0;
}

/*
 * Initialize Local Bus
 */

void
local_bus_init(void)
{
	volatile ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);
	volatile fsl_lbc_t *lbc = LBC_BASE_ADDR;

	uint clkdiv;
	uint lbc_hz;
	sys_info_t sysinfo;

	/*
	 * Errata LBC11.
	 * Fix Local Bus clock glitch when DLL is enabled.
	 *
	 * If localbus freq is < 66MHz, DLL bypass mode must be used.
	 * If localbus freq is > 133MHz, DLL can be safely enabled.
	 * Between 66 and 133, the DLL is enabled with an override workaround.
	 */

	get_sys_info(&sysinfo);
	clkdiv = lbc->lcrr & LCRR_CLKDIV;
	lbc_hz = sysinfo.freqSystemBus / 1000000 / clkdiv;

	if (lbc_hz < 66) {
		lbc->lcrr = CONFIG_SYS_LBC_LCRR | 0x80000000;	/* DLL Bypass */

	} else if (lbc_hz >= 133) {
		lbc->lcrr = CONFIG_SYS_LBC_LCRR & (~0x80000000); /* DLL Enabled */

	} else {
		/*
		 * On REV1 boards, need to change CLKDIV before enable DLL.
		 * Default CLKDIV is 8, change it to 4 temporarily.
		 */
		uint pvr = get_pvr();
		uint temp_lbcdll = 0;

		if (pvr == PVR_85xx_REV1) {
			/* FIXME: Justify the high bit here. */
			lbc->lcrr = 0x10000004;
		}

		lbc->lcrr = CONFIG_SYS_LBC_LCRR & (~0x80000000);/* DLL Enabled */
		udelay(200);

		/*
		 * Sample LBC DLL ctrl reg, upshift it to set the
		 * override bits.
		 */
		temp_lbcdll = gur->lbcdllcr;
		gur->lbcdllcr = (((temp_lbcdll & 0xff) << 16) | 0x80000000);
		asm("sync;isync;msync");
	}
}


/*
 * Initialize SDRAM memory on the Local Bus.
 */
void lbc_sdram_init(void)
{
	volatile fsl_lbc_t *lbc = LBC_BASE_ADDR;
	uint *sdram_addr = (uint *)CONFIG_SYS_LBC_SDRAM_BASE;

	puts("LBC SDRAM: ");
	print_size(CONFIG_SYS_LBC_SDRAM_SIZE * 1024 * 1024,
		   "\n       ");

	/*
	 * Setup SDRAM Base and Option Registers
	 */
	set_lbc_or(2, CONFIG_SYS_OR2_PRELIM);
	set_lbc_br(2, CONFIG_SYS_BR2_PRELIM);
	lbc->lbcr = CONFIG_SYS_LBC_LBCR;
	asm("msync");

	lbc->lsrt = CONFIG_SYS_LBC_LSRT;
	lbc->mrtpr = CONFIG_SYS_LBC_MRTPR;
	asm("sync");

	/*
	 * Configure the SDRAM controller.
	 */
	lbc->lsdmr = CONFIG_SYS_LBC_LSDMR_1;
	asm("sync");
	*sdram_addr = 0xff;
	ppcDcbf((unsigned long) sdram_addr);
	udelay(100);

	lbc->lsdmr = CONFIG_SYS_LBC_LSDMR_2;
	asm("sync");
	*sdram_addr = 0xff;
	ppcDcbf((unsigned long) sdram_addr);
	udelay(100);

	lbc->lsdmr = CONFIG_SYS_LBC_LSDMR_3;
	asm("sync");
	*sdram_addr = 0xff;
	ppcDcbf((unsigned long) sdram_addr);
	udelay(100);

	lbc->lsdmr = CONFIG_SYS_LBC_LSDMR_4;
	asm("sync");
	*sdram_addr = 0xff;
	ppcDcbf((unsigned long) sdram_addr);
	udelay(100);

	lbc->lsdmr = CONFIG_SYS_LBC_LSDMR_5;
	asm("sync");
	*sdram_addr = 0xff;
	ppcDcbf((unsigned long) sdram_addr);
	udelay(100);
}

#if !defined(CONFIG_SPD_EEPROM)
/*************************************************************************
 *  fixed sdram init -- doesn't use serial presence detect.
 ************************************************************************/
phys_size_t fixed_sdram(void)
{
  #ifndef CONFIG_SYS_RAMBOOT
	volatile ccsr_ddr_t *ddr= (void *)(CONFIG_SYS_MPC85xx_DDR_ADDR);

	ddr->cs0_bnds = CONFIG_SYS_DDR_CS0_BNDS;
	ddr->cs0_config = CONFIG_SYS_DDR_CS0_CONFIG;
	ddr->timing_cfg_1 = CONFIG_SYS_DDR_TIMING_1;
	ddr->timing_cfg_2 = CONFIG_SYS_DDR_TIMING_2;
	ddr->sdram_mode = CONFIG_SYS_DDR_MODE;
	ddr->sdram_interval = CONFIG_SYS_DDR_INTERVAL;
    #if defined (CONFIG_DDR_ECC)
	ddr->err_disable = 0x0000000D;
	ddr->err_sbe = 0x00ff0000;
    #endif
	asm("sync;isync;msync");
	udelay(500);
    #if defined (CONFIG_DDR_ECC)
	/* Enable ECC checking */
	ddr->sdram_cfg = (CONFIG_SYS_DDR_CONTROL | 0x20000000);
    #else
	ddr->sdram_cfg = CONFIG_SYS_DDR_CONTROL;
    #endif
	asm("sync; isync; msync");
	udelay(500);
  #endif
	return CONFIG_SYS_SDRAM_SIZE * 1024 * 1024;
}
#endif	/* !defined(CONFIG_SPD_EEPROM) */


#if defined(CONFIG_PCI)
/*
 * Initialize PCI Devices, report devices found.
 */

#ifndef CONFIG_PCI_PNP
static struct pci_config_table pci_mpc85xxads_config_table[] = {
    { PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID,
      PCI_IDSEL_NUMBER, PCI_ANY_ID,
      pci_cfgfunc_config_device, { PCI_ENET0_IOADDR,
				   PCI_ENET0_MEMADDR,
				   PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER
      } },
    { }
};
#endif


static struct pci_controller hose = {
#ifndef CONFIG_PCI_PNP
	config_table: pci_mpc85xxads_config_table,
#endif
};

#endif	/* CONFIG_PCI */


void
pci_init_board(void)
{
#ifdef CONFIG_PCI
	pci_mpc85xx_init(&hose);
#endif /* CONFIG_PCI */
}


#if defined(CONFIG_OF_BOARD_SETUP)
void
ft_board_setup(void *blob, bd_t *bd)
{
	int node, tmp[2];
	const char *path;

	ft_cpu_setup(blob, bd);

	node = fdt_path_offset(blob, "/aliases");
	tmp[0] = 0;
	if (node >= 0) {
#ifdef CONFIG_PCI
		path = fdt_getprop(blob, node, "pci0", NULL);
		if (path) {
			tmp[1] = hose.last_busno - hose.first_busno;
			do_fixup_by_path(blob, path, "bus-range", &tmp, 8, 1);
		}
#endif
	}
}
#endif
