/*
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
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

/* S25FLxx-specific commands */
#define CMD_S25FLXX_SE		0xd8	/* Sector Erase */
#define CMD_S25FLXX_BE		0xc7	/* Bulk Erase */
#define CMD_S25FLXX_4SE		0xdc	/* 4-byte Sector Erase */
#define CMD_S25FSXX_BE		0x60	/* Bulk Erase */

struct spansion_spi_flash_params {
	u16 idcode1;
	u16 idcode2;
	u16 page_size;
	u16 pages_per_sector;
	u16 nr_sectors;
	u16 bulkerase_timeout;	/* in seconds */
	const char *name;
};

static const struct spansion_spi_flash_params spansion_spi_flash_table[] = {
	{
		.idcode1 = 0x0213,
		.idcode2 = 0,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 16,
		.name = "S25FL008A",
	},
	{
		.idcode1 = 0x0214,
		.idcode2 = 0,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 32,
		.name = "S25FL016A",
	},
	{
		.idcode1 = 0x0215,
		.idcode2 = 0,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 64,
		.name = "S25FL032A",
	},
	{
		.idcode1 = 0x0216,
		.idcode2 = 0,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 128,
		.name = "S25FL064A",
	},
	{
		.idcode1 = 0x2018,
		.idcode2 = 0x0301,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 256,
		.name = "S25FL128P_64K",
	},
	{
		.idcode1 = 0x2018,
		.idcode2 = 0x0300,
		.page_size = 256,
		.pages_per_sector = 1024,
		.nr_sectors = 64,
		.name = "S25FL128P_256K",
	},
	{
		.idcode1 = 0x0215,
		.idcode2 = 0x4d00,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 64,
		.name = "S25FL032P",
	},
	{
		.idcode1 = 0x2018,
		.idcode2 = 0x4d01,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 256,
		/*
		 * http://www.spansion.com/Support/Datasheets/S25FS-S_00.pdf
		 * Refer to Table 11.1, tBE entry
		 */
		.bulkerase_timeout = 180,
		.name = "S25FL128S",
	},
	{
		.idcode1 = 0x0219,
		.idcode2 = 0x4d01,
		.page_size = 256,
		.pages_per_sector = 256,
		.nr_sectors = 512,
		/*
		 * http://www.spansion.com/Support/Datasheets/S25FS-S_00.pdf
		 * Refer to Table 11.1, tBE entry
		 */
		.bulkerase_timeout = 360,
		.name = "S25FL256S",
	},
	{
		.idcode1 = 0x0220,
		.idcode2 = 0x4d00,
		.page_size = 256,
		.pages_per_sector = 1024,
		.nr_sectors = 256,
		.name = "S25FL512S",
	},
};

static int spansion_erase(struct spi_flash *flash, u32 offset, size_t len)
{
	u8 erase_opcode;
	if (flash->addr_width == 4)
		erase_opcode = CMD_S25FLXX_4SE;
	else
		erase_opcode = CMD_S25FLXX_SE;

	return spi_flash_cmd_erase(flash, erase_opcode, offset, len);
}

static int spansion_berase(struct spi_flash *flash)
{
	return spi_flash_cmd_berase(flash, CMD_S25FSXX_BE);
}

struct spi_flash *spi_flash_probe_spansion(struct spi_slave *spi, u8 *idcode)
{
	const struct spansion_spi_flash_params *params;
	struct spi_flash *flash;
	unsigned int i;
	unsigned short jedec, ext_jedec;

	jedec = idcode[1] << 8 | idcode[2];
	ext_jedec = idcode[3] << 8 | idcode[4];

	for (i = 0; i < ARRAY_SIZE(spansion_spi_flash_table); i++) {
		params = &spansion_spi_flash_table[i];
		if (params->idcode1 == jedec) {
			if (params->idcode2 == ext_jedec)
				break;
		}
	}

	if (i == ARRAY_SIZE(spansion_spi_flash_table)) {
		debug("SF: Unsupported SPANSION ID %04x %04x\n", jedec, ext_jedec);
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
	flash->erase = spansion_erase;
	if (params->bulkerase_timeout) {
		flash->berase = spansion_berase;
		flash->berase_timeout = params->bulkerase_timeout;
	}
	flash->read = spi_flash_cmd_read_fast;
	flash->page_size = params->page_size;
	flash->sector_size = params->page_size * params->pages_per_sector;
	flash->size = flash->sector_size * params->nr_sectors;

	if (flash->size > 0x1000000) {
		flash->read_opcode  = CMD_4READ_ARRAY_FAST;
		flash->write_opcode = CMD_4PAGE_PROGRAM;
	}

	return flash;
}
