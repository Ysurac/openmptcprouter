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

#ifndef _QCA8075_PHY_H_
#define _QCA8075_PHY_H_

#define QCA8075_PHY_V1_0_5P			0x004DD0B0
#define QCA8075_PHY_V1_1_5P			0x004DD0B1
#define QCA8075_PHY_V1_1_2P			0x004DD0B2

#define QCA8075_PHY_CONTROL			0
#define QCA8075_PHY_STATUS			1
#define QCA8075_PHY_ID1				2
#define QCA8075_PHY_ID2				3
#define QCA8075_AUTONEG_ADVERT			4
#define QCA8075_LINK_PARTNER_ABILITY		5
#define QCA8075_AUTONEG_EXPANSION		6
#define QCA8075_NEXT_PAGE_TRANSMIT		7
#define QCA8075_LINK_PARTNER_NEXT_PAGE		8
#define QCA8075_1000BASET_CONTROL		9
#define QCA8075_1000BASET_STATUS			10
#define QCA8075_MMD_CTRL_REG			13
#define QCA8075_MMD_DATA_REG			14
#define QCA8075_EXTENDED_STATUS			15
#define QCA8075_PHY_SPEC_CONTROL			16
#define QCA8075_PHY_SPEC_STATUS			17
#define QCA8075_PHY_INTR_MASK			18
#define QCA8075_PHY_INTR_STATUS			19
#define QCA8075_PHY_CDT_CONTROL			22
#define QCA8075_PHY_CDT_STATUS			28
#define QCA8075_DEBUG_PORT_ADDRESS		29
#define QCA8075_DEBUG_PORT_DATA			30
#define	COMBO_PHY_ID	4
#define PSGMII_ID	5

#define QCA8075_DEBUG_PHY_HIBERNATION_CTRL	0xb
#define QCA8075_DEBUG_PHY_POWER_SAVING_CTRL	0x29
#define QCA8075_PHY_MMD7_ADDR_8023AZ_EEE_CTRL	0x3c
#define QCA8075_PHY_MMD3_ADDR_REMOTE_LOOPBACK_CTRL	0x805a
#define QCA8075_PHY_MMD3_WOL_MAGIC_MAC_CTRL1	0x804a
#define QCA8075_PHY_MMD3_WOL_MAGIC_MAC_CTRL2	0x804b
#define QCA8075_PHY_MMD3_WOL_MAGIC_MAC_CTRL3	0x804c
#define QCA8075_PHY_MMD3_WOL_CTRL	0x8012

#define QCA8075_PSGMII_FIFI_CTRL		0x6e
#define QCA8075_PSGMII_CALIB_CTRL	0x27
#define QCA8075_PSGMII_MODE_CTRL	0x6d
#define QCA8075_PHY_PSGMII_MODE_CTRL_ADJUST_VALUE	0x220c

#define QCA8075_PHY_MMD7_NUM		7
#define QCA8075_PHY_MMD3_NUM		3
#define QCA8075_PHY_MMD1_NUM		1

#define QCA8075_PHY_SGMII_STATUS		0x1a	/* sgmii_status  Register  */
#define QCA8075_PHY4_AUTO_SGMII_SELECT	0x40
#define QCA8075_PHY4_AUTO_COPPER_SELECT	0x20
#define QCA8075_PHY4_AUTO_BX1000_SELECT	0x10
#define QCA8075_PHY4_AUTO_FX100_SELECT	0x8

#define QCA8075_PHY_CHIP_CONFIG		0x1f	/* Chip Configuration Register  */
#define BT_BX_SG_REG_SELECT		BIT_15
#define BT_BX_SG_REG_SELECT_OFFSET	15
#define BT_BX_SG_REG_SELECT_LEN		1
#define QCA8075_SG_BX_PAGES		0x0
#define QCA8075_SG_COPPER_PAGES		0x1

#define QCA8075_PHY_PSGMII_BASET		0x0
#define QCA8075_PHY_PSGMII_BX1000	0x1
#define QCA8075_PHY_PSGMII_FX100		0x2
#define QCA8075_PHY_PSGMII_AMDET		0x3
#define QCA8075_PHY_SGMII_BASET		0x4

#define QCA8075_PHY4_PREFER_FIBER	0x400
#define PHY4_PREFER_COPPER		0x0
#define PHY4_PREFER_FIBER		0x1

#define QCA8075_PHY4_FIBER_MODE_1000BX	0x100
#define AUTO_100FX_FIBER		0x0
#define AUTO_1000BX_FIBER		0x1

#define QCA8075_PHY_MDIX			0x0020
#define QCA8075_PHY_MDIX_AUTO		0x0060
#define QCA8075_PHY_MDIX_STATUS		0x0040

#define MODE_CFG_QUAL			BIT_4
#define MODE_CFG_QUAL_OFFSET		4
#define MODE_CFG_QUAL_LEN		4

#define MODE_CFG			BIT_0
#define MODE_CFG_OFFSET			0
#define MODE_CFG_LEN			4

#define QCA8075_PHY_MMD3_ADDR_8023AZ_CLD_CTRL		0x8007
#define QCA8075_PHY_MMD3_ADDR_8023AZ_TIMER_CTRL		0x804e
#define QCA8075_PHY_MMD3_ADDR_CLD_CTRL5			0x8005
#define QCA8075_PHY_MMD3_ADDR_CLD_CTRL3			0x8003

#define AZ_TIMER_CTRL_DEFAULT_VALUE	0x3062
#define AZ_CLD_CTRL_DEFAULT_VALUE	0x83f6
#define AZ_TIMER_CTRL_ADJUST_VALUE	0x7062
#define AZ_CLD_CTRL_ADJUST_VALUE	0x8396

  /*debug port */
#define QCA8075_DEBUG_PORT_RGMII_MODE		18
#define QCA8075_DEBUG_PORT_RGMII_MODE_EN		0x0008

#define QCA8075_DEBUG_PORT_RX_DELAY		0
#define QCA8075_DEBUG_PORT_RX_DELAY_EN		0x8000

#define QCA8075_DEBUG_PORT_TX_DELAY		5
#define QCA8075_DEBUG_PORT_TX_DELAY_EN		0x0100

  /* PHY Registers Field */

  /* Control Register fields  offset:0 */
  /* bits 6,13: 10=1000, 01=100, 00=10 */
#define QCA8075_CTRL_SPEED_MSB		0x0040

  /* Collision test enable */
#define QCA8075_CTRL_COLL_TEST_ENABLE	0x0080

  /* FDX =1, half duplex =0 */
#define QCA8075_CTRL_FULL_DUPLEX		0x0100

  /* Restart auto negotiation */
#define QCA8075_CTRL_RESTART_AUTONEGOTIATION	0x0200

  /* Isolate PHY from MII */
#define QCA8075_CTRL_ISOLATE		0x0400

  /* Power down */
#define QCA8075_CTRL_POWER_DOWN		0x0800

  /* Auto Neg Enable */
#define QCA8075_CTRL_AUTONEGOTIATION_ENABLE	0x1000

  /* Local Loopback Enable */
#define QCA8075_LOCAL_LOOPBACK_ENABLE		0x4000

  /* bits 6,13: 10=1000, 01=100, 00=10 */
#define QCA8075_CTRL_SPEED_LSB			0x2000

  /* 0 = normal, 1 = loopback */
#define QCA8075_CTRL_LOOPBACK			0x4000
#define QCA8075_CTRL_SOFTWARE_RESET              0x8000

#define QCA8075_CTRL_SPEED_MASK			0x2040
#define QCA8075_CTRL_SPEED_1000			0x0040
#define QCA8075_CTRL_SPEED_100			0x2000
#define QCA8075_CTRL_SPEED_10			0x0000

#define QCA8075_RESET_DONE(phy_control)                   \
    (((phy_control) & (QCA8075_CTRL_SOFTWARE_RESET)) == 0)

  /* Status Register fields offset:1 */
  /* Extended register capabilities */
#define QCA8075_STATUS_EXTENDED_CAPS		0x0001

  /* Jabber Detected */
#define QCA8075_STATUS_JABBER_DETECT		0x0002

  /* Link Status 1 = link */
#define QCA8075_STATUS_LINK_STATUS_UP		0x0004

  /* Auto Neg Capable */
#define QCA8075_STATUS_AUTONEG_CAPS		0x0008

  /* Remote Fault Detect */
#define QCA8075_STATUS_REMOTE_FAULT		0x0010

  /* Auto Neg Complete */
#define QCA8075_STATUS_AUTO_NEG_DONE		0x0020

  /* Preamble may be suppressed */
#define QCA8075_STATUS_PREAMBLE_SUPPRESS		0x0040

  /* Ext. status info in Reg 0x0F */
#define QCA8075_STATUS_EXTENDED_STATUS		0x0100

  /* 100T2 Half Duplex Capable */
#define QCA8075_STATUS_100T2_HD_CAPS		0x0200

  /* 100T2 Full Duplex Capable */
#define QCA8075_STATUS_100T2_FD_CAPS		0x0400

  /* 10T   Half Duplex Capable */
#define QCA8075_STATUS_10T_HD_CAPS		0x0800

  /* 10T   Full Duplex Capable */
#define QCA8075_STATUS_10T_FD_CAPS		0x1000

  /* 100X  Half Duplex Capable */
#define QCA8075_STATUS_100X_HD_CAPS		0x2000

  /* 100X  Full Duplex Capable */
#define QCA8075_STATUS_100X_FD_CAPS		0x4000

  /* 100T4 Capable */
#define QCA8075_STATUS_100T4_CAPS		0x8000

  /* extended status register capabilities */

#define QCA8075_STATUS_1000T_HD_CAPS		0x1000

#define QCA8075_STATUS_1000T_FD_CAPS		0x2000

#define QCA8075_STATUS_1000X_HD_CAPS		0x4000

#define QCA8075_STATUS_1000X_FD_CAPS		0x8000

#define QCA8075_AUTONEG_DONE(ip_phy_status) \
	(((ip_phy_status) & (QCA8075_STATUS_AUTO_NEG_DONE)) ==  \
	(QCA8075_STATUS_AUTO_NEG_DONE))

  /* PHY identifier1  offset:2 */
//Organizationally Unique Identifier bits 3:18

  /* PHY identifier2  offset:3 */
//Organizationally Unique Identifier bits 19:24

  /* Auto-Negotiation Advertisement register. offset:4 */
  /* indicates IEEE 802.3 CSMA/CD */
#define QCA8075_ADVERTISE_SELECTOR_FIELD		0x0001

  /* 10T   Half Duplex Capable */
#define QCA8075_ADVERTISE_10HALF			0x0020

  /* 10T   Full Duplex Capable */
#define QCA8075_ADVERTISE_10FULL			0x0040

  /* 100TX Half Duplex Capable */
#define QCA8075_ADVERTISE_100HALF		0x0080

  /* 100TX Full Duplex Capable */
#define QCA8075_ADVERTISE_100FULL		0x0100

  /* 100T4 Capable */
#define QCA8075_ADVERTISE_100T4			0x0200

  /* Pause operation desired */
#define QCA8075_ADVERTISE_PAUSE			0x0400

  /* Asymmetric Pause Direction bit */
#define QCA8075_ADVERTISE_ASYM_PAUSE		0x0800

  /* Remote Fault detected */
#define QCA8075_ADVERTISE_REMOTE_FAULT		0x2000

  /* Next Page ability supported */
#define QCA8075_ADVERTISE_NEXT_PAGE		0x8000

  /* 100TX Half Duplex Capable */
#define QCA8075_ADVERTISE_1000HALF		0x0100

  /* 100TX Full Duplex Capable */
#define QCA8075_ADVERTISE_1000FULL		0x0200

#define QCA8075_ADVERTISE_ALL \
	(QCA8075_ADVERTISE_10HALF | QCA8075_ADVERTISE_10FULL | \
	QCA8075_ADVERTISE_100HALF | QCA8075_ADVERTISE_100FULL | \
	QCA8075_ADVERTISE_1000FULL)

#define QCA8075_ADVERTISE_MEGA_ALL \
	(QCA8075_ADVERTISE_10HALF | QCA8075_ADVERTISE_10FULL | \
	QCA8075_ADVERTISE_100HALF | QCA8075_ADVERTISE_100FULL)

#define QCA8075_BX_ADVERTISE_1000FULL		0x0020
#define QCA8075_BX_ADVERTISE_1000HALF		0x0040
#define QCA8075_BX_ADVERTISE_PAUSE		0x0080
#define QCA8075_BX_ADVERTISE_ASYM_PAUSE		0x0100

#define QCA8075_BX_ADVERTISE_ALL \
	(QCA8075_BX_ADVERTISE_ASYM_PAUSE | QCA8075_BX_ADVERTISE_PAUSE | \
	QCA8075_BX_ADVERTISE_1000HALF | QCA8075_BX_ADVERTISE_1000FULL)

  /* Link Partner ability offset:5 */
  /* Same as advertise selector  */
#define QCA8075_LINK_SLCT		0x001f

  /* Can do 10mbps half-duplex   */
#define QCA8075_LINK_10BASETX_HALF_DUPLEX	0x0020

  /* Can do 10mbps full-duplex   */
#define QCA8075_LINK_10BASETX_FULL_DUPLEX	0x0040

  /* Can do 100mbps half-duplex  */
#define QCA8075_LINK_100BASETX_HALF_DUPLEX	0x0080

  /* Can do 100mbps full-duplex  */
#define QCA8075_LINK_100BASETX_FULL_DUPLEX	0x0100

  /* Can do 1000mbps full-duplex  */
#define QCA8075_LINK_1000BASETX_FULL_DUPLEX	0x0800

  /* Can do 1000mbps half-duplex  */
#define QCA8075_LINK_1000BASETX_HALF_DUPLEX	0x0400

  /* 100BASE-T4  */
#define QCA8075_LINK_100BASE4			0x0200

  /* PAUSE */
#define QCA8075_LINK_PAUSE			0x0400

  /* Asymmetrical PAUSE */
#define QCA8075_LINK_ASYPAUSE			0x0800

  /* Link partner faulted  */
#define QCA8075_LINK_RFAULT			0x2000

  /* Link partner acked us */
#define QCA8075_LINK_LPACK			0x4000

  /* Next page bit  */
#define QCA8075_LINK_NPAGE			0x8000

  /* Auto-Negotiation Expansion Register offset:6 */

  /* Next Page Transmit Register offset:7 */

  /* Link partner Next Page Register offset:8 */

  /* 1000BASE-T Control Register offset:9 */
  /* Advertise 1000T HD capability */
#define QCA8075_CTL_1000T_HD_CAPS		0x0100

  /* Advertise 1000T FD capability  */
#define QCA8075_CTL_1000T_FD_CAPS		0x0200

  /* 1=Repeater/switch device port 0=DTE device */
#define QCA8075_CTL_1000T_REPEATER_DTE		0x0400

  /* 1=Configure PHY as Master  0=Configure PHY as Slave */
#define QCA8075_CTL_1000T_MS_VALUE		0x0800

  /* 1=Master/Slave manual config value  0=Automatic Master/Slave config */
#define QCA8075_CTL_1000T_MS_ENABLE		0x1000

  /* Normal Operation */
#define QCA8075_CTL_1000T_TEST_MODE_NORMAL	0x0000

  /* Transmit Waveform test */
#define QCA8075_CTL_1000T_TEST_MODE_1		0x2000

  /* Master Transmit Jitter test */
#define QCA8075_CTL_1000T_TEST_MODE_2		0x4000

  /* Slave Transmit Jitter test */
#define QCA8075_CTL_1000T_TEST_MODE_3		0x6000

  /* Transmitter Distortion test */
#define QCA8075_CTL_1000T_TEST_MODE_4		0x8000
#define QCA8075_CTL_1000T_SPEED_MASK		0x0300
#define QCA8075_CTL_1000T_DEFAULT_CAP_MASK	0x0300

  /* 1000BASE-T Status Register offset:10 */
  /* LP is 1000T HD capable */
#define QCA8075_STATUS_1000T_LP_HD_CAPS		0x0400

  /* LP is 1000T FD capable */
#define QCA8075_STATUS_1000T_LP_FD_CAPS		0x0800

  /* Remote receiver OK */
#define QCA8075_STATUS_1000T_REMOTE_RX_STATUS	0x1000

  /* Local receiver OK */
#define QCA8075_STATUS_1000T_LOCAL_RX_STATUS	0x2000

  /* 1=Local TX is Master, 0=Slave */
#define QCA8075_STATUS_1000T_MS_CONFIG_RES	0x4000

#define QCA8075_STATUS_1000T_MS_CONFIG_FAULT	0x8000

  /* Master/Slave config fault */
#define QCA8075_STATUS_1000T_REMOTE_RX_STATUS_SHIFT	12
#define QCA8075_STATUS_1000T_LOCAL_RX_STATUS_SHIFT	13

  /* Phy Specific Control Register offset:16 */
  /* 1=Jabber Function disabled */
#define QCA8075_CTL_JABBER_DISABLE	0x0001

  /* 1=Polarity Reversal enabled */
#define QCA8075_CTL_POLARITY_REVERSAL	0x0002

  /* 1=SQE Test enabled */
#define QCA8075_CTL_SQE_TEST		0x0004
#define QCA8075_CTL_MAC_POWERDOWN	0x0008

  /* 1=CLK125 low, 0=CLK125 toggling
     #define QCA8075_CTL_CLK125_DISABLE	0x0010
   */
  /* MDI Crossover Mode bits 6:5 */
  /* Manual MDI configuration */
#define QCA8075_CTL_MDI_MANUAL_MODE	0x0000

  /* Manual MDIX configuration */
#define QCA8075_CTL_MDIX_MANUAL_MODE	0x0020

  /* 1000BASE-T: Auto crossover, 100BASE-TX/10BASE-T: MDI Mode */
#define QCA8075_CTL_AUTO_X_1000T		0x0040

  /* Auto crossover enabled all speeds */
#define QCA8075_CTL_AUTO_X_MODE		0x0060

  /* 1=Enable Extended 10BASE-T distance
   * (Lower 10BASE-T RX Threshold)
   * 0=Normal 10BASE-T RX Threshold */
#define QCA8075_CTL_10BT_EXT_DIST_ENABLE	0x0080

  /* 1=5-Bit interface in 100BASE-TX
   * 0=MII interface in 100BASE-TX */
#define QCA8075_CTL_MII_5BIT_ENABLE	0x0100

  /* 1=Scrambler disable */
#define QCA8075_CTL_SCRAMBLER_DISABLE	0x0200

  /* 1=Force link good */
#define QCA8075_CTL_FORCE_LINK_GOOD	0x0400

  /* 1=Assert CRS on Transmit */
#define QCA8075_CTL_ASSERT_CRS_ON_TX	0x0800

#define QCA8075_CTL_POLARITY_REVERSAL_SHIFT	1
#define QCA8075_CTL_AUTO_X_MODE_SHIFT		5
#define QCA8075_CTL_10BT_EXT_DIST_ENABLE_SHIFT	7


  /* Phy Specific status fields offset:17 */
  /* 1=Speed & Duplex resolved */
#define QCA8075_STATUS_LINK_PASS		0x0400
#define QCA8075_STATUS_RESOVLED		0x0800

  /* 1=Duplex 0=Half Duplex */
#define QCA8075_STATUS_FULL_DUPLEX	0x2000

  /* Speed, bits 14:15 */
#define QCA8075_STATUS_SPEED		0xC000
#define QCA8075_STATUS_SPEED_MASK	0xC000

  /* 00=10Mbs */
#define QCA8075_STATUS_SPEED_10MBS	0x0000

  /* 01=100Mbs */
#define QCA8075_STATUS_SPEED_100MBS	0x4000

  /* 10=1000Mbs */
#define QCA8075_STATUS_SPEED_1000MBS	0x8000
#define QCA8075_SPEED_DUPLEX_RESOVLED(phy_status)	\
    (((phy_status) &					\
        (QCA8075_STATUS_RESOVLED)) ==			\
        (QCA8075_STATUS_RESOVLED))

  /*phy debug port1 register offset:29 */
  /*phy debug port2 register offset:30 */

  /*QCA8075 interrupt flag */
#define QCA8075_INTR_SPEED_CHANGE		0x4000
#define QCA8075_INTR_DUPLEX_CHANGE		0x2000
#define QCA8075_INTR_STATUS_UP_CHANGE		0x0400
#define QCA8075_INTR_STATUS_DOWN_CHANGE		0x0800
#define QCA8075_INTR_BX_FX_STATUS_DOWN_CHANGE	0x0100
#define QCA8075_INTR_BX_FX_STATUS_UP_CHANGE	0x0080
#define QCA8075_INTR_MEDIA_STATUS_CHANGE		0x1000
#define QCA8075_INTR_WOL				0x0001
#define QCA8075_INTR_POE				0x0002

#define RUN_CDT				0x8000
#define CABLE_LENGTH_UNIT		0x0400
#define QCA8075_MAX_TRIES		100

#endif				/* _QCA8075_PHY_H_ */
