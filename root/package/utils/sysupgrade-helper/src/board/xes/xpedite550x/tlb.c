/*
 * Copyright 2008 Extreme Engineering Solutions, Inc.
 * Copyright 2008 Freescale Semiconductor, Inc.
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
#include <asm/mmu.h>

struct fsl_e_tlb_entry tlb_table[] = {
	/* TLB 0 - for temp stack in cache */
	SET_TLB_ENTRY(0, CONFIG_SYS_INIT_RAM_ADDR, CONFIG_SYS_INIT_RAM_ADDR,
		MAS3_SX|MAS3_SW|MAS3_SR, 0,
		0, 0, BOOKE_PAGESZ_4K, 0),
	SET_TLB_ENTRY(0, CONFIG_SYS_INIT_RAM_ADDR + 4 * 1024,
		CONFIG_SYS_INIT_RAM_ADDR + 4 * 1024,
		MAS3_SX|MAS3_SW|MAS3_SR, 0,
		0, 0, BOOKE_PAGESZ_4K, 0),
	SET_TLB_ENTRY(0, CONFIG_SYS_INIT_RAM_ADDR + 8 * 1024,
		CONFIG_SYS_INIT_RAM_ADDR + 8 * 1024,
		MAS3_SX|MAS3_SW|MAS3_SR, 0,
		0, 0, BOOKE_PAGESZ_4K, 0),
	SET_TLB_ENTRY(0, CONFIG_SYS_INIT_RAM_ADDR + 12 * 1024,
		CONFIG_SYS_INIT_RAM_ADDR + 12 * 1024,
		MAS3_SX|MAS3_SW|MAS3_SR, 0,
		0, 0, BOOKE_PAGESZ_4K, 0),

	/* W**G* - NOR flashes */
	/* This will be changed to *I*G* after relocation to RAM. */
	SET_TLB_ENTRY(1, CONFIG_SYS_FLASH_BASE2, CONFIG_SYS_FLASH_BASE2,
		MAS3_SX|MAS3_SW|MAS3_SR, MAS2_W|MAS2_G,
		0, 0, BOOKE_PAGESZ_256M, 1),

	/* *I*G* - CCSRBAR */
	SET_TLB_ENTRY(1, CONFIG_SYS_CCSRBAR, CONFIG_SYS_CCSRBAR_PHYS,
		MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		0, 1, BOOKE_PAGESZ_1M, 1),

	/* *I*G* - NAND flash */
	SET_TLB_ENTRY(1, CONFIG_SYS_NAND_BASE, CONFIG_SYS_NAND_BASE,
		MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		0, 2, BOOKE_PAGESZ_1M, 1),

	/* **M** - Boot page for secondary processors */
	SET_TLB_ENTRY(1, CONFIG_BPTR_VIRT_ADDR, CONFIG_BPTR_VIRT_ADDR,
		MAS3_SX|MAS3_SW|MAS3_SR, MAS2_M,
		0, 3, BOOKE_PAGESZ_4K, 1),

#ifdef CONFIG_PCIE1
	/* *I*G* - PCIe */
	SET_TLB_ENTRY(1, CONFIG_SYS_PCIE1_MEM_PHYS, CONFIG_SYS_PCIE1_MEM_PHYS,
		MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		0, 4, BOOKE_PAGESZ_1G, 1),
#endif

#ifdef CONFIG_PCIE2
	/* *I*G* - PCIe */
	SET_TLB_ENTRY(1, CONFIG_SYS_PCIE2_MEM_PHYS, CONFIG_SYS_PCIE2_MEM_PHYS,
		MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		0, 5, BOOKE_PAGESZ_256M, 1),
#endif

#ifdef CONFIG_PCIE3
	/* *I*G* - PCIe */
	SET_TLB_ENTRY(1, CONFIG_SYS_PCIE3_MEM_PHYS, CONFIG_SYS_PCIE3_MEM_PHYS,
		MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		0, 6, BOOKE_PAGESZ_256M, 1),
#endif

#if defined(CONFIG_PCIE1) || defined(CONFIG_PCIE2) || defined(CONFIG_PCIE3)
	/* *I*G* - PCIe */
	SET_TLB_ENTRY(1, CONFIG_SYS_PCIE1_IO_PHYS, CONFIG_SYS_PCIE1_IO_PHYS,
		MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		0, 7, BOOKE_PAGESZ_64M, 1),
#endif
};

int num_tlb_entries = ARRAY_SIZE(tlb_table);
