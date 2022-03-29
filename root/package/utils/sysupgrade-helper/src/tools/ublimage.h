/*
 * (C) Copyright 2011
 * Heiko Schocher, DENX Software Engineering, hs@denx.de.
 *
 * Vased on:
 * (C) Copyright 2009
 * Stefano Babic, DENX Software Engineering, sbabic@denx.de.
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

#ifndef _UBLIMAGE_H_
#define _UBLIMAGE_H_

enum ublimage_cmd {
	CMD_INVALID,
	CMD_BOOT_MODE,
	CMD_ENTRY,
	CMD_PAGE,
	CMD_ST_BLOCK,
	CMD_ST_PAGE,
	CMD_LD_ADDR
};

enum ublimage_fld_types {
	CFG_INVALID = -1,
	CFG_COMMAND,
	CFG_REG_VALUE
};

/*
 * from sprufg5a.pdf Table 110
 * Used by RBL when doing NAND boot
 */
#define UBL_MAGIC_BASE              (0xA1ACED00)
/* Safe boot mode */
#define UBL_MAGIC_SAFE              (0x00)
/* DMA boot mode */
#define UBL_MAGIC_DMA               (0x11)
/* I Cache boot mode */
#define UBL_MAGIC_IC                (0x22)
/* Fast EMIF boot mode */
#define UBL_MAGIC_FAST              (0x33)
/* DMA + ICache boot mode */
#define UBL_MAGIC_DMA_IC            (0x44)
/* DMA + ICache + Fast EMIF boot mode */
#define UBL_MAGIC_DMA_IC_FAST       (0x55)

/* Define max UBL image size */
#define UBL_IMAGE_SIZE              (0x00003800u)

/* one NAND block */
#define UBL_BLOCK_SIZE 2048

/* from sprufg5a.pdf Table 109 */
struct ubl_header {
	uint32_t	magic;	/* Magic Number, see UBL_* defines */
	uint32_t	entry;	/* entry point address for bootloader */
	uint32_t	pages;	/* number of pages (size of bootloader) */
	uint32_t	block;	/*
				 * blocknumber where user bootloader is
				 * present
				 */
	uint32_t	page;	/*
				 * page number where user bootloader is
				 * present.
				 */
	uint32_t	pll_m;	/*
				 * PLL setting -Multiplier (only valid if
				 * Magic Number indicates PLL enable).
				 */
	uint32_t	pll_n;	/*
				 * PLL setting -Divider (only valid if
				 * Magic Number indicates PLL enable).
				 */
	uint32_t	emif;	/*
				 * fast EMIF setting (only valid if
				 * Magic Number indicates fast EMIF boot).
				 */
	/* to fit in one nand block */
	unsigned char	res[UBL_BLOCK_SIZE - 8 * 4];
};

#endif /* _UBLIMAGE_H_ */
