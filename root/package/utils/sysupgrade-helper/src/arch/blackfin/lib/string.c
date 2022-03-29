/*
 * U-boot - string.c Contains library routines.
 *
 * Copyright (c) 2005-2008 Analog Devices Inc.
 *
 * (C) Copyright 2000-2004
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <common.h>
#include <config.h>
#include <asm/blackfin.h>
#include <asm/io.h>
#include <asm/mach-common/bits/dma.h>

char *strcpy(char *dest, const char *src)
{
	char *xdest = dest;
	char temp = 0;

	__asm__ __volatile__ (
		"1:\t%2 = B [%1++] (Z);\n\t"
		"B [%0++] = %2;\n\t"
		"CC = %2;\n\t"
		"if cc jump 1b (bp);\n"
		: "=a"(dest), "=a"(src), "=d"(temp)
		: "0"(dest), "1"(src), "2"(temp)
		: "memory");

	return xdest;
}

char *strncpy(char *dest, const char *src, size_t n)
{
	char *xdest = dest;
	char temp = 0;

	if (n == 0)
		return xdest;

	__asm__ __volatile__ (
		"1:\t%3 = B [%1++] (Z);\n\t"
		"B [%0++] = %3;\n\t"
		"CC = %3;\n\t"
		"if ! cc jump 2f;\n\t"
		"%2 += -1;\n\t"
		"CC = %2 == 0;\n\t"
		"if ! cc jump 1b (bp);\n"
		"2:\n"
		: "=a"(dest), "=a"(src), "=da"(n), "=d"(temp)
		: "0"(dest), "1"(src), "2"(n), "3"(temp)
		: "memory");

	return xdest;
}

int strcmp(const char *cs, const char *ct)
{
	char __res1, __res2;

	__asm__ (
		"1:\t%2 = B[%0++] (Z);\n\t"	/* get *cs */
		"%3 = B[%1++] (Z);\n\t"	/* get *ct */
		"CC = %2 == %3;\n\t"	/* compare a byte */
		"if ! cc jump 2f;\n\t"	/* not equal, break out */
		"CC = %2;\n\t"	/* at end of cs? */
		"if cc jump 1b (bp);\n\t"	/* no, keep going */
		"jump.s 3f;\n"	/* strings are equal */
		"2:\t%2 = %2 - %3;\n"	/* *cs - *ct */
		"3:\n"
		: "=a"(cs), "=a"(ct), "=d"(__res1), "=d"(__res2)
		: "0"(cs), "1"(ct));

	return __res1;
}

int strncmp(const char *cs, const char *ct, size_t count)
{
	char __res1, __res2;

	if (!count)
		return 0;

	__asm__(
		"1:\t%3 = B[%0++] (Z);\n\t"	/* get *cs */
		"%4 = B[%1++] (Z);\n\t"	/* get *ct */
		"CC = %3 == %4;\n\t"	/* compare a byte */
		"if ! cc jump 3f;\n\t"	/* not equal, break out */
		"CC = %3;\n\t"	/* at end of cs? */
		"if ! cc jump 4f;\n\t"	/* yes, all done */
		"%2 += -1;\n\t"	/* no, adjust count */
		"CC = %2 == 0;\n\t" "if ! cc jump 1b;\n"	/* more to do, keep going */
		"2:\t%3 = 0;\n\t"	/* strings are equal */
		"jump.s    4f;\n" "3:\t%3 = %3 - %4;\n"	/* *cs - *ct */
		"4:"
		: "=a"(cs), "=a"(ct), "=da"(count), "=d"(__res1), "=d"(__res2)
		: "0"(cs), "1"(ct), "2"(count));

	return __res1;
}

#ifdef bfin_write_MDMA1_D0_IRQ_STATUS
# define bfin_write_MDMA_D0_IRQ_STATUS bfin_write_MDMA1_D0_IRQ_STATUS
# define bfin_write_MDMA_D0_START_ADDR bfin_write_MDMA1_D0_START_ADDR
# define bfin_write_MDMA_D0_X_COUNT    bfin_write_MDMA1_D0_X_COUNT
# define bfin_write_MDMA_D0_X_MODIFY   bfin_write_MDMA1_D0_X_MODIFY
# define bfin_write_MDMA_D0_CONFIG     bfin_write_MDMA1_D0_CONFIG
# define bfin_write_MDMA_S0_START_ADDR bfin_write_MDMA1_S0_START_ADDR
# define bfin_write_MDMA_S0_X_COUNT    bfin_write_MDMA1_S0_X_COUNT
# define bfin_write_MDMA_S0_X_MODIFY   bfin_write_MDMA1_S0_X_MODIFY
# define bfin_write_MDMA_S0_CONFIG     bfin_write_MDMA1_S0_CONFIG
# define bfin_write_MDMA_D0_IRQ_STATUS bfin_write_MDMA1_D0_IRQ_STATUS
# define bfin_read_MDMA_D0_IRQ_STATUS  bfin_read_MDMA1_D0_IRQ_STATUS
#endif
/* This version misbehaves for count values of 0 and 2^16+.
 * Perhaps we should detect that ?  Nowhere do we actually
 * use dma memcpy for those types of lengths though ...
 */
void dma_memcpy_nocache(void *dst, const void *src, size_t count)
{
	uint16_t wdsize, mod;

	/* Disable DMA in case it's still running (older u-boot's did not
	 * always turn them off).  Do it before the if statement below so
	 * we can be cheap and not do a SSYNC() due to the forced abort.
	 */
	bfin_write_MDMA_D0_CONFIG(0);
	bfin_write_MDMA_S0_CONFIG(0);
	bfin_write_MDMA_D0_IRQ_STATUS(DMA_RUN | DMA_DONE | DMA_ERR);

	/* Scratchpad cannot be a DMA source or destination */
	if (((unsigned long)src >= L1_SRAM_SCRATCH &&
	     (unsigned long)src < L1_SRAM_SCRATCH_END) ||
	    ((unsigned long)dst >= L1_SRAM_SCRATCH &&
	     (unsigned long)dst < L1_SRAM_SCRATCH_END))
		hang();

	if (((unsigned long)dst | (unsigned long)src | count) & 0x1) {
		wdsize = WDSIZE_8;
		mod = 1;
	} else if (((unsigned long)dst | (unsigned long)src | count) & 0x2) {
		wdsize = WDSIZE_16;
		count >>= 1;
		mod = 2;
	} else {
		wdsize = WDSIZE_32;
		count >>= 2;
		mod = 4;
	}

	/* Copy sram functions from sdram to sram */
	/* Setup destination start address */
	bfin_write_MDMA_D0_START_ADDR(dst);
	/* Setup destination xcount */
	bfin_write_MDMA_D0_X_COUNT(count);
	/* Setup destination xmodify */
	bfin_write_MDMA_D0_X_MODIFY(mod);

	/* Setup Source start address */
	bfin_write_MDMA_S0_START_ADDR(src);
	/* Setup Source xcount */
	bfin_write_MDMA_S0_X_COUNT(count);
	/* Setup Source xmodify */
	bfin_write_MDMA_S0_X_MODIFY(mod);

	/* Enable source DMA */
	bfin_write_MDMA_S0_CONFIG(wdsize | DMAEN);
	bfin_write_MDMA_D0_CONFIG(wdsize | DMAEN | WNR | DI_EN);
	SSYNC();

	while (!(bfin_read_MDMA_D0_IRQ_STATUS() & DMA_DONE))
		continue;

	bfin_write_MDMA_D0_IRQ_STATUS(DMA_RUN | DMA_DONE | DMA_ERR);
	bfin_write_MDMA_D0_CONFIG(0);
	bfin_write_MDMA_S0_CONFIG(0);
}
/* We should do a dcache invalidate on the destination after the dma, but since
 * we lack such hardware capability, we'll flush/invalidate the destination
 * before the dma and bank on the idea that u-boot is single threaded.
 */
void *dma_memcpy(void *dst, const void *src, size_t count)
{
	if (dcache_status()) {
		blackfin_dcache_flush_range(src, src + count);
		blackfin_dcache_flush_invalidate_range(dst, dst + count);
	}

	dma_memcpy_nocache(dst, src, count);

	if (icache_status())
		blackfin_icache_flush_range(dst, dst + count);

	return dst;
}

/*
 * memcpy - Copy one area of memory to another
 * @dest: Where to copy to
 * @src: Where to copy from
 * @count: The size of the area.
 *
 * We need to have this wrapper in memcpy() as common code may call memcpy()
 * to load up L1 regions.  Consider loading an ELF which has sections with
 * LMA's pointing to L1.  The common code ELF loader will simply use memcpy()
 * to move the ELF's sections into the right place.  We need to catch that
 * here and redirect to dma_memcpy().
 */
extern void *memcpy_ASM(void *dst, const void *src, size_t count);
void *memcpy(void *dst, const void *src, size_t count)
{
	if (!count)
		return dst;

#ifdef CONFIG_CMD_KGDB
	if (src >= (void *)SYSMMR_BASE) {
		if (count == 2 && (unsigned long)src % 2 == 0) {
			u16 mmr = bfin_read16(src);
			memcpy(dst, &mmr, sizeof(mmr));
			return dst;
		}
		if (count == 4 && (unsigned long)src % 4 == 0) {
			u32 mmr = bfin_read32(src);
			memcpy(dst, &mmr, sizeof(mmr));
			return dst;
		}
		/* Failed for some reason */
		memset(dst, 0xad, count);
		return dst;
	}
	if (dst >= (void *)SYSMMR_BASE) {
		if (count == 2 && (unsigned long)dst % 2 == 0) {
			u16 mmr;
			memcpy(&mmr, src, sizeof(mmr));
			bfin_write16(dst, mmr);
			return dst;
		}
		if (count == 4 && (unsigned long)dst % 4 == 0) {
			u32 mmr;
			memcpy(&mmr, src, sizeof(mmr));
			bfin_write32(dst, mmr);
			return dst;
		}
		/* Failed for some reason */
		memset(dst, 0xad, count);
		return dst;
	}
#endif

	/* if L1 is the source or dst, use DMA */
	if (addr_bfin_on_chip_mem(dst) || addr_bfin_on_chip_mem(src))
		return dma_memcpy(dst, src, count);
	else
		/* No L1 is involved, so just call regular memcpy */
		return memcpy_ASM(dst, src, count);
}
