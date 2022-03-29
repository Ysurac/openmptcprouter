/*
 * Freescale i.MX28 GPMI Register Definitions
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

#ifndef __MX28_REGS_GPMI_H__
#define __MX28_REGS_GPMI_H__

#include <asm/arch/regs-common.h>

#ifndef	__ASSEMBLY__
struct mx28_gpmi_regs {
	mx28_reg_32(hw_gpmi_ctrl0)
	mx28_reg_32(hw_gpmi_compare)
	mx28_reg_32(hw_gpmi_eccctrl)
	mx28_reg_32(hw_gpmi_ecccount)
	mx28_reg_32(hw_gpmi_payload)
	mx28_reg_32(hw_gpmi_auxiliary)
	mx28_reg_32(hw_gpmi_ctrl1)
	mx28_reg_32(hw_gpmi_timing0)
	mx28_reg_32(hw_gpmi_timing1)

	uint32_t	reserved[4];

	mx28_reg_32(hw_gpmi_data)
	mx28_reg_32(hw_gpmi_stat)
	mx28_reg_32(hw_gpmi_debug)
	mx28_reg_32(hw_gpmi_version)
};
#endif

#define	GPMI_CTRL0_SFTRST				(1 << 31)
#define	GPMI_CTRL0_CLKGATE				(1 << 30)
#define	GPMI_CTRL0_RUN					(1 << 29)
#define	GPMI_CTRL0_DEV_IRQ_EN				(1 << 28)
#define	GPMI_CTRL0_LOCK_CS				(1 << 27)
#define	GPMI_CTRL0_UDMA					(1 << 26)
#define	GPMI_CTRL0_COMMAND_MODE_MASK			(0x3 << 24)
#define	GPMI_CTRL0_COMMAND_MODE_OFFSET			24
#define	GPMI_CTRL0_COMMAND_MODE_WRITE			(0x0 << 24)
#define	GPMI_CTRL0_COMMAND_MODE_READ			(0x1 << 24)
#define	GPMI_CTRL0_COMMAND_MODE_READ_AND_COMPARE	(0x2 << 24)
#define	GPMI_CTRL0_COMMAND_MODE_WAIT_FOR_READY		(0x3 << 24)
#define	GPMI_CTRL0_WORD_LENGTH				(1 << 23)
#define	GPMI_CTRL0_CS_MASK				(0x7 << 20)
#define	GPMI_CTRL0_CS_OFFSET				20
#define	GPMI_CTRL0_ADDRESS_MASK				(0x7 << 17)
#define	GPMI_CTRL0_ADDRESS_OFFSET			17
#define	GPMI_CTRL0_ADDRESS_NAND_DATA			(0x0 << 17)
#define	GPMI_CTRL0_ADDRESS_NAND_CLE			(0x1 << 17)
#define	GPMI_CTRL0_ADDRESS_NAND_ALE			(0x2 << 17)
#define	GPMI_CTRL0_ADDRESS_INCREMENT			(1 << 16)
#define	GPMI_CTRL0_XFER_COUNT_MASK			0xffff
#define	GPMI_CTRL0_XFER_COUNT_OFFSET			0

#define	GPMI_COMPARE_MASK_MASK				(0xffff << 16)
#define	GPMI_COMPARE_MASK_OFFSET			16
#define	GPMI_COMPARE_REFERENCE_MASK			0xffff
#define	GPMI_COMPARE_REFERENCE_OFFSET			0

#define	GPMI_ECCCTRL_HANDLE_MASK			(0xffff << 16)
#define	GPMI_ECCCTRL_HANDLE_OFFSET			16
#define	GPMI_ECCCTRL_ECC_CMD_MASK			(0x3 << 13)
#define	GPMI_ECCCTRL_ECC_CMD_OFFSET			13
#define	GPMI_ECCCTRL_ECC_CMD_DECODE			(0x0 << 13)
#define	GPMI_ECCCTRL_ECC_CMD_ENCODE			(0x1 << 13)
#define	GPMI_ECCCTRL_ENABLE_ECC				(1 << 12)
#define	GPMI_ECCCTRL_BUFFER_MASK_MASK			0x1ff
#define	GPMI_ECCCTRL_BUFFER_MASK_OFFSET			0
#define	GPMI_ECCCTRL_BUFFER_MASK_BCH_AUXONLY		0x100
#define	GPMI_ECCCTRL_BUFFER_MASK_BCH_PAGE		0x1ff

#define	GPMI_ECCCOUNT_COUNT_MASK			0xffff
#define	GPMI_ECCCOUNT_COUNT_OFFSET			0

#define	GPMI_PAYLOAD_ADDRESS_MASK			(0x3fffffff << 2)
#define	GPMI_PAYLOAD_ADDRESS_OFFSET			2

#define	GPMI_AUXILIARY_ADDRESS_MASK			(0x3fffffff << 2)
#define	GPMI_AUXILIARY_ADDRESS_OFFSET			2

#define	GPMI_CTRL1_DECOUPLE_CS				(1 << 24)
#define	GPMI_CTRL1_WRN_DLY_SEL_MASK			(0x3 << 22)
#define	GPMI_CTRL1_WRN_DLY_SEL_OFFSET			22
#define	GPMI_CTRL1_TIMEOUT_IRQ_EN			(1 << 20)
#define	GPMI_CTRL1_GANGED_RDYBUSY			(1 << 19)
#define	GPMI_CTRL1_BCH_MODE				(1 << 18)
#define	GPMI_CTRL1_DLL_ENABLE				(1 << 17)
#define	GPMI_CTRL1_HALF_PERIOD				(1 << 16)
#define	GPMI_CTRL1_RDN_DELAY_MASK			(0xf << 12)
#define	GPMI_CTRL1_RDN_DELAY_OFFSET			12
#define	GPMI_CTRL1_DMA2ECC_MODE				(1 << 11)
#define	GPMI_CTRL1_DEV_IRQ				(1 << 10)
#define	GPMI_CTRL1_TIMEOUT_IRQ				(1 << 9)
#define	GPMI_CTRL1_BURST_EN				(1 << 8)
#define	GPMI_CTRL1_ABORT_WAIT_REQUEST			(1 << 7)
#define	GPMI_CTRL1_ABORT_WAIT_FOR_READY_CHANNEL_MASK	(0x7 << 4)
#define	GPMI_CTRL1_ABORT_WAIT_FOR_READY_CHANNEL_OFFSET	4
#define	GPMI_CTRL1_DEV_RESET				(1 << 3)
#define	GPMI_CTRL1_ATA_IRQRDY_POLARITY			(1 << 2)
#define	GPMI_CTRL1_CAMERA_MODE				(1 << 1)
#define	GPMI_CTRL1_GPMI_MODE				(1 << 0)

#define	GPMI_TIMING0_ADDRESS_SETUP_MASK			(0xff << 16)
#define	GPMI_TIMING0_ADDRESS_SETUP_OFFSET		16
#define	GPMI_TIMING0_DATA_HOLD_MASK			(0xff << 8)
#define	GPMI_TIMING0_DATA_HOLD_OFFSET			8
#define	GPMI_TIMING0_DATA_SETUP_MASK			0xff
#define	GPMI_TIMING0_DATA_SETUP_OFFSET			0

#define	GPMI_TIMING1_DEVICE_BUSY_TIMEOUT_MASK		(0xffff << 16)
#define	GPMI_TIMING1_DEVICE_BUSY_TIMEOUT_OFFSET		16

#define	GPMI_TIMING2_UDMA_TRP_MASK			(0xff << 24)
#define	GPMI_TIMING2_UDMA_TRP_OFFSET			24
#define	GPMI_TIMING2_UDMA_ENV_MASK			(0xff << 16)
#define	GPMI_TIMING2_UDMA_ENV_OFFSET			16
#define	GPMI_TIMING2_UDMA_HOLD_MASK			(0xff << 8)
#define	GPMI_TIMING2_UDMA_HOLD_OFFSET			8
#define	GPMI_TIMING2_UDMA_SETUP_MASK			0xff
#define	GPMI_TIMING2_UDMA_SETUP_OFFSET			0

#define	GPMI_DATA_DATA_MASK				0xffffffff
#define	GPMI_DATA_DATA_OFFSET				0

#define	GPMI_STAT_READY_BUSY_MASK			(0xff << 24)
#define	GPMI_STAT_READY_BUSY_OFFSET			24
#define	GPMI_STAT_RDY_TIMEOUT_MASK			(0xff << 16)
#define	GPMI_STAT_RDY_TIMEOUT_OFFSET			16
#define	GPMI_STAT_DEV7_ERROR				(1 << 15)
#define	GPMI_STAT_DEV6_ERROR				(1 << 14)
#define	GPMI_STAT_DEV5_ERROR				(1 << 13)
#define	GPMI_STAT_DEV4_ERROR				(1 << 12)
#define	GPMI_STAT_DEV3_ERROR				(1 << 11)
#define	GPMI_STAT_DEV2_ERROR				(1 << 10)
#define	GPMI_STAT_DEV1_ERROR				(1 << 9)
#define	GPMI_STAT_DEV0_ERROR				(1 << 8)
#define	GPMI_STAT_ATA_IRQ				(1 << 4)
#define	GPMI_STAT_INVALID_BUFFER_MASK			(1 << 3)
#define	GPMI_STAT_FIFO_EMPTY				(1 << 2)
#define	GPMI_STAT_FIFO_FULL				(1 << 1)
#define	GPMI_STAT_PRESENT				(1 << 0)

#define	GPMI_DEBUG_WAIT_FOR_READY_END_MASK		(0xff << 24)
#define	GPMI_DEBUG_WAIT_FOR_READY_END_OFFSET		24
#define	GPMI_DEBUG_DMA_SENSE_MASK			(0xff << 16)
#define	GPMI_DEBUG_DMA_SENSE_OFFSET			16
#define	GPMI_DEBUG_DMAREQ_MASK				(0xff << 8)
#define	GPMI_DEBUG_DMAREQ_OFFSET			8
#define	GPMI_DEBUG_CMD_END_MASK				0xff
#define	GPMI_DEBUG_CMD_END_OFFSET			0

#define	GPMI_VERSION_MAJOR_MASK				(0xff << 24)
#define	GPMI_VERSION_MAJOR_OFFSET			24
#define	GPMI_VERSION_MINOR_MASK				(0xff << 16)
#define	GPMI_VERSION_MINOR_OFFSET			16
#define	GPMI_VERSION_STEP_MASK				0xffff
#define	GPMI_VERSION_STEP_OFFSET			0

#define	GPMI_DEBUG2_UDMA_STATE_MASK			(0xf << 24)
#define	GPMI_DEBUG2_UDMA_STATE_OFFSET			24
#define	GPMI_DEBUG2_BUSY				(1 << 23)
#define	GPMI_DEBUG2_PIN_STATE_MASK			(0x7 << 20)
#define	GPMI_DEBUG2_PIN_STATE_OFFSET			20
#define	GPMI_DEBUG2_PIN_STATE_PSM_IDLE			(0x0 << 20)
#define	GPMI_DEBUG2_PIN_STATE_PSM_BYTCNT		(0x1 << 20)
#define	GPMI_DEBUG2_PIN_STATE_PSM_ADDR			(0x2 << 20)
#define	GPMI_DEBUG2_PIN_STATE_PSM_STALL			(0x3 << 20)
#define	GPMI_DEBUG2_PIN_STATE_PSM_STROBE		(0x4 << 20)
#define	GPMI_DEBUG2_PIN_STATE_PSM_ATARDY		(0x5 << 20)
#define	GPMI_DEBUG2_PIN_STATE_PSM_DHOLD			(0x6 << 20)
#define	GPMI_DEBUG2_PIN_STATE_PSM_DONE			(0x7 << 20)
#define	GPMI_DEBUG2_MAIN_STATE_MASK			(0xf << 16)
#define	GPMI_DEBUG2_MAIN_STATE_OFFSET			16
#define	GPMI_DEBUG2_MAIN_STATE_MSM_IDLE			(0x0 << 16)
#define	GPMI_DEBUG2_MAIN_STATE_MSM_BYTCNT		(0x1 << 16)
#define	GPMI_DEBUG2_MAIN_STATE_MSM_WAITFE		(0x2 << 16)
#define	GPMI_DEBUG2_MAIN_STATE_MSM_WAITFR		(0x3 << 16)
#define	GPMI_DEBUG2_MAIN_STATE_MSM_DMAREQ		(0x4 << 16)
#define	GPMI_DEBUG2_MAIN_STATE_MSM_DMAACK		(0x5 << 16)
#define	GPMI_DEBUG2_MAIN_STATE_MSM_WAITFF		(0x6 << 16)
#define	GPMI_DEBUG2_MAIN_STATE_MSM_LDFIFO		(0x7 << 16)
#define	GPMI_DEBUG2_MAIN_STATE_MSM_LDDMAR		(0x8 << 16)
#define	GPMI_DEBUG2_MAIN_STATE_MSM_RDCMP		(0x9 << 16)
#define	GPMI_DEBUG2_MAIN_STATE_MSM_DONE			(0xa << 16)
#define	GPMI_DEBUG2_SYND2GPMI_BE_MASK			(0xf << 12)
#define	GPMI_DEBUG2_SYND2GPMI_BE_OFFSET			12
#define	GPMI_DEBUG2_GPMI2SYND_VALID			(1 << 11)
#define	GPMI_DEBUG2_GPMI2SYND_READY			(1 << 10)
#define	GPMI_DEBUG2_SYND2GPMI_VALID			(1 << 9)
#define	GPMI_DEBUG2_SYND2GPMI_READY			(1 << 8)
#define	GPMI_DEBUG2_VIEW_DELAYED_RDN			(1 << 7)
#define	GPMI_DEBUG2_UPDATE_WINDOW			(1 << 6)
#define	GPMI_DEBUG2_RDN_TAP_MASK			0x3f
#define	GPMI_DEBUG2_RDN_TAP_OFFSET			0

#define	GPMI_DEBUG3_APB_WORD_CNTR_MASK			(0xffff << 16)
#define	GPMI_DEBUG3_APB_WORD_CNTR_OFFSET		16
#define	GPMI_DEBUG3_DEV_WORD_CNTR_MASK			0xffff
#define	GPMI_DEBUG3_DEV_WORD_CNTR_OFFSET		0

#endif	/* __MX28_REGS_GPMI_H__ */
