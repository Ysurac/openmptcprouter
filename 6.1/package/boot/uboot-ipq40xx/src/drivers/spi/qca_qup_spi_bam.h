/*
 * Register definitions for the IPQ40XX QUP-SPI Controller
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _IPQ40XX_SPI_H_
#define _IPQ40XX_SPI_H_

#define QUP0_BASE	0x78b5000
#define QUP1_BASE	0x78b6000
#define BLSP0_BAM_BASE	0x7884000

#define BLSP0_QUP_REG_BASE		(QUP0_BASE + 0x00000000)
#define BLSP1_QUP_REG_BASE		(QUP1_BASE + 0x00000000)

#define BLSP0_SPI_CONFIG_REG		(BLSP0_QUP_REG_BASE + 0x00000300)
#define BLSP1_SPI_CONFIG_REG		(BLSP1_QUP_REG_BASE + 0x00000300)

#define BLSP0_SPI_IO_CONTROL_REG	(BLSP0_QUP_REG_BASE + 0x00000304)
#define BLSP1_SPI_IO_CONTROL_REG	(BLSP1_QUP_REG_BASE + 0x00000304)

#define BLSP0_SPI_ERROR_FLAGS_REG	(BLSP0_QUP_REG_BASE + 0x00000308)
#define BLSP1_SPI_ERROR_FLAGS_REG	(BLSP1_QUP_REG_BASE + 0x00000308)

#define BLSP0_SPI_DEASSERT_WAIT_REG	(BLSP0_QUP_REG_BASE + 0x00000310)
#define BLSP1_SPI_DEASSERT_WAIT_REG	(BLSP1_QUP_REG_BASE + 0x00000310)
#define BLSP0_SPI_ERROR_FLAGS_EN_REG	(BLSP0_QUP_REG_BASE + 0x0000030c)
#define BLSP1_SPI_ERROR_FLAGS_EN_REG	(BLSP1_QUP_REG_BASE + 0x0000030c)

#define BLSP0_QUP_CONFIG_REG		(BLSP0_QUP_REG_BASE + 0x00000000)
#define BLSP1_QUP_CONFIG_REG		(BLSP1_QUP_REG_BASE + 0x00000000)

#define BLSP0_QUP_ERROR_FLAGS_REG	(BLSP0_QUP_REG_BASE      + 0x0000001c)
#define BLSP1_QUP_ERROR_FLAGS_REG	(BLSP1_QUP_REG_BASE      + 0x0000001c)

#define BLSP0_QUP_ERROR_FLAGS_EN_REG	(BLSP0_QUP_REG_BASE + 0x00000020)
#define BLSP1_QUP_ERROR_FLAGS_EN_REG	(BLSP1_QUP_REG_BASE + 0x00000020)

#define BLSP0_QUP_OPERATIONAL_MASK	(BLSP0_QUP_REG_BASE + 0x00000028)
#define BLSP1_QUP_OPERATIONAL_MASK	(BLSP1_QUP_REG_BASE + 0x00000028)

#define BLSP0_QUP_OPERATIONAL_REG	(BLSP0_QUP_REG_BASE + 0x00000018)
#define BLSP1_QUP_OPERATIONAL_REG	(BLSP1_QUP_REG_BASE + 0x00000018)

#define BLSP0_QUP_IO_MODES_REG		(BLSP0_QUP_REG_BASE + 0x00000008)
#define BLSP1_QUP_IO_MODES_REG		(BLSP1_QUP_REG_BASE + 0x00000008)

#define BLSP0_QUP_STATE_REG			(BLSP0_QUP_REG_BASE + 0x00000004)
#define BLSP1_QUP_STATE_REG			(BLSP1_QUP_REG_BASE + 0x00000004)


#define BLSP0_QUP_INPUT_FIFOc_REG(c) \
		(BLSP0_QUP_REG_BASE + 0x00000218 + 4 * (c))
#define BLSP1_QUP_INPUT_FIFOc_REG(c) \
		(BLSP1_QUP_REG_BASE + 0x00000218 + 4 * (c))

#define BLSP0_QUP_OUTPUT_FIFOc_REG(c) \
		(BLSP0_QUP_REG_BASE + 0x00000110 + 4 * (c))
#define BLSP1_QUP_OUTPUT_FIFOc_REG(c) \
		(BLSP1_QUP_REG_BASE + 0x00000110 + 4 * (c))

#define BLSP0_QUP_MX_INPUT_COUNT_REG	(BLSP0_QUP_REG_BASE + 0x00000200)
#define BLSP1_QUP_MX_INPUT_COUNT_REG	(BLSP1_QUP_REG_BASE + 0x00000200)

#define BLSP0_QUP_MX_OUTPUT_COUNT_REG	(BLSP0_QUP_REG_BASE + 0x00000100)
#define BLSP1_QUP_MX_OUTPUT_COUNT_REG	(BLSP1_QUP_REG_BASE + 0x00000100)

#define BLSP0_QUP_MX_READ_COUNT_REG		(BLSP0_QUP_REG_BASE + 0x00000208)
#define BLSP1_QUP_MX_READ_COUNT_REG		(BLSP1_QUP_REG_BASE + 0x00000208)

#define BLSP0_QUP_MX_WRITE_COUNT_REG	(BLSP0_QUP_REG_BASE + 0x00000150)
#define BLSP1_QUP_MX_WRITE_COUNT_REG	(BLSP1_QUP_REG_BASE + 0x00000150)

#define BLSP0_QUP_SW_RESET_REG			(BLSP0_QUP_REG_BASE + 0x0000000c)
#define BLSP1_QUP_SW_RESET_REG			(BLSP1_QUP_REG_BASE + 0x0000000c)

#define QUP_STATE_VALID_BIT			2
#define QUP_STATE_VALID				1
#define QUP_STATE_MASK				0x3
#define QUP_CONFIG_MINI_CORE_MSK	(0x0F << 8)
#define QUP_CONFIG_MINI_CORE_SPI	(1 << 8)
#define QUP_CONF_INPUT_MSK			(1 << 7)
#define QUP_CONF_INPUT_ENA			(0 << 7)
#define QUP_CONF_NO_INPUT			(1 << 7)
#define QUP_CONF_OUTPUT_MSK			(1 << 6)
#define QUP_CONF_OUTPUT_ENA			(0 << 6)
#define QUP_CONF_NO_OUTPUT			(1 << 6)
#define QUP_STATE_RUN_STATE			0x1
#define QUP_STATE_RESET_STATE		0x0
#define QUP_STATE_PAUSE_STATE		0x3
#define SPI_BIT_WORD_MSK			0x1F
#define SPI_8_BIT_WORD				0x07
#define LOOP_BACK_MSK				(1 << 8)
#define NO_LOOP_BACK				(0 << 8)
#define SLAVE_OPERATION_MSK			(1 << 5)
#define SLAVE_OPERATION				(0 << 5)
#define CLK_ALWAYS_ON				(0 << 9)
#define MX_CS_MODE					(1 << 8)
#define CS_POLARITY_MASK			(1 << 4)
#define NO_TRI_STATE				(1 << 0)
#define FORCE_CS_MSK				(1 << 11)
#define FORCE_CS_EN					(1 << 11)
#define FORCE_CS_DIS				(0 << 11)
#define OUTPUT_BIT_SHIFT_MSK		(1 << 16)
#define OUTPUT_BIT_SHIFT_EN			(1 << 16)
#define INPUT_BLOCK_MODE_MSK		(0x03 << 12)
#define INPUT_BLOCK_MODE			(0x01 << 12)
#define OUTPUT_BLOCK_MODE_MSK		(0x03 << 10)
#define OUTPUT_BLOCK_MODE			(0x01 << 10)
#define INPUT_BAM_MODE				(0x3 << 12)
#define OUTPUT_BAM_MODE				(0x3 << 10)
#define PACK_EN						(0x1 << 15)
#define UNPACK_EN					(0x1 << 14)
#define PACK_EN_MSK					(0x1 << 15)
#define UNPACK_EN_MSK				(0x1 << 14)
#define OUTPUT_SERVICE_MSK			(0x1 << 8)
#define INPUT_SERVICE_MSK			(0x1 << 9)
#define OUTPUT_SERVICE_DIS			(0x1 << 8)
#define INPUT_SERVICE_DIS			(0x1 << 9)




#define SPI_INPUT_FIRST_MODE			(1 << 9)
#define SPI_IO_CONTROL_CLOCK_IDLE_HIGH		(1 << 10)
#define QUP_DATA_AVAILABLE_FOR_READ		(1 << 5)
#define QUP_OUTPUT_FIFO_NOT_EMPTY		(1 << 4)
#define OUTPUT_SERVICE_FLAG			(1 << 8)
#define INPUT_SERVICE_FLAG			(1 << 9)
#define QUP_OUTPUT_FIFO_FULL			(1 << 6)
#define SPI_INPUT_BLOCK_SIZE			4
#define SPI_OUTPUT_BLOCK_SIZE			4
#define MSM_QUP_MAX_FREQ			51200000
#define MAX_COUNT_SIZE				0xffff

#define SPI_RESET_STATE				0
#define SPI_RUN_STATE				1
#define SPI_CORE_RESET				0
#define SPI_CORE_RUNNING			1
#define SPI_MODE0				0
#define SPI_MODE1				1
#define SPI_MODE2				2
#define SPI_MODE3				3
#define BLSP0_SPI				0
#define BLSP1_SPI				1

struct blsp_spi {
	unsigned int	spi_config;
	unsigned int	io_control;
	unsigned int	error_flags;
	unsigned int	error_flags_en;
	unsigned int	qup_config;
	unsigned int	qup_error_flags;
	unsigned int	qup_error_flags_en;
	unsigned int	qup_operational;
	unsigned int	qup_io_modes;
	unsigned int	qup_state;
	unsigned int	qup_input_fifo;
	unsigned int	qup_output_fifo;
	unsigned int	qup_mx_input_count;
	unsigned int	qup_mx_output_count;
	unsigned int	qup_mx_read_count;
	unsigned int	qup_mx_write_count;
	unsigned int	qup_sw_reset;
	unsigned int	qup_op_mask;
	unsigned int	qup_deassert_wait;
};

static const struct blsp_spi spi_reg[] = {
	/* BLSP0 registers for SPI interface */
	{
		BLSP0_SPI_CONFIG_REG,
		BLSP0_SPI_IO_CONTROL_REG,
		BLSP0_SPI_ERROR_FLAGS_REG,
		BLSP0_SPI_ERROR_FLAGS_EN_REG,
		BLSP0_QUP_CONFIG_REG,
		BLSP0_QUP_ERROR_FLAGS_REG,
		BLSP0_QUP_ERROR_FLAGS_EN_REG,
		BLSP0_QUP_OPERATIONAL_REG,
		BLSP0_QUP_IO_MODES_REG,
		BLSP0_QUP_STATE_REG,
		BLSP0_QUP_INPUT_FIFOc_REG(0),
		BLSP0_QUP_OUTPUT_FIFOc_REG(0),
		BLSP0_QUP_MX_INPUT_COUNT_REG,
		BLSP0_QUP_MX_OUTPUT_COUNT_REG,
		BLSP0_QUP_MX_READ_COUNT_REG,
		BLSP0_QUP_MX_WRITE_COUNT_REG,
		BLSP0_QUP_SW_RESET_REG,
		BLSP0_QUP_OPERATIONAL_MASK,
		BLSP0_SPI_DEASSERT_WAIT_REG,
	},
	/* BLSP1 registers for SPI interface */
	{
		BLSP1_SPI_CONFIG_REG,
		BLSP1_SPI_IO_CONTROL_REG,
		BLSP1_SPI_ERROR_FLAGS_REG,
		BLSP1_SPI_ERROR_FLAGS_EN_REG,
		BLSP1_QUP_CONFIG_REG,
		BLSP1_QUP_ERROR_FLAGS_REG,
		BLSP1_QUP_ERROR_FLAGS_EN_REG,
		BLSP1_QUP_OPERATIONAL_REG,
		BLSP1_QUP_IO_MODES_REG,
		BLSP1_QUP_STATE_REG,
		BLSP1_QUP_INPUT_FIFOc_REG(0),
		BLSP1_QUP_OUTPUT_FIFOc_REG(0),
		BLSP1_QUP_MX_INPUT_COUNT_REG,
		BLSP1_QUP_MX_OUTPUT_COUNT_REG,
		BLSP1_QUP_MX_READ_COUNT_REG,
		BLSP1_QUP_MX_WRITE_COUNT_REG,
		BLSP1_QUP_SW_RESET_REG,
		BLSP1_QUP_OPERATIONAL_MASK,
	},
};

#define SUCCESS		0
#define FAILURE		1

#define DUMMY_DATA_VAL	0
#define TIMEOUT_CNT	100
#define SPI_BITLEN_MSK	0x07

#define WRITE		0
#define READ		1

struct ipq_spi_slave {
	struct spi_slave slave;
	const struct blsp_spi *regs;
	unsigned int core_state;
	unsigned int mode;
	unsigned int initialized;
	unsigned long freq;
	unsigned char use_dma;
};

static inline struct ipq_spi_slave *to_ipq_spi(struct spi_slave *slave)
{
	return container_of(slave, struct ipq_spi_slave, slave);
}

/* QUP BAM related defines */

#define DATA_CONSUMER_PIPE_INDEX		0
#define DATA_PRODUCER_PIPE_INDEX		1

/* QUP0 BAM pipe numbers */
#define QUP0_DATA_CONSUMER_PIPE			4
#define QUP0_DATA_PRODUCER_PIPE			5

/* QUP1 BAM pipe numbers */
#define QUP1_DATA_CONSUMER_PIPE			6
#define QUP1_DATA_PRODUCER_PIPE			7

/* QUP0 BAM pipe groups */
#define QUP0_DATA_PRODUCER_PIPE_GRP		0
#define QUP0_DATA_CONSUMER_PIPE_GRP		0

/* QUP1 BAM pipe groups */
#define QUP1_DATA_PRODUCER_PIPE_GRP		0
#define QUP1_DATA_CONSUMER_PIPE_GRP		0


/* QUP EE */
#define QUP_SPI_EE				0

/* QUP max desc length. */
#define QUP_SPI_MAX_DESC_LEN			0xFFFF
#define QUP_SPI_BAM_THRESHOLD			16
#define SPI_MAX_TRFR_BTWN_RESETS		((64 * 1024) - 16)  /* 64KB - 16byte */
#define SPI_BAM_MAX_DESC_NUM			32
#define QUP_BAM_DATA_FIFO_SIZE			32

static struct bam_instance bam;
struct bam_desc qup_spi_data_desc_fifo[QUP_BAM_DATA_FIFO_SIZE] __attribute__ ((aligned(BAM_DESC_SIZE)));

#define min_t(type, x, y) \
    ({ type __x = (x); type __y = (y); __x < __y ? __x : __y; })

#define NUM_PORTS				1
#define NUM_CS					2

#define FUNC_SEL_GPIO				0
#define GPIO_DRV_STR_10MA			0x4

#define FUNC_SEL_GPIO				0
#define GPIO_FUNC_ENABLE			1
/* QUP0 CS GPIO mapping*/
#define QUP0_SPI_CS_0				54
#define QUP0_SPI_CS_1_DK01			63
#define QUP0_SPI_CS_1_DK04			45
#define CS_ASSERT				1
#define CS_DEASSERT				0

#endif /* _IPQ40XX_SPI_H_ */

