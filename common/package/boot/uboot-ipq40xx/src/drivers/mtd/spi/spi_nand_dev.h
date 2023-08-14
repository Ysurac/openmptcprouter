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

#ifndef SPI_NAND_DEV_H
#define SPI_NAND_DEV_H
#define MTD_MAX_OOBFREE_ENTRIES_LARGE	32
#define MTD_MAX_ECCPOS_ENTRIES_LARGE	640
#define INT_MAX				((int)(~0U>>1))

/* Flash opcodes. */
#define IPQ40XX_SPINAND_CMD_WREN		0x06    /* Write enable */
#define IPQ40XX_SPINAND_CMD_WRDI		0x04    /* Write disable */
#define IPQ40XX_SPINAND_CMD_GETFEA		0x0F    /* Get feature */
#define IPQ40XX_SPINAND_CMD_SETFEA		0x1F    /* Set feature */
#define IPQ40XX_SPINAND_CMD_READ		0x13    /* read data to cache */
#define IPQ40XX_SPINAND_CMD_NORM_READ		0x03    /* Read data bytes (low frequency) */
#define IPQ40XX_SPINAND_CMD_FAST_READ		0x0B    /* Read data bytes (high frequency) */
#define IPQ40XX_SPINAND_CMD_PLOAD		0x02    /* Program load */
#define IPQ40XX_SPINAND_CMD_PROG		0x10    /* Program execute */
#define IPQ40XX_SPINAND_CMD_ERASE		0xD8    /* Block erase */
#define IPQ40XX_SPINAND_CMD_RDID		0x9F    /* Read JEDEC ID */
#define IPQ40XX_SPINAND_CMD_RESET		0xFF    /* reset nand flash */
#define IPQ40XX_SPINAND_CMD_DIESELECT		0xC2    /* Die Select */

/* Flash Protection register */
#define IPQ40XX_SPINAND_PROTEC_REG		0xA0
#define IPQ40XX_SPINAND_PROTEC_BRWD		0x80
#define IPQ40XX_SPINAND_PROTEC_BP2		0x20
#define IPQ40XX_SPINAND_PROTEC_BP1		0x10
#define IPQ40XX_SPINAND_PROTEC_BP0		0x08
#define IPQ40XX_SPINAND_PROTEC_INV		0x04
#define IPQ40XX_SPINAND_PROTEC_CMP		0x02
#define IPQ40XX_SPINAND_PROTEC_BPx		0xC7

/* Flash feature register */
#define IPQ40XX_SPINAND_FEATURE_REG			0xB0
#define IPQ40XX_SPINAND_FEATURE_OTPPRT			0x80
#define IPQ40XX_SPINAND_FEATURE_OPTEN			0x40
#define IPQ40XX_SPINAND_FEATURE_ECC_EN			0x10
#define IPQ40XX_SPINAND_FEATURE_QE			0x01

/* Flash status register. */
#define IPQ40XX_SPINAND_STATUS_REG			0xC0
#define IPQ40XX_SPINAND_STATUS_BUSY			0x1
#define IPQ40XX_SPINAND_STATUS_WREN			0x2
#define IPQ40XX_SPINAND_STATUS_EFAIL			0x4
#define IPQ40XX_SPINAND_STATUS_PFAIL			0x8
#define IPQ40XX_SPINAND_STATUS_ECCMASK			0x30
#define IPQ40XX_SPINAND_STATUS_ECC(x)			(x << 4)
#define IPQ40XX_SPINAND_STATUS_ECC0			0x0
#define IPQ40XX_SPINAND_STATUS_ECC1			0x1
#define IPQ40XX_SPINAND_STATUS_ECC2			0x2
#define IPQ40XX_SPINAND_STATUS_ECC3			0x3

#define SPINAND_VERC_STATUS_ECCMASK			0x70
#define SPINAND_VERC_STATUS_ERR				0x70

#define STATUS_E_FAIL					0x04
#define STATUS_P_FAIL					0x08

struct spi_nand_flash_params {
	u8 id[4];
	u16 page_size;
	u16 pages_per_sector;
	u16 nr_sectors;
	u32 oob_size;
	u32 erase_size;
	u8 no_of_dies;
	u8 no_of_planes;
	int prev_die_id;
	u8 protec_bpx;
	u64 pages_per_die;
	void (*norm_read_cmd) (u8 *cmd, int column);
	int (*verify_ecc) (int status);
	int (*die_select) (struct mtd_info *mtd,
		struct spi_flash *flash, int die_id);
	const char *name;
};

struct ipq40xx_spinand_info {
	struct nand_hw_control controller;
	struct mtd_info *mtd;
	struct nand_chip *chip;
	struct spi_flash *flash;
	const struct spi_nand_flash_params *params;
	uint8_t *cmd;
	uint8_t chip_ver;
	uint16_t cmd_len;
	uint16_t ob_required;
	uint32_t status;
};

#define ECC_ERR		1
#define ECC_CORRECTED	2

#define SPINAND_4BIT_ECC_MASK		0x3C
#define SPINAND_4BIT_ECC_ERROR		0x20
#define SPINAND_4BIT_ECC_BF_THRESHOLD	0x10

#define SPINAND_3BIT_ECC_MASK		0x70
#define SPINAND_3BIT_ECC_ERROR		0x70
#define SPINAND_3BIT_ECC_BF_THRESHOLD	0x40

#define SPINAND_2BIT_ECC_MASK		0x30
#define SPINAND_2BIT_ECC_ERROR		0x20
#define SPINAND_2BIT_ECC_CORRECTED	0x10

#endif
