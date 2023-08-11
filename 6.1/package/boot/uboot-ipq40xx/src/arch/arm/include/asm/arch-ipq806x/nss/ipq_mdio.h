/*
 * Copyright (c) 2012 - 2014 The Linux Foundation. All rights reserved.
 */

/* MII address register definitions */
#define MII_BUSY				(1 << 0)
#define MII_WRITE				(1 << 1)
#define MII_CLKRANGE_60_100M			(0)
#define MII_CLKRANGE_100_150M			(0x4)
#define MII_CLKRANGE_20_35M			(0x8)
#define MII_CLKRANGE_35_60M			(0xC)
#define MII_CLKRANGE_150_250M			(0x10)
#define MII_CLKRANGE_250_300M			(0x14)
#define MII_DATA_REG_ADDR			(0x14)
#define MII_ADDR_REG_ADDR			(0x10)
#define MII_MDIO_TIMEOUT			(10000)
#define MIIADDRSHIFT				(11)
#define MIIREGSHIFT				(6)
#define MII_REGMSK	 			(0x1F << 6)
#define MII_ADDRMSK				(0x1F << 11)
#define MII_PHY_STAT_SHIFT			(10)
#define AUTO_NEG_ENABLE				(1 << 12)

/*
 * Below is "88E1011/88E1011S Integrated 10/100/1000 Gigabit Ethernet Transceiver"
 * Register and their layouts. This Phy has been used in the Dot Aster GMAC Phy daughter.
 * Since the Phy register map is standard, this map hardly changes to a different Ppy
 */
enum MiiRegisters
{
	PHY_CONTROL_REG			= 0x0000,	/* Control Register */
	PHY_STATUS_REG			= 0x0001,	/* Status Register */
	PHY_ID_HI_REG			= 0x0002,	/* PHY Identifier High Register */
	PHY_ID_LOW_REG			= 0x0003,	/* PHY Identifier High Register */
	PHY_AN_ADV_REG			= 0x0004,	/* Auto-Negotiation Advertisement Register */
	PHY_LNK_PART_ABl_REG		= 0x0005,	/* Link Partner Ability Register (Base Page) */
	PHY_AN_EXP_REG			= 0x0006,	/* Auto-Negotiation Expansion Register */
	PHY_AN_NXT_PAGE_TX_REG		= 0x0007,	/* Next Page Transmit Register */
	PHY_LNK_PART_NXT_PAGE_REG	= 0x0008,	/* Link Partner Next Page Register */
	PHY_1000BT_CTRL_REG		= 0x0009,	/* 1000BASE-T Control Register */
	PHY_1000BT_STATUS_REG		= 0x000a,	/* 1000BASE-T Status Register */
	PHY_SPECIFIC_CTRL_REG		= 0x0010,	/* Phy specific control register */
	PHY_SPECIFIC_STATUS_REG		= 0x0011,	/* Phy specific status register */
	PHY_INTERRUPT_ENABLE_REG	= 0x0012,	/* Phy interrupt enable register */
	PHY_INTERRUPT_STATUS_REG	= 0x0013,	/* Phy interrupt status register */
	PHY_EXT_PHY_SPC_CTRL		= 0x0014,	/* Extended Phy specific control */
	PHY_RX_ERR_COUNTER		= 0x0015,	/* Receive Error Counter */
	PHY_EXT_ADDR_CBL_DIAG		= 0x0016,	/* Extended address for cable diagnostic register */
	PHY_LED_CONTROL			= 0x0018,	/* LED Control */
	PHY_MAN_LED_OVERIDE		= 0x0019,	/* Manual LED override register */
	PHY_EXT_PHY_SPC_CTRL2		= 0x001a,	/* Extended Phy specific control 2 */
	PHY_EXT_PHY_SPC_STATUS		= 0x001b,	/* Extended Phy specific status */
	PHY_CBL_DIAG_REG		= 0x001c,	/* Cable diagnostic registers */
};

enum Mii_Phy_Status {
	Mii_phy_status_speed_10		= 0x0000,
	Mii_phy_status_speed_100	= 0x4000,
	Mii_phy_status_speed_1000	= 0x8000,

	Mii_phy_status_full_duplex	= 0x2000,
	Mii_phy_status_half_duplex	= 0x0000,

	Mii_phy_status_link_up		= 0x0400,
};

int ipq_phy_mdio_init(char *name);
uint ipq_mdio_write(uint phy_addr, uint reg_offset, ushort data);
uint ipq_mdio_read(uint phy_addr, uint reg_offset, ushort *data);
