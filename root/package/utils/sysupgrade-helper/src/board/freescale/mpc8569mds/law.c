/*
 * Copyright 2009-2011 Freescale Semiconductor, Inc.
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
 *1)   0xa000_0000   0xbfff_ffff     PCIe MEM                512MB
 *-)   0xe000_0000   0xe00f_ffff     CCSR                    1M
 *2)   0xe280_0000   0xe2ff_ffff     PCIe I/O                8M
 *3)   0xc000_0000   0xdfff_ffff     SRIO                    512MB
 *4.a) 0xf000_0000   0xf3ff_ffff     SDRAM                   64MB
 *4.b) 0xf800_0000   0xf800_7fff     BCSR                    32KB
 *4.c) 0xf800_8000   0xf800_ffff     PIB (CS4)		     32KB
 *4.d) 0xf801_0000   0xf801_7fff     PIB (CS5)		     32KB
 *4.e) 0xfe00_0000   0xffff_ffff     Flash                   32MB
 *
 *Notes:
 *    CCSRBAR and L2-as-SRAM don't need a configured Local Access Window.
 *    If flash is 8M at default position (last 8M), no LAW needed.
 *
 */

struct law_entry law_table[] = {
#ifndef CONFIG_SPD_EEPROM
	SET_LAW(CONFIG_SYS_DDR_SDRAM_BASE, LAW_SIZE_1G, LAW_TRGT_IF_DDR),
#endif
	SET_LAW(CONFIG_SYS_BCSR_BASE_PHYS, LAW_SIZE_128M, LAW_TRGT_IF_LBC),
};

int num_law_entries = ARRAY_SIZE(law_table);
