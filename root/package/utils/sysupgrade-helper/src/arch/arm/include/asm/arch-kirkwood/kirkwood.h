/*
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
 *
 * Header file for the Marvell's Feroceon CPU core.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef _ASM_ARCH_KIRKWOOD_H
#define _ASM_ARCH_KIRKWOOD_H

#if defined (CONFIG_FEROCEON_88FR131) || defined (CONFIG_SHEEVA_88SV131)

/* SOC specific definations */
#define INTREG_BASE			0xd0000000
#define KW_REGISTER(x)			(KW_REGS_PHY_BASE + x)
#define KW_OFFSET_REG			(INTREG_BASE + 0x20080)

/* undocumented registers */
#define KW_REG_UNDOC_0x1470		(KW_REGISTER(0x1470))
#define KW_REG_UNDOC_0x1478		(KW_REGISTER(0x1478))

#define KW_TWSI_BASE			(KW_REGISTER(0x11000))
#define KW_UART0_BASE			(KW_REGISTER(0x12000))
#define KW_UART1_BASE			(KW_REGISTER(0x12100))
#define KW_MPP_BASE			(KW_REGISTER(0x10000))
#define KW_GPIO0_BASE			(KW_REGISTER(0x10100))
#define KW_GPIO1_BASE			(KW_REGISTER(0x10140))
#define KW_RTC_BASE			(KW_REGISTER(0x10300))
#define KW_NANDF_BASE			(KW_REGISTER(0x10418))
#define KW_SPI_BASE			(KW_REGISTER(0x10600))
#define KW_CPU_WIN_BASE			(KW_REGISTER(0x20000))
#define KW_CPU_REG_BASE			(KW_REGISTER(0x20100))
#define KW_TIMER_BASE			(KW_REGISTER(0x20300))
#define KW_REG_PCIE_BASE		(KW_REGISTER(0x40000))
#define KW_USB20_BASE			(KW_REGISTER(0x50000))
#define KW_EGIGA0_BASE			(KW_REGISTER(0x72000))
#define KW_EGIGA1_BASE			(KW_REGISTER(0x76000))
#define KW_SATA_BASE			(KW_REGISTER(0x80000))

/* Kirkwood Sata controller has two ports */
#define KW_SATA_PORT0_OFFSET		0x2000
#define KW_SATA_PORT1_OFFSET		0x4000

/* Kirkwood GbE controller has two ports */
#define MAX_MVGBE_DEVS	2
#define MVGBE0_BASE	KW_EGIGA0_BASE
#define MVGBE1_BASE	KW_EGIGA1_BASE

/* Kirkwood USB Host controller */
#define MVUSB0_BASE			KW_USB20_BASE
#define MVUSB0_CPU_ATTR_DRAM_CS0	KWCPU_ATTR_DRAM_CS0
#define MVUSB0_CPU_ATTR_DRAM_CS1	KWCPU_ATTR_DRAM_CS1
#define MVUSB0_CPU_ATTR_DRAM_CS2	KWCPU_ATTR_DRAM_CS2
#define MVUSB0_CPU_ATTR_DRAM_CS3	KWCPU_ATTR_DRAM_CS3

/* Kirkwood CPU memory windows */
#define MVCPU_WIN_CTRL_DATA	KWCPU_WIN_CTRL_DATA
#define MVCPU_WIN_ENABLE	KWCPU_WIN_ENABLE
#define MVCPU_WIN_DISABLE	KWCPU_WIN_DISABLE

#if defined (CONFIG_KW88F6281)
#include <asm/arch/kw88f6281.h>
#elif defined (CONFIG_KW88F6192)
#include <asm/arch/kw88f6192.h>
#else
#error "SOC Name not defined"
#endif /* CONFIG_KW88F6281 */
#endif /* CONFIG_FEROCEON_88FR131 */
#endif /* _ASM_ARCH_KIRKWOOD_H */
