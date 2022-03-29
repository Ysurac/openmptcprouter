/*
 * (C) Copyright 2004-2008
 * Texas Instruments, <www.ti.com>
 *
 * Author :
 *	Sunil Kumar <sunilsaini05@gmail.com>
 *	Shashi Ranjan <shashiranjanmca05@gmail.com>
 *
 * (C) Copyright 2009
 * Frederik Kriewitz <frederik@kriewitz.eu>
 *
 * Derived from Beagle Board and 3430 SDP code by
 *	Richard Woodruff <r-woodruff2@ti.com>
 *	Syed Mohammed Khasim <khasim@ti.com>
 *
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
#include <twl4030.h>
#include <asm/io.h>
#include <asm/arch/mmc_host_def.h>
#include <asm/arch/mux.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/mem.h>
#include <asm/mach-types.h>
#include "devkit8000.h"
#include <asm/gpio.h>
#ifdef CONFIG_DRIVER_DM9000
#include <net.h>
#include <netdev.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

static u32 gpmc_net_config[GPMC_MAX_REG] = {
	NET_GPMC_CONFIG1,
	NET_GPMC_CONFIG2,
	NET_GPMC_CONFIG3,
	NET_GPMC_CONFIG4,
	NET_GPMC_CONFIG5,
	NET_GPMC_CONFIG6,
	0
};

/*
 * Routine: board_init
 * Description: Early hardware init.
 */
int board_init(void)
{
	gpmc_init(); /* in SRAM or SDRAM, finish GPMC */
	/* board id for Linux */
	gd->bd->bi_arch_number = MACH_TYPE_DEVKIT8000;
	/* boot param addr */
	gd->bd->bi_boot_params = (OMAP34XX_SDRC_CS0 + 0x100);

	return 0;
}

/* Configure GPMC registers for DM9000 */
static void gpmc_dm9000_config(void)
{
	enable_gpmc_cs_config(gpmc_net_config, &gpmc_cfg->cs[6],
		CONFIG_DM9000_BASE, GPMC_SIZE_16M);
}

/*
 * Routine: misc_init_r
 * Description: Configure board specific parts
 */
int misc_init_r(void)
{
	struct ctrl_id *id_base = (struct ctrl_id *)OMAP34XX_ID_L4_IO_BASE;
#ifdef CONFIG_DRIVER_DM9000
	uchar enetaddr[6];
	u32 die_id_0;
#endif

	twl4030_power_init();
#ifdef CONFIG_TWL4030_LED
	twl4030_led_init(TWL4030_LED_LEDEN_LEDAON | TWL4030_LED_LEDEN_LEDBON);
#endif

#ifdef CONFIG_DRIVER_DM9000
	/* Configure GPMC registers for DM9000 */
	enable_gpmc_cs_config(gpmc_net_config, &gpmc_cfg->cs[6],
			CONFIG_DM9000_BASE, GPMC_SIZE_16M);

	/* Use OMAP DIE_ID as MAC address */
	if (!eth_getenv_enetaddr("ethaddr", enetaddr)) {
		printf("ethaddr not set, using Die ID\n");
		die_id_0 = readl(&id_base->die_id_0);
		enetaddr[0] = 0x02; /* locally administered */
		enetaddr[1] = readl(&id_base->die_id_1) & 0xff;
		enetaddr[2] = (die_id_0 & 0xff000000) >> 24;
		enetaddr[3] = (die_id_0 & 0x00ff0000) >> 16;
		enetaddr[4] = (die_id_0 & 0x0000ff00) >> 8;
		enetaddr[5] = (die_id_0 & 0x000000ff);
		eth_setenv_enetaddr("ethaddr", enetaddr);
	}
#endif

	dieid_num_r();

	return 0;
}

/*
 * Routine: set_muxconf_regs
 * Description: Setting up the configuration Mux registers specific to the
 *		hardware. Many pins need to be moved from protect to primary
 *		mode.
 */
void set_muxconf_regs(void)
{
	MUX_DEVKIT8000();
}

#if defined(CONFIG_GENERIC_MMC) && !defined(CONFIG_SPL_BUILD)
int board_mmc_init(bd_t *bis)
{
	omap_mmc_init(0, 0, 0);
	return 0;
}
#endif

#if defined(CONFIG_DRIVER_DM9000) & !defined(CONFIG_SPL_BUILD)
/*
 * Routine: board_eth_init
 * Description: Setting up the Ethernet hardware.
 */
int board_eth_init(bd_t *bis)
{
	return dm9000_initialize(bis);
}
#endif

#ifdef CONFIG_SPL_OS_BOOT
/*
 * Do board specific preperation before SPL
 * Linux boot
 */
void spl_board_prepare_for_linux(void)
{
	gpmc_dm9000_config();
}

/*
 * devkit8000 specific implementation of spl_start_uboot()
 *
 * RETURN
 * 0 if the button is not pressed
 * 1 if the button is pressed
 */
int spl_start_uboot(void)
{
	int val = 0;
	if (!gpio_request(CONFIG_SPL_OS_BOOT_KEY, "U-Boot key")) {
		gpio_direction_input(CONFIG_SPL_OS_BOOT_KEY);
		val = gpio_get_value(CONFIG_SPL_OS_BOOT_KEY);
		gpio_free(CONFIG_SPL_OS_BOOT_KEY);
	}
	return !val;
}
#endif

/*
 * Routine: get_board_mem_timings
 * Description: If we use SPL then there is no x-loader nor config header
 * so we have to setup the DDR timings ourself on the first bank.  This
 * provides the timing values back to the function that configures
 * the memory.  We have either one or two banks of 128MB DDR.
 */
void get_board_mem_timings(u32 *mcfg, u32 *ctrla, u32 *ctrlb, u32 *rfr_ctrl,
		u32 *mr)
{
	/* General SDRC config */
	*mcfg = MICRON_V_MCFG_165(128 << 20);
	*rfr_ctrl = SDP_3430_SDRC_RFR_CTRL_165MHz;

	/* AC timings */
	*ctrla = MICRON_V_ACTIMA_165;
	*ctrlb = MICRON_V_ACTIMB_165;

	*mr = MICRON_V_MR_165;
}
