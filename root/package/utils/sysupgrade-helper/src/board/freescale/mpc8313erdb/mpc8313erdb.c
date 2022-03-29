/*
 * Copyright (C) Freescale Semiconductor, Inc. 2006-2007
 *
 * Author: Scott Wood <scottwood@freescale.com>
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
 * MERCHANTABILITY or FITNESS for A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#if defined(CONFIG_OF_LIBFDT)
#include <libfdt.h>
#endif
#include <pci.h>
#include <mpc83xx.h>
#include <vsc7385.h>
#include <ns16550.h>
#include <nand.h>
#if defined(CONFIG_MPC83XX_GPIO) && !defined(CONFIG_NAND_SPL)
#include <asm/gpio.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

int board_early_init_f(void)
{
#ifndef CONFIG_SYS_8313ERDB_BROKEN_PMC
	volatile immap_t *im = (immap_t *)CONFIG_SYS_IMMR;

	if (im->pmc.pmccr1 & PMCCR1_POWER_OFF)
		gd->flags |= GD_FLG_SILENT;
#endif
#if defined(CONFIG_MPC83XX_GPIO) && !defined(CONFIG_NAND_SPL)
	mpc83xx_gpio_init_f();
#endif

	return 0;
}

int board_early_init_r(void)
{
#if defined(CONFIG_MPC83XX_GPIO) && !defined(CONFIG_NAND_SPL)
	mpc83xx_gpio_init_r();
#endif

	return 0;
}

int checkboard(void)
{
	puts("Board: Freescale MPC8313ERDB\n");
	return 0;
}

#ifndef CONFIG_NAND_SPL
static struct pci_region pci_regions[] = {
	{
		bus_start: CONFIG_SYS_PCI1_MEM_BASE,
		phys_start: CONFIG_SYS_PCI1_MEM_PHYS,
		size: CONFIG_SYS_PCI1_MEM_SIZE,
		flags: PCI_REGION_MEM | PCI_REGION_PREFETCH
	},
	{
		bus_start: CONFIG_SYS_PCI1_MMIO_BASE,
		phys_start: CONFIG_SYS_PCI1_MMIO_PHYS,
		size: CONFIG_SYS_PCI1_MMIO_SIZE,
		flags: PCI_REGION_MEM
	},
	{
		bus_start: CONFIG_SYS_PCI1_IO_BASE,
		phys_start: CONFIG_SYS_PCI1_IO_PHYS,
		size: CONFIG_SYS_PCI1_IO_SIZE,
		flags: PCI_REGION_IO
	}
};

void pci_init_board(void)
{
	volatile immap_t *immr = (volatile immap_t *)CONFIG_SYS_IMMR;
	volatile clk83xx_t *clk = (volatile clk83xx_t *)&immr->clk;
	volatile law83xx_t *pci_law = immr->sysconf.pcilaw;
	struct pci_region *reg[] = { pci_regions };

	/* Enable all 3 PCI_CLK_OUTPUTs. */
	clk->occr |= 0xe0000000;

	/*
	 * Configure PCI Local Access Windows
	 */
	pci_law[0].bar = CONFIG_SYS_PCI1_MEM_PHYS & LAWBAR_BAR;
	pci_law[0].ar = LBLAWAR_EN | LBLAWAR_512MB;

	pci_law[1].bar = CONFIG_SYS_PCI1_IO_PHYS & LAWBAR_BAR;
	pci_law[1].ar = LBLAWAR_EN | LBLAWAR_1MB;

	mpc83xx_pci_init(1, reg);
}

/*
 * Miscellaneous late-boot configurations
 *
 * If a VSC7385 microcode image is present, then upload it.
*/
int misc_init_r(void)
{
	int rc = 0;

#ifdef CONFIG_VSC7385_IMAGE
	if (vsc7385_upload_firmware((void *) CONFIG_VSC7385_IMAGE,
		CONFIG_VSC7385_IMAGE_SIZE)) {
		puts("Failure uploading VSC7385 microcode.\n");
		rc = 1;
	}
#endif

	return rc;
}

#if defined(CONFIG_OF_BOARD_SETUP)
void ft_board_setup(void *blob, bd_t *bd)
{
	ft_cpu_setup(blob, bd);
#ifdef CONFIG_PCI
	ft_pci_setup(blob, bd);
#endif
}
#endif
#else /* CONFIG_NAND_SPL */
void board_init_f(ulong bootflag)
{
	board_early_init_f();
	NS16550_init((NS16550_t)(CONFIG_SYS_IMMR + 0x4500),
	             CONFIG_SYS_NS16550_CLK / 16 / CONFIG_BAUDRATE);
	puts("NAND boot... ");
	init_timebase();
	initdram(0);
	relocate_code(CONFIG_SYS_NAND_U_BOOT_RELOC_SP, (gd_t *)gd,
	              CONFIG_SYS_NAND_U_BOOT_RELOC);
}

void board_init_r(gd_t *gd, ulong dest_addr)
{
	nand_boot();
}

void putc(char c)
{
	if (gd->flags & GD_FLG_SILENT)
		return;

	if (c == '\n')
		NS16550_putc((NS16550_t)(CONFIG_SYS_IMMR + 0x4500), '\r');

	NS16550_putc((NS16550_t)(CONFIG_SYS_IMMR + 0x4500), c);
}
#endif
