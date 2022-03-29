/*
 * Copyright 2004 Freescale Semiconductor.
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

#ifndef __CADMUS_H_
#define __CADMUS_H_


/*
 * CADMUS Board System Register interface.
 */

/*
 * Returns board version register.
 */
extern unsigned int get_board_version(void);

/*
 * Returns either 33000000 or 66000000 as the SYS_CLK_FREQ.
 */
extern unsigned long get_clock_freq(void);


/*
 * Returns 1 - 4, as found in the USER CSR[6:7] bits.
 */
extern unsigned int get_pci_slot(void);


/*
 * Returns PCI DUAL as found in CM_PCI[3].
 */
extern unsigned int get_pci_dual(void);


#endif	/* __CADMUS_H_ */
