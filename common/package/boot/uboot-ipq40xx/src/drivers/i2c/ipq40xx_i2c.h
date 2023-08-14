/*
 * Copyright (c) 2015 The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define I2C_STATUS_ERROR_MASK			0x38000FC

    /* QUP Core register offsets */
#define QUP_CONFIG_OFFSET			0x0
#define QUP_STATE_OFFSET			0x4
#define QUP_IO_MODES_OFFSET			0x8
#define QUP_SW_RESET_OFFSET			0xc
#define QUP_TIME_OUT_OFFSET			0x10
#define QUP_TIME_OUT_CURRENT_OFFSET		0x14
#define QUP_OPERATIONAL_OFFSET			0x18
#define QUP_ERROR_FLAGS_OFFSET			0x1c
#define QUP_ERROR_FLAGS_EN_OFFSET		0x20
#define QUP_TEST_CTRL_OFFSET			0x24
#define QUP_OPERATIONAL_MASK_OFFSET		0x28
#define QUP_MX_OUTPUT_COUNT_OFFSET		0x100
#define QUP_MX_OUTPUT_CNT_CURRENT_OFFSET	0x104
#define QUP_OUTPUT_DEBUG_OFFSET			0x108
#define QUP_OUTPUT_FIFO_WORD_CNT_OFFSET		0x10c
#define QUP_OUTPUT_FIFO_OFFSET			0x110
#define QUP_MX_WRITE_COUNT_OFFSET		0x150
#define QUP_WRITE_CNT_CURRENT_OFFSET		0x154
#define QUP_MX_INPUT_COUNT_OFFSET		0x200
#define QUP_MX_READ_COUNT_OFFSET		0x208
#define QUP_MX_READ_CNT_CURRENT_OFFSET		0x20c
#define QUP_INPUT_DEBUG_OFFSET			0x210
#define QUP_INPUT_FIFO_WORD_CNT_OFFSET		0x214
#define QUP_INPUT_FIFO_OFFSET			0x218
#define QUP_I2C_MASTER_CLK_CTL_OFFSET		0x400
#define QUP_I2C_MASTER_STATUS_OFFSET		0x404
#define QUP_I2C_MASTER_CONFIG_OFFSET		0x408
#define QUP_MAX_OFFSET				0xfff

#define QUP_MX_OUTPUT_COUNT			0x0000
#define QUP_MX_INPUT_COUNT			0x0000
#define QUP_MX_WRITE_COUNT			0x0000
#define QUP_MX_READ_COUNT			0x0000

#define SUCCESS					0
#define ENACK					1
#define TIMEOUT_CNT				100

#define QUP_STATE_RESET				0x0
#define QUP_STATE_RUN				0x1D
#define QUP_STATE_PAUSE				0x1F
#define QUP_STATE_VALID_BIT			2
#define QUP_STATE_VALID				1
#define QUP_STATE_MASK				0x3

#define NACK_BIT_MASK				0x8
#define NACK_BIT_SHIFT				3

#define QUP_CONFIG_MINI_CORE_I2C		(2 << 8)
#define I2C_BIT_WORD				0x7
#define INPUT_FIFO_MODE				(0x0 << 12)
#define OUTPUT_FIFO_MODE			(0x0 << 10)
#define INPUT_BLOCK_MODE			(0x01 << 12)
#define OUTPUT_BLOCK_MODE			(0x01 << 10)
#define PACK_EN					(0x01 << 15)
#define UNPACK_EN				(0x01 << 14)
#define OUTPUT_BIT_SHIFT_EN			(0x01 << 16)
#define ERROR_FLAGS_EN				0x3C
#define I2C_MASTER_STATUS_CLEAR			0xFFFFFC
#define QUP_DATA_AVAILABLE_FOR_READ		(1 << 5)
#define OUTPUT_SERVICE_FLAG			(1 << 8)
#define INPUT_SERVICE_FLAG			(1 << 9)
#define MAX_OUTPUT_DONE_FLAG			(1 << 10)
#define MAX_INPUT_DONE_FLAG			(1 << 11)
#define OUTPUT_FIFO_NOT_EMPTY			(1 << 4)
#define OUTPUT_FIFO_FULL			(1 << 6)
#define INPUT_FIFO_NOT_EMPTY			(1 << 5)
#define INPUT_FIFO_FULL				(1 << 7)
#define QUP_APP_CLK_ON_EN			(1 << 12)
#define QUP_CORE_CLK_ON_EN			(1 << 13)
#define QUP_FIFO_CLK_GATE_EN			(1 << 14)
#define QUP_EN_VERSION_TWO_TAG			1

#define I2C_WRITE				0x0
#define I2C_READ				0x1
#define QUP_I2C_START_SEQ			0x81
#define QUP_I2C_DATA_WRITE_SEQ			0x82
#define QUP_I2C_DATA_WRITE_AND_STOP_SEQ		0x83
#define QUP_I2C_DATA_READ_SEQ			0x85
#define QUP_I2C_DATA_READ_AND_STOP_SEQ		0x87
#define QUP_I2C_STOP_SEQ			0x88
#define QUP_I2C_START_AND_STOP_SEQ		0x8A
#define QUP_I2C_NOP_PADDING			0x97

#define OUT_FIFO_WR_TAG_BYTE_CNT		4
#define OUT_FIFO_RD_TAG_BYTE_CNT		8
#define IN_FIFO_TAG_BYTE_CNT			2
#define OFFSET_BYTE_CNT				2

#define QUP_I2C_ADDR(x)				((x & 0xFF) << 1)
#define QUP_I2C_DATA(x)				(x & 0xFF)

enum dir {
	READ,
	WRITE,
};

/* I2C some pre-defined frequencies */
#define I2C_CLK_1KHZ			1
#define I2C_CLK_100KHZ			100
#define I2C_CLK_400KHZ			400
#define I2C_CLK_1MHZ			1000
#define QUP_INPUT_CLK_TCXO		19200
#define QUP_INPUT_CLK			QUP_INPUT_CLK_TCXO
#define I2C_INPUT_CLK_TCXO_DIV4		((I2C_INPUT_CLK_TCXO)/4)

