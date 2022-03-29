/*
 * (C) Copyright 2007
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
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
#include <asm/processor.h>

extern void board_pll_init_f(void);

static void acadia_gpio_init(void)
{
	/*
	 * GPIO0 setup (select GPIO or alternate function)
	 */
	out32(GPIO0_OSRL, CONFIG_SYS_GPIO0_OSRL);
	out32(GPIO0_OSRH, CONFIG_SYS_GPIO0_OSRH);	/* output select */
	out32(GPIO0_ISR1L, CONFIG_SYS_GPIO0_ISR1L);
	out32(GPIO0_ISR1H, CONFIG_SYS_GPIO0_ISR1H);	/* input select */
	out32(GPIO0_TSRL, CONFIG_SYS_GPIO0_TSRL);
	out32(GPIO0_TSRH, CONFIG_SYS_GPIO0_TSRH);	/* three-state select */
	out32(GPIO0_TCR, CONFIG_SYS_GPIO0_TCR);  /* enable output driver for outputs */

	/*
	 * Ultra (405EZ) was nice enough to add another GPIO controller
	 */
	out32(GPIO1_OSRH, CONFIG_SYS_GPIO1_OSRH);	/* output select */
	out32(GPIO1_OSRL, CONFIG_SYS_GPIO1_OSRL);
	out32(GPIO1_ISR1H, CONFIG_SYS_GPIO1_ISR1H);	/* input select */
	out32(GPIO1_ISR1L, CONFIG_SYS_GPIO1_ISR1L);
	out32(GPIO1_TSRH, CONFIG_SYS_GPIO1_TSRH);	/* three-state select */
	out32(GPIO1_TSRL, CONFIG_SYS_GPIO1_TSRL);
	out32(GPIO1_TCR, CONFIG_SYS_GPIO1_TCR);  /* enable output driver for outputs */
}

int board_early_init_f(void)
{
	unsigned int reg;

#if !defined(CONFIG_NAND_U_BOOT)
	/* don't reinit PLL when booting via I2C bootstrap option */
	mfsdr(SDR0_PINSTP, reg);
	if (reg != 0xf0000000)
		board_pll_init_f();
#endif

	acadia_gpio_init();

	/* Configure 405EZ for NAND usage */
	mtsdr(SDR0_NAND0, SDR_NAND0_NDEN | SDR_NAND0_NDAREN | SDR_NAND0_NDRBEN);
	mfsdr(SDR0_ULTRA0, reg);
	reg &= ~SDR_ULTRA0_CSN_MASK;
	reg |= (SDR_ULTRA0_CSNSEL0 >> CONFIG_SYS_NAND_CS) |
		SDR_ULTRA0_NDGPIOBP |
		SDR_ULTRA0_EBCRDYEN |
		SDR_ULTRA0_NFSRSTEN;
	mtsdr(SDR0_ULTRA0, reg);

	/* USB Host core needs this bit set */
	mfsdr(SDR0_ULTRA1, reg);
	mtsdr(SDR0_ULTRA1, reg | SDR_ULTRA1_LEDNENABLE);

	mtdcr(UIC0SR, 0xFFFFFFFF);	/* clear all ints */
	mtdcr(UIC0ER, 0x00000000);	/* disable all ints */
	mtdcr(UIC0CR, 0x00000010);
	mtdcr(UIC0PR, 0xFE7FFFF0);	/* set int polarities */
	mtdcr(UIC0TR, 0x00000010);	/* set int trigger levels */
	mtdcr(UIC0SR, 0xFFFFFFFF);	/* clear all ints */

	return 0;
}

int misc_init_f(void)
{
	/* Set EPLD to take PHY out of reset */
	out8(CONFIG_SYS_CPLD_BASE + 0x05, 0x00);
	udelay(100000);

	return 0;
}

/*
 * Check Board Identity:
 */
int checkboard(void)
{
	char buf[64];
	int i = getenv_f("serial#", buf, sizeof(buf));
	u8 rev;

	rev = in8(CONFIG_SYS_CPLD_BASE + 0);
	printf("Board: Acadia - AMCC PPC405EZ Evaluation Board, Rev. %X", rev);

	if (i > 0) {
		puts(", serial# ");
		puts(buf);
	}
	putc('\n');

	return (0);
}
