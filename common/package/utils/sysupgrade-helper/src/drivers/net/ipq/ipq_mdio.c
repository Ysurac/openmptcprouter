/*
 * Copyright (c) 2012 - 2014 The Linux Foundation. All rights reserved.
 */

#include <common.h>
#include <miiphy.h>
#include <phy.h>
#include <asm/arch-ipq806x/nss/ipq_mdio.h>
#include <asm/arch-ipq806x/nss/msm_ipq806x_gmac.h>

uint ipq_mdio_read(uint phy_addr, uint reg_offset, ushort *data)
{
	uint reg_base = NSS_GMAC0_BASE;
	uint timeout = MII_MDIO_TIMEOUT;
	uint miiaddr;
	uint start;
	uint ret_val;

	miiaddr = (((phy_addr << MIIADDRSHIFT) & MII_ADDRMSK) |
	((reg_offset << MIIREGSHIFT) & MII_REGMSK));

	miiaddr |= (MII_BUSY | MII_CLKRANGE_250_300M);
	writel(miiaddr, (reg_base + MII_ADDR_REG_ADDR));
	udelay(10);

	start = get_timer(0);
	while (get_timer(start) < timeout) {
		if (!(readl(reg_base + MII_ADDR_REG_ADDR) & MII_BUSY)) {
			ret_val = readl(reg_base + MII_DATA_REG_ADDR);
			if (data != NULL)
				*data = ret_val;
			return ret_val;
		}
		udelay(1000);
	}
	return -1;
}

uint ipq_mdio_write(uint phy_addr, uint reg_offset, ushort data)
{
	const uint reg_base = NSS_GMAC0_BASE;
	const uint timeout = MII_MDIO_TIMEOUT;
	uint miiaddr;
	uint start;

	writel(data, (reg_base + MII_DATA_REG_ADDR));

	miiaddr = (((phy_addr << MIIADDRSHIFT) & MII_ADDRMSK) |
			((reg_offset << MIIREGSHIFT) & MII_REGMSK) |
			(MII_WRITE));

	miiaddr |= (MII_BUSY | MII_CLKRANGE_250_300M);
	writel(miiaddr, (reg_base + MII_ADDR_REG_ADDR));
	udelay(10);

	start = get_timer(0);
	while (get_timer(start) < timeout) {
		if (!(readl(reg_base + MII_ADDR_REG_ADDR) & MII_BUSY)) {
			return 0;
		}
		udelay(1000);
	}
	return -1;
}

int ipq_phy_read(struct mii_dev *bus, int addr, int dev_addr, int regnum)
{
	return ipq_mdio_read(addr, regnum, NULL);
}

int ipq_phy_write(struct mii_dev *bus, int addr, int dev_addr, int regnum,
			ushort value)
{
	return ipq_mdio_write(addr, regnum, value);
}

int ipq_phy_mdio_init(char *name)
{
	struct mii_dev *bus = mdio_alloc();

	if(!bus) {
		printf("Failed to allocate IPQ MDIO bus\n");
		return -1;
	}

	bus->read = ipq_phy_read;
	bus->write = ipq_phy_write;
	bus->reset = NULL;
	snprintf(bus->name, sizeof(bus->name), name);

	return mdio_register(bus);
}
