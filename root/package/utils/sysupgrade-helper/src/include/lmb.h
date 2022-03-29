#ifndef _LINUX_LMB_H
#define _LINUX_LMB_H
#ifdef __KERNEL__
#ifdef CONFIG_LMB

#include <asm/types.h>
/*
 * Logical memory blocks.
 *
 * Copyright (C) 2001 Peter Bergner, IBM Corp.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#define MAX_LMB_REGIONS 8

struct lmb_property {
	phys_addr_t base;
	phys_size_t size;
};

struct lmb_region {
	unsigned long cnt;
	phys_size_t size;
	struct lmb_property region[MAX_LMB_REGIONS+1];
};

struct lmb {
	struct lmb_region memory;
	struct lmb_region reserved;
};

extern struct lmb lmb;

extern void lmb_init(struct lmb *lmb);
extern long lmb_add(struct lmb *lmb, phys_addr_t base, phys_size_t size);
extern long lmb_reserve(struct lmb *lmb, phys_addr_t base, phys_size_t size);
extern phys_addr_t lmb_alloc(struct lmb *lmb, phys_size_t size, ulong align);
extern phys_addr_t lmb_alloc_base(struct lmb *lmb, phys_size_t size, ulong align,
			    phys_addr_t max_addr);
extern phys_addr_t __lmb_alloc_base(struct lmb *lmb, phys_size_t size, ulong align,
			      phys_addr_t max_addr);
extern int lmb_is_reserved(struct lmb *lmb, phys_addr_t addr);
extern long lmb_free(struct lmb *lmb, phys_addr_t base, phys_size_t size);

extern void lmb_dump_all(struct lmb *lmb);

static inline phys_size_t
lmb_size_bytes(struct lmb_region *type, unsigned long region_nr)
{
	return type->region[region_nr].size;
}

void board_lmb_reserve(struct lmb *lmb);
void arch_lmb_reserve(struct lmb *lmb);

#endif /* CONFIG_LMB */
#endif /* __KERNEL__ */

#endif /* _LINUX_LMB_H */
