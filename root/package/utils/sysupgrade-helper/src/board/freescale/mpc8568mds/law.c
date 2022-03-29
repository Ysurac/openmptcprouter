/*
 * Copyright 2008, 2010-2011 Freescale Semiconductor, Inc.
 *
 * (C) Copyright 2000
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
 */

#include <common.h>
#include <asm/fsl_law.h>
#include <asm/mmu.h>

/*
 * LAW(Local Access Window) configuration:
 *
 *0)   0x0000_0000   0x7fff_ffff     DDR                     2G
 *1)   0x8000_0000   0x9fff_ffff     PCI1 MEM                512MB
 *2)   0xa000_0000   0xbfff_ffff     PCIe MEM                512MB
 *-)   0xe000_0000   0xe00f_ffff     CCSR                    1M
 *3)   0xe200_0000   0xe27f_ffff     PCI1 I/O                8M
 *4)   0xe280_0000   0xe2ff_ffff     PCIe I/O                8M
 *5)   0xc000_0000   0xdfff_ffff     SRIO                    512MB
 *6.a) 0xf000_0000   0xf3ff_ffff     SDRAM                   64MB
 *6.b) 0xf800_0000   0xf800_7fff     BCSR                    32KB
 *6.c) 0xf800_8000   0xf800_ffff     PIB (CS4)		     32KB
 *6.d) 0xf801_0000   0xf801_7fff     PIB (CS5)		     32KB
 *6.e) 0xfe00_0000   0xffff_ffff     Flash                   32MB
 *
 *Notes:
 *    CCSRBAR and L2-as-SRAM don't need a configured Local Access Window.
 *    If flash is 8M at default position (last 8M), no LAW needed.
 *
 */

struct law_entry law_table[] = {
	/* LBC window - maps 256M.  That's SDRAM, BCSR, PIBs, and Flash */
	SET_LAW(CONFIG_SYS_LBC_SDRAM_BASE, LAW_SIZE_256M, LAW_TRGT_IF_LBC),
};

int num_law_entries = ARRAY_SIZE(law_table);
