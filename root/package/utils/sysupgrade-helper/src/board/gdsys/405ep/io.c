/*
 * (C) Copyright 2010
 * Dirk Eibach,  Guntermann & Drunck GmbH, eibach@gdsys.de
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

#include <common.h>
#include <command.h>
#include <asm/processor.h>
#include <asm/io.h>
#include <asm/ppc4xx-gpio.h>

#include <dtt.h>
#include <miiphy.h>

#include "405ep.h"
#include <gdsys_fpga.h>

#define LATCH0_BASE (CONFIG_SYS_LATCH_BASE)
#define LATCH1_BASE (CONFIG_SYS_LATCH_BASE + 0x100)
#define LATCH2_BASE (CONFIG_SYS_LATCH_BASE + 0x200)

#define PHYREG_CONTROL				0
#define PHYREG_PAGE_ADDRESS			22
#define PHYREG_PG0_COPPER_SPECIFIC_CONTROL_1	16
#define PHYREG_PG2_COPPER_SPECIFIC_CONTROL_2	26

enum {
	UNITTYPE_CCD_SWITCH = 1,
};

enum {
	HWVER_100 = 0,
	HWVER_110 = 1,
	HWVER_121 = 2,
	HWVER_122 = 3,
};

int misc_init_r(void)
{
	/* startup fans */
	dtt_init();

	return 0;
}

int configure_gbit_phy(unsigned char addr)
{
	unsigned short value;

	/* select page 2 */
	if (miiphy_write(CONFIG_SYS_GBIT_MII_BUSNAME, addr,
		PHYREG_PAGE_ADDRESS, 0x0002))
		goto err_out;
	/* disable SGMII autonegotiation */
	if (miiphy_write(CONFIG_SYS_GBIT_MII_BUSNAME, addr,
		PHYREG_PG2_COPPER_SPECIFIC_CONTROL_2, 0x800a))
		goto err_out;
	/* select page 0 */
	if (miiphy_write(CONFIG_SYS_GBIT_MII_BUSNAME, addr,
		PHYREG_PAGE_ADDRESS, 0x0000))
		goto err_out;
	/* switch from powerdown to normal operation */
	if (miiphy_read(CONFIG_SYS_GBIT_MII_BUSNAME, addr,
		PHYREG_PG0_COPPER_SPECIFIC_CONTROL_1, &value))
		goto err_out;
	if (miiphy_write(CONFIG_SYS_GBIT_MII_BUSNAME, addr,
		PHYREG_PG0_COPPER_SPECIFIC_CONTROL_1, value & ~0x0004))
		goto err_out;
	/* reset phy so settings take effect */
	if (miiphy_write(CONFIG_SYS_GBIT_MII_BUSNAME, addr,
		PHYREG_CONTROL, 0x9140))
		goto err_out;

	return 0;

err_out:
	printf("Error writing to the PHY addr=%02x\n", addr);
	return -1;
}

/*
 * Check Board Identity:
 */
int checkboard(void)
{
	char *s = getenv("serial#");

	puts("Board: CATCenter Io");

	if (s != NULL) {
		puts(", serial# ");
		puts(s);
	}

	puts("\n");

	return 0;
}

static void print_fpga_info(void)
{
	struct ihs_fpga *fpga = (struct ihs_fpga *) CONFIG_SYS_FPGA_BASE(0);
	u16 versions = in_le16(&fpga->versions);
	u16 fpga_version = in_le16(&fpga->fpga_version);
	u16 fpga_features = in_le16(&fpga->fpga_features);
	unsigned unit_type;
	unsigned hardware_version;
	unsigned feature_channels;
	unsigned feature_expansion;

	unit_type = (versions & 0xf000) >> 12;
	hardware_version = versions & 0x000f;
	feature_channels = fpga_features & 0x007f;
	feature_expansion = fpga_features & (1<<15);

	puts("FPGA:  ");

	switch (unit_type) {
	case UNITTYPE_CCD_SWITCH:
		printf("CCD-Switch");
		break;

	default:
		printf("UnitType %d(not supported)", unit_type);
		break;
	}

	switch (hardware_version) {
	case HWVER_100:
		printf(" HW-Ver 1.00\n");
		break;

	case HWVER_110:
		printf(" HW-Ver 1.10\n");
		break;

	case HWVER_121:
		printf(" HW-Ver 1.21\n");
		break;

	case HWVER_122:
		printf(" HW-Ver 1.22\n");
		break;

	default:
		printf(" HW-Ver %d(not supported)\n",
		       hardware_version);
		break;
	}

	printf("       FPGA V %d.%02d, features:",
		fpga_version / 100, fpga_version % 100);

	printf(" %d channel(s)", feature_channels);

	printf(", expansion %ssupported\n", feature_expansion ? "" : "un");
}

/*
 * setup Gbit PHYs
 */
int last_stage_init(void)
{
	struct ihs_fpga *fpga = (struct ihs_fpga *) CONFIG_SYS_FPGA_BASE(0);
	unsigned int k;

	print_fpga_info();

	miiphy_register(CONFIG_SYS_GBIT_MII_BUSNAME,
		bb_miiphy_read, bb_miiphy_write);

	for (k = 0; k < 32; ++k)
		configure_gbit_phy(k);

	/* take fpga serdes blocks out of reset */
	out_le16(&fpga->quad_serdes_reset, 0);

	return 0;
}

void gd405ep_init(void)
{
}

void gd405ep_set_fpga_reset(unsigned state)
{
	if (state) {
		out_le16((void *)LATCH0_BASE, CONFIG_SYS_LATCH0_RESET);
		out_le16((void *)LATCH1_BASE, CONFIG_SYS_LATCH1_RESET);
	} else {
		out_le16((void *)LATCH0_BASE, CONFIG_SYS_LATCH0_BOOT);
		out_le16((void *)LATCH1_BASE, CONFIG_SYS_LATCH1_BOOT);
	}
}

void gd405ep_setup_hw(void)
{
	/*
	 * set "startup-finished"-gpios
	 */
	gpio_write_bit(21, 0);
	gpio_write_bit(22, 1);
}

int gd405ep_get_fpga_done(unsigned fpga)
{
	return in_le16((void *)LATCH2_BASE) & CONFIG_SYS_FPGA_DONE(fpga);
}
