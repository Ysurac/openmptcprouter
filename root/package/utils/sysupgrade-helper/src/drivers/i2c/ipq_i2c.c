/*
 * Copyright (c) 2014, 2016 The Linux Foundation. All rights reserved.
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

#include <common.h>
#include <i2c.h>
#include "ipq_i2c.h"
#include <asm/io.h>
#include <asm/errno.h>
#include <asm/arch-ipq806x/gsbi.h>
#include <asm/arch-ipq806x/gpio.h>
#include <asm/arch-ipq806x/iomap.h>
#include <asm/arch-ipq806x/clock.h>

static int gsbi_port;
static int gsbi_base_addr;
static int i2c_hw_initialized;
static int i2c_board_initialized;
static uint32_t i2c_master_clk_ctl;

/*
 * Reset entire QUP and all mini cores
 */
static void i2c_reset(void)
{
	writel(0x1, GSBI_QUP_BASE(gsbi_port) + QUP_SW_RESET_OFFSET);
	udelay(5);
}

static int check_bit_state(uint32_t reg_addr, int bit_num, int val,
				int us_delay)
{
	unsigned int count = TIMEOUT_CNT;
	unsigned int bit_val = ((readl(reg_addr) >> bit_num) & 0x01);

	while (bit_val != val) {
		count--;
		if (count == 0) {
			return -ETIMEDOUT;
		}
		udelay(us_delay);
		bit_val = ((readl(reg_addr) >> bit_num) & 0x01);
	}

	return SUCCESS;
}

/*
 * Check whether GSBIn_QUP State is valid
 */
static int check_qup_state_valid(void)
{
	return check_bit_state(GSBI_QUP_BASE(gsbi_port) + QUP_STATE_OFFSET,
				QUP_STATE_VALID_BIT,
				QUP_STATE_VALID, 1);
}

/*
 * Configure GSBIn Core state
 */
static int config_i2c_state(unsigned int state)
{
	uint32_t val;
	int ret = SUCCESS;

	ret = check_qup_state_valid();
	if (ret != SUCCESS)
		return ret;

	/* Set the state  */
	val = readl(GSBI_QUP_BASE(gsbi_port) + QUP_STATE_OFFSET);
	val = ((val & ~QUP_STATE_MASK) | state);
	writel(val, GSBI_QUP_BASE(gsbi_port) + QUP_STATE_OFFSET);
	ret = check_qup_state_valid();

	return ret;
}

/*
 * Configure I2C IO Mode.
 */
void config_i2c_mode(void)
{
	int cfg;

	cfg = readl(GSBI_QUP_BASE(gsbi_port) + QUP_IO_MODES_OFFSET);
	cfg |= (INPUT_FIFO_MODE |
		OUTPUT_FIFO_MODE |
		OUTPUT_BIT_SHIFT_EN);
	writel(cfg, GSBI_QUP_BASE(gsbi_port) + QUP_IO_MODES_OFFSET);
}

void i2c_ipq_board_init(void)
{
	gsbi_base_addr = gboard_param->i2c_gsbi_base;
	gsbi_port = gboard_param->i2c_gsbi;

	/*
	 * Currently I2C GPIO lines (12(SDA), 13(SCLK)) are configured by
	 * RPM firmware for PMIC. Trustzone firmware configures the TLMM
	 * RPU in such a way that only RPM will be able to configure those
	 * GPIOs. If Krait non secure (u-boot) tries to write to those GPIO
	 * registers will lead to Master port L2 error.
	 * GSBI4 uses GPIO 12 and 13 lines for I2C.
	 * So for GSBI4 we will not update the GPIO configs. RPM would have
	 * already configured this.`
	 */

	/* Configure the GPIOs */
	if (gsbi_port != GSBI_4)
		ipq_configure_gpio(gboard_param->i2c_gpio, NO_OF_I2C_GPIOS);

	/* Configure the I2C clock */
	i2c_clock_config(gboard_param->i2c_gsbi,
		gboard_param->i2c_mnd_value.m_value,
		gboard_param->i2c_mnd_value.n_value,
		gboard_param->i2c_mnd_value.d_value,
		gboard_param->clk_dummy);

	i2c_hw_initialized = 0;
	i2c_board_initialized = 1;
}

/*
 * GSBIn I2C Hardware Initialisation
 */
static int i2c_hw_init(void)
{
	int ret, cfg;

	/* GSBI module configuration */
	i2c_reset();

	/* Set the GSBIn QUP state */
	ret = config_i2c_state(QUP_STATE_RESET);
	if (ret)
		return ret;

	/* Configure GSBI_CTRL register to set protocol_mode to I2C_UART:110 */
	writel(GSBI_PROTOCOL_CODE_I2C_UART <<
			GSBI_CTRL_REG_PROTOCOL_CODE_S,
			GSBI_CTRL_REG(gsbi_base_addr));

	/* Configure Mini core to I2C core */
	cfg = readl(GSBI_QUP_BASE(gsbi_port) + QUP_CONFIG_OFFSET);
	cfg |= (QUP_CONFIG_MINI_CORE_I2C |
		I2C_BIT_WORD);
	writel(cfg, GSBI_QUP_BASE(gsbi_port) + QUP_CONFIG_OFFSET);

	/* Configure I2C mode */
	config_i2c_mode();

	/* Enable QUP Error Flags */
	writel(ERROR_FLAGS_EN,
		GSBI_QUP_BASE(gsbi_port) + QUP_ERROR_FLAGS_EN_OFFSET);

	/* Clear the MASTER_CTL_STATUS */
	writel(I2C_MASTER_STATUS_CLEAR,
		GSBI_QUP_BASE(gsbi_port) + QUP_I2C_MASTER_STATUS_OFFSET);

	/* Set to RUN STATE */
	ret = config_i2c_state(QUP_STATE_RUN);
	if (ret)
		return ret;

	/* Configure the I2C Master clock */
	i2c_master_clk_ctl = ((QUP_INPUT_CLK / (CONFIG_I2C_CLK_FREQ * 2)) - 3) & 0xff;
	writel(i2c_master_clk_ctl, GSBI_QUP_BASE(gsbi_port) + QUP_I2C_MASTER_CLK_CTL_OFFSET);

	i2c_hw_initialized = 1;

	return SUCCESS;
}

/*
 * Function to check wheather Input or Output FIFO
 * has data to be serviced. For invalid slaves, this
 * flag will not be set.
 */
static int check_fifo_status(uint dir)
{
	unsigned int count = TIMEOUT_CNT;
	unsigned int status_flag;
	unsigned int val;

	if (dir == READ) {
		do {
			val = readl(GSBI_QUP_BASE(gsbi_port)
				+ QUP_OPERATIONAL_OFFSET);
			count--;
			if (count == 0)
				return -ETIMEDOUT;
			status_flag = val & INPUT_SERVICE_FLAG;
			udelay(10);
		} while (!status_flag);

	} else if (dir == WRITE) {
		do {
			val = readl(GSBI_QUP_BASE(gsbi_port)
				+ QUP_OPERATIONAL_OFFSET);
			count--;
			if (count == 0)
				return -ETIMEDOUT;
			status_flag = val & OUTPUT_FIFO_FULL;
			udelay(10);
		} while (status_flag);

		/*
		 * Clear the flag and Acknowledge that the
		 * software has or will write the data.
		 */
		if (readl(GSBI_QUP_BASE(gsbi_port) + QUP_OPERATIONAL_OFFSET)
						& OUTPUT_SERVICE_FLAG) {
			writel(OUTPUT_SERVICE_FLAG, GSBI_QUP_BASE(gsbi_port)
				+ QUP_OPERATIONAL_OFFSET);
		}
	}
	return SUCCESS;
}

/*
 * Check whether the values in the OUTPUT FIFO are shifted out.
 */
static int check_write_done(void)
{
	unsigned int count = TIMEOUT_CNT;
	unsigned int status_flag;
	unsigned int val;

	do {
		val = readl(GSBI_QUP_BASE(gsbi_port)
				+ QUP_OPERATIONAL_OFFSET);
		count--;
		if (count == 0)
			return -ETIMEDOUT;
		status_flag = val & OUTPUT_FIFO_NOT_EMPTY;
		udelay(10);
	} while (status_flag);

	return SUCCESS;
}

int i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	int ret = 0;
	unsigned int data = 0;

	if (!i2c_board_initialized) {
		i2c_ipq_board_init();
	}

	if(!i2c_hw_initialized) {
		i2c_hw_init();
	}

	writel(0xFF, GSBI_QUP_BASE(gsbi_port)
			+ QUP_ERROR_FLAGS_OFFSET);

	writel(0, GSBI_QUP_BASE(gsbi_port)
			+ QUP_I2C_MASTER_STATUS_OFFSET);

	/* Set to RUN state */
	ret = config_i2c_state(QUP_STATE_RUN);
	if (ret != SUCCESS)
		goto out;

	/* Configure the I2C Master clock */
	writel(i2c_master_clk_ctl, GSBI_QUP_BASE(gsbi_port) + QUP_I2C_MASTER_CLK_CTL_OFFSET);

	/* Send a write request to the chip */
	writel((QUP_I2C_START_SEQ | QUP_I2C_ADDR(chip)),
		GSBI_QUP_BASE(gsbi_port) + QUP_OUTPUT_FIFO_OFFSET);

	writel((QUP_I2C_DATA_SEQ | QUP_I2C_DATA(addr)),
		 GSBI_QUP_BASE(gsbi_port) + QUP_OUTPUT_FIFO_OFFSET);

	ret = check_write_done();
	if (ret != SUCCESS)
		goto out;

	ret = check_fifo_status(WRITE);
	if (ret != SUCCESS)
		goto out;

	/* Send read request */
	writel((QUP_I2C_START_SEQ |
		(QUP_I2C_ADDR(chip)|
		QUP_I2C_SLAVE_READ)),
		GSBI_QUP_BASE(gsbi_port) + QUP_OUTPUT_FIFO_OFFSET);

	writel((QUP_I2C_RECV_SEQ | len),
		GSBI_QUP_BASE(gsbi_port) + QUP_OUTPUT_FIFO_OFFSET);

	while (len--) {

		ret = check_fifo_status(READ);
		if (ret != SUCCESS)
			goto out;

		/* Read the data from the FIFO */
		data = readl(GSBI_QUP_BASE(gsbi_port) + QUP_INPUT_FIFO_OFFSET);
		*buffer = QUP_I2C_DATA(data);

		/*
		 * Clear the flag and Acknowledge that the
		 * software has or will read the data.
		 */
		writel(INPUT_SERVICE_FLAG,
			GSBI_QUP_BASE(gsbi_port) + QUP_OPERATIONAL_OFFSET);

		buffer++;
	}

	/* Set to PAUSE state */
	ret = config_i2c_state(QUP_STATE_PAUSE);
	if (ret != SUCCESS)
		goto out;

out:
	/*
	 * Put the I2C Core back in the Reset State to end the transfer.
	 */
	(void)config_i2c_state(QUP_STATE_RESET);
	return ret;
}

int i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	int ret = 0;
	int idx = 0;

	if (!i2c_board_initialized) {
		i2c_ipq_board_init();
	}

	if(!i2c_hw_initialized) {
		i2c_hw_init();
	}

	/* Set to RUN state */
	ret = config_i2c_state(QUP_STATE_RUN);
	if (ret != SUCCESS)
		goto out;

	/* Configure the I2C Master clock */
	writel(i2c_master_clk_ctl, GSBI_QUP_BASE(gsbi_port) + QUP_I2C_MASTER_CLK_CTL_OFFSET);

	/* Send the write request */
	writel((QUP_I2C_START_SEQ | QUP_I2C_ADDR(chip)),
		GSBI_QUP_BASE(gsbi_port) + QUP_OUTPUT_FIFO_OFFSET);
	writel((QUP_I2C_DATA_SEQ | QUP_I2C_DATA(addr)),
		GSBI_QUP_BASE(gsbi_port) + QUP_OUTPUT_FIFO_OFFSET);

	while (len) {
		if (len == 1) {
			writel((QUP_I2C_STOP_SEQ | QUP_I2C_DATA(buffer[idx])),
			GSBI_QUP_BASE(gsbi_port) + QUP_OUTPUT_FIFO_OFFSET);
		} else {
			writel((QUP_I2C_DATA_SEQ | QUP_I2C_DATA(buffer[idx])),
			GSBI_QUP_BASE(gsbi_port) + QUP_OUTPUT_FIFO_OFFSET);
		}
		len--;
		idx++;

		ret = check_fifo_status(WRITE);
		if (ret != SUCCESS)
			goto out;
	}

	ret = check_write_done();
	if (ret != SUCCESS)
		goto out;

	/* Set to PAUSE state */
	ret = config_i2c_state(QUP_STATE_PAUSE);
	if (ret != SUCCESS)
		goto out;

	return ret;
out:
	/*
	 * Put the I2C Core back in the Reset State to end the transfer.
	 */
	(void)config_i2c_state(QUP_STATE_RESET);
	return ret;
}


/*
 * Probe the given I2C chip address.
 * Returns 0 if a chip responded.
 */
int i2c_probe(uchar chip)
{
	uchar buf;

	return i2c_read(chip, 0x0, 0x1, &buf, 0x1);
}

void i2c_init(int speed, int slaveaddr)
{
	debug("i2c_init(speed=%u, slaveaddr=0x%x)\n", speed, slaveaddr);
}


