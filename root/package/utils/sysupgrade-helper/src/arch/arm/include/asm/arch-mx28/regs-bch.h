/*
 * Freescale i.MX28 BCH Register Definitions
 *
 * Copyright (C) 2011 Marek Vasut <marek.vasut@gmail.com>
 * on behalf of DENX Software Engineering GmbH
 *
 * Based on code from LTIB:
 * Copyright 2008-2010 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#ifndef __MX28_REGS_BCH_H__
#define __MX28_REGS_BCH_H__

#include <asm/arch/regs-common.h>

#ifndef	__ASSEMBLY__
struct mx28_bch_regs {
	mx28_reg_32(hw_bch_ctrl)
	mx28_reg_32(hw_bch_status0)
	mx28_reg_32(hw_bch_mode)
	mx28_reg_32(hw_bch_encodeptr)
	mx28_reg_32(hw_bch_dataptr)
	mx28_reg_32(hw_bch_metaptr)

	uint32_t	reserved[4];

	mx28_reg_32(hw_bch_layoutselect)
	mx28_reg_32(hw_bch_flash0layout0)
	mx28_reg_32(hw_bch_flash0layout1)
	mx28_reg_32(hw_bch_flash1layout0)
	mx28_reg_32(hw_bch_flash1layout1)
	mx28_reg_32(hw_bch_flash2layout0)
	mx28_reg_32(hw_bch_flash2layout1)
	mx28_reg_32(hw_bch_flash3layout0)
	mx28_reg_32(hw_bch_flash3layout1)
	mx28_reg_32(hw_bch_dbgkesread)
	mx28_reg_32(hw_bch_dbgcsferead)
	mx28_reg_32(hw_bch_dbgsyndegread)
	mx28_reg_32(hw_bch_dbgahbmread)
	mx28_reg_32(hw_bch_blockname)
	mx28_reg_32(hw_bch_version)
};
#endif

#define	BCH_CTRL_SFTRST					(1 << 31)
#define	BCH_CTRL_CLKGATE				(1 << 30)
#define	BCH_CTRL_DEBUGSYNDROME				(1 << 22)
#define	BCH_CTRL_M2M_LAYOUT_MASK			(0x3 << 18)
#define	BCH_CTRL_M2M_LAYOUT_OFFSET			18
#define	BCH_CTRL_M2M_ENCODE				(1 << 17)
#define	BCH_CTRL_M2M_ENABLE				(1 << 16)
#define	BCH_CTRL_DEBUG_STALL_IRQ_EN			(1 << 10)
#define	BCH_CTRL_COMPLETE_IRQ_EN			(1 << 8)
#define	BCH_CTRL_BM_ERROR_IRQ				(1 << 3)
#define	BCH_CTRL_DEBUG_STALL_IRQ			(1 << 2)
#define	BCH_CTRL_COMPLETE_IRQ				(1 << 0)

#define	BCH_STATUS0_HANDLE_MASK				(0xfff << 20)
#define	BCH_STATUS0_HANDLE_OFFSET			20
#define	BCH_STATUS0_COMPLETED_CE_MASK			(0xf << 16)
#define	BCH_STATUS0_COMPLETED_CE_OFFSET			16
#define	BCH_STATUS0_STATUS_BLK0_MASK			(0xff << 8)
#define	BCH_STATUS0_STATUS_BLK0_OFFSET			8
#define	BCH_STATUS0_STATUS_BLK0_ZERO			(0x00 << 8)
#define	BCH_STATUS0_STATUS_BLK0_ERROR1			(0x01 << 8)
#define	BCH_STATUS0_STATUS_BLK0_ERROR2			(0x02 << 8)
#define	BCH_STATUS0_STATUS_BLK0_ERROR3			(0x03 << 8)
#define	BCH_STATUS0_STATUS_BLK0_ERROR4			(0x04 << 8)
#define	BCH_STATUS0_STATUS_BLK0_UNCORRECTABLE		(0xfe << 8)
#define	BCH_STATUS0_STATUS_BLK0_ERASED			(0xff << 8)
#define	BCH_STATUS0_ALLONES				(1 << 4)
#define	BCH_STATUS0_CORRECTED				(1 << 3)
#define	BCH_STATUS0_UNCORRECTABLE			(1 << 2)

#define	BCH_MODE_ERASE_THRESHOLD_MASK			0xff
#define	BCH_MODE_ERASE_THRESHOLD_OFFSET			0

#define	BCH_ENCODEPTR_ADDR_MASK				0xffffffff
#define	BCH_ENCODEPTR_ADDR_OFFSET			0

#define	BCH_DATAPTR_ADDR_MASK				0xffffffff
#define	BCH_DATAPTR_ADDR_OFFSET				0

#define	BCH_METAPTR_ADDR_MASK				0xffffffff
#define	BCH_METAPTR_ADDR_OFFSET				0

#define	BCH_LAYOUTSELECT_CS15_SELECT_MASK		(0x3 << 30)
#define	BCH_LAYOUTSELECT_CS15_SELECT_OFFSET		30
#define	BCH_LAYOUTSELECT_CS14_SELECT_MASK		(0x3 << 28)
#define	BCH_LAYOUTSELECT_CS14_SELECT_OFFSET		28
#define	BCH_LAYOUTSELECT_CS13_SELECT_MASK		(0x3 << 26)
#define	BCH_LAYOUTSELECT_CS13_SELECT_OFFSET		26
#define	BCH_LAYOUTSELECT_CS12_SELECT_MASK		(0x3 << 24)
#define	BCH_LAYOUTSELECT_CS12_SELECT_OFFSET		24
#define	BCH_LAYOUTSELECT_CS11_SELECT_MASK		(0x3 << 22)
#define	BCH_LAYOUTSELECT_CS11_SELECT_OFFSET		22
#define	BCH_LAYOUTSELECT_CS10_SELECT_MASK		(0x3 << 20)
#define	BCH_LAYOUTSELECT_CS10_SELECT_OFFSET		20
#define	BCH_LAYOUTSELECT_CS9_SELECT_MASK		(0x3 << 18)
#define	BCH_LAYOUTSELECT_CS9_SELECT_OFFSET		18
#define	BCH_LAYOUTSELECT_CS8_SELECT_MASK		(0x3 << 16)
#define	BCH_LAYOUTSELECT_CS8_SELECT_OFFSET		16
#define	BCH_LAYOUTSELECT_CS7_SELECT_MASK		(0x3 << 14)
#define	BCH_LAYOUTSELECT_CS7_SELECT_OFFSET		14
#define	BCH_LAYOUTSELECT_CS6_SELECT_MASK		(0x3 << 12)
#define	BCH_LAYOUTSELECT_CS6_SELECT_OFFSET		12
#define	BCH_LAYOUTSELECT_CS5_SELECT_MASK		(0x3 << 10)
#define	BCH_LAYOUTSELECT_CS5_SELECT_OFFSET		10
#define	BCH_LAYOUTSELECT_CS4_SELECT_MASK		(0x3 << 8)
#define	BCH_LAYOUTSELECT_CS4_SELECT_OFFSET		8
#define	BCH_LAYOUTSELECT_CS3_SELECT_MASK		(0x3 << 6)
#define	BCH_LAYOUTSELECT_CS3_SELECT_OFFSET		6
#define	BCH_LAYOUTSELECT_CS2_SELECT_MASK		(0x3 << 4)
#define	BCH_LAYOUTSELECT_CS2_SELECT_OFFSET		4
#define	BCH_LAYOUTSELECT_CS1_SELECT_MASK		(0x3 << 2)
#define	BCH_LAYOUTSELECT_CS1_SELECT_OFFSET		2
#define	BCH_LAYOUTSELECT_CS0_SELECT_MASK		(0x3 << 0)
#define	BCH_LAYOUTSELECT_CS0_SELECT_OFFSET		0

#define	BCH_FLASHLAYOUT0_NBLOCKS_MASK			(0xff << 24)
#define	BCH_FLASHLAYOUT0_NBLOCKS_OFFSET			24
#define	BCH_FLASHLAYOUT0_META_SIZE_MASK			(0xff << 16)
#define	BCH_FLASHLAYOUT0_META_SIZE_OFFSET		16
#define	BCH_FLASHLAYOUT0_ECC0_MASK			(0xf << 12)
#define	BCH_FLASHLAYOUT0_ECC0_OFFSET			12
#define	BCH_FLASHLAYOUT0_ECC0_NONE			(0x0 << 12)
#define	BCH_FLASHLAYOUT0_ECC0_ECC2			(0x1 << 12)
#define	BCH_FLASHLAYOUT0_ECC0_ECC4			(0x2 << 12)
#define	BCH_FLASHLAYOUT0_ECC0_ECC6			(0x3 << 12)
#define	BCH_FLASHLAYOUT0_ECC0_ECC8			(0x4 << 12)
#define	BCH_FLASHLAYOUT0_ECC0_ECC10			(0x5 << 12)
#define	BCH_FLASHLAYOUT0_ECC0_ECC12			(0x6 << 12)
#define	BCH_FLASHLAYOUT0_ECC0_ECC14			(0x7 << 12)
#define	BCH_FLASHLAYOUT0_ECC0_ECC16			(0x8 << 12)
#define	BCH_FLASHLAYOUT0_ECC0_ECC18			(0x9 << 12)
#define	BCH_FLASHLAYOUT0_ECC0_ECC20			(0xa << 12)
#define	BCH_FLASHLAYOUT0_ECC0_ECC22			(0xb << 12)
#define	BCH_FLASHLAYOUT0_ECC0_ECC24			(0xc << 12)
#define	BCH_FLASHLAYOUT0_ECC0_ECC26			(0xd << 12)
#define	BCH_FLASHLAYOUT0_ECC0_ECC28			(0xe << 12)
#define	BCH_FLASHLAYOUT0_ECC0_ECC30			(0xf << 12)
#define	BCH_FLASHLAYOUT0_ECC0_ECC32			(0x10 << 12)
#define	BCH_FLASHLAYOUT0_GF13_0_GF14_1			(1 << 10)
#define	BCH_FLASHLAYOUT0_DATA0_SIZE_MASK		0xfff
#define	BCH_FLASHLAYOUT0_DATA0_SIZE_OFFSET		0

#define	BCH_FLASHLAYOUT1_PAGE_SIZE_MASK			(0xffff << 16)
#define	BCH_FLASHLAYOUT1_PAGE_SIZE_OFFSET		16
#define	BCH_FLASHLAYOUT1_ECCN_MASK			(0xf << 12)
#define	BCH_FLASHLAYOUT1_ECCN_OFFSET			12
#define	BCH_FLASHLAYOUT1_ECCN_NONE			(0x0 << 12)
#define	BCH_FLASHLAYOUT1_ECCN_ECC2			(0x1 << 12)
#define	BCH_FLASHLAYOUT1_ECCN_ECC4			(0x2 << 12)
#define	BCH_FLASHLAYOUT1_ECCN_ECC6			(0x3 << 12)
#define	BCH_FLASHLAYOUT1_ECCN_ECC8			(0x4 << 12)
#define	BCH_FLASHLAYOUT1_ECCN_ECC10			(0x5 << 12)
#define	BCH_FLASHLAYOUT1_ECCN_ECC12			(0x6 << 12)
#define	BCH_FLASHLAYOUT1_ECCN_ECC14			(0x7 << 12)
#define	BCH_FLASHLAYOUT1_ECCN_ECC16			(0x8 << 12)
#define	BCH_FLASHLAYOUT1_ECCN_ECC18			(0x9 << 12)
#define	BCH_FLASHLAYOUT1_ECCN_ECC20			(0xa << 12)
#define	BCH_FLASHLAYOUT1_ECCN_ECC22			(0xb << 12)
#define	BCH_FLASHLAYOUT1_ECCN_ECC24			(0xc << 12)
#define	BCH_FLASHLAYOUT1_ECCN_ECC26			(0xd << 12)
#define	BCH_FLASHLAYOUT1_ECCN_ECC28			(0xe << 12)
#define	BCH_FLASHLAYOUT1_ECCN_ECC30			(0xf << 12)
#define	BCH_FLASHLAYOUT1_ECCN_ECC32			(0x10 << 12)
#define	BCH_FLASHLAYOUT1_GF13_0_GF14_1			(1 << 10)
#define	BCH_FLASHLAYOUT1_DATAN_SIZE_MASK		0xfff
#define	BCH_FLASHLAYOUT1_DATAN_SIZE_OFFSET		0

#define	BCH_DEBUG0_RSVD1_MASK				(0x1f << 27)
#define	BCH_DEBUG0_RSVD1_OFFSET				27
#define	BCH_DEBUG0_ROM_BIST_ENABLE			(1 << 26)
#define	BCH_DEBUG0_ROM_BIST_COMPLETE			(1 << 25)
#define	BCH_DEBUG0_KES_DEBUG_SYNDROME_SYMBOL_MASK	(0x1ff << 16)
#define	BCH_DEBUG0_KES_DEBUG_SYNDROME_SYMBOL_OFFSET	16
#define	BCH_DEBUG0_KES_DEBUG_SYNDROME_SYMBOL_NORMAL	(0x0 << 16)
#define	BCH_DEBUG0_KES_DEBUG_SYNDROME_SYMBOL_TEST_MODE	(0x1 << 16)
#define	BCH_DEBUG0_KES_DEBUG_SHIFT_SYND			(1 << 15)
#define	BCH_DEBUG0_KES_DEBUG_PAYLOAD_FLAG		(1 << 14)
#define	BCH_DEBUG0_KES_DEBUG_MODE4K			(1 << 13)
#define	BCH_DEBUG0_KES_DEBUG_KICK			(1 << 12)
#define	BCH_DEBUG0_KES_STANDALONE			(1 << 11)
#define	BCH_DEBUG0_KES_DEBUG_STEP			(1 << 10)
#define	BCH_DEBUG0_KES_DEBUG_STALL			(1 << 9)
#define	BCH_DEBUG0_BM_KES_TEST_BYPASS			(1 << 8)
#define	BCH_DEBUG0_RSVD0_MASK				(0x3 << 6)
#define	BCH_DEBUG0_RSVD0_OFFSET				6
#define	BCH_DEBUG0_DEBUG_REG_SELECT_MASK		0x3f
#define	BCH_DEBUG0_DEBUG_REG_SELECT_OFFSET		0

#define	BCH_DBGKESREAD_VALUES_MASK			0xffffffff
#define	BCH_DBGKESREAD_VALUES_OFFSET			0

#define	BCH_DBGCSFEREAD_VALUES_MASK			0xffffffff
#define	BCH_DBGCSFEREAD_VALUES_OFFSET			0

#define	BCH_DBGSYNDGENREAD_VALUES_MASK			0xffffffff
#define	BCH_DBGSYNDGENREAD_VALUES_OFFSET		0

#define	BCH_DBGAHBMREAD_VALUES_MASK			0xffffffff
#define	BCH_DBGAHBMREAD_VALUES_OFFSET			0

#define	BCH_BLOCKNAME_NAME_MASK				0xffffffff
#define	BCH_BLOCKNAME_NAME_OFFSET			0

#define	BCH_VERSION_MAJOR_MASK				(0xff << 24)
#define	BCH_VERSION_MAJOR_OFFSET			24
#define	BCH_VERSION_MINOR_MASK				(0xff << 16)
#define	BCH_VERSION_MINOR_OFFSET			16
#define	BCH_VERSION_STEP_MASK				0xffff
#define	BCH_VERSION_STEP_OFFSET				0

#endif	/* __MX28_REGS_BCH_H__ */
