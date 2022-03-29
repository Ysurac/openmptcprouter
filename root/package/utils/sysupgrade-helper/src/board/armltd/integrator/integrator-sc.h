/*
 * (C) Copyright 2011
 * Linaro
 * Linus Walleij <linus.walleij@linaro.org>
 * Register definitions for the System Controller (SC) and
 * the similar "CP Controller" found in the ARM Integrator/AP and
 * Integrator/CP reference designs
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

#ifndef __ARM_SC_H
#define __ARM_SC_H

#define SC_BASE			0x11000000

/*
 * The system controller registers
 */
#define SC_ID_OFFSET		0x00
#define SC_OSC_OFFSET		0x04
/* Setting this bit switches to 25 MHz mode, clear means 33 MHz */
#define SC_OSC_DIVXY		(1 << 8)
#define SC_CTRLS_OFFSET		0x08
#define SC_CTRLC_OFFSET		0x0C
/* Set bits by writing CTRLS, clear bits by writing CTRLC */
#define SC_CTRL_SOFTRESET	(1 << 0)
#define SC_CTRL_FLASHVPP	(1 << 1)
#define SC_CTRL_FLASHWP		(1 << 2)
#define SC_CTRL_UART1DTR	(1 << 4)
#define SC_CTRL_UART1RTS	(1 << 5)
#define SC_CTRL_UART0DTR	(1 << 6)
#define SC_CTRL_UART0RTS	(1 << 7)
#define SC_DEC_OFFSET		0x10
#define SC_ARB_OFFSET		0x14
#define SC_PCI_OFFSET		0x18
#define SC_PCI_PCIEN		(1 << 0)
#define SC_PCI_PCIBINT_CLR	(1 << 1)
#define SC_LOCK_OFFSET		0x1C
#define SC_LBFADDR_OFFSET	0x20
#define SC_LBFCODE_OFFSET	0x24

#define SC_ID (SC_BASE + SC_ID_OFFSET)
#define SC_OSC (SC_BASE + SC_OSC_OFFSET)
#define SC_CTRLS (SC_BASE + SC_CTRLS_OFFSET)
#define SC_CTRLC (SC_BASE + SC_CTRLC_OFFSET)
#define SC_DEC (SC_BASE + SC_DEC_OFFSET)
#define SC_ARB (SC_BASE + SC_ARB_OFFSET)
#define SC_PCI (SC_BASE + SC_PCI_OFFSET)
#define SC_LOCK (SC_BASE + SC_LOCK_OFFSET)
#define SC_LBFADDR (SC_BASE + SC_LBFADDR_OFFSET)
#define SC_LBFCODE (SC_BASE + SC_LBFCODE_OFFSET)

/*
 * The Integrator/CP as a smaller set of registers, at a different
 * offset - probably not to disturb old software.
 */

#define CP_BASE			0xCB000000

#define CP_IDFIELD_OFFSET	0x00
#define CP_FLASHPROG_OFFSET	0x04
#define CP_FLASHPROG_FLVPPEN	(1 << 0)
#define CP_FLASHPROG_FLWREN	(1 << 1)
#define CP_FLASHPROG_FLASHSIZE	(1 << 2)
#define CP_FLASHPROG_EXTRABANK	(1 << 3)
#define CP_INTREG_OFFSET	0x08
#define CP_DECODE_OFFSET	0x0C

#define CP_IDFIELD (CP_BASE + CP_ID_OFFSET)
#define CP_FLASHPROG (CP_BASE + CP_FLASHPROG_OFFSET)
#define CP_INTREG (CP_BASE + CP_INTREG_OFFSET)
#define CP_DECODE (CP_BASE + CP_DECODE_OFFSET)

#endif
