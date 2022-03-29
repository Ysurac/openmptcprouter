/*
 * Copyright 2011 Freescale Semiconductor, Inc.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/mmu.h>

struct fsl_e_tlb_entry tlb_table[] = {
	/* TLB 0 - for temp stack in cache */
	SET_TLB_ENTRY(0, CONFIG_SYS_INIT_RAM_ADDR,
			CONFIG_SYS_INIT_RAM_ADDR_PHYS,
			MAS3_SW|MAS3_SR, 0,
			0, 0, BOOKE_PAGESZ_4K, 0),
	SET_TLB_ENTRY(0, CONFIG_SYS_INIT_RAM_ADDR + 4 * 1024 ,
			CONFIG_SYS_INIT_RAM_ADDR_PHYS + 4 * 1024,
			MAS3_SW|MAS3_SR, 0,
			0, 0, BOOKE_PAGESZ_4K, 0),
	SET_TLB_ENTRY(0, CONFIG_SYS_INIT_RAM_ADDR + 8 * 1024 ,
			CONFIG_SYS_INIT_RAM_ADDR_PHYS + 8 * 1024,
			MAS3_SW|MAS3_SR, 0,
			0, 0, BOOKE_PAGESZ_4K, 0),
	SET_TLB_ENTRY(0, CONFIG_SYS_INIT_RAM_ADDR + 12 * 1024 ,
			CONFIG_SYS_INIT_RAM_ADDR_PHYS + 12 * 1024,
			MAS3_SW|MAS3_SR, 0,
			0, 0, BOOKE_PAGESZ_4K, 0),

	/* TLB 1 */
	/* *I*** - Covers boot page */
	SET_TLB_ENTRY(1, 0xfffff000, 0xfffff000,
			MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
			0, 0, BOOKE_PAGESZ_4K, 1),

	/* *I*G* - CCSRBAR */
	SET_TLB_ENTRY(1, CONFIG_SYS_CCSRBAR, CONFIG_SYS_CCSRBAR_PHYS,
			MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
			0, 1, BOOKE_PAGESZ_1M, 1),

#if defined(CONFIG_PCI)
	/* *I*G* - PCI3 - PCI2 0x8000,0000 - 0xbfff,ffff, size = 1G */
	SET_TLB_ENTRY(1, CONFIG_SYS_PCIE3_MEM_VIRT, CONFIG_SYS_PCIE3_MEM_PHYS,
			MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
			0, 2, BOOKE_PAGESZ_1G, 1),

	/* *I*G* - PCI1 0xC000,0000 - 0xcfff,ffff, size = 256M */
	SET_TLB_ENTRY(1, CONFIG_SYS_PCIE1_MEM_VIRT, CONFIG_SYS_PCIE1_MEM_VIRT,
			MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
			0, 3, BOOKE_PAGESZ_256M, 1),

	/* *I*G* - PCI1  0xD000,0000 - 0xDFFF,FFFF, size = 256M */
	SET_TLB_ENTRY(1, CONFIG_SYS_PCIE1_MEM_VIRT + 0x10000000,
			CONFIG_SYS_PCIE1_MEM_PHYS + 0x10000000,
			MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
			0, 4, BOOKE_PAGESZ_256M, 1),

	/*
	 * *I*G* - PCI I/O
	 *
	 * PCI3 => 0xFFC10000
	 * PCI2 => 0xFFC2,0000
	 * PCI1 => 0xFFC3,0000
	 */
	SET_TLB_ENTRY(1, CONFIG_SYS_PCIE3_IO_VIRT, CONFIG_SYS_PCIE3_IO_PHYS,
			MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
			0, 5, BOOKE_PAGESZ_256K, 1),
#endif /* #if defined(CONFIG_PCI) */

#if defined(CONFIG_SYS_RAMBOOT) && defined(CONFIG_SYS_INIT_L2_ADDR)
	/* *I*G - DDR3  2G     Part 1: 0 - 0x3fff,ffff , size = 1G */
	SET_TLB_ENTRY(1, CONFIG_SYS_INIT_L2_ADDR, CONFIG_SYS_INIT_L2_ADDR_PHYS,
			MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
			0, 6, BOOKE_PAGESZ_256K, 1),

	/*        DDR3  2G     Part 2: 0x4000,0000 - 0x7fff,ffff , size = 1G */
	SET_TLB_ENTRY(1, CONFIG_SYS_INIT_L2_ADDR + 0x40000,
			CONFIG_SYS_INIT_L2_ADDR_PHYS + 0x40000,
			MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
			0, 7, BOOKE_PAGESZ_256K, 1),
#endif
};

int num_tlb_entries = ARRAY_SIZE(tlb_table);
