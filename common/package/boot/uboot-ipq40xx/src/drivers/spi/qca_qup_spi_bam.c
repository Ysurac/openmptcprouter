/*
 * BLSP QUP SPI controller driver.
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

#include <common.h>
#include <watchdog.h>
#include <spi.h>
#include <malloc.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <asm/arch-qcom-common/gpio.h>
#include <asm/arch-qcom-common/bam.h>
#include <asm/arch-ipq40xx/iomap.h>
#include "qca_qup_spi_bam.h"

DECLARE_GLOBAL_DATA_PTR;

/*
 * CS GPIO number array cs_gpio_array[port_num][cs_num]
 * cs_gpio_array[0][x] -- QUP0
 */
static unsigned int cs_gpio_array_dk01[NUM_PORTS][NUM_CS] = {
	{
		QUP0_SPI_CS_0, QUP0_SPI_CS_1_DK01,
	},
};

static unsigned int cs_gpio_array_dk04[NUM_PORTS][NUM_CS] = {
	{
		QUP0_SPI_CS_0, QUP0_SPI_CS_1_DK04,
	},
};

static unsigned int (*cs_gpio_array)[NUM_CS] = cs_gpio_array_dk01;

static int check_bit_state(uint32_t reg_addr, int bit_num, int val,
							int us_delay)
{
	unsigned int count = TIMEOUT_CNT;
	unsigned int bit_val = ((readl(reg_addr) >> bit_num) & 0x01);

	while (bit_val != val) {
		count--;
		if (count == 0)
			return -ETIMEDOUT;
		udelay(us_delay);
		bit_val = ((readl(reg_addr) >> bit_num) & 0x01);
	}

	return SUCCESS;
}

/*
 * Check whether QUPn State is valid
 */
static int check_qup_state_valid(struct ipq_spi_slave *ds)
{
	return check_bit_state(ds->regs->qup_state, QUP_STATE_VALID_BIT,
				QUP_STATE_VALID, 1);
}

/*
 * Configure QUPn Core state
 */
static int config_spi_state(struct ipq_spi_slave *ds, unsigned int state)
{
	uint32_t val;
	int ret;

	ret = check_qup_state_valid(ds);
	if (ret != SUCCESS)
		return ret;

	switch (state) {
	case SPI_RUN_STATE:
		/* Set the state to RUN */
		val = ((readl(ds->regs->qup_state) & ~QUP_STATE_MASK)
					| QUP_STATE_RUN_STATE);
		writel(val, ds->regs->qup_state);
		ret = check_qup_state_valid(ds);
		if (ret != SUCCESS)
			return ret;
		ds->core_state = SPI_CORE_RUNNING;
		break;
	case SPI_RESET_STATE:
		/* Set the state to RESET */
		val = ((readl(ds->regs->qup_state) & ~QUP_STATE_MASK)
					| QUP_STATE_RESET_STATE);
		writel(val, ds->regs->qup_state);
		ret = check_qup_state_valid(ds);
		if (ret != SUCCESS)
			return ret;
		ds->core_state = SPI_CORE_RESET;
		break;
	default:
		printf("err: unsupported QUP SPI state : %d\n", state);
		ret = -EINVAL;
		break;
	}
	return ret;
}

/*
 * Set QUPn SPI Mode
 */
static void spi_set_mode(struct ipq_spi_slave *ds, unsigned int mode)
{
	unsigned int clk_idle_state;
	unsigned int input_first_mode;
	uint32_t val;

	switch (mode) {
	case SPI_MODE0:
		clk_idle_state = 0;
		input_first_mode = SPI_INPUT_FIRST_MODE;
		break;
	case SPI_MODE1:
		clk_idle_state = 0;
		input_first_mode = 0;
		break;
	case SPI_MODE2:
		clk_idle_state = 1;
		input_first_mode = SPI_INPUT_FIRST_MODE;
		break;
	case SPI_MODE3:
		clk_idle_state = 1;
		input_first_mode = 0;
		break;
	default:
		printf("err : unsupported spi mode : %d\n", mode);
		return;
	}

	val = readl(ds->regs->spi_config);
	val |= input_first_mode;
	writel(val, ds->regs->spi_config);

	val = readl(ds->regs->io_control);
	if (clk_idle_state)
		val |= SPI_IO_CONTROL_CLOCK_IDLE_HIGH;
	else
		val &= ~SPI_IO_CONTROL_CLOCK_IDLE_HIGH;

	writel(val, ds->regs->io_control);
}

/*
 * Reset entire QUP and all mini cores
 */
static void spi_reset(struct ipq_spi_slave *ds)
{
	writel(0x1, ds->regs->qup_sw_reset);
	udelay(5);
}

void spi_init()
{
	/* do nothing */
}

/*
 * Function to assert and De-assert chip select
 */
static void CS_change(int port_num, int cs_num, int enable)
{
	unsigned int cs_gpio = cs_gpio_array[port_num][cs_num];
	uint32_t addr = GPIO_IN_OUT_ADDR(cs_gpio);
	uint32_t val = readl(addr);

	val &= ~(GPIO_OUT);
	if (!enable)
		val |= GPIO_OUT;
	writel(val, addr);
}

static void blsp_pin_config(unsigned int port_num, int cs_num)
{
        unsigned int gpio;
	gpio = cs_gpio_array[port_num][cs_num];
	/* configure CS */
	gpio_tlmm_config(gpio, FUNC_SEL_GPIO, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_DRV_STR_10MA, GPIO_FUNC_ENABLE, 0, 0, 0);
}

int qup_bam_init(struct ipq_spi_slave *ds)
{
	unsigned int read_pipe = QUP0_DATA_PRODUCER_PIPE;
	unsigned int write_pipe = QUP0_DATA_CONSUMER_PIPE;
	uint8_t read_pipe_grp = QUP0_DATA_PRODUCER_PIPE_GRP;
	uint8_t write_pipe_grp = QUP0_DATA_CONSUMER_PIPE_GRP;
	int bam_ret;

	/* Pipe numbers based on the QUP index */
	if (ds->slave.bus == BLSP0_SPI) {
		read_pipe = QUP0_DATA_PRODUCER_PIPE;
		write_pipe = QUP0_DATA_CONSUMER_PIPE;
		read_pipe_grp = QUP0_DATA_PRODUCER_PIPE_GRP;
		write_pipe_grp = QUP0_DATA_CONSUMER_PIPE_GRP;
	} else if (ds->slave.bus == BLSP1_SPI) {
		read_pipe = QUP1_DATA_PRODUCER_PIPE;
		write_pipe = QUP1_DATA_CONSUMER_PIPE;
		read_pipe_grp = QUP1_DATA_PRODUCER_PIPE_GRP;
		write_pipe_grp = QUP1_DATA_CONSUMER_PIPE_GRP;
	}

	bam.base = BLSP0_BAM_BASE;
	/* Set Read Pipe Params */
	bam.pipe[DATA_PRODUCER_PIPE_INDEX].pipe_num = read_pipe;
	bam.pipe[DATA_PRODUCER_PIPE_INDEX].trans_type = BAM2SYS;
	bam.pipe[DATA_PRODUCER_PIPE_INDEX].fifo.size = QUP_BAM_DATA_FIFO_SIZE;
	bam.pipe[DATA_PRODUCER_PIPE_INDEX].fifo.head = qup_spi_data_desc_fifo;
	bam.pipe[DATA_PRODUCER_PIPE_INDEX].lock_grp = read_pipe_grp;

	/* Set Write pipe params. */
	bam.pipe[DATA_CONSUMER_PIPE_INDEX].pipe_num = write_pipe;
	bam.pipe[DATA_CONSUMER_PIPE_INDEX].trans_type = SYS2BAM;
	bam.pipe[DATA_CONSUMER_PIPE_INDEX].fifo.size = QUP_BAM_DATA_FIFO_SIZE;
	bam.pipe[DATA_CONSUMER_PIPE_INDEX].fifo.head = qup_spi_data_desc_fifo;
	bam.pipe[DATA_CONSUMER_PIPE_INDEX].lock_grp = write_pipe_grp;

	/* Programs the threshold for BAM transfer
	 * When this threshold is reached, BAM signals the peripheral via the
	 * pipe_bytes_available interface.
	 * The peripheral is signalled with this notification in the following
	 * cases:
	 * a.  It has accumulated all the descriptors.
	 * b.  It has accumulated more than threshold bytes.
	 * c.  It has reached EOT (End Of Transfer).
	 * Note: this value needs to be set by the h/w folks and is specific for
	 * each peripheral.
	 */
	bam.threshold = QUP_SPI_BAM_THRESHOLD;
	/* Set the EE.  */
	bam.ee = QUP_SPI_EE;
	/* Set the max desc length for this BAM. */
	bam.max_desc_len = QUP_SPI_MAX_DESC_LEN;
	/* BAM Init. */
	bam_init(&bam);
	/* Initialize BAM QPIC read pipe */
	bam_sys_pipe_init(&bam, DATA_PRODUCER_PIPE_INDEX);
	/* Init read fifo */
	bam_ret = bam_pipe_fifo_init(&bam, DATA_PRODUCER_PIPE_INDEX);
	if (bam_ret) {
		printf("QUP: SPI:  BAM Read FIFO init error\n");
		bam_ret = FAILURE;
		goto qup_spi_bam_init_error;
	}

	/* Initialize BAM QPIC write pipe */
	bam_sys_pipe_init(&bam, DATA_CONSUMER_PIPE_INDEX);

	/* Init write fifo. Use the same fifo as read fifo. */
	bam_ret = bam_pipe_fifo_init(&bam, DATA_CONSUMER_PIPE_INDEX);
	if (bam_ret) {
		printf("QUP: SPI: BAM Write FIFO init error\n");
		bam_ret = FAILURE;
		goto qup_spi_bam_init_error;
	}
qup_spi_bam_init_error:
	return bam_ret;
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
				unsigned int max_hz, unsigned int mode)
{
	struct ipq_spi_slave *ds;

	/*
	 * spi gpios are not initialised for DK07-C2. If probed, the
	 * controller will indefinitely wait for response from slave.
	 * Hence, return NULL.
	 */
	if (gd->bd->bi_arch_number == MACH_TYPE_IPQ40XX_AP_DK07_1_C2)
		return NULL;

	ds = malloc(sizeof(struct ipq_spi_slave));
	if (!ds) {
		printf("SPI error: malloc of SPI structure failed\n");
		return NULL;
	}

	memset(ds, 0, sizeof(struct ipq_spi_slave));

	/*
	 * QCA BLSP supports SPI Flash
	 * on different BLSP0 and BLSP1
	 * with different number of chip selects (CS, channels):
	*/
	if ((bus > BLSP1_SPI)
		|| ((bus == BLSP0_SPI) && (cs > 2))
		|| ((bus == BLSP1_SPI) && (cs > 0))) {
		printf("SPI error: unsupported bus %d "
			"(Supported busses 0,1 and 2) or chipselect\n", bus);
		goto err;
	}
	ds->slave.bus	= bus;
	ds->slave.cs	= cs;

	ds->regs	= &spi_reg[bus];

	/* TODO For different clock frequency */
	if (max_hz > MSM_QUP_MAX_FREQ) {
		printf("SPI error: unsupported frequency %d Hz "
			"Max frequency is %d Hz\n", max_hz, MSM_QUP_MAX_FREQ);
		goto err;
	}
	ds->freq = max_hz;

	if (mode > SPI_MODE3) {
		printf("SPI error: unsupported SPI mode %d\n", mode);
		goto err;
	}
	ds->mode = mode;

	/* DMA mode */
	ds->use_dma = CONFIG_QUP_SPI_USE_DMA;

	if (ds->slave.cs == CONFIG_SF_SPI_NAND_CS) {
		/* GPIO Configuration for SPI port */
		if (gd->bd->bi_arch_number == MACH_TYPE_IPQ40XX_AP_DK04_1_C5)
			cs_gpio_array = cs_gpio_array_dk04;
		blsp_pin_config(ds->slave.bus, ds->slave.cs);
		CS_change(ds->slave.bus, ds->slave.cs, CS_DEASSERT);
	}

	return &ds->slave;
err:
	free(ds);
	return NULL;
}

void spi_free_slave(struct spi_slave *slave)
{
	struct ipq_spi_slave *ds = to_ipq_spi(slave);

	if (ds != NULL)
		free(ds);
}

/*
 * BLSP QUPn SPI Hardware Initialisation
 */
static int spi_hw_init(struct ipq_spi_slave *ds)
{
	int ret;

	ds->initialized = 0;

	/* QUPn module configuration */
	spi_reset(ds);

	/* Set the QUPn state */
	ret = config_spi_state(ds, SPI_RESET_STATE);
	if (ret)
		return ret;

	/* Initialize BAM */
	if(ds->use_dma) {
		ret = qup_bam_init(ds);
		if (ret != SUCCESS)
			return ret;
	}


	/*
	 * Configure Mini core to SPI core with Input Output enabled,
	 * SPI master, N = 8 bits
	 */
	clrsetbits_le32(ds->regs->qup_config, (QUP_CONFIG_MINI_CORE_MSK |
						QUP_CONF_INPUT_MSK |
						QUP_CONF_OUTPUT_MSK |
						SPI_BIT_WORD_MSK),
						(QUP_CONFIG_MINI_CORE_SPI |
						QUP_CONF_INPUT_ENA |
						QUP_CONF_OUTPUT_ENA |
						SPI_8_BIT_WORD));

	/*
	 * Configure Input first SPI protocol,
	 * SPI master mode and no loopback
	 */
	clrsetbits_le32(ds->regs->spi_config, (LOOP_BACK_MSK |
						SLAVE_OPERATION_MSK),
						(NO_LOOP_BACK |
						SLAVE_OPERATION));

	/*
	 * Configure SPI IO Control Register
	 * CLK_ALWAYS_ON = 0
	 * MX_CS_MODE = 0
	 * NO_TRI_STATE = 1
	 */
	writel((CLK_ALWAYS_ON | NO_TRI_STATE),
				ds->regs->io_control);

	/*
	 * Configure SPI IO Modes.
	 * OUTPUT_BIT_SHIFT_EN = 1
	 * INPUT_MODE = BAM Mode
	 * OUTPUT MODE = BAM Mode
	 */

	if (ds->use_dma)
		clrsetbits_le32(ds->regs->qup_io_modes, (OUTPUT_BIT_SHIFT_MSK |
						INPUT_BLOCK_MODE_MSK |
						OUTPUT_BLOCK_MODE_MSK |
						PACK_EN_MSK |
						UNPACK_EN_MSK),
						(OUTPUT_BIT_SHIFT_EN |
						INPUT_BAM_MODE |
						OUTPUT_BAM_MODE | PACK_EN |
						UNPACK_EN));
	else
	/*
	 * Configure SPI IO Modes.
	 * OUTPUT_BIT_SHIFT_EN = 1
	 * INPUT_MODE = Block Mode
	 * OUTPUT MODE = Block Mode
	 */

		clrsetbits_le32(ds->regs->qup_io_modes, (OUTPUT_BIT_SHIFT_MSK |
					INPUT_BLOCK_MODE_MSK |
					OUTPUT_BLOCK_MODE_MSK),
					(OUTPUT_BIT_SHIFT_EN |
					INPUT_BLOCK_MODE |
					OUTPUT_BLOCK_MODE));

	spi_set_mode(ds, ds->mode);

	/* Disable Error mask */
	writel(0, ds->regs->error_flags_en);
	writel(0, ds->regs->qup_error_flags_en);
	writel(0, ds->regs->qup_deassert_wait);

	if (ds->use_dma)
		clrsetbits_le32(ds->regs->qup_op_mask, (OUTPUT_SERVICE_MSK |
					INPUT_SERVICE_MSK),
					(OUTPUT_SERVICE_DIS |
					INPUT_SERVICE_DIS));
	ds->initialized = 1;

	return ret;
}

int spi_claim_bus(struct spi_slave *slave)
{
	struct ipq_spi_slave *ds = to_ipq_spi(slave);
	unsigned int ret;

	ret = spi_hw_init(ds);
	if (ret)
		return -EIO;

	return SUCCESS;
}

void spi_release_bus(struct spi_slave *slave)
{
	struct ipq_spi_slave *ds = to_ipq_spi(slave);

	/* Reset the SPI hardware */
	spi_reset(ds);
	ds->initialized = 0;
}

static void write_force_cs(struct spi_slave *slave, int assert)
{
	struct ipq_spi_slave *ds = to_ipq_spi(slave);

	if (assert)
		clrsetbits_le32(ds->regs->io_control,
			FORCE_CS_MSK, FORCE_CS_EN);
	else
		clrsetbits_le32(ds->regs->io_control,
			FORCE_CS_MSK, FORCE_CS_DIS);

	return;

}

/*
 * Function to write data to OUTPUT FIFO
 */
static void spi_write_byte(struct ipq_spi_slave *ds, unsigned char data)
{
	/* Wait for space in the FIFO */
	while ((readl(ds->regs->qup_operational) & QUP_OUTPUT_FIFO_FULL))
		udelay(1);

	/* Write the byte of data */
	writel(data, ds->regs->qup_output_fifo);
}

/*
 * Function to read data from Input FIFO
 */
static unsigned char spi_read_byte(struct ipq_spi_slave *ds)
{
	/* Wait for Data in FIFO */
	while (!(readl(ds->regs->qup_operational) &
			QUP_DATA_AVAILABLE_FOR_READ)) {
		udelay(1);
	}

	/* Read a byte of data */
	return readl(ds->regs->qup_input_fifo) & 0xff;
}

/*
 * Function to check wheather Input or Output FIFO
 * has data to be serviced
 */
static int check_fifo_status(uint32_t reg_addr)
{
	unsigned int count = TIMEOUT_CNT;
	unsigned int status_flag;
	unsigned int val;

	do {
		val = readl(reg_addr);
		count--;
		if (count == 0)
			return -ETIMEDOUT;
		status_flag = ((val & OUTPUT_SERVICE_FLAG) | (val & INPUT_SERVICE_FLAG));
	} while (!status_flag);

	return SUCCESS;
}

/*
 * Function to configure Input and Output enable/disable
 */
static void enable_io_config(struct ipq_spi_slave *ds,
				uint32_t write_cnt, uint32_t read_cnt)
{
	if (write_cnt) {
		clrsetbits_le32(ds->regs->qup_config,
				QUP_CONF_OUTPUT_MSK, QUP_CONF_OUTPUT_ENA);
	} else {
		clrsetbits_le32(ds->regs->qup_config,
				QUP_CONF_OUTPUT_MSK, QUP_CONF_NO_OUTPUT);
	}

	if (read_cnt) {
		clrsetbits_le32(ds->regs->qup_config,
				QUP_CONF_INPUT_MSK, QUP_CONF_INPUT_ENA);
	} else {
		clrsetbits_le32(ds->regs->qup_config,
				QUP_CONF_INPUT_MSK, QUP_CONF_NO_INPUT);
	}

	return;
}

static void
blsp_spi_wait_for_data(uint32_t pipe_num)
{
	/* Wait for the descriptors to be processed */
	bam_wait_for_interrupt(&bam, pipe_num, P_PRCSD_DESC_EN_MASK);

	/* Read offset update for the circular FIFO */
	bam_read_offset_update(&bam, pipe_num);
}


static int blsp_spi_bam_begin_xfer(struct ipq_spi_slave *ds, const u8 *buffer,
                unsigned int bytes, unsigned int type)
{
	u32 tx_bytes_to_send = 0, rx_bytes_to_recv = 0;
	u32 n_words_xfr;
	u32 ret = 0;
	u32 prod_desc_cnt = SPI_BAM_MAX_DESC_NUM - 1;
	u32 cons_desc_cnt = SPI_BAM_MAX_DESC_NUM - 1;
	u32 tx_bytes_sent = 0;
	u32 rx_bytes_rcvd = 0;
	int state_config;
	u32 data_xfer_size;
	int cons_flag = 0, prod_flag = 0;
	int num_desc = 0;
	u32 rem_bytes = 0;

	rem_bytes = bytes;

	while (rem_bytes) {
		tx_bytes_to_send = min_t(u32, bytes,
			SPI_MAX_TRFR_BTWN_RESETS);

		rx_bytes_to_recv = min_t(u32, bytes,
			SPI_MAX_TRFR_BTWN_RESETS);

		if (bytes > SPI_MAX_TRFR_BTWN_RESETS)
			rem_bytes = bytes - SPI_MAX_TRFR_BTWN_RESETS;
		else
			rem_bytes = 0;

		writel(0, ds->regs->qup_mx_output_count);
		if (type == READ) {
			n_words_xfr = rx_bytes_to_recv;
			writel(n_words_xfr, ds->regs->qup_mx_input_count);
		}

		state_config = config_spi_state(ds, SPI_RUN_STATE);
		if (state_config)
			return state_config;


		if (type == WRITE) {
			if (cons_desc_cnt > 0) {
				data_xfer_size = tx_bytes_to_send;

				if (rem_bytes == 0 || cons_desc_cnt == 1)
					cons_flag = BAM_DESC_EOT_FLAG |
							BAM_DESC_NWD_FLAG;
				cons_flag |= BAM_DESC_INT_FLAG;
				bam_add_one_desc(&bam, DATA_CONSUMER_PIPE_INDEX,
						 (unsigned char*)(buffer),
						 data_xfer_size, cons_flag);
				num_desc++;
				bam_sys_gen_event(&bam, DATA_CONSUMER_PIPE_INDEX,
						  num_desc);
				blsp_spi_wait_for_data(DATA_CONSUMER_PIPE_INDEX);
				num_desc = 0;
				tx_bytes_sent += data_xfer_size;
				buffer = buffer + data_xfer_size;
				bytes = rem_bytes;
				cons_desc_cnt--;
				if (cons_desc_cnt == 0)
					cons_desc_cnt = SPI_BAM_MAX_DESC_NUM - 1;

			}
		} else if (type == READ) {
			if (prod_desc_cnt > 0) {
				data_xfer_size = rx_bytes_to_recv;
				if (rem_bytes == 0 || prod_desc_cnt == 1)
					prod_flag = (BAM_DESC_EOT_FLAG |
							BAM_DESC_NWD_FLAG);
				prod_flag |= BAM_DESC_INT_FLAG;
				bam_add_one_desc(&bam, DATA_PRODUCER_PIPE_INDEX,
						 (unsigned char*)(buffer), data_xfer_size, prod_flag);
				num_desc++;
				bam_sys_gen_event(&bam, DATA_PRODUCER_PIPE_INDEX,
						  num_desc);
				blsp_spi_wait_for_data(DATA_PRODUCER_PIPE_INDEX);
				num_desc = 0;
				rx_bytes_rcvd += data_xfer_size;
				buffer = buffer + data_xfer_size;
				bytes = rem_bytes;
				prod_desc_cnt--;
				if (prod_desc_cnt == 0)
					prod_desc_cnt = SPI_BAM_MAX_DESC_NUM - 1;
			}
		}
		state_config = config_spi_state(ds, SPI_RESET_STATE);
		if (state_config)
		return state_config;
	}

	return ret;
}

/*
 * Function to read bytes number of data from the Input FIFO
 */
static int __blsp_spi_read(struct ipq_spi_slave *ds, u8 *data_buffer,
				unsigned int bytes)
{
	uint32_t val;
	unsigned int i;
	unsigned int read_bytes = bytes;
	unsigned int fifo_count;
	int ret = SUCCESS;
	int state_config;


	/* Configure no of bytes to read */
	state_config = config_spi_state(ds, SPI_RESET_STATE);
	if (state_config)
		return state_config;

	/* Configure input and output enable */
	enable_io_config(ds, 0, read_bytes);

	if (!(ds->use_dma))
		writel(bytes, ds->regs->qup_mx_input_count);

	if (ds->use_dma && read_bytes)
		blsp_spi_bam_begin_xfer(ds, data_buffer,
			bytes, READ);
	else {
		state_config = config_spi_state(ds, SPI_RUN_STATE);
		if (state_config)
			return state_config;

		while (read_bytes) {
			ret = check_fifo_status(ds->regs->qup_operational);
			if (ret != SUCCESS)
				goto out;

			val = readl(ds->regs->qup_operational);
			if (val & INPUT_SERVICE_FLAG) {
				/*
				 * acknowledge to hw that software will
				 * read input data
				 */
				val &= INPUT_SERVICE_FLAG;
				writel(val, ds->regs->qup_operational);

				fifo_count = ((read_bytes > SPI_INPUT_BLOCK_SIZE) ?
						SPI_INPUT_BLOCK_SIZE : read_bytes);

				for (i = 0; i < fifo_count; i++) {
					*data_buffer = spi_read_byte(ds);
					data_buffer++;
					read_bytes--;
				}
			}
		}
	}
out:
	/*
	 * Put the SPI Core back in the Reset State
	 * to end the transfer
	 */
	(void)config_spi_state(ds, SPI_RESET_STATE);

	return ret;

}

static int blsp_spi_read(struct ipq_spi_slave *ds, u8 *data_buffer,
				unsigned int bytes)
{
	int length, ret;

	if (!(ds->use_dma)) {
		while (bytes) {
			length = (bytes < MAX_COUNT_SIZE) ? bytes : MAX_COUNT_SIZE;

			ret = __blsp_spi_read(ds, data_buffer, length);
			if (ret != SUCCESS)
				return ret;

			data_buffer += length;
			bytes -= length;
		}
	} else {
		ret = __blsp_spi_read(ds, data_buffer, bytes);
		if (ret != SUCCESS)
			return ret;
	}
	return 0;
}

/*
 * Function to write data to the Output FIFO
 */
static int __blsp_spi_write(struct ipq_spi_slave *ds, const u8 *cmd_buffer,
				unsigned int bytes)
{
	uint32_t val;
	unsigned int i;
	unsigned int write_len = bytes;
	unsigned int read_len = bytes;
	unsigned int fifo_count;
	int ret = SUCCESS;
	int state_config;

	state_config = config_spi_state(ds, SPI_RESET_STATE);
	if (state_config)
		return state_config;

	if (ds->use_dma) {
	/* No of bytes to be written in Output FIFO */
		writel(0, ds->regs->qup_mx_read_count);
		writel(0, ds->regs->qup_mx_write_count);
		writel(0, ds->regs->qup_mx_output_count);
		writel(0, ds->regs->qup_mx_input_count);
	}
	else {
		writel(bytes, ds->regs->qup_mx_output_count);
		writel(bytes, ds->regs->qup_mx_input_count);
		state_config = config_spi_state(ds, SPI_RUN_STATE);
		if (state_config)
			return state_config;
	}

	/* Configure input and output enable */
	if (!(ds->use_dma)) {
		enable_io_config(ds, write_len, read_len);
	} else {
		enable_io_config(ds, write_len, 0);
	}

	if (ds->use_dma && write_len)
		blsp_spi_bam_begin_xfer(ds, cmd_buffer,
							    bytes, WRITE);

	else {
		/*
		 * read_len considered to ensure that we read the dummy data for the
		 * write we performed. This is needed to ensure with WR-RD transaction
		 * to get the actual data on the subsequent read cycle that happens
		 */
		while (write_len || read_len) {
			ret = check_fifo_status(ds->regs->qup_operational);
			if (ret != SUCCESS)
				goto out;

			val = readl(ds->regs->qup_operational);
			if (val & OUTPUT_SERVICE_FLAG) {
				/*
				 * acknowledge to hw that software will write
				 * expected output data
				 */
				val &= OUTPUT_SERVICE_FLAG;
				writel(val, ds->regs->qup_operational);

				if (write_len > SPI_OUTPUT_BLOCK_SIZE)
					fifo_count = SPI_OUTPUT_BLOCK_SIZE;
				else
					fifo_count = write_len;

				for (i = 0; i < fifo_count; i++) {
					/* Write actual data to output FIFO */
					spi_write_byte(ds, *cmd_buffer);
					cmd_buffer++;
					write_len--;
				}
			}
			if (val & INPUT_SERVICE_FLAG) {
				/*
				 * acknowledge to hw that software
				 * will read input data
				 */
				val &= INPUT_SERVICE_FLAG;
				writel(val, ds->regs->qup_operational);

				if (read_len > SPI_INPUT_BLOCK_SIZE)
					fifo_count = SPI_INPUT_BLOCK_SIZE;
				else
					fifo_count = read_len;

				for (i = 0; i < fifo_count; i++) {
					/* Read dummy data for the data written */
					(void)spi_read_byte(ds);

					/* Decrement the write count after reading the
					 * dummy data from the device. This is to make
					 * sure we read dummy data before we write the
					 * data to fifo
					 */
					read_len--;
				}
			}
		}

	}
out:
	/*
	 * Put the SPI Core back in the Reset State
	 * to end the transfer
	 */
	(void)config_spi_state(ds, SPI_RESET_STATE);

	return ret;
}

static int blsp_spi_write(struct ipq_spi_slave *ds, const u8 *cmd_buffer,
                                unsigned int bytes)
{
	int length, ret;

	if (!(ds->use_dma)) {
		while (bytes) {
			length = (bytes < MAX_COUNT_SIZE) ? bytes : MAX_COUNT_SIZE;

			ret = __blsp_spi_write(ds, cmd_buffer, length);
			if (ret != SUCCESS)
				return ret;

			cmd_buffer += length;
			bytes -= length;
		}
	} else {
		ret = __blsp_spi_write(ds, cmd_buffer, bytes);
		if (ret != SUCCESS)
			return ret;
	}

	return 0;
}
/*
 * This function is invoked with either tx_buf or rx_buf.
 * Calling this function with both null does a chip select change.
 */
int spi_xfer(struct spi_slave *slave, unsigned int bitlen,
		const void *dout, void *din, unsigned long flags)
{
	struct ipq_spi_slave *ds = to_ipq_spi(slave);
	unsigned int len;
	const u8 *txp = dout;
	u8 *rxp = din;
	int ret = SUCCESS;

	if (bitlen & SPI_BITLEN_MSK) {
		printf("err : Invalid bit length");
		return -EINVAL;
	}

	len = bitlen >> 3;

	if (flags & SPI_XFER_BEGIN) {
		if (!(ds->use_dma)) {
			ret = spi_hw_init(ds);
			if (ret != SUCCESS)
				return ret;
		}

		if (ds->slave.cs == CONFIG_SF_SPI_NAND_CS) {
			setbits_le32(ds->regs->io_control, CS_POLARITY_MASK);
			CS_change(ds->slave.bus, ds->slave.cs, CS_ASSERT);
		} else {
			write_force_cs(slave, 1);
		}
	}

	if (dout != NULL) {
		ret = blsp_spi_write(ds, txp, len);
		if (ret != SUCCESS)
			return ret;
	}

	if (din != NULL) {
		ret = blsp_spi_read(ds, rxp, len);
		if (ret != SUCCESS)
		return ret;
	}

	if (flags & SPI_XFER_END) {
		/* To handle only when chip select change is needed */
		if (ds->slave.cs == CONFIG_SF_SPI_NAND_CS) {
			clrbits_le32(ds->regs->io_control, CS_POLARITY_MASK);
			CS_change(ds->slave.bus, ds->slave.cs, CS_DEASSERT);
		} else {
			write_force_cs(slave, 0);
		}
	}

	return ret;
}

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return 1;
}

void spi_cs_activate(struct spi_slave *slave)
{

}

void spi_cs_deactivate(struct spi_slave *slave)
{

}
