/*
 * Copyright (C) 2007, Guennadi Liakhovetski <lg@denx.de>
 *
 * (C) Copyright 2009 Freescale Semiconductor, Inc.
 *
 * Configuration settings for the MX51EVK Board
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <config_cmd_default.h>

/*
 * High Level Board Configuration Options
 */
/* An i.MX51 CPU */
#define CONFIG_MX51

#define	machine_is_efikamx()	(CONFIG_MACH_TYPE == MACH_TYPE_MX51_EFIKAMX)
#define	machine_is_efikasb()	(CONFIG_MACH_TYPE == MACH_TYPE_MX51_EFIKASB)

#include <asm/arch/imx-regs.h>

#define CONFIG_SYS_MX5_HCLK		24000000
#define CONFIG_SYS_MX5_CLK32		32768
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

#define CONFIG_SYS_TEXT_BASE		0x97800000

#define	CONFIG_L2_OFF
#define	CONFIG_SYS_ICACHE_OFF
#define	CONFIG_SYS_DCACHE_OFF

/*
 * Bootloader Components Configuration
 */
#define CONFIG_CMD_SPI
#define CONFIG_CMD_SF
#define CONFIG_CMD_MMC
#define CONFIG_CMD_FAT
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_IDE
#define CONFIG_CMD_NET
#define CONFIG_CMD_DATE
#undef CONFIG_CMD_IMLS

/*
 * Environmental settings
 */

#define CONFIG_ENV_OFFSET		(6 * 64 * 1024)
#define CONFIG_ENV_SECT_SIZE		(1 * 64 * 1024)
#define CONFIG_ENV_SIZE			(4 * 1024)

/*
 * ATAG setup
 */
#define CONFIG_CMDLINE_TAG		/* enable passing of ATAGs */
#define CONFIG_REVISION_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

#define CONFIG_OF_LIBFDT		1

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 2 * 1024 * 1024)

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_LATE_INIT

/*
 * Hardware drivers
 */
#define CONFIG_MXC_UART
#define CONFIG_MXC_UART_BASE		UART1_BASE
#define CONFIG_CONS_INDEX		1
#define CONFIG_BAUDRATE			115200

#define CONFIG_MXC_GPIO

/*
 * SPI Interface
 */
#ifdef CONFIG_CMD_SPI

#define CONFIG_HARD_SPI
#define CONFIG_MXC_SPI
#define CONFIG_DEFAULT_SPI_BUS		1
#define CONFIG_DEFAULT_SPI_MODE		(SPI_MODE_0 | SPI_CS_HIGH)

/* SPI FLASH */
#ifdef CONFIG_CMD_SF

#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_SST
#define CONFIG_SF_DEFAULT_CS		(1 | 121 << 8)
#define CONFIG_SF_DEFAULT_MODE		(SPI_MODE_0)
#define CONFIG_SF_DEFAULT_SPEED		25000000

#define CONFIG_ENV_SPI_CS		(1 | 121 << 8)
#define CONFIG_ENV_SPI_BUS		0
#define CONFIG_ENV_SPI_MAX_HZ		25000000
#define CONFIG_ENV_SPI_MODE		(SPI_MODE_0)
#define CONFIG_FSL_ENV_IN_SF
#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_SYS_NO_FLASH

#else
#define CONFIG_ENV_IS_NOWHERE
#endif

/* SPI PMIC */
#define CONFIG_PMIC
#define CONFIG_PMIC_SPI
#define CONFIG_PMIC_FSL
#define CONFIG_FSL_PMIC_BUS		0
#define CONFIG_FSL_PMIC_CS		(0 | 120 << 8)
#define CONFIG_FSL_PMIC_CLK		25000000
#define CONFIG_FSL_PMIC_MODE		(SPI_MODE_0 | SPI_CS_HIGH)
#define CONFIG_FSL_PMIC_BITLEN	32
#define CONFIG_RTC_MC13XXX
#endif

/*
 * MMC Configs
 */
#ifdef CONFIG_CMD_MMC
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_FSL_ESDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR	0
#define CONFIG_SYS_FSL_ESDHC_NUM	2
#endif

/*
 * ATA/IDE
 */
#ifdef CONFIG_CMD_IDE
#define CONFIG_LBA48
#undef CONFIG_IDE_LED
#undef CONFIG_IDE_RESET

#define CONFIG_MX51_PATA

#define __io

#define CONFIG_SYS_IDE_MAXBUS		1
#define CONFIG_SYS_IDE_MAXDEVICE	1

#define CONFIG_SYS_ATA_BASE_ADDR	0x83fe0000
#define CONFIG_SYS_ATA_IDE0_OFFSET	0x0

#define CONFIG_SYS_ATA_DATA_OFFSET	0xa0
#define CONFIG_SYS_ATA_REG_OFFSET	0xa0
#define CONFIG_SYS_ATA_ALT_OFFSET	0xd8

#define CONFIG_SYS_ATA_STRIDE		4

#define CONFIG_IDE_PREINIT
#define CONFIG_MXC_ATA_PIO_MODE		4
#endif

/*
 * USB
 */
#define	CONFIG_CMD_USB
#ifdef	CONFIG_CMD_USB
#define	CONFIG_USB_EHCI			/* Enable EHCI USB support */
#define	CONFIG_USB_EHCI_MX5
#define	CONFIG_USB_ULPI
#define	CONFIG_USB_ULPI_VIEWPORT
#define	CONFIG_MXC_USB_PORT	1
#if	(CONFIG_MXC_USB_PORT == 0)
#define	CONFIG_MXC_USB_PORTSC	(1 << 28)
#define	CONFIG_MXC_USB_FLAGS	MXC_EHCI_INTERNAL_PHY
#else
#define	CONFIG_MXC_USB_PORTSC	(2 << 30)
#define	CONFIG_MXC_USB_FLAGS	0
#endif
#define	CONFIG_EHCI_IS_TDI
#define	CONFIG_USB_STORAGE
#define	CONFIG_USB_HOST_ETHER
#define	CONFIG_USB_KEYBOARD
#define	CONFIG_SYS_USB_EVENT_POLL_VIA_CONTROL_EP
#define CONFIG_PREBOOT
/* USB NET */
#ifdef	CONFIG_CMD_NET
#define	CONFIG_USB_ETHER_ASIX
#define	CONFIG_CMD_PING
#define	CONFIG_CMD_DHCP
#endif
#endif /* CONFIG_CMD_USB */

/*
 * Filesystems
 */
#ifdef CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
#ifdef	CONFIG_CMD_NET
#define	CONFIG_CMD_NFS
#endif
#endif

/*
 * Miscellaneous configurable options
 */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_BOOTDELAY		3
#define CONFIG_LOADADDR			0x90800000

#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER		/* use "hush" command parser */
#define CONFIG_SYS_PROMPT		"Efika> "
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE		256	/* Console I/O Buffer Size */
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS		16	/* max number of command args */
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE /* Boot Argument Buffer Size */

#define CONFIG_SYS_MEMTEST_START	0x90000000
#define CONFIG_SYS_MEMTEST_END		0x90010000

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR

#define CONFIG_SYS_HZ			1000
#define CONFIG_CMDLINE_EDITING

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(128 * 1024)	/* regular stack */

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM_1			CSD0_BASE_ADDR
#define PHYS_SDRAM_1_SIZE		(512 * 1024 * 1024)

#define CONFIG_SYS_SDRAM_BASE		(PHYS_SDRAM_1)
#define CONFIG_SYS_INIT_RAM_ADDR	(IRAM_BASE_ADDR)
#define CONFIG_SYS_INIT_RAM_SIZE	(IRAM_SIZE)

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

#define CONFIG_SYS_DDR_CLKSEL		0
#define CONFIG_SYS_CLKTL_CBCDR		0x59E35145

#endif
