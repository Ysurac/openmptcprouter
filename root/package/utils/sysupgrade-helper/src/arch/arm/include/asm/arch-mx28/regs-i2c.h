/*
 * Freescale i.MX28 I2C Register Definitions
 *
 * Copyright (C) 2011 Marek Vasut <marek.vasut@gmail.com>
 * on behalf of DENX Software Engineering GmbH
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

#ifndef __MX28_REGS_I2C_H__
#define __MX28_REGS_I2C_H__

#include <asm/arch/regs-common.h>

#ifndef	__ASSEMBLY__
struct mx28_i2c_regs {
	mx28_reg_32(hw_i2c_ctrl0)
	mx28_reg_32(hw_i2c_timing0)
	mx28_reg_32(hw_i2c_timing1)
	mx28_reg_32(hw_i2c_timing2)
	mx28_reg_32(hw_i2c_ctrl1)
	mx28_reg_32(hw_i2c_stat)
	mx28_reg_32(hw_i2c_queuectrl)
	mx28_reg_32(hw_i2c_queuestat)
	mx28_reg_32(hw_i2c_queuecmd)
	mx28_reg_32(hw_i2c_queuedata)
	mx28_reg_32(hw_i2c_data)
	mx28_reg_32(hw_i2c_debug0)
	mx28_reg_32(hw_i2c_debug1)
	mx28_reg_32(hw_i2c_version)
};
#endif

#define	I2C_CTRL_SFTRST				(1 << 31)
#define	I2C_CTRL_CLKGATE			(1 << 30)
#define	I2C_CTRL_RUN				(1 << 29)
#define	I2C_CTRL_PREACK				(1 << 27)
#define	I2C_CTRL_ACKNOWLEDGE			(1 << 26)
#define	I2C_CTRL_SEND_NAK_ON_LAST		(1 << 25)
#define	I2C_CTRL_MULTI_MASTER			(1 << 23)
#define	I2C_CTRL_CLOCK_HELD			(1 << 22)
#define	I2C_CTRL_RETAIN_CLOCK			(1 << 21)
#define	I2C_CTRL_POST_SEND_STOP			(1 << 20)
#define	I2C_CTRL_PRE_SEND_START			(1 << 19)
#define	I2C_CTRL_SLAVE_ADDRESS_ENABLE		(1 << 18)
#define	I2C_CTRL_MASTER_MODE			(1 << 17)
#define	I2C_CTRL_DIRECTION			(1 << 16)
#define	I2C_CTRL_XFER_COUNT_MASK		0xffff
#define	I2C_CTRL_XFER_COUNT_OFFSET		0

#define	I2C_TIMING0_HIGH_COUNT_MASK		(0x3ff << 16)
#define	I2C_TIMING0_HIGH_COUNT_OFFSET		16
#define	I2C_TIMING0_RCV_COUNT_MASK		0x3ff
#define	I2C_TIMING0_RCV_COUNT_OFFSET		0

#define	I2C_TIMING1_LOW_COUNT_MASK		(0x3ff << 16)
#define	I2C_TIMING1_LOW_COUNT_OFFSET		16
#define	I2C_TIMING1_XMIT_COUNT_MASK		0x3ff
#define	I2C_TIMING1_XMIT_COUNT_OFFSET		0

#define	I2C_TIMING2_BUS_FREE_MASK		(0x3ff << 16)
#define	I2C_TIMING2_BUS_FREE_OFFSET		16
#define	I2C_TIMING2_LEADIN_COUNT_MASK		0x3ff
#define	I2C_TIMING2_LEADIN_COUNT_OFFSET		0

#define	I2C_CTRL1_RD_QUEUE_IRQ			(1 << 30)
#define	I2C_CTRL1_WR_QUEUE_IRQ			(1 << 29)
#define	I2C_CTRL1_CLR_GOT_A_NAK			(1 << 28)
#define	I2C_CTRL1_ACK_MODE			(1 << 27)
#define	I2C_CTRL1_FORCE_DATA_IDLE		(1 << 26)
#define	I2C_CTRL1_FORCE_CLK_IDLE		(1 << 25)
#define	I2C_CTRL1_BCAST_SLAVE_EN		(1 << 24)
#define	I2C_CTRL1_SLAVE_ADDRESS_BYTE_MASK	(0xff << 16)
#define	I2C_CTRL1_SLAVE_ADDRESS_BYTE_OFFSET	16
#define	I2C_CTRL1_BUS_FREE_IRQ_EN		(1 << 15)
#define	I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ_EN	(1 << 14)
#define	I2C_CTRL1_NO_SLAVE_ACK_IRQ_EN		(1 << 13)
#define	I2C_CTRL1_OVERSIZE_XFER_TERM_IRQ_EN	(1 << 12)
#define	I2C_CTRL1_EARLY_TERM_IRQ_EN		(1 << 11)
#define	I2C_CTRL1_MASTER_LOSS_IRQ_EN		(1 << 10)
#define	I2C_CTRL1_SLAVE_STOP_IRQ_EN		(1 << 9)
#define	I2C_CTRL1_SLAVE_IRQ_EN			(1 << 8)
#define	I2C_CTRL1_BUS_FREE_IRQ			(1 << 7)
#define	I2C_CTRL1_DATA_ENGINE_CMPLT_IRQ		(1 << 6)
#define	I2C_CTRL1_NO_SLAVE_ACK_IRQ		(1 << 5)
#define	I2C_CTRL1_OVERSIZE_XFER_TERM_IRQ	(1 << 4)
#define	I2C_CTRL1_EARLY_TERM_IRQ		(1 << 3)
#define	I2C_CTRL1_MASTER_LOSS_IRQ		(1 << 2)
#define	I2C_CTRL1_SLAVE_STOP_IRQ		(1 << 1)
#define	I2C_CTRL1_SLAVE_IRQ			(1 << 0)

#define	I2C_STAT_MASTER_PRESENT			(1 << 31)
#define	I2C_STAT_SLAVE_PRESENT			(1 << 30)
#define	I2C_STAT_ANY_ENABLED_IRQ		(1 << 29)
#define	I2C_STAT_GOT_A_NAK			(1 << 28)
#define	I2C_STAT_RCVD_SLAVE_ADDR_MASK		(0xff << 16)
#define	I2C_STAT_RCVD_SLAVE_ADDR_OFFSET		16
#define	I2C_STAT_SLAVE_ADDR_EQ_ZERO		(1 << 15)
#define	I2C_STAT_SLAVE_FOUND			(1 << 14)
#define	I2C_STAT_SLAVE_SEARCHING		(1 << 13)
#define	I2C_STAT_DATA_ENGING_DMA_WAIT		(1 << 12)
#define	I2C_STAT_BUS_BUSY			(1 << 11)
#define	I2C_STAT_CLK_GEN_BUSY			(1 << 10)
#define	I2C_STAT_DATA_ENGINE_BUSY		(1 << 9)
#define	I2C_STAT_SLAVE_BUSY			(1 << 8)
#define	I2C_STAT_BUS_FREE_IRQ_SUMMARY		(1 << 7)
#define	I2C_STAT_DATA_ENGINE_CMPLT_IRQ_SUMMARY	(1 << 6)
#define	I2C_STAT_NO_SLAVE_ACK_IRQ_SUMMARY	(1 << 5)
#define	I2C_STAT_OVERSIZE_XFER_TERM_IRQ_SUMMARY	(1 << 4)
#define	I2C_STAT_EARLY_TERM_IRQ_SUMMARY		(1 << 3)
#define	I2C_STAT_MASTER_LOSS_IRQ_SUMMARY	(1 << 2)
#define	I2C_STAT_SLAVE_STOP_IRQ_SUMMARY		(1 << 1)
#define	I2C_STAT_SLAVE_IRQ_SUMMARY		(1 << 0)

#define	I2C_QUEUECTRL_RD_THRESH_MASK		(0x1f << 16)
#define	I2C_QUEUECTRL_RD_THRESH_OFFSET		16
#define	I2C_QUEUECTRL_WR_THRESH_MASK		(0x1f << 8)
#define	I2C_QUEUECTRL_WR_THRESH_OFFSET		8
#define	I2C_QUEUECTRL_QUEUE_RUN			(1 << 5)
#define	I2C_QUEUECTRL_RD_CLEAR			(1 << 4)
#define	I2C_QUEUECTRL_WR_CLEAR			(1 << 3)
#define	I2C_QUEUECTRL_PIO_QUEUE_MODE		(1 << 2)
#define	I2C_QUEUECTRL_RD_QUEUE_IRQ_EN		(1 << 1)
#define	I2C_QUEUECTRL_WR_QUEUE_IRQ_EN		(1 << 0)

#define	I2C_QUEUESTAT_RD_QUEUE_FULL		(1 << 14)
#define	I2C_QUEUESTAT_RD_QUEUE_EMPTY		(1 << 13)
#define	I2C_QUEUESTAT_RD_QUEUE_CNT_MASK		(0x1f << 8)
#define	I2C_QUEUESTAT_RD_QUEUE_CNT_OFFSET	8
#define	I2C_QUEUESTAT_WR_QUEUE_FULL		(1 << 6)
#define	I2C_QUEUESTAT_WR_QUEUE_EMPTY		(1 << 5)
#define	I2C_QUEUESTAT_WR_QUEUE_CNT_MASK		0x1f
#define	I2C_QUEUESTAT_WR_QUEUE_CNT_OFFSET	0

#define	I2C_QUEUECMD_PREACK			(1 << 27)
#define	I2C_QUEUECMD_ACKNOWLEDGE		(1 << 26)
#define	I2C_QUEUECMD_SEND_NAK_ON_LAST		(1 << 25)
#define	I2C_QUEUECMD_MULTI_MASTER		(1 << 23)
#define	I2C_QUEUECMD_CLOCK_HELD			(1 << 22)
#define	I2C_QUEUECMD_RETAIN_CLOCK		(1 << 21)
#define	I2C_QUEUECMD_POST_SEND_STOP		(1 << 20)
#define	I2C_QUEUECMD_PRE_SEND_START		(1 << 19)
#define	I2C_QUEUECMD_SLAVE_ADDRESS_ENABLE	(1 << 18)
#define	I2C_QUEUECMD_MASTER_MODE		(1 << 17)
#define	I2C_QUEUECMD_DIRECTION			(1 << 16)
#define	I2C_QUEUECMD_XFER_COUNT_MASK		0xffff
#define	I2C_QUEUECMD_XFER_COUNT_OFFSET		0

#define	I2C_QUEUEDATA_DATA_MASK			0xffffffff
#define	I2C_QUEUEDATA_DATA_OFFSET		0

#define	I2C_DATA_DATA_MASK			0xffffffff
#define	I2C_DATA_DATA_OFFSET			0

#define	I2C_DEBUG0_DMAREQ			(1 << 31)
#define	I2C_DEBUG0_DMAENDCMD			(1 << 30)
#define	I2C_DEBUG0_DMAKICK			(1 << 29)
#define	I2C_DEBUG0_DMATERMINATE			(1 << 28)
#define	I2C_DEBUG0_STATE_VALUE_MASK		(0x3 << 26)
#define	I2C_DEBUG0_STATE_VALUE_OFFSET		26
#define	I2C_DEBUG0_DMA_STATE_MASK		(0x3ff << 16)
#define	I2C_DEBUG0_DMA_STATE_OFFSET		16
#define	I2C_DEBUG0_START_TOGGLE			(1 << 15)
#define	I2C_DEBUG0_STOP_TOGGLE			(1 << 14)
#define	I2C_DEBUG0_GRAB_TOGGLE			(1 << 13)
#define	I2C_DEBUG0_CHANGE_TOGGLE		(1 << 12)
#define	I2C_DEBUG0_STATE_LATCH			(1 << 11)
#define	I2C_DEBUG0_SLAVE_HOLD_CLK		(1 << 10)
#define	I2C_DEBUG0_STATE_STATE_MASK		0x3ff
#define	I2C_DEBUG0_STATE_STATE_OFFSET		0

#define	I2C_DEBUG1_I2C_CLK_IN			(1 << 31)
#define	I2C_DEBUG1_I2C_DATA_IN			(1 << 30)
#define	I2C_DEBUG1_DMA_BYTE_ENABLES_MASK	(0xf << 24)
#define	I2C_DEBUG1_DMA_BYTE_ENABLES_OFFSET	24
#define	I2C_DEBUG1_CLK_GEN_STATE_MASK		(0xff << 16)
#define	I2C_DEBUG1_CLK_GEN_STATE_OFFSET		16
#define	I2C_DEBUG1_LST_MODE_MASK		(0x3 << 9)
#define	I2C_DEBUG1_LST_MODE_OFFSET		9
#define	I2C_DEBUG1_LOCAL_SLAVE_TEST		(1 << 8)
#define	I2C_DEBUG1_FORCE_CLK_ON			(1 << 4)
#define	I2C_DEBUG1_FORCE_ABR_LOSS		(1 << 3)
#define	I2C_DEBUG1_FORCE_RCV_ACK		(1 << 2)
#define	I2C_DEBUG1_FORCE_I2C_DATA_OE		(1 << 1)
#define	I2C_DEBUG1_FORCE_I2C_CLK_OE		(1 << 0)

#define	I2C_VERSION_MAJOR_MASK			(0xff << 24)
#define	I2C_VERSION_MAJOR_OFFSET		24
#define	I2C_VERSION_MINOR_MASK			(0xff << 16)
#define	I2C_VERSION_MINOR_OFFSET		16
#define	I2C_VERSION_STEP_MASK			0xffff
#define	I2C_VERSION_STEP_OFFSET			0

#endif	/* __MX28_REGS_I2C_H__ */
