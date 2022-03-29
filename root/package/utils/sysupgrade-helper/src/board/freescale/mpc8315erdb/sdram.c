/*
 * Copyright (C) 2007 Freescale Semiconductor, Inc.
 *
 * Authors: Nick.Spence@freescale.com
 *          Wilson.Lo@freescale.com
 *          scottwood@freescale.com
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
 * MERCHANTABILITY or FITNESS for A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <mpc83xx.h>
#include <spd_sdram.h>

#include <asm/bitops.h>
#include <asm/io.h>

#include <asm/processor.h>

DECLARE_GLOBAL_DATA_PTR;

static void resume_from_sleep(void)
{
	u32 magic = *(u32 *)0;

	typedef void (*func_t)(void);
	func_t resume = *(func_t *)4;

	if (magic == 0xf5153ae5)
		resume();

	gd->flags &= ~GD_FLG_SILENT;
	puts("\nResume from sleep failed: bad magic word\n");
}

/* Fixed sdram init -- doesn't use serial presence detect.
 *
 * This is useful for faster booting in configs where the RAM is unlikely
 * to be changed, or for things like NAND booting where space is tight.
 */
#ifndef CONFIG_SYS_RAMBOOT
static long fixed_sdram(void)
{
	volatile immap_t *im = (volatile immap_t *)CONFIG_SYS_IMMR;
	u32 msize = CONFIG_SYS_DDR_SIZE * 1024 * 1024;
	u32 msize_log2 = __ilog2(msize);

	im->sysconf.ddrlaw[0].bar = CONFIG_SYS_DDR_SDRAM_BASE  & 0xfffff000;
	im->sysconf.ddrlaw[0].ar = LBLAWAR_EN | (msize_log2 - 1);
	im->sysconf.ddrcdr = CONFIG_SYS_DDRCDR_VALUE;

	/*
	 * Erratum DDR3 requires a 50ms delay after clearing DDRCDR[DDR_cfg],
	 * or the DDR2 controller may fail to initialize correctly.
	 */
	__udelay(50000);

	im->ddr.csbnds[0].csbnds = (msize - 1) >> 24;
	im->ddr.cs_config[0] = CONFIG_SYS_DDR_CS0_CONFIG;

	/* Currently we use only one CS, so disable the other bank. */
	im->ddr.cs_config[1] = 0;

	im->ddr.sdram_clk_cntl = CONFIG_SYS_DDR_SDRAM_CLK_CNTL;
	im->ddr.timing_cfg_3 = CONFIG_SYS_DDR_TIMING_3;
	im->ddr.timing_cfg_1 = CONFIG_SYS_DDR_TIMING_1;
	im->ddr.timing_cfg_2 = CONFIG_SYS_DDR_TIMING_2;
	im->ddr.timing_cfg_0 = CONFIG_SYS_DDR_TIMING_0;

	if (im->pmc.pmccr1 & PMCCR1_POWER_OFF)
		im->ddr.sdram_cfg = CONFIG_SYS_DDR_SDRAM_CFG | SDRAM_CFG_BI;
	else
		im->ddr.sdram_cfg = CONFIG_SYS_DDR_SDRAM_CFG;

	im->ddr.sdram_cfg2 = CONFIG_SYS_DDR_SDRAM_CFG2;
	im->ddr.sdram_mode = CONFIG_SYS_DDR_MODE;
	im->ddr.sdram_mode2 = CONFIG_SYS_DDR_MODE2;

	im->ddr.sdram_interval = CONFIG_SYS_DDR_INTERVAL;
	sync();

	/* enable DDR controller */
	im->ddr.sdram_cfg |= SDRAM_CFG_MEM_EN;
	sync();

	return msize;
}
#else
static long fixed_sdram(void)
{
	return CONFIG_SYS_DDR_SIZE * 1024 * 1024;
}
#endif /* CONFIG_SYS_RAMBOOT */

phys_size_t initdram(int board_type)
{
	volatile immap_t *im = (volatile immap_t *)CONFIG_SYS_IMMR;
	u32 msize;

	if ((im->sysconf.immrbar & IMMRBAR_BASE_ADDR) != (u32)im)
		return -1;

	/* DDR SDRAM */
	msize = fixed_sdram();

	if (im->pmc.pmccr1 & PMCCR1_POWER_OFF)
		resume_from_sleep();

	/* return total bus SDRAM size(bytes)  -- DDR */
	return msize;
}
