/*
 * (C) Copyright 2000-2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include <common.h>
#include <asm/io.h>
#include <asm/immap.h>

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_CMD_NAND)
#include <nand.h>
#include <linux/mtd/mtd.h>

#define SET_CLE		0x10
#define SET_ALE		0x08

static void nand_hwcontrol(struct mtd_info *mtdinfo, int cmd, unsigned int ctrl)
{
	struct nand_chip *this = mtdinfo->priv;
	volatile u16 *nCE = (u16 *) CONFIG_SYS_LATCH_ADDR;

	if (ctrl & NAND_CTRL_CHANGE) {
		ulong IO_ADDR_W = (ulong) this->IO_ADDR_W;

		IO_ADDR_W &= ~(SET_ALE | SET_CLE);

		if (ctrl & NAND_NCE)
			*nCE &= 0xFFFB;
		else
			*nCE |= 0x0004;

		if (ctrl & NAND_CLE)
			IO_ADDR_W |= SET_CLE;
		if (ctrl & NAND_ALE)
			IO_ADDR_W |= SET_ALE;

		this->IO_ADDR_W = (void *)IO_ADDR_W;

	}

	if (cmd != NAND_CMD_NONE)
		writeb(cmd, this->IO_ADDR_W);
}

int board_nand_init(struct nand_chip *nand)
{
	volatile gpio_t *gpio = (gpio_t *) MMAP_GPIO;
	volatile fbcs_t *fbcs = (fbcs_t *) MMAP_FBCS;

	fbcs->csmr2 &= ~FBCS_CSMR_WP;

	/*
	 * set up pin configuration - enabled 2nd output buffer's signals
	 * (nand_ngpio - nCE USB1/2_PWR_EN, LATCH_GPIOs, LCD_VEEEN, etc)
	 * to use nCE signal
	 */
	gpio->par_timer &= ~GPIO_PAR_TIN3_TIN3;
	gpio->pddr_timer |= 0x08;
	gpio->ppd_timer |= 0x08;
	gpio->pclrr_timer = 0;
	gpio->podr_timer = 0;

	nand->chip_delay = 60;
	nand->ecc.mode = NAND_ECC_SOFT;
	nand->cmd_ctrl = nand_hwcontrol;

	return 0;
}
#endif
