/*
 * Copyright (c) 2013 The Linux Foundation. All rights reserved.
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

#ifndef _ATHRS17_PHY_H
#define _ATHRS17_PHY_H

/*****************/
/* PHY Registers */
/*****************/
#define ATHR_PHY_CONTROL                 0
#define ATHR_PHY_STATUS                  1
#define ATHR_PHY_ID1                     2
#define ATHR_PHY_ID2                     3
#define ATHR_AUTONEG_ADVERT              4
#define ATHR_LINK_PARTNER_ABILITY        5
#define ATHR_AUTONEG_EXPANSION           6
#define ATHR_NEXT_PAGE_TRANSMIT          7
#define ATHR_LINK_PARTNER_NEXT_PAGE      8
#define ATHR_1000BASET_CONTROL           9
#define ATHR_1000BASET_STATUS            10
#define ATHR_PHY_SPEC_CONTROL            16
#define ATHR_PHY_SPEC_STATUS             17
#define ATHR_DEBUG_PORT_ADDRESS          29
#define ATHR_DEBUG_PORT_DATA             30

/* ATHR_PHY_CONTROL fields */
#define ATHR_CTRL_SOFTWARE_RESET                    0x8000
#define ATHR_CTRL_SPEED_LSB                         0x2000
#define ATHR_CTRL_AUTONEGOTIATION_ENABLE            0x1000
#define ATHR_CTRL_RESTART_AUTONEGOTIATION           0x0200
#define ATHR_CTRL_SPEED_FULL_DUPLEX                 0x0100
#define ATHR_CTRL_SPEED_MSB                         0x0040

#define ATHR_RESET_DONE(phy_control)                   \
	(((phy_control) & (ATHR_CTRL_SOFTWARE_RESET)) == 0)

/* Phy status fields */
#define ATHR_STATUS_AUTO_NEG_DONE                   0x0020

#define ATHR_AUTONEG_DONE(ip_phy_status)                   \
	(((ip_phy_status) &                                \
	(ATHR_STATUS_AUTO_NEG_DONE)) ==                    \
	(ATHR_STATUS_AUTO_NEG_DONE))

/* Link Partner ability */
#define ATHR_LINK_100BASETX_FULL_DUPLEX       0x0100
#define ATHR_LINK_100BASETX                   0x0080
#define ATHR_LINK_10BASETX_FULL_DUPLEX        0x0040
#define ATHR_LINK_10BASETX                    0x0020

/* Advertisement register. */
#define ATHR_ADVERTISE_NEXT_PAGE              0x8000
#define ATHR_ADVERTISE_ASYM_PAUSE             0x0800
#define ATHR_ADVERTISE_PAUSE                  0x0400
#define ATHR_ADVERTISE_100FULL                0x0100
#define ATHR_ADVERTISE_100HALF                0x0080
#define ATHR_ADVERTISE_10FULL                 0x0040
#define ATHR_ADVERTISE_10HALF                 0x0020

#define ATHR_ADVERTISE_ALL (ATHR_ADVERTISE_ASYM_PAUSE | ATHR_ADVERTISE_PAUSE | \
		ATHR_ADVERTISE_10HALF | ATHR_ADVERTISE_10FULL | \
		ATHR_ADVERTISE_100HALF | ATHR_ADVERTISE_100FULL)

/* 1000BASET_CONTROL */
#define ATHR_ADVERTISE_1000FULL               0x0200

/* Phy Specific status fields */
#define ATHER_STATUS_LINK_MASK                0xC000
#define ATHER_STATUS_LINK_SHIFT               14
#define ATHER_STATUS_FULL_DEPLEX              0x2000
#define ATHR_STATUS_LINK_PASS                 0x0400
#define ATHR_STATUS_RESOVLED                  0x0800

/*phy debug port  register */
#define ATHER_DEBUG_SERDES_REG                5

/* Serdes debug fields */
#define ATHER_SERDES_BEACON                   0x0100

/* S17 CSR Registers */

#define S17_ENABLE_CPU_BROADCAST             (1 << 26)

#define S17_PHY_LINK_CHANGE_REG              0x4
#define S17_PHY_LINK_UP                      0x400
#define S17_PHY_LINK_DOWN                    0x800
#define S17_PHY_LINK_DUPLEX_CHANGE           0x2000
#define S17_PHY_LINK_SPEED_CHANGE            0x4000
#define S17_PHY_LINK_INTRS                   (PHY_LINK_UP | PHY_LINK_DOWN | PHY_LINK_DUPLEX_CHANGE | PHY_LINK_SPEED_CHANGE)

#define S17_MASK_CTRL_REG               0x0000
#define S17_P0PAD_MODE_REG              0x0004
#define S17_P5PAD_MODE_REG              0x0008
#define S17_P6PAD_MODE_REG              0x000c
#define S17_PWS_REG                     0x0010
#define S17_GLOBAL_INT0_REG             0x0020
#define S17_GLOBAL_INT1_REG             0x0024
#define S17_GLOBAL_INTMASK0             0x0028
#define S17_GLOBAL_INTMASK1             0x002c
#define S17_MODULE_EN_REG               0x0030
#define S17_MIB_REG                     0x0034
#define S17_INTF_HIADDR_REG             0x0038
#define S17_MDIO_CTRL_REG               0x003c
#define S17_BIST_CTRL_REG               0x0040
#define S17_BIST_REC_REG                0x0044
#define S17_SERVICE_REG                 0x0048
#define S17_LED_CTRL0_REG               0x0050
#define S17_LED_CTRL1_REG               0x0054
#define S17_LED_CTRL2_REG               0x0058
#define S17_LED_CTRL3_REG               0x005c
#define S17_MACADDR0_REG                0x0060
#define S17_MACADDR1_REG                0x0064
#define S17_MAX_FRAME_SIZE_REG          0x0078
#define S17_P0STATUS_REG                0x007c
#define S17_P1STATUS_REG                0x0080
#define S17_P2STATUS_REG                0x0084
#define S17_P3STATUS_REG                0x0088
#define S17_P4STATUS_REG                0x008c
#define S17_P5STATUS_REG                0x0090
#define S17_P6STATUS_REG                0x0094
#define S17_HDRCTRL_REG                 0x0098
#define S17_P0HDRCTRL_REG               0x009c
#define S17_P1HDRCTRL_REG               0x00A0
#define S17_P2HDRCTRL_REG               0x00a4
#define S17_P3HDRCTRL_REG               0x00a8
#define S17_P4HDRCTRL_REG               0x00ac
#define S17_P5HDRCTRL_REG               0x00b0
#define S17_P6HDRCTRL_REG               0x00b4
#define S17_SGMII_CTRL_REG              0x00e0
#define S17_MAC_PWR_REG                 0x00e4
#define S17_EEE_CTRL_REG                0x0100

/* ACL Registers */
#define S17_ACL_FUNC0_REG               0x0400
#define S17_ACL_FUNC1_REG               0x0404
#define S17_ACL_FUNC2_REG               0x0408
#define S17_ACL_FUNC3_REG               0x040c
#define S17_ACL_FUNC4_REG               0x0410
#define S17_ACL_FUNC5_REG               0x0414
#define S17_PRIVATE_IP_REG              0x0418
#define S17_P0VLAN_CTRL0_REG            0x0420
#define S17_P0VLAN_CTRL1_REG            0x0424
#define S17_P1VLAN_CTRL0_REG            0x0428
#define S17_P1VLAN_CTRL1_REG            0x042c
#define S17_P2VLAN_CTRL0_REG            0x0430
#define S17_P2VLAN_CTRL1_REG            0x0434
#define S17_P3VLAN_CTRL0_REG            0x0438
#define S17_P3VLAN_CTRL1_REG            0x043c
#define S17_P4VLAN_CTRL0_REG            0x0440
#define S17_P4VLAN_CTRL1_REG            0x0444
#define S17_P5VLAN_CTRL0_REG            0x0448
#define S17_P5VLAN_CTRL1_REG            0x044c
#define S17_P6VLAN_CTRL0_REG            0x0450
#define S17_P6VLAN_CTRL1_REG            0x0454

/* Table Lookup Registers */
#define S17_ATU_DATA0_REG               0x0600
#define S17_ATU_DATA1_REG               0x0604
#define S17_ATU_DATA2_REG               0x0608
#define S17_ATU_FUNC_REG                0x060C
#define S17_VTU_FUNC0_REG               0x0610
#define S17_VTU_FUNC1_REG               0x0614
#define S17_ARL_CTRL_REG                0x0618
#define S17_GLOFW_CTRL0_REG             0x0620
#define S17_GLOFW_CTRL1_REG             0x0624
#define S17_GLOLEARN_LIMIT_REG          0x0628
#define S17_TOS_PRIMAP_REG0             0x0630
#define S17_TOS_PRIMAP_REG1             0x0634
#define S17_TOS_PRIMAP_REG2             0x0638
#define S17_TOS_PRIMAP_REG3             0x063c
#define S17_TOS_PRIMAP_REG4             0x0640
#define S17_TOS_PRIMAP_REG5             0x0644
#define S17_TOS_PRIMAP_REG6             0x0648
#define S17_TOS_PRIMAP_REG7             0x064c
#define S17_VLAN_PRIMAP_REG0            0x0650
#define S17_LOOP_CHECK_REG              0x0654
#define S17_P0LOOKUP_CTRL_REG           0x0660
#define S17_P0PRI_CTRL_REG              0x0664
#define S17_P0LEARN_LMT_REG             0x0668
#define S17_P1LOOKUP_CTRL_REG           0x066c
#define S17_P1PRI_CTRL_REG              0x0670
#define S17_P1LEARN_LMT_REG             0x0674
#define S17_P2LOOKUP_CTRL_REG           0x0678
#define S17_P2PRI_CTRL_REG              0x067c
#define S17_P2LEARN_LMT_REG             0x0680
#define S17_P3LOOKUP_CTRL_REG           0x0684
#define S17_P3PRI_CTRL_REG              0x0688
#define S17_P3LEARN_LMT_REG             0x068c
#define S17_P4LOOKUP_CTRL_REG           0x0690
#define S17_P4PRI_CTRL_REG              0x0694
#define S17_P4LEARN_LMT_REG             0x0698
#define S17_P5LOOKUP_CTRL_REG           0x069c
#define S17_P5PRI_CTRL_REG              0x06a0
#define S17_P5LEARN_LMT_REG             0x06a4
#define S17_P6LOOKUP_CTRL_REG           0x06a8
#define S17_P6PRI_CTRL_REG              0x06ac
#define S17_P6LEARN_LMT_REG             0x06b0
#define S17_GLO_TRUNK_CTRL0_REG         0x0700
#define S17_GLO_TRUNK_CTRL1_REG         0x0704
#define S17_GLO_TRUNK_CTRL2_REG         0x0708

/* Queue Management Registers */
#define S17_PORT0_HOL_CTRL0             0x0970
#define S17_PORT0_HOL_CTRL1             0x0974
#define S17_PORT1_HOL_CTRL0             0x0978
#define S17_PORT1_HOL_CTRL1             0x097c
#define S17_PORT2_HOL_CTRL0             0x0980
#define S17_PORT2_HOL_CTRL1             0x0984
#define S17_PORT3_HOL_CTRL0             0x0988
#define S17_PORT3_HOL_CTRL1             0x098c
#define S17_PORT4_HOL_CTRL0             0x0990
#define S17_PORT4_HOL_CTRL1             0x0994
#define S17_PORT5_HOL_CTRL0             0x0998
#define S17_PORT5_HOL_CTRL1             0x099c
#define S17_PORT6_HOL_CTRL0             0x09a0
#define S17_PORT6_HOL_CTRL1             0x09a4

/* Port flow control registers */
#define S17_P0_FLCTL_REG                0x09b0
#define S17_P1_FLCTL_REG                0x09b4
#define S17_P2_FLCTL_REG                0x09b8
#define S17_P3_FLCTL_REG                0x09bc
#define S17_P4_FLCTL_REG                0x09c0
#define S17_P5_FLCTL_REG                0x09c4

/* Packet Edit registers */
#define S17_PKT_EDIT_CTRL               0x0c00
#define S17_P0Q_REMAP_REG0              0x0c40
#define S17_P0Q_REMAP_REG1              0x0c44
#define S17_P1Q_REMAP_REG0              0x0c48
#define S17_P2Q_REMAP_REG0              0x0c4c
#define S17_P3Q_REMAP_REG0              0x0c50
#define S17_P4Q_REMAP_REG0              0x0c54
#define S17_P5Q_REMAP_REG0              0x0c58
#define S17_P5Q_REMAP_REG1              0x0c5c
#define S17_P6Q_REMAP_REG0              0x0c60
#define S17_P6Q_REMAP_REG1              0x0c64
#define S17_ROUTER_VID0                 0x0c70
#define S17_ROUTER_VID1                 0x0c74
#define S17_ROUTER_VID2                 0x0c78
#define S17_ROUTER_VID3                 0x0c7c
#define S17_ROUTER_EG_VLAN_MODE         0x0c80

/* L3 Registers */
#define S17_HROUTER_CTRL_REG            0x0e00
#define S17_HROUTER_PBCTRL0_REG         0x0e04
#define S17_HROUTER_PBCTRL1_REG         0x0e08
#define S17_HROUTER_PBCTRL2_REG         0x0e0c
#define S17_WCMP_HASH_TABLE0_REG        0x0e10
#define S17_WCMP_HASH_TABLE1_REG        0x0e14
#define S17_WCMP_HASH_TABLE2_REG        0x0e18
#define S17_WCMP_HASH_TABLE3_REG        0x0e1c
#define S17_WCMP_NHOP_TABLE0_REG        0x0e20
#define S17_WCMP_NHOP_TABLE1_REG        0x0e24
#define S17_WCMP_NHOP_TABLE2_REG        0x0e28
#define S17_WCMP_NHOP_TABLE3_REG        0x0e2c
#define S17_ARP_ENTRY_CTRL_REG          0x0e30
#define S17_ARP_USECNT_REG              0x0e34
#define S17_HNAT_CTRL_REG               0x0e38
#define S17_NAPT_ENTRY_CTRL0_REG        0x0e3c
#define S17_NAPT_ENTRY_CTRL1_REG        0x0e40
#define S17_NAPT_USECNT_REG             0x0e44
#define S17_ENTRY_EDIT_DATA0_REG        0x0e48
#define S17_ENTRY_EDIT_DATA1_REG        0x0e4c
#define S17_ENTRY_EDIT_DATA2_REG        0x0e50
#define S17_ENTRY_EDIT_DATA3_REG        0x0e54
#define S17_ENTRY_EDIT_CTRL_REG         0x0e58
#define S17_HNAT_PRIVATE_IP_REG         0x0e5c

/* MIB counters */
#define S17_MIB_PORT0                   0x1000
#define S17_MIB_PORT1                   0x1100
#define S17_MIB_PORT2                   0x1200
#define S17_MIB_PORT3                   0x1300
#define S17_MIB_PORT4                   0x1400
#define S17_MIB_PORT5                   0x1500
#define S17_MIB_PORT6                   0x1600

#define S17_MIB_RXBROAD                 0x0
#define S17_MIB_RXPAUSE                 0x4
#define S17_MIB_RXMULTI                 0x8
#define S17_MIB_RXFCSERR                0xC
#define S17_MIB_RXALIGNERR              0x10
#define S17_MIB_RXUNDERSIZE             0x14
#define S17_MIB_RXFRAG                  0x18
#define S17_MIB_RX64B                   0x1C
#define S17_MIB_RX128B                  0x20
#define S17_MIB_RX256B                  0x24
#define S17_MIB_RX512B                  0x28
#define S17_MIB_RX1024B                 0x2C
#define S17_MIB_RX1518B                 0x30
#define S17_MIB_RXMAXB                  0x34
#define S17_MIB_RXTOOLONG               0x38
#define S17_MIB_RXBYTE1                 0x3C
#define S17_MIB_RXBYTE2                 0x40
#define S17_MIB_RXOVERFLOW              0x4C
#define S17_MIB_FILTERED                0x50
#define S17_MIB_TXBROAD                 0x54
#define S17_MIB_TXPAUSE                 0x58
#define S17_MIB_TXMULTI                 0x5C
#define S17_MIB_TXUNDERRUN              0x60
#define S17_MIB_TX64B                   0x64
#define S17_MIB_TX128B                  0x68
#define S17_MIB_TX256B                  0x6c
#define S17_MIB_TX512B                  0x70
#define S17_MIB_TX1024B                 0x74
#define S17_MIB_TX1518B                 0x78
#define S17_MIB_TXMAXB                  0x7C
#define S17_MIB_TXOVERSIZE              0x80
#define S17_MIB_TXBYTE1                 0x84
#define S17_MIB_TXBYTE2                 0x88
#define S17_MIB_TXCOL                   0x8C
#define S17_MIB_TXABORTCOL              0x90
#define S17_MIB_TXMULTICOL              0x94
#define S17_MIB_TXSINGLECOL             0x98
#define S17_MIB_TXEXCDEFER              0x9C
#define S17_MIB_TXDEFER                 0xA0
#define S17_MIB_TXLATECOL               0xA4

/* Register fields */
#define S17_CHIPID_V1_0                 0x1201
#define S17_CHIPID_V1_1                 0x1202

#define S17_MASK_CTRL_SOFT_RET		(1 << 31)

#define S17_GLOBAL_INT0_ACL_INI_INT        (1<<29)
#define S17_GLOBAL_INT0_LOOKUP_INI_INT     (1<<28)
#define S17_GLOBAL_INT0_QM_INI_INT         (1<<27)
#define S17_GLOBAL_INT0_MIB_INI_INT        (1<<26)
#define S17_GLOBAL_INT0_OFFLOAD_INI_INT    (1<<25)
#define S17_GLOBAL_INT0_HARDWARE_INI_DONE  (1<<24)

#define S17_GLOBAL_INITIALIZED_STATUS 				\
			(					\
			S17_GLOBAL_INT0_ACL_INI_INT | 		\
			S17_GLOBAL_INT0_LOOKUP_INI_INT | 	\
			S17_GLOBAL_INT0_QM_INI_INT | 		\
			S17_GLOBAL_INT0_MIB_INI_INT | 		\
			S17_GLOBAL_INT0_OFFLOAD_INI_INT |	\
			S17_GLOBAL_INT0_HARDWARE_INI_DONE	\
			)

#define S17_MAC0_MAC_MII_RXCLK_SEL      (1 << 0)
#define S17_MAC0_MAC_MII_TXCLK_SEL      (1 << 1)
#define S17_MAC0_MAC_MII_EN             (1 << 2)
#define S17_MAC0_MAC_GMII_RXCLK_SEL     (1 << 4)
#define S17_MAC0_MAC_GMII_TXCLK_SEL     (1 << 5)
#define S17_MAC0_MAC_GMII_EN            (1 << 6)
#define S17_MAC0_SGMII_EN               (1 << 7)
#define S17_MAC0_PHY_MII_RXCLK_SEL      (1 << 8)
#define S17_MAC0_PHY_MII_TXCLK_SEL      (1 << 9)
#define S17_MAC0_PHY_MII_EN             (1 << 10)
#define S17_MAC0_PHY_MII_PIPE_SEL       (1 << 11)
#define S17_MAC0_PHY_GMII_RXCLK_SEL     (1 << 12)
#define S17_MAC0_PHY_GMII_TXCLK_SEL     (1 << 13)
#define S17_MAC0_PHY_GMII_EN            (1 << 14)
#define S17_MAC0_RGMII_RXCLK_SHIFT      20
#define S17_MAC0_RGMII_TXCLK_SHIFT      22
#define S17_MAC0_RGMII_RXCLK_DELAY      (1 << 24)
#define S17_MAC0_RGMII_TXCLK_DELAY      (1 << 25)
#define S17_MAC0_RGMII_EN               (1 << 26)

#define S17_MAC5_MAC_MII_RXCLK_SEL      (1 << 0)
#define S17_MAC5_MAC_MII_TXCLK_SEL      (1 << 1)
#define S17_MAC5_MAC_MII_EN             (1 << 2)
#define S17_MAC5_PHY_MII_RXCLK_SEL      (1 << 8)
#define S17_MAC5_PHY_MII_TXCLK_SEL      (1 << 9)
#define S17_MAC5_PHY_MII_EN             (1 << 10)
#define S17_MAC5_PHY_MII_PIPE_SEL       (1 << 11)
#define S17_MAC5_RGMII_RXCLK_SHIFT      20
#define S17_MAC5_RGMII_TXCLK_SHIFT      22
#define S17_MAC5_RGMII_RXCLK_DELAY      (1 << 24)
#define S17_MAC5_RGMII_TXCLK_DELAY      (1 << 25)
#define S17_MAC5_RGMII_EN               (1 << 26)

#define S17_MAC6_MAC_MII_RXCLK_SEL      (1 << 0)
#define S17_MAC6_MAC_MII_TXCLK_SEL      (1 << 1)
#define S17_MAC6_MAC_MII_EN             (1 << 2)
#define S17_MAC6_MAC_GMII_RXCLK_SEL     (1 << 4)
#define S17_MAC6_MAC_GMII_TXCLK_SEL     (1 << 5)
#define S17_MAC6_MAC_GMII_EN            (1 << 6)
#define S17_MAC6_SGMII_EN               (1 << 7)
#define S17_MAC6_PHY_MII_RXCLK_SEL      (1 << 8)
#define S17_MAC6_PHY_MII_TXCLK_SEL      (1 << 9)
#define S17_MAC6_PHY_MII_EN             (1 << 10)
#define S17_MAC6_PHY_MII_PIPE_SEL       (1 << 11)
#define S17_MAC6_PHY_GMII_RXCLK_SEL     (1 << 12)
#define S17_MAC6_PHY_GMII_TXCLK_SEL     (1 << 13)
#define S17_MAC6_PHY_GMII_EN            (1 << 14)
#define S17_PHY4_GMII_EN                (1 << 16)
#define S17_PHY4_RGMII_EN               (1 << 17)
#define S17_PHY4_MII_EN                 (1 << 18)
#define S17_MAC6_RGMII_RXCLK_SHIFT      20
#define S17_MAC6_RGMII_TXCLK_SHIFT      22
#define S17_MAC6_RGMII_RXCLK_DELAY      (1 << 24)
#define S17_MAC6_RGMII_TXCLK_DELAY      (1 << 25)
#define S17_MAC6_RGMII_EN               (1 << 26)

#define S17_SPEED_10M                   (0 << 0)
#define S17_SPEED_100M                  (1 << 0)
#define S17_SPEED_1000M                 (2 << 0)
#define S17_TXMAC_EN                    (1 << 2)
#define S17_RXMAC_EN                    (1 << 3)
#define S17_TX_FLOW_EN                  (1 << 4)
#define S17_RX_FLOW_EN                  (1 << 5)
#define S17_DUPLEX_FULL                 (1 << 6)
#define S17_DUPLEX_HALF                 (0 << 6)
#define S17_TX_HALF_FLOW_EN             (1 << 7)
#define S17_LINK_EN                     (1 << 9)
#define S17_FLOW_LINK_EN                (1 << 12)
#define S17_PORT_STATUS_DEFAULT         (S17_SPEED_1000M | S17_TXMAC_EN | \
                                        S17_RXMAC_EN | S17_TX_FLOW_EN | \
                                        S17_RX_FLOW_EN | S17_DUPLEX_FULL | \
                                        S17_TX_HALF_FLOW_EN)

#define S17_PORT_STATUS_AZ_DEFAULT      (S17_SPEED_1000M | S17_TXMAC_EN | \
                                        S17_RXMAC_EN | S17_TX_FLOW_EN | \
                                        S17_RX_FLOW_EN | S17_DUPLEX_FULL)


#define S17_HDRLENGTH_SEL               (1 << 16)
#define S17_HDR_VALUE                   0xAAAA

#define S17_TXHDR_MODE_NO               0
#define S17_TXHDR_MODE_MGM              1
#define S17_TXHDR_MODE_ALL              2
#define S17_RXHDR_MODE_NO               (0 << 2)
#define S17_RXHDR_MODE_MGM              (1 << 2)
#define S17_RXHDR_MODE_ALL              (2 << 2)

#define S17_CPU_PORT_EN                 (1 << 10)
#define S17_PPPOE_REDIR_EN              (1 << 8)
#define S17_MIRROR_PORT_SHIFT           4
#define S17_IGMP_COPY_EN                (1 << 3)
#define S17_RIP_COPY_EN                 (1 << 2)
#define S17_EAPOL_REDIR_EN              (1 << 0)

#define S17_IGMP_JOIN_LEAVE_DP_SHIFT    24
#define S17_BROAD_DP_SHIFT              16
#define S17_MULTI_FLOOD_DP_SHIFT        8
#define S17_UNI_FLOOD_DP_SHIFT          0
#define S17_IGMP_JOIN_LEAVE_DPALL       (0x7f << S17_IGMP_JOIN_LEAVE_DP_SHIFT)
#define S17_BROAD_DPALL                 (0x7f << S17_BROAD_DP_SHIFT)
#define S17_MULTI_FLOOD_DPALL           (0x7f << S17_MULTI_FLOOD_DP_SHIFT)
#define S17_UNI_FLOOD_DPALL             (0x7f << S17_UNI_FLOOD_DP_SHIFT)

#define S17_PWS_CHIP_AR8327		(1 << 30)
#define S17c_PWS_SERDES_ANEG_DISABLE	(1 << 7)

/* S17_PHY_CONTROL fields */
#define S17_CTRL_SOFTWARE_RESET                    0x8000
#define S17_CTRL_SPEED_LSB                         0x2000
#define S17_CTRL_AUTONEGOTIATION_ENABLE            0x1000
#define S17_CTRL_RESTART_AUTONEGOTIATION           0x0200
#define S17_CTRL_SPEED_FULL_DUPLEX                 0x0100
#define S17_CTRL_SPEED_MSB                         0x0040

/* For EEE_CTRL_REG */
#define S17_LPI_DISABLE_P1              (1 << 4)
#define S17_LPI_DISABLE_P2              (1 << 6)
#define S17_LPI_DISABLE_P3              (1 << 8)
#define S17_LPI_DISABLE_P4              (1 << 10)
#define S17_LPI_DISABLE_P5              (1 << 12)
#define S17_LPI_DISABLE_ALL             0x1550

/* For MMD register control */
#define S17_MMD_FUNC_ADDR               (0 << 14)
#define S17_MMD_FUNC_DATA               (1 << 14)
#define S17_MMD_FUNC_DATA_2             (2 << 14)
#define S17_MMD_FUNC_DATA_3             (3 << 14)

/* For phyInfo_t azFeature */
#define S17_8023AZ_PHY_ENABLED                  (1 << 0)
#define S17_8023AZ_PHY_LINKED                   (1 << 1)

/* Queue Management registe fields */
#define S17_HOL_CTRL0_LAN		0x2a008888 /* egress priority 8, eg_portq = 0x2a */
#define S17_HOL_CTRL0_WAN		0x2a666666 /* egress priority 6, eg_portq = 0x2a */
#define S17_HOL_CTRL1_DEFAULT		0xc6	   /* enable HOL control */

/* Packet Edit register fields */
#define S17_ROUTER_EG_UNMOD             0x0	/* unmodified */
#define S17_ROUTER_EG_WOVLAN            0x1	/* without VLAN */
#define S17_ROUTER_EG_WVLAN             0x2	/* with VLAN */
#define S17_ROUTER_EG_UNTOUCH           0x3	/* untouched */
#define S17_ROUTER_EG_MODE_DEFAULT      0x01111111 /* all ports without VLAN */

#define S17_RESET_DONE(phy_control)                   \
	(((phy_control) & (S17_CTRL_SOFTWARE_RESET)) == 0)

/* Phy status fields */
#define S17_STATUS_AUTO_NEG_DONE                   0x0020

#define S17_AUTONEG_DONE(ip_phy_status)                   \
	(((ip_phy_status) &                               \
	(S17_STATUS_AUTO_NEG_DONE)) ==                    \
	(S17_STATUS_AUTO_NEG_DONE))

/* Link Partner ability */
#define S17_LINK_100BASETX_FULL_DUPLEX       0x0100
#define S17_LINK_100BASETX                   0x0080
#define S17_LINK_10BASETX_FULL_DUPLEX        0x0040
#define S17_LINK_10BASETX                    0x0020

/* Advertisement register. */
#define S17_ADVERTISE_NEXT_PAGE              0x8000
#define S17_ADVERTISE_ASYM_PAUSE             0x0800
#define S17_ADVERTISE_PAUSE                  0x0400
#define S17_ADVERTISE_100FULL                0x0100
#define S17_ADVERTISE_100HALF                0x0080
#define S17_ADVERTISE_10FULL                 0x0040
#define S17_ADVERTISE_10HALF                 0x0020

#define S17_ADVERTISE_ALL (S17_ADVERTISE_ASYM_PAUSE | S17_ADVERTISE_PAUSE | \
			S17_ADVERTISE_10HALF | S17_ADVERTISE_10FULL | \
			S17_ADVERTISE_100HALF | S17_ADVERTISE_100FULL)

/* 1000BASET_CONTROL */
#define S17_ADVERTISE_1000FULL               0x0200

/* Phy Specific status fields */
#define S17_STATUS_LINK_MASK                 0xC000
#define S17_STATUS_LINK_SHIFT                14
#define S17_STATUS_FULL_DEPLEX               0x2000
#define S17_STATUS_LINK_PASS                 0x0400
#define S17_STATUS_RESOLVED                  0x0800
#define S17_STATUS_LINK_10M                  0
#define S17_STATUS_LINK_100M                 1
#define S17_STATUS_LINK_1000M                2

#define S17_GLOBAL_INT_PHYMASK               (1 << 15)

#define S17_PHY_LINK_UP                      0x400
#define S17_PHY_LINK_DOWN                    0x800
#define S17_PHY_LINK_DUPLEX_CHANGE           0x2000
#define S17_PHY_LINK_SPEED_CHANGE            0x4000

/* For Port flow control registers */
#define S17_PORT_FLCTL_XON_DEFAULT           (0x3a << 16)
#define S17_PORT_FLCTL_XOFF_DEFAULT          (0x4a)

/* Module enable Register */
#define S17_MODULE_L3_EN                     (1 << 2)
#define S17_MODULE_ACL_EN                    (1 << 1)
#define S17_MODULE_MIB_EN                    (1 << 0)

/* MIB Function Register 1 */
#define S17_MIB_FUNC_ALL                     (3 << 24)
#define S17_MIB_CPU_KEEP                     (1 << 20)
#define S17_MIB_BUSY                         (1 << 17)
#define S17_MIB_AT_HALF_EN                   (1 << 16)
#define S17_MIB_TIMER_DEFAULT                0x100

#define S17_MAC_MAX                          7

/* MAC power selector bit definitions */
#define S17_RGMII0_1_8V                       (1 << 19)
#define S17_RGMII1_1_8V                       (1 << 18)

/* SGMII_CTRL bit definitions */
#define S17c_SGMII_EN_LCKDT		      (1 << 0)
#define S17c_SGMII_EN_PLL		      (1 << 1)
#define S17c_SGMII_EN_RX		      (1 << 2)
#define S17c_SGMII_EN_TX		      (1 << 3)
#define S17c_SGMII_EN_SD		      (1 << 4)
#define S17c_SGMII_BW_HIGH		      (1 << 6)
#define S17c_SGMII_SEL_CLK125M		      (1 << 7)
#define S17c_SGMII_TXDR_CTRL_600mV	      (1 << 10)
#define S17c_SGMII_CDR_BW_8		      (3 << 13)
#define S17c_SGMII_DIS_AUTO_LPI_25M	      (1 << 16)
#define S17c_SGMII_MODE_CTRL_SGMII_PHY	      (1 << 22)
#define S17c_SGMII_PAUSE_SG_TX_EN_25M	      (1 << 24)
#define S17c_SGMII_ASYM_PAUSE_25M	      (1 << 25)
#define S17c_SGMII_PAUSE_25M		      (1 << 26)
#define S17c_SGMII_HALF_DUPLEX_25M	      (1 << 30)
#define S17c_SGMII_FULL_DUPLEX_25M	      (1 << 31)

#ifndef BOOL
#define BOOL    int
#endif

/*add feature define here*/

#ifdef CONFIG_AR7242_S17_PHY
#undef HEADER_REG_CONF
#undef HEADER_EN
#endif
int athrs17_init_switch(void);
void athrs17_reg_init(ipq_gmac_board_cfg_t *);
void athrs17_reg_init_lan(ipq_gmac_board_cfg_t *gmac_cfg);
void athrs17_vlan_config(void);
#endif


