/*
 * Copyright (c) 2015 The Linux Foundation. All rights reserved.

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

#ifndef _IPQ40XX_EDMA_ETH_H
#define _IPQ40XX_EDMA_ETH_H
#include <common.h>
#include <net.h>
#include <configs/ipq40xx_cdp.h>
#include <phy.h>
#include <malloc.h>

#define IPQ40XX_EDMA_DEV		1
#define IPQ40XX_EDMA_TX_QUEUE		1
#define IPQ40XX_EDMA_RX_QUEUE		1
#define IPQ40XX_EDMA_TX_RING_SIZE	8
#define IPQ40XX_EDMA_RX_RING_SIZE	16
#define IPQ40XX_EDMA_TX_IMR_NORMAL_MASK	1
#define IPQ40XX_EDMA_RX_IMR_NORMAL_MASK	1
#define IPQ40XX_EDMA_RX_BUFF_SIZE	1540
#define IPQ40XX_EDMA_INTR_CLEAR_TYPE	0
#define IPQ40XX_EDMA_INTR_SW_IDX_W_TYPE	0
#define IPQ40XX_EDMA_RSS_TYPE_NONE	0x1

#define IPQ40XX_EDMA_TX_PER_CPU_MASK		0xF
#define IPQ40XX_EDMA_TX_PER_CPU_MASK_SHIFT	0x2
#define IPQ40XX_EDMA_RX_PER_CPU_MASK		0x3
#define IPQ40XX_EDMA_RX_PER_CPU_MASK_SHIFT	0x1
#define IPQ40XX_EDMA_TX_CPU_START_SHIFT		0x2
#define IPQ40XX_EDMA_RX_CPU_START_SHIFT		0x1

#define PSGMIIPHY_PLL_VCO_RELATED_CTRL	0x0009878c
#define PSGMIIPHY_PLL_VCO_VAL		0x2803

#define PSGMIIPHY_VCO_CALIBRATION_CTRL	0x0009809c
#define PSGMIIPHY_VCO_VAL	0x4ADA
#define PSGMIIPHY_VCO_RST_VAL	0xADA

#define RGMII_TCSR_ESS_CFG	0x01953000
#define ESS_RGMII_CTRL		0x0C000004

/* edma transmit descriptor */
struct edma_tx_desc {
	__le16  len; /* full packet including CRC */
	__le16  svlan_tag; /* vlan tag */
	__le32  word1; /* byte 4-7 */
	__le32  addr; /* address of buffer */
	__le32  word3; /* byte 12 */
};

/* edma receive return descriptor */
struct edma_rx_return_desc {
	__le32  word0;
	__le32  word1;
	__le32  word2;
	__le32  word3;
};

/* RFD descriptor */
struct edma_rx_free_desc {
        __le32  buffer_addr; /* buffer address */
};

/* edma_sw_desc stores software descriptor
 * SW descriptor has 1:1 map with HW descriptor
 */
struct edma_sw_desc {
	uchar *data;
	dma_addr_t dma; /* dma address */
	u16 length; /* Tx/Rx buffer length */
	u32 flags;
};

/* per core queue related information */
struct queue_per_cpu_info {
	u32 tx_mask; /* tx interrupt mask */
	u32 rx_mask; /* rx interrupt mask */
	u32 tx_status; /* tx interrupt status */
	u32 rx_status; /* rx interrupt status */
	u32 tx_start; /* tx queue start */
	u32 rx_start; /* rx queue start */
	struct ipq40xx_edma_common_info *c_info; /* edma common info */
};

/* edma hw specific data */
struct ipq40xx_edma_hw {
	unsigned long  __iomem *hw_addr; /* inner register address */
	struct edma_adapter *adapter; /* netdevice adapter */
	u32 rx_intr_mask; /*rx interrupt mask */
	u32 tx_intr_mask; /* tx interrupt nask */
	u32 misc_intr_mask; /* misc interrupt mask */
	u32 wol_intr_mask; /* wake on lan interrupt mask */
	u8 intr_clear_type; /* interrupt clear */
	u8 intr_sw_idx_w; /* To do chk type interrupt software index */
	u16 rx_buff_size; /* To do chk type Rx buffer size */
	u8 rss_type; /* rss protocol type */
};

/* transimit packet descriptor (tpd) ring */
struct ipq40xx_edma_desc_ring {
	u8 queue_index; /* queue index */
	u16 size; /* descriptor ring length in bytes */
	u16 count; /* number of descriptors in the ring */
	void *hw_desc; /* descriptor ring virtual address */
	dma_addr_t dma; /* descriptor ring physical address */
	u16 sw_next_to_fill; /* next Tx descriptor to fill */
	u16 sw_next_to_clean; /* next Tx descriptor to clean */
	struct edma_sw_desc *sw_desc; /* buffer associated with ring */
};

struct ipq40xx_edma_common_info {
	struct ipq40xx_edma_desc_ring *tpd_ring[IPQ40XX_EDMA_TX_QUEUE];
	struct ipq40xx_edma_desc_ring *rfd_ring[IPQ40XX_EDMA_RX_QUEUE];
	int num_rx_queues; /* number of rx queue */
	int num_tx_queues; /* number of tx queue */
	u16 tx_ring_count;
	u16 rx_ring_count;
	u16 rx_buffer_len;
	struct ipq40xx_edma_hw hw;
	struct queue_per_cpu_info q_cinfo[IPQ40XX_EDMA_DEV];
};

struct ipq40xx_eth_dev {
	u8 *phy_address;
	uint no_of_phys;
	uint interface;
	uint speed;
	uint duplex;
	uint sw_configured;
	uint mac_unit;
	uint mac_ps;
	int link_printed;
	u32 padding;
	struct eth_device *dev;
	struct ipq40xx_edma_common_info *c_info;
	struct phy_ops *ops;
	const char phy_name[MDIO_NAME_LEN];
} __attribute__ ((aligned(8)));

static int edma_hw_addr;

static inline void ipq40xx_edma_write_reg(u16 reg_addr, u32 reg_value)
{
	writel(reg_value, ((void __iomem *)(edma_hw_addr + reg_addr)));
}
static inline void ipq40xx_edma_read_reg(u16 reg_addr, volatile u32 *reg_value)
{
	*reg_value = readl((void __iomem *)(edma_hw_addr + reg_addr));
}

static inline void* ipq40xx_alloc_mem(u32 size)
{
	return malloc(size);
}

static inline void ipq40xx_free_mem(void *ptr)
{
	if (ptr)
		free(ptr);
}
#endif	/* _IPQ40XX_EDMA_H */
