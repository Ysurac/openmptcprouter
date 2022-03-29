/*
 * Copyright (C) 2012 Andes Technology Corporation
 * Shawn Lin, Andes Technology Corporation <nobuhiro@andestech.com>
 * Macpaul Lin, Andes Technology Corporation <macpaul@andestech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307	 USA
 *
 */

#include <common.h>

static inline unsigned long CACHE_LINE_SIZE(enum cache_t cache)
{
	if (cache == ICACHE)
		return 8 << (((GET_ICM_CFG() & ICM_CFG_MSK_ISZ) \
					>> ICM_CFG_OFF_ISZ) - 1);
	else
		return 8 << (((GET_DCM_CFG() & DCM_CFG_MSK_DSZ) \
					>> DCM_CFG_OFF_DSZ) - 1);
}

void flush_dcache_range(unsigned long start, unsigned long end)
{
	unsigned long line_size;

	line_size = CACHE_LINE_SIZE(DCACHE);

	while (end > start) {
		asm volatile (
			"\n\tcctl %0, L1D_VA_WB"
			"\n\tcctl %0, L1D_VA_INVAL"
			:
			: "r" (start)
		);
		start += line_size;
	}
}

void invalidate_icache_range(unsigned long start, unsigned long end)
{
	unsigned long line_size;

	line_size = CACHE_LINE_SIZE(ICACHE);
	while (end > start) {
		asm volatile (
			"\n\tcctl %0, L1I_VA_INVAL"
			:
			: "r"(start)
		);
		start += line_size;
	}
}

void invalidate_dcache_range(unsigned long start, unsigned long end)
{
	unsigned long line_size;

	line_size = CACHE_LINE_SIZE(DCACHE);
	while (end > start) {
		asm volatile (
			"\n\tcctl %0, L1D_VA_INVAL"
			:
			: "r"(start)
		);
		start += line_size;
	}
}

void flush_cache(unsigned long addr, unsigned long size)
{
	flush_dcache_range(addr, addr + size);
	invalidate_icache_range(addr, addr + size);
}

void icache_enable(void)
{
	asm volatile (
		"mfsr	$p0, $mr8\n\t"
		"ori	$p0, $p0, 0x01\n\t"
		"mtsr	$p0, $mr8\n\t"
		"isb\n\t"
	);
}

void icache_disable(void)
{
	asm volatile (
		"mfsr	$p0, $mr8\n\t"
		"li	$p1, ~0x01\n\t"
		"and	$p0, $p0, $p1\n\t"
		"mtsr	$p0, $mr8\n\t"
		"isb\n\t"
	);
}

int icache_status(void)
{
	int ret;

	asm volatile (
		"mfsr	$p0, $mr8\n\t"
		"andi	%0,  $p0, 0x01\n\t"
		: "=r" (ret)
		:
		: "memory"
	);

	return ret;
}

void dcache_enable(void)
{
	asm volatile (
		"mfsr	$p0, $mr8\n\t"
		"ori	$p0, $p0, 0x02\n\t"
		"mtsr	$p0, $mr8\n\t"
		"isb\n\t"
	);
}

void dcache_disable(void)
{
	asm volatile (
		"mfsr	$p0, $mr8\n\t"
		"li	$p1, ~0x02\n\t"
		"and	$p0, $p0, $p1\n\t"
		"mtsr	$p0, $mr8\n\t"
		"isb\n\t"
	);
}

int dcache_status(void)
{
	int ret;

	asm volatile (
		"mfsr	$p0, $mr8\n\t"
		"andi	%0, $p0, 0x02\n\t"
		: "=r" (ret)
		:
		: "memory"
	);

	return ret;
}
