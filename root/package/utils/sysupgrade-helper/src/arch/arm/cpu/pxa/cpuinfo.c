/*
 * PXA CPU information display
 *
 * Copyright (C) 2011 Marek Vasut <marek.vasut@gmail.com>
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

#include <common.h>
#include <asm/io.h>
#include <errno.h>
#include <linux/compiler.h>

#define	CPU_MASK_PXA_REVID	0x00f

#define	CPU_MASK_PXA_PRODID	0x3f0
#define	CPU_VALUE_PXA25X	0x100
#define	CPU_VALUE_PXA27X	0x110

static uint32_t pxa_get_cpuid(void)
{
	uint32_t cpuid;
	asm volatile("mrc p15, 0, %0, c0, c0, 0" : "=r"(cpuid));
	return cpuid;
}

int cpu_is_pxa25x(void)
{
	uint32_t id = pxa_get_cpuid();
	id &= CPU_MASK_PXA_PRODID;
	return id == CPU_VALUE_PXA25X;
}

int cpu_is_pxa27x(void)
{
	uint32_t id = pxa_get_cpuid();
	id &= CPU_MASK_PXA_PRODID;
	return id == CPU_VALUE_PXA27X;
}

#ifdef	CONFIG_DISPLAY_CPUINFO
static const char *pxa25x_get_revision(void)
{
	static __maybe_unused const char * const revs_25x[] = { "A0" };
	static __maybe_unused const char * const revs_26x[] = {
								"A0", "B0", "B1"
								};
	static const char *unknown = "Unknown";
	uint32_t id;

	if (!cpu_is_pxa25x())
		return unknown;

	id = pxa_get_cpuid() & CPU_MASK_PXA_REVID;

/* PXA26x is a sick special case as it can't be told apart from PXA25x :-( */
#ifdef	CONFIG_CPU_PXA26X
	switch (id) {
	case 3: return revs_26x[0];
	case 5: return revs_26x[1];
	case 6: return revs_26x[2];
	}
#else
	if (id == 6)
		return revs_25x[0];
#endif
	return unknown;
}

static const char *pxa27x_get_revision(void)
{
	static const char *const rev[] = { "A0", "A1", "B0", "B1", "C0", "C5" };
	static const char *unknown = "Unknown";
	uint32_t id;

	if (!cpu_is_pxa27x())
		return unknown;

	id = pxa_get_cpuid() & CPU_MASK_PXA_REVID;

	if ((id == 5) || (id == 6) || (id > 7))
		return unknown;

	/* Cap the special PXA270 C5 case. */
	if (id == 7)
		id = 5;

	return rev[id];
}

static int print_cpuinfo_pxa2xx(void)
{
	if (cpu_is_pxa25x()) {
		puts("Marvell PXA25x rev. ");
		puts(pxa25x_get_revision());
	} else if (cpu_is_pxa27x()) {
		puts("Marvell PXA27x rev. ");
		puts(pxa27x_get_revision());
	} else
		return -EINVAL;

	puts("\n");

	return 0;
}

int print_cpuinfo(void)
{
	int ret;

	puts("CPU: ");

	ret = print_cpuinfo_pxa2xx();
	if (!ret)
		return ret;

	return ret;
}
#endif
