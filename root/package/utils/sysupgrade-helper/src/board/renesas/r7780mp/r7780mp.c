/*
 * Copyright (C) 2007,2008 Nobuhiro Iwamatsu <iwamatsu@nigauri.org>
 * Copyright (C) 2008 Yusuke Goda <goda.yusuke@renesas.com>
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
#include <asm/processor.h>
#include <asm/io.h>
#include <asm/pci.h>
#include <netdev.h>
#include "r7780mp.h"

DECLARE_GLOBAL_DATA_PTR;

int checkboard(void)
{
#if defined(CONFIG_R7780MP)
	puts("BOARD: Renesas Solutions R7780MP\n");
#else
	puts("BOARD: Renesas Solutions R7780RP\n");
#endif
	return 0;
}

int board_init(void)
{
	/* SCIF Enable */
	writew(0x0, PHCR);

	return 0;
}

int dram_init(void)
{
	gd->bd->bi_memstart = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_memsize = CONFIG_SYS_SDRAM_SIZE;
	printf("DRAM:  %dMB\n", CONFIG_SYS_SDRAM_SIZE / (1024 * 1024));
	return 0;
}

void led_set_state(unsigned short value)
{

}

void ide_set_reset(int idereset)
{
	/* if reset = 1 IDE reset will be asserted */
	if (idereset) {
		writew(0x432, FPGA_CFCTL);
#if defined(CONFIG_R7780MP)
		writew(inw(FPGA_CFPOW)|0x01, FPGA_CFPOW);
#else
		writew(inw(FPGA_CFPOW)|0x02, FPGA_CFPOW);
#endif
		writew(0x01, FPGA_CFCDINTCLR);
	}
}

static struct pci_controller hose;
void pci_init_board(void)
{
	pci_sh7780_init(&hose);
}

int board_eth_init(bd_t *bis)
{
	/* return >= 0 if a chip is found, the board's AX88796L is n2k-based */
	return ne2k_register() + pci_eth_init(bis);
}
