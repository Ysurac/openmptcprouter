/*
 * (C) Copyright 2007 Semihalf
 *
 * Written by: Rafal Jaworowski <raj@semihalf.com>
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
 *
 *
 * This file contains routines that fetch data from PowerPC-dependent sources
 * (bd_info etc.)
 *
 */

#include <config.h>
#include <linux/types.h>
#include <api_public.h>

#include <asm/u-boot.h>
#include <asm/global_data.h>

#include "api_private.h"

DECLARE_GLOBAL_DATA_PTR;

/*
 * Important notice: handling of individual fields MUST be kept in sync with
 * include/asm-ppc/u-boot.h and include/asm-ppc/global_data.h, so any changes
 * need to reflect their current state and layout of structures involved!
 */
int platform_sys_info(struct sys_info *si)
{
	si->clk_bus = gd->bus_clk;
	si->clk_cpu = gd->cpu_clk;

#if defined(CONFIG_5xx) || defined(CONFIG_8xx) || defined(CONFIG_8260) || \
    defined(CONFIG_E500) || defined(CONFIG_MPC86xx)
#define bi_bar	bi_immr_base
#elif defined(CONFIG_MPC5xxx)
#define bi_bar	bi_mbar_base
#elif defined(CONFIG_MPC83xx)
#define bi_bar	bi_immrbar
#elif defined(CONFIG_MPC8220)
#define bi_bar	bi_mbar_base
#endif

#if defined(bi_bar)
	si->bar = gd->bd->bi_bar;
#undef bi_bar
#else
	si->bar = 0;
#endif

	platform_set_mr(si, gd->bd->bi_memstart, gd->bd->bi_memsize, MR_ATTR_DRAM);
	platform_set_mr(si, gd->bd->bi_flashstart, gd->bd->bi_flashsize, MR_ATTR_FLASH);
	platform_set_mr(si, gd->bd->bi_sramstart, gd->bd->bi_sramsize, MR_ATTR_SRAM);

	return 1;
}
