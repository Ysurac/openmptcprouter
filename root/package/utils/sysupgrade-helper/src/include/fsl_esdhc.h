/*
 * FSL SD/MMC Defines
 *-------------------------------------------------------------------
 *
 * Copyright 2007-2008,2010-2011 Freescale Semiconductor, Inc
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
 *
 *-------------------------------------------------------------------
 *
 */

#ifndef  __FSL_ESDHC_H__
#define	__FSL_ESDHC_H__

#include <asm/errno.h>
#include <asm/byteorder.h>

/* FSL eSDHC-specific constants */
#define SYSCTL			0x0002e02c
#define SYSCTL_INITA		0x08000000
#define SYSCTL_TIMEOUT_MASK	0x000f0000
#define SYSCTL_CLOCK_MASK	0x0000fff0
#define SYSCTL_CKEN		0x00000008
#define SYSCTL_PEREN		0x00000004
#define SYSCTL_HCKEN		0x00000002
#define SYSCTL_IPGEN		0x00000001
#define SYSCTL_RSTA		0x01000000
#define SYSCTL_RSTC		0x02000000
#define SYSCTL_RSTD		0x04000000

#define IRQSTAT			0x0002e030
#define IRQSTAT_DMAE		(0x10000000)
#define IRQSTAT_AC12E		(0x01000000)
#define IRQSTAT_DEBE		(0x00400000)
#define IRQSTAT_DCE		(0x00200000)
#define IRQSTAT_DTOE		(0x00100000)
#define IRQSTAT_CIE		(0x00080000)
#define IRQSTAT_CEBE		(0x00040000)
#define IRQSTAT_CCE		(0x00020000)
#define IRQSTAT_CTOE		(0x00010000)
#define IRQSTAT_CINT		(0x00000100)
#define IRQSTAT_CRM		(0x00000080)
#define IRQSTAT_CINS		(0x00000040)
#define IRQSTAT_BRR		(0x00000020)
#define IRQSTAT_BWR		(0x00000010)
#define IRQSTAT_DINT		(0x00000008)
#define IRQSTAT_BGE		(0x00000004)
#define IRQSTAT_TC		(0x00000002)
#define IRQSTAT_CC		(0x00000001)

#define CMD_ERR		(IRQSTAT_CIE | IRQSTAT_CEBE | IRQSTAT_CCE)
#define DATA_ERR	(IRQSTAT_DEBE | IRQSTAT_DCE | IRQSTAT_DTOE)

#define IRQSTATEN		0x0002e034
#define IRQSTATEN_DMAE		(0x10000000)
#define IRQSTATEN_AC12E		(0x01000000)
#define IRQSTATEN_DEBE		(0x00400000)
#define IRQSTATEN_DCE		(0x00200000)
#define IRQSTATEN_DTOE		(0x00100000)
#define IRQSTATEN_CIE		(0x00080000)
#define IRQSTATEN_CEBE		(0x00040000)
#define IRQSTATEN_CCE		(0x00020000)
#define IRQSTATEN_CTOE		(0x00010000)
#define IRQSTATEN_CINT		(0x00000100)
#define IRQSTATEN_CRM		(0x00000080)
#define IRQSTATEN_CINS		(0x00000040)
#define IRQSTATEN_BRR		(0x00000020)
#define IRQSTATEN_BWR		(0x00000010)
#define IRQSTATEN_DINT		(0x00000008)
#define IRQSTATEN_BGE		(0x00000004)
#define IRQSTATEN_TC		(0x00000002)
#define IRQSTATEN_CC		(0x00000001)

#define PRSSTAT			0x0002e024
#define PRSSTAT_DAT0		(0x01000000)
#define PRSSTAT_CLSL		(0x00800000)
#define PRSSTAT_WPSPL		(0x00080000)
#define PRSSTAT_CDPL		(0x00040000)
#define PRSSTAT_CINS		(0x00010000)
#define PRSSTAT_BREN		(0x00000800)
#define PRSSTAT_BWEN		(0x00000400)
#define PRSSTAT_DLA		(0x00000004)
#define PRSSTAT_CICHB		(0x00000002)
#define PRSSTAT_CIDHB		(0x00000001)

#define PROCTL			0x0002e028
#define PROCTL_INIT		0x00000020
#define PROCTL_DTW_4		0x00000002
#define PROCTL_DTW_8		0x00000004

#define CMDARG			0x0002e008

#define XFERTYP			0x0002e00c
#define XFERTYP_CMD(x)		((x & 0x3f) << 24)
#define XFERTYP_CMDTYP_NORMAL	0x0
#define XFERTYP_CMDTYP_SUSPEND	0x00400000
#define XFERTYP_CMDTYP_RESUME	0x00800000
#define XFERTYP_CMDTYP_ABORT	0x00c00000
#define XFERTYP_DPSEL		0x00200000
#define XFERTYP_CICEN		0x00100000
#define XFERTYP_CCCEN		0x00080000
#define XFERTYP_RSPTYP_NONE	0
#define XFERTYP_RSPTYP_136	0x00010000
#define XFERTYP_RSPTYP_48	0x00020000
#define XFERTYP_RSPTYP_48_BUSY	0x00030000
#define XFERTYP_MSBSEL		0x00000020
#define XFERTYP_DTDSEL		0x00000010
#define XFERTYP_AC12EN		0x00000004
#define XFERTYP_BCEN		0x00000002
#define XFERTYP_DMAEN		0x00000001

#define CINS_TIMEOUT		1000
#define PIO_TIMEOUT		100000

#define DSADDR		0x2e004

#define CMDRSP0		0x2e010
#define CMDRSP1		0x2e014
#define CMDRSP2		0x2e018
#define CMDRSP3		0x2e01c

#define DATPORT		0x2e020

#define WML		0x2e044
#define WML_WRITE	0x00010000
#ifdef CONFIG_FSL_SDHC_V2_3
#define WML_RD_WML_MAX		0x80
#define WML_WR_WML_MAX		0x80
#define WML_RD_WML_MAX_VAL	0x0
#define WML_WR_WML_MAX_VAL	0x0
#define WML_RD_WML_MASK		0x7f
#define WML_WR_WML_MASK		0x7f0000
#else
#define WML_RD_WML_MAX		0x10
#define WML_WR_WML_MAX		0x80
#define WML_RD_WML_MAX_VAL	0x10
#define WML_WR_WML_MAX_VAL	0x80
#define WML_RD_WML_MASK	0xff
#define WML_WR_WML_MASK	0xff0000
#endif

#define BLKATTR		0x2e004
#define BLKATTR_CNT(x)	((x & 0xffff) << 16)
#define BLKATTR_SIZE(x)	(x & 0x1fff)
#define MAX_BLK_CNT	0x7fff	/* so malloc will have enough room with 32M */

#define ESDHC_HOSTCAPBLT_VS18	0x04000000
#define ESDHC_HOSTCAPBLT_VS30	0x02000000
#define ESDHC_HOSTCAPBLT_VS33	0x01000000
#define ESDHC_HOSTCAPBLT_SRS	0x00800000
#define ESDHC_HOSTCAPBLT_DMAS	0x00400000
#define ESDHC_HOSTCAPBLT_HSS	0x00200000

struct fsl_esdhc_cfg {
	u32	esdhc_base;
	u32	no_snoop;
};

/* Select the correct accessors depending on endianess */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define esdhc_read32		in_le32
#define esdhc_write32		out_le32
#define esdhc_clrsetbits32	clrsetbits_le32
#define esdhc_clrbits32		clrbits_le32
#define esdhc_setbits32		setbits_le32
#elif __BYTE_ORDER == __BIG_ENDIAN
#define esdhc_read32		in_be32
#define esdhc_write32		out_be32
#define esdhc_clrsetbits32	clrsetbits_be32
#define esdhc_clrbits32		clrbits_be32
#define esdhc_setbits32		setbits_be32
#else
#error "Endianess is not defined: please fix to continue"
#endif

#ifdef CONFIG_FSL_ESDHC
int fsl_esdhc_mmc_init(bd_t *bis);
int fsl_esdhc_initialize(bd_t *bis, struct fsl_esdhc_cfg *cfg);
void fdt_fixup_esdhc(void *blob, bd_t *bd);
#else
static inline int fsl_esdhc_mmc_init(bd_t *bis) { return -ENOSYS; }
static inline void fdt_fixup_esdhc(void *blob, bd_t *bd) {}
#endif /* CONFIG_FSL_ESDHC */

#endif  /* __FSL_ESDHC_H__ */
