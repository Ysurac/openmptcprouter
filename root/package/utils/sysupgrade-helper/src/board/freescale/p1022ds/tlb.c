/*
 * Copyright 2010 Freescale Semiconductor, Inc.
 * Authors: Srikanth Srinivasan <srikanth.srinivasan@freescale.com>
 *          Timur Tabi <timur@freescale.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
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

	/* TLB 1 */
	/* *I*** - Covers boot page */
	SET_TLB_ENTRY(1, 0xfffff000, 0xfffff000,
		      MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I,
		      0, 0, BOOKE_PAGESZ_4K, 1),

	/* *I*G* - CCSRBAR */
	SET_TLB_ENTRY(1, CONFIG_SYS_CCSRBAR, CONFIG_SYS_CCSRBAR_PHYS,
		      MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		      0, 1, BOOKE_PAGESZ_1M, 1),

	/* W**G* - Flash/promjet, localbus */
	/* This will be changed to *I*G* after relocation to RAM. */
	SET_TLB_ENTRY(1, CONFIG_SYS_FLASH_BASE, CONFIG_SYS_FLASH_BASE_PHYS,
		      MAS3_SX|MAS3_SR, MAS2_W|MAS2_G,
		      0, 2, BOOKE_PAGESZ_256M, 1),

	/* *I*G* - PCI */
	SET_TLB_ENTRY(1, CONFIG_SYS_PCIE3_MEM_VIRT, CONFIG_SYS_PCIE3_MEM_PHYS,
		      MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		      0, 3, BOOKE_PAGESZ_1G, 1),

	/* *I*G* - PCI */
	SET_TLB_ENTRY(1, CONFIG_SYS_PCIE3_MEM_VIRT + 0x40000000,
		      CONFIG_SYS_PCIE3_MEM_PHYS + 0x40000000,
		      MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		      0, 4, BOOKE_PAGESZ_256M, 1),

	SET_TLB_ENTRY(1, CONFIG_SYS_PCIE3_MEM_VIRT + 0x50000000,
		      CONFIG_SYS_PCIE3_MEM_PHYS + 0x50000000,
		      MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		      0, 5, BOOKE_PAGESZ_256M, 1),

	/* *I*G* - PCI I/O */
	SET_TLB_ENTRY(1, CONFIG_SYS_PCIE3_IO_VIRT, CONFIG_SYS_PCIE3_IO_PHYS,
		      MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		      0, 6, BOOKE_PAGESZ_256K, 1),

	SET_TLB_ENTRY(1, PIXIS_BASE, PIXIS_BASE_PHYS,
		      MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		      0, 7, BOOKE_PAGESZ_4K, 1),
};

int num_tlb_entries = ARRAY_SIZE(tlb_table);
