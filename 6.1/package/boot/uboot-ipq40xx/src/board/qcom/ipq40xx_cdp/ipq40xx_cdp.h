/*
 *
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
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

#ifndef _IPQ40XX_CDP_H_
#define _IPQ40XX_CDP_H_

#include <configs/ipq40xx_cdp.h>
#include <asm/u-boot.h>
#include <phy.h>
#include "qca_common.h"

#define NO_OF_DBG_UART_GPIOS	2
#ifdef CONFIG_IPQ40XX_I2C
#define NO_OF_I2C_GPIOS		2
#endif
#define MAX_CONF_NAME		5

unsigned int smem_get_board_machtype(void);

#define IPQ40XX_EDMA_DEV	1
typedef struct {
	uint count;
	u8 addr[7];
} ipq40xx_edma_phy_addr_t;

/* ipq40xx edma Paramaters */
typedef struct {
	uint base;
	int unit;
	uint mac_conn_to_phy;
	phy_interface_t phy;
	ipq40xx_edma_phy_addr_t phy_addr;
	const char phy_name[MDIO_NAME_LEN];
} ipq40xx_edma_board_cfg_t;

typedef struct {
	int gpio;
	unsigned int func;
	unsigned int out;
	unsigned int pull;
	unsigned int drvstr;
	unsigned int oe;
	unsigned int gpio_vm;
	unsigned int gpio_od_en;
	unsigned int gpio_pu_res;
} gpio_func_data_t;

typedef struct {
	unsigned int uart_dm_base;
	gpio_func_data_t *dbg_uart_gpio;
} uart_cfg_t;

#ifdef CONFIG_IPQ40XX_I2C
typedef struct {
	unsigned int i2c_base;
	gpio_func_data_t *i2c_gpio;
} i2c_cfg_t;
#endif

#ifdef CONFIG_IPQ40XX_PCI
#define PCI_MAX_DEVICES	1

typedef struct {
	gpio_func_data_t	*pci_gpio;
	uint32_t		pci_gpio_count;
	uint32_t		parf;
	uint32_t		elbi;
	uint32_t		pcie20;
	uint32_t		axi_bar_start;
	uint32_t		axi_bar_size;
	uint32_t		pcie_rst;
	uint32_t		axi_conf;
	int			linkup;
} pcie_params_t;

void board_pci_init(void);
#endif /* CONFIG_IPQ40XX_PCI */

/* Board specific parameters */
typedef struct {
	unsigned int machid;
	unsigned int ddr_size;
	const char *mtdids;
	gpio_func_data_t *spi_nor_gpio;
	unsigned int spi_nor_gpio_count;
	gpio_func_data_t *nand_gpio;
	unsigned int nand_gpio_count;
	gpio_func_data_t *sw_gpio;
	unsigned int sw_gpio_count;
	gpio_func_data_t *rgmii_gpio;
	unsigned int rgmii_gpio_count;
	ipq40xx_edma_board_cfg_t edma_cfg[IPQ40XX_EDMA_DEV];
	uart_cfg_t *uart_cfg;
	uart_cfg_t *console_uart_cfg;
#ifdef CONFIG_IPQ40XX_I2C
	i2c_cfg_t *i2c_cfg;
#endif
	gpio_func_data_t *mmc_gpio;
	unsigned int mmc_gpio_count;
	unsigned int spi_nand_available;
	unsigned int nor_nand_available;
	unsigned int nor_emmc_available;
#ifdef CONFIG_IPQ40XX_PCI
	pcie_params_t pcie_cfg[PCI_MAX_DEVICES];
#endif
	const char *dtb_config_name[MAX_CONF_NAME];
} __attribute__ ((__packed__)) board_ipq40xx_params_t;

/* 74HC595x shift register contol */
#ifdef CONFIG_SHIFT_REG
#define SHIFTREG_ANIMATION_TIME_STEP	500
#define SHIFTREG_TIME_UNIT				1 /* 1 usec */
#define SHIFTREG_CLK					1
#define SHIFTREG_LATCH					2
#define SHIFTREG_DATA					3

/* LED Shift Register Configuration */

/* RUTX 09 */
#define RUTX09_SIM1_LED		0x00000400
#define RUTX09_SIM2_LED		0x00000200

#define RUTX09_WAN_ETH_LED	0x00000800
#define RUTX09_3G_LED		0x00800000
#define RUTX09_4G_LED		0x00400000

#define RUTX09_SSID_1_LED	0x00200000
#define RUTX09_SSID_2_LED	0x00100000
#define RUTX09_SSID_3_LED	0x00080000
#define RUTX09_SSID_4_LED	0x00020000
#define RUTX09_SSID_5_LED	0x00010000

#define RUTX09_ALL_LED_ON	0x11FB0E00
#define RUTX09_ALL_LED_OFF	0x10000000

/* RUTX 10 */
#define RUTX10_WIFI_24_LED	0x04000000
#define RUTX10_WIFI_5_LED	0x02000000

#define RUTX10_ALL_LED_ON	0x06000000
#define RUTX10_ALL_LED_OFF	0x00000001

/* RUTX 11 */
#define RUTX11_SIM1_LED				0x00000400
#define RUTX11_SIM2_LED				0x00000200
#define RUTX11_WAN_WIFI_WIFI_24_LED	0x04000800
#define RUTX11_WAN_ETH_WIFI_5_LED	0x02040000

#define RUTX11_3G_LED		0x00800000
#define RUTX11_4G_LED		0x00400000

#define RUTX11_SSID_1_LED	0x00200000
#define RUTX11_SSID_2_LED	0x00100000
#define RUTX11_SSID_3_LED	0x00080000
#define RUTX11_SSID_4_LED	0x00020000
#define RUTX11_SSID_5_LED	0x00010000

#define RUTX11_ALL_LED_ON	0x17FF0E00
#define RUTX11_ALL_LED_OFF	0x10000000

/* RUTX12 */
#define RUTX12_SIM1_LED				0x00020000
#define RUTX12_SIM2_LED				0x00010000
#define RUTX12_WAN_WIFI_WIFI_24_LED	0x01000200
#define RUTX12_WAN_ETH_WIFI_5_LED	0x02000100

#define RUTX12_3G_1_LED		0x08000000
#define RUTX12_4G_1_LED		0x10000000

#define RUTX12_SSID_1_1_LED 0x80000000
#define RUTX12_SSID_2_1_LED 0x40000000
#define RUTX12_SSID_3_1_LED	0x20000000

#define RUTX12_3G_2_LED		0x00080000
#define RUTX12_4G_2_LED		0x00040000

#define RUTX12_SSID_1_2_LED 0x00800000
#define RUTX12_SSID_2_2_LED	0x00400000
#define RUTX12_SSID_3_2_LED	0x00200000

#define RUTX12_ALL_LED_ON	0xFFFF0300
#define RUTX12_ALL_LED_OFF	0x00000080

/* RUTX14 */
#define RUTX14_SIM1_LED				0x00020000
#define RUTX14_SIM2_LED				0x00010000
#define RUTX14_WAN_WIFI_WIFI_24_LED	0x02080000
#define RUTX14_WAN_ETH_WIFI_5_LED	0x01040000

#define RUTX14_3G_LED	0x04000000
#define RUTX14_4G_LED	0x08000000

#define RUTX14_SSID_1_LED	0x80000000
#define RUTX14_SSID_2_LED	0x40000000
#define RUTX14_SSID_3_LED	0x20000000

#define RUTX14_ALL_LED_ON	0xEF0F0000
#define RUTX14_ALL_LED_OFF	0x00000001

/* RUTXR1 */

#define RUTXR1_SIM1_LED 0x08000000
#define RUTXR1_SIM2_LED 0x04000000

#define RUTXR1_SSID_1_LED 0x00800000
#define RUTXR1_SSID_2_LED 0x00400000
#define RUTXR1_SSID_3_LED 0x00200000

#define RUTXR1_3G_4G_LED	0x03000000
#define RUTXR1_ETH_MOB_LED	0x80100000

#define RUTXR1_ALL_LED_ON	0x1FF00000
#define RUTXR1_ALL_LED_OFF	0x10000000

/* RUTX50 */

#define RUTX50_SIM1_LED				0x00C20000
#define RUTX50_SIM2_LED				0x00C10000
#define RUTX50_WAN_WIFI_WIFI_24_LED	0x01C80000
#define RUTX50_WAN_ETH_WIFI_5_LED	0x02C40000

#define RUTX50_3G_LED	0x04C00000
#define RUTX50_4G_LED	0x08C00000
#define RUTX50_5G_LED	0x10C00000

#define RUTX50_SSID_1_LED	0x80C00000
#define RUTX50_SSID_2_LED	0x40C00000
#define RUTX50_SSID_3_LED	0x20C00000

#define RUTX50_ALL_LED_ON	0xFF0F0000
#define RUTX50_ALL_LED_OFF	0x00008000

/* Helper defines */

#define SHIFTREG_ARRAY_MAX_LED_COUNT 16
#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

#endif

extern board_ipq40xx_params_t *gboard_param;
unsigned int get_board_index(unsigned int machid);
void qca_configure_gpio(gpio_func_data_t *gpio, uint count);
void sr_led_on(void);
void sr_led_off(void);
void check_timer_led(void);
void set_led_configuration(void);

#define MSM_SDC1_BASE      0x7824000
extern qca_mmc mmc_host;
#endif
