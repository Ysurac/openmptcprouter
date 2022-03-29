/*
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>
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

#ifndef _MPC106_PCI_H
#define _MPC106_PCI_H

/*
 * Defines for the MPC106 PCI Config address and data registers followed by
 * defines for the standard PCI device configuration header.
 */
#define PCIDEVID_MPC106			0x0

/*
 * MPC106 Registers
 */
#define	MPC106_REG			0x80000000

#ifdef CONFIG_SYS_ADDRESS_MAP_A
#define MPC106_REG_ADDR			0x80000cf8
#define	MPC106_REG_DATA			0x80000cfc
#define MPC106_ISA_IO_PHYS		0x80000000
#define MPC106_ISA_IO_BUS		0x00000000
#define MPC106_ISA_IO_SIZE		0x00800000
#define MPC106_PCI_IO_PHYS		0x81000000
#define MPC106_PCI_IO_BUS		0x01000000
#define MPC106_PCI_IO_SIZE		0x3e800000
#define MPC106_PCI_MEM_PHYS		0xc0000000
#define MPC106_PCI_MEM_BUS		0x00000000
#define MPC106_PCI_MEM_SIZE		0x3f000000
#define	MPC106_PCI_MEMORY_PHYS		0x00000000
#define	MPC106_PCI_MEMORY_BUS		0x80000000
#define	MPC106_PCI_MEMORY_SIZE		0x80000000
#else
#define MPC106_REG_ADDR			0xfec00cf8
#define	MPC106_REG_DATA			0xfee00cfc
#define MPC106_ISA_MEM_PHYS		0xfd000000
#define MPC106_ISA_MEM_BUS		0x00000000
#define MPC106_ISA_MEM_SIZE		0x01000000
#define MPC106_ISA_IO_PHYS		0xfe000000
#define MPC106_ISA_IO_BUS		0x00000000
#define MPC106_ISA_IO_SIZE		0x00800000
#define MPC106_PCI_IO_PHYS		0xfe800000
#define MPC106_PCI_IO_BUS		0x00800000
#define MPC106_PCI_IO_SIZE		0x00400000
#define MPC106_PCI_MEM_PHYS		0x80000000
#define MPC106_PCI_MEM_BUS		0x80000000
#define MPC106_PCI_MEM_SIZE		0x7d000000
#define	MPC106_PCI_MEMORY_PHYS		0x00000000
#define	MPC106_PCI_MEMORY_BUS		0x00000000
#define MPC106_PCI_MEMORY_SIZE		0x40000000
#endif

#define CMD_SERR			0x0100
#define PCI_CMD_MASTER			0x0004
#define PCI_CMD_MEMEN			0x0002
#define PCI_CMD_IOEN			0x0001

#define PCI_STAT_NO_RSV_BITS		0xffff

#define PCI_BUSNUM			0x40
#define PCI_SUBBUSNUM			0x41
#define PCI_DISCOUNT			0x42

#define PCI_PICR1			0xA8
#define PICR1_CF_CBA(value)		((value & 0xff) << 24)
#define PICR1_CF_BREAD_WS(value)	((value & 0x3) << 22)
#define PICR1_PROC_TYPE_603		0x40000
#define PICR1_PROC_TYPE_604		0x60000
#define PICR1_MCP_EN			0x800
#define PICR1_CF_DPARK			0x200
#define PICR1_CF_LOOP_SNOOP		0x10
#define PICR1_CF_L2_COPY_BACK		0x2
#define PICR1_CF_L2_CACHE_MASK		0x3
#define PICR1_CF_APARK			0x8
#define PICR1_ADDRESS_MAP		0x10000
#define PICR1_XIO_MODE			0x80000
#define PICR1_CF_CACHE_1G		0x200000

#define PCI_PICR2			0xAC
#define PICR2_CF_SNOOP_WS(value)	((value & 0x3) << 18)
#define PICR2_CF_FLUSH_L2		0x10000000
#define PICR2_CF_L2_HIT_DELAY(value)	((value & 0x3) << 9)
#define PICR2_CF_APHASE_WS(value)	((value & 0x3) << 2)
#define PICR2_CF_INV_MODE		0x00001000
#define PICR2_CF_MOD_HIGH		0x00020000
#define PICR2_CF_HIT_HIGH		0x00010000
#define PICR2_L2_SIZE_256K		0x00000000
#define PICR2_L2_SIZE_512K		0x00000010
#define PICR2_L2_SIZE_1MB		0x00000020
#define PICR2_L2_EN			0x40000000
#define PICR2_L2_UPDATE_EN		0x80000000
#define PICR2_CF_ADDR_ONLY_DISABLE	0x00004000
#define PICR2_CF_FAST_CASTOUT		0x00000080
#define PICR2_CF_WDATA			0x00000001
#define PICR2_CF_DATA_RAM_PBURST	0x00400000

/*
 * Memory controller
 */
#define MPC106_MCCR1			0xF0
#define MCCR1_TYPE_EDO			0x00020000
#define MCCR1_BK0_9BITS			0x0
#define MCCR1_BK0_10BITS		0x1
#define MCCR1_BK0_11BITS		0x2
#define MCCR1_BK0_12BITS		0x3
#define MCCR1_BK1_9BITS			0x0
#define MCCR1_BK1_10BITS		0x4
#define MCCR1_BK1_11BITS		0x8
#define MCCR1_BK1_12BITS		0xC
#define MCCR1_BK2_9BITS			0x00
#define MCCR1_BK2_10BITS		0x10
#define MCCR1_BK2_11BITS		0x20
#define MCCR1_BK2_12BITS		0x30
#define MCCR1_BK3_9BITS			0x00
#define MCCR1_BK3_10BITS		0x40
#define MCCR1_BK3_11BITS		0x80
#define MCCR1_BK3_12BITS		0xC0
#define MCCR1_MEMGO			0x00080000

#define MPC106_MCCR2			0xF4
#define MPC106_MCCR3			0xF8
#define MPC106_MCCR4			0xFC

#define MPC106_MSAR1			0x80
#define MPC106_EMSAR1			0x88
#define MPC106_EMSAR2			0x8C
#define MPC106_MEAR1			0x90
#define MPC106_EMEAR1			0x98
#define MPC106_EMEAR2			0x9C

#define MPC106_MBER			0xA0
#define MBER_BANK0			0x1
#define MBER_BANK1			0x2
#define MBER_BANK2			0x4
#define MBER_BANK3			0x8

#endif
