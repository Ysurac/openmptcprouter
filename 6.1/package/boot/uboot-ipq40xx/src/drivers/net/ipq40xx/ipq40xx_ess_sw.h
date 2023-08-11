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

#ifndef _AR8327_PHY_H
#define _AR8327_PHY_H
#define IPQ40XX_NSS_BASE	0xC000000

#define S17_P0STATUS_REG	0x007c
#define S17_P1STATUS_REG	0x0080
#define S17_P2STATUS_REG	0x0084
#define S17_P3STATUS_REG	0x0088
#define S17_P4STATUS_REG	0x008c
#define S17_P5STATUS_REG	0x0090
#define S17_P6STATUS_REG	0x0094

#define S17_PORT_SPEED(x)	(x << 0)
#define S17_TX_FLOW_EN		(1 << 4)
#define S17_RX_FLOW_EN		(1 << 5)
#define S17_PORT_TX_MAC_EN	(1 << 2)
#define S17_PORT_RX_MAC_EN	(1 << 3)
#define S17_PORT_FULL_DUP	(1 << 6)

#define S17_GLOFW_CTRL1_REG		0x0624
#define S17_GLOLEARN_LIMIT_REG		0x0628
#define S17_TOS_PRIMAP_REG0		0x0630
#define S17_TOS_PRIMAP_REG1		0x0634
#define S17_TOS_PRIMAP_REG2		0x0638
#define S17_TOS_PRIMAP_REG3		0x063c
#define S17_TOS_PRIMAP_REG4		0x0640
#define S17_TOS_PRIMAP_REG5		0x0644
#define S17_TOS_PRIMAP_REG6		0x0648
#define S17_TOS_PRIMAP_REG7		0x064c
#define S17_VLAN_PRIMAP_REG0		0x0650
#define S17_LOOP_CHECK_REG		0x0654
#define S17_P0LOOKUP_CTRL_REG		0x0660
#define S17_P0PRI_CTRL_REG		0x0664
#define S17_P0LEARN_LMT_REG		0x0668
#define S17_P1LOOKUP_CTRL_REG		0x066c
#define S17_P1PRI_CTRL_REG		0x0670
#define S17_P1LEARN_LMT_REG		0x0674
#define S17_P2LOOKUP_CTRL_REG		0x0678
#define S17_P2PRI_CTRL_REG		0x067c
#define S17_P2LEARN_LMT_REG		0x0680
#define S17_P3LOOKUP_CTRL_REG		0x0684
#define S17_P3PRI_CTRL_REG		0x0688
#define S17_P3LEARN_LMT_REG		0x068c
#define S17_P4LOOKUP_CTRL_REG		0x0690
#define S17_P4PRI_CTRL_REG		0x0694
#define S17_P4LEARN_LMT_REG		0x0698
#define S17_P5LOOKUP_CTRL_REG		0x069c

/* Queue Management Registers */
#define S17_PORT0_HOL_CTRL0		0x0970
#define S17_PORT0_HOL_CTRL1		0x0974
#define S17_PORT1_HOL_CTRL0		0x0978
#define S17_PORT1_HOL_CTRL1		0x097c
#define S17_PORT2_HOL_CTRL0		0x0980
#define S17_PORT2_HOL_CTRL1		0x0984
#define S17_PORT3_HOL_CTRL0		0x0988
#define S17_PORT3_HOL_CTRL1		0x098c
#define S17_PORT4_HOL_CTRL0		0x0990
#define S17_PORT4_HOL_CTRL1		0x0994
#define S17_PORT5_HOL_CTRL0		0x0998
#define S17_PORT5_HOL_CTRL1		0x099c

#define ess_mib(x)	((0x1000 | (x * 0x100)))
#define ESS_MIB_OFFSET	0x30
#define ESS_MIB_REG	0x34
#define ESS_MIB_EN	(1 << 0)
#ifdef CONFIG_ESS_MIB_EN
/*
 * Tx Mib counter offset
 */
#define ESS_PHY_TX_BROAD_REG		0x54
#define ESS_PHY_TX_PAUSE_REG		0x58
#define ESS_PHY_TX_MULTI_REG		0x5c
#define ESS_PHY_TX_UNDERRN_REG		0x60
#define ESS_PHY_TX_64B_REG		0x64
#define ESS_PHY_TX_128B_REG		0x68
#define ESS_PHY_TX_256B_REG		0x6c
#define ESS_PHY_TX_512B_REG		0x70
#define ESS_PHY_TX_1024B_REG		0x74
#define ESS_PHY_TX_1518B_REG		0x78
#define ESS_PHY_TX_MAXB_REG		0x7c
#define ESS_PHY_TX_OVSIZE_REG		0x80
#define ESS_PHY_TX_TXBYTEL_REG		0x88
#define ESS_PHY_TX_TXBYTEU_REG		0x84
#define ESS_PHY_TX_COLL_REG		0x8c
#define ESS_PHY_TX_ABTCOLL_REG		0x90
#define ESS_PHY_TX_MLTCOL_REG		0x94
#define ESS_PHY_TX_SINGCOL_REG		0x98
#define ESS_PHY_TX_EXDF_REG		0x9c
#define ESS_PHY_TX_DEF_REG		0xA0
#define ESS_PHY_TX_LATECOL_REG		0xA4
#define ESS_PHY_TX_UNICAST		0xAc
/*
 * Rx Mib counter offset
 */
#define ESS_PHY_RX_BROAD_REG		0x00
#define ESS_PHY_RX_PAUSE_REG		0x04
#define ESS_PHY_RX_MULTI_REG		0x08
#define ESS_PHY_RX_FCSERR_REG		0x0c
#define ESS_PHY_RX_ALIGNERR_REG		0x10
#define ESS_PHY_RX_RUNT_REG		0x14
#define ESS_PHY_RX_FRAGMENT_REG		0x18
#define ESS_PHY_RX_64B_REG		0x1c
#define ESS_PHY_RX_128B_REG		0x20
#define ESS_phy_RX_256B_REG		0x24
#define ESS_PHY_RX_512B_REG		0x28
#define ESS_PHY_RX_1024B_REG		0x2c
#define ESS_PHY_RX_1518B_REG		0x30
#define ESS_PHY_RX_MAXB_REG		0x34
#define ESS_PHY_RX_TOLO_REG		0x38
#define ESS_PHY_RX_GOODBL_REG		0x40
#define ESS_PHY_RX_GOODBU_REG		0x3c
#define ESS_PHY_RX_BADBL_REG		0x48
#define ESS_PHY_RX_BADBU_REG		0x44
#define ESS_PHY_RX_OVERFLW_REG		0x4c
#define ESS_PHY_RX_FILTERD_REG		0x50
#define ESS_PHY_RX_UNICAST		0xA8

struct ess_rx_stats {
	u32 rx_broad;
	u32 rx_pause;
	u32 rx_multi;
	u32 rx_fcserr;
	u32 rx_allignerr;
	u32 rx_runt;
	u32 rx_frag;
	u32 rx_64b;
	u32 rx_128b;
	u32 rx_256b;
	u32 rx_512b;
	u32 rx_1024b;
	u32 rx_1518b;
	u32 rx_maxb;
	u32 rx_tool;
	u32 rx_goodbl;
	u32 rx_goodbh;
	u32 rx_overflow;
	u32 rx_badbl;
	u32 rx_badbu;
	u32 rx_unicast;
};

struct ess_tx_stats{
	u32 tx_broad;
	u32 tx_pause;
	u32 tx_multi;
	u32 tx_underrun;
	u32 tx_64b;
	u32 tx_128b;
	u32 tx_256b;
	u32 tx_512b;
	u32 tx_1024b;
	u32 tx_1518b;
	u32 tx_maxb;
	u32 tx_oversiz;
	u32 tx_bytel;
	u32 tx_byteh;
	u32 tx_collision;
	u32 tx_abortcol;
	u32 tx_multicol;
	u32 tx_singalcol;
	u32 tx_execdefer;
	u32 tx_defer;
	u32 tx_latecol;
	u32 tx_unicast;
};
#endif
#endif
