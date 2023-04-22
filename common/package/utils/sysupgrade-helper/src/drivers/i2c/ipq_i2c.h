/*
 * Copyright (c) 2014 The Linux Foundation. All rights reserved.
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

static int gsbi_base[] = {
	0x12440000,
	0x12480000,
	0x16200000,
	0x16300000,
	0x1A200000,
	0x16500000,
	0x16600000
};

/* GSBI QUP devices base address */
static int qup_offset[] = {
	0x20000,
	0x20000,
	0x80000,
	0x80000,
	0x80000,
	0x80000,
	0x80000,
};

/* QUP Registers Base Address */
#define GSBI_QUP_BASE(x)	(gsbi_base[x-1] + qup_offset[x-1])

#define GSBI_RESET(N)		(0x009029DC + (32*(N-1)))

#define I2C_STATUS_ERROR_MASK	0x38000FC


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
#define QUP_MAX_OFFSET				0xfff

#define SUCCESS					0
#define TIMEOUT_CNT				100

#define QUP_STATE_RESET				0x0
#define QUP_STATE_RUN				0x1D
#define QUP_STATE_PAUSE				0x1F
#define QUP_STATE_VALID_BIT			2
#define QUP_STATE_VALID				1
#define QUP_STATE_MASK				0x3

#define QUP_CONFIG_MINI_CORE_I2C		(2 << 8)
#define I2C_BIT_WORD				0xF
#define INPUT_FIFO_MODE				(0x0 << 12)
#define OUTPUT_FIFO_MODE			(0x0 << 10)
#define INPUT_BLOCK_MODE			(0x01 << 12)
#define OUTPUT_BLOCK_MODE			(0x01 << 10)
#define PACK_EN					(0x01 << 15)
#define UNPACK_EN				(0x01 << 14)
#define OUTPUT_BIT_SHIFT_EN			(0x01 << 16)
#define ERROR_FLAGS_EN				0x7C
#define I2C_MASTER_STATUS_CLEAR			0xFFFFFC
#define QUP_DATA_AVAILABLE_FOR_READ		(1 << 5)
#define QUP_OUTPUT_FIFO_NOT_EMPTY		(1 << 4)
#define OUTPUT_SERVICE_FLAG			(1 << 8)
#define INPUT_SERVICE_FLAG			(1 << 9)
#define QUP_OUTPUT_FIFO_FULL			(1 << 6)
#define MAX_OUTPUT_DONE_FLAG			(1 << 10)
#define MAX_INPUT_DONE_FLAG			(1 << 11)
#define QUP_DATA_AVAILABLE_FOR_READ             (1 << 5)
#define OUTPUT_FIFO_NOT_EMPTY               	(1 << 4)
#define OUTPUT_FIFO_FULL			(1 << 6)

#define QUP_I2C_START_SEQ			(0x1 << 8)
#define QUP_I2C_DATA_SEQ			(0x2 << 8)
#define QUP_I2C_STOP_SEQ			(0x3 << 8)
#define QUP_I2C_RECV_SEQ			(0x4 << 8)

/* Tags for input FIFO */
#define QUP_I2C_MIDATA_SEQ			(0x5 << 8)
#define QUP_I2C_MISTOP_SEQ			(0x6 << 8)
#define QUP_I2C_MINACK_SEQ			(0x7 << 8)

#define QUP_I2C_ADDR(x)				((x & 0xFF) << 1)
#define QUP_I2C_DATA(x)				(x & 0xFF)
#define QUP_I2C_MI_TAG(x)			(x & 0xFF00)
#define QUP_I2C_SLAVE_READ			(0x1)

enum dir {
	READ,
	WRITE,
};

typedef enum
{
	QUPI2C_NOP,
	QUPI2C_START,
	QUPI2C_MO_DATA,
	QUPI2C_MO_STOP,
	QUPI2C_MI_REC,
	QUPI2C_MI_DATA,
	QUPI2C_MI_STOP,
	QUPI2C_MI_NACK,
	QUPI2C_SO_DATA,
	QUPI2C_SI_START,
	QUPI2C_SI_DATA
} qupi2c_TagType;

#define qupi2c_NumberOfNops(c)		((QUPI2C_NOP     << 8) | c)
#define qupi2c_StartWrite(c)		((QUPI2C_START   << 8) | (c << 1) | 0)
#define qupi2c_StartRead(c)		((QUPI2C_START   << 8) | (c << 1) | 1)
#define qupi2c_ByteWrite(c)		((QUPI2C_MO_DATA << 8) | c)
#define qupi2c_LastByteWrite(c)		((QUPI2C_MO_STOP << 8) | c)
#define qupi2c_NumberofReads(c)		((QUPI2C_MI_REC  << 8) | c)

/* I2C some pre-defined frequencies */
#define I2C_CLK_1KHZ			1
#define I2C_CLK_100KHZ			100
#define I2C_CLK_400KHZ			400
#define I2C_CLK_1MHZ			1000
#define QUP_INPUT_CLK_TCXO		19200
#define QUP_INPUT_CLK_PLL8		24000
#define I2C_INPUT_CLK_TCXO_DIV4		((I2C_INPUT_CLK_TCXO)/4)
#define QUP_INPUT_CLK			QUP_INPUT_CLK_PLL8
#define CONFIG_I2C_CLK_FREQ		I2C_CLK_100KHZ

