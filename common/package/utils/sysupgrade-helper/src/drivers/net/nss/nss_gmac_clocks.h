/* * Copyright (c) 2013 The Linux Foundation. All rights reserved.* */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __NSS_CLOCKS_H
#define __NSS_CLOCKS_H

#include <asm/io.h>
#include <asm/arch-ipq806x/iomap.h>
#include <asm/arch-ipq806x/nss/msm_ipq806x_gmac.h>
//#include <mach/msm_iomap.h>

/* Peripheral clock registers. */
#define NSS_ACC_REG				(0x28EC)
#define CLK_HALT_NSSFAB0_NSSFAB1_STATEA		(0x3C20)
#define GMAC_COREn_CLK_SRC_CTL(n)		(0x3CA0+(32*n))
#define GMAC_COREn_CLK_SRC0_MD(n)		(0x3CA4+(32*n))
#define GMAC_COREn_CLK_SRC1_MD(n)		(0x3CA8+(32*n))
#define GMAC_COREn_CLK_SRC0_NS(n)		(0x3CAC+(32*n))
#define GMAC_COREn_CLK_SRC1_NS(n)		(0x3CB0+(32*n))
#define GMAC_COREn_CLK_CTL(n)			(0x3CB4+(32*n))
#define GMAC_COREn_CLK_FS(n)			(0x3CB8+(32*n))
#define GMAC_COREn_RESET(n)			(0x3CBC+(32*n))
#define GMAC_AHB_RESET				(0x3E24)


#define GMAC_ACC_CUST_MASK		0xFF000000	/* Custom ACC fields for GMAC memories */
#define GMAC_FS_S_W_VAL			8		/* Wake and sleep counter value of
							   memory footswitch control. Assuming
							   max core frequency is 266MHz */
/* GMAC_COREn_CLK_SRC_CTL register bits */
#define GMAC_DUAL_MN8_SEL		0x00000001
#define GMAC_CLK_ROOT_ENA		0x00000002
#define GMAC_CLK_LOW_PWR_ENA		0x00000004

/* GMAC_COREn_CLK_SRC[0,1]_MD register bits (Assuming 133MHz) */
#define GMAC_CORE_CLK_M			0x32
#define GMAC_CORE_CLK_D			0		/* NOT(2*D) value */
#define GMAC_CORE_CLK_M_SHIFT		16
#define GMAC_CORE_CLK_D_SHIFT		0
#define GMAC_CORE_CLK_M_VAL		(GMAC_CORE_CLK_M << GMAC_CORE_CLK_M_SHIFT)
#define GMAC_CORE_CLK_D_VAL		(GMAC_CORE_CLK_D << GMAC_CORE_CLK_D_SHIFT)

/* GMAC_COREn_CLK_SRC[0,1]_NS register bits (Assuming 133MHz) */
#define GMAC_CORE_CLK_N			0x4		/* NOT(N-M) value, N=301 */
#define GMAC_CORE_CLK_N_SHIFT		16
#define GMAC_CORE_CLK_N_VAL		(GMAC_CORE_CLK_N << GMAC_CORE_CLK_N_SHIFT)
#define GMAC_CORE_CLK_MNCNTR_EN		0x00000100	/* Enable M/N counter */
#define GMAC_CORE_CLK_MNCNTR_RST	0x00000080	/* Activate reset for M/N counter */
#define GMAC_CORE_CLK_MNCNTR_MODE_MASK	0x00000060	/* M/N counter mode mask */
#define GMAC_CORE_CLK_MNCNTR_MODE_SHIFT	5
#define GMAC_CORE_CLK_MNCNTR_MODE_DUAL	(2 << GMAC_CORE_CLK_MNCNTR_MODE_SHIFT) /* M/N counter mode dual-edge */
#define GMAC_CORE_CLK_PRE_DIV_SEL_MASK	0x00000018	/* Pre divider select mask */
#define GMAC_CORE_CLK_PRE_DIV_SEL_SHIFT	3
#define GMAC_CORE_CLK_PRE_DIV_SEL_BYP	(0 << GMAC_CORE_CLK_PRE_DIV_SEL_SHIFT)	/* Pre divider bypass */
#define GMAC_CORE_CLK_SRC_SEL_MASK	0x00000007				/* clk source Mux select mask */
#define GMAC_CORE_CLK_SRC_SEL_SHIFT	0
#define GMAC_CORE_CLK_SRC_SEL_PLL0	(2 << GMAC_CORE_CLK_SRC_SEL_SHIFT)	/* output of clk source Mux is PLL0 */

/* CLK_HALT_NSSFAB0_NSSFAB1_STATEA register bits */
#define GMACn_CORE_CLK_HALT(x)		(0x0010 << x)

/* GMAC_COREn_CLK_CTL register bits */
#define GMAC_CLK_BRANCH_EN		0x0010
#define GMAC_CLK_INV			0x0020
#define GMAC_CLK_FABRIC_GATE_EN		0x0040

#endif /* __NSS_CLOCKS_H */
