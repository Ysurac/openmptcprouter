/*
 * boot-common.c
 *
 * Common bootmode functions for omap based boards
 *
 * Copyright (C) 2011, Texas Instruments, Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR /PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <asm/omap_common.h>
#include <asm/arch/omap.h>

/*
 * This is used to verify if the configuration header
 * was executed by rom code prior to control of transfer
 * to the bootloader. SPL is responsible for saving and
 * passing the boot_params pointer to the u-boot.
 */
struct omap_boot_parameters boot_params __attribute__ ((section(".data")));

#ifdef CONFIG_SPL_BUILD
/*
 * We use static variables because global data is not ready yet.
 * Initialized data is available in SPL right from the beginning.
 * We would not typically need to save these parameters in regular
 * U-Boot. This is needed only in SPL at the moment.
 */
u32 omap_bootmode = MMCSD_MODE_FAT;

u32 omap_boot_device(void)
{
	return (u32) (boot_params.omap_bootdevice);
}

u32 omap_boot_mode(void)
{
	return omap_bootmode;
}
#endif
