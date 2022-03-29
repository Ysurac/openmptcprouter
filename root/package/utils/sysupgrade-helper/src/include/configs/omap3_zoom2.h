/*
 * (C) Copyright 2006-2009
 * Texas Instruments.
 * Richard Woodruff <r-woodruff2@ti.com>
 * Syed Mohammed Khasim <x0khasim@ti.com>
 * Nishanth Menon <nm@ti.com>
 * Tom Rix <Tom.Rix@windriver.com>
 *
 * Configuration settings for the TI OMAP3430 Zoom II board.
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

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 */
#define CONFIG_OMAP		1	/* in a TI OMAP core */
#define CONFIG_OMAP34XX		1	/* which is a 34XX */
#define CONFIG_OMAP3_ZOOM2	1	/* working with Zoom II */

#define CONFIG_SDRC	/* The chip has SDRC controller */

#include <asm/arch/cpu.h>	/* get chip and board defs */
#include <asm/arch/omap3.h>

/*
 * Display CPU and Board information
 */
#define CONFIG_DISPLAY_CPUINFO		1
#define CONFIG_DISPLAY_BOARDINFO	1

/* Clock Defines */
#define V_OSCK			26000000	/* Clock output from T2 */
#define V_SCLK			(V_OSCK >> 1)

#undef CONFIG_USE_IRQ		/* no support for IRQs */
#define CONFIG_MISC_INIT_R

#define CONFIG_CMDLINE_TAG		1	/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS	1
#define CONFIG_INITRD_TAG		1
#define CONFIG_REVISION_TAG		1

#define CONFIG_OF_LIBFDT		1

/*
 * Size of malloc() pool
 */
#define CONFIG_ENV_SIZE			(128 << 10)	/* 128 KiB */
						/* Sector */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (128 << 10))
/*
 * Hardware drivers
 */

/*
 * NS16550 Configuration
 * Zoom2 uses the TL16CP754C on the debug board
 */
#define CONFIG_SERIAL_MULTI		1
/*
 * 0 - 1 : first  USB with respect to the left edge of the debug board
 * 2 - 3 : second USB with respect to the left edge of the debug board
 */
#define ZOOM2_DEFAULT_SERIAL_DEVICE	(&zoom2_serial_device0)

#define V_NS16550_CLK			(1843200)	/* 1.8432 Mhz */

#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_REG_SIZE	(-2)
#define CONFIG_SYS_NS16550_CLK		V_NS16550_CLK
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{115200}

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_GENERIC_MMC		1
#define CONFIG_MMC			1
#define CONFIG_OMAP_HSMMC		1
#define CONFIG_DOS_PARTITION		1

/* Status LED */
#define CONFIG_STATUS_LED		1 /* Status LED enabled	*/
#define CONFIG_BOARD_SPECIFIC_LED	1
#define STATUS_LED_BLUE			0
#define STATUS_LED_RED			1
/* Blue */
#define STATUS_LED_BIT			STATUS_LED_BLUE
#define STATUS_LED_STATE		STATUS_LED_ON
#define STATUS_LED_PERIOD		(CONFIG_SYS_HZ / 2)
/* Red */
#define STATUS_LED_BIT1			STATUS_LED_RED
#define STATUS_LED_STATE1		STATUS_LED_OFF
#define STATUS_LED_PERIOD1		(CONFIG_SYS_HZ / 2)
/* Optional value */
#define STATUS_LED_BOOT			STATUS_LED_BIT

/* GPIO banks */
#ifdef CONFIG_STATUS_LED
#define CONFIG_OMAP3_GPIO_2 /* ZOOM2_LED_BLUE2 */
#define CONFIG_OMAP3_GPIO_6 /* ZOOM2_LED_RED */
#endif
#define CONFIG_OMAP3_GPIO_3 /* board revision */
#define CONFIG_OMAP3_GPIO_5 /* debug board detection, ZOOM2_LED_BLUE */

/* USB */
#define CONFIG_MUSB_UDC			1
#define CONFIG_USB_OMAP3		1
#define CONFIG_TWL4030_USB		1

/* USB device configuration */
#define CONFIG_USB_DEVICE		1
#define CONFIG_USB_TTY			1
/* Change these to suit your needs */
#define CONFIG_USBD_VENDORID		0x0451
#define CONFIG_USBD_PRODUCTID		0x5678
#define CONFIG_USBD_MANUFACTURER	"Texas Instruments"
#define CONFIG_USBD_PRODUCT_NAME	"Zoom2"

/* commands to include */
#include <config_cmd_default.h>

#define CONFIG_CMD_FAT			/* FAT support			*/
#define CONFIG_CMD_I2C			/* I2C serial bus support	*/
#define CONFIG_CMD_MMC			/* MMC support			*/
#define CONFIG_CMD_NAND			/* NAND support			*/
#define CONFIG_CMD_NAND_LOCK_UNLOCK	/* Enable lock/unlock support	*/

#undef CONFIG_CMD_FLASH			/* flinfo, erase, protect	*/
#undef CONFIG_CMD_FPGA			/* FPGA configuration Support	*/
#undef CONFIG_CMD_IMI			/* iminfo			*/
#undef CONFIG_CMD_IMLS			/* List all found images	*/
#undef CONFIG_CMD_NET			/* bootp, tftpboot, rarpboot	*/
#undef CONFIG_CMD_NFS			/* NFS support			*/

#define CONFIG_SYS_NO_FLASH
#define CONFIG_HARD_I2C			1
#define CONFIG_SYS_I2C_SPEED		100000
#define CONFIG_SYS_I2C_SLAVE		1
#define CONFIG_SYS_I2C_BUS		0
#define CONFIG_SYS_I2C_BUS_SELECT	1
#define CONFIG_DRIVER_OMAP34XX_I2C	1

/*
 * TWL4030
 */
#define CONFIG_TWL4030_POWER		1
#define CONFIG_TWL4030_LED		1

/*
 * Board NAND Info.
 */
#define CONFIG_NAND_OMAP_GPMC
#define CONFIG_SYS_NAND_ADDR		NAND_BASE	/* physical address */
							/* to access nand */
#define CONFIG_SYS_NAND_BASE		NAND_BASE	/* physical address */
							/* to access nand at */
							/* CS0 */
#define GPMC_NAND_ECC_LP_x16_LAYOUT	1
#define CONFIG_SYS_MAX_NAND_DEVICE	1

/* Environment information */
#define CONFIG_BOOTDELAY		10

#define CONFIG_EXTRA_ENV_SETTINGS \
	"usbtty=cdc_acm\0" \

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1
#define CONFIG_SYS_INIT_RAM_ADDR	0x4020f800
#define CONFIG_SYS_INIT_RAM_SIZE	0x800
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SYS_INIT_RAM_ADDR + \
					 CONFIG_SYS_INIT_RAM_SIZE - \
					 GENERATED_GBL_DATA_SIZE)
/*
 * Miscellaneous configurable options
 */

#define CONFIG_SYS_PROMPT		"OMAP3 Zoom2 # "
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_CBSIZE		512
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					 sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS		16
#define CONFIG_SYS_BARGSIZE		(CONFIG_SYS_CBSIZE)
/* Memtest from start of memory to 31MB */
#define CONFIG_SYS_MEMTEST_START	(OMAP34XX_SDRC_CS0)
#define CONFIG_SYS_MEMTEST_END		(OMAP34XX_SDRC_CS0 + 0x01F00000)
/* The default load address is the start of memory */
#define CONFIG_SYS_LOAD_ADDR		(OMAP34XX_SDRC_CS0)
/* everything, incl board info, in Hz */
#undef CONFIG_SYS_CLKS_IN_HZ
/*
 * 2430 has 12 GP timers, they can be driven by the SysClk (12/13/19.2) or by
 * 32KHz clk, or from external sig. This rate is divided by a local divisor.
 */
#define CONFIG_SYS_TIMERBASE		(OMAP34XX_GPT2)
#define CONFIG_SYS_PTV			7	/* 2^(PTV+1) */
#define CONFIG_SYS_HZ			((V_SCLK) / (2 << CONFIG_SYS_PTV))

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using these settings
 */
#define CONFIG_STACKSIZE	(128 << 10)	/* regular stack 128 KiB */

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	2	/* CS1 may or may not be populated */
#define PHYS_SDRAM_1		OMAP34XX_SDRC_CS0
#define PHYS_SDRAM_1_SIZE	(32 << 20)	/* at least 32 MiB */
#define PHYS_SDRAM_2		OMAP34XX_SDRC_CS1

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */

/* **** PISMO SUPPORT *** */

/* Configure the PISMO */
#define PISMO1_NAND_SIZE		GPMC_SIZE_128M
#define PISMO1_ONEN_SIZE		GPMC_SIZE_128M

#define CONFIG_SYS_MONITOR_LEN		(256 << 10)	/* Reserve 2 sectors */

#if defined(CONFIG_CMD_NAND)
#define CONFIG_SYS_FLASH_BASE		PISMO1_NAND_BASE
#endif

/* Monitor at start of flash */
#define CONFIG_SYS_MONITOR_BASE		CONFIG_SYS_FLASH_BASE

#define CONFIG_ENV_IS_IN_NAND		1
#define SMNAND_ENV_OFFSET		0x0c0000 /* environment starts here */

#define CONFIG_SYS_ENV_SECT_SIZE	(128 << 10)	/* 128 KiB */
#define CONFIG_ENV_OFFSET		SMNAND_ENV_OFFSET
#define CONFIG_ENV_ADDR			SMNAND_ENV_OFFSET

#define CONFIG_SYS_CACHELINE_SIZE	64

#endif /* __CONFIG_H */
