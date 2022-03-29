/*
 * (C) Copyright 2004-2011
 * Texas Instruments, <www.ti.com>
 *
 * Author :
 *	Sunil Kumar <sunilsaini05@gmail.com>
 *	Shashi Ranjan <shashiranjanmca05@gmail.com>
 *
 * Derived from Beagle Board and 3430 SDP code by
 *	Richard Woodruff <r-woodruff2@ti.com>
 *	Syed Mohammed Khasim <khasim@ti.com>
 *
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
#ifdef CONFIG_STATUS_LED
#include <status_led.h>
#endif
#include <twl4030.h>
#include <linux/mtd/nand.h>
#include <asm/io.h>
#include <asm/arch/mmc_host_def.h>
#include <asm/arch/mux.h>
#include <asm/arch/mem.h>
#include <asm/arch/sys_proto.h>
#include <asm/gpio.h>
#include <asm/mach-types.h>
#include "beagle.h"
#include <command.h>

#ifdef CONFIG_USB_EHCI
#include <usb.h>
#include <asm/ehci-omap.h>
#endif

#define pr_debug(fmt, args...) debug(fmt, ##args)

#define TWL4030_I2C_BUS			0
#define EXPANSION_EEPROM_I2C_BUS	1
#define EXPANSION_EEPROM_I2C_ADDRESS	0x50

#define TINCANTOOLS_ZIPPY		0x01000100
#define TINCANTOOLS_ZIPPY2		0x02000100
#define TINCANTOOLS_TRAINER		0x04000100
#define TINCANTOOLS_SHOWDOG		0x03000100
#define KBADC_BEAGLEFPGA		0x01000600
#define LW_BEAGLETOUCH			0x01000700
#define BRAINMUX_LCDOG			0x01000800
#define BRAINMUX_LCDOGTOUCH		0x02000800
#define BBTOYS_WIFI			0x01000B00
#define BBTOYS_VGA			0x02000B00
#define BBTOYS_LCD			0x03000B00
#define BCT_BRETTL3			0x01000F00
#define BCT_BRETTL4			0x02000F00
#define BEAGLE_NO_EEPROM		0xffffffff

DECLARE_GLOBAL_DATA_PTR;

static struct {
	unsigned int device_vendor;
	unsigned char revision;
	unsigned char content;
	char fab_revision[8];
	char env_var[16];
	char env_setting[64];
} expansion_config;

/*
 * Routine: board_init
 * Description: Early hardware init.
 */
int board_init(void)
{
	gpmc_init(); /* in SRAM or SDRAM, finish GPMC */
	/* board id for Linux */
	gd->bd->bi_arch_number = MACH_TYPE_OMAP3_BEAGLE;
	/* boot param addr */
	gd->bd->bi_boot_params = (OMAP34XX_SDRC_CS0 + 0x100);

#if defined(CONFIG_STATUS_LED) && defined(STATUS_LED_BOOT)
	status_led_set (STATUS_LED_BOOT, STATUS_LED_ON);
#endif

	return 0;
}

/*
 * Routine: get_board_revision
 * Description: Detect if we are running on a Beagle revision Ax/Bx,
 *		C1/2/3, C4 or xM. This can be done by reading
 *		the level of GPIO173, GPIO172 and GPIO171. This should
 *		result in
 *		GPIO173, GPIO172, GPIO171: 1 1 1 => Ax/Bx
 *		GPIO173, GPIO172, GPIO171: 1 1 0 => C1/2/3
 *		GPIO173, GPIO172, GPIO171: 1 0 1 => C4
 *		GPIO173, GPIO172, GPIO171: 0 0 0 => xM
 */
int get_board_revision(void)
{
	int revision;

	if (!gpio_request(171, "") &&
	    !gpio_request(172, "") &&
	    !gpio_request(173, "")) {

		gpio_direction_input(171);
		gpio_direction_input(172);
		gpio_direction_input(173);

		revision = gpio_get_value(173) << 2 |
			   gpio_get_value(172) << 1 |
			   gpio_get_value(171);
	} else {
		printf("Error: unable to acquire board revision GPIOs\n");
		revision = -1;
	}

	return revision;
}

#ifdef CONFIG_SPL_BUILD
/*
 * Routine: get_board_mem_timings
 * Description: If we use SPL then there is no x-loader nor config header
 * so we have to setup the DDR timings ourself on both banks.
 */
void get_board_mem_timings(u32 *mcfg, u32 *ctrla, u32 *ctrlb, u32 *rfr_ctrl,
		u32 *mr)
{
	int pop_mfr, pop_id;

	/*
	 * We need to identify what PoP memory is on the board so that
	 * we know what timings to use.  If we can't identify it then
	 * we know it's an xM.  To map the ID values please see nand_ids.c
	 */
	identify_nand_chip(&pop_mfr, &pop_id);

	*mr = MICRON_V_MR_165;
	switch (get_board_revision()) {
	case REVISION_C4:
		if (pop_mfr == NAND_MFR_STMICRO && pop_id == 0xba) {
			/* 512MB DDR */
			*mcfg = NUMONYX_V_MCFG_165(512 << 20);
			*ctrla = NUMONYX_V_ACTIMA_165;
			*ctrlb = NUMONYX_V_ACTIMB_165;
			*rfr_ctrl = SDP_3430_SDRC_RFR_CTRL_165MHz;
			break;
		} else if (pop_mfr == NAND_MFR_MICRON && pop_id == 0xba) {
			/* Beagleboard Rev C4, 512MB Nand/256MB DDR*/
			*mcfg = MICRON_V_MCFG_165(128 << 20);
			*ctrla = MICRON_V_ACTIMA_165;
			*ctrlb = MICRON_V_ACTIMB_165;
			*rfr_ctrl = SDP_3430_SDRC_RFR_CTRL_165MHz;
			break;
		} else if (pop_mfr == NAND_MFR_MICRON && pop_id == 0xbc) {
			/* Beagleboard Rev C5, 256MB DDR */
			*mcfg = MICRON_V_MCFG_200(256 << 20);
			*ctrla = MICRON_V_ACTIMA_200;
			*ctrlb = MICRON_V_ACTIMB_200;
			*rfr_ctrl = SDP_3430_SDRC_RFR_CTRL_200MHz;
			break;
		}
	case REVISION_XM_A:
	case REVISION_XM_B:
	case REVISION_XM_C:
		if (pop_mfr == 0) {
			/* 256MB DDR */
			*mcfg = MICRON_V_MCFG_200(256 << 20);
			*ctrla = MICRON_V_ACTIMA_200;
			*ctrlb = MICRON_V_ACTIMB_200;
			*rfr_ctrl = SDP_3430_SDRC_RFR_CTRL_200MHz;
		} else {
			/* 512MB DDR */
			*mcfg = NUMONYX_V_MCFG_165(512 << 20);
			*ctrla = NUMONYX_V_ACTIMA_165;
			*ctrlb = NUMONYX_V_ACTIMB_165;
			*rfr_ctrl = SDP_3430_SDRC_RFR_CTRL_165MHz;
		}
		break;
	default:
		/* Assume 128MB and Micron/165MHz timings to be safe */
		*mcfg = MICRON_V_MCFG_165(128 << 20);
		*ctrla = MICRON_V_ACTIMA_165;
		*ctrlb = MICRON_V_ACTIMB_165;
		*rfr_ctrl = SDP_3430_SDRC_RFR_CTRL_165MHz;
	}
}
#endif

/*
 * Routine: get_expansion_id
 * Description: This function checks for expansion board by checking I2C
 *		bus 1 for the availability of an AT24C01B serial EEPROM.
 *		returns the device_vendor field from the EEPROM
 */
unsigned int get_expansion_id(void)
{
	i2c_set_bus_num(EXPANSION_EEPROM_I2C_BUS);

	/* return BEAGLE_NO_EEPROM if eeprom doesn't respond */
	if (i2c_probe(EXPANSION_EEPROM_I2C_ADDRESS) == 1) {
		i2c_set_bus_num(TWL4030_I2C_BUS);
		return BEAGLE_NO_EEPROM;
	}

	/* read configuration data */
	i2c_read(EXPANSION_EEPROM_I2C_ADDRESS, 0, 1, (u8 *)&expansion_config,
		 sizeof(expansion_config));

	i2c_set_bus_num(TWL4030_I2C_BUS);

	return expansion_config.device_vendor;
}

/*
 * Configure DSS to display background color on DVID
 * Configure VENC to display color bar on S-Video
 */
void beagle_display_init(void)
{
	omap3_dss_venc_config(&venc_config_std_tv, VENC_HEIGHT, VENC_WIDTH);
	switch (get_board_revision()) {
	case REVISION_AXBX:
	case REVISION_CX:
	case REVISION_C4:
		omap3_dss_panel_config(&dvid_cfg);
		break;
	case REVISION_XM_A:
	case REVISION_XM_B:
	case REVISION_XM_C:
	default:
		omap3_dss_panel_config(&dvid_cfg_xm);
		break;
	}
}

/*
 * Enable DVI power
 */
static void beagle_dvi_pup(void)
{
	uchar val;

	switch (get_board_revision()) {
	case REVISION_AXBX:
	case REVISION_CX:
	case REVISION_C4:
	case REVISION_XM_A:
		gpio_request(170, "");
		gpio_direction_output(170, 0);
		gpio_set_value(170, 1);
		break;
	case REVISION_XM_B:
	case REVISION_XM_C:
	default:
		#define GPIODATADIR1 (TWL4030_BASEADD_GPIO+3)
		#define GPIODATAOUT1 (TWL4030_BASEADD_GPIO+6)

		i2c_read(TWL4030_CHIP_GPIO, GPIODATADIR1, 1, &val, 1);
		val |= 4;
		i2c_write(TWL4030_CHIP_GPIO, GPIODATADIR1, 1, &val, 1);

		i2c_read(TWL4030_CHIP_GPIO, GPIODATAOUT1, 1, &val, 1);
		val |= 4;
		i2c_write(TWL4030_CHIP_GPIO, GPIODATAOUT1, 1, &val, 1);
		break;
	}
}

/*
 * Routine: misc_init_r
 * Description: Configure board specific parts
 */
int misc_init_r(void)
{
	struct gpio *gpio5_base = (struct gpio *)OMAP34XX_GPIO5_BASE;
	struct gpio *gpio6_base = (struct gpio *)OMAP34XX_GPIO6_BASE;
	struct control_prog_io *prog_io_base = (struct control_prog_io *)OMAP34XX_CTRL_BASE;

	/* Enable i2c2 pullup resisters */
	writel(~(PRG_I2C2_PULLUPRESX), &prog_io_base->io1);

	switch (get_board_revision()) {
	case REVISION_AXBX:
		printf("Beagle Rev Ax/Bx\n");
		setenv("beaglerev", "AxBx");
		break;
	case REVISION_CX:
		printf("Beagle Rev C1/C2/C3\n");
		setenv("beaglerev", "Cx");
		MUX_BEAGLE_C();
		break;
	case REVISION_C4:
		printf("Beagle Rev C4\n");
		setenv("beaglerev", "C4");
		MUX_BEAGLE_C();
		/* Set VAUX2 to 1.8V for EHCI PHY */
		twl4030_pmrecv_vsel_cfg(TWL4030_PM_RECEIVER_VAUX2_DEDICATED,
					TWL4030_PM_RECEIVER_VAUX2_VSEL_18,
					TWL4030_PM_RECEIVER_VAUX2_DEV_GRP,
					TWL4030_PM_RECEIVER_DEV_GRP_P1);
		break;
	case REVISION_XM_A:
		printf("Beagle xM Rev A\n");
		setenv("beaglerev", "xMA");
		MUX_BEAGLE_XM();
		/* Set VAUX2 to 1.8V for EHCI PHY */
		twl4030_pmrecv_vsel_cfg(TWL4030_PM_RECEIVER_VAUX2_DEDICATED,
					TWL4030_PM_RECEIVER_VAUX2_VSEL_18,
					TWL4030_PM_RECEIVER_VAUX2_DEV_GRP,
					TWL4030_PM_RECEIVER_DEV_GRP_P1);
		break;
	case REVISION_XM_B:
		printf("Beagle xM Rev B\n");
		setenv("beaglerev", "xMB");
		MUX_BEAGLE_XM();
		/* Set VAUX2 to 1.8V for EHCI PHY */
		twl4030_pmrecv_vsel_cfg(TWL4030_PM_RECEIVER_VAUX2_DEDICATED,
					TWL4030_PM_RECEIVER_VAUX2_VSEL_18,
					TWL4030_PM_RECEIVER_VAUX2_DEV_GRP,
					TWL4030_PM_RECEIVER_DEV_GRP_P1);
		break;
	case REVISION_XM_C:
		printf("Beagle xM Rev C\n");
		setenv("beaglerev", "xMC");
		MUX_BEAGLE_XM();
		/* Set VAUX2 to 1.8V for EHCI PHY */
		twl4030_pmrecv_vsel_cfg(TWL4030_PM_RECEIVER_VAUX2_DEDICATED,
					TWL4030_PM_RECEIVER_VAUX2_VSEL_18,
					TWL4030_PM_RECEIVER_VAUX2_DEV_GRP,
					TWL4030_PM_RECEIVER_DEV_GRP_P1);
		break;
	default:
		printf("Beagle unknown 0x%02x\n", get_board_revision());
		MUX_BEAGLE_XM();
		/* Set VAUX2 to 1.8V for EHCI PHY */
		twl4030_pmrecv_vsel_cfg(TWL4030_PM_RECEIVER_VAUX2_DEDICATED,
					TWL4030_PM_RECEIVER_VAUX2_VSEL_18,
					TWL4030_PM_RECEIVER_VAUX2_DEV_GRP,
					TWL4030_PM_RECEIVER_DEV_GRP_P1);
	}

	switch (get_expansion_id()) {
	case TINCANTOOLS_ZIPPY:
		printf("Recognized Tincantools Zippy board (rev %d %s)\n",
			expansion_config.revision,
			expansion_config.fab_revision);
		MUX_TINCANTOOLS_ZIPPY();
		setenv("buddy", "zippy");
		break;
	case TINCANTOOLS_ZIPPY2:
		printf("Recognized Tincantools Zippy2 board (rev %d %s)\n",
			expansion_config.revision,
			expansion_config.fab_revision);
		MUX_TINCANTOOLS_ZIPPY();
		setenv("buddy", "zippy2");
		break;
	case TINCANTOOLS_TRAINER:
		printf("Recognized Tincantools Trainer board (rev %d %s)\n",
			expansion_config.revision,
			expansion_config.fab_revision);
		MUX_TINCANTOOLS_ZIPPY();
		MUX_TINCANTOOLS_TRAINER();
		setenv("buddy", "trainer");
		break;
	case TINCANTOOLS_SHOWDOG:
		printf("Recognized Tincantools Showdow board (rev %d %s)\n",
			expansion_config.revision,
			expansion_config.fab_revision);
		/* Place holder for DSS2 definition for showdog lcd */
		setenv("defaultdisplay", "showdoglcd");
		setenv("buddy", "showdog");
		break;
	case KBADC_BEAGLEFPGA:
		printf("Recognized KBADC Beagle FPGA board\n");
		MUX_KBADC_BEAGLEFPGA();
		setenv("buddy", "beaglefpga");
		break;
	case LW_BEAGLETOUCH:
		printf("Recognized Liquidware BeagleTouch board\n");
		setenv("buddy", "beagletouch");
		break;
	case BRAINMUX_LCDOG:
		printf("Recognized Brainmux LCDog board\n");
		setenv("buddy", "lcdog");
		break;
	case BRAINMUX_LCDOGTOUCH:
		printf("Recognized Brainmux LCDog Touch board\n");
		setenv("buddy", "lcdogtouch");
		break;
	case BBTOYS_WIFI:
		printf("Recognized BeagleBoardToys WiFi board\n");
		MUX_BBTOYS_WIFI()
		setenv("buddy", "bbtoys-wifi");
		break;;
	case BBTOYS_VGA:
		printf("Recognized BeagleBoardToys VGA board\n");
		break;;
	case BBTOYS_LCD:
		printf("Recognized BeagleBoardToys LCD board\n");
		break;;
	case BCT_BRETTL3:
		printf("Recognized bct electronic GmbH brettl3 board\n");
		break;
	case BCT_BRETTL4:
		printf("Recognized bct electronic GmbH brettl4 board\n");
		break;
	case BEAGLE_NO_EEPROM:
		printf("No EEPROM on expansion board\n");
		setenv("buddy", "none");
		break;
	default:
		printf("Unrecognized expansion board: %x\n",
			expansion_config.device_vendor);
		setenv("buddy", "unknown");
	}

	if (expansion_config.content == 1)
		setenv(expansion_config.env_var, expansion_config.env_setting);

	twl4030_power_init();
	switch (get_board_revision()) {
	case REVISION_XM_A:
	case REVISION_XM_B:
		twl4030_led_init(TWL4030_LED_LEDEN_LEDBON);
		break;
	default:
		twl4030_led_init(TWL4030_LED_LEDEN_LEDAON | TWL4030_LED_LEDEN_LEDBON);
		break;
	}

	/* Set GPIO states before they are made outputs */
	writel(GPIO23 | GPIO10 | GPIO8 | GPIO2 | GPIO1,
		&gpio6_base->setdataout);
	writel(GPIO31 | GPIO30 | GPIO29 | GPIO28 | GPIO22 | GPIO21 |
		GPIO15 | GPIO14 | GPIO13 | GPIO12, &gpio5_base->setdataout);

	/* Configure GPIOs to output */
	writel(~(GPIO23 | GPIO10 | GPIO8 | GPIO2 | GPIO1), &gpio6_base->oe);
	writel(~(GPIO31 | GPIO30 | GPIO29 | GPIO28 | GPIO22 | GPIO21 |
		GPIO15 | GPIO14 | GPIO13 | GPIO12), &gpio5_base->oe);

	dieid_num_r();

	beagle_dvi_pup();
	beagle_display_init();
	omap3_dss_enable();

	return 0;
}

/*
 * Routine: set_muxconf_regs
 * Description: Setting up the configuration Mux registers specific to the
 *		hardware. Many pins need to be moved from protect to primary
 *		mode.
 */
void set_muxconf_regs(void)
{
	MUX_BEAGLE();
}

#if defined(CONFIG_GENERIC_MMC) && !defined(CONFIG_SPL_BUILD)
int board_mmc_init(bd_t *bis)
{
	omap_mmc_init(0, 0, 0);
	return 0;
}
#endif

#ifdef CONFIG_USB_EHCI
/* Call usb_stop() before starting the kernel */
void show_boot_progress(int val)
{
	if (val == BOOTSTAGE_ID_RUN_OS)
		usb_stop();
}

static struct omap_usbhs_board_data usbhs_bdata = {
	.port_mode[0] = OMAP_EHCI_PORT_MODE_PHY,
	.port_mode[1] = OMAP_EHCI_PORT_MODE_PHY,
	.port_mode[2] = OMAP_USBHS_PORT_MODE_UNUSED
};

int ehci_hcd_init(void)
{
	return omap_ehci_hcd_init(&usbhs_bdata);
}

int ehci_hcd_stop(void)
{
	return omap_ehci_hcd_stop();
}

#endif /* CONFIG_USB_EHCI */
