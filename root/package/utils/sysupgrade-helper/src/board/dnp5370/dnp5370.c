/*
 * U-boot - main board file
 *
 * (C) Copyright 2010 3ality Digital Systems
 *
 * Copyright (c) 2005-2008 Analog Devices Inc.
 *
 * (C) Copyright 2000-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <common.h>
#include <config.h>
#include <asm/blackfin.h>
#include <asm/net.h>
#include <net.h>
#include <netdev.h>
#include <asm/gpio.h>

static void disable_external_watchdog(void)
{
#ifdef CONFIG_DNP5370_EXT_WD_DISABLE
	/* disable external HW watchdog with PH13 = WD1 = 1 */
	gpio_request(GPIO_PH13, "ext_wd");
	gpio_direction_output(GPIO_PH13, 1);
#endif
}

int checkboard(void)
{
	printf("Board: SSV DilNet DNP5370\n");
	return 0;
}

#ifdef CONFIG_BFIN_MAC
static void board_init_enetaddr(uchar *mac_addr)
{
#ifdef CONFIG_SYS_NO_FLASH
# define USE_MAC_IN_FLASH 0
#else
# define USE_MAC_IN_FLASH 1
#endif
	bool valid_mac = false;

	if (USE_MAC_IN_FLASH) {
		/* we cram the MAC in the last flash sector */
		uchar *board_mac_addr = (uchar *)0x202F0000;
		if (is_valid_ether_addr(board_mac_addr)) {
			memcpy(mac_addr, board_mac_addr, 6);
			valid_mac = true;
		}
	}

	if (!valid_mac) {
		puts("Warning: Generating 'random' MAC address\n");
		bfin_gen_rand_mac(mac_addr);
	}

	eth_setenv_enetaddr("ethaddr", mac_addr);
}

int board_eth_init(bd_t *bis)
{
	return bfin_EMAC_initialize(bis);
}
#endif

/* miscellaneous platform dependent initialisations */
int misc_init_r(void)
{
	disable_external_watchdog();

#ifdef CONFIG_BFIN_MAC
	uchar enetaddr[6];
	if (!eth_getenv_enetaddr("ethaddr", enetaddr))
		board_init_enetaddr(enetaddr);
#endif

#ifndef CONFIG_SYS_NO_FLASH
	/* we use the last sector for the MAC address / POST LDR */
	extern flash_info_t flash_info[];
	flash_protect(FLAG_PROTECT_SET, 0x202F0000, 0x202FFFFF, &flash_info[0]);
#endif

	return 0;
}
