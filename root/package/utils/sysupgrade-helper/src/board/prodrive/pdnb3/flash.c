/*
 * (C) Copyright 2006
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
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
#include <asm/arch/ixp425.h>

#if !defined(CONFIG_FLASH_CFI_DRIVER)

/*
 * include common flash code (for esd boards)
 */
#include "../common/flash.c"

/*
 * Prototypes
 */
static ulong flash_get_size (vu_long * addr, flash_info_t * info);

static inline ulong ld(ulong x)
{
	ulong k = 0;

	while (x >>= 1)
		++k;

	return k;
}

unsigned long flash_init(void)
{
	unsigned long size;
	int i;

	/* Init: no FLASHes known */
	for (i=0; i<CONFIG_SYS_MAX_FLASH_BANKS; i++)
		flash_info[i].flash_id = FLASH_UNKNOWN;

	size = flash_get_size((vu_long *)FLASH_BASE0_PRELIM, &flash_info[0]);

	if (flash_info[0].flash_id == FLASH_UNKNOWN)
		printf ("## Unknown FLASH on Bank 0 - Size = 0x%08lx = %ld MB\n",
			size, size<<20);

	/* Reconfigure CS0 to actual FLASH size */
	*IXP425_EXP_CS0 = (*IXP425_EXP_CS0 & ~0x00003C00) | ((ld(size) - 9) << 10);

	/* Monitor protection ON by default */
	flash_protect(FLAG_PROTECT_SET,
		      CONFIG_SYS_MONITOR_BASE, CONFIG_SYS_MONITOR_BASE + monitor_flash_len - 1,
		      &flash_info[CONFIG_SYS_MAX_FLASH_BANKS - 1]);

	/* Environment protection ON by default */
	flash_protect(FLAG_PROTECT_SET,
		      CONFIG_ENV_ADDR,
		      CONFIG_ENV_ADDR + CONFIG_ENV_SECT_SIZE - 1,
		      &flash_info[CONFIG_SYS_MAX_FLASH_BANKS - 1]);

	/* Redundant environment protection ON by default */
	flash_protect(FLAG_PROTECT_SET,
		      CONFIG_ENV_ADDR_REDUND,
		      CONFIG_ENV_ADDR_REDUND + CONFIG_ENV_SECT_SIZE - 1,
		      &flash_info[CONFIG_SYS_MAX_FLASH_BANKS - 1]);

	flash_info[0].size = size;

	return size;
}

#endif /* CONFIG_FLASH_CFI_DRIVER */
