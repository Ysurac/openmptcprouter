/*
 * (C) Copyright 2010
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
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
#include <asm/io.h>

/*
 * Only override CONFIG_SYS_BOOTCOUNT_ADDR if not already defined. This
 * way, some boards can define it directly in their config header.
 */
#if !defined(CONFIG_SYS_BOOTCOUNT_ADDR)

#if defined(CONFIG_MPC5xxx)
#define CONFIG_SYS_BOOTCOUNT_ADDR	(MPC5XXX_CDM_BRDCRMB)
#define CONFIG_SYS_BOOTCOUNT_SINGLEWORD
#endif /* defined(CONFIG_MPC5xxx) */

#if defined(CONFIG_MPC512X)
#define CONFIG_SYS_BOOTCOUNT_ADDR	(&((immap_t *)CONFIG_SYS_IMMR)->clk.bcr)
#define CONFIG_SYS_BOOTCOUNT_SINGLEWORD
#endif /* defined(CONFIG_MPC512X) */

#if defined(CONFIG_8xx)
#define CONFIG_SYS_BOOTCOUNT_ADDR (((immap_t *)CONFIG_SYS_IMMR)->im_cpm.cp_dpmem + \
				CPM_BOOTCOUNT_ADDR)
#endif /* defined(CONFIG_8xx) */

#if defined(CONFIG_MPC8260)
#include <asm/cpm_8260.h>

#define CONFIG_SYS_BOOTCOUNT_ADDR	(CONFIG_SYS_IMMR + CPM_BOOTCOUNT_ADDR)
#endif /* defined(CONFIG_MPC8260) */

#if defined(CONFIG_QE)
#include <asm/immap_qe.h>

#define CONFIG_SYS_BOOTCOUNT_ADDR	(CONFIG_SYS_IMMR + 0x110000 + \
					 QE_MURAM_SIZE - 2 * sizeof(u32))
#endif /* defined(CONFIG_MPC8360) */

#if defined(CONFIG_4xx)
#define CONFIG_SYS_BOOTCOUNT_ADDR	(CONFIG_SYS_OCM_DATA_ADDR + \
				CONFIG_SYS_BOOTCOUNT_ADDR)
#endif /* defined(CONFIG_4xx) */

#endif /* !defined(CONFIG_SYS_BOOTCOUNT_ADDR) */

void bootcount_store(ulong a)
{
	void *reg = (void *)CONFIG_SYS_BOOTCOUNT_ADDR;

#if defined(CONFIG_SYS_BOOTCOUNT_SINGLEWORD)
	out_be32(reg, (BOOTCOUNT_MAGIC & 0xffff0000) | a);
#else
	out_be32(reg, a);
	out_be32(reg + 4, BOOTCOUNT_MAGIC);
#endif
}

ulong bootcount_load(void)
{
	void *reg = (void *)CONFIG_SYS_BOOTCOUNT_ADDR;

#if defined(CONFIG_SYS_BOOTCOUNT_SINGLEWORD)
	u32 tmp = in_be32(reg);

	if ((tmp & 0xffff0000) != (BOOTCOUNT_MAGIC & 0xffff0000))
		return 0;
	else
		return (tmp & 0x0000ffff);
#else
	if (in_be32(reg + 4) != BOOTCOUNT_MAGIC)
		return 0;
	else
		return in_be32(reg);
#endif
}
