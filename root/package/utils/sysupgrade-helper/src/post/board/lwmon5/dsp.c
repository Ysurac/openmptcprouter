/*
 * (C) Copyright 2008 Dmitry Rakhchev, EmCraft Systems, rda@emcraft.com
 *
 * Developed for DENX Software Engineering GmbH
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

#include <post.h>

#if CONFIG_POST & CONFIG_SYS_POST_DSP
#include <asm/io.h>

/* This test verifies DSP status bits in FPGA */

DECLARE_GLOBAL_DATA_PTR;

#define DSP_STATUS_REG		0xC4000008
#define FPGA_STATUS_REG		0xC400000C

int dsp_post_test(int flags)
{
	uint   old_value;
	uint   read_value;
	int    ret;

	/* momorize fpga status */
	old_value = in_be32((void *)FPGA_STATUS_REG);
	/* enable outputs */
	out_be32((void *)FPGA_STATUS_REG, 0x30);

	/* generate sync signal */
	out_be32((void *)DSP_STATUS_REG, 0x300);
	udelay(5);
	out_be32((void *)DSP_STATUS_REG, 0);
	udelay(500);

	/* read status */
	ret = 0;
	read_value = in_be32((void *)DSP_STATUS_REG) & 0x3;
	if (read_value != 0x03) {
		post_log("\nDSP status read %08X\n", read_value);
		ret = 1;
	}

	/* restore fpga status */
	out_be32((void *)FPGA_STATUS_REG, old_value);

	return ret;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_DSP */
