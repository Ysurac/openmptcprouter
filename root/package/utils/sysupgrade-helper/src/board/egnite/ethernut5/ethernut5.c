/*
 * (C) Copyright 2011
 * egnite GmbH <info@egnite.de>
 *
 * (C) Copyright 2010
 * Ole Reinhardt <ole.reinhardt@thermotemp.de>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Ethernut 5 general board support
 *
 * Ethernut is an open source hardware and software project for
 * embedded Ethernet devices. Hardware layouts and CAD files are
 * freely available under BSD-like license.
 *
 * Ethernut 5 is the first member of the Ethernut board family
 * with U-Boot and Linux support. This implementation is based
 * on the original work done by Ole Reinhardt, but heavily modified
 * to support additional features and the latest board revision 5.0F.
 *
 * Main board components are by default:
 *
 * Atmel AT91SAM9XE512 CPU with 512 kBytes NOR Flash
 * 2 x 64 MBytes Micron MT48LC32M16A2P SDRAM
 * 512 MBytes Micron MT29F4G08ABADA NAND Flash
 * 4 MBytes Atmel AT45DB321D DataFlash
 * SMSC LAN8710 Ethernet PHY
 * Atmel ATmega168 MCU used for power management
 * Linear Technology LTC4411 PoE controller
 *
 * U-Boot relevant board interfaces are:
 *
 * 100 Mbit Ethernet with IEEE 802.3af PoE
 * RS-232 serial port
 * USB host and device
 * MMC/SD-Card slot
 * Expansion port with I2C, SPI and more...
 *
 * Typically the U-Boot image is loaded from serial DataFlash into
 * SDRAM by the samboot boot loader, which is located in internal
 * NOR Flash and provides all essential initializations like CPU
 * and peripheral clocks and, of course, the SDRAM configuration.
 *
 * For testing purposes it is also possibly to directly transfer
 * the image into SDRAM via JTAG. A tested configuration exists
 * for the Turtelizer 2 hardware dongle and the OpenOCD software.
 * In this case the latter will do the basic hardware configuration
 * via its reset-init script.
 *
 * For additional information visit the project home page at
 * http://www.ethernut.de/
 */

#include <common.h>
#include <net.h>
#include <netdev.h>
#include <miiphy.h>
#include <i2c.h>
#include <spi.h>
#include <dataflash.h>
#include <mmc.h>
#include <atmel_mci.h>

#include <asm/arch/at91sam9260.h>
#include <asm/arch/at91sam9260_matrix.h>
#include <asm/arch/at91sam9_smc.h>
#include <asm/arch/at91_common.h>
#include <asm/arch/at91_pmc.h>
#include <asm/arch/at91_spi.h>
#include <asm/arch/gpio.h>
#include <asm/io.h>

#include "ethernut5_pwrman.h"

DECLARE_GLOBAL_DATA_PTR;

AT91S_DATAFLASH_INFO dataflash_info[CONFIG_SYS_MAX_DATAFLASH_BANKS];

struct dataflash_addr cs[CONFIG_SYS_MAX_DATAFLASH_BANKS] = {
	{CONFIG_SYS_DATAFLASH_LOGIC_ADDR_CS0, 0}
};

/*
 * In fact we have 7 partitions, but u-boot supports 5 only. This is
 * no big deal, because the first partition is reserved for applications
 * and the last one is used by Nut/OS. Both need not to be visible here.
 */
dataflash_protect_t area_list[NB_DATAFLASH_AREA] = {
	{ 0x00021000, 0x00041FFF, FLAG_PROTECT_SET, 0, "setup" },
	{ 0x00042000, 0x000C5FFF, FLAG_PROTECT_SET, 0, "uboot" },
	{ 0x000C6000, 0x00359FFF, FLAG_PROTECT_SET, 0, "kernel" },
	{ 0x0035A000, 0x003DDFFF, FLAG_PROTECT_SET, 0, "nutos" },
	{ 0x003DE000, 0x003FEFFF, FLAG_PROTECT_CLEAR, 0, "env" }
};

/*
 * This is called last during early initialization. Most of the basic
 * hardware interfaces are up and running.
 *
 * The SDRAM hardware has been configured by the first stage boot loader.
 * We only need to announce its size, using u-boot's memory check.
 */
int dram_init(void)
{
	gd->ram_size = get_ram_size(
			(void *)CONFIG_SYS_SDRAM_BASE,
			CONFIG_SYS_SDRAM_SIZE);
	return 0;
}

#ifdef CONFIG_CMD_NAND
static void ethernut5_nand_hw_init(void)
{
	struct at91_smc *smc = (struct at91_smc *)ATMEL_BASE_SMC;
	struct at91_matrix *matrix = (struct at91_matrix *)ATMEL_BASE_MATRIX;
	unsigned long csa;

	/* Assign CS3 to NAND/SmartMedia Interface */
	csa = readl(&matrix->ebicsa);
	csa |= AT91_MATRIX_CS3A_SMC_SMARTMEDIA;
	writel(csa, &matrix->ebicsa);

	/* Configure SMC CS3 for NAND/SmartMedia */
	writel(AT91_SMC_SETUP_NWE(1) | AT91_SMC_SETUP_NCS_WR(0) |
		AT91_SMC_SETUP_NRD(1) | AT91_SMC_SETUP_NCS_RD(0),
		&smc->cs[3].setup);
	writel(AT91_SMC_PULSE_NWE(3) | AT91_SMC_PULSE_NCS_WR(3) |
		AT91_SMC_PULSE_NRD(3) | AT91_SMC_PULSE_NCS_RD(3),
		&smc->cs[3].pulse);
	writel(AT91_SMC_CYCLE_NWE(5) | AT91_SMC_CYCLE_NRD(5),
		&smc->cs[3].cycle);
	writel(AT91_SMC_MODE_RM_NRD | AT91_SMC_MODE_WM_NWE |
		AT91_SMC_MODE_EXNW_DISABLE |
		AT91_SMC_MODE_DBW_8 |
		AT91_SMC_MODE_TDF_CYCLE(2),
		&smc->cs[3].mode);

#ifdef CONFIG_SYS_NAND_READY_PIN
	/* Ready pin is optional. */
	at91_set_pio_input(CONFIG_SYS_NAND_READY_PIN, 1);
#endif
	at91_set_pio_output(CONFIG_SYS_NAND_ENABLE_PIN, 1);
}
#endif

/*
 * This is called first during late initialization.
 */
int board_init(void)
{
	struct at91_pmc *pmc = (struct at91_pmc *)ATMEL_BASE_PMC;

	/* Enable clocks for all PIOs */
	writel((1 << ATMEL_ID_PIOA) | (1 << ATMEL_ID_PIOB) |
		(1 << ATMEL_ID_PIOC),
		&pmc->pcer);
	/* Set adress of boot parameters. */
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;
	/* Initialize UARTs and power management. */
	at91_seriald_hw_init();
	ethernut5_power_init();
#ifdef CONFIG_CMD_NAND
	ethernut5_nand_hw_init();
#endif
#ifdef CONFIG_HAS_DATAFLASH
	at91_spi0_hw_init(1 << 0);
#endif
	return 0;
}

#ifdef CONFIG_MACB
/*
 * This is optionally called last during late initialization.
 */
int board_eth_init(bd_t *bis)
{
	const char *devname;
	unsigned short mode;
	struct at91_pmc *pmc = (struct at91_pmc *)ATMEL_BASE_PMC;

	/* Enable on-chip EMAC clock. */
	writel(1 << ATMEL_ID_EMAC0, &pmc->pcer);
	/* Need to reset PHY via power management. */
	ethernut5_phy_reset();
	/* Set peripheral pins. */
	at91_macb_hw_init();
	/* Basic EMAC initialization. */
	if (macb_eth_initialize(0, (void *)ATMEL_BASE_EMAC0, CONFIG_PHY_ID))
		return -1;
	/*
	 * Early board revisions have a pull-down at the PHY's MODE0
	 * strap pin, which forces the PHY into power down. Here we
	 * switch to all-capable mode.
	 */
	devname = miiphy_get_current_dev();
	if (miiphy_read(devname, 0, 18, &mode) == 0) {
		/* Set mode[2:0] to 0b111. */
		mode |= 0x00E0;
		miiphy_write(devname, 0, 18, mode);
		/* Soft reset overrides strap pins. */
		miiphy_write(devname, 0, MII_BMCR, BMCR_RESET);
	}
	/* Sync environment with network devices, needed for nfsroot. */
	return eth_init(gd->bd);
}
#endif

#ifdef CONFIG_GENERIC_ATMEL_MCI
int board_mmc_init(bd_t *bd)
{
	struct at91_pmc *pmc = (struct at91_pmc *)ATMEL_BASE_PMC;

	/* Enable MCI clock. */
	writel(1 << ATMEL_ID_MCI, &pmc->pcer);
	/* Initialize MCI hardware. */
	at91_mci_hw_init();
	/* Register the device. */
	return atmel_mci_init((void *)ATMEL_BASE_MCI);
}

int board_mmc_getcd(struct mmc *mmc)
{
	return !at91_get_pio_value(CONFIG_SYS_MMC_CD_PIN);
}
#endif

#ifdef CONFIG_ATMEL_SPI
/*
 * Note, that u-boot uses different code for SPI bus access. While
 * memory routines use automatic chip select control, the serial
 * flash support requires 'manual' GPIO control. Thus, we switch
 * modes.
 */
void spi_cs_activate(struct spi_slave *slave)
{
	/* Enable NPCS0 in GPIO mode. This disables peripheral control. */
	at91_set_pio_output(AT91_PIO_PORTA, 3, 0);
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	/* Disable NPCS0 in GPIO mode. */
	at91_set_pio_output(AT91_PIO_PORTA, 3, 1);
	/* Switch back to peripheral chip select control. */
	at91_set_a_periph(AT91_PIO_PORTA, 3, 1);
}

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return bus == 0 && cs == 0;
}
#endif
