/*
 * Copyright (C) 2010 Marek Vasut <marek.vasut@gmail.com>
 *
 * (C) Copyright 2009 Freescale Semiconductor, Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/mx5x_pins.h>
#include <asm/arch/iomux.h>
#include <asm/gpio.h>
#include <asm/errno.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/crm_regs.h>
#include <i2c.h>
#include <mmc.h>
#include <fsl_esdhc.h>
#include <pmic.h>
#include <fsl_pmic.h>
#include <mc13892.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * Compile-time error checking
 */
#ifndef	CONFIG_MXC_SPI
#error "CONFIG_MXC_SPI not set, this is essential for board's operation!"
#endif

/*
 * Shared variables / local defines
 */
/* LED */
#define	EFIKAMX_LED_BLUE	0x1
#define	EFIKAMX_LED_GREEN	0x2
#define	EFIKAMX_LED_RED		0x4

void efikamx_toggle_led(uint32_t mask);

/* Board revisions */
#define	EFIKAMX_BOARD_REV_11	0x1
#define	EFIKAMX_BOARD_REV_12	0x2
#define	EFIKAMX_BOARD_REV_13	0x3
#define	EFIKAMX_BOARD_REV_14	0x4

#define	EFIKASB_BOARD_REV_13	0x1
#define	EFIKASB_BOARD_REV_20	0x2

/*
 * Board identification
 */
u32 get_efikamx_rev(void)
{
	u32 rev = 0;
	/*
	 * Retrieve board ID:
	 *      rev1.1: 1,1,1
	 *      rev1.2: 1,1,0
	 *      rev1.3: 1,0,1
	 *      rev1.4: 1,0,0
	 */
	mxc_request_iomux(MX51_PIN_NANDF_CS0, IOMUX_CONFIG_GPIO);
	/* set to 1 in order to get correct value on board rev1.1 */
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_NANDF_CS0), 1);

	mxc_request_iomux(MX51_PIN_NANDF_CS0, IOMUX_CONFIG_GPIO);
	mxc_iomux_set_pad(MX51_PIN_NANDF_CS0, PAD_CTL_100K_PU);
	gpio_direction_input(IOMUX_TO_GPIO(MX51_PIN_NANDF_CS0));
	rev |= (!!gpio_get_value(IOMUX_TO_GPIO(MX51_PIN_NANDF_CS0))) << 0;

	mxc_request_iomux(MX51_PIN_NANDF_CS1, IOMUX_CONFIG_GPIO);
	mxc_iomux_set_pad(MX51_PIN_NANDF_CS1, PAD_CTL_100K_PU);
	gpio_direction_input(IOMUX_TO_GPIO(MX51_PIN_NANDF_CS1));
	rev |= (!!gpio_get_value(IOMUX_TO_GPIO(MX51_PIN_NANDF_CS1))) << 1;

	mxc_request_iomux(MX51_PIN_NANDF_RB3, IOMUX_CONFIG_GPIO);
	mxc_iomux_set_pad(MX51_PIN_NANDF_RB3, PAD_CTL_100K_PU);
	gpio_direction_input(IOMUX_TO_GPIO(MX51_PIN_NANDF_RB3));
	rev |= (!!gpio_get_value(IOMUX_TO_GPIO(MX51_PIN_NANDF_RB3))) << 2;

	return (~rev & 0x7) + 1;
}

inline u32 get_efikasb_rev(void)
{
	u32 rev = 0;

	mxc_request_iomux(MX51_PIN_EIM_CS3, IOMUX_CONFIG_GPIO);
	mxc_iomux_set_pad(MX51_PIN_EIM_CS3, PAD_CTL_100K_PU);
	gpio_direction_input(IOMUX_TO_GPIO(MX51_PIN_EIM_CS3));
	rev |= (!!gpio_get_value(IOMUX_TO_GPIO(MX51_PIN_EIM_CS3))) << 0;

	mxc_request_iomux(MX51_PIN_EIM_CS4, IOMUX_CONFIG_GPIO);
	mxc_iomux_set_pad(MX51_PIN_EIM_CS4, PAD_CTL_100K_PU);
	gpio_direction_input(IOMUX_TO_GPIO(MX51_PIN_EIM_CS4));
	rev |= (!!gpio_get_value(IOMUX_TO_GPIO(MX51_PIN_EIM_CS4))) << 1;

	return rev;
}

inline uint32_t get_efika_rev(void)
{
	if (machine_is_efikamx())
		return get_efikamx_rev();
	else
		return get_efikasb_rev();
}

u32 get_board_rev(void)
{
	return get_cpu_rev() | (get_efika_rev() << 8);
}

/*
 * DRAM initialization
 */
int dram_init(void)
{
	/* dram_init must store complete ramsize in gd->ram_size */
	gd->ram_size = get_ram_size((void *)CONFIG_SYS_SDRAM_BASE,
				PHYS_SDRAM_1_SIZE);
	return 0;
}

/*
 * UART configuration
 */
static void setup_iomux_uart(void)
{
	unsigned int pad = PAD_CTL_HYS_ENABLE | PAD_CTL_PKE_ENABLE |
			PAD_CTL_PUE_PULL | PAD_CTL_DRV_HIGH;

	mxc_request_iomux(MX51_PIN_UART1_RXD, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_UART1_RXD, pad | PAD_CTL_SRE_FAST);
	mxc_request_iomux(MX51_PIN_UART1_TXD, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_UART1_TXD, pad | PAD_CTL_SRE_FAST);
	mxc_request_iomux(MX51_PIN_UART1_RTS, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_UART1_RTS, pad);
	mxc_request_iomux(MX51_PIN_UART1_CTS, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_UART1_CTS, pad);
}

/*
 * SPI configuration
 */
#ifdef CONFIG_MXC_SPI
static void setup_iomux_spi(void)
{
	/* 000: Select mux mode: ALT0 mux port: MOSI of instance: ecspi1 */
	mxc_request_iomux(MX51_PIN_CSPI1_MOSI, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_CSPI1_MOSI,
		PAD_CTL_HYS_ENABLE | PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);

	/* 000: Select mux mode: ALT0 mux port: MISO of instance: ecspi1. */
	mxc_request_iomux(MX51_PIN_CSPI1_MISO, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_CSPI1_MISO,
		PAD_CTL_HYS_ENABLE | PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);

	/* Configure SS0 as a GPIO */
	mxc_request_iomux(MX51_PIN_CSPI1_SS0, IOMUX_CONFIG_GPIO);
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_CSPI1_SS0), 0);

	/* Configure SS1 as a GPIO */
	mxc_request_iomux(MX51_PIN_CSPI1_SS1, IOMUX_CONFIG_GPIO);
	gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_CSPI1_SS1), 1);

	/* 000: Select mux mode: ALT0 mux port: SS2 of instance: ecspi1. */
	mxc_request_iomux(MX51_PIN_CSPI1_RDY, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_CSPI1_RDY,
		PAD_CTL_HYS_ENABLE | PAD_CTL_PKE_ENABLE);

	/* 000: Select mux mode: ALT0 mux port: SCLK of instance: ecspi1. */
	mxc_request_iomux(MX51_PIN_CSPI1_SCLK, IOMUX_CONFIG_ALT0);
	mxc_iomux_set_pad(MX51_PIN_CSPI1_SCLK,
		PAD_CTL_HYS_ENABLE | PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
}
#else
static inline void setup_iomux_spi(void) { }
#endif

/*
 * PMIC configuration
 */
#ifdef CONFIG_MXC_SPI
static void power_init(void)
{
	unsigned int val;
	struct mxc_ccm_reg *mxc_ccm = (struct mxc_ccm_reg *)MXC_CCM_BASE;
	struct pmic *p;

	pmic_init();
	p = get_pmic();

	/* Write needed to Power Gate 2 register */
	pmic_reg_read(p, REG_POWER_MISC, &val);
	val &= ~PWGT2SPIEN;
	pmic_reg_write(p, REG_POWER_MISC, val);

	/* Externally powered */
	pmic_reg_read(p, REG_CHARGE, &val);
	val |= ICHRG0 | ICHRG1 | ICHRG2 | ICHRG3 | CHGAUTOB;
	pmic_reg_write(p, REG_CHARGE, val);

	/* power up the system first */
	pmic_reg_write(p, REG_POWER_MISC, PWUP);

	/* Set core voltage to 1.1V */
	pmic_reg_read(p, REG_SW_0, &val);
	val = (val & ~SWx_VOLT_MASK) | SWx_1_200V;
	pmic_reg_write(p, REG_SW_0, val);

	/* Setup VCC (SW2) to 1.25 */
	pmic_reg_read(p, REG_SW_1, &val);
	val = (val & ~SWx_VOLT_MASK) | SWx_1_250V;
	pmic_reg_write(p, REG_SW_1, val);

	/* Setup 1V2_DIG1 (SW3) to 1.25 */
	pmic_reg_read(p, REG_SW_2, &val);
	val = (val & ~SWx_VOLT_MASK) | SWx_1_250V;
	pmic_reg_write(p, REG_SW_2, val);
	udelay(50);

	/* Raise the core frequency to 800MHz */
	writel(0x0, &mxc_ccm->cacrr);

	/* Set switchers in Auto in NORMAL mode & STANDBY mode */
	/* Setup the switcher mode for SW1 & SW2*/
	pmic_reg_read(p, REG_SW_4, &val);
	val = (val & ~((SWMODE_MASK << SWMODE1_SHIFT) |
		(SWMODE_MASK << SWMODE2_SHIFT)));
	val |= (SWMODE_AUTO_AUTO << SWMODE1_SHIFT) |
		(SWMODE_AUTO_AUTO << SWMODE2_SHIFT);
	pmic_reg_write(p, REG_SW_4, val);

	/* Setup the switcher mode for SW3 & SW4 */
	pmic_reg_read(p, REG_SW_5, &val);
	val = (val & ~((SWMODE_MASK << SWMODE3_SHIFT) |
		(SWMODE_MASK << SWMODE4_SHIFT)));
	val |= (SWMODE_AUTO_AUTO << SWMODE3_SHIFT) |
		(SWMODE_AUTO_AUTO << SWMODE4_SHIFT);
	pmic_reg_write(p, REG_SW_5, val);

	/* Set VDIG to 1.8V, VGEN3 to 1.8V, VCAM to 2.6V */
	pmic_reg_read(p, REG_SETTING_0, &val);
	val &= ~(VCAM_MASK | VGEN3_MASK | VDIG_MASK);
	val |= VDIG_1_8 | VGEN3_1_8 | VCAM_2_6;
	pmic_reg_write(p, REG_SETTING_0, val);

	/* Set VVIDEO to 2.775V, VAUDIO to 3V, VSD to 3.15V */
	pmic_reg_read(p, REG_SETTING_1, &val);
	val &= ~(VVIDEO_MASK | VSD_MASK | VAUDIO_MASK);
	val |= VSD_3_15 | VAUDIO_3_0 | VVIDEO_2_775 | VGEN1_1_2 | VGEN2_3_15;
	pmic_reg_write(p, REG_SETTING_1, val);

	/* Enable VGEN1, VGEN2, VDIG, VPLL */
	pmic_reg_read(p, REG_MODE_0, &val);
	val |= VGEN1EN | VDIGEN | VGEN2EN | VPLLEN;
	pmic_reg_write(p, REG_MODE_0, val);

	/* Configure VGEN3 and VCAM regulators to use external PNP */
	val = VGEN3CONFIG | VCAMCONFIG;
	pmic_reg_write(p, REG_MODE_1, val);
	udelay(200);

	/* Enable VGEN3, VCAM, VAUDIO, VVIDEO, VSD regulators */
	val = VGEN3EN | VGEN3CONFIG | VCAMEN | VCAMCONFIG |
		VVIDEOEN | VAUDIOEN | VSDEN;
	pmic_reg_write(p, REG_MODE_1, val);

	pmic_reg_read(p, REG_POWER_CTL2, &val);
	val |= WDIRESET;
	pmic_reg_write(p, REG_POWER_CTL2, val);

	udelay(2500);
}
#else
static inline void power_init(void) { }
#endif

/*
 * MMC configuration
 */
#ifdef CONFIG_FSL_ESDHC
struct fsl_esdhc_cfg esdhc_cfg[2] = {
	{MMC_SDHC1_BASE_ADDR, 1},
	{MMC_SDHC2_BASE_ADDR, 1},
};

static inline uint32_t efika_mmc_cd(void)
{
	if (machine_is_efikamx())
		return MX51_PIN_GPIO1_0;
	else
		return MX51_PIN_EIM_CS2;
}

int board_mmc_getcd(struct mmc *mmc)
{
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;
	uint32_t cd = efika_mmc_cd();
	int ret;

	if (cfg->esdhc_base == MMC_SDHC1_BASE_ADDR)
		ret = !gpio_get_value(IOMUX_TO_GPIO(cd));
	else
		ret = !gpio_get_value(IOMUX_TO_GPIO(MX51_PIN_GPIO1_8));

	return ret;
}

int board_mmc_init(bd_t *bis)
{
	int ret;
	uint32_t cd = efika_mmc_cd();

	/* SDHC1 is used on all revisions, setup control pins first */
	mxc_request_iomux(cd,
		IOMUX_CONFIG_ALT0 | IOMUX_CONFIG_SION);
	mxc_iomux_set_pad(cd,
		PAD_CTL_DRV_HIGH | PAD_CTL_HYS_ENABLE |
		PAD_CTL_PUE_KEEPER | PAD_CTL_100K_PU |
		PAD_CTL_ODE_OPENDRAIN_NONE |
		PAD_CTL_PKE_ENABLE | PAD_CTL_SRE_FAST);
	mxc_request_iomux(MX51_PIN_GPIO1_1,
		IOMUX_CONFIG_ALT0 | IOMUX_CONFIG_SION);
	mxc_iomux_set_pad(MX51_PIN_GPIO1_1,
		PAD_CTL_DRV_HIGH | PAD_CTL_HYS_ENABLE |
		PAD_CTL_100K_PU | PAD_CTL_ODE_OPENDRAIN_NONE |
		PAD_CTL_SRE_FAST);

	gpio_direction_input(IOMUX_TO_GPIO(cd));
	gpio_direction_input(IOMUX_TO_GPIO(MX51_PIN_GPIO1_1));

	/* Internal SDHC1 IOMUX + SDHC2 IOMUX on old boards */
	if (machine_is_efikasb() || (machine_is_efikamx() &&
		(get_efika_rev() < EFIKAMX_BOARD_REV_12))) {
		/* SDHC1 IOMUX */
		mxc_request_iomux(MX51_PIN_SD1_CMD,
			IOMUX_CONFIG_ALT0 | IOMUX_CONFIG_SION);
		mxc_iomux_set_pad(MX51_PIN_SD1_CMD,
			PAD_CTL_PUE_KEEPER | PAD_CTL_PKE_ENABLE |
			PAD_CTL_DRV_HIGH | PAD_CTL_47K_PU | PAD_CTL_SRE_FAST);

		mxc_request_iomux(MX51_PIN_SD1_CLK,
			IOMUX_CONFIG_ALT0 | IOMUX_CONFIG_SION);
		mxc_iomux_set_pad(MX51_PIN_SD1_CLK,
			PAD_CTL_PUE_KEEPER | PAD_CTL_PKE_ENABLE |
			PAD_CTL_DRV_HIGH | PAD_CTL_47K_PU | PAD_CTL_SRE_FAST);

		mxc_request_iomux(MX51_PIN_SD1_DATA0, IOMUX_CONFIG_ALT0);
		mxc_iomux_set_pad(MX51_PIN_SD1_DATA0,
			PAD_CTL_PUE_KEEPER | PAD_CTL_PKE_ENABLE |
			PAD_CTL_DRV_HIGH | PAD_CTL_47K_PU | PAD_CTL_SRE_FAST);

		mxc_request_iomux(MX51_PIN_SD1_DATA1, IOMUX_CONFIG_ALT0);
		mxc_iomux_set_pad(MX51_PIN_SD1_DATA1,
			PAD_CTL_PUE_KEEPER | PAD_CTL_PKE_ENABLE |
			PAD_CTL_DRV_HIGH | PAD_CTL_47K_PU | PAD_CTL_SRE_FAST);

		mxc_request_iomux(MX51_PIN_SD1_DATA2, IOMUX_CONFIG_ALT0);
		mxc_iomux_set_pad(MX51_PIN_SD1_DATA2,
			PAD_CTL_PUE_KEEPER | PAD_CTL_PKE_ENABLE |
			PAD_CTL_DRV_HIGH | PAD_CTL_47K_PU | PAD_CTL_SRE_FAST);

		mxc_request_iomux(MX51_PIN_SD1_DATA3, IOMUX_CONFIG_ALT0);
		mxc_iomux_set_pad(MX51_PIN_SD1_DATA3,
			PAD_CTL_PUE_KEEPER | PAD_CTL_PKE_ENABLE |
			PAD_CTL_DRV_HIGH | PAD_CTL_47K_PU | PAD_CTL_SRE_FAST);

		/* SDHC2 IOMUX */
		mxc_request_iomux(MX51_PIN_SD2_CMD,
			IOMUX_CONFIG_ALT0 | IOMUX_CONFIG_SION);
		mxc_iomux_set_pad(MX51_PIN_SD2_CMD,
			PAD_CTL_DRV_MAX | PAD_CTL_22K_PU | PAD_CTL_SRE_FAST);

		mxc_request_iomux(MX51_PIN_SD2_CLK,
			IOMUX_CONFIG_ALT0 | IOMUX_CONFIG_SION);
		mxc_iomux_set_pad(MX51_PIN_SD2_CLK,
			PAD_CTL_DRV_MAX | PAD_CTL_22K_PU | PAD_CTL_SRE_FAST);

		mxc_request_iomux(MX51_PIN_SD2_DATA0, IOMUX_CONFIG_ALT0);
		mxc_iomux_set_pad(MX51_PIN_SD2_DATA0,
			PAD_CTL_DRV_MAX | PAD_CTL_22K_PU | PAD_CTL_SRE_FAST);

		mxc_request_iomux(MX51_PIN_SD2_DATA1, IOMUX_CONFIG_ALT0);
		mxc_iomux_set_pad(MX51_PIN_SD2_DATA1,
			PAD_CTL_DRV_MAX | PAD_CTL_22K_PU | PAD_CTL_SRE_FAST);

		mxc_request_iomux(MX51_PIN_SD2_DATA2, IOMUX_CONFIG_ALT0);
		mxc_iomux_set_pad(MX51_PIN_SD2_DATA2,
			PAD_CTL_DRV_MAX | PAD_CTL_22K_PU | PAD_CTL_SRE_FAST);

		mxc_request_iomux(MX51_PIN_SD2_DATA3, IOMUX_CONFIG_ALT0);
		mxc_iomux_set_pad(MX51_PIN_SD2_DATA3,
			PAD_CTL_DRV_MAX | PAD_CTL_22K_PU | PAD_CTL_SRE_FAST);

		/* SDHC2 Control lines IOMUX */
		mxc_request_iomux(MX51_PIN_GPIO1_7,
			IOMUX_CONFIG_ALT0 | IOMUX_CONFIG_SION);
		mxc_iomux_set_pad(MX51_PIN_GPIO1_7,
			PAD_CTL_DRV_HIGH | PAD_CTL_HYS_ENABLE |
			PAD_CTL_PUE_KEEPER | PAD_CTL_100K_PU |
			PAD_CTL_ODE_OPENDRAIN_NONE |
			PAD_CTL_PKE_ENABLE | PAD_CTL_SRE_FAST);
		mxc_request_iomux(MX51_PIN_GPIO1_8,
			IOMUX_CONFIG_ALT0 | IOMUX_CONFIG_SION);
		mxc_iomux_set_pad(MX51_PIN_GPIO1_8,
			PAD_CTL_DRV_HIGH | PAD_CTL_HYS_ENABLE |
			PAD_CTL_100K_PU | PAD_CTL_ODE_OPENDRAIN_NONE |
			PAD_CTL_SRE_FAST);

		gpio_direction_input(IOMUX_TO_GPIO(MX51_PIN_GPIO1_8));
		gpio_direction_input(IOMUX_TO_GPIO(MX51_PIN_GPIO1_7));

		ret = fsl_esdhc_initialize(bis, &esdhc_cfg[0]);
		if (!ret)
			ret = fsl_esdhc_initialize(bis, &esdhc_cfg[1]);
	} else {	/* New boards use only SDHC1 */
		/* SDHC1 IOMUX */
		mxc_request_iomux(MX51_PIN_SD1_CMD,
			IOMUX_CONFIG_ALT0 | IOMUX_CONFIG_SION);
		mxc_iomux_set_pad(MX51_PIN_SD1_CMD,
			PAD_CTL_DRV_MAX | PAD_CTL_22K_PU | PAD_CTL_SRE_FAST);

		mxc_request_iomux(MX51_PIN_SD1_CLK,
			IOMUX_CONFIG_ALT0 | IOMUX_CONFIG_SION);
		mxc_iomux_set_pad(MX51_PIN_SD1_CLK,
			PAD_CTL_DRV_MAX | PAD_CTL_22K_PU | PAD_CTL_SRE_FAST);

		mxc_request_iomux(MX51_PIN_SD1_DATA0, IOMUX_CONFIG_ALT0);
		mxc_iomux_set_pad(MX51_PIN_SD1_DATA0,
			PAD_CTL_DRV_MAX | PAD_CTL_22K_PU | PAD_CTL_SRE_FAST);

		mxc_request_iomux(MX51_PIN_SD1_DATA1, IOMUX_CONFIG_ALT0);
		mxc_iomux_set_pad(MX51_PIN_SD1_DATA1,
			PAD_CTL_DRV_MAX | PAD_CTL_22K_PU | PAD_CTL_SRE_FAST);

		mxc_request_iomux(MX51_PIN_SD1_DATA2, IOMUX_CONFIG_ALT0);
		mxc_iomux_set_pad(MX51_PIN_SD1_DATA2,
			PAD_CTL_DRV_MAX | PAD_CTL_22K_PU | PAD_CTL_SRE_FAST);

		mxc_request_iomux(MX51_PIN_SD1_DATA3, IOMUX_CONFIG_ALT0);
		mxc_iomux_set_pad(MX51_PIN_SD1_DATA3,
			PAD_CTL_DRV_MAX | PAD_CTL_22K_PU | PAD_CTL_SRE_FAST);

		ret = fsl_esdhc_initialize(bis, &esdhc_cfg[0]);
	}

	return ret;
}
#endif

/*
 * ATA
 */
#ifdef	CONFIG_MX51_PATA
#define	ATA_PAD_CONFIG	(PAD_CTL_DRV_HIGH | PAD_CTL_DRV_VOT_HIGH)
void setup_iomux_ata(void)
{
	mxc_request_iomux(MX51_PIN_NANDF_ALE, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_ALE, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_CS2, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_CS2, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_CS3, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_CS3, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_CS4, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_CS4, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_CS5, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_CS5, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_CS6, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_CS6, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_RE_B, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_RE_B, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_WE_B, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_WE_B, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_CLE, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_CLE, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_RB0, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_RB0, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_WP_B, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_WP_B, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_GPIO_NAND, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_GPIO_NAND, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_RB1, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_RB1, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_D0, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D0, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_D1, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D1, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_D2, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D2, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_D3, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D3, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_D4, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D4, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_D5, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D5, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_D6, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D6, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_D7, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D7, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_D8, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D8, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_D9, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D9, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_D10, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D10, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_D11, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D11, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_D12, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D12, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_D13, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D13, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_D14, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D14, ATA_PAD_CONFIG);
	mxc_request_iomux(MX51_PIN_NANDF_D15, IOMUX_CONFIG_ALT1);
	mxc_iomux_set_pad(MX51_PIN_NANDF_D15, ATA_PAD_CONFIG);
}
#else
static inline void setup_iomux_ata(void) { }
#endif

/*
 * EHCI USB
 */
#ifdef	CONFIG_CMD_USB
extern void setup_iomux_usb(void);
#else
static inline void setup_iomux_usb(void) { }
#endif

/*
 * LED configuration
 */
void setup_iomux_led(void)
{
	if (machine_is_efikamx()) {
		/* Blue LED */
		mxc_request_iomux(MX51_PIN_CSI1_D9, IOMUX_CONFIG_ALT3);
		gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_CSI1_D9), 0);

		/* Green LED */
		mxc_request_iomux(MX51_PIN_CSI1_VSYNC, IOMUX_CONFIG_ALT3);
		gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_CSI1_VSYNC), 0);

		/* Red LED */
		mxc_request_iomux(MX51_PIN_CSI1_HSYNC, IOMUX_CONFIG_ALT3);
		gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_CSI1_HSYNC), 0);
	} else {
		/* CAPS-LOCK LED */
		mxc_request_iomux(MX51_PIN_EIM_CS0, IOMUX_CONFIG_GPIO);
		gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_EIM_CS0), 0);

		/* ALARM-LED LED */
		mxc_request_iomux(MX51_PIN_GPIO1_3, IOMUX_CONFIG_GPIO);
		gpio_direction_output(IOMUX_TO_GPIO(MX51_PIN_GPIO1_3), 0);
	}
}

void efikamx_toggle_led(uint32_t mask)
{
	if (machine_is_efikamx()) {
		gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_CSI1_D9),
				mask & EFIKAMX_LED_BLUE);
		gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_CSI1_VSYNC),
				mask & EFIKAMX_LED_GREEN);
		gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_CSI1_HSYNC),
				mask & EFIKAMX_LED_RED);
	} else {
		gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_EIM_CS0),
				mask & EFIKAMX_LED_BLUE);
		gpio_set_value(IOMUX_TO_GPIO(MX51_PIN_GPIO1_3),
				!(mask & EFIKAMX_LED_GREEN));
	}
}

/*
 * Board initialization
 */
static void init_drive_strength(void)
{
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_PKEDDR, PAD_CTL_DDR_INPUT_CMOS);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_PKEADDR, PAD_CTL_PKE_ENABLE);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDRAPKS, PAD_CTL_PUE_KEEPER);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDRAPUS, PAD_CTL_100K_PU);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDR_SR_A1, PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDR_A0, PAD_CTL_DRV_HIGH);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDR_A1, PAD_CTL_DRV_HIGH);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_RAS,
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_CAS,
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_PKEDDR, PAD_CTL_PKE_ENABLE);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDRPKS, PAD_CTL_PUE_KEEPER);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_HYSDDR0, PAD_CTL_HYS_NONE);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_HYSDDR1, PAD_CTL_HYS_NONE);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_HYSDDR2, PAD_CTL_HYS_NONE);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_HYSDDR3, PAD_CTL_HYS_NONE);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDR_SR_B0, PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDR_SR_B1, PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDR_SR_B2, PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDR_SR_B4, PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DDRPUS, PAD_CTL_100K_PU);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_INMODE1, PAD_CTL_DDR_INPUT_CMOS);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DRAM_B0, PAD_CTL_DRV_MEDIUM);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DRAM_B1, PAD_CTL_DRV_MEDIUM);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DRAM_B2, PAD_CTL_DRV_MEDIUM);
	mxc_iomux_set_pad(MX51_PIN_CTL_GRP_DRAM_B4, PAD_CTL_DRV_MEDIUM);

	/* Setting pad options */
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_SDWE,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_SDCKE0,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_SDCKE1,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_SDCLK,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_SDQS0,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_SDQS1,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_SDQS2,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_SDQS3,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_CS0,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_CS1,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_DQM0,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_DQM1,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_DQM2,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX51_PIN_CTL_DRAM_DQM3,
		PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_KEEPER |
		PAD_CTL_DRV_HIGH | PAD_CTL_SRE_FAST);
}

int board_early_init_f(void)
{
	init_drive_strength();

	setup_iomux_uart();
	setup_iomux_spi();
	setup_iomux_led();

	return 0;
}

int board_init(void)
{
	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x100;

	return 0;
}

int board_late_init(void)
{
	setup_iomux_spi();

	power_init();

	setup_iomux_led();
	setup_iomux_ata();
	setup_iomux_usb();

	if (machine_is_efikasb())
		setenv("preboot", "usb reset ; setenv stdin usbkbd\0");

	setup_iomux_led();

	efikamx_toggle_led(EFIKAMX_LED_BLUE);

	return 0;
}

int checkboard(void)
{
	u32 rev = get_efika_rev();

	if (machine_is_efikamx()) {
		printf("Board: Efika MX, rev1.%i\n", rev & 0xf);
		return 0;
	} else {
		switch (rev) {
		case EFIKASB_BOARD_REV_13:
			printf("Board: Efika SB rev1.3\n");
			break;
		case EFIKASB_BOARD_REV_20:
			printf("Board: Efika SB rev2.0\n");
			break;
		default:
			printf("Board: Efika SB, rev Unknown\n");
			break;
		}
	}

	return 0;
}
