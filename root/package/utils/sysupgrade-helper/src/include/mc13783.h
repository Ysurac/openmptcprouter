/*
 * (C) Copyright 2011
 * Helmut Raiger, HALE electronic GmbH, helmut.raiger@hale.at
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


#ifndef __MC13783_H__
#define __MC13783_H__

/* REG_MODE_0 */
#define VAUDIOEN	(1 << 0)
#define VAUDIOSTBY	(1 << 1)
#define VAUDIOMODE	(1 << 2)
#define VIOHIEN		(1 << 3)
#define VIOHISTBY	(1 << 4)
#define VIOHIMODE	(1 << 5)
#define VIOLOEN		(1 << 6)
#define VIOLOSTBY	(1 << 7)
#define VIOLOMODE	(1 << 8)
#define VDIGEN 		(1 << 9)
#define VDIGSTBY	(1 << 10)
#define VDIGMODE	(1 << 11)
#define VGENEN		(1 << 12)
#define VGENSTBY	(1 << 13)
#define VGENMODE	(1 << 14)
#define VRFDIGEN	(1 << 15)
#define VRFDIGSTBY	(1 << 16)
#define VRFDIGMODE	(1 << 17)
#define VRFREFEN	(1 << 18)
#define VRFREFSTBY	(1 << 19)
#define VRFREFMODE	(1 << 20)
#define VRFCPEN		(1 << 21)
#define VRFCPSTBY	(1 << 22)
#define VRFCPMODE	(1 << 23)

/* REG_MODE_1 */
#define VSIMEN		(1 << 0)
#define VSIMSTBY	(1 << 1)
#define VSIMMODE	(1 << 2)
#define VESIMEN		(1 << 3)
#define VESIMSTBY	(1 << 4)
#define VESIMMODE	(1 << 5)
#define VCAMEN		(1 << 6)
#define VCAMSTBY	(1 << 7)
#define VCAMMODE	(1 << 8)
#define VRFBGEN		(1 << 9)
#define VRFBGSTBY	(1 << 10)
#define VVIBEN		(1 << 11)
#define VRF1EN		(1 << 12)
#define VRF1STBY	(1 << 13)
#define VRF1MODE	(1 << 14)
#define VRF2EN		(1 << 15)
#define VRF2STBY	(1 << 16)
#define VRF2MODE	(1 << 17)
#define VMMC1EN		(1 << 18)
#define VMMC1STBY	(1 << 19)
#define VMMC1MODE	(1 << 20)
#define VMMC2EN		(1 << 21)
#define VMMC2STBY	(1 << 22)
#define VMMC2MODE	(1 << 23)

#endif
