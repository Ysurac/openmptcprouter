/*
 * (C) Copyright 2007 Michal Simek
 *
 * Michal  SIMEK <monstr@monstr.eu>
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

/* FSL macros */
#define NGET(val, fslnum) \
	__asm__ __volatile__ ("nget %0, rfsl" #fslnum :"=r" (val));

#define GET(val, fslnum) \
	__asm__ __volatile__ ("get %0, rfsl" #fslnum :"=r" (val));

#define NCGET(val, fslnum) \
	__asm__ __volatile__ ("ncget %0, rfsl" #fslnum :"=r" (val));

#define CGET(val, fslnum) \
	__asm__ __volatile__ ("cget %0, rfsl" #fslnum :"=r" (val));

#define NPUT(val, fslnum) \
	__asm__ __volatile__ ("nput %0, rfsl" #fslnum ::"r" (val));

#define PUT(val, fslnum) \
	__asm__ __volatile__ ("put %0, rfsl" #fslnum ::"r" (val));

#define NCPUT(val, fslnum) \
	__asm__ __volatile__ ("ncput %0, rfsl" #fslnum ::"r" (val));

#define CPUT(val, fslnum) \
	__asm__ __volatile__ ("cput %0, rfsl" #fslnum ::"r" (val));

/* CPU dependent */
/* machine status register */
#define MFS(val, reg) \
	__asm__ __volatile__ ("mfs %0," #reg :"=r" (val));

#define MTS(val, reg) \
	__asm__ __volatile__ ("mts " #reg ", %0"::"r" (val));

/* get return address from interrupt */
#define R14(val) \
	__asm__ __volatile__ ("addi %0, r14, 0":"=r" (val));

#define NOP	__asm__ __volatile__ ("nop");

/* use machine status registe USE_MSR_REG */
#if XILINX_USE_MSR_INSTR == 1
#define MSRSET(val) \
	__asm__ __volatile__ ("msrset r0," #val );

#define MSRCLR(val) \
	__asm__ __volatile__ ("msrclr r0," #val );

#else
#define MSRSET(val)						\
{								\
	register unsigned tmp;					\
	__asm__ __volatile__ ("					\
			mfs	%0, rmsr;			\
			ori	%0, %0, "#val";			\
			mts	rmsr, %0;			\
			nop;"					\
			: "=r" (tmp)				\
			: "d" (val)				\
			: "memory");				\
}

#define MSRCLR(val)						\
{								\
	register unsigned tmp;					\
	__asm__ __volatile__ ("					\
			mfs	%0, rmsr;			\
			andi	%0, %0, ~"#val";		\
			mts	rmsr, %0;			\
			nop;"					\
			: "=r" (tmp)				\
			: "d" (val)				\
			: "memory");				\
}
#endif
