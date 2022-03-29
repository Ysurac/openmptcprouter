/*
 * Copyright (C) 2007,2008 Nobuhiro Iwamatsu <iwamatsu@nigauri.org>
 * Copyright (C) 2008 Renesas Solutions Corp.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _ASM_CPU_SH2_H_
#define _ASM_CPU_SH2_H_

/* cache control */
#define CCR_CACHE_STOP		0x00000008
#define CCR_CACHE_ENABLE	0x00000005
#define CCR_CACHE_ICI		0x00000008

#define CACHE_OC_ADDRESS_ARRAY	0xf0000000
#define CACHE_OC_WAY_SHIFT	13
#define CACHE_OC_NUM_ENTRIES	256
#define CACHE_OC_ENTRY_SHIFT	4

#if defined(CONFIG_CPU_SH7203)
# include <asm/cpu_sh7203.h>
#elif defined(CONFIG_CPU_SH7264)
# include <asm/cpu_sh7264.h>
#elif defined(CONFIG_CPU_SH7269)
# include <asm/cpu_sh7269.h>
#else
# error "Unknown SH2 variant"
#endif

#endif	/* _ASM_CPU_SH2_H_ */
