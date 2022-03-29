/*
 * Procedures for maintaining information about logical memory blocks.
 *
 * Peter Bergner, IBM Corp.	June 2001.
 * Copyright (C) 2001 Peter Bergner.
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */

#include <common.h>
#include <lmb.h>

#define LMB_ALLOC_ANYWHERE	0

void lmb_dump_all(struct lmb *lmb)
{
#ifdef DEBUG
	unsigned long i;

	debug("lmb_dump_all:\n");
	debug("    memory.cnt		   = 0x%lx\n", lmb->memory.cnt);
	debug("    memory.size		   = 0x%llx\n",
	      (unsigned long long)lmb->memory.size);
	for (i=0; i < lmb->memory.cnt ;i++) {
		debug("    memory.reg[0x%lx].base   = 0x%llx\n", i,
			(long long unsigned)lmb->memory.region[i].base);
		debug("		   .size   = 0x%llx\n",
			(long long unsigned)lmb->memory.region[i].size);
	}

	debug("\n    reserved.cnt	   = 0x%lx\n",
		lmb->reserved.cnt);
	debug("    reserved.size	   = 0x%llx\n",
		(long long unsigned)lmb->reserved.size);
	for (i=0; i < lmb->reserved.cnt ;i++) {
		debug("    reserved.reg[0x%lx].base = 0x%llx\n", i,
			(long long unsigned)lmb->reserved.region[i].base);
		debug("		     .size = 0x%llx\n",
			(long long unsigned)lmb->reserved.region[i].size);
	}
#endif /* DEBUG */
}

static long lmb_addrs_overlap(phys_addr_t base1,
		phys_size_t size1, phys_addr_t base2, phys_size_t size2)
{
	return ((base1 < (base2+size2)) && (base2 < (base1+size1)));
}

static long lmb_addrs_adjacent(phys_addr_t base1, phys_size_t size1,
		phys_addr_t base2, phys_size_t size2)
{
	if (base2 == base1 + size1)
		return 1;
	else if (base1 == base2 + size2)
		return -1;

	return 0;
}

static long lmb_regions_adjacent(struct lmb_region *rgn,
		unsigned long r1, unsigned long r2)
{
	phys_addr_t base1 = rgn->region[r1].base;
	phys_size_t size1 = rgn->region[r1].size;
	phys_addr_t base2 = rgn->region[r2].base;
	phys_size_t size2 = rgn->region[r2].size;

	return lmb_addrs_adjacent(base1, size1, base2, size2);
}

static void lmb_remove_region(struct lmb_region *rgn, unsigned long r)
{
	unsigned long i;

	for (i = r; i < rgn->cnt - 1; i++) {
		rgn->region[i].base = rgn->region[i + 1].base;
		rgn->region[i].size = rgn->region[i + 1].size;
	}
	rgn->cnt--;
}

/* Assumption: base addr of region 1 < base addr of region 2 */
static void lmb_coalesce_regions(struct lmb_region *rgn,
		unsigned long r1, unsigned long r2)
{
	rgn->region[r1].size += rgn->region[r2].size;
	lmb_remove_region(rgn, r2);
}

void lmb_init(struct lmb *lmb)
{
	/* Create a dummy zero size LMB which will get coalesced away later.
	 * This simplifies the lmb_add() code below...
	 */
	lmb->memory.region[0].base = 0;
	lmb->memory.region[0].size = 0;
	lmb->memory.cnt = 1;
	lmb->memory.size = 0;

	/* Ditto. */
	lmb->reserved.region[0].base = 0;
	lmb->reserved.region[0].size = 0;
	lmb->reserved.cnt = 1;
	lmb->reserved.size = 0;
}

/* This routine called with relocation disabled. */
static long lmb_add_region(struct lmb_region *rgn, phys_addr_t base, phys_size_t size)
{
	unsigned long coalesced = 0;
	long adjacent, i;

	if ((rgn->cnt == 1) && (rgn->region[0].size == 0)) {
		rgn->region[0].base = base;
		rgn->region[0].size = size;
		return 0;
	}

	/* First try and coalesce this LMB with another. */
	for (i=0; i < rgn->cnt; i++) {
		phys_addr_t rgnbase = rgn->region[i].base;
		phys_size_t rgnsize = rgn->region[i].size;

		if ((rgnbase == base) && (rgnsize == size))
			/* Already have this region, so we're done */
			return 0;

		adjacent = lmb_addrs_adjacent(base,size,rgnbase,rgnsize);
		if ( adjacent > 0 ) {
			rgn->region[i].base -= size;
			rgn->region[i].size += size;
			coalesced++;
			break;
		}
		else if ( adjacent < 0 ) {
			rgn->region[i].size += size;
			coalesced++;
			break;
		}
	}

	if ((i < rgn->cnt-1) && lmb_regions_adjacent(rgn, i, i+1) ) {
		lmb_coalesce_regions(rgn, i, i+1);
		coalesced++;
	}

	if (coalesced)
		return coalesced;
	if (rgn->cnt >= MAX_LMB_REGIONS)
		return -1;

	/* Couldn't coalesce the LMB, so add it to the sorted table. */
	for (i = rgn->cnt-1; i >= 0; i--) {
		if (base < rgn->region[i].base) {
			rgn->region[i+1].base = rgn->region[i].base;
			rgn->region[i+1].size = rgn->region[i].size;
		} else {
			rgn->region[i+1].base = base;
			rgn->region[i+1].size = size;
			break;
		}
	}

	if (base < rgn->region[0].base) {
		rgn->region[0].base = base;
		rgn->region[0].size = size;
	}

	rgn->cnt++;

	return 0;
}

/* This routine may be called with relocation disabled. */
long lmb_add(struct lmb *lmb, phys_addr_t base, phys_size_t size)
{
	struct lmb_region *_rgn = &(lmb->memory);

	return lmb_add_region(_rgn, base, size);
}

long lmb_free(struct lmb *lmb, phys_addr_t base, phys_size_t size)
{
	struct lmb_region *rgn = &(lmb->reserved);
	phys_addr_t rgnbegin, rgnend;
	phys_addr_t end = base + size;
	int i;

	rgnbegin = rgnend = 0; /* supress gcc warnings */

	/* Find the region where (base, size) belongs to */
	for (i=0; i < rgn->cnt; i++) {
		rgnbegin = rgn->region[i].base;
		rgnend = rgnbegin + rgn->region[i].size;

		if ((rgnbegin <= base) && (end <= rgnend))
			break;
	}

	/* Didn't find the region */
	if (i == rgn->cnt)
		return -1;

	/* Check to see if we are removing entire region */
	if ((rgnbegin == base) && (rgnend == end)) {
		lmb_remove_region(rgn, i);
		return 0;
	}

	/* Check to see if region is matching at the front */
	if (rgnbegin == base) {
		rgn->region[i].base = end;
		rgn->region[i].size -= size;
		return 0;
	}

	/* Check to see if the region is matching at the end */
	if (rgnend == end) {
		rgn->region[i].size -= size;
		return 0;
	}

	/*
	 * We need to split the entry -  adjust the current one to the
	 * beginging of the hole and add the region after hole.
	 */
	rgn->region[i].size = base - rgn->region[i].base;
	return lmb_add_region(rgn, end, rgnend - end);
}

long lmb_reserve(struct lmb *lmb, phys_addr_t base, phys_size_t size)
{
	struct lmb_region *_rgn = &(lmb->reserved);

	return lmb_add_region(_rgn, base, size);
}

long lmb_overlaps_region(struct lmb_region *rgn, phys_addr_t base,
				phys_size_t size)
{
	unsigned long i;

	for (i=0; i < rgn->cnt; i++) {
		phys_addr_t rgnbase = rgn->region[i].base;
		phys_size_t rgnsize = rgn->region[i].size;
		if ( lmb_addrs_overlap(base,size,rgnbase,rgnsize) ) {
			break;
		}
	}

	return (i < rgn->cnt) ? i : -1;
}

phys_addr_t lmb_alloc(struct lmb *lmb, phys_size_t size, ulong align)
{
	return lmb_alloc_base(lmb, size, align, LMB_ALLOC_ANYWHERE);
}

phys_addr_t lmb_alloc_base(struct lmb *lmb, phys_size_t size, ulong align, phys_addr_t max_addr)
{
	phys_addr_t alloc;

	alloc = __lmb_alloc_base(lmb, size, align, max_addr);

	if (alloc == 0)
		printf("ERROR: Failed to allocate 0x%lx bytes below 0x%lx.\n",
		      (ulong)size, (ulong)max_addr);

	return alloc;
}

static phys_addr_t lmb_align_down(phys_addr_t addr, phys_size_t size)
{
	return addr & ~(size - 1);
}

static phys_addr_t lmb_align_up(phys_addr_t addr, ulong size)
{
	return (addr + (size - 1)) & ~(size - 1);
}

phys_addr_t __lmb_alloc_base(struct lmb *lmb, phys_size_t size, ulong align, phys_addr_t max_addr)
{
	long i, j;
	phys_addr_t base = 0;
	phys_addr_t res_base;

	for (i = lmb->memory.cnt-1; i >= 0; i--) {
		phys_addr_t lmbbase = lmb->memory.region[i].base;
		phys_size_t lmbsize = lmb->memory.region[i].size;

		if (lmbsize < size)
			continue;
		if (max_addr == LMB_ALLOC_ANYWHERE)
			base = lmb_align_down(lmbbase + lmbsize - size, align);
		else if (lmbbase < max_addr) {
			base = min(lmbbase + lmbsize, max_addr);
			base = lmb_align_down(base - size, align);
		} else
			continue;

		while (base && lmbbase <= base) {
			j = lmb_overlaps_region(&lmb->reserved, base, size);
			if (j < 0) {
				/* This area isn't reserved, take it */
				if (lmb_add_region(&lmb->reserved, base,
							lmb_align_up(size,
								align)) < 0)
					return 0;
				return base;
			}
			res_base = lmb->reserved.region[j].base;
			if (res_base < size)
				break;
			base = lmb_align_down(res_base - size, align);
		}
	}
	return 0;
}

int lmb_is_reserved(struct lmb *lmb, phys_addr_t addr)
{
	int i;

	for (i = 0; i < lmb->reserved.cnt; i++) {
		phys_addr_t upper = lmb->reserved.region[i].base +
			lmb->reserved.region[i].size - 1;
		if ((addr >= lmb->reserved.region[i].base) && (addr <= upper))
			return 1;
	}
	return 0;
}

void __board_lmb_reserve(struct lmb *lmb)
{
	/* please define platform specific board_lmb_reserve() */
}
void board_lmb_reserve(struct lmb *lmb) __attribute__((weak, alias("__board_lmb_reserve")));

void __arch_lmb_reserve(struct lmb *lmb)
{
	/* please define platform specific arch_lmb_reserve() */
}
void arch_lmb_reserve(struct lmb *lmb) __attribute__((weak, alias("__arch_lmb_reserve")));
