/*
 * Copyright 2007-2011 Freescale Semiconductor, Inc.
 *
 * (C) Copyright 2003 Motorola Inc.
 * Modified by Xianghua Xiao, X.Xiao@motorola.com
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
#include <watchdog.h>
#include <asm/processor.h>
#include <ioports.h>
#include <sata.h>
#include <fm_eth.h>
#include <asm/io.h>
#include <asm/cache.h>
#include <asm/mmu.h>
#include <asm/fsl_law.h>
#include <asm/fsl_serdes.h>
#include <asm/fsl_srio.h>
#include <linux/compiler.h>
#include "mp.h"
#ifdef CONFIG_SYS_QE_FMAN_FW_IN_NAND
#include <nand.h>
#include <errno.h>
#endif

#include "../../../../drivers/block/fsl_sata.h"

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_QE
extern qe_iop_conf_t qe_iop_conf_tab[];
extern void qe_config_iopin(u8 port, u8 pin, int dir,
				int open_drain, int assign);
extern void qe_init(uint qe_base);
extern void qe_reset(void);

static void config_qe_ioports(void)
{
	u8      port, pin;
	int     dir, open_drain, assign;
	int     i;

	for (i = 0; qe_iop_conf_tab[i].assign != QE_IOP_TAB_END; i++) {
		port		= qe_iop_conf_tab[i].port;
		pin		= qe_iop_conf_tab[i].pin;
		dir		= qe_iop_conf_tab[i].dir;
		open_drain	= qe_iop_conf_tab[i].open_drain;
		assign		= qe_iop_conf_tab[i].assign;
		qe_config_iopin(port, pin, dir, open_drain, assign);
	}
}
#endif

#ifdef CONFIG_CPM2
void config_8560_ioports (volatile ccsr_cpm_t * cpm)
{
	int portnum;

	for (portnum = 0; portnum < 4; portnum++) {
		uint pmsk = 0,
		     ppar = 0,
		     psor = 0,
		     pdir = 0,
		     podr = 0,
		     pdat = 0;
		iop_conf_t *iopc = (iop_conf_t *) & iop_conf_tab[portnum][0];
		iop_conf_t *eiopc = iopc + 32;
		uint msk = 1;

		/*
		 * NOTE:
		 * index 0 refers to pin 31,
		 * index 31 refers to pin 0
		 */
		while (iopc < eiopc) {
			if (iopc->conf) {
				pmsk |= msk;
				if (iopc->ppar)
					ppar |= msk;
				if (iopc->psor)
					psor |= msk;
				if (iopc->pdir)
					pdir |= msk;
				if (iopc->podr)
					podr |= msk;
				if (iopc->pdat)
					pdat |= msk;
			}

			msk <<= 1;
			iopc++;
		}

		if (pmsk != 0) {
			volatile ioport_t *iop = ioport_addr (cpm, portnum);
			uint tpmsk = ~pmsk;

			/*
			 * the (somewhat confused) paragraph at the
			 * bottom of page 35-5 warns that there might
			 * be "unknown behaviour" when programming
			 * PSORx and PDIRx, if PPARx = 1, so I
			 * decided this meant I had to disable the
			 * dedicated function first, and enable it
			 * last.
			 */
			iop->ppar &= tpmsk;
			iop->psor = (iop->psor & tpmsk) | psor;
			iop->podr = (iop->podr & tpmsk) | podr;
			iop->pdat = (iop->pdat & tpmsk) | pdat;
			iop->pdir = (iop->pdir & tpmsk) | pdir;
			iop->ppar |= ppar;
		}
	}
}
#endif

#ifdef CONFIG_SYS_FSL_CPC
static void enable_cpc(void)
{
	int i;
	u32 size = 0;

	cpc_corenet_t *cpc = (cpc_corenet_t *)CONFIG_SYS_FSL_CPC_ADDR;

	for (i = 0; i < CONFIG_SYS_NUM_CPC; i++, cpc++) {
		u32 cpccfg0 = in_be32(&cpc->cpccfg0);
		size += CPC_CFG0_SZ_K(cpccfg0);
#ifdef CONFIG_RAMBOOT_PBL
		if (in_be32(&cpc->cpcsrcr0) & CPC_SRCR0_SRAMEN) {
			/* find and disable LAW of SRAM */
			struct law_entry law = find_law(CONFIG_SYS_INIT_L3_ADDR);

			if (law.index == -1) {
				printf("\nFatal error happened\n");
				return;
			}
			disable_law(law.index);

			clrbits_be32(&cpc->cpchdbcr0, CPC_HDBCR0_CDQ_SPEC_DIS);
			out_be32(&cpc->cpccsr0, 0);
			out_be32(&cpc->cpcsrcr0, 0);
		}
#endif

#ifdef CONFIG_SYS_FSL_ERRATUM_CPC_A002
		setbits_be32(&cpc->cpchdbcr0, CPC_HDBCR0_TAG_ECC_SCRUB_DIS);
#endif
#ifdef CONFIG_SYS_FSL_ERRATUM_CPC_A003
		setbits_be32(&cpc->cpchdbcr0, CPC_HDBCR0_DATA_ECC_SCRUB_DIS);
#endif

		out_be32(&cpc->cpccsr0, CPC_CSR0_CE | CPC_CSR0_PE);
		/* Read back to sync write */
		in_be32(&cpc->cpccsr0);

	}

	printf("Corenet Platform Cache: %d KB enabled\n", size);
}

void invalidate_cpc(void)
{
	int i;
	cpc_corenet_t *cpc = (cpc_corenet_t *)CONFIG_SYS_FSL_CPC_ADDR;

	for (i = 0; i < CONFIG_SYS_NUM_CPC; i++, cpc++) {
		/* skip CPC when it used as all SRAM */
		if (in_be32(&cpc->cpcsrcr0) & CPC_SRCR0_SRAMEN)
			continue;
		/* Flash invalidate the CPC and clear all the locks */
		out_be32(&cpc->cpccsr0, CPC_CSR0_FI | CPC_CSR0_LFC);
		while (in_be32(&cpc->cpccsr0) & (CPC_CSR0_FI | CPC_CSR0_LFC))
			;
	}
}
#else
#define enable_cpc()
#define invalidate_cpc()
#endif /* CONFIG_SYS_FSL_CPC */

/*
 * Breathe some life into the CPU...
 *
 * Set up the memory map
 * initialize a bunch of registers
 */

#ifdef CONFIG_FSL_CORENET
static void corenet_tb_init(void)
{
	volatile ccsr_rcpm_t *rcpm =
		(void *)(CONFIG_SYS_FSL_CORENET_RCPM_ADDR);
	volatile ccsr_pic_t *pic =
		(void *)(CONFIG_SYS_MPC8xxx_PIC_ADDR);
	u32 whoami = in_be32(&pic->whoami);

	/* Enable the timebase register for this core */
	out_be32(&rcpm->ctbenrl, (1 << whoami));
}
#endif

void cpu_init_f (void)
{
	extern void m8560_cpm_reset (void);
#ifdef CONFIG_SYS_DCSRBAR_PHYS
	ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);
#endif
#if defined(CONFIG_SECURE_BOOT)
	struct law_entry law;
#endif
#ifdef CONFIG_MPC8548
	ccsr_local_ecm_t *ecm = (void *)(CONFIG_SYS_MPC85xx_ECM_ADDR);
	uint svr = get_svr();

	/*
	 * CPU2 errata workaround: A core hang possible while executing
	 * a msync instruction and a snoopable transaction from an I/O
	 * master tagged to make quick forward progress is present.
	 * Fixed in silicon rev 2.1.
	 */
	if ((SVR_MAJ(svr) == 1) || ((SVR_MAJ(svr) == 2 && SVR_MIN(svr) == 0x0)))
		out_be32(&ecm->eebpcr, in_be32(&ecm->eebpcr) | (1 << 16));
#endif

	disable_tlb(14);
	disable_tlb(15);

#if defined(CONFIG_SECURE_BOOT)
	/* Disable the LAW created for NOR flash by the PBI commands */
	law = find_law(CONFIG_SYS_PBI_FLASH_BASE);
	if (law.index != -1)
		disable_law(law.index);
#endif

#ifdef CONFIG_CPM2
	config_8560_ioports((ccsr_cpm_t *)CONFIG_SYS_MPC85xx_CPM_ADDR);
#endif

       init_early_memctl_regs();

#if defined(CONFIG_CPM2)
	m8560_cpm_reset();
#endif
#ifdef CONFIG_QE
	/* Config QE ioports */
	config_qe_ioports();
#endif
#if defined(CONFIG_FSL_DMA)
	dma_init();
#endif
#ifdef CONFIG_FSL_CORENET
	corenet_tb_init();
#endif
	init_used_tlb_cams();

	/* Invalidate the CPC before DDR gets enabled */
	invalidate_cpc();

 #ifdef CONFIG_SYS_DCSRBAR_PHYS
	/* set DCSRCR so that DCSR space is 1G */
	setbits_be32(&gur->dcsrcr, FSL_CORENET_DCSR_SZ_1G);
	in_be32(&gur->dcsrcr);
#endif

}

/* Implement a dummy function for those platforms w/o SERDES */
static void __fsl_serdes__init(void)
{
	return ;
}
__attribute__((weak, alias("__fsl_serdes__init"))) void fsl_serdes_init(void);

/*
 * Initialize L2 as cache.
 *
 * The newer 8548, etc, parts have twice as much cache, but
 * use the same bit-encoding as the older 8555, etc, parts.
 *
 */
int cpu_init_r(void)
{
	__maybe_unused u32 svr = get_svr();
#ifdef CONFIG_SYS_LBC_LCRR
	volatile fsl_lbc_t *lbc = LBC_BASE_ADDR;
#endif

#if defined(CONFIG_SYS_P4080_ERRATUM_CPU22) || \
	defined(CONFIG_SYS_FSL_ERRATUM_NMG_CPU_A011)
	/*
	 * CPU22 applies to P4080 rev 1.0, 2.0, fixed in 3.0
	 * NMG_CPU_A011 applies to P4080 rev 1.0, 2.0, fixed in 3.0
	 * also applies to P3041 rev 1.0, 1.1, P2041 rev 1.0, 1.1
	 */
	if (SVR_SOC_VER(svr) != SVR_P4080 || SVR_MAJ(svr) < 3) {
		flush_dcache();
		mtspr(L1CSR2, (mfspr(L1CSR2) | L1CSR2_DCWS));
		sync();
	}
#endif

	puts ("L2:    ");

#if defined(CONFIG_L2_CACHE)
	volatile ccsr_l2cache_t *l2cache = (void *)CONFIG_SYS_MPC85xx_L2_ADDR;
	volatile uint cache_ctl;
	uint ver;
	u32 l2siz_field;

	ver = SVR_SOC_VER(svr);

	asm("msync;isync");
	cache_ctl = l2cache->l2ctl;

#if defined(CONFIG_SYS_RAMBOOT) && defined(CONFIG_SYS_INIT_L2_ADDR)
	if (cache_ctl & MPC85xx_L2CTL_L2E) {
		/* Clear L2 SRAM memory-mapped base address */
		out_be32(&l2cache->l2srbar0, 0x0);
		out_be32(&l2cache->l2srbar1, 0x0);

		/* set MBECCDIS=0, SBECCDIS=0 */
		clrbits_be32(&l2cache->l2errdis,
				(MPC85xx_L2ERRDIS_MBECC |
				 MPC85xx_L2ERRDIS_SBECC));

		/* set L2E=0, L2SRAM=0 */
		clrbits_be32(&l2cache->l2ctl,
				(MPC85xx_L2CTL_L2E |
				 MPC85xx_L2CTL_L2SRAM_ENTIRE));
	}
#endif

	l2siz_field = (cache_ctl >> 28) & 0x3;

	switch (l2siz_field) {
	case 0x0:
		printf(" unknown size (0x%08x)\n", cache_ctl);
		return -1;
		break;
	case 0x1:
		if (ver == SVR_8540 || ver == SVR_8560   ||
		    ver == SVR_8541 || ver == SVR_8555) {
			puts("128 KB ");
			/* set L2E=1, L2I=1, & L2BLKSZ=1 (128 Kbyte) */
			cache_ctl = 0xc4000000;
		} else {
			puts("256 KB ");
			cache_ctl = 0xc0000000; /* set L2E=1, L2I=1, & L2SRAM=0 */
		}
		break;
	case 0x2:
		if (ver == SVR_8540 || ver == SVR_8560   ||
		    ver == SVR_8541 || ver == SVR_8555) {
			puts("256 KB ");
			/* set L2E=1, L2I=1, & L2BLKSZ=2 (256 Kbyte) */
			cache_ctl = 0xc8000000;
		} else {
			puts ("512 KB ");
			/* set L2E=1, L2I=1, & L2SRAM=0 */
			cache_ctl = 0xc0000000;
		}
		break;
	case 0x3:
		puts("1024 KB ");
		/* set L2E=1, L2I=1, & L2SRAM=0 */
		cache_ctl = 0xc0000000;
		break;
	}

	if (l2cache->l2ctl & MPC85xx_L2CTL_L2E) {
		puts("already enabled");
#if defined(CONFIG_SYS_INIT_L2_ADDR) && defined(CONFIG_SYS_FLASH_BASE)
		u32 l2srbar = l2cache->l2srbar0;
		if (l2cache->l2ctl & MPC85xx_L2CTL_L2SRAM_ENTIRE
				&& l2srbar >= CONFIG_SYS_FLASH_BASE) {
			l2srbar = CONFIG_SYS_INIT_L2_ADDR;
			l2cache->l2srbar0 = l2srbar;
			printf("moving to 0x%08x", CONFIG_SYS_INIT_L2_ADDR);
		}
#endif /* CONFIG_SYS_INIT_L2_ADDR */
		puts("\n");
	} else {
		asm("msync;isync");
		l2cache->l2ctl = cache_ctl; /* invalidate & enable */
		asm("msync;isync");
		puts("enabled\n");
	}
#elif defined(CONFIG_BACKSIDE_L2_CACHE)
	if (SVR_SOC_VER(svr) == SVR_P2040) {
		puts("N/A\n");
		goto skip_l2;
	}

	u32 l2cfg0 = mfspr(SPRN_L2CFG0);

	/* invalidate the L2 cache */
	mtspr(SPRN_L2CSR0, (L2CSR0_L2FI|L2CSR0_L2LFC));
	while (mfspr(SPRN_L2CSR0) & (L2CSR0_L2FI|L2CSR0_L2LFC))
		;

#ifdef CONFIG_SYS_CACHE_STASHING
	/* set stash id to (coreID) * 2 + 32 + L2 (1) */
	mtspr(SPRN_L2CSR1, (32 + 1));
#endif

	/* enable the cache */
	mtspr(SPRN_L2CSR0, CONFIG_SYS_INIT_L2CSR0);

	if (CONFIG_SYS_INIT_L2CSR0 & L2CSR0_L2E) {
		while (!(mfspr(SPRN_L2CSR0) & L2CSR0_L2E))
			;
		printf("%d KB enabled\n", (l2cfg0 & 0x3fff) * 64);
	}

skip_l2:
#else
	puts("disabled\n");
#endif

	enable_cpc();

	/* needs to be in ram since code uses global static vars */
	fsl_serdes_init();

#ifdef CONFIG_SYS_SRIO
	srio_init();
#ifdef CONFIG_SRIOBOOT_MASTER
	srio_boot_master();
#ifdef CONFIG_SRIOBOOT_SLAVE_HOLDOFF
	srio_boot_master_release_slave();
#endif
#endif
#endif

#if defined(CONFIG_MP)
	setup_mp();
#endif

#ifdef CONFIG_SYS_FSL_ERRATUM_ESDHC136
	{
		void *p;
		p = (void *)CONFIG_SYS_DCSRBAR + 0x20520;
		setbits_be32(p, 1 << (31 - 14));
	}
#endif

#ifdef CONFIG_SYS_LBC_LCRR
	/*
	 * Modify the CLKDIV field of LCRR register to improve the writing
	 * speed for NOR flash.
	 */
	clrsetbits_be32(&lbc->lcrr, LCRR_CLKDIV, CONFIG_SYS_LBC_LCRR);
	__raw_readl(&lbc->lcrr);
	isync();
#ifdef CONFIG_SYS_FSL_ERRATUM_NMG_LBC103
	udelay(100);
#endif
#endif

#ifdef CONFIG_SYS_FSL_USB1_PHY_ENABLE
	{
		ccsr_usb_phy_t *usb_phy1 =
			(void *)CONFIG_SYS_MPC85xx_USB1_PHY_ADDR;
		out_be32(&usb_phy1->usb_enable_override,
				CONFIG_SYS_FSL_USB_ENABLE_OVERRIDE);
	}
#endif
#ifdef CONFIG_SYS_FSL_USB2_PHY_ENABLE
	{
		ccsr_usb_phy_t *usb_phy2 =
			(void *)CONFIG_SYS_MPC85xx_USB2_PHY_ADDR;
		out_be32(&usb_phy2->usb_enable_override,
				CONFIG_SYS_FSL_USB_ENABLE_OVERRIDE);
	}
#endif

#ifdef CONFIG_FMAN_ENET
	fman_enet_init();
#endif

#if defined(CONFIG_FSL_SATA_V2) && defined(CONFIG_FSL_SATA_ERRATUM_A001)
	/*
	 * For P1022/1013 Rev1.0 silicon, after power on SATA host
	 * controller is configured in legacy mode instead of the
	 * expected enterprise mode. Software needs to clear bit[28]
	 * of HControl register to change to enterprise mode from
	 * legacy mode.  We assume that the controller is offline.
	 */
	if (IS_SVR_REV(svr, 1, 0) &&
	    ((SVR_SOC_VER(svr) == SVR_P1022) ||
	     (SVR_SOC_VER(svr) == SVR_P1013))) {
		fsl_sata_reg_t *reg;

		/* first SATA controller */
		reg = (void *)CONFIG_SYS_MPC85xx_SATA1_ADDR;
		clrbits_le32(&reg->hcontrol, HCONTROL_ENTERPRISE_EN);

		/* second SATA controller */
		reg = (void *)CONFIG_SYS_MPC85xx_SATA2_ADDR;
		clrbits_le32(&reg->hcontrol, HCONTROL_ENTERPRISE_EN);
	}
#endif


	return 0;
}

extern void setup_ivors(void);

void arch_preboot_os(void)
{
	u32 msr;

	/*
	 * We are changing interrupt offsets and are about to boot the OS so
	 * we need to make sure we disable all async interrupts. EE is already
	 * disabled by the time we get called.
	 */
	msr = mfmsr();
	msr &= ~(MSR_ME|MSR_CE);
	mtmsr(msr);

	setup_ivors();
}

#if defined(CONFIG_CMD_SATA) && defined(CONFIG_FSL_SATA)
int sata_initialize(void)
{
	if (is_serdes_configured(SATA1) || is_serdes_configured(SATA2))
		return __sata_initialize();

	return 1;
}
#endif

void cpu_secondary_init_r(void)
{
#ifdef CONFIG_QE
	uint qe_base = CONFIG_SYS_IMMR + 0x00080000; /* QE immr base */
#ifdef CONFIG_SYS_QE_FMAN_FW_IN_NAND
	int ret;
	size_t fw_length = CONFIG_SYS_QE_FMAN_FW_LENGTH;

	/* load QE firmware from NAND flash to DDR first */
	ret = nand_read(&nand_info[0], (loff_t)CONFIG_SYS_QE_FMAN_FW_IN_NAND,
			&fw_length, (u_char *)CONFIG_SYS_QE_FMAN_FW_ADDR);

	if (ret && ret == -EUCLEAN) {
		printf ("NAND read for QE firmware at offset %x failed %d\n",
				CONFIG_SYS_QE_FMAN_FW_IN_NAND, ret);
	}
#endif
	qe_init(qe_base);
	qe_reset();
#endif
}
