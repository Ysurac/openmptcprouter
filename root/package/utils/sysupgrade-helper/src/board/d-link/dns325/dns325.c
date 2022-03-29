/*
 * Copyright (C) 2011
 * Stefan Herbrechtsmeier <stefan@code.herbrechtsmeier.net>
 *
 * Based on Kirkwood support:
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <common.h>
#include <miiphy.h>
#include <netdev.h>
#include <asm/arch/cpu.h>
#include <asm/arch/kirkwood.h>
#include <asm/arch/mpp.h>
#include <asm/arch/gpio.h>
#include "dns325.h"

DECLARE_GLOBAL_DATA_PTR;

int board_early_init_f(void)
{
	/* Gpio configuration */
	kw_config_gpio(DNS325_OE_VAL_LOW, DNS325_OE_VAL_HIGH,
			DNS325_OE_LOW, DNS325_OE_HIGH);

	/* Multi-Purpose Pins Functionality configuration */
	u32 kwmpp_config[] = {
		MPP0_NF_IO2,
		MPP1_NF_IO3,
		MPP2_NF_IO4,
		MPP3_NF_IO5,
		MPP4_NF_IO6,
		MPP5_NF_IO7,
		MPP6_SYSRST_OUTn,
		MPP7_GPO,
		MPP8_TW_SDA,
		MPP9_TW_SCK,
		MPP10_UART0_TXD,
		MPP11_UART0_RXD,
		MPP12_SD_CLK,
		MPP13_SD_CMD,
		MPP14_SD_D0,
		MPP15_SD_D1,
		MPP16_SD_D2,
		MPP17_SD_D3,
		MPP18_NF_IO0,
		MPP19_NF_IO1,
		MPP20_SATA1_ACTn,	/* sata1(left) status led */
		MPP21_SATA0_ACTn,	/* sata0(right) status led */
		MPP22_GPIO,
		MPP23_GPIO,
		MPP24_GPIO,		/* power off out */
		MPP25_GPIO,
		MPP26_GPIO,		/* power led */
		MPP27_GPIO,		/* sata0(right) error led */
		MPP28_GPIO,		/* sata1(left) error led */
		MPP29_GPIO,		/* usb error led */
		MPP30_GPIO,
		MPP31_GPIO,
		MPP32_GPIO,
		MPP33_GPIO,
		MPP34_GPIO,		/* power key */
		MPP35_GPIO,
		MPP36_GPIO,
		MPP37_GPIO,
		MPP38_GPIO,
		MPP39_GPIO,		/* enable sata 0 */
		MPP40_GPIO,		/* enable sata 1 */
		MPP41_GPIO,		/* hdd0 present */
		MPP42_GPIO,		/* hdd1 present */
		MPP43_GPIO,		/* usb status led */
		MPP44_GPIO,		/* fan status */
		MPP45_GPIO,		/* fan high speed */
		MPP46_GPIO,		/* fan low speed */
		MPP47_GPIO,		/* usb umount */
		MPP48_GPIO,		/* factory reset */
		MPP49_GPIO,		/* thermal sensor */
		0
	};
	kirkwood_mpp_conf(kwmpp_config, NULL);

	kw_gpio_set_blink(DNS325_GPIO_LED_POWER , 1);

	kw_gpio_set_value(DNS325_GPIO_SATA0_EN , 1);
	return 0;
}

int board_init(void)
{
	/* Boot parameters address */
	gd->bd->bi_boot_params = kw_sdram_bar(0) + 0x100;

	return 0;
}

#ifdef CONFIG_RESET_PHY_R
/* Configure and initialize PHY */
void reset_phy(void)
{
	u16 reg;
	u16 devadr;
	char *name = "egiga0";

	if (miiphy_set_current_dev(name))
		return;

	/* command to read PHY dev address */
	if (miiphy_read(name, 0xEE, 0xEE, (u16 *) &devadr)) {
		printf("Err..(%s) could not read PHY dev address\n", __func__);
		return;
	}

	/*
	 * Enable RGMII delay on Tx and Rx for CPU port
	 * Ref: sec 4.7.2 of chip datasheet
	 */
	miiphy_write(name, devadr, MV88E1116_PGADR_REG, 2);
	miiphy_read(name, devadr, MV88E1116_MAC_CTRL_REG, &reg);
	reg |= (MV88E1116_RGMII_RXTM_CTRL | MV88E1116_RGMII_TXTM_CTRL);
	miiphy_write(name, devadr, MV88E1116_MAC_CTRL_REG, reg);
	miiphy_write(name, devadr, MV88E1116_PGADR_REG, 0);

	/* reset the phy */
	miiphy_reset(name, devadr);

	debug("88E1116 Initialized on %s\n", name);
}
#endif /* CONFIG_RESET_PHY_R */
