/*
 * Copyright (c) 2015 The Linux Foundation. All rights reserved.

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

#include <common.h>
#include <malloc.h>
#include <spi_flash.h>
#include <asm/arch-ipq40xx/smem.h>
#include "spi_flash_internal.h"

#define CMD_BLOCK_ERASE		0xd8	/* Block Erase */
#define CMD_BLOCK_4B_ERASE	0xdc	/* 4 byte Block Erase */
#define CMD_SECTOR_4B_ERASE	0x21	/* 4 byte Sector Erase */
#define CMD_SECTOR_ERASE	0x20	/* Sector Erase */
#define CMD_W25_EN4B		0xb7	/* Enter 4-byte mode Winbond*/
#define CMD_GIGA_EN4B		0xb7	/* Enter 4-byte mode giga*/

#define WINBOND_ID	0xEF
#define STMICRO_ID	0x20
#define MACRONIX_ID	0xc2
#define SPANSION_ID	0x01
#define GIGA_ID		0xc8
#define XTX_ID		0x0B
#define SIZE_64K	0x10000

unsigned char manufacturer_id;

static int spi_nor_erase(struct spi_flash *flash, u32 offset, size_t len)
{
	u8 erase_opcode;

	if (flash->sector_size == SIZE_64K) {
		/* Block erase 64K */
		if ((flash->addr_width == 4) && (manufacturer_id != WINBOND_ID))
			erase_opcode = CMD_BLOCK_4B_ERASE;
		else
			erase_opcode = CMD_BLOCK_ERASE;
	} else {
		/* Sector erase 4K*/
		if ((flash->addr_width == 4) && (manufacturer_id != WINBOND_ID))
			erase_opcode = CMD_SECTOR_4B_ERASE;
		else
			erase_opcode = CMD_SECTOR_ERASE;
	}

	return spi_flash_cmd_erase(flash, erase_opcode, offset, len);
}

struct spi_flash *spi_nor_probe_generic(struct spi_slave *spi, u8 *idcode)
{
	struct spi_flash *flash;
	int ret;
	unsigned short jedec;
	qca_smem_flash_info_t sfi;

	jedec = idcode[1] << 8 | idcode[2];

	if ((jedec == 0) || (jedec == 0xffff))
		return NULL;

	manufacturer_id = idcode[0];

	flash = malloc(sizeof(*flash));
	if (!flash) {
		debug("SF: Failed to allocate memory\n");
		return NULL;
	}

	/* Get flash parameters from smem */
	smem_get_boot_flash(&sfi.flash_type,
				&sfi.flash_index,
				&sfi.flash_chip_select,
				&sfi.flash_block_size,
				&sfi.flash_density);

	flash->spi = spi;
	flash->write = spi_flash_cmd_write_multi;
	flash->erase = spi_nor_erase;
	flash->read = spi_flash_cmd_read_fast;
	flash->page_size = 256;
	flash->sector_size = sfi.flash_block_size;
	flash->size = sfi.flash_density;

	if (idcode[0] == WINBOND_ID) {
		flash->name = "winbond";
		flash->read_opcode  = CMD_READ_ARRAY_FAST;
		flash->write_opcode = CMD_PAGE_PROGRAM;
		if (flash->size > 0x1000000) {
			/* Switch to 4 byte addressing mode */
			ret = spi_flash_cmd(spi, CMD_W25_EN4B, NULL, 0);
			if (ret) {
				debug("SF: Setting 4 byte mode failed, moving to 3 byte mode\n");
				flash->size = 0x1000000;
			}
		}
	} else if (idcode[0] == GIGA_ID) {
		flash->name = "Giga";
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
	} else {
		if (idcode[0] == SPANSION_ID)
			flash->name = "Spansion";
		else if (idcode[0] == STMICRO_ID)
			flash->name = "ST micro";
		else if (idcode[0] == MACRONIX_ID)
			flash->name = "Macronix";
		else if (idcode[0] == XTX_ID)
			flash->name = "XTX Technology";
		else
			flash->name = "default";

		if (flash->size > 0x1000000) {
			flash->read_opcode  = CMD_4READ_ARRAY_FAST;
			flash->write_opcode = CMD_4PAGE_PROGRAM;
		}
	}

	return flash;
}
