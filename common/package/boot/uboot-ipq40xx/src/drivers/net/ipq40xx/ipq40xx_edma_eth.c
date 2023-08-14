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

#include <common.h>
#include <net.h>
#include <asm-generic/errno.h>
#include <asm/io.h>
#include <malloc.h>
#include <phy.h>
#include <net.h>
#include <miiphy.h>
#include <asm/arch-ipq40xx/ess/ipq40xx_edma.h>
#include "ipq40xx_edma_eth.h"
#include "ipq40xx_cdp.h"
#ifdef DEBUG
#define debugf(fmt, args...) printf(fmt, ##args);
#else
#define debugf(fmt, args...)
#endif

extern int ipq40xx_ess_sw_init(ipq40xx_edma_board_cfg_t *cfg);
extern void ipq40xx_ess_enable_lookup(void);
extern void ipq40xx_ess_disable_lookup(void);
extern void psgmii_self_test(void);
extern void clear_self_test_config(void);
uchar ipq40xx_def_enetaddr[6] = {0x00, 0x03, 0x7F, 0xBA, 0xDB, 0xAD};
static struct ipq40xx_eth_dev *ipq40xx_edma_dev[IPQ40XX_EDMA_DEV];
static int (*ipq40xx_switch_init)(struct ipq40xx_eth_dev *cfg);
extern void qca8075_ess_reset(void);

void ipq40xx_register_switch(int(*sw_init)(struct ipq40xx_eth_dev *cfg))
{
	ipq40xx_switch_init = sw_init;
}

/*
 * Enable RX queue control
 */
static void ipq40xx_edma_enable_rx_ctrl(struct ipq40xx_edma_hw *hw)
{
	volatile u32 data;

	ipq40xx_edma_read_reg(EDMA_REG_RXQ_CTRL, &data);
	data |= EDMA_RXQ_CTRL_EN;
	ipq40xx_edma_write_reg(EDMA_REG_RXQ_CTRL, data);
}

/*
 * Enable TX queue control
 */
static void ipq40xx_edma_enable_tx_ctrl(struct ipq40xx_edma_hw *hw)
{
	volatile u32 data;

	ipq40xx_edma_read_reg(EDMA_REG_TXQ_CTRL, &data);
	data |= EDMA_TXQ_CTRL_TXQ_EN;
	ipq40xx_edma_write_reg(EDMA_REG_TXQ_CTRL, data);
}

/*
 * ipq40xx_edma_init_desc()
 * Update descriptor ring size,
 * Update buffer and producer/consumer index
 */
static void ipq40xx_edma_init_desc(
	struct ipq40xx_edma_common_info *c_info, u8 unit)
{
	struct ipq40xx_edma_desc_ring *rfd_ring;
	struct ipq40xx_edma_desc_ring *etdr;
	volatile u32 data = 0;
	u16 hw_cons_idx = 0;

	/*
	 * Set the base address of every TPD ring.
	 */
	etdr = c_info->tpd_ring[unit];
	/*
	 * Update TX descriptor ring base address.
	 */
	ipq40xx_edma_write_reg(EDMA_REG_TPD_BASE_ADDR_Q(unit),
		(u32)(etdr->dma & 0xffffffff));
	ipq40xx_edma_read_reg(EDMA_REG_TPD_IDX_Q(unit), &data);
	/*
	 * Calculate hardware consumer index for Tx.
	 */
	hw_cons_idx = (data >> EDMA_TPD_CONS_IDX_SHIFT) & 0xffff;
	etdr->sw_next_to_fill = hw_cons_idx;
	etdr->sw_next_to_clean = hw_cons_idx;
	data &= ~(EDMA_TPD_PROD_IDX_MASK << EDMA_TPD_PROD_IDX_SHIFT);
	data |= hw_cons_idx;
	/*
	 * Update producer index for Tx.
	 */
	ipq40xx_edma_write_reg(EDMA_REG_TPD_IDX_Q(unit), data);
	/*
	 * Update SW consumer index register for Tx.
	 */
	ipq40xx_edma_write_reg(EDMA_REG_TX_SW_CONS_IDX_Q(unit),
						hw_cons_idx);
	/*
	 * Set TPD ring size.
	 */
	ipq40xx_edma_write_reg(EDMA_REG_TPD_RING_SIZE,
		(u32)(c_info->tx_ring_count & EDMA_TPD_RING_SIZE_MASK));
	/*
	 * Configure Rx ring.
	 */
	rfd_ring = c_info->rfd_ring[unit];
	/*
	 * Update Receive Free descriptor ring base address.
	 */
	ipq40xx_edma_write_reg(EDMA_REG_RFD_BASE_ADDR_Q(unit),
		(u32)(rfd_ring->dma & 0xffffffff));
	ipq40xx_edma_read_reg(EDMA_REG_RFD_BASE_ADDR_Q(unit), &data);
	/*
	 * Update RFD ring size and RX buffer size.
	 */
	data = (c_info->rx_ring_count & EDMA_RFD_RING_SIZE_MASK)
				<< EDMA_RFD_RING_SIZE_SHIFT;
	data |= (c_info->rx_buffer_len & EDMA_RX_BUF_SIZE_MASK)
				<< EDMA_RX_BUF_SIZE_SHIFT;
	ipq40xx_edma_write_reg(EDMA_REG_RX_DESC0, data);
	/*
	 * Disable TX FIFO low watermark and high watermark
	 */
	ipq40xx_edma_write_reg(EDMA_REG_TXF_WATER_MARK, 0);
	/*
	 * Load all of base address above
	 */
	ipq40xx_edma_read_reg(EDMA_REG_TX_SRAM_PART, &data);
	data |= 1 << EDMA_LOAD_PTR_SHIFT;
	ipq40xx_edma_write_reg(EDMA_REG_TX_SRAM_PART, data);
}

/*
 * ipq40xx_edma_alloc_rx_buf()
 * Allocates buffer for the received packets.
 */
static int ipq40xx_edma_alloc_rx_buf(
	struct ipq40xx_edma_common_info *c_info,
        struct ipq40xx_edma_desc_ring *erdr,
	u32 cleaned_count, u8 queue_id)
{
	struct edma_rx_free_desc *rx_desc;
	struct edma_sw_desc *sw_desc;
	unsigned int i;
	u16 prod_idx, length;
	u32 reg_data;

	if (cleaned_count > erdr->count) {
		debugf("Incorrect cleaned_count %d", cleaned_count);
		return -1;
	}
	i = erdr->sw_next_to_fill;

	while (cleaned_count--) {
		sw_desc = &erdr->sw_desc[i];
		length = c_info->rx_buffer_len;

		sw_desc->dma = virt_to_phys(NetRxPackets[i]);
		/*
		 * Update the buffer info.
		 */
		sw_desc->data = NetRxPackets[i];
		sw_desc->length = length;
		rx_desc = (&((struct edma_rx_free_desc *)(erdr->hw_desc))[i]);
		rx_desc->buffer_addr = cpu_to_le64(sw_desc->dma);
		if (unlikely(++i == erdr->count))
			i = 0;
	}
	erdr->sw_next_to_fill = i;

	if (unlikely(i == 0))
		prod_idx = erdr->count - 1;
	else
		prod_idx = i - 1;

	/* Update the producer index */
	ipq40xx_edma_read_reg(EDMA_REG_RFD_IDX_Q(queue_id), &reg_data);
	reg_data &= ~EDMA_RFD_PROD_IDX_BITS;
	reg_data |= prod_idx;
	ipq40xx_edma_write_reg(EDMA_REG_RFD_IDX_Q(queue_id), reg_data);
	return 0;
}

/*
 * configure reception control data.
 */
static void ipq40xx_edma_configure_rx(
		struct ipq40xx_edma_common_info *c_info)
{
	struct ipq40xx_edma_hw *hw = &c_info->hw;
	u32 rss_type, rx_desc1, rxq_ctrl_data;

	/*
	 * Set RSS type
	 */
	rss_type = hw->rss_type;
	ipq40xx_edma_write_reg(EDMA_REG_RSS_TYPE, rss_type);
	/*
	 * Set RFD burst number
	 */
	rx_desc1 = (EDMA_RFD_BURST << EDMA_RXQ_RFD_BURST_NUM_SHIFT);
	/*
	 * Set RFD prefetch threshold
	 */
	rx_desc1 |= (EDMA_RFD_THR << EDMA_RXQ_RFD_PF_THRESH_SHIFT);
	/*
	 * Set RFD in host ring low threshold to generte interrupt
	 */
	rx_desc1 |= (EDMA_RFD_LTHR << EDMA_RXQ_RFD_LOW_THRESH_SHIFT);
	ipq40xx_edma_write_reg(EDMA_REG_RX_DESC1, rx_desc1);
	/*
	 * Set Rx FIFO threshold to start to DMA data to host
	 */
	rxq_ctrl_data = EDMA_FIFO_THRESH_128_BYTE;
	/*
	 * Set RX remove vlan bit
	 */
	rxq_ctrl_data |= EDMA_RXQ_CTRL_RMV_VLAN;
	ipq40xx_edma_write_reg(EDMA_REG_RXQ_CTRL, rxq_ctrl_data);
}

/*
 * Configure transmission control data
 */
static void ipq40xx_edma_configure_tx(
		struct ipq40xx_edma_common_info *c_info)
{
	u32 txq_ctrl_data;

	txq_ctrl_data = (EDMA_TPD_BURST << EDMA_TXQ_NUM_TPD_BURST_SHIFT);
	txq_ctrl_data |=
		EDMA_TXQ_CTRL_TPD_BURST_EN;
	txq_ctrl_data |=
		(EDMA_TXF_BURST << EDMA_TXQ_TXF_BURST_NUM_SHIFT);
	ipq40xx_edma_write_reg(EDMA_REG_TXQ_CTRL, txq_ctrl_data);
}

static int ipq40xx_edma_configure(
	struct ipq40xx_edma_common_info *c_info)
{
	struct ipq40xx_edma_hw *hw = &c_info->hw;
	u32 intr_modrt_data;
	u32 intr_ctrl_data = 0;

	ipq40xx_edma_read_reg(EDMA_REG_INTR_CTRL, &intr_ctrl_data);
	intr_ctrl_data &= ~(1 << EDMA_INTR_SW_IDX_W_TYP_SHIFT);
	intr_ctrl_data |=
		hw->intr_sw_idx_w << EDMA_INTR_SW_IDX_W_TYP_SHIFT;
	ipq40xx_edma_write_reg(EDMA_REG_INTR_CTRL, intr_ctrl_data);

	/* clear interrupt status */
	ipq40xx_edma_write_reg(EDMA_REG_RX_ISR, 0xff);
	ipq40xx_edma_write_reg(EDMA_REG_TX_ISR, 0xffff);
	ipq40xx_edma_write_reg(EDMA_REG_MISC_ISR, 0x1fff);
	ipq40xx_edma_write_reg(EDMA_REG_WOL_ISR, 0x1);

	/* Clear any WOL status */
	ipq40xx_edma_write_reg(EDMA_REG_WOL_CTRL, 0);
	intr_modrt_data = (EDMA_TX_IMT << EDMA_IRQ_MODRT_TX_TIMER_SHIFT);
	intr_modrt_data |= (EDMA_RX_IMT << EDMA_IRQ_MODRT_RX_TIMER_SHIFT);
	ipq40xx_edma_write_reg(EDMA_REG_IRQ_MODRT_TIMER_INIT,
						intr_modrt_data);

	ipq40xx_edma_configure_tx(c_info);
	ipq40xx_edma_configure_rx(c_info);
	return 0;
}

static void ipq40xx_edma_stop_rx_tx(struct ipq40xx_edma_hw *hw)
{
	volatile u32 data;

	ipq40xx_edma_read_reg(EDMA_REG_RXQ_CTRL, &data);
	data &= ~EDMA_RXQ_CTRL_EN;
	ipq40xx_edma_write_reg(EDMA_REG_RXQ_CTRL, data);
	ipq40xx_edma_read_reg(EDMA_REG_TXQ_CTRL, &data);
	data &= ~EDMA_TXQ_CTRL_TXQ_EN;
	ipq40xx_edma_write_reg(EDMA_REG_TXQ_CTRL, data);
}

static int ipq40xx_edma_reset(struct ipq40xx_edma_common_info *c_info)
{
	struct ipq40xx_edma_hw *hw = &c_info->hw;
	int i;

	for (i = 0; i < IPQ40XX_EDMA_DEV; i++)
		ipq40xx_edma_write_reg(EDMA_REG_RX_INT_MASK_Q(i), 0);

	for (i = 0; i < IPQ40XX_EDMA_DEV; i++)
		ipq40xx_edma_write_reg(EDMA_REG_TX_INT_MASK_Q(i), 0);

	ipq40xx_edma_write_reg(EDMA_REG_MISC_IMR, 0);
	ipq40xx_edma_write_reg(EDMA_REG_WOL_IMR, 0);
	ipq40xx_edma_write_reg(EDMA_REG_RX_ISR, 0xff);
	ipq40xx_edma_write_reg(EDMA_REG_TX_ISR, 0xffff);
	ipq40xx_edma_write_reg(EDMA_REG_MISC_ISR, 0x1fff);
	ipq40xx_edma_write_reg(EDMA_REG_WOL_ISR, 0x1);
	ipq40xx_edma_stop_rx_tx(hw);
	return 0;
}

/*
 * ipq40xx_edma_get_tx_buffer()
 * Get sw_desc corresponding to the TPD
 */
static struct edma_sw_desc *ipq40xx_edma_get_tx_buffer(
		struct ipq40xx_edma_common_info *c_info,
		struct edma_tx_desc *tpd, int queue_id)
{
	struct ipq40xx_edma_desc_ring *etdr =
		c_info->tpd_ring[queue_id];
	return &etdr->sw_desc[tpd -
		(struct edma_tx_desc *)etdr->hw_desc];
}

/*
 * edma_get_next_tpd()
 * Return a TPD descriptor for transfer
 */
static struct edma_tx_desc *ipq40xx_edma_get_next_tpd(
		struct ipq40xx_edma_common_info *c_info,
		               		int queue_id)
{
	struct ipq40xx_edma_desc_ring *etdr =
		c_info->tpd_ring[queue_id];
	u16 sw_next_to_fill = etdr->sw_next_to_fill;
	struct edma_tx_desc *tpd_desc =
	(&((struct edma_tx_desc *)(etdr->hw_desc))[sw_next_to_fill]);
	etdr->sw_next_to_fill =
	(etdr->sw_next_to_fill + 1) % etdr->count;
	return tpd_desc;
}

/*
 * ipq40xx_edma_tx_update_hw_idx()
 * update the producer index for the ring transmitted
 */
static void ipq40xx_edma_tx_update_hw_idx(
		struct ipq40xx_edma_common_info *c_info,
                void *skb, int queue_id)
{
	struct ipq40xx_edma_desc_ring *etdr =
		c_info->tpd_ring[queue_id];
	volatile u32 tpd_idx_data;

	/* Read and update the producer index */
	ipq40xx_edma_read_reg(
		EDMA_REG_TPD_IDX_Q(queue_id), &tpd_idx_data);
	tpd_idx_data &= ~EDMA_TPD_PROD_IDX_BITS;
	tpd_idx_data |=
		((etdr->sw_next_to_fill & EDMA_TPD_PROD_IDX_MASK)
		<< EDMA_TPD_PROD_IDX_SHIFT);
	ipq40xx_edma_write_reg(
		EDMA_REG_TPD_IDX_Q(queue_id), tpd_idx_data);
}

/*
 * ipq40xx_edma_tx_map_and_fill()
 * gets called from edma_xmit_frame
 * This is where the dma of the buffer to be transmitted
 * gets mapped
 */
static int ipq40xx_edma_tx_map_and_fill(
		struct ipq40xx_edma_common_info *c_info,
                void *skb, int queue_id,
		unsigned int flags_transmit,
		unsigned int length)
{
	struct edma_sw_desc *sw_desc = NULL;
	struct edma_tx_desc *tpd = NULL;
	u32 word1 = 0, word3 = 0, lso_word1 = 0, svlan_tag = 0;
	u16 buf_len = length;

	if (likely (buf_len)) {
		tpd = ipq40xx_edma_get_next_tpd(c_info, queue_id);
		sw_desc = ipq40xx_edma_get_tx_buffer(c_info, tpd, queue_id);
		sw_desc->dma = virt_to_phys(skb);


		tpd->addr = cpu_to_le32(sw_desc->dma);
		tpd->len  = cpu_to_le16(buf_len);

		word3 |= EDMA_PORT_ENABLE_ALL << EDMA_TPD_PORT_BITMAP_SHIFT;

		tpd->svlan_tag = svlan_tag;
		tpd->word1 = word1 | lso_word1;
		tpd->word3 = word3;

		/* The last buffer info contain the skb address,
		 * so it will be free after unmap
		 */
		sw_desc->length = buf_len;
		sw_desc->flags |= EDMA_SW_DESC_FLAG_SKB_HEAD;
		tpd->word1 |= 1 << EDMA_TPD_EOP_SHIFT;

		sw_desc->data = skb;
		sw_desc->flags |= EDMA_SW_DESC_FLAG_LAST;
	}
	return 0;
}

/*
 * ipq40xx_edma_tpd_available()
 * Check number of free TPDs
 */
static inline u16 ipq40xx_edma_tpd_available(
		struct ipq40xx_edma_common_info *c_info,
                int queue_id)
{
	struct ipq40xx_edma_desc_ring *etdr =
			c_info->tpd_ring[queue_id];
	u16 sw_next_to_fill;
	u16 sw_next_to_clean;
	u16 count = 0;

	sw_next_to_clean = etdr->sw_next_to_clean;
	sw_next_to_fill = etdr->sw_next_to_fill;

	if (likely(sw_next_to_clean <= sw_next_to_fill))
		count = etdr->count;

	return count + sw_next_to_clean - sw_next_to_fill - 1;
}

static inline void ipq40xx_edma_tx_unmap_and_free(
		struct edma_sw_desc *sw_desc)
{
	sw_desc->flags = 0;
}

/*
 * ipq40xx_edma_tx_complete()
 * used to clean tx queues and
 * update hardware and consumer index
 */
static void ipq40xx_edma_tx_complete(
		struct ipq40xx_edma_common_info *c_info,
		int queue_id)
{
	struct ipq40xx_edma_desc_ring *etdr = c_info->tpd_ring[queue_id];
	struct edma_sw_desc *sw_desc;

	u16 sw_next_to_clean = etdr->sw_next_to_clean;
	u16 hw_next_to_clean = 0;
	volatile u32 data = 0;
	ipq40xx_edma_read_reg(EDMA_REG_TPD_IDX_Q(queue_id), &data);
	hw_next_to_clean =
		(data >> EDMA_TPD_CONS_IDX_SHIFT) & EDMA_TPD_CONS_IDX_MASK;
	/* clean the buffer here */
	while (sw_next_to_clean != hw_next_to_clean) {
		sw_desc = &etdr->sw_desc[sw_next_to_clean];
		ipq40xx_edma_tx_unmap_and_free(sw_desc);
		sw_next_to_clean = (sw_next_to_clean + 1) % etdr->count;
		etdr->sw_next_to_clean = sw_next_to_clean;
	}
	/* update the TPD consumer index register */
	ipq40xx_edma_write_reg(
		EDMA_REG_TX_SW_CONS_IDX_Q(queue_id), sw_next_to_clean);

}

/*
 * ipq40xx_edma_rx_complete()
 */
static int ipq40xx_edma_rx_complete(
		struct ipq40xx_edma_common_info *c_info,
		int queue_id)
{
	u16 cleaned_count = 0;
	u16 length = 0;
	int i = 0;
	u8 rrd[16];
	volatile u32 data = 0;
	u16 hw_next_to_clean = 0;
	u16 sw_next_to_clean = 0;
	struct ipq40xx_edma_desc_ring *erdr = c_info->rfd_ring[queue_id];
	struct edma_sw_desc *sw_desc;
	uchar *skb;
	int rx = CONFIG_SYS_RX_ETH_BUFFER;
	sw_next_to_clean = erdr->sw_next_to_clean;

	while (rx) {
		sw_desc = &erdr->sw_desc[sw_next_to_clean];
		ipq40xx_edma_read_reg(EDMA_REG_RFD_IDX_Q(queue_id), &data);
		hw_next_to_clean = (data >> RFD_CONS_IDX_SHIFT) &
		                              RFD_CONS_IDX_MASK;

		if (hw_next_to_clean == sw_next_to_clean) {
			break;
		}
		skb = sw_desc->data;

		/* Get RRD */
		for (i = 0; i < 16; i++)
			rrd[i] = skb[i];

		/* use next descriptor */
		sw_next_to_clean = (sw_next_to_clean + 1) % erdr->count;
		cleaned_count++;

		/* Check if RRD is valid */
		if (rrd[15] & 0x80) {
			/* Get the packet size and allocate buffer */
			length = ((rrd[13] & 0x3f) << 8) + rrd[12];
			/* Get the number of RFD from RRD */
		}
		skb = skb + 16;
		NetReceive(skb, length);
		rx--;
	}
	erdr->sw_next_to_clean = sw_next_to_clean;
	/* alloc_rx_buf */
	if (cleaned_count) {
		ipq40xx_edma_alloc_rx_buf(c_info, erdr,
				cleaned_count, queue_id);
		ipq40xx_edma_write_reg(EDMA_REG_RX_SW_CONS_IDX_Q(queue_id),
			erdr->sw_next_to_clean);
	}
	return 0;
}

static int ipq40xx_eth_recv(struct eth_device *dev)
{
	struct ipq40xx_eth_dev *priv = dev->priv;
	struct ipq40xx_edma_common_info *c_info = priv->c_info;
	struct queue_per_cpu_info *q_cinfo = c_info->q_cinfo;
	volatile u32 reg_data;
	u32 shadow_rx_status;

	ipq40xx_edma_read_reg(EDMA_REG_RX_ISR, &reg_data);
	q_cinfo->rx_status |= reg_data & q_cinfo->rx_mask;
	shadow_rx_status = q_cinfo->rx_status;

	ipq40xx_edma_rx_complete(c_info, priv->mac_unit);
	ipq40xx_edma_write_reg(EDMA_REG_RX_ISR, shadow_rx_status);
	return 0;
}

static int ipq40xx_edma_wr_macaddr(struct eth_device *dev)
{
	return 0;
}

static int ipq40xx_eth_init(struct eth_device *eth_dev, bd_t *this)
{
	struct ipq40xx_eth_dev *priv = eth_dev->priv;
	struct ipq40xx_edma_common_info *c_info = priv->c_info;
	struct ipq40xx_edma_desc_ring *ring;
	struct ipq40xx_edma_hw *hw;
	struct phy_ops *phy_get_ops;
	static int linkup = 0;
	int i;
	u8 status;
	fal_port_speed_t speed;
	fal_port_duplex_t duplex;
	char *lstatus[] = {"up", "Down"};
	char *dp[] = {"Half", "Full"};
	hw = &c_info->hw;
	/*
	 * Allocate the RX buffer
	 * Qid is based on mac unit.
	 */
	ring = c_info->rfd_ring[priv->mac_unit];
	ipq40xx_edma_alloc_rx_buf(c_info, ring, ring->count,
					priv->mac_unit);
	if (!priv->ops) {
		printf ("Phy ops not mapped\n");
		return -1;
	}
	phy_get_ops = priv->ops;
	if (!phy_get_ops->phy_get_link_status ||
	    !phy_get_ops->phy_get_speed ||
	    !phy_get_ops->phy_get_duplex) {
		printf ("Link status/Get speed/Get duplex not mapped\n");
		return -1;
	}
	/*
	 * Check PHY link, speed, Duplex on all phys.
	 * we will proceed even if single link is up
	 * else we will return with -1;
	 */
	for (i =  0; i < PHY_MAX; i++) {
		status = phy_get_ops->phy_get_link_status(priv->mac_unit, i);
		if (status == 0)
			linkup++;
		phy_get_ops->phy_get_speed(priv->mac_unit, i, &speed);
		phy_get_ops->phy_get_duplex(priv->mac_unit, i, &duplex);
		switch (speed) {
		case FAL_SPEED_10:
		case FAL_SPEED_100:
		case FAL_SPEED_1000:
			printf ("eth%d PHY%d %s Speed :%d %s duplex\n",
				priv->mac_unit, i, lstatus[status], speed,
				dp[duplex]);
			break;
		default:
			printf("Unknown speed\n");
			break;
		}
	}

	if (linkup <= 0) {
		/* No PHY link is alive */
		return -1;
	} else {
		/* reset the flag */
		linkup = 0;
	}
	/* Configure RSS indirection table.
	 * 128 hash will be configured in the following
	 * pattern: hash{0,1,2,3} = {Q0,Q2,Q4,Q6} respectively
	 * and so on
	 */
	for (i = 0; i < EDMA_NUM_IDT; i++)
		ipq40xx_edma_write_reg(EDMA_REG_RSS_IDT(i), EDMA_RSS_IDT_VALUE);

	ipq40xx_edma_enable_tx_ctrl(hw);
	ipq40xx_edma_enable_rx_ctrl(hw);
	ipq40xx_ess_enable_lookup();
	return 0;
}

static int ipq40xx_eth_snd(struct eth_device *dev, void *packet, int length)
{
	int num_tpds_needed;
	struct ipq40xx_eth_dev *priv = dev->priv;
	struct ipq40xx_edma_common_info *c_info = priv->c_info;
	struct queue_per_cpu_info *q_cinfo = c_info->q_cinfo;
	unsigned int flags_transmit = 0;
	u32 shadow_tx_status, reg_data;

	num_tpds_needed = 1;

	if ((num_tpds_needed >
	ipq40xx_edma_tpd_available(c_info, priv->mac_unit))) {
		debugf("Not enough descriptors available");
		return NETDEV_TX_BUSY;
	}

	flags_transmit |= EDMA_HW_CHECKSUM;
	ipq40xx_edma_tx_map_and_fill(c_info,
			packet, priv->mac_unit,
			flags_transmit, length);

	ipq40xx_edma_tx_update_hw_idx(c_info,
			packet, priv->mac_unit);

	/* Check for tx dma completion */
	ipq40xx_edma_read_reg(EDMA_REG_TX_ISR, &reg_data);
	q_cinfo->tx_status |= reg_data & q_cinfo->tx_mask;
	shadow_tx_status = q_cinfo->tx_status;

	ipq40xx_edma_tx_complete(c_info, priv->mac_unit);
	ipq40xx_edma_write_reg(EDMA_REG_TX_ISR, shadow_tx_status);
	return 0;
}

static void ipq40xx_eth_halt(struct eth_device *dev)
{
	struct ipq40xx_eth_dev *priv = dev->priv;
	struct ipq40xx_edma_common_info *c_info = priv->c_info;

	ipq40xx_ess_disable_lookup();
	ipq40xx_edma_reset(c_info);
}

/*
 * Free Tx and Rx rings
 */
static void ipq40xx_edma_free_rings(
		struct ipq40xx_edma_common_info *c_info)
{
	int i;
	struct ipq40xx_edma_desc_ring *etdr;
	struct ipq40xx_edma_desc_ring *rxdr;

	for (i = 0; i < c_info->num_tx_queues; i++) {
		if (!c_info->tpd_ring[i])
			continue;
		etdr = c_info->tpd_ring[i];
		if (etdr->hw_desc)
			ipq40xx_free_mem(etdr->hw_desc);
		if (etdr->sw_desc)
			ipq40xx_free_mem(etdr->sw_desc);
	}

	for (i = 0; i < c_info->num_rx_queues; i++) {
		if (!c_info->tpd_ring[i])
			continue;
		rxdr = c_info->rfd_ring[i];
		if (rxdr->hw_desc)
			ipq40xx_free_mem(rxdr->hw_desc);
		if (rxdr->sw_desc)
			ipq40xx_free_mem(rxdr->sw_desc);
	}
}

/*
 * ipq40xx_edma_alloc_ring()
 * allocate edma ring descriptor.
 */
static int ipq40xx_edma_alloc_ring(
		struct ipq40xx_edma_common_info *c_info,
		struct ipq40xx_edma_desc_ring *erd)
{
	erd->size = (sizeof(struct edma_sw_desc) * erd->count);
	erd->sw_next_to_fill = 0;
	erd->sw_next_to_clean = 0;
	/* Allocate SW descriptors */
	erd->sw_desc = ipq40xx_alloc_mem(erd->size);
	if (!erd->sw_desc)
		return -ENOMEM;

	 /* Alloc HW descriptors */
	erd->hw_desc = ipq40xx_alloc_mem(erd->size);
	erd->dma = virt_to_phys(erd->hw_desc);
	if (!erd->hw_desc) {
		ipq40xx_free_mem(erd->sw_desc);
		 return -ENOMEM;
	}
	return 0;

}

/*
 * ipq40xx_allocate_tx_rx_rings()
 */
static int ipq40xx_edma_alloc_tx_rx_rings(
		struct ipq40xx_edma_common_info *c_info)
{
	int i, ret;
	for (i = 0; i < c_info->num_tx_queues; i++) {
		ret = ipq40xx_edma_alloc_ring(c_info,
				c_info->tpd_ring[i]);
		if (ret)
			goto err_ring;
	}

	for (i = 0; i < c_info->num_rx_queues; i++) {
		ret = ipq40xx_edma_alloc_ring(c_info,
				c_info->rfd_ring[i]);
		if (ret)
			goto err_ring;
	}
	return 0;
err_ring:
	return -1;
}

/*
 * Free Tx and Rx Queues.
 */
static void ipq40xx_edma_free_queues(
		struct ipq40xx_edma_common_info *c_info)
{
	int i;
	for (i = 0; i < c_info->num_tx_queues; i++) {
		if (c_info->tpd_ring[i]) {
			ipq40xx_free_mem(c_info->tpd_ring[i]);
			c_info->tpd_ring[i] = NULL;
		}
	}

	for (i = 0; i < c_info->num_rx_queues; i++) {
		if (c_info->rfd_ring[i]) {
			ipq40xx_free_mem(c_info->rfd_ring[i]);
			c_info->rfd_ring[i] = NULL;
		}
	}

	c_info->num_tx_queues = 0;
	c_info->num_rx_queues = 0;
}

/*
 * Allocate Tx and Rx queues.
 */
static int ipq40xx_edma_alloc_tx_rx_queue(
		struct ipq40xx_edma_common_info *c_info)
{
	int i;
	struct ipq40xx_edma_desc_ring *etdr;
	struct ipq40xx_edma_desc_ring *rfd_ring;
	/* Tx queue allocation*/
	for (i = 0; i < c_info->num_tx_queues; i++) {
		etdr = ipq40xx_alloc_mem(
			sizeof(struct ipq40xx_edma_desc_ring));
		if (!etdr)
			goto err;
		etdr->count = c_info->tx_ring_count;
		c_info->tpd_ring[i] = etdr;
	}
	/* Rx Queue allocation */
	for (i = 0; i < c_info->num_rx_queues; i++) {
		rfd_ring = ipq40xx_alloc_mem(
			sizeof(struct ipq40xx_edma_desc_ring));
		if (!rfd_ring)
			goto err;
		rfd_ring->count = c_info->rx_ring_count;
		rfd_ring->queue_index = i;
		c_info->rfd_ring[i] = rfd_ring;
	}
	return 0;
err:
	return -1;
}

int ipq40xx_edma_init(ipq40xx_edma_board_cfg_t *edma_cfg)
{
	static int ipq40xx_ess_init_done = 0;
	static int cfg_edma  = 0;
	static int sw_init_done = 0;
	struct eth_device *dev[IPQ40XX_EDMA_DEV];
	struct ipq40xx_edma_common_info *c_info[IPQ40XX_EDMA_DEV];
	struct ipq40xx_edma_hw *hw[IPQ40XX_EDMA_DEV];
	uchar enet_addr[IPQ40XX_EDMA_DEV * 6];
	int i;
	int ret;

	memset(c_info, 0, (sizeof(c_info) * IPQ40XX_EDMA_DEV));
	memset(enet_addr, 0, sizeof(enet_addr));
	/* Getting the MAC address from ART partition */
	ret = get_eth_mac_address(enet_addr, IPQ40XX_EDMA_DEV);
	/*
	 * Register EDMA as single ethernet
	 * interface.
	 */
	for (i = 0; i < IPQ40XX_EDMA_DEV; edma_cfg++, i++) {
		dev[i] = ipq40xx_alloc_mem(sizeof(struct eth_device));
		if (!dev[i])
			goto failed;
		memset(dev[i], 0, sizeof(struct eth_device));

		c_info[i] = ipq40xx_alloc_mem(
			sizeof(struct ipq40xx_edma_common_info));
		if (!c_info[i])
			goto failed;
		memset(c_info[i], 0,
			sizeof(struct ipq40xx_edma_common_info));

		c_info[i]->num_tx_queues = IPQ40XX_EDMA_TX_QUEUE;
		c_info[i]->tx_ring_count = IPQ40XX_EDMA_TX_RING_SIZE;
		c_info[i]->num_rx_queues = IPQ40XX_EDMA_RX_QUEUE;
		c_info[i]->rx_ring_count = IPQ40XX_EDMA_RX_RING_SIZE;
		c_info[i]->rx_buffer_len = IPQ40XX_EDMA_RX_BUFF_SIZE;

		hw[i] = &c_info[i]->hw;

		hw[i]->tx_intr_mask = IPQ40XX_EDMA_TX_IMR_NORMAL_MASK;
		hw[i]->rx_intr_mask = IPQ40XX_EDMA_RX_IMR_NORMAL_MASK;
		hw[i]->rx_buff_size = IPQ40XX_EDMA_RX_BUFF_SIZE;
		hw[i]->misc_intr_mask = 0;
		hw[i]->wol_intr_mask = 0;
		hw[i]->intr_clear_type = IPQ40XX_EDMA_INTR_CLEAR_TYPE;
		hw[i]->intr_sw_idx_w = IPQ40XX_EDMA_INTR_SW_IDX_W_TYPE;
		hw[i]->rss_type = IPQ40XX_EDMA_RSS_TYPE_NONE;

		c_info[i]->hw.hw_addr = (unsigned long  __iomem *)
						IPQ40XX_EDMA_CFG_BASE;

		ipq40xx_edma_dev[i] = ipq40xx_alloc_mem(
				sizeof(struct ipq40xx_eth_dev));
		if (!ipq40xx_edma_dev[i])
			goto failed;
		memset (ipq40xx_edma_dev[i], 0,
				sizeof(struct ipq40xx_eth_dev));

		dev[i]->iobase = edma_cfg->base;
		dev[i]->init = ipq40xx_eth_init;
		dev[i]->halt = ipq40xx_eth_halt;
		dev[i]->recv = ipq40xx_eth_recv;
		dev[i]->send = ipq40xx_eth_snd;
		dev[i]->write_hwaddr = ipq40xx_edma_wr_macaddr;
		dev[i]->priv = (void *)ipq40xx_edma_dev[i];

		if ((ret < 0) ||
			(!is_valid_ether_addr(&enet_addr[edma_cfg->unit * 6]))) {
			memcpy(&dev[i]->enetaddr[0], ipq40xx_def_enetaddr, 6);
		} else {
			memcpy(&dev[i]->enetaddr[0],
				&enet_addr[edma_cfg->unit * 6], 6);
		}
		printf("MAC%x addr:%x:%x:%x:%x:%x:%x\n",
			edma_cfg->unit, dev[i]->enetaddr[0],
			dev[i]->enetaddr[1],
			dev[i]->enetaddr[2],
			dev[i]->enetaddr[3],
			dev[i]->enetaddr[4],
			dev[i]->enetaddr[5]);

		snprintf(dev[i]->name, sizeof(dev[i]->name), "eth%d", i);
		ipq40xx_edma_dev[i]->dev  = dev[i];
		ipq40xx_edma_dev[i]->mac_unit = edma_cfg->unit;
		ipq40xx_edma_dev[i]->c_info = c_info[i];
		edma_hw_addr = (unsigned long)c_info[i]->hw.hw_addr;

		ret = ipq40xx_edma_alloc_tx_rx_queue(c_info[i]);
		if (ret)
			goto failed;

		ret = ipq40xx_edma_alloc_tx_rx_rings(c_info[i]);
		if (ret)
			goto failed;

		c_info[i]->q_cinfo[i].tx_mask =
			(IPQ40XX_EDMA_TX_PER_CPU_MASK <<
			(i << IPQ40XX_EDMA_TX_PER_CPU_MASK_SHIFT));
		c_info[i]->q_cinfo[i].rx_mask =
			(IPQ40XX_EDMA_RX_PER_CPU_MASK <<
			(i << IPQ40XX_EDMA_RX_PER_CPU_MASK_SHIFT));
                c_info[i]->q_cinfo[i].tx_start =
			i << IPQ40XX_EDMA_TX_CPU_START_SHIFT;
		c_info[i]->q_cinfo[i].rx_start =
				i << IPQ40XX_EDMA_RX_CPU_START_SHIFT;
		c_info[i]->q_cinfo[i].tx_status = 0;
		c_info[i]->q_cinfo[i].rx_status = 0;
		c_info[i]->q_cinfo[i].c_info = c_info[i];

		ret = ipq40xx_sw_mdio_init(edma_cfg->phy_name);
		if (ret)
			goto failed;

		switch (edma_cfg->phy) {
		case PHY_INTERFACE_MODE_PSGMII:
			writel(PSGMIIPHY_PLL_VCO_VAL,
				PSGMIIPHY_PLL_VCO_RELATED_CTRL);
			writel(PSGMIIPHY_VCO_VAL,
				PSGMIIPHY_VCO_CALIBRATION_CTRL);
			/* wait for 10ms */
			mdelay(10);
			writel(PSGMIIPHY_VCO_RST_VAL, PSGMIIPHY_VCO_CALIBRATION_CTRL);
			break;
		case PHY_INTERFACE_MODE_RGMII:
			writel(0x1, RGMII_TCSR_ESS_CFG);
			writel(0x400, ESS_RGMII_CTRL);
			break;
		default:
			printf("unknown MII interface\n");
			goto failed;
		}
		eth_register(dev[i]);

		if (!sw_init_done) {
			if (ipq40xx_switch_init(ipq40xx_edma_dev[i]) == 0) {
				sw_init_done = 1;
			} else {
				printf ("SW inits failed\n");
				goto failed;
			}
		}

		if(edma_cfg->phy == PHY_INTERFACE_MODE_PSGMII) {
			qca8075_ess_reset();
			mdelay(100);
			psgmii_self_test();
			mdelay(300);
			clear_self_test_config();
		}

		/*
		 * Configure EDMA This should
		 * happen Only once.
		 */
		if (!cfg_edma) {
			ipq40xx_edma_reset(c_info[i]);
			ipq40xx_edma_configure(c_info[i]);
			cfg_edma = 1;
		}
		/*
		 * Configure descriptor Ring based on eth_unit
		 * 1 Rx/Tx Q is maintained per eth device.
		 */
		ipq40xx_edma_init_desc(c_info[i],
				edma_cfg->unit);

		if (!ipq40xx_ess_init_done) {
			ipq40xx_ess_sw_init(edma_cfg);
			ipq40xx_ess_disable_lookup();
			ipq40xx_ess_init_done = 1;
		}

	}
	return 0;

failed:
	printf("Error in allocating Mem\n");
	for (i = 0; i < IPQ40XX_EDMA_DEV; i++) {
		if (dev[i]) {
			eth_unregister(dev[i]);
			ipq40xx_free_mem(dev[i]);
		}
		if (c_info[i]) {
			ipq40xx_edma_free_rings(c_info[i]);
			ipq40xx_edma_free_queues(c_info[i]);
			ipq40xx_free_mem(c_info[i]);
		}
		if (ipq40xx_edma_dev[i]) {
			ipq40xx_free_mem(ipq40xx_edma_dev[i]);
		}
	}
	return -1;
}
