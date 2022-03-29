/*
 * (C) Copyright 2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
 *
 */

#include <common.h>
#include <ioports.h>
#include <mpc83xx.h>
#include <asm/mpc8349_pci.h>
#include <i2c.h>
#include <spi.h>
#include <miiphy.h>
#ifdef CONFIG_FSL_DDR2
#include <asm/fsl_ddr_sdram.h>
#else
#include <spd_sdram.h>
#endif

#if defined(CONFIG_OF_LIBFDT)
#include <libfdt.h>
#endif

int fixed_sdram(void);
void sdram_init(void);

#if defined(CONFIG_DDR_ECC) && defined(CONFIG_MPC83xx)
void ddr_enable_ecc(unsigned int dram_size);
#endif

int board_early_init_f (void)
{
	volatile u8* bcsr = (volatile u8*)CONFIG_SYS_BCSR;

	/* Enable flash write */
	bcsr[1] &= ~0x01;

#ifdef CONFIG_SYS_USE_MPC834XSYS_USB_PHY
	/* Use USB PHY on SYS board */
	bcsr[5] |= 0x02;
#endif

	return 0;
}

#define ns2clk(ns) (ns / (1000000000 / CONFIG_8349_CLKIN) + 1)

phys_size_t initdram (int board_type)
{
	volatile immap_t *im = (immap_t *)CONFIG_SYS_IMMR;
	phys_size_t msize = 0;

	if ((im->sysconf.immrbar & IMMRBAR_BASE_ADDR) != (u32)im)
		return -1;

	/* DDR SDRAM - Main SODIMM */
	im->sysconf.ddrlaw[0].bar = CONFIG_SYS_DDR_BASE & LAWBAR_BAR;
#if defined(CONFIG_SPD_EEPROM)
#ifndef CONFIG_FSL_DDR2
	msize = spd_sdram() * 1024 * 1024;
#if defined(CONFIG_DDR_ECC) && !defined(CONFIG_ECC_INIT_VIA_DDRCONTROLLER)
	ddr_enable_ecc(msize);
#endif
#else
	msize = fsl_ddr_sdram();
#endif
#else
	msize = fixed_sdram() * 1024 * 1024;
#endif
	/*
	 * Initialize SDRAM if it is on local bus.
	 */
	sdram_init();

	/* return total bus SDRAM size(bytes)  -- DDR */
	return msize;
}

#if !defined(CONFIG_SPD_EEPROM)
/*************************************************************************
 *  fixed sdram init -- doesn't use serial presence detect.
 ************************************************************************/
int fixed_sdram(void)
{
	volatile immap_t *im = (immap_t *)CONFIG_SYS_IMMR;
	u32 msize = CONFIG_SYS_DDR_SIZE;
	u32 ddr_size = msize << 20;	/* DDR size in bytes */
	u32 ddr_size_log2 = __ilog2(ddr_size);

	im->sysconf.ddrlaw[0].bar = CONFIG_SYS_DDR_SDRAM_BASE & 0xfffff000;
	im->sysconf.ddrlaw[0].ar = LAWAR_EN | ((ddr_size_log2 - 1) & LAWAR_SIZE);

#if (CONFIG_SYS_DDR_SIZE != 256)
#warning Currenly any ddr size other than 256 is not supported
#endif
#ifdef CONFIG_DDR_II
	im->ddr.csbnds[2].csbnds = CONFIG_SYS_DDR_CS2_BNDS;
	im->ddr.cs_config[2] = CONFIG_SYS_DDR_CS2_CONFIG;
	im->ddr.timing_cfg_0 = CONFIG_SYS_DDR_TIMING_0;
	im->ddr.timing_cfg_1 = CONFIG_SYS_DDR_TIMING_1;
	im->ddr.timing_cfg_2 = CONFIG_SYS_DDR_TIMING_2;
	im->ddr.timing_cfg_3 = CONFIG_SYS_DDR_TIMING_3;
	im->ddr.sdram_cfg = CONFIG_SYS_DDR_SDRAM_CFG;
	im->ddr.sdram_cfg2 = CONFIG_SYS_DDR_SDRAM_CFG2;
	im->ddr.sdram_mode = CONFIG_SYS_DDR_MODE;
	im->ddr.sdram_mode2 = CONFIG_SYS_DDR_MODE2;
	im->ddr.sdram_interval = CONFIG_SYS_DDR_INTERVAL;
	im->ddr.sdram_clk_cntl = CONFIG_SYS_DDR_CLK_CNTL;
#else

#if ((CONFIG_SYS_DDR_SDRAM_BASE & 0x00FFFFFF) != 0)
#warning Chip select bounds is only configurable in 16MB increments
#endif
	im->ddr.csbnds[2].csbnds =
		((CONFIG_SYS_DDR_SDRAM_BASE >> CSBNDS_SA_SHIFT) & CSBNDS_SA) |
		(((CONFIG_SYS_DDR_SDRAM_BASE + ddr_size - 1) >>
				CSBNDS_EA_SHIFT) & CSBNDS_EA);
	im->ddr.cs_config[2] = CONFIG_SYS_DDR_CS2_CONFIG;

	/* currently we use only one CS, so disable the other banks */
	im->ddr.cs_config[0] = 0;
	im->ddr.cs_config[1] = 0;
	im->ddr.cs_config[3] = 0;

	im->ddr.timing_cfg_1 = CONFIG_SYS_DDR_TIMING_1;
	im->ddr.timing_cfg_2 = CONFIG_SYS_DDR_TIMING_2;

	im->ddr.sdram_cfg =
		SDRAM_CFG_SREN
#if defined(CONFIG_DDR_2T_TIMING)
		| SDRAM_CFG_2T_EN
#endif
		| 2 << SDRAM_CFG_SDRAM_TYPE_SHIFT;
#if defined (CONFIG_DDR_32BIT)
	/* for 32-bit mode burst length is 8 */
	im->ddr.sdram_cfg |= (SDRAM_CFG_32_BE | SDRAM_CFG_8_BE);
#endif
	im->ddr.sdram_mode = CONFIG_SYS_DDR_MODE;

	im->ddr.sdram_interval = CONFIG_SYS_DDR_INTERVAL;
#endif
	udelay(200);

	/* enable DDR controller */
	im->ddr.sdram_cfg |= SDRAM_CFG_MEM_EN;
	return msize;
}
#endif/*!CONFIG_SYS_SPD_EEPROM*/


int checkboard (void)
{
	/*
	 * Warning: do not read the BCSR registers here
	 *
	 * There is a timing bug in the 8349E and 8349EA BCSR code
	 * version 1.2 (read from BCSR 11) that will cause the CFI
	 * flash initialization code to overwrite BCSR 0, disabling
	 * the serial ports and gigabit ethernet
	 */

	puts("Board: Freescale MPC8349EMDS\n");
	return 0;
}

/*
 * if MPC8349EMDS is soldered with SDRAM
 */
#if defined(CONFIG_SYS_BR2_PRELIM)  \
	&& defined(CONFIG_SYS_OR2_PRELIM) \
	&& defined(CONFIG_SYS_LBLAWBAR2_PRELIM) \
	&& defined(CONFIG_SYS_LBLAWAR2_PRELIM)
/*
 * Initialize SDRAM memory on the Local Bus.
 */

void sdram_init(void)
{
	volatile immap_t *immap = (immap_t *)CONFIG_SYS_IMMR;
	volatile fsl_lbc_t *lbc = &immap->im_lbc;
	uint *sdram_addr = (uint *)CONFIG_SYS_LBC_SDRAM_BASE;

	/*
	 * Setup SDRAM Base and Option Registers, already done in cpu_init.c
	 */

	/* setup mtrpt, lsrt and lbcr for LB bus */
	lbc->lbcr = CONFIG_SYS_LBC_LBCR;
	lbc->mrtpr = CONFIG_SYS_LBC_MRTPR;
	lbc->lsrt = CONFIG_SYS_LBC_LSRT;
	asm("sync");

	/*
	 * Configure the SDRAM controller Machine Mode Register.
	 */
	lbc->lsdmr = CONFIG_SYS_LBC_LSDMR_5; /* 0x40636733; normal operation */

	lbc->lsdmr = CONFIG_SYS_LBC_LSDMR_1; /* 0x68636733; precharge all the banks */
	asm("sync");
	*sdram_addr = 0xff;
	udelay(100);

	lbc->lsdmr = CONFIG_SYS_LBC_LSDMR_2; /* 0x48636733; auto refresh */
	asm("sync");
	/*1 times*/
	*sdram_addr = 0xff;
	udelay(100);
	/*2 times*/
	*sdram_addr = 0xff;
	udelay(100);
	/*3 times*/
	*sdram_addr = 0xff;
	udelay(100);
	/*4 times*/
	*sdram_addr = 0xff;
	udelay(100);
	/*5 times*/
	*sdram_addr = 0xff;
	udelay(100);
	/*6 times*/
	*sdram_addr = 0xff;
	udelay(100);
	/*7 times*/
	*sdram_addr = 0xff;
	udelay(100);
	/*8 times*/
	*sdram_addr = 0xff;
	udelay(100);

	/* 0x58636733; mode register write operation */
	lbc->lsdmr = CONFIG_SYS_LBC_LSDMR_4;
	asm("sync");
	*sdram_addr = 0xff;
	udelay(100);

	lbc->lsdmr = CONFIG_SYS_LBC_LSDMR_5; /* 0x40636733; normal operation */
	asm("sync");
	*sdram_addr = 0xff;
	udelay(100);
}
#else
void sdram_init(void)
{
}
#endif

/*
 * The following are used to control the SPI chip selects for the SPI command.
 */
#ifdef CONFIG_MPC8XXX_SPI

#define SPI_CS_MASK	0x80000000

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return bus == 0 && cs == 0;
}

void spi_cs_activate(struct spi_slave *slave)
{
	volatile gpio83xx_t *iopd = &((immap_t *)CONFIG_SYS_IMMR)->gpio[0];

	iopd->dat &= ~SPI_CS_MASK;
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	volatile gpio83xx_t *iopd = &((immap_t *)CONFIG_SYS_IMMR)->gpio[0];

	iopd->dat |=  SPI_CS_MASK;
}
#endif /* CONFIG_HARD_SPI */

#if defined(CONFIG_OF_BOARD_SETUP)
void ft_board_setup(void *blob, bd_t *bd)
{
	ft_cpu_setup(blob, bd);
#ifdef CONFIG_PCI
	ft_pci_setup(blob, bd);
#endif
}
#endif
