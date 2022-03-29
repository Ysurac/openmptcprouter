/*
 * Copyright (C) 2004-2006 Atmel Corporation
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
#ifndef __ASM_U_BOOT_H__
#define __ASM_U_BOOT_H__ 1

typedef struct bd_info {
	unsigned long		bi_baudrate;
	unsigned char		bi_phy_id[4];
	unsigned long		bi_board_number;
	void			*bi_boot_params;
	struct {
		unsigned long	start;
		unsigned long	size;
	}			bi_dram[CONFIG_NR_DRAM_BANKS];
	unsigned long		bi_flashstart;
	unsigned long		bi_flashsize;
	unsigned long		bi_flashoffset;
} bd_t;

#define bi_memstart bi_dram[0].start
#define bi_memsize bi_dram[0].size

/* For image.h:image_check_target_arch() */
#define IH_ARCH_DEFAULT IH_ARCH_AVR32

#endif /* __ASM_U_BOOT_H__ */
