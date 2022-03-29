/*
 * (C) Copyright 2009-2010
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
 *
 */

#include <common.h>
#include <asm/ppc4xx_config.h>

struct ppc4xx_config ppc4xx_config_val[] = {
	{
		"400-133", "CPU: 400 PLB: 133 OPB:  66 EBC:  66",
		{ 0x86, 0x78, 0xc2, 0xc6, 0x05, 0xa5, 0x04, 0xe1 }
	},
	{
		"500-166", "CPU: 500 PLB: 166 OPB:  83 EBC:  83",
		{ 0x87, 0x78, 0xf2, 0xc6, 0x05, 0xa5, 0x04, 0xe1 }
	},
	{
		"533-133", "CPU: 533 PLB: 133 OPB:  66 EBC:  66",
		{ 0x87, 0x79, 0x02, 0x52, 0x05, 0xa5, 0x04, 0xe1 }
	},
	{
		"667-133", "CPU: 667 PLB: 133 OPB:  66 EBC:  66",
		{ 0x87, 0x79, 0x42, 0x56, 0x05, 0xa5, 0x04, 0xe1 }
	},
	{
		"667-166", "CPU: 667 PLB: 166 OPB:  83 EBC:  83",
		{ 0x87, 0x79, 0x42, 0x06, 0x05, 0xa5, 0x04, 0xe1 }
	},
	{
		"800-160", "CPU: 800 PLB: 160 OPB:  53 EBC:  17",
		{ 0x86, 0x79, 0x81, 0xa7, 0x07, 0xa5, 0x04, 0xe1 }
	},
};

int ppc4xx_config_count = ARRAY_SIZE(ppc4xx_config_val);
