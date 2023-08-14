/*
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <asm/arch-qcom-common/clk.h>
#include <asm/arch-ipq40xx/iomap.h>
#include <asm/io.h>
#include <asm/errno.h>

#define GCC_SDCC1_MISC  0x1818014
#define GCC_SDCC1_APPS_CBCR 0x181800C
#define GCC_SDCC1_APPS_RCGR 0x1818008
#define GCC_SDCC1_APPS_CMD_RCGR 0x1818004

void emmc_clock_config(int mode)
{
	/* Select SDCC clock source as DDR_PLL_SDCC1_CLK  192MHz */
	writel(0x100, GCC_SDCC1_APPS_RCGR);
	/* Update APPS_CMD_RCGR to reflect source selection */
	writel(0x1, GCC_SDCC1_APPS_CMD_RCGR);
	udelay(10);

	if (mode == MMC_IDENTIFY_MODE) {
		/* Set root clock generator to bypass mode */
		writel(0x0, GCC_SDCC1_APPS_CBCR);
		udelay(10);
		/* Choose divider for 400KHz */
		writel(0x1e4 , GCC_SDCC1_MISC);
		/* Enable root clock generator */
		writel(0x1, GCC_SDCC1_APPS_CBCR);
		udelay(10);
	}
	if (mode == MMC_DATA_TRANSFER_MODE) {
		/* Set root clock generator to bypass mode */
		writel(0x0, GCC_SDCC1_APPS_CBCR);
		udelay(10);
		/* Choose divider for 48MHz */
		writel(0x3, GCC_SDCC1_MISC);
		/* Enable root clock generator */
		writel(0x1, GCC_SDCC1_APPS_CBCR);
		udelay(10);
	}
}
void emmc_clock_disable(void)
{
	/* Clear divider */
	writel(0x0, GCC_SDCC1_MISC);

}

void uart2_configure_mux(void)
{
	unsigned long cfg_rcgr;

	cfg_rcgr = readl(GCC_BLSP1_UART2_APPS_CFG_RCGR);
	/* Clear mode, src sel, src div */
	cfg_rcgr &= ~(GCC_UART_CFG_RCGR_MODE_MASK |
			GCC_UART_CFG_RCGR_SRCSEL_MASK |
			GCC_UART_CFG_RCGR_SRCDIV_MASK);

	cfg_rcgr |= ((UART2_RCGR_SRC_SEL << GCC_UART_CFG_RCGR_SRCSEL_SHIFT)
			& GCC_UART_CFG_RCGR_SRCSEL_MASK);

	cfg_rcgr |= ((UART2_RCGR_SRC_DIV << GCC_UART_CFG_RCGR_SRCDIV_SHIFT)
			& GCC_UART_CFG_RCGR_SRCDIV_MASK);

	cfg_rcgr |= ((UART2_RCGR_MODE << GCC_UART_CFG_RCGR_MODE_SHIFT)
			& GCC_UART_CFG_RCGR_MODE_MASK);

	writel(cfg_rcgr, GCC_BLSP1_UART2_APPS_CFG_RCGR);
}

void uart2_set_rate_mnd(unsigned int m,
			unsigned int n, unsigned int two_d)
{
	writel(m, GCC_BLSP1_UART2_APPS_M);
	writel(NOT_N_MINUS_M(n, m), GCC_BLSP1_UART2_APPS_N);
	writel(NOT_2D(two_d), GCC_BLSP1_UART2_APPS_D);
}

int uart2_trigger_update(void)
{
	unsigned long cmd_rcgr;
	int timeout = 0;

	cmd_rcgr = readl(GCC_BLSP1_UART2_APPS_CMD_RCGR);
	cmd_rcgr |= UART2_CMD_RCGR_UPDATE;
	writel(cmd_rcgr, GCC_BLSP1_UART2_APPS_CMD_RCGR);

	while (readl(GCC_BLSP1_UART2_APPS_CMD_RCGR) & UART2_CMD_RCGR_UPDATE) {
		if (timeout++ >= CLOCK_UPDATE_TIMEOUT_US) {
			printf("Timeout waiting for UART2 clock update\n");
			return -ETIMEDOUT;
		}
		udelay(1);
	}
	cmd_rcgr = readl(GCC_BLSP1_UART2_APPS_CMD_RCGR);
	return 0;
}

void uart2_toggle_clock(void)
{
	unsigned long cbcr_val;

	cbcr_val = readl(GCC_BLSP1_UART2_APPS_CBCR);
	cbcr_val |= UART2_CBCR_CLK_ENABLE;
	writel(cbcr_val, GCC_BLSP1_UART2_APPS_CBCR);
}

void uart2_clock_config(unsigned int m,
			unsigned int n, unsigned int two_d)
{
	uart2_configure_mux();
	uart2_set_rate_mnd(m, n, two_d);
	uart2_trigger_update();
	uart2_toggle_clock();
}

#ifdef CONFIG_IPQ40XX_I2C
void i2c0_configure_mux(void)
{
	unsigned long cfg_rcgr;

	cfg_rcgr = readl(GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR);
	/* Clear mode, src sel, src div */
	cfg_rcgr &= ~(GCC_I2C_CFG_RCGR_SRCSEL_MASK |
			GCC_I2C_CFG_RCGR_SRCDIV_MASK);

	cfg_rcgr |= ((I2C0_RCGR_SRC_SEL << GCC_I2C_CFG_RCGR_SRCSEL_SHIFT)
			& GCC_UART_CFG_RCGR_SRCSEL_MASK);

	cfg_rcgr |= ((I2C0_RCGR_SRC_DIV << GCC_I2C_CFG_RCGR_SRCDIV_SHIFT)
			& GCC_UART_CFG_RCGR_SRCDIV_MASK);

	writel(cfg_rcgr, GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR);
}

int i2c0_trigger_update(void)
{
	unsigned long cmd_rcgr;
	int timeout = 0;

	cmd_rcgr = readl(GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR);
	cmd_rcgr |= I2C0_CMD_RCGR_UPDATE;
	writel(cmd_rcgr, GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR);

	while (readl(GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR) & I2C0_CMD_RCGR_UPDATE) {
		if (timeout++ >= CLOCK_UPDATE_TIMEOUT_US) {
			printf("Timeout waiting for I2C0 clock update\n");
			return -ETIMEDOUT;
		}
		udelay(1);
	}
	cmd_rcgr = readl(GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR);
	return 0;
}

void i2c0_toggle_clock(void)
{
	unsigned long cbcr_val;

	cbcr_val = readl(GCC_BLSP1_QUP1_I2C_APPS_CBCR);
	cbcr_val |= I2C0_CBCR_CLK_ENABLE;
	writel(cbcr_val, GCC_BLSP1_QUP1_I2C_APPS_CBCR);
}

void i2c_clock_config(void)
{
	i2c0_configure_mux();
	i2c0_trigger_update();
	i2c0_toggle_clock();
}
#endif

int pcie_clock_enable(int clk_addr)
{
	unsigned int count = PCIE_TIMEOUT_CNT;
	int state, val;

	writel(ENABLE, clk_addr);
	do {
		val = readl(clk_addr);
		count--;
		if (count == 0) {
			printf("Timeout waiting for %d enable \n", clk_addr);
			return -ETIMEDOUT;
		}
		state = (val & BIT(31));
		udelay(10);
	} while (state);
	return 0;
}

void pcie_clock_disable(int clk_addr)
{
	writel(0, clk_addr);
}
