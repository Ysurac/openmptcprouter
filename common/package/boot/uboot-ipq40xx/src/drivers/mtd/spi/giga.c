/*
 * Copyright (c) 2015 The Linux Foundation. All rights reserved.
 * Copyright (C) 2009 Freescale Semiconductor, Inc.
 *
 * Author: Mingkai Hu (Mingkai.hu@freescale.com)
 * Based on stmicro.c by Wolfgang Denk (wd@denx.de),
 * TsiChung Liew (Tsi-Chung.Liew@freescale.com),
 * and  Jason McMullan (mcmullan@netapp.com)
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
#include <malloc.h>
#include <spi_flash.h>

#include "spi_flash_internal.h"

/* Giga-specific commands */
#define CMD_64K_SE		0xd8	/* 64K Sector Erase */
#define CMD_4K_SE		0x20	/* 4K Sector Erase */

#define CMD_GIGA_EN4B		0xb7	/* Enter 4-byte mode */

#define SIZE_4K			0x1000

struct giga_spi_flash_params {
	u16 id;
	u16 page_size;
	u16 pages_per_sector;
	u16 sectors_per_block;
	u16 nr_blocks;
	const char *name;
};

static const struct giga_spi_flash_params giga_spi_flash_table[] = {
	{
		.id = 0x4015,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 32,
		.name = "GD25Q16",
	},
	{
		.id = 0x4017,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 128,
		.name = "GD25Q64",
	},
	{
		.id = 0x4018,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 256,
		.name = "GD25Q128",
	},
	{
		.id = 0x4019,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 512,
		.name = "GD25Q256",
	},
};

static int giga_erase(struct spi_flash *flash, u32 offset, size_t len)
{
	if ((offset % flash->block_size) == 0 && (len % flash->block_size) == 0)
		return spi_flash_cmd_erase_block(flash, CMD_64K_SE, offset, len);
	else
		return spi_flash_cmd_erase(flash, CMD_4K_SE, offset, len);
}

struct spi_flash *spi_flash_probe_giga(struct spi_slave *spi, u8 *idcode)
{
	const struct giga_spi_flash_params *params;
	struct spi_flash *flash;
	unsigned int i;
	int ret;

	for (i = 0; i < ARRAY_SIZE(giga_spi_flash_table); i++) {
		params = &giga_spi_flash_table[i];
		if (params->id == ((idcode[1] << 8) | idcode[2]))
			break;
	}

	if (i == ARRAY_SIZE(giga_spi_flash_table)) {
		debug("SF: Unsupported Giga ID %02x%02x\n",
				idcode[1], idcode[2]);
		return NULL;
	}

	flash = malloc(sizeof(*flash));
	if (!flash) {
		debug("SF: Failed to allocate memory\n");
		return NULL;
	}

	flash->spi = spi;
	flash->name = params->name;

	flash->write = spi_flash_cmd_write_multi;
	flash->erase = giga_erase;
	flash->read = spi_flash_cmd_read_fast;
	flash->page_size = params->page_size;
	flash->sector_size = params->page_size * params->pages_per_sector;
	flash->block_size = flash->sector_size * params->sectors_per_block;
	flash->size = flash->block_size * params->nr_blocks;

	flash->read_opcode  = CMD_READ_ARRAY_FAST;
	flash->write_opcode = CMD_PAGE_PROGRAM;

	if (flash->size > 0x1000000) {
		/* Switch to 4 byte addressing mode */
		ret = spi_flash_cmd(spi, CMD_GIGA_EN4B, NULL, 0);
		if (ret) {
			debug("SF: Setting 4 byte mode failed, moving to 3 byte mode\n");
			flash->size = 0x1000000;
		}
	}

	return flash;
}
