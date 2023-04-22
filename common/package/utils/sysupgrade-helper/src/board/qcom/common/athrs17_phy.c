/*
 * Copyright (c) 2015 The Linux Foundation. All rights reserved.
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

/*
 * Manage the atheros ethernet PHY.
 *
 * All definitions in this file are operating system independent!
 */

#include <common.h>
#include <miiphy.h>
#include "athrs17_phy.h"
#include "../../../drivers/net/ipq/ipq_gmac.h"
/******************************************************************************
 * FUNCTION DESCRIPTION: Read switch internal register.
 *                       Switch internal register is accessed through the
 *                       MDIO interface. MDIO access is only 16 bits wide so
 *                       it needs the two time access to complete the internal
 *                       register access.
 * INPUT               : register address
 * OUTPUT              : Register value
 *
 *****************************************************************************/
static uint32_t
athrs17_reg_read(ipq_gmac_board_cfg_t *gmac_cfg, uint32_t reg_addr)
{
	uint32_t reg_word_addr;
	uint32_t phy_addr, reg_val;
	uint16_t phy_val;
	uint16_t tmp_val;
	uint8_t phy_reg;

	/* change reg_addr to 16-bit word address, 32-bit aligned */
	reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

	/* configure register high address */
	phy_addr = 0x18;
	phy_reg = 0x0;
	phy_val = (uint16_t) ((reg_word_addr >> 8) & 0x1ff);  /* bit16-8 of reg address */
	miiphy_write(gmac_cfg->phy_name, phy_addr, phy_reg, phy_val);

	/*
	 * For some registers such as MIBs, since it is read/clear, we should
	 * read the lower 16-bit register then the higher one
	 */

	/* read register in lower address */
	phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
	phy_reg = (uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
	miiphy_read(gmac_cfg->phy_name, phy_addr, phy_reg, &phy_val);

	/* read register in higher address */
	reg_word_addr++;
	phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
	phy_reg = (uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
	miiphy_read(gmac_cfg->phy_name, phy_addr, phy_reg, &tmp_val);
	reg_val = (tmp_val << 16 | phy_val);

	return reg_val;
}

/******************************************************************************
 * FUNCTION DESCRIPTION: Write switch internal register.
 *                       Switch internal register is accessed through the
 *                       MDIO interface. MDIO access is only 16 bits wide so
 *                       it needs the two time access to complete the internal
 *                       register access.
 * INPUT               : register address, value to be written
 * OUTPUT              : NONE
 *
 *****************************************************************************/
static void
athrs17_reg_write(ipq_gmac_board_cfg_t *gmac_cfg, uint32_t reg_addr,
		   uint32_t reg_val)
{
	uint32_t reg_word_addr;
	uint32_t phy_addr;
	uint16_t phy_val;
	uint8_t phy_reg;

	/* change reg_addr to 16-bit word address, 32-bit aligned */
	reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

	/* configure register high address */
	phy_addr = 0x18;
	phy_reg = 0x0;
	phy_val = (uint16_t) ((reg_word_addr >> 8) & 0x1ff);  /* bit16-8 of reg address */
	miiphy_write(gmac_cfg->phy_name, phy_addr, phy_reg, phy_val);

	/*
	 * For some registers such as ARL and VLAN, since they include BUSY bit
	 * in lower address, we should write the higher 16-bit register then the
	 * lower one
	 */

	/* read register in higher address */
	reg_word_addr++;
	phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
	phy_reg = (uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
	phy_val = (uint16_t) ((reg_val >> 16) & 0xffff);
	miiphy_write(gmac_cfg->phy_name, phy_addr, phy_reg, phy_val);

	/* write register in lower address */
	reg_word_addr--;
	phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
	phy_reg = (uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
	phy_val = (uint16_t) (reg_val & 0xffff);
	miiphy_write(gmac_cfg->phy_name, phy_addr, phy_reg, phy_val);
}

/*********************************************************************
 * FUNCTION DESCRIPTION: V-lan configuration given by Switch team
			 Vlan 1:PHY0,1,2,3 and Mac 6 of s17c
			 Vlan 2:PHY4 and Mac 0 of s17c
 * INPUT : NONE
 * OUTPUT: NONE
 *********************************************************************/
void athrs17_vlan_config(ipq_gmac_board_cfg_t *gmac_cfg)
{
	athrs17_reg_write(gmac_cfg, S17_P0LOOKUP_CTRL_REG, 0x00140020);
	athrs17_reg_write(gmac_cfg, S17_P0VLAN_CTRL0_REG, 0x20001);

	athrs17_reg_write(gmac_cfg, S17_P1LOOKUP_CTRL_REG, 0x0014005c);
	athrs17_reg_write(gmac_cfg, S17_P1VLAN_CTRL0_REG, 0x10001);

	athrs17_reg_write(gmac_cfg, S17_P2LOOKUP_CTRL_REG, 0x0014005a);
	athrs17_reg_write(gmac_cfg, S17_P2VLAN_CTRL0_REG, 0x10001);

	athrs17_reg_write(gmac_cfg, S17_P3LOOKUP_CTRL_REG, 0x00140056);
	athrs17_reg_write(gmac_cfg, S17_P3VLAN_CTRL0_REG, 0x10001);

	athrs17_reg_write(gmac_cfg, S17_P4LOOKUP_CTRL_REG, 0x0014004e);
	athrs17_reg_write(gmac_cfg, S17_P4VLAN_CTRL0_REG, 0x10001);

	athrs17_reg_write(gmac_cfg, S17_P5LOOKUP_CTRL_REG, 0x00140001);
	athrs17_reg_write(gmac_cfg, S17_P5VLAN_CTRL0_REG, 0x20001);

	athrs17_reg_write(gmac_cfg, S17_P6LOOKUP_CTRL_REG, 0x0014001e);
	athrs17_reg_write(gmac_cfg, S17_P6VLAN_CTRL0_REG, 0x10001);
	printf("%s ...done\n", __func__);
}

/*******************************************************************
* FUNCTION DESCRIPTION: Reset S17 register
* INPUT: NONE
* OUTPUT: NONE
*******************************************************************/
int athrs17_init_switch(ipq_gmac_board_cfg_t *gmac_cfg)
{
	uint32_t data;
	uint32_t i = 0;

	/* Reset the switch before initialization */
	athrs17_reg_write(gmac_cfg, S17_MASK_CTRL_REG, S17_MASK_CTRL_SOFT_RET);
	do {
		udelay(10);
		data = athrs17_reg_read(gmac_cfg, S17_MASK_CTRL_REG);
	} while (data & S17_MASK_CTRL_SOFT_RET);

	do {
		udelay(10);
		data = athrs17_reg_read(gmac_cfg, S17_GLOBAL_INT0_REG);
		i++;
		if (i == 10)
			return -1;
	} while ((data & S17_GLOBAL_INITIALIZED_STATUS) != S17_GLOBAL_INITIALIZED_STATUS);

	return 0;
}

/*********************************************************************
 * FUNCTION DESCRIPTION: Configure S17 register
 * INPUT : NONE
 * OUTPUT: NONE
 *********************************************************************/
void athrs17_reg_init(ipq_gmac_board_cfg_t *gmac_cfg)
{
	uint32_t data;

	data = athrs17_reg_read(gmac_cfg, S17_MAC_PWR_REG) | gmac_cfg->mac_pwr0;
	athrs17_reg_write(gmac_cfg, S17_MAC_PWR_REG, data);

	athrs17_reg_write(gmac_cfg, S17_P0STATUS_REG, (S17_SPEED_1000M | S17_TXMAC_EN |
					     S17_RXMAC_EN | S17_TX_FLOW_EN |
					     S17_RX_FLOW_EN | S17_DUPLEX_FULL));

	athrs17_reg_write(gmac_cfg, S17_GLOFW_CTRL1_REG, (S17_IGMP_JOIN_LEAVE_DPALL |
						S17_BROAD_DPALL |
						S17_MULTI_FLOOD_DPALL |
						S17_UNI_FLOOD_DPALL));

	athrs17_reg_write(gmac_cfg, S17_P5PAD_MODE_REG, S17_MAC0_RGMII_RXCLK_DELAY);
	athrs17_reg_write(gmac_cfg, S17_P0PAD_MODE_REG, (S17_MAC0_RGMII_EN | \
		S17_MAC0_RGMII_TXCLK_DELAY | S17_MAC0_RGMII_RXCLK_DELAY | \
		(0x1 << S17_MAC0_RGMII_TXCLK_SHIFT) | \
		(0x3 << S17_MAC0_RGMII_RXCLK_SHIFT)));

	printf("%s: complete\n", __func__);
}

/*********************************************************************
 * FUNCTION DESCRIPTION: Configure S17 register
 * INPUT : NONE
 * OUTPUT: NONE
 *********************************************************************/
void athrs17_reg_init_lan(ipq_gmac_board_cfg_t *gmac_cfg)
{
	uint32_t reg_val;

	athrs17_reg_write(gmac_cfg, S17_P6STATUS_REG, (S17_SPEED_1000M | S17_TXMAC_EN |
					     S17_RXMAC_EN |
					     S17_DUPLEX_FULL));

	reg_val = athrs17_reg_read(gmac_cfg, S17_MAC_PWR_REG) | gmac_cfg->mac_pwr1;
	athrs17_reg_write(gmac_cfg, S17_MAC_PWR_REG, reg_val);

	reg_val = athrs17_reg_read(gmac_cfg, S17_P6PAD_MODE_REG);
	athrs17_reg_write(gmac_cfg, S17_P6PAD_MODE_REG, (reg_val | S17_MAC6_SGMII_EN));

	reg_val = athrs17_reg_read(gmac_cfg, S17_PWS_REG);
	athrs17_reg_write(gmac_cfg, S17_PWS_REG, (reg_val |
			   S17c_PWS_SERDES_ANEG_DISABLE));

	athrs17_reg_write(gmac_cfg, S17_SGMII_CTRL_REG,(S17c_SGMII_EN_PLL |
					S17c_SGMII_EN_RX |
					S17c_SGMII_EN_TX |
					S17c_SGMII_EN_SD |
					S17c_SGMII_BW_HIGH |
					S17c_SGMII_SEL_CLK125M |
					S17c_SGMII_TXDR_CTRL_600mV |
					S17c_SGMII_CDR_BW_8 |
					S17c_SGMII_DIS_AUTO_LPI_25M |
					S17c_SGMII_MODE_CTRL_SGMII_PHY |
					S17c_SGMII_PAUSE_SG_TX_EN_25M |
					S17c_SGMII_ASYM_PAUSE_25M |
					S17c_SGMII_PAUSE_25M |
					S17c_SGMII_HALF_DUPLEX_25M |
					S17c_SGMII_FULL_DUPLEX_25M));
}

struct athrs17_regmap {
	uint32_t start;
	uint32_t end;
};

struct athrs17_regmap regmap[] = {
	{ 0x000, 0x0e0 },
	{ 0x100, 0x168 },
	{ 0x200, 0x270 },
	{ 0x400, 0x454 },
	{ 0x600, 0x718 },
	{ 0x800, 0xb70 },
	{ 0xC00, 0xC80 },
};

int do_ar8xxx_dump(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]) {
	int i;

	for (i = 0; i < ARRAY_SIZE(regmap); i++) {
		uint32_t reg;
		struct athrs17_regmap *section = &regmap[i];

		for (reg = section->start; reg <= section->end; reg += sizeof(uint32_t)) {
			ipq_gmac_board_cfg_t *gmac_tmp_cfg = gboard_param->gmac_cfg;
			uint32_t val = athrs17_reg_read(gmac_tmp_cfg, reg);
			printf("%03zu: %08zu\n", reg, val);
		}
	}

	return 0;
};
U_BOOT_CMD(
	ar8xxx_dump,    1,    1,    do_ar8xxx_dump,
	"Dump ar8xxx registers",
	"\n    - print all ar8xxx registers\n"
);

/*********************************************************************
 *
 * FUNCTION DESCRIPTION: This function invokes RGMII,
 * 			SGMII switch init routines.
 * INPUT : ipq_gmac_board_cfg_t *
 * OUTPUT: NONE
 *
**********************************************************************/
int ipq_athrs17_init(ipq_gmac_board_cfg_t *gmac_cfg)
{
	int ret;

	if (gmac_cfg == NULL)
		return -1;

	ret = athrs17_init_switch(gmac_cfg);
	if (ret != -1) {
		athrs17_reg_init(gmac_cfg);
		athrs17_reg_init_lan(gmac_cfg);
		athrs17_vlan_config(gmac_cfg);
		printf ("S17c init  done\n");
	}

	return ret;
}
