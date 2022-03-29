/*
 * Copyright 2008-2011 Freescale Semiconductor, Inc.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <libfdt.h>
#include <fdt_support.h>

#include <asm/immap_85xx.h>
#include <asm/io.h>
#include <asm/processor.h>
#include <asm/fsl_portals.h>
#include <asm/fsl_liodn.h>

int get_dpaa_liodn(enum fsl_dpaa_dev dpaa_dev, u32 *liodns, int liodn_offset)
{
	liodns[0] = liodn_bases[dpaa_dev].id[0] + liodn_offset;

	if (liodn_bases[dpaa_dev].num_ids == 2)
		liodns[1] = liodn_bases[dpaa_dev].id[1] + liodn_offset;

	return liodn_bases[dpaa_dev].num_ids;
}

static void set_srio_liodn(struct srio_liodn_id_table *tbl, int size)
{
	int i;

	for (i = 0; i < size; i++) {
		unsigned long reg_off = tbl[i].reg_offset[0];
		out_be32((u32 *)reg_off, tbl[i].id[0]);

		if (tbl[i].num_ids == 2) {
			reg_off = tbl[i].reg_offset[1];
			out_be32((u32 *)reg_off, tbl[i].id[1]);
		}
	}
}

static void set_liodn(struct liodn_id_table *tbl, int size)
{
	int i;

	for (i = 0; i < size; i++) {
		u32 liodn;
		if (tbl[i].num_ids == 2) {
			liodn = (tbl[i].id[0] << 16) | tbl[i].id[1];
		} else {
			liodn = tbl[i].id[0];
		}

		out_be32((volatile u32 *)(tbl[i].reg_offset), liodn);
	}
}

static void setup_sec_liodn_base(void)
{
	ccsr_sec_t *sec = (void *)CONFIG_SYS_FSL_SEC_ADDR;
	u32 base;

	if (!IS_E_PROCESSOR(get_svr()))
		return;

	/* QILCR[QSLOM] */
	out_be32(&sec->qilcr_ms, 0x3ff<<16);

	base = (liodn_bases[FSL_HW_PORTAL_SEC].id[0] << 16) |
		liodn_bases[FSL_HW_PORTAL_SEC].id[1];

	out_be32(&sec->qilcr_ls, base);
}

#ifdef CONFIG_SYS_DPAA_FMAN
static void setup_fman_liodn_base(enum fsl_dpaa_dev dev,
				  struct liodn_id_table *tbl, int size)
{
	int i;
	ccsr_fman_t *fm;
	u32 base;

	switch(dev) {
	case FSL_HW_PORTAL_FMAN1:
		fm = (void *)CONFIG_SYS_FSL_FM1_ADDR;
		break;

#if (CONFIG_SYS_NUM_FMAN == 2)
	case FSL_HW_PORTAL_FMAN2:
		fm = (void *)CONFIG_SYS_FSL_FM2_ADDR;
		break;
#endif
	default:
		printf("Error: Invalid device type to %s\n", __FUNCTION__);
		return ;
	}

	base = (liodn_bases[dev].id[0] << 16) | liodn_bases[dev].id[0];

	/* setup all bases the same */
	for (i = 0; i < 32; i++) {
		out_be32(&fm->fm_dma.fmdmplr[i], base);
	}

	/* update tbl to ... */
	for (i = 0; i < size; i++)
		tbl[i].id[0] += liodn_bases[dev].id[0];
}
#endif

static void setup_pme_liodn_base(void)
{
#ifdef CONFIG_SYS_DPAA_PME
	ccsr_pme_t *pme = (void *)CONFIG_SYS_FSL_CORENET_PME_ADDR;
	u32 base = (liodn_bases[FSL_HW_PORTAL_PME].id[0] << 16) |
			liodn_bases[FSL_HW_PORTAL_PME].id[1];

	out_be32(&pme->liodnbr, base);
#endif
}

#ifdef CONFIG_SYS_FSL_RAID_ENGINE
static void setup_raide_liodn_base(void)
{
	struct ccsr_raide *raide = (void *)CONFIG_SYS_FSL_RAID_ENGINE_ADDR;

	/* setup raid engine liodn base for data/desc ; both set to 47 */
	u32 base = (liodn_bases[FSL_HW_PORTAL_RAID_ENGINE].id[0] << 16) |
			liodn_bases[FSL_HW_PORTAL_RAID_ENGINE].id[0];

	out_be32(&raide->liodnbr, base);
}
#endif

#ifdef CONFIG_SYS_DPAA_RMAN
static void set_rman_liodn(struct liodn_id_table *tbl, int size)
{
	int i;
	struct ccsr_rman *rman = (void *)CONFIG_SYS_FSL_CORENET_RMAN_ADDR;

	for (i = 0; i < size; i++) {
		/* write the RMan block number */
		out_be32(&rman->mmitar, i);
		/* write the liodn offset corresponding to the block */
		out_be32((u32 *)(tbl[i].reg_offset), tbl[i].id[0]);
	}
}

static void setup_rman_liodn_base(struct liodn_id_table *tbl, int size)
{
	int i;
	struct ccsr_rman *rman = (void *)CONFIG_SYS_FSL_CORENET_RMAN_ADDR;
	u32 base = liodn_bases[FSL_HW_PORTAL_RMAN].id[0];

	out_be32(&rman->mmliodnbr, base);

	/* update liodn offset */
	for (i = 0; i < size; i++)
		tbl[i].id[0] += base;
}
#endif

void set_liodns(void)
{
	/* setup general liodn offsets */
	set_liodn(liodn_tbl, liodn_tbl_sz);

	/* setup SRIO port liodns */
	set_srio_liodn(srio_liodn_tbl, srio_liodn_tbl_sz);

	/* setup SEC block liodn bases & offsets if we have one */
	if (IS_E_PROCESSOR(get_svr())) {
		set_liodn(sec_liodn_tbl, sec_liodn_tbl_sz);
		setup_sec_liodn_base();
	}

	/* setup FMAN block(s) liodn bases & offsets if we have one */
#ifdef CONFIG_SYS_DPAA_FMAN
	set_liodn(fman1_liodn_tbl, fman1_liodn_tbl_sz);
	setup_fman_liodn_base(FSL_HW_PORTAL_FMAN1, fman1_liodn_tbl,
				fman1_liodn_tbl_sz);

#if (CONFIG_SYS_NUM_FMAN == 2)
	set_liodn(fman2_liodn_tbl, fman2_liodn_tbl_sz);
	setup_fman_liodn_base(FSL_HW_PORTAL_FMAN2, fman2_liodn_tbl,
				fman2_liodn_tbl_sz);
#endif
#endif
	/* setup PME liodn base */
	setup_pme_liodn_base();

#ifdef CONFIG_SYS_FSL_RAID_ENGINE
	/* raid engine ccr addr code for liodn */
	set_liodn(raide_liodn_tbl, raide_liodn_tbl_sz);
	setup_raide_liodn_base();
#endif

#ifdef CONFIG_SYS_DPAA_RMAN
	/* setup RMan liodn offsets */
	set_rman_liodn(rman_liodn_tbl, rman_liodn_tbl_sz);
	/* setup RMan liodn base */
	setup_rman_liodn_base(rman_liodn_tbl, rman_liodn_tbl_sz);
#endif
}

static void fdt_fixup_srio_liodn(void *blob, struct srio_liodn_id_table *tbl)
{
	int i, srio_off;

	/* search for srio node, if doesn't exist just return - nothing todo */
	srio_off = fdt_node_offset_by_compatible(blob, -1, "fsl,srio");
	if (srio_off < 0)
		return ;

	for (i = 0; i < srio_liodn_tbl_sz; i++) {
		int off, portid = tbl[i].portid;

		off = fdt_node_offset_by_prop_value(blob, srio_off,
			 "cell-index", &portid, 4);
		if (off >= 0) {
			off = fdt_setprop(blob, off, "fsl,liodn",
				&tbl[i].id[0],
				sizeof(u32) * tbl[i].num_ids);
			if (off > 0)
				printf("WARNING unable to set fsl,liodn for "
					"fsl,srio port %d: %s\n",
					portid, fdt_strerror(off));
		} else {
			debug("WARNING: couldn't set fsl,liodn for srio: %s.\n",
				fdt_strerror(off));
		}
	}
}

static void fdt_fixup_liodn_tbl(void *blob, struct liodn_id_table *tbl, int sz)
{
	int i;

	for (i = 0; i < sz; i++) {
		int off;

		if (tbl[i].compat == NULL)
			continue;

		off = fdt_node_offset_by_compat_reg(blob,
				tbl[i].compat, tbl[i].compat_offset);
		if (off >= 0) {
			off = fdt_setprop(blob, off, "fsl,liodn",
				&tbl[i].id[0],
				sizeof(u32) * tbl[i].num_ids);
			if (off > 0)
				printf("WARNING unable to set fsl,liodn for "
					"%s: %s\n",
					tbl[i].compat, fdt_strerror(off));
		} else {
			debug("WARNING: could not set fsl,liodn for %s: %s.\n",
					tbl[i].compat, fdt_strerror(off));
		}
	}
}

void fdt_fixup_liodn(void *blob)
{
	fdt_fixup_srio_liodn(blob, srio_liodn_tbl);

	fdt_fixup_liodn_tbl(blob, liodn_tbl, liodn_tbl_sz);
#ifdef CONFIG_SYS_DPAA_FMAN
	fdt_fixup_liodn_tbl(blob, fman1_liodn_tbl, fman1_liodn_tbl_sz);
#if (CONFIG_SYS_NUM_FMAN == 2)
	fdt_fixup_liodn_tbl(blob, fman2_liodn_tbl, fman2_liodn_tbl_sz);
#endif
#endif
	fdt_fixup_liodn_tbl(blob, sec_liodn_tbl, sec_liodn_tbl_sz);

#ifdef CONFIG_SYS_FSL_RAID_ENGINE
	fdt_fixup_liodn_tbl(blob, raide_liodn_tbl, raide_liodn_tbl_sz);
#endif

#ifdef CONFIG_SYS_DPAA_RMAN
	fdt_fixup_liodn_tbl(blob, rman_liodn_tbl, rman_liodn_tbl_sz);
#endif
}
