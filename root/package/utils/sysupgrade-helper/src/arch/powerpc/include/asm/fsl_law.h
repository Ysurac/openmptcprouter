/*
 * Copyright 2008-2011 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
 */

#ifndef _FSL_LAW_H_
#define _FSL_LAW_H_

#include <asm/io.h>

#define LAW_EN	0x80000000

#define SET_LAW_ENTRY(idx, a, sz, trgt) \
	{ .index = idx, .addr = a, .size = sz, .trgt_id = trgt }

#define SET_LAW(a, sz, trgt) \
	{ .index = -1, .addr = a, .size = sz, .trgt_id = trgt }

enum law_size {
	LAW_SIZE_4K = 0xb,
	LAW_SIZE_8K,
	LAW_SIZE_16K,
	LAW_SIZE_32K,
	LAW_SIZE_64K,
	LAW_SIZE_128K,
	LAW_SIZE_256K,
	LAW_SIZE_512K,
	LAW_SIZE_1M,
	LAW_SIZE_2M,
	LAW_SIZE_4M,
	LAW_SIZE_8M,
	LAW_SIZE_16M,
	LAW_SIZE_32M,
	LAW_SIZE_64M,
	LAW_SIZE_128M,
	LAW_SIZE_256M,
	LAW_SIZE_512M,
	LAW_SIZE_1G,
	LAW_SIZE_2G,
	LAW_SIZE_4G,
	LAW_SIZE_8G,
	LAW_SIZE_16G,
	LAW_SIZE_32G,
};

#define law_size_bits(sz)	(__ilog2_u64(sz) - 1)
#define lawar_size(x)	(1ULL << ((x & 0x3f) + 1))

#ifdef CONFIG_FSL_CORENET
enum law_trgt_if {
	LAW_TRGT_IF_PCIE_1 = 0x00,
	LAW_TRGT_IF_PCIE_2 = 0x01,
	LAW_TRGT_IF_PCIE_3 = 0x02,
	LAW_TRGT_IF_PCIE_4 = 0x03,
	LAW_TRGT_IF_RIO_1 = 0x08,
	LAW_TRGT_IF_RIO_2 = 0x09,

	LAW_TRGT_IF_DDR_1 = 0x10,
	LAW_TRGT_IF_DDR_2 = 0x11,	/* 2nd controller */
	LAW_TRGT_IF_DDR_INTRLV = 0x14,

	LAW_TRGT_IF_BMAN = 0x18,
	LAW_TRGT_IF_DCSR = 0x1d,
	LAW_TRGT_IF_LBC = 0x1f,
	LAW_TRGT_IF_QMAN = 0x3c,
};
#define LAW_TRGT_IF_DDR		LAW_TRGT_IF_DDR_1
#else
enum law_trgt_if {
	LAW_TRGT_IF_PCI = 0x00,
	LAW_TRGT_IF_PCI_2 = 0x01,
#ifndef CONFIG_MPC8641
	LAW_TRGT_IF_PCIE_1 = 0x02,
#endif
#if !defined(CONFIG_MPC8572) && !defined(CONFIG_P2020)
	LAW_TRGT_IF_PCIE_3 = 0x03,
#endif
	LAW_TRGT_IF_LBC = 0x04,
	LAW_TRGT_IF_CCSR = 0x08,
	LAW_TRGT_IF_DDR_INTRLV = 0x0b,
	LAW_TRGT_IF_RIO = 0x0c,
	LAW_TRGT_IF_RIO_2 = 0x0d,
	LAW_TRGT_IF_DPAA_SWP_SRAM = 0x0e,
	LAW_TRGT_IF_DDR = 0x0f,
	LAW_TRGT_IF_DDR_2 = 0x16,	/* 2nd controller */
};
#define LAW_TRGT_IF_DDR_1	LAW_TRGT_IF_DDR
#define LAW_TRGT_IF_PCI_1	LAW_TRGT_IF_PCI
#define LAW_TRGT_IF_PCIX	LAW_TRGT_IF_PCI
#define LAW_TRGT_IF_PCIE_2	LAW_TRGT_IF_PCI_2
#define LAW_TRGT_IF_RIO_1	LAW_TRGT_IF_RIO
#define LAW_TRGT_IF_IFC		LAW_TRGT_IF_LBC

#ifdef CONFIG_MPC8641
#define LAW_TRGT_IF_PCIE_1	LAW_TRGT_IF_PCI
#endif

#if defined(CONFIG_MPC8572) || defined(CONFIG_P2020)
#define LAW_TRGT_IF_PCIE_3	LAW_TRGT_IF_PCI
#endif
#endif /* CONFIG_FSL_CORENET */

struct law_entry {
	int index;
	phys_addr_t addr;
	enum law_size size;
	enum law_trgt_if trgt_id;
};

extern void set_law(u8 idx, phys_addr_t addr, enum law_size sz, enum law_trgt_if id);
extern int set_next_law(phys_addr_t addr, enum law_size sz, enum law_trgt_if id);
extern int set_last_law(phys_addr_t addr, enum law_size sz, enum law_trgt_if id);
extern int set_ddr_laws(u64 start, u64 sz, enum law_trgt_if id);
extern struct law_entry find_law(phys_addr_t addr);
extern void disable_law(u8 idx);
extern void init_laws(void);
extern void print_laws(void);

/* define in board code */
extern struct law_entry law_table[];
extern int num_law_entries;
#endif
