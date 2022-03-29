/*
 * (C) Copyright 2002
 * Daniel Engström, Omicron Ceti AB, daniel@omicron.se.
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

/* i8259.h i8259 PIC Registers */

#ifndef _ASMI386_I8259_H_
#define _ASMI386_I8959_H_       1


/* PIC I/O mapped registers */

#define IRR		0x0	/* Interrupt Request Register */
#define ISR		0x0	/* In-Service Register */
#define ICW1		0x0	/* Initialization Control Word 1 */
#define OCW2		0x0	/* Operation Control Word 2 */
#define OCW3		0x0	/* Operation Control Word 3 */
#define ICW2		0x1	/* Initialization Control Word 2 */
#define ICW3		0x1	/* Initialization Control Word 3 */
#define ICW4		0x1	/* Initialization Control Word 4 */
#define IMR		0x1	/* Interrupt Mask Register */

/* bits for IRR, IMR, ISR and ICW3 */
#define	IR7		0x80	/* IR7 */
#define	IR6		0x40	/* IR6 */
#define	IR5		0x20	/* IR5 */
#define	IR4		0x10	/* IR4 */
#define	IR3		0x08	/* IR3 */
#define	IR2		0x04	/* IR2 */
#define	IR1		0x02	/* IR1 */
#define	IR0		0x01	/* IR0 */

/* bits for SEOI */
#define	SEOI_IR7	0x07	/* IR7 */
#define	SEOI_IR6	0x06	/* IR6 */
#define	SEOI_IR5	0x05	/* IR5 */
#define	SEOI_IR4	0x04	/* IR4 */
#define	SEOI_IR3	0x03	/* IR3 */
#define	SEOI_IR2	0x02	/* IR2 */
#define	SEOI_IR1	0x01	/* IR1 */
#define	SEOI_IR0	0x00	/* IR0 */

/* OCW2 bits */
#define OCW2_RCLR	0x00	/* Rotate/clear */
#define OCW2_NEOI	0x20	/* Non specific EOI */
#define OCW2_NOP	0x40	/* NOP */
#define OCW2_SEOI	0x60	/* Specific EOI */
#define OCW2_RSET	0x80	/* Rotate/set */
#define OCW2_REOI	0xA0	/* Rotate on non specific EOI */
#define OCW2_PSET	0xC0	/* Priority Set Command */
#define OCW2_RSEOI	0xE0	/* Rotate on specific EOI */

/* ICW1 bits */
#define ICW1_SEL	0x10	/* Select ICW1 */
#define ICW1_LTIM	0x08	/* Level-Triggered Interrupt Mode */
#define ICW1_ADI	0x04	/* Address Interval */
#define ICW1_SNGL	0x02	/* Single PIC */
#define ICW1_EICW4	0x01	/* Expect initilization ICW4 */

/* ICW2 is the starting vector number */

/* ICW2 is bit-mask of present slaves for a master device,
 * or the slave ID for a slave device */

/* ICW4 bits */
#define	ICW4_AEOI	0x02	/* Automatic EOI Mode */
#define ICW4_PM		0x01	/* Microprocessor Mode */

#endif
