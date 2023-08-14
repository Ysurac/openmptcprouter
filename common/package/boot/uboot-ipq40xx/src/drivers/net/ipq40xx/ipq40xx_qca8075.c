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
#include <asm/arch-ipq40xx/ess/ipq40xx_edma.h>
#include <malloc.h>
#include "ipq40xx_edma_eth.h"
#include "ipq40xx_qca8075.h"

extern int ipq40xx_mdio_write(int mii_id,
		int regnum, u16 value);
extern int ipq40xx_mdio_read(int mii_id,
		int regnum, ushort *data);

static u32 qca8075_id;
static u16 qca8075_phy_reg_write(u32 dev_id, u32 phy_id,
		u32 reg_id, u16 reg_val)
{
	ipq40xx_mdio_write(phy_id, reg_id, reg_val);
	return 0;
}

u16 qca8075_phy_reg_read(u32 dev_id, u32 phy_id, u32 reg_id)
{
	return ipq40xx_mdio_read(phy_id, reg_id, NULL);
}

/*
 * phy4 prfer medium
 * get phy4 prefer medum, fiber or copper;
 */
static qca8075_phy_medium_t __phy_prefer_medium_get(u32 dev_id,
                                                   u32 phy_id)
{
	u16 phy_medium;
	phy_medium =
		qca8075_phy_reg_read(dev_id, phy_id,
			QCA8075_PHY_CHIP_CONFIG);

	return ((phy_medium & QCA8075_PHY4_PREFER_FIBER) ?
		QCA8075_PHY_MEDIUM_FIBER : QCA8075_PHY_MEDIUM_COPPER);
}

/*
 *  phy4 activer medium
 *  get phy4 current active medium, fiber or copper;
 */
static qca8075_phy_medium_t __phy_active_medium_get(u32 dev_id,
                                                   u32 phy_id)
{
	u16 phy_data = 0;
	u32 phy_mode;

	phy_mode = qca8075_phy_reg_read(dev_id, phy_id,
				QCA8075_PHY_CHIP_CONFIG);
	phy_mode &= 0x000f;

	if (phy_mode == QCA8075_PHY_PSGMII_AMDET) {
		phy_data = qca8075_phy_reg_read(dev_id,
			phy_id, QCA8075_PHY_SGMII_STATUS);

		if ((phy_data & QCA8075_PHY4_AUTO_COPPER_SELECT)) {
			return QCA8075_PHY_MEDIUM_COPPER;
		} else if ((phy_data & QCA8075_PHY4_AUTO_BX1000_SELECT)) {
			/* PHY_MEDIUM_FIBER_BX1000 */
			return QCA8075_PHY_MEDIUM_FIBER;
		} else if ((phy_data & QCA8075_PHY4_AUTO_FX100_SELECT)) {
			 /* PHY_MEDIUM_FIBER_FX100 */
			return QCA8075_PHY_MEDIUM_FIBER;
		}
		/* link down */
		return __phy_prefer_medium_get(dev_id, phy_id);
	} else if ((phy_mode == QCA8075_PHY_PSGMII_BASET) ||
			(phy_mode == QCA8075_PHY_SGMII_BASET)) {
		return QCA8075_PHY_MEDIUM_COPPER;
	} else if ((phy_mode == QCA8075_PHY_PSGMII_BX1000) ||
			(phy_mode == QCA8075_PHY_PSGMII_FX100)) {
		return QCA8075_PHY_MEDIUM_FIBER;
	} else {
		return QCA8075_PHY_MEDIUM_COPPER;
	}
}

/*
 *  phy4 copper page or fiber page select
 *  set phy4 copper or fiber page
 */

static u8  __phy_reg_pages_sel(u32 dev_id, u32 phy_id,
		qca8075_phy_reg_pages_t phy_reg_pages)
{
	u16 reg_pages;
	reg_pages = qca8075_phy_reg_read(dev_id,
			phy_id, QCA8075_PHY_CHIP_CONFIG);

	if (phy_reg_pages == QCA8075_PHY_COPPER_PAGES) {
		reg_pages |= 0x8000;
	} else if (phy_reg_pages == QCA8075_PHY_SGBX_PAGES) {
		reg_pages &= ~0x8000;
	} else
		return -EINVAL;

	qca8075_phy_reg_write(dev_id, phy_id,
		QCA8075_PHY_CHIP_CONFIG, reg_pages);
	return 0;
}

/*
 *  phy4 reg pages selection by active medium
 *  phy4 reg pages selection
 */
static  u32 __phy_reg_pages_sel_by_active_medium(u32 dev_id,
						u32 phy_id)
{
	qca8075_phy_medium_t phy_medium;
	qca8075_phy_reg_pages_t reg_pages;

	phy_medium = __phy_active_medium_get(dev_id, phy_id);
	if (phy_medium == QCA8075_PHY_MEDIUM_FIBER) {
		reg_pages = QCA8075_PHY_SGBX_PAGES;
	} else if (phy_medium == QCA8075_PHY_MEDIUM_COPPER) {
		reg_pages = QCA8075_PHY_COPPER_PAGES;
	} else {
		return -1;
	}

	return __phy_reg_pages_sel(dev_id, phy_id, reg_pages);
}

u8 qca8075_phy_get_link_status(u32 dev_id, u32 phy_id)
{
	u16 phy_data;
	if (phy_id == COMBO_PHY_ID)
		__phy_reg_pages_sel_by_active_medium(dev_id, phy_id);
	phy_data = qca8075_phy_reg_read(dev_id,
			phy_id, QCA8075_PHY_SPEC_STATUS);
	if (phy_data & QCA8075_STATUS_LINK_PASS)
		return 0;

	return 1;
}

u32 qca8075_phy_get_duplex(u32 dev_id, u32 phy_id,
                      fal_port_duplex_t * duplex)
{
	u16 phy_data;

	if (phy_id == COMBO_PHY_ID) {
		__phy_reg_pages_sel_by_active_medium(dev_id, phy_id);
	}

	phy_data = qca8075_phy_reg_read(dev_id, phy_id,
				QCA8075_PHY_SPEC_STATUS);

	/*
	 * Read duplex
	 */
	if (phy_data & QCA8075_STATUS_FULL_DUPLEX)
		*duplex = FAL_FULL_DUPLEX;
	else
		*duplex = FAL_HALF_DUPLEX;

	return 0;
}

u32 qca8075_phy_get_speed(u32 dev_id, u32 phy_id,
                     fal_port_speed_t * speed)
{
	u16 phy_data;

	if (phy_id == COMBO_PHY_ID) {
		__phy_reg_pages_sel_by_active_medium(dev_id, phy_id);
	}
	phy_data = qca8075_phy_reg_read(dev_id,
			phy_id, QCA8075_PHY_SPEC_STATUS);

	switch (phy_data & QCA8075_STATUS_SPEED_MASK) {
	case QCA8075_STATUS_SPEED_1000MBS:
		*speed = FAL_SPEED_1000;
		break;
	case QCA8075_STATUS_SPEED_100MBS:
		*speed = FAL_SPEED_100;
		break;
	case QCA8075_STATUS_SPEED_10MBS:
		*speed = FAL_SPEED_10;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static u32 qca8075_phy_mmd_write(u32 dev_id, u32 phy_id,
                     u16 mmd_num, u16 reg_id, u16 reg_val)
{
	qca8075_phy_reg_write(dev_id, phy_id,
			QCA8075_MMD_CTRL_REG, mmd_num);
	qca8075_phy_reg_write(dev_id, phy_id,
			QCA8075_MMD_DATA_REG, reg_id);
	qca8075_phy_reg_write(dev_id, phy_id,
			QCA8075_MMD_CTRL_REG,
			0x4000 | mmd_num);
	qca8075_phy_reg_write(dev_id, phy_id,
		QCA8075_MMD_DATA_REG, reg_val);

	return 0;
}

static u16 qca8075_phy_mmd_read(u32 dev_id, u32 phy_id,
		u16 mmd_num, u16 reg_id)
{
	qca8075_phy_reg_write(dev_id, phy_id,
			QCA8075_MMD_CTRL_REG, mmd_num);
	qca8075_phy_reg_write(dev_id, phy_id,
			QCA8075_MMD_DATA_REG, reg_id);
	qca8075_phy_reg_write(dev_id, phy_id,
			QCA8075_MMD_CTRL_REG,
			0x4000 | mmd_num);
	return qca8075_phy_reg_read(dev_id, phy_id,
			QCA8075_MMD_DATA_REG);
}

/*
 *  get phy4 medium is 100fx
 */
static u8 __medium_is_fiber_100fx(u32 dev_id, u32 phy_id)
{
	u16 phy_data;
	phy_data = qca8075_phy_reg_read(dev_id, phy_id,
				QCA8075_PHY_SGMII_STATUS);

	if (phy_data & QCA8075_PHY4_AUTO_FX100_SELECT) {
		return 1;
	}
	/* Link down */
	if ((!(phy_data & QCA8075_PHY4_AUTO_COPPER_SELECT)) &&
	    (!(phy_data & QCA8075_PHY4_AUTO_BX1000_SELECT)) &&
	    (!(phy_data & QCA8075_PHY4_AUTO_SGMII_SELECT))) {

		phy_data = qca8075_phy_reg_read(dev_id, phy_id,
					QCA8075_PHY_CHIP_CONFIG);
		if ((phy_data & QCA8075_PHY4_PREFER_FIBER)
		    && (!(phy_data & QCA8075_PHY4_FIBER_MODE_1000BX))) {
			return 1;
		}
	}
	return 0;
}

/*
 * qca8075_phy_set_hibernate - set hibernate status
 * set hibernate status
 */
static u32 qca8075_phy_set_hibernate(u32 dev_id, u32 phy_id, u8 enable)
{
	u16 phy_data;

	qca8075_phy_reg_write(dev_id, phy_id, QCA8075_DEBUG_PORT_ADDRESS,
					QCA8075_DEBUG_PHY_HIBERNATION_CTRL);

	phy_data = qca8075_phy_reg_read(dev_id, phy_id,
				QCA8075_DEBUG_PORT_DATA);

	if (enable) {
		phy_data |= 0x8000;
	} else {
		phy_data &= ~0x8000;
	}

	qca8075_phy_reg_write(dev_id, phy_id, QCA8075_DEBUG_PORT_DATA, phy_data);
	return 0;
}

/*
 * qca8075_restart_autoneg - restart the phy autoneg
 */
static u32 qca8075_phy_restart_autoneg(u32 dev_id, u32 phy_id)
{
	u16 phy_data;

	if (phy_id == COMBO_PHY_ID) {
		if (__medium_is_fiber_100fx(dev_id, phy_id))
			return -1;
		__phy_reg_pages_sel_by_active_medium(dev_id, phy_id);
	}
	phy_data = qca8075_phy_reg_read(dev_id, phy_id, QCA8075_PHY_CONTROL);
	phy_data |= QCA8075_CTRL_AUTONEGOTIATION_ENABLE;
	qca8075_phy_reg_write(dev_id, phy_id, QCA8075_PHY_CONTROL,
			phy_data | QCA8075_CTRL_RESTART_AUTONEGOTIATION);

	return 0;
}

/*
 * qca8075_phy_get_8023az status
 * get 8023az status
 */
static u32 qca8075_phy_get_8023az(u32 dev_id, u32 phy_id, u8 *enable)
{
	u16 phy_data;
	if (phy_id == COMBO_PHY_ID) {
		if (QCA8075_PHY_MEDIUM_COPPER !=
			__phy_active_medium_get(dev_id, phy_id))
			return -1;
	}
	*enable = 0;

	phy_data = qca8075_phy_mmd_read(dev_id, phy_id, QCA8075_PHY_MMD7_NUM,
					QCA8075_PHY_MMD7_ADDR_8023AZ_EEE_CTRL);

	if ((phy_data & 0x0004) && (phy_data & 0x0002))
		*enable = 1;

	return 0;
}

/*
 * qca8075_phy_set_powersave - set power saving status
 */
static u32 qca8075_phy_set_powersave(u32 dev_id, u32 phy_id, u8 enable)
{
	u16 phy_data;
	u8 status = 0;

	if (phy_id == COMBO_PHY_ID) {
		if (QCA8075_PHY_MEDIUM_COPPER !=
			__phy_active_medium_get(dev_id, phy_id))
			return -1;
	}

	if (enable) {
		qca8075_phy_get_8023az(dev_id, phy_id, &status);
		if (!status) {
			phy_data = qca8075_phy_mmd_read(dev_id, phy_id,
							QCA8075_PHY_MMD3_NUM,
					QCA8075_PHY_MMD3_ADDR_8023AZ_TIMER_CTRL);
			phy_data &= ~(1 << 14);
			qca8075_phy_mmd_write(dev_id, phy_id,
					QCA8075_PHY_MMD3_NUM,
					QCA8075_PHY_MMD3_ADDR_8023AZ_TIMER_CTRL,
					phy_data);
		}
		phy_data = qca8075_phy_mmd_read(dev_id, phy_id,
						QCA8075_PHY_MMD3_NUM,
						QCA8075_PHY_MMD3_ADDR_CLD_CTRL5);
		phy_data &= ~(1 << 14);
		qca8075_phy_mmd_write(dev_id, phy_id, QCA8075_PHY_MMD3_NUM,
					QCA8075_PHY_MMD3_ADDR_CLD_CTRL5,
								phy_data);

		phy_data = qca8075_phy_mmd_read(dev_id, phy_id,
					QCA8075_PHY_MMD3_NUM,
					QCA8075_PHY_MMD3_ADDR_CLD_CTRL3);
		phy_data &= ~(1 << 15);
		qca8075_phy_mmd_write(dev_id, phy_id, QCA8075_PHY_MMD3_NUM,
					QCA8075_PHY_MMD3_ADDR_CLD_CTRL3, phy_data);

	} else {
		phy_data = qca8075_phy_mmd_read(dev_id, phy_id,
					QCA8075_PHY_MMD3_NUM,
					QCA8075_PHY_MMD3_ADDR_8023AZ_TIMER_CTRL);
		phy_data |= (1 << 14);
		qca8075_phy_mmd_write(dev_id, phy_id, QCA8075_PHY_MMD3_NUM,
				QCA8075_PHY_MMD3_ADDR_8023AZ_TIMER_CTRL,
				phy_data);

		phy_data = qca8075_phy_mmd_read(dev_id, phy_id,
					QCA8075_PHY_MMD3_NUM,
					QCA8075_PHY_MMD3_ADDR_CLD_CTRL5);
		phy_data |= (1 << 14);
		qca8075_phy_mmd_write(dev_id, phy_id, QCA8075_PHY_MMD3_NUM,
				QCA8075_PHY_MMD3_ADDR_CLD_CTRL5, phy_data);

		phy_data = qca8075_phy_mmd_read(dev_id, phy_id,
						QCA8075_PHY_MMD3_NUM,
						QCA8075_PHY_MMD3_ADDR_CLD_CTRL3);
		phy_data |= (1 << 15);
		qca8075_phy_mmd_write(dev_id, phy_id, QCA8075_PHY_MMD3_NUM,
				QCA8075_PHY_MMD3_ADDR_CLD_CTRL3, phy_data);

	}
	qca8075_phy_reg_write(dev_id, phy_id, QCA8075_PHY_CONTROL, 0x9040);
	return 0;
}

/*
 * qca8075_phy_set_802.3az
 */
 static u32 qca8075_phy_set_8023az(u32 dev_id, u32 phy_id, u8 enable)
{
	u16 phy_data;

	if (phy_id == COMBO_PHY_ID) {
		if (QCA8075_PHY_MEDIUM_COPPER !=
			 __phy_active_medium_get(dev_id, phy_id))
			return -1;
	}
	phy_data = qca8075_phy_mmd_read(dev_id, phy_id, QCA8075_PHY_MMD7_NUM,
				       QCA8075_PHY_MMD7_ADDR_8023AZ_EEE_CTRL);
	if (enable) {
		phy_data |= 0x0006;

		qca8075_phy_mmd_write(dev_id, phy_id, QCA8075_PHY_MMD7_NUM,
			     QCA8075_PHY_MMD7_ADDR_8023AZ_EEE_CTRL, phy_data);
		if (qca8075_id == QCA8075_PHY_V1_0_5P) {
			/*
			 * Workaround to avoid packet loss and < 10m cable
			 * 1000M link not stable under az enable
			 */
			qca8075_phy_mmd_write(dev_id, phy_id,
				QCA8075_PHY_MMD3_NUM,
				QCA8075_PHY_MMD3_ADDR_8023AZ_TIMER_CTRL,
				AZ_TIMER_CTRL_ADJUST_VALUE);

			qca8075_phy_mmd_write(dev_id, phy_id,
					QCA8075_PHY_MMD3_NUM,
					QCA8075_PHY_MMD3_ADDR_8023AZ_CLD_CTRL,
					AZ_CLD_CTRL_ADJUST_VALUE);
		}
	} else {
		phy_data &= ~0x0006;
		qca8075_phy_mmd_write(dev_id, phy_id, QCA8075_PHY_MMD7_NUM,
				QCA8075_PHY_MMD7_ADDR_8023AZ_EEE_CTRL, phy_data);
		if (qca8075_id == QCA8075_PHY_V1_0_5P) {
			qca8075_phy_mmd_write(dev_id, phy_id,
					QCA8075_PHY_MMD3_NUM,
					QCA8075_PHY_MMD3_ADDR_8023AZ_TIMER_CTRL,
					AZ_TIMER_CTRL_DEFAULT_VALUE);
		}
	}
	qca8075_phy_restart_autoneg(dev_id, phy_id);
	return 0;
}

void ess_reset(void)
{
	writel(0x1, 0x1812008);
	mdelay(10);
	writel(0x0, 0x1812008);
	mdelay(100);
}

void qca8075_ess_reset(void)
{
	int i;
	u32 status;
	/*
	 * Fix phy psgmii RX 20bit
	 */
	qca8075_phy_reg_write(0, 5, 0x0, 0x005b);
	/*
	 * Reset phy psgmii
	 */
	qca8075_phy_reg_write(0, 5, 0x0, 0x001b);
	/*
	 * Release reset phy psgmii
	 */
	qca8075_phy_reg_write(0, 5, 0x0, 0x005b);
	for (i = 0; i < QCA8075_MAX_TRIES; i++) {
		status = qca8075_phy_mmd_read(0, 5, 1, 0x28);
		if(status & 0x1)
			break;
		mdelay(10);
	}
	if (i >= QCA8075_MAX_TRIES)
		printf("qca8075 PSGMII PLL_VCO_CALIB Not Ready\n");
	mdelay(50);
	/*
	 * Check qca8075 psgmii calibration done end.
	 * Freeze phy psgmii RX CDR
	 */
	qca8075_phy_reg_write(0, 5, 0x1a, 0x2230);

	ess_reset();
	/*
	 * Check ipq40xx psgmii calibration done start
	 */
	for (i = 0; i < QCA8075_MAX_TRIES; i++) {
		status = readl(0x000980A0);
		if (status & 0x1)
			break;
		mdelay(10);
	}
	if (i >= QCA8075_MAX_TRIES)
		printf("PSGMII PLL_VCO_CALIB Not Ready\n");
	mdelay(50);
	/*
	 * Check ipq40xx psgmii calibration done end.
	 * Relesae phy psgmii RX CDR
	 */
	qca8075_phy_reg_write(0, 5, 0x1a, 0x3230);
	/*
	 * Release phy psgmii RX 20bit
	 */
	qca8075_phy_reg_write(0, 5, 0x0, 0x005f);
	mdelay(200);
}

void psgmii_self_test(void)
{
	int i, phy, j;
	u32 value;
	u32 phy_t_status;
	u16 status;
	u32 tx_counter_ok, tx_counter_error;
	u32 rx_counter_ok, rx_counter_error;
	u32 tx_counter_ok_high16;
	u32 rx_counter_ok_high16;
	u32 tx_ok, rx_ok;

	/*
	 * Switch to access MII reg for copper
	 */
	qca8075_phy_reg_write(0, 4, 0x1f, 0x8500);
	for (phy = 0; phy < 5; phy++) {
		/*
		 * Enable phy mdio broadcast write
		 */
		qca8075_phy_mmd_write(0, phy, 7, 0x8028, 0x801f);
	}
	/*
	 * Force no link by power down
	 */
	qca8075_phy_reg_write(0, 0x1f, 0x0, 0x1840);
	/*
	 * Packet number
	 */
	qca8075_phy_mmd_write(0, 0x1f, 7, 0x8021, 0x3000);
	qca8075_phy_mmd_write(0, 0x1f, 7, 0x8062, 0x05e0);
	/*
	 * Fix mdi status
	 */
	qca8075_phy_reg_write(0, 0x1f, 0x10, 0x6800);

	for (i = 0; i < 100; i++) {
		phy_t_status = 0;
		for (phy = 0; phy < 5; phy++) {
			value = readl(0xc00066c + (phy * 0xc));
			/*
			 * Enable mac loop back
			 */
			writel((value | (1 << 21)), (0xc00066c + (phy * 0xc)));
		}
		/*
		 * Phy single test
		 */
		for (phy = 0; phy < 5; phy++) {
			/*
			 * Enable loopback
			 */
			qca8075_phy_reg_write(0, phy, 0x0, 0x9000);
			qca8075_phy_reg_write(0, phy, 0x0, 0x4140);
			/*
			 * Check link
			 */
			j = 0;
			while (j < 100) {
				status = qca8075_phy_reg_read(0, phy, 0x11);
				if (status & (1 << 10))
					break;
				mdelay(10);
				j++;
			}
			/*
			 * Enable check
			 */
			qca8075_phy_mmd_write(0, phy, 7, 0x8029, 0x0000);
			qca8075_phy_mmd_write(0, phy, 7, 0x8029, 0x0003);
			/*
			 * Start traffic
			 */
			qca8075_phy_mmd_write(0, phy, 7, 0x8020, 0xa000);
			mdelay(200);
			/*
			 * check counter
			 */
			tx_counter_ok = qca8075_phy_mmd_read(0, phy, 7, 0x802e);
			tx_counter_ok_high16 = qca8075_phy_mmd_read(0, phy, 7, 0x802d);
			tx_counter_error = qca8075_phy_mmd_read(0, phy, 7, 0x802f);
			rx_counter_ok = qca8075_phy_mmd_read(0, phy, 7, 0x802b);
			rx_counter_ok_high16 = qca8075_phy_mmd_read(0, phy, 7, 0x802a);
			rx_counter_error = qca8075_phy_mmd_read(0, phy, 7, 0x802c);
			tx_ok = tx_counter_ok + (tx_counter_ok_high16 << 16);
			rx_ok = rx_counter_ok + (rx_counter_ok_high16 << 16);
			/*
			 * Success
			 */
			if((tx_ok == 0x3000) && (tx_counter_error == 0)) {
				phy_t_status &= (~(1 << phy));
			} else {
				phy_t_status |= (1 << phy);
			}
			/*
			 * Power down
			 */
			qca8075_phy_reg_write(0, phy, 0x0, 0x1840);
		}
		/*
		 * Reset 5-phy
		 */
		qca8075_phy_reg_write(0, 0x1f, 0x0, 0x9000);
		/*
		 * Enable 5-phy loopback
		 */
		qca8075_phy_reg_write(0, 0x1f, 0x0, 0x4140);
		/*
		 * check link
		 */
		j = 0;
		while (j < 100) {
			for (phy = 0; phy < 5; phy++) {
				status = qca8075_phy_reg_read(0, phy, 0x11);
				if (!(status & (1 << 10)))
					break;
			}
			if (phy >= 5)
				break;
			mdelay(10);
			j++;
		}
		/*
		 * Enable check
		 */
		qca8075_phy_mmd_write(0, 0x1f, 7, 0x8029, 0x0000);
		qca8075_phy_mmd_write(0, 0x1f, 7, 0x8029, 0x0003);
		/*
		 * Start traffic
		 */
		qca8075_phy_mmd_write(0, 0x1f, 7, 0x8020, 0xa000);
		mdelay(200);
		for (phy = 0; phy < 5; phy++) {
			/*
			 * Check counter
			 */
			tx_counter_ok = qca8075_phy_mmd_read(0, phy, 7, 0x802e);
			tx_counter_ok_high16 = qca8075_phy_mmd_read(0, phy, 7, 0x802d);
			tx_counter_error = qca8075_phy_mmd_read(0, phy, 7, 0x802f);
			rx_counter_ok = qca8075_phy_mmd_read(0, phy, 7, 0x802b);
			rx_counter_ok_high16 = qca8075_phy_mmd_read(0, phy, 7, 0x802a);
			rx_counter_error = qca8075_phy_mmd_read(0, phy, 7, 0x802c);
			tx_ok = tx_counter_ok + (tx_counter_ok_high16 << 16);
			rx_ok = rx_counter_ok + (rx_counter_ok_high16 << 16);
			debug("rx_ok: %d, tx_ok: %d", rx_ok, tx_ok);
                        debug("rx_counter_error: %d, tx_counter_error: %d",
						rx_counter_error, tx_counter_error);
			/*
			 * Success
			 */
			if ((tx_ok == 0x3000) && (tx_counter_error == 0)) {
				phy_t_status &= (~(1 << (phy + 8)));
			} else {
				phy_t_status |= (1 << (phy + 8));
			}
		}
		if (phy_t_status) {
			qca8075_ess_reset();
		} else {
			break;
		}
	}
	/*
	 * Configuration recover
	 */
	/*
	 * Packet number
	 */
	qca8075_phy_mmd_write(0, 0x1f, 7, 0x8021, 0x0);
	/*
	 * Disable check
	 */
	qca8075_phy_mmd_write(0, 0x1f, 7, 0x8029, 0x0);
	/*
	 * Disable traffic
	 */
	qca8075_phy_mmd_write(0, 0x1f, 7, 0x8020, 0x0);
}

void clear_self_test_config(void)
{
	int phy = 0;
	u32 value = 0;

	/*
	 * Disable phy internal loopback
	 */
	qca8075_phy_reg_write(0, 0x1f, 0x10, 0x6860);
	qca8075_phy_reg_write(0, 0x1f, 0x0, 0x9040);

	for (phy = 0; phy < 5; phy++) {
		value = readl(0xc00066c + (phy * 0xc));
		/*
		 * Disable mac loop back
		 */
		writel((value&(~(1 << 21))), (0xc00066c + (phy * 0xc)));
		/*
		 * Disable phy mdio broadcast writei
		 */
		qca8075_phy_mmd_write(0, phy, 7, 0x8028, 0x001f);
	}

}

void qca8075_phy_set_power(int phy, u8 power)
{
	u16 reg;

	reg = qca8075_phy_reg_read(0, phy, 0x0);

	if (power) {
		reg &= ~(0x400);
	} else {
		reg |= 0x400;
	}

	qca8075_phy_reg_write(0, phy, 0x0, reg);
}

int ipq40xx_qca8075_phy_init(struct ipq40xx_eth_dev *info)
{
	u16 phy_data;
	u32 phy_id = 0;
	struct phy_ops *qca8075_ops;

	qca8075_ops = (struct phy_ops *)malloc(sizeof(struct phy_ops));
	if (!qca8075_ops)
		return -ENOMEM;

	qca8075_ops->phy_get_link_status = qca8075_phy_get_link_status;
	qca8075_ops->phy_get_speed = qca8075_phy_get_speed;
	qca8075_ops->phy_get_duplex = qca8075_phy_get_duplex;
	info->ops = qca8075_ops;

	qca8075_id = phy_data = qca8075_phy_reg_read(0x0, 0x0, QCA8075_PHY_ID1);
	printf ("PHY ID1: 0x%x\n", phy_data);
	phy_data = qca8075_phy_reg_read(0x0, 0x0, QCA8075_PHY_ID2);
	printf ("PHY ID2: 0x%x\n", phy_data);
	qca8075_id = (qca8075_id << 16) | phy_data;

	if (qca8075_id == QCA8075_PHY_V1_0_5P) {
		phy_data = qca8075_phy_mmd_read(0, PSGMII_ID,
			QCA8075_PHY_MMD1_NUM, QCA8075_PSGMII_FIFI_CTRL);
		phy_data &= 0xbfff;
		qca8075_phy_mmd_write(0, PSGMII_ID, QCA8075_PHY_MMD1_NUM,
			QCA8075_PSGMII_FIFI_CTRL, phy_data);
	}

	/*
	 * Enable AZ transmitting ability
	 */
	qca8075_phy_mmd_write(0, PSGMII_ID, QCA8075_PHY_MMD1_NUM,
				QCA8075_PSGMII_MODE_CTRL,
				QCA8075_PHY_PSGMII_MODE_CTRL_ADJUST_VALUE);

	/*
	 * Enable phy power saving function by default
	 */
	if (qca8075_id == QCA8075_PHY_V1_1_2P)
		phy_id = 3;

	if ((qca8075_id == QCA8075_PHY_V1_0_5P) ||
	    (qca8075_id == QCA8075_PHY_V1_1_5P) ||
	    (qca8075_id == QCA8075_PHY_V1_1_2P)) {
		for (; phy_id < 5; phy_id++) {
			qca8075_phy_set_8023az(0x0, phy_id, 0x1);
			qca8075_phy_set_powersave(0x0, phy_id, 0x1);
			qca8075_phy_set_hibernate(0x0, phy_id, 0x1);
		}
	}

	phy_data = qca8075_phy_mmd_read(0, 4, QCA8075_PHY_MMD3_NUM, 0x805a);
	phy_data &= (~(1 << 1));
	qca8075_phy_mmd_write(0, 4, QCA8075_PHY_MMD3_NUM, 0x805a, phy_data);

	return 0;
}
