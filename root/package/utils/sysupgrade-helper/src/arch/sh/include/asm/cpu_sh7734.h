/*
 * (C) Copyright 2008, 2011 Renesas Solutions Corp.
 *
 * SH7734 Internal I/O register
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

#ifndef _ASM_CPU_SH7734_H_
#define _ASM_CPU_SH7734_H_

#define CCR 0xFF00001C

#define CACHE_OC_NUM_WAYS	4
#define CCR_CACHE_INIT	0x0000090d

/* SCIF */
#define SCIF0_BASE  0xFFE40000
#define SCIF1_BASE  0xFFE41000
#define SCIF2_BASE  0xFFE42000
#define SCIF3_BASE  0xFFE43000
#define SCIF4_BASE  0xFFE44000
#define SCIF5_BASE  0xFFE45000

/* Timer */
#define TSTR	0xFFD80004
#define TCNT0	0xFFD8000C
#define TCR0	0xFFD80010

/* PFC */
#define PMMR    (0xFFFC0000)
#define MODESEL0    (0xFFFC004C)
#define MODESEL2    (MODESEL0 + 0x4)
#define MODESEL2_INIT   (0x00003000)

#define IPSR0	(0xFFFC001C)
#define IPSR1	(IPSR0 + 0x4)
#define IPSR2	(IPSR0 + 0x8)
#define IPSR3	(IPSR0 + 0xC)
#define IPSR4	(IPSR0 + 0x10)
#define IPSR5	(IPSR0 + 0x14)
#define IPSR6	(IPSR0 + 0x18)
#define IPSR7	(IPSR0 + 0x1C)
#define IPSR8	(IPSR0 + 0x20)
#define IPSR9	(IPSR0 + 0x24)
#define IPSR10	(IPSR0 + 0x28)
#define IPSR11	(IPSR0 + 0x2C)

#define GPSR0	(0xFFFC0004)
#define GPSR1	(GPSR0 + 0x4)
#define GPSR2	(GPSR0 + 0x8)
#define GPSR3	(GPSR0 + 0xC)
#define GPSR4	(GPSR0 + 0x10)
#define GPSR5	(GPSR0 + 0x14)


#endif /* _ASM_CPU_SH7734_H_ */
