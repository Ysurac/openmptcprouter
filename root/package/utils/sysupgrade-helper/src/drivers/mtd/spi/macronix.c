/*
 * Copyright 2009(C) Marvell International Ltd. and its affiliates
 * Prafulla Wadaskar <prafulla@marvell.com>
 *
 * Based on drivers/mtd/spi/stmicro.c
 *
 * Copyright 2008, Network Appliance Inc.
 * Jason McMullan <mcmullan@netapp.com>
 *
 * Copyright (C) 2004-2007 Freescale Semiconductor, Inc.
 * TsiChung Liew (Tsi-Chung.Liew@freescale.com)
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <common.h>
#include <malloc.h>
#include <spi_flash.h>

#include "spi_flash_internal.h"

/* MX25xx-specific commands */
#define CMD_MX25XX_SE		0x20	/* Sector Erase */
#define CMD_MX25XX_4SE		0x21	/* Sector Erase 4-Byte address */
#define CMD_MX25XX_BE		0xD8	/* Block Erase */
#define CMD_MX25XX_4BE		0xDC	/* Block Erase 4-byte address */
#define CMD_MX25XX_CE		0xc7	/* Chip Erase */

struct macronix_spi_flash_params {
	u16 idcode;
	u16 page_size;
	u16 pages_per_sector;
	u16 sectors_per_block;
	u16 nr_blocks;
	const char *name;
};

static const struct macronix_spi_flash_params macronix_spi_flash_table[] = {
	{
		.idcode = 0x2013,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 8,
		.name = "MX25L4005",
	},
	{
		.idcode = 0x2014,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 16,
		.name = "MX25L8005",
	},
	{
		.idcode = 0x2015,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 32,
		.name = "MX25L1605D",
	},
	{
		.idcode = 0x2016,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 64,
		.name = "MX25L3205D",
	},
	{
		.idcode = 0x2017,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 128,
		.name = "MX25L6405D",
	},
	{
		.idcode = 0x2018,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 256,
		.name = "MX25L12805D",
	},
	{
		.idcode = 0x2618,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 256,
		.name = "MX25L12855E",
	},
	{
		.idcode = 0x2536,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 64,
		.name = "MX25U3235F",
	},
	{
		.idcode = 0x2537,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 128,
		.name = "MX25U6435F",
	},
	{
		.idcode = 0x2538,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 256,
		.name = "MX25U12835F",
	},
	{
		.idcode = 0x2539,
		.page_size = 256,
		.pages_per_sector = 16,
		.sectors_per_block = 16,
		.nr_blocks = 512,
		.name = "MX25U25635F",
	}
};

static int macronix_write_status(struct spi_flash *flash, u8 sr)
{
	u8 cmd;
	int ret;

	ret = spi_flash_cmd_write_enable(flash);
	if (ret < 0) {
		debug("SF: enabling write failed\n");
		return ret;
	}

	cmd = CMD_WRITE_STATUS;
	ret = spi_flash_cmd_write(flash->spi, &cmd, 1, &sr, 1);
	if (ret) {
		debug("SF: fail to write status register\n");
		return ret;
	}

	ret = spi_flash_cmd_wait_ready(flash, SPI_FLASH_PROG_TIMEOUT);
	if (ret < 0) {
		debug("SF: write status register timed out\n");
		return ret;
	}

	return 0;
}

static int macronix_unlock(struct spi_flash *flash)
{
	int ret;

	/* Enable status register writing and clear BP# bits */
	ret = macronix_write_status(flash, 0);
	if (ret)
		debug("SF: fail to disable write protection\n");

	return ret;
}

static int macronix_erase(struct spi_flash *flash, u32 offset, size_t len)
{
	u8 erase_opcode;
	int ret;

	if ((offset % flash->block_size) == 0 && (len % flash->block_size) == 0) {
		/* Block Erase */
		if (flash->addr_width == 4)
			erase_opcode = CMD_MX25XX_4BE;
		else
			erase_opcode = CMD_MX25XX_BE;

		ret = spi_flash_cmd_erase_block(flash, erase_opcode, offset, len);
	} else {
		/* Sector Erase */

		if (flash->addr_width == 4)
			erase_opcode = CMD_MX25XX_4SE;
		else
			erase_opcode = CMD_MX25XX_SE;

		ret = spi_flash_cmd_erase(flash, erase_opcode, offset, len);
	}

	return ret;
}

struct spi_flash *spi_flash_probe_macronix(struct spi_slave *spi, u8 *idcode)
{
	const struct macronix_spi_flash_params *params;
	struct spi_flash *flash;
	unsigned int i;
	u16 id = idcode[2] | idcode[1] << 8;

	for (i = 0; i < ARRAY_SIZE(macronix_spi_flash_table); i++) {
		params = &macronix_spi_flash_table[i];
		if (params->idcode == id)
			break;
	}

	if (i == ARRAY_SIZE(macronix_spi_flash_table)) {
		debug("SF: Unsupported Macronix ID %04x\n", id);
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
	flash->erase = macronix_erase;
	flash->read = spi_flash_cmd_read_fast;
	flash->page_size = params->page_size;
	flash->sector_size = params->page_size * params->pages_per_sector;
	flash->block_size = flash->sector_size * params->sectors_per_block;
	flash->size = flash->block_size * params->nr_blocks;

	if (flash->size > 0x1000000) {
		flash->read_opcode  = CMD_4READ_ARRAY_FAST;
		flash->write_opcode = CMD_4PAGE_PROGRAM;
	}

	/* Clear BP# bits for read-only flash */
	macronix_unlock(flash);

	return flash;
}
