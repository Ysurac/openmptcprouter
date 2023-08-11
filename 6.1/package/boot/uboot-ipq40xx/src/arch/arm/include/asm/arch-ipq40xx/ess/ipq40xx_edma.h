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

#ifndef _IPQ40XX_EDMA_H
#define _IPQ40XX_EDMA_H
#include <common.h>
#include <net.h>
#include <configs/ipq40xx_cdp.h>

#define IPQ40XX_EDMA_CFG_BASE	0xc080000

#define EDMA_FIFO_THRESH_TYPE	0
#define REG_SW_CTRL0	0x100
#define REG_SW_CTRL1	0x104

/* RX Interrupt Mask Register */
/* x = queue id */
#define EDMA_REG_RX_INT_MASK_Q(x)	(0x300 + ((x) << 2))

/* TX Interrupt mask register */
 /* x = queue id */
#define EDMA_REG_TX_INT_MASK_Q(x)	(0x340 + ((x) << 2))

/* Interrupt Mask Register */
#define EDMA_REG_MISC_IMR	0x214
#define EDMA_REG_WOL_IMR	0x218

/* Interrupt Status Register */
#define EDMA_REG_RX_ISR		0x200
#define EDMA_REG_TX_ISR		0x208
#define EDMA_REG_MISC_ISR	0x210
#define EDMA_REG_WOL_ISR	0x218

#define MISC_ISR_RX_URG_Q(x)	(1 << x)

#define MISC_ISR_AXIR_TIMEOUT	0x00000100
#define MISC_ISR_AXIR_ERR	0x00000200
#define MISC_ISR_TXF_DEAD	0x00000400
#define MISC_ISR_AXIW_ERR	0x00000800
#define MISC_ISR_AXIW_TIMEOUT	0x00001000

#define WOL_ISR_WOL		0x00000001

#define EDMA_FIFO_THRESH_TYPE	0
#define EDMA_RSS_TYPE		0
#define EDMA_RX_IMT		200
#define EDMA_TX_IMT		1
#define ISR_RX_PKT(x)		(1 << x)
#define ISR_TX_PKT(x)		(1 << x)
#define EDMA_NUM_TXQ_PER_CORE	4
#define EDMA_NUM_RXQ_PER_CORE	2
#define EDMA_TPD_EOP_SHIFT	31
#define EDMA_HW_CHECKSUM	0x00000001

/* tpd word 3 bit 18-28 */
#define EDMA_TPD_PORT_BITMAP_SHIFT	18

/* Enable Tx for all ports */
#define EDMA_PORT_ENABLE_ALL	0x3E


#define MISC_IMR_NORMAL_MASK	0x80001FFF
#define WOL_IMR_NORMAL_MASK	0x1

/* Edma receive consumer index */
/* x = queue id */
#define EDMA_REG_RX_SW_CONS_IDX_Q(x)	(0x220 + ((x) << 2))
/* Edma transmit consumer index */
#define EDMA_REG_TX_SW_CONS_IDX_Q(x)	(0x240 + ((x) << 2))
/* TPD Index Register */
#define EDMA_REG_TPD_IDX_Q(x)		(0x460 + ((x) << 2))
/* Tx Descriptor Control Register */
#define EDMA_REG_TPD_RING_SIZE		0x41C
#define TPD_RING_SIZE_SHIFT		0
#define EDMA_TPD_RING_SIZE_MASK		0xFFFF

/* Transmit descriptor base address */
 /* x = queue id */
#define EDMA_REG_TPD_BASE_ADDR_Q(x)	(0x420 + ((x) << 2))
#define EDMA_TPD_PROD_IDX_BITS		0x0000FFFF
#define TPD_CONS_IDX_BITS		0xFFFF0000
#define EDMA_TPD_PROD_IDX_MASK		0xFFFF
#define EDMA_TPD_CONS_IDX_MASK		0xFFFF
#define EDMA_TPD_PROD_IDX_SHIFT		0
#define EDMA_TPD_CONS_IDX_SHIFT		16

/* IRQ Moderator Initial Timer Register */
#define EDMA_REG_IRQ_MODRT_TIMER_INIT	0x280
#define EDMA_IRQ_MODRT_TIMER_MASK	0xFFFF
#define EDMA_IRQ_MODRT_RX_TIMER_SHIFT	0
#define EDMA_IRQ_MODRT_TX_TIMER_SHIFT	16

/* Interrupt Control Register */
#define EDMA_REG_INTR_CTRL		0x284
#define EDMA_INTR_CLR_TYP_SHIFT		0
#define EDMA_INTR_SW_IDX_W_TYP_SHIFT	1
#define EDMA_INTR_CLEAR_TYPE_W1		0
#define EDMA_INTR_CLEAR_TYPE_R		1

#define EDMA_SW_DESC_FLAG_LAST		0x1
#define EDMA_SW_DESC_FLAG_SKB_HEAD	0x2
#define EDMA_SW_DESC_FLAG_SKB_FRAG	0x4
#define EDMA_SW_DESC_FLAG_SKB_NONE	0x8

#define EDMA_REG_TX_SRAM_PART		0x400
#define EDMA_LOAD_PTR_SHIFT		16

/* TXQ Control Register */
#define EDMA_REG_TXQ_CTRL		0x404
#define TXQ_CTRL_IP_OPTION_EN		0x10
#define EDMA_TXQ_CTRL_TXQ_EN		0x20
#define TXQ_CTRL_ENH_MODE		0x40
#define TXQ_CTRL_LS_8023_EN		0x80
#define EDMA_TXQ_CTRL_TPD_BURST_EN	0x100
#define TXQ_CTRL_LSO_BREAK_EN		0x200
#define EDMA_TXQ_NUM_TPD_BURST_MASK	0xF
#define EDMA_TXQ_TXF_BURST_NUM_MASK	0xFFFF
#define EDMA_TXQ_NUM_TPD_BURST_SHIFT	0
#define EDMA_TXQ_TXF_BURST_NUM_SHIFT	16
#define EDMA_TXF_BURST			0x100
#define EDMA_TPD_BURST			5

#define EDMA_REG_TXF_WATER_MARK		0x408
#define TXF_WATER_MARK_MASK		0x0FFF
#define TXF_LOW_WATER_MARK_SHIFT	0
#define TXF_HIGH_WATER_MARK_SHIFT	16
#define TXQ_CTRL_BURST_MODE_EN		0x80000000

/* WRR Control Register */
 /* x is the queue id */
#define REG_WRR_CTRL(x)			(0x40c + ((x) << 2))
#define WRR_WEIGHT_Q_SHIFT(x)		(((x) * 5) % 20)

/* TX Virtual Queue Mapping Control Register */
#define REG_VQ_CTRL0		0x4A0
#define REG_VQ_CTRL1		0x4A4
#define VQ_ID_MASK		0x7
#define VQ0_ID_SHIFT		0
#define VQ1_ID_SHIFT		3
#define VQ2_ID_SHIFT		6
#define VQ3_ID_SHIFT		9
#define VQ4_ID_SHIFT		12
#define VQ5_ID_SHIFT		15
#define VQ6_ID_SHIFT		18
#define VQ7_ID_SHIFT		21
#define VQ8_ID_SHIFT		0
#define VQ9_ID_SHIFT		3
#define VQ10_ID_SHIFT		6
#define VQ11_ID_SHIFT		9
#define VQ12_ID_SHIFT		12
#define VQ13_ID_SHIFT		15
#define VQ14_ID_SHIFT		18
#define VQ15_ID_SHIFT		21

/* Tx side Port Interface Control Register */
#define REG_PORT_CTRL		0x4A8
#define PAD_EN_SHIFT		15

/* Tx side VLAN Configuration Register */
#define REG_VLAN_CFG		0x4AC

#define EDMA_TX_CVLAN		16
#define EDMA_TX_INS_CVLAN	17
#define EDMA_TX_CVLAN_TAG_SHIFT	0

#define EDMA_TX_SVLAN		14
#define EDMA_TX_INS_SVLAN	15
#define EDMA_TX_SVLAN_TAG_SHIFT	16

#define EDMA_RRD_CVLAN_SHIFT	0
#define EDMA_RRD_SVLAN_SHIFT	7
#define EDMA_RRD_VLAN_MASK	0x1

/* Tx Queue Packet Statistic Register */
/* x = queue id */
#define REG_TX_STAT_PKT_Q(x)	(0x700 + ((x) << 2))
#define TX_STAT_PKT_MASK	0xFFFFFF

/* Tx Queue Byte Statistic Register */
 /* x = queue id */
#define REG_TX_STAT_BYTE_Q(x)	(0x704 + ((x) << 2))

/* Load Balance Based Ring Offset Register */
#define REG_LB_RING			0x800
#define LB_RING_ENTRY_MASK		0xff
#define LB_RING_ID_MASK			0x7
#define LB_RING_PROFILE_ID_MASK		0x3
#define LB_RING_ENTRY_BIT_OFFSET	8
#define LB_RING_ID_OFFSET		0
#define LB_RING_PROFILE_ID_OFFSET	3

/* Load Balance Priority Mapping Register */
#define REG_LB_PRI_START	0x804
#define REG_LB_PRI_END		0x810
#define LB_PRI_REG_INC		4
#define LB_PRI_ENTRY_BIT_OFFSET	4
#define LB_PRI_ENTRY_MASK	0xf

/* RSS Priority Mapping Register */
#define REG_RSS_PRI			0x820
#define RSS_PRI_ENTRY_MASK		0xf
#define RSS_RING_ID_MASK		0x7
#define RSS_PRI_ENTRY_BIT_OFFSET	4

/* RSS Indirection Register */
/* x = No. of indirection table */
#define EDMA_REG_RSS_IDT(x)		(0x840 + ((x) << 2))
#define EDMA_NUM_IDT			16
#define EDMA_RSS_IDT_VALUE		0x64206420

/* Default RSS Ring Register */
#define REG_DEF_RSS		0x890
#define DEF_RSS_MASK		0x7

/* RSS Hash Function Type Register */
#define EDMA_REG_RSS_TYPE	0x894
#define EDMA_RSS_TYPE_NONE	0x01
#define EDMA_RSS_TYPE_IPV4TCP	0x02
#define EDMA_RSS_TYPE_IPV6_TCP	0x04
#define EDMA_RSS_TYPE_IPV4_UDP	0x08
#define EDMA_RSS_TYPE_IPV6UDP	0x10
#define EDMA_RSS_TYPE_IPV4	0x20
#define EDMA_RSS_TYPE_IPV6	0x40
#define RSS_HASH_MODE_MASK	0x7f

#define REG_RSS_HASH_VALUE	0x8C0
#define REG_RSS_TYPE_RESULT	0x8C4

/* x = queue id */
#define EDMA_REG_RFD_BASE_ADDR_Q(x)	(0x950 + ((x) << 2))
/* RFD Index Register */
#define EDMA_RFD_BURST		8
#define EDMA_RFD_THR		16
#define EDMA_RFD_LTHR		0
#define EDMA_REG_RFD_IDX_Q(x)	(0x9B0 + ((x) << 2))

#define EDMA_RFD_PROD_IDX_BITS	0x00000FFF
#define RFD_CONS_IDX_BITS	0x0FFF0000
#define RFD_PROD_IDX_MASK	0xFFF
#define RFD_CONS_IDX_MASK	0xFFF
#define RFD_PROD_IDX_SHIFT	0
#define RFD_CONS_IDX_SHIFT	16

/* Rx Descriptor Control Register */
#define EDMA_REG_RX_DESC0		0xA10
#define EDMA_RFD_RING_SIZE_MASK		0xFFF
#define EDMA_RX_BUF_SIZE_MASK		0xFFFF
#define EDMA_RFD_RING_SIZE_SHIFT	0
#define EDMA_RX_BUF_SIZE_SHIFT		16

#define EDMA_REG_RX_DESC1		0xA14
#define RXQ_RFD_BURST_NUM_MASK		0x3F
#define RXQ_RFD_PF_THRESH_MASK		0x1F
#define RXQ_RFD_LOW_THRESH_MASK		0xFFF
#define EDMA_RXQ_RFD_BURST_NUM_SHIFT	0
#define EDMA_RXQ_RFD_PF_THRESH_SHIFT	8
#define EDMA_RXQ_RFD_LOW_THRESH_SHIFT	16

/* RXQ Control Register */
#define EDMA_REG_RXQ_CTRL		0xA18
#define FIFO_THRESH_TYPE_SHIF		0
#define EDMA_FIFO_THRESH_128_BYTE	0x0
#define FIFO_THRESH_64_BYTE		0x1
#define EDMA_RXQ_CTRL_RMV_VLAN		0x00000002
#define EDMA_RXQ_CTRL_EN		0x0000FF00

/* Rx Statistics Register */
#define REG_RX_STAT_BYTE_Q(x)		(0xA30 + ((x) << 2)) /* x = queue id */
#define REG_RX_STAT_PKT_Q(x)		(0xA50 + ((x) << 2)) /* x = queue id */

/* WoL Pattern Length Register */
#define REG_WOL_PATTERN_LEN0	0xC00
#define WOL_PT_LEN_MASK		0xFF
#define WOL_PT0_LEN_SHIFT	0
#define WOL_PT1_LEN_SHIFT	8
#define WOL_PT2_LEN_SHIFT	16
#define WOL_PT3_LEN_SHIFT	24

#define REG_WOL_PATTERN_LEN1	0xC04
#define WOL_PT4_LEN_SHIFT	0
#define WOL_PT5_LEN_SHIFT	8
#define WOL_PT6_LEN_SHIFT	16

/* WoL Control Register */
#define EDMA_REG_WOL_CTRL	0xC08

/* MAC Control Register */
#define REG_MAC_CTRL0		0xC20
#define REG_MAC_CTRL1		0xC24

/* WoL Pattern Register */
#define REG_WOL_PATTERN_START		0x5000
#define PATTERN_PART_REG_OFFSET		0x40


/* TX descriptor checksum offload */
#define EDMA_TPD_HDR_SHIFT		0
#define EDMA_TPD_IP_CSUM_EN		0x00000200
#define EDMA_TPD_TCP_CSUM_EN		0x0000400
#define EDMA_TPD_UDP_CSUM_EN		0x00000800
#define EDMA_TPD_CUSTOM_CSUM_EN		0x00000C00
#define EDMA_TPD_LSO_EN			0x00001000
#define EDMA_TPD_LSO_V2_EN		0x00002000
#define EDMA_TPD_IPV4_EN		0x00010000
#define EDMA_TPD_MSS_MASK		0x1FFF
#define EDMA_TPD_MSS_SHIFT		18
#define EDMA_TPD_CUSTOM_CSUM_SHIFT	18

/* RRD checksum field */
#define EDMA_RRD_L4_CSUM_OFFSET		0x6
#define EDMA_RRD_IP_CSUM_OFFSET		0x7
#define EDMA_RRD_CSUM_MASK		0x1

/* WoL Pattern Register */
#define REG_WOL_PATTERN_START		0x5000
#define PATTERN_PART_REG_OFFSET		0x40

#define NETDEV_TX_BUSY	1
#define PHY_MAX		5
#define GCC_ESS_BCR		0x01812008
#define GCC_MDIO_AHB_CBCR	0x1826000
#define MDIO_CTRL_0_REG		0x00090040
#define MDIO_CTRL_0_DIV(x)	(x << 0)
#define MDIO_CTRL_0_MODE	(1 << 8)
#define MDIO_CTRL_0_RES(x)	(x << 9)
#define MDIO_CTRL_0_MDC_MODE	(1 << 12)
#define MDIO_CTRL_0_GPHY(x)	(x << 13)
#define MDIO_CTRL_0_RES1(x)	(x << 17)

#define GP_PULL_DOWN		1
#define GP_OE_EN		(1 << 9)
#define GP_VM_EN		(1 << 11)
#define GP_PU_RES(x)		(x << 13)
#define QCA8075_RST_VAL		(GP_PULL_DOWN | GP_OE_EN | \
				GP_VM_EN | GP_PU_RES(2))

/* Phy preferred medium type */
typedef enum {
	QCA8075_PHY_MEDIUM_COPPER = 0,
	QCA8075_PHY_MEDIUM_FIBER = 1, /**< Fiber */
} qca8075_phy_medium_t;

/* Phy pages */
typedef enum {
	QCA8075_PHY_SGBX_PAGES = 0, /* sgbx pages */
	QCA8075_PHY_COPPER_PAGES = 1 /* copper pages */
} qca8075_phy_reg_pages_t;
typedef enum {
	FAL_HALF_DUPLEX = 0,
	FAL_FULL_DUPLEX,
	FAL_DUPLEX_BUTT = 0xffff
} fal_port_duplex_t;

typedef enum {
	FAL_SPEED_10 = 10,
	FAL_SPEED_100 = 100,
	FAL_SPEED_1000 = 1000,
	FAL_SPEED_10000 = 10000,
	FAL_SPEED_BUTT = 0xffff,
} fal_port_speed_t;

typedef enum {
	FAL_CABLE_STATUS_NORMAL = 0,
	FAL_CABLE_STATUS_SHORT = 1,
	FAL_CABLE_STATUS_OPENED = 2,
	FAL_CABLE_STATUS_INVALID = 3,
	FAL_CABLE_STATUS_BUTT = 0xffff,
} fal_cable_status_t;

struct phy_ops {
	u8 (*phy_get_link_status) (u32 dev_id, u32 phy_id);
	u32 (*phy_get_duplex) (u32 dev_id, u32 phy_id,
				fal_port_duplex_t * duplex);
	u32 (*phy_get_speed) (u32 dev_id, u32 phy_id,
				fal_port_speed_t * speed);
};

int get_eth_mac_address(uchar *enetaddr, uint no_of_macs);
int ipq40xx_sw_mdio_init(const char *name);
#endif	/* _IPQ40XX_EDMA_H */
