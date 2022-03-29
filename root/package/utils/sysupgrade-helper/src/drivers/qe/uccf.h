/*
 * Copyright (C) 2006 Freescale Semiconductor, Inc.
 *
 * Dave Liu <daveliu@freescale.com>
 * based on source code of Shlomi Gridish
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

#ifndef __UCCF_H__
#define __UCCF_H__

#include "common.h"
#include "qe.h"
#include "asm/immap_qe.h"

/* Fast or Giga ethernet
*/
typedef enum enet_type {
	FAST_ETH,
	GIGA_ETH,
} enet_type_e;

/* General UCC Extended Mode Register
*/
#define UCC_GUEMR_MODE_MASK_RX		0x02
#define UCC_GUEMR_MODE_MASK_TX		0x01
#define UCC_GUEMR_MODE_FAST_RX		0x02
#define UCC_GUEMR_MODE_FAST_TX		0x01
#define UCC_GUEMR_MODE_SLOW_RX		0x00
#define UCC_GUEMR_MODE_SLOW_TX		0x00
#define UCC_GUEMR_SET_RESERVED3		0x10 /* Bit 3 must be set 1 */

/* General UCC FAST Mode Register
*/
#define UCC_FAST_GUMR_TCI		0x20000000
#define UCC_FAST_GUMR_TRX		0x10000000
#define UCC_FAST_GUMR_TTX		0x08000000
#define UCC_FAST_GUMR_CDP		0x04000000
#define UCC_FAST_GUMR_CTSP		0x02000000
#define UCC_FAST_GUMR_CDS		0x01000000
#define UCC_FAST_GUMR_CTSS		0x00800000
#define UCC_FAST_GUMR_TXSY		0x00020000
#define UCC_FAST_GUMR_RSYN		0x00010000
#define UCC_FAST_GUMR_RTSM		0x00002000
#define UCC_FAST_GUMR_REVD		0x00000400
#define UCC_FAST_GUMR_ENR		0x00000020
#define UCC_FAST_GUMR_ENT		0x00000010

/* GUMR [MODE] bit maps
*/
#define UCC_FAST_GUMR_HDLC		0x00000000
#define UCC_FAST_GUMR_QMC		0x00000002
#define UCC_FAST_GUMR_UART		0x00000004
#define UCC_FAST_GUMR_BISYNC		0x00000008
#define UCC_FAST_GUMR_ATM		0x0000000a
#define UCC_FAST_GUMR_ETH		0x0000000c

/* Transmit On Demand (UTORD)
*/
#define UCC_SLOW_TOD			0x8000
#define UCC_FAST_TOD			0x8000

/* Fast Ethernet (10/100 Mbps)
*/
#define UCC_GETH_URFS_INIT		512        /* Rx virtual FIFO size */
#define UCC_GETH_URFET_INIT		256        /* 1/2 urfs */
#define UCC_GETH_URFSET_INIT		384        /* 3/4 urfs */
#define UCC_GETH_UTFS_INIT		512        /* Tx virtual FIFO size */
#define UCC_GETH_UTFET_INIT		256        /* 1/2 utfs */
#define UCC_GETH_UTFTT_INIT		128

/* Gigabit Ethernet (1000 Mbps)
*/
#define UCC_GETH_URFS_GIGA_INIT		4096/*2048*/    /* Rx virtual FIFO size */
#define UCC_GETH_URFET_GIGA_INIT	2048/*1024*/    /* 1/2 urfs */
#define UCC_GETH_URFSET_GIGA_INIT	3072/*1536*/    /* 3/4 urfs */
#define UCC_GETH_UTFS_GIGA_INIT		8192/*2048*/    /* Tx virtual FIFO size */
#define UCC_GETH_UTFET_GIGA_INIT	4096/*1024*/    /* 1/2 utfs */
#define UCC_GETH_UTFTT_GIGA_INIT	0x400/*0x40*/   /*  */

/* UCC fast alignment
*/
#define UCC_FAST_RX_ALIGN			4
#define UCC_FAST_MRBLR_ALIGNMENT		4
#define UCC_FAST_VIRT_FIFO_REGS_ALIGNMENT	8

/* Sizes
*/
#define UCC_FAST_RX_VIRTUAL_FIFO_SIZE_PAD	8

/* UCC fast structure.
*/
typedef struct ucc_fast_info {
	int		ucc_num;
	qe_clock_e	rx_clock;
	qe_clock_e	tx_clock;
	enet_type_e	eth_type;
} ucc_fast_info_t;

typedef struct ucc_fast_private {
	ucc_fast_info_t	*uf_info;
	ucc_fast_t	*uf_regs; /* a pointer to memory map of UCC regs */
	u32		*p_ucce; /* a pointer to the event register */
	u32		*p_uccm; /* a pointer to the mask register */
	int		enabled_tx; /* whether UCC is enabled for Tx (ENT) */
	int		enabled_rx; /* whether UCC is enabled for Rx (ENR) */
	u32		ucc_fast_tx_virtual_fifo_base_offset;
	u32		ucc_fast_rx_virtual_fifo_base_offset;
} ucc_fast_private_t;

void ucc_fast_transmit_on_demand(ucc_fast_private_t *uccf);
u32 ucc_fast_get_qe_cr_subblock(int ucc_num);
void ucc_fast_enable(ucc_fast_private_t *uccf, comm_dir_e mode);
void ucc_fast_disable(ucc_fast_private_t *uccf, comm_dir_e mode);
int ucc_fast_init(ucc_fast_info_t *uf_info, ucc_fast_private_t **uccf_ret);

#endif /* __UCCF_H__ */
