/*
 * Copyright (c) 2013-2016 The Linux Foundation. All rights reserved.
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

#ifndef   _IPQ806X_BOARD_PARAM_H_
#define   _IPQ806X_BOARD_PARAM_H_

#include <asm/arch-ipq806x/iomap.h>
#include "ipq806x_cdp.h"
#include "../board/qcom/common/athrs17_phy.h"
#include <asm/arch-ipq806x/gpio.h>
#include <asm/arch-ipq806x/nss/msm_ipq806x_gmac.h>
#include <phy.h>
#include <asm/sizes.h>
#include <asm/arch-ipq806x/clock.h>

gpio_func_data_t gmac0_gpio[] = {
	{
		.gpio = 0,
		.func = 1,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 1,
		.func = 1,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_ENABLE
	},
	{
		.gpio = 2,
		.func = 0,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 66,
		.func = 0,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_16MA,
		.oe = GPIO_OE_DISABLE
	},
};

gpio_func_data_t gmac1_gpio[] = {
	{
		.gpio = 0,
		.func = 1,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 1,
		.func = 1,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_ENABLE
	},
	{
		.gpio = 51,
		.func = 2,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 52,
		.func = 2,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 59,
		.func = 2,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 60,
		.func = 2,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 61,
		.func = 2,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 62,
		.func = 2,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 27,
		.func = 2,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_ENABLE
	},
	{
		.gpio = 28,
		.func = 2,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_ENABLE
	},
	{
		.gpio = 29,
		.func = 2,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_ENABLE
	},
	{
		.gpio = 30,
		.func = 2,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_ENABLE
	},
	{
		.gpio = 31,
		.func = 2,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_ENABLE
	},
	{
		.gpio = 32,
		.func = 2,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_ENABLE
	},
};

gpio_func_data_t gmac3_gpio[] = {
	{
		.gpio = 0,
		.func = 1,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 1,
		.func = 1,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_ENABLE
	},
	{
		.gpio = 2,
		.func = 0,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 32,
		.func = 0,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_DISABLE
	},
};

gpio_func_data_t storm_switch_gpio = {
	.gpio = 26,
	.func = 0,
	.out = GPIO_OUT_LOW,
	.pull = GPIO_PULL_DOWN,
	.drvstr = GPIO_8MA,
	.oe = GPIO_OE_ENABLE
};

gpio_func_data_t reset_s17_gpio = {
	.gpio = 63,
	.func = 0,
	.out = GPIO_OUTPUT,
	.pull = GPIO_PULL_DOWN,
	.drvstr = GPIO_8MA,
	.oe = GPIO_OE_ENABLE
};

gpio_func_data_t reset_ak01_8033_gpio = {
	.gpio = 32,
	.func = 0,
	.out = GPIO_OUTPUT,
	.pull = GPIO_PULL_UP,
	.drvstr = GPIO_12MA,
	.oe = GPIO_OE_ENABLE
};

gpio_func_data_t ar8033_gpio[] = {
	{
		.gpio = 2,
		.func = 0,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 66,
		.func = 0,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_16MA,
		.oe = GPIO_OE_DISABLE
	},
};

#ifdef CONFIG_IPQ806X_PCI
/* Address of PCIE20 PARF */
#define PCIE20_0_PARF_PHYS      0x1b600000
#define PCIE20_1_PARF_PHYS      0x1b800000
#define PCIE20_2_PARF_PHYS      0x1ba00000

/* Address of PCIE20 ELBI */
#define PCIE20_0_ELBI_PHYS      0x1b502000
#define PCIE20_1_ELBI_PHYS      0x1b702000
#define PCIE20_2_ELBI_PHYS      0x1b902000


/* Address of PCIE20 */
#define PCIE20_0_PHYS           0x1b500000
#define PCIE20_1_PHYS           0x1b700000
#define PCIE20_2_PHYS           0x1b900000
#define PCIE20_SIZE             SZ_4K

#define PCIE20_0_AXI_BAR_PHYS	0x08000000
#define PCIE20_0_AXI_BAR_SIZE	SZ_128M
#define PCIE20_1_AXI_BAR_PHYS	0x2E000000
#define PCIE20_1_AXI_BAR_SIZE	SZ_64M
#define PCIE20_2_AXI_BAR_PHYS	0x32000000
#define PCIE20_2_AXI_BAR_SIZE	SZ_64M
#define PCIE_AXI_CONF_SIZE	SZ_1M

#define MSM_PCIE_DEV_CFG_ADDR   0x01000000

#define PCIE20_PLR_IATU_VIEWPORT        0x900
#define PCIE20_PLR_IATU_CTRL1           0x904
#define PCIE20_PLR_IATU_CTRL2           0x908
#define PCIE20_PLR_IATU_LBAR            0x90C
#define PCIE20_PLR_IATU_UBAR            0x910
#define PCIE20_PLR_IATU_LAR             0x914
#define PCIE20_PLR_IATU_LTAR            0x918
#define PCIE20_PLR_IATU_UTAR            0x91c

#define PCIE20_PARF_CONFIG_BITS        0x50
#define PCIE20_ELBI_SYS_CTRL           0x04


#define PCIE20_0_RESET            (MSM_CLK_CTL_BASE + 0x22DC)
#define PCIE20_1_RESET            (MSM_CLK_CTL_BASE + 0x3A9C)
#define PCIE20_2_RESET            (MSM_CLK_CTL_BASE + 0x3ADC)
#define PCIE20_CAP                     0x70
#define PCIE20_CAP_LINKCTRLSTATUS      (PCIE20_CAP + 0x10)

#define PCIE_RST_GPIO           3
#define PCIE_1_RST_GPIO         48
#define PCIE_2_RST_GPIO         63
#define WIFI_PCIE_1_POWER_GPIO	9
#define WIFI_PCIE_2_POWER_GPIO	26

gpio_func_data_t pci_0_rst_gpio = {
		.gpio = PCIE_RST_GPIO,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_12MA,
		.oe = GPIO_OE_ENABLE
};

gpio_func_data_t pci_1_rst_gpio = {
		.gpio = PCIE_1_RST_GPIO,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_12MA,
		.oe = GPIO_OE_ENABLE
};

gpio_func_data_t pci_2_rst_gpio = {
		.gpio = PCIE_2_RST_GPIO,
		.func = 1,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_12MA,
		.oe = GPIO_OE_ENABLE
};

gpio_func_data_t wifi_pcie_1_power_gpio = {
		.gpio = WIFI_PCIE_1_POWER_GPIO,
		.func = 0,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_12MA,
		.oe = GPIO_OE_ENABLE
};

gpio_func_data_t wifi_pcie_2_power_gpio = {
		.gpio = WIFI_PCIE_2_POWER_GPIO,
		.func = 0,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_12MA,
		.oe = GPIO_OE_ENABLE
};

clk_offset_t pcie_0_clk	= {
	.aclk_ctl = PCIE_0_ACLK_CTL,
	.pclk_ctl = PCIE_0_PCLK_CTL,
	.hclk_ctl = PCIE_0_HCLK_CTL,
	.aux_clk_ctl = PCIE_0_AUX_CLK_CTL,
	.alt_ref_clk_ns = PCIE_0_ALT_REF_CLK_NS,
	.alt_ref_clk_acr = PCIE_0_ALT_REF_CLK_ACR,
	.aclk_fs = PCIE_0_ACLK_FS,
	.pclk_fs = PCIE_0_PCLK_FS,
	.parf_phy_refclk = PCIE20_0_PARF_PHY_REFCLK
};

clk_offset_t pcie_1_clk	= {
	.aclk_ctl = PCIE_1_ACLK_CTL,
	.pclk_ctl = PCIE_1_PCLK_CTL,
	.hclk_ctl = PCIE_1_HCLK_CTL,
	.aux_clk_ctl = PCIE_1_AUX_CLK_CTL,
	.alt_ref_clk_ns = PCIE_1_ALT_REF_CLK_NS,
	.alt_ref_clk_acr = PCIE_1_ALT_REF_CLK_ACR,
	.aclk_fs = PCIE_1_ACLK_FS,
	.pclk_fs = PCIE_1_PCLK_FS,
	.parf_phy_refclk = PCIE20_1_PARF_PHY_REFCLK
};

clk_offset_t pcie_2_clk	= {
	.aclk_ctl = PCIE_2_ACLK_CTL,
	.pclk_ctl = PCIE_2_PCLK_CTL,
	.hclk_ctl = PCIE_2_HCLK_CTL,
	.aux_clk_ctl = PCIE_2_AUX_CLK_CTL,
	.alt_ref_clk_ns = PCIE_2_ALT_REF_CLK_NS,
	.alt_ref_clk_acr = PCIE_2_ALT_REF_CLK_ACR,
	.aclk_fs = PCIE_2_ACLK_FS,
	.pclk_fs = PCIE_2_PCLK_FS,
	.parf_phy_refclk = PCIE20_2_PARF_PHY_REFCLK
};

#define pcie_board_cfg(_id)	\
{									\
	.pci_rst_gpio		= &pci_##_id##_rst_gpio,		\
	.parf			= PCIE20_##_id##_PARF_PHYS,		\
	.elbi			= PCIE20_##_id##_ELBI_PHYS,		\
	.pcie20			= PCIE20_##_id##_PHYS,			\
	.axi_bar_start		= PCIE20_##_id##_AXI_BAR_PHYS,		\
	.axi_bar_size		= PCIE20_##_id##_AXI_BAR_SIZE,		\
	.pcie_rst		= PCIE20_##_id##_RESET,			\
	.pci_clks		= &pcie_##_id##_clk			\
}

#define PCIE20_PARF_PHY_CTRL           0x40
#define __mask(a, b)    (((1 << ((a) + 1)) - 1) & ~((1 << (b)) - 1))
#define __set(v, a, b)  (((v) << (b)) & __mask(a, b))
#define PCIE20_PARF_PHY_CTRL_PHY_TX0_TERM_OFFST(x)      __set(x, 20, 16)
#define PCIE20_PARF_PCS_DEEMPH         0x34
#define PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN1(x)        __set(x, 21, 16)
#define PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN2_3_5DB(x)  __set(x, 13, 8)
#define PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN2_6DB(x)    __set(x, 5, 0)
#define PCIE20_PARF_PCS_SWING          0x38
#define PCIE20_PARF_PCS_SWING_TX_SWING_FULL(x)          __set(x, 14, 8)
#define PCIE20_PARF_PCS_SWING_TX_SWING_LOW(x)           __set(x, 6, 0)
#define PCIE20_PARF_PHY_REFCLK         0x4C
#define PCIE_SFAB_AXI_S5_FCLK_CTL       (MSM_CLK_CTL_BASE + 0x2154)
#define PCIE20_AXI_MSTR_RESP_COMP_CTRL0 0x818
#define PCIE20_AXI_MSTR_RESP_COMP_CTRL1 0x81c
#endif /* CONFIG_IPQ806X_PCI*/

#ifdef CONFIG_IPQ_MMC
gpio_func_data_t emmc1_gpio[] = {
	{
		.gpio = 38,
		.func = 2,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_10MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 39,
		.func = 2,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_10MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 40,
		.func = 2,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_10MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 41,
		.func = 2,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_10MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 42,
		.func = 2,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_16MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 43,
		.func = 2,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_10MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 44,
		.func = 2,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_10MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 45,
		.func = 2,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_10MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 46,
		.func = 2,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_10MA,
		.oe = GPIO_OE_DISABLE
	},
	{
		.gpio = 47,
		.func = 2,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_10MA,
		.oe = GPIO_OE_DISABLE
	},
};
#endif

gpio_func_data_t gsbi1_gpio[] = {
	{
		.gpio = 51,
		.func = 1,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_12MA,
		.oe = GPIO_OE_ENABLE
	},
	{
		.gpio = 52,
		.func = 1,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_12MA,
		.oe = GPIO_OE_ENABLE
	},
};

gpio_func_data_t gsbi2_gpio[] = {
	{
		.gpio = 22,
		.func = 1,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_12MA,
		.oe = GPIO_OE_ENABLE
	},
	{
		.gpio = 23,
		.func = 1,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_12MA,
		.oe = GPIO_OE_ENABLE
	},
};

gpio_func_data_t gsbi4_gpio[] = {
	{
		.gpio = 10,
		.func = 1,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_12MA,
		.oe = GPIO_OE_ENABLE
	},
	{
		.gpio = 11,
		.func = 1,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_12MA,
		.oe = GPIO_OE_ENABLE
	},
};

uart_cfg_t gsbi1_console_uart = {
	.base 	   	= GSBI_1,
	.gsbi_base	= UART_GSBI1_BASE,
	.uart_dm_base   = UART1_DM_BASE,
	.uart_mnd_value	= { .m_value = 48, .n_value = 125, .d_value = 63 },
	.dbg_uart_gpio  = gsbi1_gpio,
};

uart_cfg_t gsbi2_console_uart = {
	.base 	   	= GSBI_2,
	.gsbi_base	= UART_GSBI2_BASE,
	.uart_dm_base   = UART2_DM_BASE,
	.uart_mnd_value	= { .m_value = 12, .n_value = 625, .d_value = 313 },
	.dbg_uart_gpio  = gsbi2_gpio,
};

uart_cfg_t gsbi4_console_uart = {
	.base 	   	= GSBI_4,
	.gsbi_base	= UART_GSBI4_BASE,
	.uart_dm_base   = UART4_DM_BASE,
	.uart_mnd_value	= { .m_value = 12, .n_value = 625, .d_value = 313 },
	.dbg_uart_gpio  = gsbi4_gpio,
};

#define gmac_board_cfg(_b, _sec, _p, _p0, _p1, _mp, _pn, ...)		\
{									\
	.base			= NSS_GMAC##_b##_BASE,			\
	.unit			= _b,					\
	.is_macsec		= _sec,					\
	.phy			= PHY_INTERFACE_MODE_##_p,		\
	.phy_addr		= { .count = _pn, { __VA_ARGS__ } },	\
	.mac_pwr0		= _p0,					\
	.mac_pwr1		= _p1,					\
	.mac_conn_to_phy	= _mp,					\
	.phy_name		= "IPQ MDIO"#_b		 		\
}

#define uart_board_cfg(_b, _m, _n, _d)							\
{											\
	.base			= _b,							\
	.gsbi_base		= UART_GSBI##_b##_BASE,					\
	.uart_dm_base		= UART##_b##_DM_BASE,					\
	.uart_mnd_value		= { .m_value = _m, .n_value = _n, .d_value = _d },	\
	.dbg_uart_gpio		= gsbi##_b##_gpio,					\
}

#define gmac_board_cfg_invalid()	{ .unit = -1, }

/* Board specific parameter Array */
board_ipq806x_params_t board_params[] = {
	/*
	 * Replicate DB149 details for RUMI until, the board no.s are
	 * properly sorted out
	 */
	{
		.machid = MACH_TYPE_IPQ806X_RUMI3,
		.ddr_size = (256 << 20),
		.usb_core_mnd_value = { 5, 32, 1 },
		.usb_utmi_mnd_value = { 1, 40, 1 },
		.flashdesc = NAND_NOR,
		.flash_param = {
			.mode =	NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_SPANSION,
		},
		.console_uart_cfg = &gsbi1_console_uart,
#ifdef CONFIG_IPQ806X_I2C
		.i2c_gsbi = GSBI_4,
		.i2c_gsbi_base = I2C_GSBI4_BASE,
		.i2c_mnd_value = { 1, 4, 2 },
		.i2c_gpio = {
			{
				.gpio = 13,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
			{
				.gpio = 12,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
		},
#endif
		.clk_dummy = 1,
	},
	{
		.machid = MACH_TYPE_IPQ806X_DB149,
		.ddr_size = (256 << 20),
		.usb_core_mnd_value = { 1, 5, 32 },
		.usb_utmi_mnd_value = { 1, 40, 1 },
		.gmac_gpio_count = ARRAY_SIZE(gmac0_gpio),
		.gmac_gpio = gmac0_gpio,
		.gmac_cfg = {
			gmac_board_cfg(0, 0, RGMII, 0, 0, 0,
					1, 4),
			gmac_board_cfg(1, 1, SGMII, 0, 0, 0,
					4, 0, 1, 2, 3),
			gmac_board_cfg(2, 1, SGMII, 0, 0, 1,
					1, 6),
			gmac_board_cfg(3, 1, SGMII, 0, 0, 1,
					1, 7),
		},
		.console_uart_cfg = &gsbi2_console_uart,
		.flashdesc = NAND_NOR,
		.flash_param = {
			.mode =	NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_SPANSION,
		},

#ifdef CONFIG_IPQ806X_I2C
		.i2c_gsbi = GSBI_4,
		.i2c_gsbi_base = I2C_GSBI4_BASE,
		.i2c_mnd_value = { 1, 4, 2 },
		.i2c_gpio = {
			{
				.gpio = 13,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
			{
				.gpio = 12,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
		},
#endif
#ifdef CONFIG_IPQ806X_PCI
		.pcie_cfg = {
			pcie_board_cfg(0),
			pcie_board_cfg(1),
			pcie_board_cfg(2),
		},
#endif /* CONFIG_IPQ806X_PCI */
		.dtb_config_name ={ "db149", "v2.0-db149", "v3.0-db149"},
	},
	{
		.machid = MACH_TYPE_IPQ806X_DB149_1XX,
		.ddr_size = (256 << 20),
		.usb_core_mnd_value = { 1, 5, 32 },
		.usb_utmi_mnd_value = { 1, 40, 1 },
		.gmac_gpio_count = ARRAY_SIZE(gmac0_gpio),
		.gmac_gpio = gmac0_gpio,
		.gmac_cfg = {
			gmac_board_cfg(0, 0, RGMII, 0, 0, 0,
					1, 4),
			gmac_board_cfg(1, 1, SGMII, 0, 0, 0,
					4, 0, 1, 2, 3),
			gmac_board_cfg(2, 1, SGMII, 0, 0, 1,
					1, 6),
			gmac_board_cfg(3, 1, SGMII, 0, 0, 1,
					1, 7),
		},
		.flashdesc = NOR_MMC,
		.flash_param = {
			.mode =	NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_SPANSION,
		},
		.console_uart_cfg = &gsbi2_console_uart,
#ifdef CONFIG_IPQ806X_I2C
		.i2c_gsbi = GSBI_4,
		.i2c_gsbi_base = I2C_GSBI4_BASE,
		.i2c_mnd_value = { 1, 4, 2 },
		.i2c_gpio = {
			{
				.gpio = 13,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
			{
				.gpio = 12,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
		},
#endif
#ifdef CONFIG_IPQ806X_PCI
		.pcie_cfg = {
			pcie_board_cfg(0),
			pcie_board_cfg(1),
			pcie_board_cfg(2),
		},
#endif /* CONFIG_IPQ806X_PCI */
	},
	{
		.machid = MACH_TYPE_IPQ806X_DB149_2XX,
		.ddr_size = (256 << 20),
		.usb_core_mnd_value = { 1, 5, 32 },
		.usb_utmi_mnd_value = { 1, 40, 1 },
		.gmac_gpio_count = ARRAY_SIZE(gmac0_gpio),
		.gmac_gpio = gmac0_gpio,
		.gmac_cfg = {
			gmac_board_cfg(0, 0, RGMII, 0, 0, 0,
					1, 4),
			gmac_board_cfg(1, 1, SGMII, 0, 0, 0,
					4, 0, 1, 2, 3),
			gmac_board_cfg(2, 1, SGMII, 0, 0, 1,
					1, 6),
			gmac_board_cfg(3, 1, SGMII, 0, 0, 1,
					1, 7),
		},
		.flashdesc = NAND_NOR,
		.flash_param = {
			.mode =	NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_SPANSION,
		},
		.console_uart_cfg = &gsbi2_console_uart,
#ifdef CONFIG_IPQ806X_I2C
		.i2c_gsbi = GSBI_4,
		.i2c_gsbi_base = I2C_GSBI4_BASE,
		.i2c_mnd_value = { 1, 4, 2 },
		.i2c_gpio = {
			{
				.gpio = 13,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
			{
				.gpio = 12,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
		},
#endif
#ifdef CONFIG_IPQ806X_PCI
		.pcie_cfg = {
			pcie_board_cfg(0),
			pcie_board_cfg(1),
			pcie_board_cfg(2),
		},
#endif /* CONFIG_IPQ806X_PCI */
	},
	{
		.machid = MACH_TYPE_IPQ806X_TB726,
		.ddr_size = (256 << 20),
		.usb_core_mnd_value = { 1, 5, 32 },
		.usb_utmi_mnd_value = { 1, 40, 1 },
		.gmac_gpio_count = ARRAY_SIZE(gmac1_gpio),
		.gmac_gpio = gmac1_gpio,

		/* This GMAC config table is not valid as of now.
		 * To accomodate this config, TB726 board needs
		 * hardware rework.Moreover this setting is not
		 * been validated in TB726 board */
		.gmac_cfg = {
			gmac_board_cfg(0, 0, RGMII, 0, 0, 0,
					1, 4),
			gmac_board_cfg(1, 1, SGMII, 0, 0, 0,
					4, 0, 1, 2, 3),
			gmac_board_cfg(2, 1, SGMII, 0, 0, 1,
					1, 6),
			gmac_board_cfg(3, 1, SGMII, 0, 0, 1,
					1, 7),
		},
		.flashdesc = NAND_NOR,
		.flash_param = {
			.mode =	NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_SPANSION,
		},
		.console_uart_cfg = &gsbi2_console_uart,
#ifdef CONFIG_IPQ806X_I2C
		.i2c_gsbi = GSBI_4,
		.i2c_gsbi_base = I2C_GSBI4_BASE,
		.i2c_mnd_value = { 1, 4, 2 },
		.i2c_gpio = {
			{
				.gpio = 13,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
			{
				.gpio = 12,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
		},
#endif
#ifdef CONFIG_IPQ806X_PCI
		.pcie_cfg = {
			pcie_board_cfg(0),
			pcie_board_cfg(1),
			pcie_board_cfg(2),
		},
#endif /* CONFIG_IPQ806X_PCI */
	},
	{
		.machid = MACH_TYPE_IPQ806X_DB147,
		.ddr_size = (512 << 20),
		.usb_core_mnd_value = { 1, 5, 32 },
		.usb_utmi_mnd_value = { 1, 40, 1 },
		.gmac_gpio_count = ARRAY_SIZE(gmac1_gpio),
		.gmac_gpio = gmac1_gpio,
		.gmac_cfg = {
			gmac_board_cfg(1, 1, RGMII, S17_RGMII0_1_8V,
					S17_RGMII1_1_8V, 0, 1, 4),
			gmac_board_cfg(2, 1, SGMII, S17_RGMII0_1_8V,
					S17_RGMII1_1_8V, 0, 4, 0, 1, 2, 3),
			gmac_board_cfg_invalid(),
			gmac_board_cfg_invalid(),
		},
		.flashdesc = NAND_NOR,
		.flash_param = {
			.mode =	NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_SPANSION,
		},
		.console_uart_cfg = &gsbi2_console_uart,
#ifdef CONFIG_IPQ806X_I2C
		.i2c_gsbi = GSBI_4,
		.i2c_gsbi_base = I2C_GSBI4_BASE,
		.i2c_mnd_value = { 1, 4, 2 },
		.i2c_gpio = {
			{
				.gpio = 13,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
			{
				.gpio = 12,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
		},
#endif
#ifdef CONFIG_IPQ806X_PCI
		.pcie_cfg = {
			pcie_board_cfg(0),
			pcie_board_cfg(1),
			pcie_board_cfg(2),
		},
#endif /* CONFIG_IPQ806X_PCI */
	},
	{
		.machid = MACH_TYPE_IPQ806X_AP148,
		.ddr_size = (256 << 20),
		.usb_core_mnd_value = { 1, 5, 32 },
		.usb_utmi_mnd_value = { 1, 40, 1 },
		.gmac_gpio_count = ARRAY_SIZE(gmac1_gpio),
		.gmac_gpio = gmac1_gpio,
		.gmac_cfg = {
			gmac_board_cfg(1, 1, RGMII, S17_RGMII0_1_8V,
					S17_RGMII1_1_8V, 0, 1, 4),
			gmac_board_cfg(2, 1, SGMII, S17_RGMII0_1_8V,
					S17_RGMII1_1_8V, 0, 4, 0, 1, 2, 3),
			gmac_board_cfg_invalid(),
			gmac_board_cfg_invalid(),
		},
		.console_uart_cfg = &gsbi4_console_uart,
		.flashdesc = NAND_NOR,
		.reset_switch_gpio = &reset_s17_gpio,
		.flash_param = {
			.mode =	NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_SPANSION,
		},
#ifdef CONFIG_IPQ806X_I2C
		.i2c_gsbi = GSBI_4,
		.i2c_gsbi_base = I2C_GSBI4_BASE,
		.i2c_mnd_value = { 1, 4, 2 },
		.i2c_gpio = {
			{
				.gpio = 13,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
			{
				.gpio = 12,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
		},
#endif
#ifdef CONFIG_IPQ806X_PCI
		.pcie_cfg = {
			pcie_board_cfg(0),
			pcie_board_cfg(1),
			pcie_board_cfg(2),
		},
#endif /* CONFIG_IPQ806X_PCI */
		.dtb_config_name = { "ap148", "v2.0-ap148", "v3.0-ap148"},
	},
	{
		.machid = MACH_TYPE_IPQ806X_AP148_1XX,
		.ddr_size = (256 << 20),
		.usb_core_mnd_value = { 1, 5, 32 },
		.usb_utmi_mnd_value = { 1, 40, 1 },
		.gmac_gpio_count = ARRAY_SIZE(gmac1_gpio),
		.gmac_gpio = gmac1_gpio,
		.gmac_cfg = {
			gmac_board_cfg(1, 1, RGMII, S17_RGMII0_1_8V,
					S17_RGMII1_1_8V, 0, 1, 4),
			gmac_board_cfg(2, 1, SGMII, S17_RGMII0_1_8V,
					S17_RGMII1_1_8V, 0, 4, 0, 1, 2, 3),
			gmac_board_cfg_invalid(),
			gmac_board_cfg_invalid(),
		},
		.console_uart_cfg = &gsbi4_console_uart,
		.uart_cfg = {
			uart_board_cfg(2, 12, 625, 313),
		},
		.flashdesc = NAND_NOR,
		.reset_switch_gpio = &reset_s17_gpio,
		.flash_param = {
			.mode =	NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_SPANSION,
		},
#ifdef CONFIG_IPQ806X_I2C
		.i2c_gsbi = GSBI_4,
		.i2c_gsbi_base = I2C_GSBI4_BASE,
		.i2c_mnd_value = { 1, 4, 2 },
		.i2c_gpio = {
			{
				.gpio = 13,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
			{
				.gpio = 12,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
		},
#endif
#ifdef CONFIG_IPQ806X_PCI
		.pcie_cfg = {
			pcie_board_cfg(0),
			pcie_board_cfg(1),
			pcie_board_cfg(2),
		},
#endif /* CONFIG_IPQ806X_PCI */
		.dtb_config_name = { "ap148_1xx", "v2.0-ap148_1xx", "v3.0-ap148_1xx"},
	},
	{
		.machid = MACH_TYPE_IPQ806X_AP145,
		.ddr_size = (256 << 20),
		.usb_core_mnd_value = { 1, 5, 32 },
		.usb_utmi_mnd_value = { 1, 40, 1 },
		.gmac_gpio_count = ARRAY_SIZE(gmac1_gpio),
		.gmac_gpio = gmac1_gpio,
		.gmac_cfg = {
			gmac_board_cfg(1, 1, RGMII, S17_RGMII0_1_8V,
					S17_RGMII1_1_8V, 0, 1, 4),
			gmac_board_cfg(2, 1, SGMII, S17_RGMII0_1_8V,
					S17_RGMII1_1_8V, 0, 4, 0, 1, 2, 3),
			gmac_board_cfg_invalid(),
			gmac_board_cfg_invalid(),
		},
		.flashdesc = NAND_NOR,
		.flash_param = {
			.mode =	NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_SPANSION,
		},
		.console_uart_cfg = &gsbi4_console_uart,
#ifdef CONFIG_IPQ806X_I2C
		.i2c_gsbi = GSBI_4,
		.i2c_gsbi_base = I2C_GSBI4_BASE,
		.i2c_mnd_value = { 1, 4, 2 },
		.i2c_gpio = {
			{
				.gpio = 13,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
			{
				.gpio = 12,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
		},
#endif
#ifdef CONFIG_IPQ806X_PCI
		.pcie_cfg = {
			pcie_board_cfg(0),
			pcie_board_cfg(1),
			pcie_board_cfg(2),
		},
#endif /* CONFIG_IPQ806X_PCI */
		.dtb_config_name = { "ap145", "v2.0-ap145", "v3.0-ap145"},
	},
	{
		.machid = MACH_TYPE_IPQ806X_AP145_1XX,
		.ddr_size = (256 << 20),
		.usb_core_mnd_value = { 1, 5, 32 },
		.usb_utmi_mnd_value = { 1, 40, 1 },
		.gmac_gpio_count = ARRAY_SIZE(gmac1_gpio),
		.gmac_gpio = gmac1_gpio,
		.gmac_cfg = {
			gmac_board_cfg(1, 1, RGMII, S17_RGMII0_1_8V,
					S17_RGMII1_1_8V, 0, 1, 4),
			gmac_board_cfg(2, 1, SGMII, S17_RGMII0_1_8V,
					S17_RGMII1_1_8V, 0, 4, 0, 1, 2, 3),
			gmac_board_cfg_invalid(),
			gmac_board_cfg_invalid(),
		},
		.flashdesc = NOR_MMC,
		.flash_param = {
			.mode =	NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_SPANSION,
		},
		.console_uart_cfg = &gsbi4_console_uart,
#ifdef CONFIG_IPQ806X_I2C
		.i2c_gsbi = GSBI_4,
		.i2c_gsbi_base = I2C_GSBI4_BASE,
		.i2c_mnd_value = { 1, 4, 2 },
		.i2c_gpio = {
			{
				.gpio = 13,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
			{
				.gpio = 12,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
		},
#endif
#ifdef CONFIG_IPQ806X_PCI
		.pcie_cfg = {
			pcie_board_cfg(0),
			pcie_board_cfg(1),
			pcie_board_cfg(2),
		},
#endif /* CONFIG_IPQ806X_PCI */
		.dtb_config_name = { "ap145_1xx", "v2.0-ap145_1xx", "v3.0-ap145_1xx"},
	},
	{
		.machid = MACH_TYPE_IPQ806X_AP160,
		.ddr_size = (256 << 20),
		.usb_core_mnd_value = { 1, 5, 32 },
		.usb_utmi_mnd_value = { 1, 40, 1 },
		.gmac_gpio_count = ARRAY_SIZE(gmac1_gpio),
		.gmac_gpio = gmac1_gpio,
		.gmac_cfg = {
			gmac_board_cfg(1, 1, RGMII, S17_RGMII0_1_8V,
			S17_RGMII1_1_8V, 0, 1, 4),
			gmac_board_cfg(2, 1, SGMII, S17_RGMII0_1_8V,
			S17_RGMII1_1_8V, 0, 4, 0, 1, 2, 3),
			gmac_board_cfg_invalid(),
			gmac_board_cfg_invalid(),
		},
		.flashdesc = NAND_NOR,
		.flash_param = {
			.mode = NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_SPANSION,
		},
		.console_uart_cfg = &gsbi4_console_uart,
#ifdef CONFIG_IPQ806X_I2C
		.i2c_gsbi = GSBI_4,
		.i2c_gsbi_base = I2C_GSBI4_BASE,
		.i2c_mnd_value = { 1, 4, 2 },
		.i2c_gpio = {
			{
				.gpio = 13,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
			{
				.gpio = 12,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
		},
#endif
#ifdef CONFIG_IPQ806X_PCI
		.pcie_cfg = {
			pcie_board_cfg(0),
			pcie_board_cfg(1),
			pcie_board_cfg(2),
		},
		.wifi_pcie_power_gpio_cnt = 2,
		.wifi_pcie_power_gpio = {
			&wifi_pcie_1_power_gpio,
			&wifi_pcie_2_power_gpio,
		},
#endif /* CONFIG_IPQ806X_PCI */
		.dtb_config_name = { "ap160", "v2.0-ap160", "v3.0-ap160"},
	},
	{
		.machid = MACH_TYPE_IPQ806X_AP160_2XX,
		.ddr_size = (256 << 20),
		.usb_core_mnd_value = { 1, 5, 32 },
		.usb_utmi_mnd_value = { 1, 40, 1 },
		.gmac_gpio_count = ARRAY_SIZE(gmac1_gpio),
		.gmac_gpio = gmac1_gpio,
		.ar8033_gpio = ar8033_gpio,
		.gmac_cfg = {
			gmac_board_cfg(0, 0, QSGMII, 0,
			0, 0, 1, 4),
			gmac_board_cfg_invalid(),
			gmac_board_cfg_invalid(),
			gmac_board_cfg_invalid(),
		},
		.flashdesc = NAND_NOR,
		.flash_param = {
			.mode = NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_SPANSION,
		},
		.console_uart_cfg = &gsbi4_console_uart,
#ifdef CONFIG_IPQ806X_I2C
		.i2c_gsbi = GSBI_4,
		.i2c_gsbi_base = I2C_GSBI4_BASE,
		.i2c_mnd_value = { 1, 4, 2 },
		.i2c_gpio = {
			{
				.gpio = 13,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
			{
				.gpio = 12,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
		},
#endif
#ifdef CONFIG_IPQ806X_PCI
		.pcie_cfg = {
			pcie_board_cfg(0),
			pcie_board_cfg(1),
			pcie_board_cfg(2),
		},
		.wifi_pcie_power_gpio_cnt = 2,
		.wifi_pcie_power_gpio = {
			&wifi_pcie_1_power_gpio,
			&wifi_pcie_2_power_gpio,
		},
#endif /* CONFIG_IPQ806X_PCI */
		.dtb_config_name = { "ap160_2xx", "v2.0-ap160_2xx", "v3.0-ap160_2xx"},
	},
	{
		.machid = MACH_TYPE_IPQ806X_AK01_1XX,
		.ddr_size = (256 << 20),
		.usb_core_mnd_value = { 1, 5, 32 },
		.usb_utmi_mnd_value = { 1, 40, 1 },
		.gmac_gpio_count = ARRAY_SIZE(gmac3_gpio),
		.gmac_gpio = gmac3_gpio,
		.gmac_cfg = {
			gmac_board_cfg(3, 1, SGMII, 0,
			0, 1, 1, 5),
			gmac_board_cfg_invalid(),
			gmac_board_cfg_invalid(),
			gmac_board_cfg_invalid(),
		},
		.flashdesc = NAND_NOR,
		.reset_ak01_gmac_gpio = &reset_ak01_8033_gpio,
		.flash_param = {
			.mode = NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_SPANSION,
		},
		.console_uart_cfg = &gsbi4_console_uart,
#ifdef CONFIG_IPQ806X_I2C
		.i2c_gsbi = GSBI_4,
		.i2c_gsbi_base = I2C_GSBI4_BASE,
		.i2c_mnd_value = { 1, 4, 2 },
		.i2c_gpio = {
			{
				.gpio = 13,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
			{
				.gpio = 12,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
		},
#endif
#ifdef CONFIG_IPQ806X_PCI
		.pcie_cfg = {
			pcie_board_cfg(0),
			pcie_board_cfg(1),
			pcie_board_cfg(2),
		},
#endif /* CONFIG_IPQ806X_PCI */
		.dtb_config_name = { "ak01", "v2.0-ak01", "v3.0-ak01"},
	},
	{
		.machid = MACH_TYPE_IPQ806X_AP161,
		.ddr_size = (256 << 20),
		.usb_core_mnd_value = { 1, 5, 32 },
		.usb_utmi_mnd_value = { 1, 40, 1 },
		.gmac_gpio_count = ARRAY_SIZE(gmac1_gpio),
		.gmac_gpio = gmac1_gpio,
		.gmac_cfg = {
			gmac_board_cfg(1, 1, RGMII, S17_RGMII0_1_8V,
					S17_RGMII1_1_8V, 0, 1, 4),
			gmac_board_cfg(2, 1, SGMII, S17_RGMII0_1_8V,
					S17_RGMII1_1_8V, 0, 4, 0, 1, 2, 3),
			gmac_board_cfg_invalid(),
			gmac_board_cfg_invalid(),
		},
		.flashdesc = NAND_NOR,
		.flash_param = {
			.mode =	NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_SPANSION,
		},
		.console_uart_cfg = &gsbi4_console_uart,
#ifdef CONFIG_IPQ806X_I2C
		.i2c_gsbi = GSBI_4,
		.i2c_gsbi_base = I2C_GSBI4_BASE,
		.i2c_mnd_value = { 1, 4, 2 },
		.i2c_gpio = {
			{
				.gpio = 13,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
			{
				.gpio = 12,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.oe = GPIO_OE_ENABLE
			},
		},
#endif
#ifdef CONFIG_IPQ806X_PCI
		.pcie_cfg = {
			pcie_board_cfg(0),
			pcie_board_cfg(1),
			pcie_board_cfg(2),
		},
#endif /* CONFIG_IPQ806X_PCI */
		.dtb_config_name = { "ap161", "v2.0-ap161", "v3.0-ap161"},
	},
	{
		.machid = MACH_TYPE_IPQ806X_STORM,
		.ddr_size = (1024 << 20),
		.usb_core_mnd_value = { 1, 5, 32 },
		.usb_utmi_mnd_value = { 1, 40, 1 },
		.gmac_gpio_count = ARRAY_SIZE(gmac0_gpio),
		.gmac_gpio = gmac0_gpio,
		.gmac_cfg = {
			gmac_board_cfg(0, 1, RGMII, S17_RGMII0_1_8V,
					S17_RGMII1_1_8V, 0, 1, 4),
			gmac_board_cfg(2, 1, SGMII, S17_RGMII0_1_8V,
					S17_RGMII1_1_8V, 0, 3, 0, 1, 2),
			gmac_board_cfg_invalid(),
			gmac_board_cfg_invalid(),
		},
		.switch_gpio = &storm_switch_gpio,
		.flashdesc = ONLY_NOR,
		.flash_param = {
			.mode =	NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_SPANSION,
		},
		.console_uart_cfg = &gsbi4_console_uart,
	},

};

#define NUM_IPQ806X_BOARDS	ARRAY_SIZE(board_params)
#endif
