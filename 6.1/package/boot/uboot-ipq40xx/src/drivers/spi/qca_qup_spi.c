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
#include <asm/arch-ipq40xx/iomap.h>
#include "qca_qup_spi.h"

static int check_bit_state(uint32_t reg_addr, int bit_num, int val, int us_delay)
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
	int ret = SUCCESS;

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

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
				unsigned int max_hz, unsigned int mode)
{
	struct ipq_spi_slave *ds;

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
	if ((bus < BLSP0_SPI) || (bus > BLSP1_SPI)
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

	ds->initialized = 1;

	return SUCCESS;
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

	writel(bytes, ds->regs->qup_mx_input_count);

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

	while (bytes) {
		length = (bytes < MAX_COUNT_SIZE) ? bytes : MAX_COUNT_SIZE;

		ret = __blsp_spi_read(ds, data_buffer, length);
		if (ret != SUCCESS)
			return ret;

		data_buffer += length;
		bytes -= length;
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

	/* No of bytes to be written in Output FIFO */
	writel(bytes, ds->regs->qup_mx_output_count);
	writel(bytes, ds->regs->qup_mx_input_count);
	state_config = config_spi_state(ds, SPI_RUN_STATE);
	if (state_config)
		return state_config;

	/* Configure input and output enable */
	enable_io_config(ds, write_len, read_len);

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

				/* Decrement the write count after reading the dummy data
				 * from the device. This is to make sure we read dummy data
				 * before we write the data to fifo
				 */
				read_len--;
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

static int blsp_spi_write(struct ipq_spi_slave *ds, u8 *cmd_buffer,
                                unsigned int bytes)
{
	int length, ret;

	while (bytes) {
		length = (bytes < MAX_COUNT_SIZE) ? bytes : MAX_COUNT_SIZE;

		ret = __blsp_spi_write(ds, cmd_buffer, length);
		if (ret != SUCCESS)
			return ret;

		cmd_buffer += length;
		bytes -= length;
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
	int ret;

	if (bitlen & 0x07) {
		printf("err : Invalid bit length");
		return -EINVAL;
	}

	len = bitlen >> 3;

	if (flags & SPI_XFER_BEGIN) {
		ret = spi_hw_init(ds);
		if (ret != SUCCESS)
			return ret;
		write_force_cs(ds, 1);
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
		write_force_cs(ds, 0);
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
