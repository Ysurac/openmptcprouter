/*
 * U-boot - muldi3.c contains routines for mult and div
 *
 * Copyright (c) 2005-2007 Analog Devices Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

/* Generic function got from GNU gcc package, libgcc2.c */
#ifndef SI_TYPE_SIZE
#define SI_TYPE_SIZE 32
#endif
#define __ll_B (1L << (SI_TYPE_SIZE / 2))
#define __ll_lowpart(t) ((USItype) (t) % __ll_B)
#define __ll_highpart(t) ((USItype) (t) / __ll_B)
#define BITS_PER_UNIT 8

#if !defined (umul_ppmm)
#define umul_ppmm(w1, w0, u, v)						\
do {									\
	USItype __x0, __x1, __x2, __x3;					\
	USItype __ul, __vl, __uh, __vh;					\
									\
	__ul = __ll_lowpart (u);					\
	__uh = __ll_highpart (u);					\
	__vl = __ll_lowpart (v);					\
	__vh = __ll_highpart (v);					\
									\
	__x0 = (USItype) __ul * __vl;					\
	__x1 = (USItype) __ul * __vh;					\
	__x2 = (USItype) __uh * __vl;					\
	__x3 = (USItype) __uh * __vh;					\
									\
	__x1 += __ll_highpart (__x0);/* this can't give carry */	\
	__x1 += __x2;	/* but this indeed can */			\
	if (__x1 < __x2)	/* did we get it? */			\
		__x3 += __ll_B;	/* yes, add it in the proper pos. */	\
									\
	(w1) = __x3 + __ll_highpart (__x1);				\
	(w0) = __ll_lowpart (__x1) * __ll_B + __ll_lowpart (__x0);	\
} while (0)
#endif

#if !defined (__umulsidi3)
#define __umulsidi3(u, v)						\
	({DIunion __w;							\
	umul_ppmm (__w.s.high, __w.s.low, u, v);			\
	__w.ll; })
#endif

typedef unsigned int USItype __attribute__ ((mode(SI)));
typedef int SItype __attribute__ ((mode(SI)));
typedef int DItype __attribute__ ((mode(DI)));
typedef int word_type __attribute__ ((mode(__word__)));

struct DIstruct {
	SItype low, high;
};
typedef union {
	struct DIstruct s;
	DItype ll;
} DIunion;

DItype __muldi3(DItype u, DItype v)
{
	DIunion w;
	DIunion uu, vv;

	uu.ll = u, vv.ll = v;
	/*  panic("kernel panic for __muldi3"); */
	w.ll = __umulsidi3(uu.s.low, vv.s.low);
	w.s.high += ((USItype) uu.s.low * (USItype) vv.s.high
		     + (USItype) uu.s.high * (USItype) vv.s.low);

	return w.ll;
}
