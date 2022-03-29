/*
 * (C) Copyright 2001
 * Stefan Roese, esd gmbh germany, stefan.roese@esd-electronics.com
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
#include <asm/ppc4xx.h>
#include <asm/processor.h>

/*
 * include common flash code (for esd boards)
 */
#include "../common/flash.c"

/*-----------------------------------------------------------------------
 * Functions
 */
static ulong flash_get_size (vu_long * addr, flash_info_t * info);
static void flash_get_offsets (ulong base, flash_info_t * info);

/*-----------------------------------------------------------------------
 */

unsigned long flash_init (void)
{
	unsigned long size_b0, size_b1;
	int i;
	uint pbcr;
	unsigned long base_b0, base_b1;
	int size_val = 0;

	/* Init: no FLASHes known */
	for (i = 0; i < CONFIG_SYS_MAX_FLASH_BANKS; ++i) {
		flash_info[i].flash_id = FLASH_UNKNOWN;
	}

	/* Static FLASH Bank configuration here - FIXME XXX */

	base_b0 = FLASH_BASE0_PRELIM;
	size_b0 = flash_get_size ((vu_long *) base_b0, &flash_info[0]);

	if (flash_info[0].flash_id == FLASH_UNKNOWN) {
		printf ("## Unknown FLASH on Bank 0 - Size = 0x%08lx = %ld MB\n",
			size_b0, size_b0 << 20);
	}

	base_b1 = FLASH_BASE1_PRELIM;
	size_b1 = flash_get_size ((vu_long *) base_b1, &flash_info[1]);

	/* Re-do sizing to get full correct info */

	if (size_b1) {
		mtdcr (EBC0_CFGADDR, PB0CR);
		pbcr = mfdcr (EBC0_CFGDATA);
		mtdcr (EBC0_CFGADDR, PB0CR);
		base_b1 = -size_b1;
		switch (size_b1) {
		case 1 << 20:
			size_val = 0;
			break;
		case 2 << 20:
			size_val = 1;
			break;
		case 4 << 20:
			size_val = 2;
			break;
		case 8 << 20:
			size_val = 3;
			break;
		case 16 << 20:
			size_val = 4;
			break;
		}
		pbcr = (pbcr & 0x0001ffff) | base_b1 | (size_val << 17);
		mtdcr (EBC0_CFGDATA, pbcr);
		/*          printf("PB1CR = %x\n", pbcr); */
	}

	if (size_b0) {
		mtdcr (EBC0_CFGADDR, PB1CR);
		pbcr = mfdcr (EBC0_CFGDATA);
		mtdcr (EBC0_CFGADDR, PB1CR);
		base_b0 = base_b1 - size_b0;
		switch (size_b1) {
		case 1 << 20:
			size_val = 0;
			break;
		case 2 << 20:
			size_val = 1;
			break;
		case 4 << 20:
			size_val = 2;
			break;
		case 8 << 20:
			size_val = 3;
			break;
		case 16 << 20:
			size_val = 4;
			break;
		}
		pbcr = (pbcr & 0x0001ffff) | base_b0 | (size_val << 17);
		mtdcr (EBC0_CFGDATA, pbcr);
		/*            printf("PB0CR = %x\n", pbcr); */
	}

	size_b0 = flash_get_size ((vu_long *) base_b0, &flash_info[0]);

	flash_get_offsets (base_b0, &flash_info[0]);

	/* monitor protection ON by default */
	flash_protect (FLAG_PROTECT_SET,
			base_b0 + size_b0 - monitor_flash_len,
			base_b0 + size_b0 - 1, &flash_info[0]);

	if (size_b1) {
		/* Re-do sizing to get full correct info */
		size_b1 = flash_get_size ((vu_long *) base_b1, &flash_info[1]);

		flash_get_offsets (base_b1, &flash_info[1]);

		/* monitor protection ON by default */
		flash_protect (FLAG_PROTECT_SET,
				base_b1 + size_b1 - monitor_flash_len,
				base_b1 + size_b1 - 1, &flash_info[1]);
		/* monitor protection OFF by default (one is enough) */
		flash_protect (FLAG_PROTECT_CLEAR,
				base_b0 + size_b0 - monitor_flash_len,
				base_b0 + size_b0 - 1, &flash_info[0]);
	} else {
		flash_info[1].flash_id = FLASH_UNKNOWN;
		flash_info[1].sector_count = -1;
	}

	flash_info[0].size = size_b0;
	flash_info[1].size = size_b1;

	return (size_b0 + size_b1);
}
