/*
 * Copyright 2007-2011 Freescale Semiconductor, Inc.
 *
 * (C) Copyright 2000
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

#include <common.h>
#include <libfdt.h>
#include <fdt_support.h>
#include <asm/processor.h>
#include <linux/ctype.h>
#include <asm/io.h>
#include <asm/fsl_portals.h>
#ifdef CONFIG_FSL_ESDHC
#include <fsl_esdhc.h>
#endif
#include "../../../../drivers/qe/qe.h"		/* For struct qe_firmware */

DECLARE_GLOBAL_DATA_PTR;

extern void ft_qe_setup(void *blob);
extern void ft_fixup_num_cores(void *blob);
extern void ft_srio_setup(void *blob);

#ifdef CONFIG_MP
#include "mp.h"

void ft_fixup_cpu(void *blob, u64 memory_limit)
{
	int off;
	ulong spin_tbl_addr = get_spin_phys_addr();
	u32 bootpg = determine_mp_bootpg();
	u32 id = get_my_id();
	const char *enable_method;

	off = fdt_node_offset_by_prop_value(blob, -1, "device_type", "cpu", 4);
	while (off != -FDT_ERR_NOTFOUND) {
		u32 *reg = (u32 *)fdt_getprop(blob, off, "reg", 0);

		if (reg) {
			u64 val = *reg * SIZE_BOOT_ENTRY + spin_tbl_addr;
			val = cpu_to_fdt32(val);
			if (*reg == id) {
				fdt_setprop_string(blob, off, "status",
								"okay");
			} else {
				fdt_setprop_string(blob, off, "status",
								"disabled");
			}

			if (hold_cores_in_reset(0)) {
#ifdef CONFIG_FSL_CORENET
				/* Cores held in reset, use BRR to release */
				enable_method = "fsl,brr-holdoff";
#else
				/* Cores held in reset, use EEBPCR to release */
				enable_method = "fsl,eebpcr-holdoff";
#endif
			} else {
				/* Cores out of reset and in a spin-loop */
				enable_method = "spin-table";

				fdt_setprop(blob, off, "cpu-release-addr",
						&val, sizeof(val));
			}

			fdt_setprop_string(blob, off, "enable-method",
							enable_method);
		} else {
			printf ("cpu NULL\n");
		}
		off = fdt_node_offset_by_prop_value(blob, off,
				"device_type", "cpu", 4);
	}

	/* Reserve the boot page so OSes dont use it */
	if ((u64)bootpg < memory_limit) {
		off = fdt_add_mem_rsv(blob, bootpg, (u64)4096);
		if (off < 0)
			printf("%s: %s\n", __FUNCTION__, fdt_strerror(off));
	}
}
#endif

#ifdef CONFIG_SYS_FSL_CPC
static inline void ft_fixup_l3cache(void *blob, int off)
{
	u32 line_size, num_ways, size, num_sets;
	cpc_corenet_t *cpc = (void *)CONFIG_SYS_FSL_CPC_ADDR;
	u32 cfg0 = in_be32(&cpc->cpccfg0);

	size = CPC_CFG0_SZ_K(cfg0) * 1024 * CONFIG_SYS_NUM_CPC;
	num_ways = CPC_CFG0_NUM_WAYS(cfg0);
	line_size = CPC_CFG0_LINE_SZ(cfg0);
	num_sets = size / (line_size * num_ways);

	fdt_setprop(blob, off, "cache-unified", NULL, 0);
	fdt_setprop_cell(blob, off, "cache-block-size", line_size);
	fdt_setprop_cell(blob, off, "cache-size", size);
	fdt_setprop_cell(blob, off, "cache-sets", num_sets);
	fdt_setprop_cell(blob, off, "cache-level", 3);
#ifdef CONFIG_SYS_CACHE_STASHING
	fdt_setprop_cell(blob, off, "cache-stash-id", 1);
#endif
}
#else
#define ft_fixup_l3cache(x, y)
#endif

#if defined(CONFIG_L2_CACHE)
/* return size in kilobytes */
static inline u32 l2cache_size(void)
{
	volatile ccsr_l2cache_t *l2cache = (void *)CONFIG_SYS_MPC85xx_L2_ADDR;
	volatile u32 l2siz_field = (l2cache->l2ctl >> 28) & 0x3;
	u32 ver = SVR_SOC_VER(get_svr());

	switch (l2siz_field) {
	case 0x0:
		break;
	case 0x1:
		if (ver == SVR_8540 || ver == SVR_8560   ||
		    ver == SVR_8541 || ver == SVR_8555)
			return 128;
		else
			return 256;
		break;
	case 0x2:
		if (ver == SVR_8540 || ver == SVR_8560   ||
		    ver == SVR_8541 || ver == SVR_8555)
			return 256;
		else
			return 512;
		break;
	case 0x3:
		return 1024;
		break;
	}

	return 0;
}

static inline void ft_fixup_l2cache(void *blob)
{
	int len, off;
	u32 *ph;
	struct cpu_type *cpu = identify_cpu(SVR_SOC_VER(get_svr()));

	const u32 line_size = 32;
	const u32 num_ways = 8;
	const u32 size = l2cache_size() * 1024;
	const u32 num_sets = size / (line_size * num_ways);

	off = fdt_node_offset_by_prop_value(blob, -1, "device_type", "cpu", 4);
	if (off < 0) {
		debug("no cpu node fount\n");
		return;
	}

	ph = (u32 *)fdt_getprop(blob, off, "next-level-cache", 0);

	if (ph == NULL) {
		debug("no next-level-cache property\n");
		return ;
	}

	off = fdt_node_offset_by_phandle(blob, *ph);
	if (off < 0) {
		printf("%s: %s\n", __func__, fdt_strerror(off));
		return ;
	}

	if (cpu) {
		char buf[40];

		if (isdigit(cpu->name[0])) {
			/* MPCxxxx, where xxxx == 4-digit number */
			len = sprintf(buf, "fsl,mpc%s-l2-cache-controller",
				cpu->name) + 1;
		} else {
			/* Pxxxx or Txxxx, where xxxx == 4-digit number */
			len = sprintf(buf, "fsl,%c%s-l2-cache-controller",
				tolower(cpu->name[0]), cpu->name + 1) + 1;
		}

		/*
		 * append "cache" after the NULL character that the previous
		 * sprintf wrote.  This is how a device tree stores multiple
		 * strings in a property.
		 */
		len += sprintf(buf + len, "cache") + 1;

		fdt_setprop(blob, off, "compatible", buf, len);
	}
	fdt_setprop(blob, off, "cache-unified", NULL, 0);
	fdt_setprop_cell(blob, off, "cache-block-size", line_size);
	fdt_setprop_cell(blob, off, "cache-size", size);
	fdt_setprop_cell(blob, off, "cache-sets", num_sets);
	fdt_setprop_cell(blob, off, "cache-level", 2);

	/* we dont bother w/L3 since no platform of this type has one */
}
#elif defined(CONFIG_BACKSIDE_L2_CACHE)
static inline void ft_fixup_l2cache(void *blob)
{
	int off, l2_off, l3_off = -1;
	u32 *ph;
	u32 l2cfg0 = mfspr(SPRN_L2CFG0);
	u32 size, line_size, num_ways, num_sets;
	int has_l2 = 1;

	/* P2040/P2040E has no L2, so dont set any L2 props */
	if (SVR_SOC_VER(get_svr()) == SVR_P2040)
		has_l2 = 0;

	size = (l2cfg0 & 0x3fff) * 64 * 1024;
	num_ways = ((l2cfg0 >> 14) & 0x1f) + 1;
	line_size = (((l2cfg0 >> 23) & 0x3) + 1) * 32;
	num_sets = size / (line_size * num_ways);

	off = fdt_node_offset_by_prop_value(blob, -1, "device_type", "cpu", 4);

	while (off != -FDT_ERR_NOTFOUND) {
		ph = (u32 *)fdt_getprop(blob, off, "next-level-cache", 0);

		if (ph == NULL) {
			debug("no next-level-cache property\n");
			goto next;
		}

		l2_off = fdt_node_offset_by_phandle(blob, *ph);
		if (l2_off < 0) {
			printf("%s: %s\n", __func__, fdt_strerror(off));
			goto next;
		}

		if (has_l2) {
#ifdef CONFIG_SYS_CACHE_STASHING
			u32 *reg = (u32 *)fdt_getprop(blob, off, "reg", 0);
			if (reg)
				fdt_setprop_cell(blob, l2_off, "cache-stash-id",
					 (*reg * 2) + 32 + 1);
#endif

			fdt_setprop(blob, l2_off, "cache-unified", NULL, 0);
			fdt_setprop_cell(blob, l2_off, "cache-block-size",
						line_size);
			fdt_setprop_cell(blob, l2_off, "cache-size", size);
			fdt_setprop_cell(blob, l2_off, "cache-sets", num_sets);
			fdt_setprop_cell(blob, l2_off, "cache-level", 2);
			fdt_setprop(blob, l2_off, "compatible", "cache", 6);
		}

		if (l3_off < 0) {
			ph = (u32 *)fdt_getprop(blob, l2_off, "next-level-cache", 0);

			if (ph == NULL) {
				debug("no next-level-cache property\n");
				goto next;
			}
			l3_off = *ph;
		}
next:
		off = fdt_node_offset_by_prop_value(blob, off,
				"device_type", "cpu", 4);
	}
	if (l3_off > 0) {
		l3_off = fdt_node_offset_by_phandle(blob, l3_off);
		if (l3_off < 0) {
			printf("%s: %s\n", __func__, fdt_strerror(off));
			return ;
		}
		ft_fixup_l3cache(blob, l3_off);
	}
}
#else
#define ft_fixup_l2cache(x)
#endif

static inline void ft_fixup_cache(void *blob)
{
	int off;

	off = fdt_node_offset_by_prop_value(blob, -1, "device_type", "cpu", 4);

	while (off != -FDT_ERR_NOTFOUND) {
		u32 l1cfg0 = mfspr(SPRN_L1CFG0);
		u32 l1cfg1 = mfspr(SPRN_L1CFG1);
		u32 isize, iline_size, inum_sets, inum_ways;
		u32 dsize, dline_size, dnum_sets, dnum_ways;

		/* d-side config */
		dsize = (l1cfg0 & 0x7ff) * 1024;
		dnum_ways = ((l1cfg0 >> 11) & 0xff) + 1;
		dline_size = (((l1cfg0 >> 23) & 0x3) + 1) * 32;
		dnum_sets = dsize / (dline_size * dnum_ways);

		fdt_setprop_cell(blob, off, "d-cache-block-size", dline_size);
		fdt_setprop_cell(blob, off, "d-cache-size", dsize);
		fdt_setprop_cell(blob, off, "d-cache-sets", dnum_sets);

#ifdef CONFIG_SYS_CACHE_STASHING
		{
			u32 *reg = (u32 *)fdt_getprop(blob, off, "reg", 0);
			if (reg)
				fdt_setprop_cell(blob, off, "cache-stash-id",
					 (*reg * 2) + 32 + 0);
		}
#endif

		/* i-side config */
		isize = (l1cfg1 & 0x7ff) * 1024;
		inum_ways = ((l1cfg1 >> 11) & 0xff) + 1;
		iline_size = (((l1cfg1 >> 23) & 0x3) + 1) * 32;
		inum_sets = isize / (iline_size * inum_ways);

		fdt_setprop_cell(blob, off, "i-cache-block-size", iline_size);
		fdt_setprop_cell(blob, off, "i-cache-size", isize);
		fdt_setprop_cell(blob, off, "i-cache-sets", inum_sets);

		off = fdt_node_offset_by_prop_value(blob, off,
				"device_type", "cpu", 4);
	}

	ft_fixup_l2cache(blob);
}


void fdt_add_enet_stashing(void *fdt)
{
	do_fixup_by_compat(fdt, "gianfar", "bd-stash", NULL, 0, 1);

	do_fixup_by_compat_u32(fdt, "gianfar", "rx-stash-len", 96, 1);

	do_fixup_by_compat_u32(fdt, "gianfar", "rx-stash-idx", 0, 1);
	do_fixup_by_compat(fdt, "fsl,etsec2", "bd-stash", NULL, 0, 1);
	do_fixup_by_compat_u32(fdt, "fsl,etsec2", "rx-stash-len", 96, 1);
	do_fixup_by_compat_u32(fdt, "fsl,etsec2", "rx-stash-idx", 0, 1);
}

#if defined(CONFIG_SYS_DPAA_FMAN) || defined(CONFIG_SYS_DPAA_PME)
#ifdef CONFIG_SYS_DPAA_FMAN
static void ft_fixup_clks(void *blob, const char *compat, u32 offset,
			  unsigned long freq)
{
	phys_addr_t phys = offset + CONFIG_SYS_CCSRBAR_PHYS;
	int off = fdt_node_offset_by_compat_reg(blob, compat, phys);

	if (off >= 0) {
		off = fdt_setprop_cell(blob, off, "clock-frequency", freq);
		if (off > 0)
			printf("WARNING enable to set clock-frequency "
				"for %s: %s\n", compat, fdt_strerror(off));
	}
}
#endif

static void ft_fixup_dpaa_clks(void *blob)
{
	sys_info_t sysinfo;

	get_sys_info(&sysinfo);
#ifdef CONFIG_SYS_DPAA_FMAN
	ft_fixup_clks(blob, "fsl,fman", CONFIG_SYS_FSL_FM1_OFFSET,
			sysinfo.freqFMan[0]);

#if (CONFIG_SYS_NUM_FMAN == 2)
	ft_fixup_clks(blob, "fsl,fman", CONFIG_SYS_FSL_FM2_OFFSET,
			sysinfo.freqFMan[1]);
#endif
#endif

#ifdef CONFIG_SYS_DPAA_PME
	do_fixup_by_compat_u32(blob, "fsl,pme",
		"clock-frequency", sysinfo.freqPME, 1);
#endif
}
#else
#define ft_fixup_dpaa_clks(x)
#endif

#ifdef CONFIG_QE
static void ft_fixup_qe_snum(void *blob)
{
	unsigned int svr;

	svr = mfspr(SPRN_SVR);
	if (SVR_SOC_VER(svr) == SVR_8569) {
		if(IS_SVR_REV(svr, 1, 0))
			do_fixup_by_compat_u32(blob, "fsl,qe",
				"fsl,qe-num-snums", 46, 1);
		else
			do_fixup_by_compat_u32(blob, "fsl,qe",
				"fsl,qe-num-snums", 76, 1);
	}
}
#endif

/**
 * fdt_fixup_fman_firmware -- insert the Fman firmware into the device tree
 *
 * The binding for an Fman firmware node is documented in
 * Documentation/powerpc/dts-bindings/fsl/dpaa/fman.txt.  This node contains
 * the actual Fman firmware binary data.  The operating system is expected to
 * be able to parse the binary data to determine any attributes it needs.
 */
#ifdef CONFIG_SYS_DPAA_FMAN
void fdt_fixup_fman_firmware(void *blob)
{
	int rc, fmnode, fwnode = -1;
	uint32_t phandle;
	struct qe_firmware *fmanfw;
	const struct qe_header *hdr;
	unsigned int length;
	uint32_t crc;
	const char *p;

	/* The first Fman we find will contain the actual firmware. */
	fmnode = fdt_node_offset_by_compatible(blob, -1, "fsl,fman");
	if (fmnode < 0)
		/* Exit silently if there are no Fman devices */
		return;

	/* If we already have a firmware node, then also exit silently. */
	if (fdt_node_offset_by_compatible(blob, -1, "fsl,fman-firmware") > 0)
		return;

	/* If the environment variable is not set, then exit silently */
	p = getenv("fman_ucode");
	if (!p)
		return;

	fmanfw = (struct qe_firmware *) simple_strtoul(p, NULL, 0);
	if (!fmanfw)
		return;

	hdr = &fmanfw->header;
	length = be32_to_cpu(hdr->length);

	/* Verify the firmware. */
	if ((hdr->magic[0] != 'Q') || (hdr->magic[1] != 'E') ||
		(hdr->magic[2] != 'F')) {
		printf("Data at %p is not an Fman firmware\n", fmanfw);
		return;
	}

	if (length > CONFIG_SYS_QE_FMAN_FW_LENGTH) {
		printf("Fman firmware at %p is too large (size=%u)\n",
		       fmanfw, length);
		return;
	}

	length -= sizeof(u32);	/* Subtract the size of the CRC */
	crc = be32_to_cpu(*(u32 *)((void *)fmanfw + length));
	if (crc != crc32_no_comp(0, (void *)fmanfw, length)) {
		printf("Fman firmware at %p has invalid CRC\n", fmanfw);
		return;
	}

	/* Increase the size of the fdt to make room for the node. */
	rc = fdt_increase_size(blob, fmanfw->header.length);
	if (rc < 0) {
		printf("Unable to make room for Fman firmware: %s\n",
			fdt_strerror(rc));
		return;
	}

	/* Create the firmware node. */
	fwnode = fdt_add_subnode(blob, fmnode, "fman-firmware");
	if (fwnode < 0) {
		char s[64];
		fdt_get_path(blob, fmnode, s, sizeof(s));
		printf("Could not add firmware node to %s: %s\n", s,
		       fdt_strerror(fwnode));
		return;
	}
	rc = fdt_setprop_string(blob, fwnode, "compatible", "fsl,fman-firmware");
	if (rc < 0) {
		char s[64];
		fdt_get_path(blob, fwnode, s, sizeof(s));
		printf("Could not add compatible property to node %s: %s\n", s,
		       fdt_strerror(rc));
		return;
	}
	phandle = fdt_create_phandle(blob, fwnode);
	if (!phandle) {
		char s[64];
		fdt_get_path(blob, fwnode, s, sizeof(s));
		printf("Could not add phandle property to node %s: %s\n", s,
		       fdt_strerror(rc));
		return;
	}
	rc = fdt_setprop(blob, fwnode, "fsl,firmware", fmanfw, fmanfw->header.length);
	if (rc < 0) {
		char s[64];
		fdt_get_path(blob, fwnode, s, sizeof(s));
		printf("Could not add firmware property to node %s: %s\n", s,
		       fdt_strerror(rc));
		return;
	}

	/* Find all other Fman nodes and point them to the firmware node. */
	while ((fmnode = fdt_node_offset_by_compatible(blob, fmnode, "fsl,fman")) > 0) {
		rc = fdt_setprop_cell(blob, fmnode, "fsl,firmware-phandle", phandle);
		if (rc < 0) {
			char s[64];
			fdt_get_path(blob, fmnode, s, sizeof(s));
			printf("Could not add pointer property to node %s: %s\n",
			       s, fdt_strerror(rc));
			return;
		}
	}
}
#else
#define fdt_fixup_fman_firmware(x)
#endif

#if defined(CONFIG_PPC_P4080) || defined(CONFIG_PPC_P3060)
static void fdt_fixup_usb(void *fdt)
{
	ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);
	u32 rcwsr11 = in_be32(&gur->rcwsr[11]);
	int off;

	off = fdt_node_offset_by_compatible(fdt, -1, "fsl,mpc85xx-usb2-mph");
	if ((rcwsr11 & FSL_CORENET_RCWSR11_EC1) !=
				FSL_CORENET_RCWSR11_EC1_FM1_USB1)
		fdt_status_disabled(fdt, off);

	off = fdt_node_offset_by_compatible(fdt, -1, "fsl,mpc85xx-usb2-dr");
	if ((rcwsr11 & FSL_CORENET_RCWSR11_EC2) !=
				FSL_CORENET_RCWSR11_EC2_USB2)
		fdt_status_disabled(fdt, off);
}
#else
#define fdt_fixup_usb(x)
#endif

void ft_cpu_setup(void *blob, bd_t *bd)
{
	int off;
	int val;
	sys_info_t sysinfo;

	/* delete crypto node if not on an E-processor */
	if (!IS_E_PROCESSOR(get_svr()))
		fdt_fixup_crypto_node(blob, 0);

	fdt_fixup_ethernet(blob);

	fdt_add_enet_stashing(blob);

	do_fixup_by_prop_u32(blob, "device_type", "cpu", 4,
		"timebase-frequency", get_tbclk(), 1);
	do_fixup_by_prop_u32(blob, "device_type", "cpu", 4,
		"bus-frequency", bd->bi_busfreq, 1);
	get_sys_info(&sysinfo);
	off = fdt_node_offset_by_prop_value(blob, -1, "device_type", "cpu", 4);
	while (off != -FDT_ERR_NOTFOUND) {
		u32 *reg = (u32 *)fdt_getprop(blob, off, "reg", 0);
		val = cpu_to_fdt32(sysinfo.freqProcessor[*reg]);
		fdt_setprop(blob, off, "clock-frequency", &val, 4);
		off = fdt_node_offset_by_prop_value(blob, off, "device_type",
							"cpu", 4);
	}
	do_fixup_by_prop_u32(blob, "device_type", "soc", 4,
		"bus-frequency", bd->bi_busfreq, 1);

	do_fixup_by_compat_u32(blob, "fsl,pq3-localbus",
		"bus-frequency", gd->lbc_clk, 1);
	do_fixup_by_compat_u32(blob, "fsl,elbc",
		"bus-frequency", gd->lbc_clk, 1);
#ifdef CONFIG_QE
	ft_qe_setup(blob);
	ft_fixup_qe_snum(blob);
#endif

	fdt_fixup_fman_firmware(blob);

#ifdef CONFIG_SYS_NS16550
	do_fixup_by_compat_u32(blob, "ns16550",
		"clock-frequency", CONFIG_SYS_NS16550_CLK, 1);
#endif

#ifdef CONFIG_CPM2
	do_fixup_by_compat_u32(blob, "fsl,cpm2-scc-uart",
		"current-speed", bd->bi_baudrate, 1);

	do_fixup_by_compat_u32(blob, "fsl,cpm2-brg",
		"clock-frequency", bd->bi_brgfreq, 1);
#endif

#ifdef CONFIG_FSL_CORENET
	do_fixup_by_compat_u32(blob, "fsl,qoriq-clockgen-1.0",
		"clock-frequency", CONFIG_SYS_CLK_FREQ, 1);
#endif

	fdt_fixup_memory(blob, (u64)bd->bi_memstart, (u64)bd->bi_memsize);

#ifdef CONFIG_MP
	ft_fixup_cpu(blob, (u64)bd->bi_memstart + (u64)bd->bi_memsize);
	ft_fixup_num_cores(blob);
#endif

	ft_fixup_cache(blob);

#if defined(CONFIG_FSL_ESDHC)
	fdt_fixup_esdhc(blob, bd);
#endif

	ft_fixup_dpaa_clks(blob);

#if defined(CONFIG_SYS_BMAN_MEM_PHYS)
	fdt_portal(blob, "fsl,bman-portal", "bman-portals",
			(u64)CONFIG_SYS_BMAN_MEM_PHYS,
			CONFIG_SYS_BMAN_MEM_SIZE);
	fdt_fixup_bportals(blob);
#endif

#if defined(CONFIG_SYS_QMAN_MEM_PHYS)
	fdt_portal(blob, "fsl,qman-portal", "qman-portals",
			(u64)CONFIG_SYS_QMAN_MEM_PHYS,
			CONFIG_SYS_QMAN_MEM_SIZE);

	fdt_fixup_qportals(blob);
#endif

#ifdef CONFIG_SYS_SRIO
	ft_srio_setup(blob);
#endif

	/*
	 * system-clock = CCB clock/2
	 * Here gd->bus_clk = CCB clock
	 * We are using the system clock as 1588 Timer reference
	 * clock source select
	 */
	do_fixup_by_compat_u32(blob, "fsl,gianfar-ptp-timer",
			"timer-frequency", gd->bus_clk/2, 1);

	/*
	 * clock-freq should change to clock-frequency and
	 * flexcan-v1.0 should change to p1010-flexcan respectively
	 * in the future.
	 */
	do_fixup_by_compat_u32(blob, "fsl,flexcan-v1.0",
			"clock_freq", gd->bus_clk/2, 1);

	do_fixup_by_compat_u32(blob, "fsl,flexcan-v1.0",
			"clock-frequency", gd->bus_clk/2, 1);

	do_fixup_by_compat_u32(blob, "fsl,p1010-flexcan",
			"clock-frequency", gd->bus_clk/2, 1);

	fdt_fixup_usb(blob);
}

/*
 * For some CCSR devices, we only have the virtual address, not the physical
 * address.  This is because we map CCSR as a whole, so we typically don't need
 * a macro for the physical address of any device within CCSR.  In this case,
 * we calculate the physical address of that device using it's the difference
 * between the virtual address of the device and the virtual address of the
 * beginning of CCSR.
 */
#define CCSR_VIRT_TO_PHYS(x) \
	(CONFIG_SYS_CCSRBAR_PHYS + ((x) - CONFIG_SYS_CCSRBAR))

static void msg(const char *name, uint64_t uaddr, uint64_t daddr)
{
	printf("Warning: U-Boot configured %s at address %llx,\n"
	       "but the device tree has it at %llx\n", name, uaddr, daddr);
}

/*
 * Verify the device tree
 *
 * This function compares several CONFIG_xxx macros that contain physical
 * addresses with the corresponding nodes in the device tree, to see if
 * the physical addresses are all correct.  For example, if
 * CONFIG_SYS_NS16550_COM1 is defined, then it contains the virtual address
 * of the first UART.  We convert this to a physical address and compare
 * that with the physical address of the first ns16550-compatible node
 * in the device tree.  If they don't match, then we display a warning.
 *
 * Returns 1 on success, 0 on failure
 */
int ft_verify_fdt(void *fdt)
{
	uint64_t addr = 0;
	int aliases;
	int off;

	/* First check the CCSR base address */
	off = fdt_node_offset_by_prop_value(fdt, -1, "device_type", "soc", 4);
	if (off > 0)
		addr = fdt_get_base_address(fdt, off);

	if (!addr) {
		printf("Warning: could not determine base CCSR address in "
		       "device tree\n");
		/* No point in checking anything else */
		return 0;
	}

	if (addr != CONFIG_SYS_CCSRBAR_PHYS) {
		msg("CCSR", CONFIG_SYS_CCSRBAR_PHYS, addr);
		/* No point in checking anything else */
		return 0;
	}

	/*
	 * Check some nodes via aliases.  We assume that U-Boot and the device
	 * tree enumerate the devices equally.  E.g. the first serial port in
	 * U-Boot is the same as "serial0" in the device tree.
	 */
	aliases = fdt_path_offset(fdt, "/aliases");
	if (aliases > 0) {
#ifdef CONFIG_SYS_NS16550_COM1
		if (!fdt_verify_alias_address(fdt, aliases, "serial0",
			CCSR_VIRT_TO_PHYS(CONFIG_SYS_NS16550_COM1)))
			return 0;
#endif

#ifdef CONFIG_SYS_NS16550_COM2
		if (!fdt_verify_alias_address(fdt, aliases, "serial1",
			CCSR_VIRT_TO_PHYS(CONFIG_SYS_NS16550_COM2)))
			return 0;
#endif
	}

	/*
	 * The localbus node is typically a root node, even though the lbc
	 * controller is part of CCSR.  If we were to put the lbc node under
	 * the SOC node, then the 'ranges' property in the lbc node would
	 * translate through the 'ranges' property of the parent SOC node, and
	 * we don't want that.  Since it's a separate node, it's possible for
	 * the 'reg' property to be wrong, so check it here.  For now, we
	 * only check for "fsl,elbc" nodes.
	 */
#ifdef CONFIG_SYS_LBC_ADDR
	off = fdt_node_offset_by_compatible(fdt, -1, "fsl,elbc");
	if (off > 0) {
		const u32 *reg = fdt_getprop(fdt, off, "reg", NULL);
		if (reg) {
			uint64_t uaddr = CCSR_VIRT_TO_PHYS(CONFIG_SYS_LBC_ADDR);

			addr = fdt_translate_address(fdt, off, reg);
			if (uaddr != addr) {
				msg("the localbus", uaddr, addr);
				return 0;
			}
		}
	}
#endif

	return 1;
}
