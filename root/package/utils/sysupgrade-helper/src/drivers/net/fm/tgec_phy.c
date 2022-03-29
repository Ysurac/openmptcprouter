/*
 * Copyright 2009-2011 Freescale Semiconductor, Inc.
 *	Andy Fleming <afleming@freescale.com>
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
 * Some part is taken from tsec.c
 */
#include <common.h>
#include <miiphy.h>
#include <phy.h>
#include <asm/io.h>
#include <asm/fsl_tgec.h>
#include <fm_eth.h>

/*
 * Write value to the PHY for this device to the register at regnum, waiting
 * until the write is done before it returns.  All PHY configuration has to be
 * done through the TSEC1 MIIM regs
 */
int tgec_mdio_write(struct mii_dev *bus, int port_addr, int dev_addr,
			int regnum, u16 value)
{
	u32 mdio_ctl;
	u32 stat_val;
	struct tgec_mdio_controller *regs = bus->priv;

	if (dev_addr == MDIO_DEVAD_NONE)
		return 0;

	/* Wait till the bus is free */
	stat_val = MDIO_STAT_CLKDIV(100);
	out_be32(&regs->mdio_stat, stat_val);
	while ((in_be32(&regs->mdio_stat)) & MDIO_STAT_BSY)
		;

	/* Set the port and dev addr */
	mdio_ctl = MDIO_CTL_PORT_ADDR(port_addr) | MDIO_CTL_DEV_ADDR(dev_addr);
	out_be32(&regs->mdio_ctl, mdio_ctl);

	/* Set the register address */
	out_be32(&regs->mdio_addr, regnum & 0xffff);

	/* Wait till the bus is free */
	while ((in_be32(&regs->mdio_stat)) & MDIO_STAT_BSY)
		;

	/* Write the value to the register */
	out_be32(&regs->mdio_data, MDIO_DATA(value));

	/* Wait till the MDIO write is complete */
	while ((in_be32(&regs->mdio_data)) & MDIO_DATA_BSY)
		;

	return 0;
}

/*
 * Reads from register regnum in the PHY for device dev, returning the value.
 * Clears miimcom first.  All PHY configuration has to be done through the
 * TSEC1 MIIM regs
 */
int tgec_mdio_read(struct mii_dev *bus, int port_addr, int dev_addr,
			int regnum)
{
	u32 mdio_ctl;
	u32 stat_val;
	struct tgec_mdio_controller *regs = bus->priv;

	if (dev_addr == MDIO_DEVAD_NONE)
		return 0xffff;

	stat_val = MDIO_STAT_CLKDIV(100);
	out_be32(&regs->mdio_stat, stat_val);
	/* Wait till the bus is free */
	while ((in_be32(&regs->mdio_stat)) & MDIO_STAT_BSY)
		;

	/* Set the Port and Device Addrs */
	mdio_ctl = MDIO_CTL_PORT_ADDR(port_addr) | MDIO_CTL_DEV_ADDR(dev_addr);
	out_be32(&regs->mdio_ctl, mdio_ctl);

	/* Set the register address */
	out_be32(&regs->mdio_addr, regnum & 0xffff);

	/* Wait till the bus is free */
	while ((in_be32(&regs->mdio_stat)) & MDIO_STAT_BSY)
		;

	/* Initiate the read */
	mdio_ctl |= MDIO_CTL_READ;
	out_be32(&regs->mdio_ctl, mdio_ctl);

	/* Wait till the MDIO write is complete */
	while ((in_be32(&regs->mdio_data)) & MDIO_DATA_BSY)
		;

	/* Return all Fs if nothing was there */
	if (in_be32(&regs->mdio_stat) & MDIO_STAT_RD_ER)
		return 0xffff;

	return in_be32(&regs->mdio_data) & 0xffff;
}

int tgec_mdio_reset(struct mii_dev *bus)
{
	return 0;
}

int fm_tgec_mdio_init(bd_t *bis, struct tgec_mdio_info *info)
{
	struct mii_dev *bus = mdio_alloc();

	if (!bus) {
		printf("Failed to allocate FM TGEC MDIO bus\n");
		return -1;
	}

	bus->read = tgec_mdio_read;
	bus->write = tgec_mdio_write;
	bus->reset = tgec_mdio_reset;
	sprintf(bus->name, info->name);

	bus->priv = info->regs;

	return mdio_register(bus);
}
