/*
 * Copyright (C) 2007,2008
 * Nobuhiro Iwamatsu <iwamatsu@nigauri.org>
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
#include <ide.h>
#include <netdev.h>
#include <asm/processor.h>
#include <asm/io.h>
#include <asm/pci.h>

DECLARE_GLOBAL_DATA_PTR;

int checkboard(void)
{
	puts("BOARD: Renesas Solutions R2D Plus\n");
	return 0;
}

int board_init(void)
{
	return 0;
}

int dram_init(void)
{
	gd->bd->bi_memstart = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_memsize = CONFIG_SYS_SDRAM_SIZE;
	printf("DRAM:  %dMB\n", CONFIG_SYS_SDRAM_SIZE / (1024 * 1024));
	return 0;
}

int board_late_init(void)
{
	return 0;
}

#define FPGA_BASE		0xA4000000
#define FPGA_CFCTL		(FPGA_BASE + 0x04)
#define CFCTL_EN		(0x432)
#define FPGA_CFPOW		(FPGA_BASE + 0x06)
#define CFPOW_ON		(0x02)
#define FPGA_CFCDINTCLR	(FPGA_BASE + 0x2A)
#define CFCDINTCLR_EN	(0x01)

void ide_set_reset(int idereset)
{
	/* if reset = 1 IDE reset will be asserted */
	if (idereset) {
		outw(CFCTL_EN, FPGA_CFCTL);	/* CF enable */
		outw(inw(FPGA_CFPOW)|CFPOW_ON, FPGA_CFPOW); /* Power OM */
		outw(CFCDINTCLR_EN, FPGA_CFCDINTCLR); /* Int clear */
	}
}

static struct pci_controller hose;
void pci_init_board(void)
{
	pci_sh7751_init(&hose);
}

int board_eth_init(bd_t *bis)
{
	return pci_eth_init(bis);
}
