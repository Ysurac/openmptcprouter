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
#include <miiphy.h>
#include <phy.h>
#include <asm/io.h>
#include <errno.h>
#include "ipq40xx_mdio.h"

struct ipq40xx_mdio_data {
	struct mii_bus *bus;
	int phy_irq[PHY_MAX_ADDR];
};

static int ipq40xx_mdio_wait_busy(void)
{
	int i;
	u32 busy;
	for (i = 0; i < IPQ40XX_MDIO_RETRY; i++) {
		udelay(IPQ40XX_MDIO_DELAY);
		busy = readl(IPQ40XX_MDIO_BASE +
			MDIO_CTRL_4_REG) &
			MDIO_CTRL_4_ACCESS_BUSY;

		if (!busy)
			return 0;
		udelay(IPQ40XX_MDIO_DELAY);
	}
	printf("%s: MDIO operation timed out\n",
			__func__);
	return -ETIMEDOUT;
}

int ipq40xx_mdio_write(int mii_id, int regnum, u16 value)
{
	if (ipq40xx_mdio_wait_busy())
		return -ETIMEDOUT;
	/* Issue the phy addreass and reg */
	writel((mii_id << 8 | regnum),
		IPQ40XX_MDIO_BASE + MDIO_CTRL_1_REG);

	/* Issue a write data */
	writel(value, IPQ40XX_MDIO_BASE + MDIO_CTRL_2_REG);

	/* Issue write command */
	writel((MDIO_CTRL_4_ACCESS_START |
		MDIO_CTRL_4_ACCESS_CODE_WRITE),
		(IPQ40XX_MDIO_BASE + MDIO_CTRL_4_REG));

	/* Wait for write complete */

	if (ipq40xx_mdio_wait_busy())
		return -ETIMEDOUT;

	return 0;
}

int ipq40xx_mdio_read(int mii_id, int regnum, ushort *data)
{
	u32 val;
	if (ipq40xx_mdio_wait_busy())
		return -ETIMEDOUT;

	/* Issue the phy address and reg */
	writel((mii_id << 8) | regnum,
		IPQ40XX_MDIO_BASE + MDIO_CTRL_1_REG);

	/* issue read command */
	writel((MDIO_CTRL_4_ACCESS_START |
		MDIO_CTRL_4_ACCESS_CODE_READ),
		(IPQ40XX_MDIO_BASE + MDIO_CTRL_4_REG));

	if (ipq40xx_mdio_wait_busy())
		return -ETIMEDOUT;

	/* Read data */
	val = readl(IPQ40XX_MDIO_BASE + MDIO_CTRL_3_REG);

	if (data != NULL)
		*data = val;

	return val;
}

int ipq40xx_phy_write(struct mii_dev *bus,
		int addr, int dev_addr,
		int regnum, ushort value)
{
	return ipq40xx_mdio_write(
		addr, regnum, value);
}

int ipq40xx_phy_read(struct mii_dev *bus,
		int addr, int dev_addr, int regnum)
{
	return ipq40xx_mdio_read(
		addr, regnum, NULL);
}

int ipq40xx_sw_mdio_init(const char *name)
{
	struct mii_dev *bus = mdio_alloc();
	if(!bus) {
		printf("Failed to allocate IPQ MDIO bus\n");
		return -1;
	}
	bus->read = ipq40xx_phy_read;
	bus->write = ipq40xx_phy_write;
	bus->reset = NULL;
	snprintf(bus->name, MDIO_NAME_LEN, name);
	return mdio_register(bus);
}

