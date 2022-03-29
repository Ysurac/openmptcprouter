/*
 * Copyright (C) 2007
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
/*
 * Test for correctness of composite floating-point comparisons.
 * Written by Paolo Bonzini, 26th May 2004.
 * This file is originally a part of the GCC testsuite.
 */

#include <common.h>

#include <post.h>

GNU_FPOST_ATTR

#if CONFIG_POST & CONFIG_SYS_POST_FPU

static int failed;

#define TEST(c) if ((c) != ok) failed++
#define ORD(a, b) (!__builtin_isunordered ((a), (b)))
#define UNORD(a, b) (__builtin_isunordered ((a), (b)))
#define UNEQ(a, b) (__builtin_isunordered ((a), (b)) || ((a) == (b)))
#define UNLT(a, b) (__builtin_isunordered ((a), (b)) || ((a) < (b)))
#define UNLE(a, b) (__builtin_isunordered ((a), (b)) || ((a) <= (b)))
#define UNGT(a, b) (__builtin_isunordered ((a), (b)) || ((a) > (b)))
#define UNGE(a, b) (__builtin_isunordered ((a), (b)) || ((a) >= (b)))
#define LTGT(a, b) (__builtin_islessgreater ((a), (b)))

static float pinf;
static float ninf;
static float NaN;

static void iuneq (float x, float y, int ok)
{
	TEST (UNEQ (x, y));
	TEST (!LTGT (x, y));
	TEST (UNLE (x, y) && UNGE (x,y));
}

static void ieq (float x, float y, int ok)
{
	TEST (ORD (x, y) && UNEQ (x, y));
}

static void iltgt (float x, float y, int ok)
{
	TEST (!UNEQ (x, y)); /* Not optimizable. */
	TEST (LTGT (x, y)); /* Same, __builtin_islessgreater does not trap. */
	TEST (ORD (x, y) && (UNLT (x, y) || UNGT (x,y)));
}

static void ine (float x, float y, int ok)
{
	TEST (UNLT (x, y) || UNGT (x, y));
}

static void iunlt (float x, float y, int ok)
{
	TEST (UNLT (x, y));
	TEST (UNORD (x, y) || (x < y));
}

static void ilt (float x, float y, int ok)
{
	TEST (ORD (x, y) && UNLT (x, y)); /* Not optimized */
	TEST ((x <= y) && (x != y));
	TEST ((x <= y) && (y != x));
	TEST ((x != y) && (x <= y)); /* Not optimized */
	TEST ((y != x) && (x <= y)); /* Not optimized */
}

static void iunle (float x, float y, int ok)
{
	TEST (UNLE (x, y));
	TEST (UNORD (x, y) || (x <= y));
}

static void ile (float x, float y, int ok)
{
	TEST (ORD (x, y) && UNLE (x, y)); /* Not optimized */
	TEST ((x < y) || (x == y));
	TEST ((y > x) || (x == y));
	TEST ((x == y) || (x < y)); /* Not optimized */
	TEST ((y == x) || (x < y)); /* Not optimized */
}

static void iungt (float x, float y, int ok)
{
	TEST (UNGT (x, y));
	TEST (UNORD (x, y) || (x > y));
}

static void igt (float x, float y, int ok)
{
	TEST (ORD (x, y) && UNGT (x, y)); /* Not optimized */
	TEST ((x >= y) && (x != y));
	TEST ((x >= y) && (y != x));
	TEST ((x != y) && (x >= y)); /* Not optimized */
	TEST ((y != x) && (x >= y)); /* Not optimized */
}

static void iunge (float x, float y, int ok)
{
	TEST (UNGE (x, y));
	TEST (UNORD (x, y) || (x >= y));
}

static void ige (float x, float y, int ok)
{
	TEST (ORD (x, y) && UNGE (x, y)); /* Not optimized */
	TEST ((x > y) || (x == y));
	TEST ((y < x) || (x == y));
	TEST ((x == y) || (x > y)); /* Not optimized */
	TEST ((y == x) || (x > y)); /* Not optimized */
}

int fpu_post_test_math6 (void)
{
	pinf = __builtin_inf ();
	ninf = -__builtin_inf ();
	NaN = __builtin_nan ("");

	iuneq (ninf, pinf, 0);
	iuneq (NaN, NaN, 1);
	iuneq (pinf, ninf, 0);
	iuneq (1, 4, 0);
	iuneq (3, 3, 1);
	iuneq (5, 2, 0);

	ieq (1, 4, 0);
	ieq (3, 3, 1);
	ieq (5, 2, 0);

	iltgt (ninf, pinf, 1);
	iltgt (NaN, NaN, 0);
	iltgt (pinf, ninf, 1);
	iltgt (1, 4, 1);
	iltgt (3, 3, 0);
	iltgt (5, 2, 1);

	ine (1, 4, 1);
	ine (3, 3, 0);
	ine (5, 2, 1);

	iunlt (NaN, ninf, 1);
	iunlt (pinf, NaN, 1);
	iunlt (pinf, ninf, 0);
	iunlt (pinf, pinf, 0);
	iunlt (ninf, ninf, 0);
	iunlt (1, 4, 1);
	iunlt (3, 3, 0);
	iunlt (5, 2, 0);

	ilt (1, 4, 1);
	ilt (3, 3, 0);
	ilt (5, 2, 0);

	iunle (NaN, ninf, 1);
	iunle (pinf, NaN, 1);
	iunle (pinf, ninf, 0);
	iunle (pinf, pinf, 1);
	iunle (ninf, ninf, 1);
	iunle (1, 4, 1);
	iunle (3, 3, 1);
	iunle (5, 2, 0);

	ile (1, 4, 1);
	ile (3, 3, 1);
	ile (5, 2, 0);

	iungt (NaN, ninf, 1);
	iungt (pinf, NaN, 1);
	iungt (pinf, ninf, 1);
	iungt (pinf, pinf, 0);
	iungt (ninf, ninf, 0);
	iungt (1, 4, 0);
	iungt (3, 3, 0);
	iungt (5, 2, 1);

	igt (1, 4, 0);
	igt (3, 3, 0);
	igt (5, 2, 1);

	iunge (NaN, ninf, 1);
	iunge (pinf, NaN, 1);
	iunge (ninf, pinf, 0);
	iunge (pinf, pinf, 1);
	iunge (ninf, ninf, 1);
	iunge (1, 4, 0);
	iunge (3, 3, 1);
	iunge (5, 2, 1);

	ige (1, 4, 0);
	ige (3, 3, 1);
	ige (5, 2, 1);

	if (failed) {
		post_log ("Error in FPU math6 test\n");
		return -1;
	}
	return 0;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_FPU */
