/*
 * Copyright 2011 Freescale Semiconductor, Inc.
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
#include <asm/fsl_portals.h>
#include <asm/fsl_liodn.h>

#ifdef CONFIG_SYS_DPAA_QBMAN
struct qportal_info qp_info[CONFIG_SYS_QMAN_NUM_PORTALS] = {
	/* dqrr liodn, frame data liodn, liodn off, sdest */
	SET_QP_INFO(1, 2, 1, 0),
	SET_QP_INFO(3, 4, 2, 1),
	SET_QP_INFO(5, 6, 3, 2),
	SET_QP_INFO(7, 8, 4, 3),
	SET_QP_INFO(9, 10, 5, 0),
	SET_QP_INFO(11, 12, 1, 1),
	SET_QP_INFO(13, 14, 2, 2),
	SET_QP_INFO(15, 16, 3, 3),
	SET_QP_INFO(17, 18, 4, 0), /* for now sdest to 0 */
	SET_QP_INFO(19, 20, 5, 0), /* for now sdest to 0 */
};
#endif

struct srio_liodn_id_table srio_liodn_tbl[] = {
	SET_SRIO_LIODN_2(1, 199, 200),
	SET_SRIO_LIODN_2(2, 201, 202),
};
int srio_liodn_tbl_sz = ARRAY_SIZE(srio_liodn_tbl);

struct liodn_id_table liodn_tbl[] = {
#ifdef CONFIG_SYS_DPAA_QBMAN
	SET_QMAN_LIODN(31),
	SET_BMAN_LIODN(32),
#endif

	SET_SDHC_LIODN(1, 64),

	SET_PME_LIODN(117),

	SET_USB_LIODN(1, "fsl-usb2-mph", 125),
	SET_USB_LIODN(2, "fsl-usb2-dr", 126),

	SET_SATA_LIODN(1, 127),
	SET_SATA_LIODN(2, 128),

	SET_PCI_LIODN("fsl,qoriq-pcie-v2.2", 1, 193),
	SET_PCI_LIODN("fsl,qoriq-pcie-v2.2", 2, 194),
	SET_PCI_LIODN("fsl,qoriq-pcie-v2.2", 3, 195),

	SET_DMA_LIODN(1, 197),
	SET_DMA_LIODN(2, 198),

	SET_GUTS_LIODN("fsl,rapidio-delta", 199, rio1liodnr, 0),
	SET_GUTS_LIODN(NULL, 200, rio2liodnr, 0),
	SET_GUTS_LIODN(NULL, 201, rio1maintliodnr, 0),
	SET_GUTS_LIODN(NULL, 202, rio2maintliodnr, 0),
};
int liodn_tbl_sz = ARRAY_SIZE(liodn_tbl);

#ifdef CONFIG_SYS_DPAA_FMAN
struct liodn_id_table fman1_liodn_tbl[] = {
	SET_FMAN_RX_1G_LIODN(1, 0, 10),
	SET_FMAN_RX_1G_LIODN(1, 1, 11),
	SET_FMAN_RX_1G_LIODN(1, 2, 12),
	SET_FMAN_RX_1G_LIODN(1, 3, 13),
	SET_FMAN_RX_1G_LIODN(1, 4, 14),
#if (CONFIG_SYS_NUM_FM1_10GEC == 1)
	SET_FMAN_RX_10G_LIODN(1, 0, 15),
#endif
};
int fman1_liodn_tbl_sz = ARRAY_SIZE(fman1_liodn_tbl);
#endif

struct liodn_id_table sec_liodn_tbl[] = {
	SET_SEC_JR_LIODN_ENTRY(0, 129, 130),
	SET_SEC_JR_LIODN_ENTRY(1, 131, 132),
	SET_SEC_JR_LIODN_ENTRY(2, 133, 134),
	SET_SEC_JR_LIODN_ENTRY(3, 135, 136),
	SET_SEC_RTIC_LIODN_ENTRY(a, 154),
	SET_SEC_RTIC_LIODN_ENTRY(b, 155),
	SET_SEC_RTIC_LIODN_ENTRY(c, 156),
	SET_SEC_RTIC_LIODN_ENTRY(d, 157),
	SET_SEC_DECO_LIODN_ENTRY(0, 97, 98),
	SET_SEC_DECO_LIODN_ENTRY(1, 99, 100),
};
int sec_liodn_tbl_sz = ARRAY_SIZE(sec_liodn_tbl);

#ifdef CONFIG_SYS_DPAA_RMAN
struct liodn_id_table rman_liodn_tbl[] = {
	/* Set RMan block 0-3 liodn offset */
	SET_RMAN_LIODN(0, 6),
	SET_RMAN_LIODN(1, 7),
	SET_RMAN_LIODN(2, 8),
	SET_RMAN_LIODN(3, 9),
};
int rman_liodn_tbl_sz = ARRAY_SIZE(rman_liodn_tbl);
#endif

struct liodn_id_table liodn_bases[] = {
	[FSL_HW_PORTAL_SEC]  = SET_LIODN_BASE_2(64, 100),
#ifdef CONFIG_SYS_DPAA_FMAN
	[FSL_HW_PORTAL_FMAN1] = SET_LIODN_BASE_1(32),
#endif
#ifdef CONFIG_SYS_DPAA_PME
	[FSL_HW_PORTAL_PME]   = SET_LIODN_BASE_2(136, 172),
#endif
#ifdef CONFIG_SYS_DPAA_RMAN
	[FSL_HW_PORTAL_RMAN] = SET_LIODN_BASE_1(80),
#endif
};
