/*
 * Copyright 2008, 2011 Freescale Semiconductor, Inc.
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
	SET_TLB_ENTRY(0, CONFIG_SYS_INIT_RAM_ADDR + 4 * 1024 , CONFIG_SYS_INIT_RAM_ADDR + 4 * 1024,
		      MAS3_SX|MAS3_SW|MAS3_SR, 0,
		      0, 0, BOOKE_PAGESZ_4K, 0),
	SET_TLB_ENTRY(0, CONFIG_SYS_INIT_RAM_ADDR + 8 * 1024 , CONFIG_SYS_INIT_RAM_ADDR + 8 * 1024,
		      MAS3_SX|MAS3_SW|MAS3_SR, 0,
		      0, 0, BOOKE_PAGESZ_4K, 0),
	SET_TLB_ENTRY(0, CONFIG_SYS_INIT_RAM_ADDR + 12 * 1024 , CONFIG_SYS_INIT_RAM_ADDR + 12 * 1024,
		      MAS3_SX|MAS3_SW|MAS3_SR, 0,
		      0, 0, BOOKE_PAGESZ_4K, 0),

	/* TLB 1 */
	/*
	 * Entry 0:
	 * FLASH(cover boot page)	16M	Non-cacheable, guarded
	 */
	SET_TLB_ENTRY(1, CONFIG_SYS_FLASH_BASE, CONFIG_SYS_FLASH_BASE_PHYS,
		      MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		      0, 0, BOOKE_PAGESZ_16M, 1),

	/*
	 * Entry 1:
	 * CCSRBAR	1M	Non-cacheable, guarded
	 */
	SET_TLB_ENTRY(1, CONFIG_SYS_CCSRBAR, CONFIG_SYS_CCSRBAR_PHYS,
		      MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		      0, 1, BOOKE_PAGESZ_1M, 1),

	/*
	 * Entry 2:
	 * LBC SDRAM	64M	Cacheable, non-guarded
	 */
	SET_TLB_ENTRY(1, CONFIG_SYS_LBC_SDRAM_BASE,
		      CONFIG_SYS_LBC_SDRAM_BASE_PHYS,
		      MAS3_SX|MAS3_SW|MAS3_SR, 0,
		      0, 2, BOOKE_PAGESZ_64M, 1),

	/*
	 * Entry 3:
	 * CADMUS registers	1M	Non-cacheable, guarded
	 */
	SET_TLB_ENTRY(1, CADMUS_BASE_ADDR, CADMUS_BASE_ADDR_PHYS,
		      MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		      0, 3, BOOKE_PAGESZ_1M, 1),

	/*
	 * Entry 4:
	 * PCI and PCIe MEM	1G	Non-cacheable, guarded
	 */
	SET_TLB_ENTRY(1, CONFIG_SYS_PCI1_MEM_VIRT, CONFIG_SYS_PCI1_MEM_PHYS,
		      MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		      0, 4, BOOKE_PAGESZ_1G, 1),

	/*
	 * Entry 5:
	 * PCI1 IO	1M	Non-cacheable, guarded
	 */
	SET_TLB_ENTRY(1, CONFIG_SYS_PCI1_IO_VIRT, CONFIG_SYS_PCI1_IO_PHYS,
		      MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		      0, 5, BOOKE_PAGESZ_1M, 1),

	/*
	 * Entry 6:
	 * PCIe IO	1M	Non-cacheable, guarded
	 */
	SET_TLB_ENTRY(1, CONFIG_SYS_PCIE1_IO_VIRT, CONFIG_SYS_PCIE1_IO_PHYS,
		      MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		      0, 6, BOOKE_PAGESZ_1M, 1),
};

int num_tlb_entries = ARRAY_SIZE(tlb_table);
