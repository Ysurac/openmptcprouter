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

#ifndef QCA_CLK_H
#define QCA_CLK_H

#define MMC_IDENTIFY_MODE	0
#define MMC_DATA_TRANSFER_MODE	1

#define GCC_BLSP1_UART2_APPS_CFG_RCGR	0x01803038
#define GCC_BLSP1_UART2_APPS_M		0x0180303C
#define GCC_BLSP1_UART2_APPS_N		0x01803040
#define GCC_BLSP1_UART2_APPS_D		0x01803044
#define GCC_BLSP1_UART2_APPS_CMD_RCGR	0x01803034
#define GCC_BLSP1_UART2_APPS_CBCR	0x0180302C

#define GCC_UART_CFG_RCGR_MODE_MASK	0x3000
#define GCC_UART_CFG_RCGR_SRCSEL_MASK	0x0700
#define GCC_UART_CFG_RCGR_SRCDIV_MASK	0x001F

#define GCC_UART_CFG_RCGR_MODE_SHIFT	12
#define GCC_UART_CFG_RCGR_SRCSEL_SHIFT	8
#define GCC_UART_CFG_RCGR_SRCDIV_SHIFT	0

#define UART2_RCGR_SRC_SEL	0x0
#define UART2_RCGR_SRC_DIV	0x0
#define UART2_RCGR_MODE		0x2
#define UART2_CMD_RCGR_UPDATE	0x1
#define UART2_CBCR_CLK_ENABLE	0x1

#define NOT_2D(two_d)		(~two_d)
#define NOT_N_MINUS_M(n,m)	(~(n - m))
#define CLOCK_UPDATE_TIMEOUT_US	1000

#ifdef CONFIG_IPQ40XX_I2C
#define GCC_BLSP1_QUP1_I2C_APPS_CFG_RCGR	0x01802010

#define GCC_BLSP1_QUP1_I2C_APPS_M		0x0180303C
#define GCC_BLSP1_QUP1_I2C_APPS_N		0x01803040
#define GCC_BLSP1_QUP1_I2C_APPS_D		0x01803044
#define GCC_BLSP1_QUP1_I2C_APPS_CMD_RCGR	0x0180200C
#define GCC_BLSP1_QUP1_I2C_APPS_CBCR		0x01802008

#define GCC_I2C_CFG_RCGR_SRCSEL_MASK	0x0700
#define GCC_I2C_CFG_RCGR_SRCDIV_MASK	0x001F

#define GCC_I2C_CFG_RCGR_SRCSEL_SHIFT	8
#define GCC_I2C_CFG_RCGR_SRCDIV_SHIFT	0

#define I2C0_RCGR_SRC_SEL	1
#define I2C0_RCGR_SRC_DIV	20
#define I2C0_CMD_RCGR_UPDATE	0x1
#define I2C0_CBCR_CLK_ENABLE	0x1
#define NOT_2D(two_d)		(~two_d)
#define NOT_N_MINUS_M(n,m)	(~(n - m))
#define CLOCK_UPDATE_TIMEOUT_US	1000
#endif

#define GCC_PCIE_SLEEP_CBCR	0x0181D014
#define GCC_PCIE_AXI_M_CBCR	0x0181D004
#define GCC_PCIE_AXI_S_CBCR	0x0181D008
#define GCC_PCIE_AHB_CBCR	0x0181D00C
#define PCIE_TIMEOUT_CNT	100
#define ENABLE			0x1
#define DISABLE			0x0
#define BIT(s)			(1<<s)

void emmc_clock_config(int mode);
void emmc_clock_disable(void);

/* UART clocks configuration */
void uart2_clock_config(unsigned int m,
			unsigned int n, unsigned int two_d);
void uart2_toggle_clock(void);
int uart2_trigger_update(void);
void uart2_set_rate_mnd(unsigned int m,
			unsigned int n, unsigned int two_d);
void uart2_configure_mux(void);

/* I2C clocks configuration */
#ifdef CONFIG_IPQ40XX_I2C
void i2c_clock_config(void);
void i2c0_toggle_clock(void);
int i2c0_trigger_update(void);
void i2c0_configure_mux(void);
#endif

int pcie_clock_enable(int clk_addr);
void pcie_clock_disable(int clk_addr);
#endif /*QCA_CLK_H*/
