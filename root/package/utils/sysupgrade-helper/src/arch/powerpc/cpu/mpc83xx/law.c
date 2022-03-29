/*
 * Copyright 2011 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
 */

#include <common.h>
#include <asm/fsl_law.h>
#include <asm/mmu.h>

int set_ddr_laws(u64 start, u64 sz, enum law_trgt_if id)
{
	immap_t *immap = (immap_t *)CONFIG_SYS_IMMR;
	law83xx_t *ecm = &immap->sysconf.ddrlaw[0];
	u64 start_align, law_sz;
	int law_sz_enc;

	if (start == 0)
		start_align = 1ull << (LAW_SIZE_2G + 1);
	else
		start_align = 1ull << (ffs64(start) - 1);
	law_sz = min(start_align, sz);
	law_sz_enc = __ilog2_u64(law_sz) - 1;

	/*
	 * Set up LAWBAR for all of DDR.
	 */
	ecm->bar = start & 0xfffff000;
	ecm->ar  = (LAWAR_EN | (id << 20) | (LAWAR_SIZE & law_sz_enc));
	debug("DDR:bar=0x%08x\n", ecm->bar);
	debug("DDR:ar=0x%08x\n", ecm->ar);

	/* recalculate size based on what was actually covered by the law */
	law_sz = 1ull << __ilog2_u64(law_sz);

	/* do we still have anything to map */
	sz = sz - law_sz;
	if (sz) {
		start += law_sz;

		start_align = 1ull << (ffs64(start) - 1);
		law_sz = min(start_align, sz);
		law_sz_enc = __ilog2_u64(law_sz) - 1;
		ecm = &immap->sysconf.ddrlaw[1];
		ecm->bar = start & 0xfffff000;
		ecm->ar  = (LAWAR_EN | (id << 20) | (LAWAR_SIZE & law_sz_enc));
		debug("DDR:bar=0x%08x\n", ecm->bar);
		debug("DDR:ar=0x%08x\n", ecm->ar);
	} else {
		return 0;
	}

	/* do we still have anything to map */
	sz = sz - law_sz;
	if (sz)
		return 1;

	return 0;
}
