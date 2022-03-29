/*
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
 *
 * Derived from drivers/spi/mpc8xxx_spi.c
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

#ifndef __KW_SPI_H__
#define __KW_SPI_H__

/* SPI Registers on kirkwood SOC */
struct kwspi_registers {
	u32 ctrl;	/* 0x10600 */
	u32 cfg;	/* 0x10604 */
	u32 dout;	/* 0x10608 */
	u32 din;	/* 0x1060c */
	u32 irq_cause;	/* 0x10610 */
	u32 irq_mask;	/* 0x10614 */
};

/* They are used to define CONFIG_SYS_KW_SPI_MPP
 * each of the below #defines selects which mpp is
 * configured for each SPI signal in spi_claim_bus
 * bit 0: selects pin for MOSI (MPP1 if 0, MPP6 if 1)
 * bit 1: selects pin for SCK (MPP2 if 0, MPP10 if 1)
 * bit 2: selects pin for MISO (MPP3 if 0, MPP11 if 1)
 */
#define MOSI_MPP6	(1 << 0)
#define SCK_MPP10	(1 << 1)
#define MISO_MPP11	(1 << 2)

#define KWSPI_CLKPRESCL_MASK	0x1f
#define KWSPI_CSN_ACT		1 /* Activates serial memory interface */
#define KWSPI_SMEMRDY		(1 << 1) /* SerMem Data xfer ready */
#define KWSPI_IRQUNMASK		1 /* unmask SPI interrupt */
#define KWSPI_IRQMASK		0 /* mask SPI interrupt */
#define KWSPI_SMEMRDIRQ		1 /* SerMem data xfer ready irq */
#define KWSPI_XFERLEN_1BYTE	0
#define KWSPI_XFERLEN_2BYTE	(1 << 5)
#define KWSPI_XFERLEN_MASK	(1 << 5)
#define KWSPI_ADRLEN_1BYTE	0
#define KWSPI_ADRLEN_2BYTE	1 << 8
#define KWSPI_ADRLEN_3BYTE	2 << 8
#define KWSPI_ADRLEN_4BYTE	3 << 8
#define KWSPI_ADRLEN_MASK	3 << 8
#define KWSPI_TIMEOUT		10000

#endif /* __KW_SPI_H__ */
