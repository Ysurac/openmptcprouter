/*
 * Copyright (C) Freescale Semiconductor, Inc. 2006.
 * Author: Jason Jin<Jason.jin@freescale.com>
 *         Zhang Wei<wei.zhang@freescale.com>
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
 *
 */
#ifndef _AHCI_H_
#define _AHCI_H_

#include <pci.h>

#define AHCI_PCI_BAR		0x24
#define AHCI_MAX_SG		56 /* hardware max is 64K */
#define AHCI_CMD_SLOT_SZ	32
#define AHCI_MAX_CMD_SLOT	32
#define AHCI_RX_FIS_SZ		256
#define AHCI_CMD_TBL_HDR	0x80
#define AHCI_CMD_TBL_CDB	0x40
#define AHCI_CMD_TBL_SZ		AHCI_CMD_TBL_HDR + (AHCI_MAX_SG * 16)
#define AHCI_PORT_PRIV_DMA_SZ	(AHCI_CMD_SLOT_SZ * AHCI_MAX_CMD_SLOT + \
				AHCI_CMD_TBL_SZ	+ AHCI_RX_FIS_SZ)
#define AHCI_CMD_ATAPI		(1 << 5)
#define AHCI_CMD_WRITE		(1 << 6)
#define AHCI_CMD_PREFETCH	(1 << 7)
#define AHCI_CMD_RESET		(1 << 8)
#define AHCI_CMD_CLR_BUSY	(1 << 10)

#define RX_FIS_D2H_REG		0x40	/* offset of D2H Register FIS data */

/* Global controller registers */
#define HOST_CAP		0x00 /* host capabilities */
#define HOST_CTL		0x04 /* global host control */
#define HOST_IRQ_STAT		0x08 /* interrupt status */
#define HOST_PORTS_IMPL		0x0c /* bitmap of implemented ports */
#define HOST_VERSION		0x10 /* AHCI spec. version compliancy */

/* HOST_CTL bits */
#define HOST_RESET		(1 << 0)  /* reset controller; self-clear */
#define HOST_IRQ_EN		(1 << 1)  /* global IRQ enable */
#define HOST_AHCI_EN		(1 << 31) /* AHCI enabled */

/* Registers for each SATA port */
#define PORT_LST_ADDR		0x00 /* command list DMA addr */
#define PORT_LST_ADDR_HI	0x04 /* command list DMA addr hi */
#define PORT_FIS_ADDR		0x08 /* FIS rx buf addr */
#define PORT_FIS_ADDR_HI	0x0c /* FIS rx buf addr hi */
#define PORT_IRQ_STAT		0x10 /* interrupt status */
#define PORT_IRQ_MASK		0x14 /* interrupt enable/disable mask */
#define PORT_CMD		0x18 /* port command */
#define PORT_TFDATA		0x20 /* taskfile data */
#define PORT_SIG		0x24 /* device TF signature */
#define PORT_CMD_ISSUE		0x38 /* command issue */
#define PORT_SCR		0x28 /* SATA phy register block */
#define PORT_SCR_STAT		0x28 /* SATA phy register: SStatus */
#define PORT_SCR_CTL		0x2c /* SATA phy register: SControl */
#define PORT_SCR_ERR		0x30 /* SATA phy register: SError */
#define PORT_SCR_ACT		0x34 /* SATA phy register: SActive */

/* PORT_IRQ_{STAT,MASK} bits */
#define PORT_IRQ_COLD_PRES	(1 << 31) /* cold presence detect */
#define PORT_IRQ_TF_ERR		(1 << 30) /* task file error */
#define PORT_IRQ_HBUS_ERR	(1 << 29) /* host bus fatal error */
#define PORT_IRQ_HBUS_DATA_ERR	(1 << 28) /* host bus data error */
#define PORT_IRQ_IF_ERR		(1 << 27) /* interface fatal error */
#define PORT_IRQ_IF_NONFATAL	(1 << 26) /* interface non-fatal error */
#define PORT_IRQ_OVERFLOW	(1 << 24) /* xfer exhausted available S/G */
#define PORT_IRQ_BAD_PMP	(1 << 23) /* incorrect port multiplier */

#define PORT_IRQ_PHYRDY		(1 << 22) /* PhyRdy changed */
#define PORT_IRQ_DEV_ILCK	(1 << 7) /* device interlock */
#define PORT_IRQ_CONNECT	(1 << 6) /* port connect change status */
#define PORT_IRQ_SG_DONE	(1 << 5) /* descriptor processed */
#define PORT_IRQ_UNK_FIS	(1 << 4) /* unknown FIS rx'd */
#define PORT_IRQ_SDB_FIS	(1 << 3) /* Set Device Bits FIS rx'd */
#define PORT_IRQ_DMAS_FIS	(1 << 2) /* DMA Setup FIS rx'd */
#define PORT_IRQ_PIOS_FIS	(1 << 1) /* PIO Setup FIS rx'd */
#define PORT_IRQ_D2H_REG_FIS	(1 << 0) /* D2H Register FIS rx'd */

#define PORT_IRQ_FATAL		PORT_IRQ_TF_ERR | PORT_IRQ_HBUS_ERR	\
				| PORT_IRQ_HBUS_DATA_ERR | PORT_IRQ_IF_ERR

#define DEF_PORT_IRQ		PORT_IRQ_FATAL | PORT_IRQ_PHYRDY	\
				| PORT_IRQ_CONNECT | PORT_IRQ_SG_DONE	\
				| PORT_IRQ_UNK_FIS | PORT_IRQ_SDB_FIS	\
				| PORT_IRQ_DMAS_FIS | PORT_IRQ_PIOS_FIS	\
				| PORT_IRQ_D2H_REG_FIS

/* PORT_CMD bits */
#define PORT_CMD_ATAPI		(1 << 24) /* Device is ATAPI */
#define PORT_CMD_LIST_ON	(1 << 15) /* cmd list DMA engine running */
#define PORT_CMD_FIS_ON		(1 << 14) /* FIS DMA engine running */
#define PORT_CMD_FIS_RX		(1 << 4) /* Enable FIS receive DMA engine */
#define PORT_CMD_CLO		(1 << 3) /* Command list override */
#define PORT_CMD_POWER_ON	(1 << 2) /* Power up device */
#define PORT_CMD_SPIN_UP	(1 << 1) /* Spin up device */
#define PORT_CMD_START		(1 << 0) /* Enable port DMA engine */

#define PORT_CMD_ICC_ACTIVE	(0x1 << 28) /* Put i/f in active state */
#define PORT_CMD_ICC_PARTIAL	(0x2 << 28) /* Put i/f in partial state */
#define PORT_CMD_ICC_SLUMBER	(0x6 << 28) /* Put i/f in slumber state */

#define AHCI_MAX_PORTS		32

/* SETFEATURES stuff */
#define SETFEATURES_XFER	0x03
#define XFER_UDMA_7		0x47
#define XFER_UDMA_6		0x46
#define XFER_UDMA_5		0x45
#define XFER_UDMA_4		0x44
#define XFER_UDMA_3		0x43
#define XFER_UDMA_2		0x42
#define XFER_UDMA_1		0x41
#define XFER_UDMA_0		0x40
#define XFER_MW_DMA_2		0x22
#define XFER_MW_DMA_1		0x21
#define XFER_MW_DMA_0		0x20
#define XFER_SW_DMA_2		0x12
#define XFER_SW_DMA_1		0x11
#define XFER_SW_DMA_0		0x10
#define XFER_PIO_4		0x0C
#define XFER_PIO_3		0x0B
#define XFER_PIO_2		0x0A
#define XFER_PIO_1		0x09
#define XFER_PIO_0		0x08
#define XFER_PIO_SLOW		0x00

#define ATA_FLAG_SATA		(1 << 3)
#define ATA_FLAG_NO_LEGACY	(1 << 4) /* no legacy mode check */
#define ATA_FLAG_MMIO		(1 << 6) /* use MMIO, not PIO */
#define ATA_FLAG_SATA_RESET	(1 << 7) /* (obsolete) use COMRESET */
#define ATA_FLAG_PIO_DMA	(1 << 8) /* PIO cmds via DMA */
#define ATA_FLAG_NO_ATAPI	(1 << 11) /* No ATAPI support */

struct ahci_cmd_hdr {
	u32	opts;
	u32	status;
	u32	tbl_addr;
	u32	tbl_addr_hi;
	u32	reserved[4];
};

struct ahci_sg {
	u32	addr;
	u32	addr_hi;
	u32	reserved;
	u32	flags_size;
};

struct ahci_ioports {
	u32	cmd_addr;
	u32	scr_addr;
	u32	port_mmio;
	struct ahci_cmd_hdr	*cmd_slot;
	struct ahci_sg		*cmd_tbl_sg;
	u32	cmd_tbl;
	u32	rx_fis;
};

struct ahci_probe_ent {
	pci_dev_t	dev;
	struct ahci_ioports	port[AHCI_MAX_PORTS];
	u32	n_ports;
	u32	hard_port_no;
	u32	host_flags;
	u32	host_set_flags;
	u32	mmio_base;
	u32     pio_mask;
	u32	udma_mask;
	u32	flags;
	u32	cap;	/* cache of HOST_CAP register */
	u32	port_map; /* cache of HOST_PORTS_IMPL reg */
	u32	link_port_map; /*linkup port map*/
};

int ahci_init(u32 base);

#endif
