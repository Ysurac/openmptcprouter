/*
 * Copyright (C) 2010 Albert ARIBAUD <albert.u.boot@aribaud.net>
 *
 * Based on original Kirkwood support which is
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef _CONFIG_EDMINIV2_H
#define _CONFIG_EDMINIV2_H

/*
 * Version number information
 */

#define CONFIG_IDENT_STRING	" EDMiniV2"

/*
 * High Level Configuration Options (easy to change)
 */

#define CONFIG_MARVELL		1
#define CONFIG_ARM926EJS	1	/* Basic Architecture */
#define CONFIG_FEROCEON		1	/* CPU Core subversion */
#define CONFIG_ORION5X		1	/* SOC Family Name */
#define CONFIG_88F5182		1	/* SOC Name */
#define CONFIG_MACH_EDMINIV2	1	/* Machine type */

#include <asm/arch/orion5x.h>
/*
 * CLKs configurations
 */

#define CONFIG_SYS_HZ		1000

/*
 * Board-specific values for Orion5x MPP low level init:
 * - MPPs 12 to 15 are SATA LEDs (mode 5)
 * - Others are GPIO/unused (mode 3 for MPP0, mode 5 for
 *   MPP16 to MPP19, mode 0 for others
 */

#define ORION5X_MPP0_7		0x00000003
#define ORION5X_MPP8_15		0x55550000
#define ORION5X_MPP16_23	0x00005555

/*
 * Board-specific values for Orion5x GPIO low level init:
 * - GPIO3 is input (RTC interrupt)
 * - GPIO16 is Power LED control (0 = on, 1 = off)
 * - GPIO17 is Power LED source select (0 = CPLD, 1 = GPIO16)
 * - GPIO18 is Power Button status (0 = Released, 1 = Pressed)
 * - Last GPIO is 26, further bits are supposed to be 0.
 * Enable mask has ones for INPUT, 0 for OUTPUT.
 * Default is LED ON.
 */

#define ORION5X_GPIO_OUT_ENABLE	0x03fcffff
#define ORION5X_GPIO_OUT_VALUE	0x03fcffff

/*
 * NS16550 Configuration
 */

#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
#define CONFIG_SYS_NS16550_CLK		CONFIG_SYS_TCLK
#define CONFIG_SYS_NS16550_COM1		ORION5X_UART0_BASE

/*
 * Serial Port configuration
 * The following definitions let you select what serial you want to use
 * for your console driver.
 */

#define CONFIG_CONS_INDEX	1	/*Console on UART0 */
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE \
	{ 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600 }

/*
 * FLASH configuration
 */

#define CONFIG_SYS_FLASH_CFI
#define CONFIG_FLASH_CFI_DRIVER
#define CONFIG_FLASH_CFI_LEGACY
#define CONFIG_SYS_MAX_FLASH_BANKS	1  /* max num of flash banks       */
#define CONFIG_SYS_MAX_FLASH_SECT	11 /* max num of sects on one chip */
#define CONFIG_SYS_FLASH_BASE		0xfff80000
#define CONFIG_SYS_FLASH_SECTSZ \
	{16384, 8192, 8192, 32768, \
	 65536, 65536, 65536, 65536, 65536, 65536, 65536}

/* auto boot */
#define CONFIG_BOOTDELAY	3	/* default enable autoboot */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_CMDLINE_TAG	1	/* enable passing of ATAGs  */
#define CONFIG_INITRD_TAG	1	/* enable INITRD tag */
#define CONFIG_SETUP_MEMORY_TAGS 1	/* enable memory tag */

#define	CONFIG_SYS_PROMPT	"EDMiniV2> "	/* Command Prompt */
#define	CONFIG_SYS_CBSIZE	1024	/* Console I/O Buff Size */
#define	CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE \
		+sizeof(CONFIG_SYS_PROMPT) + 16)	/* Print Buff */
/*
 * Commands configuration - using default command set for now
 */
#include <config_cmd_default.h>
#define CONFIG_CMD_IDE
#define CONFIG_CMD_I2C
#define CONFIG_CMD_USB

/*
 * Network
 */

#ifdef CONFIG_CMD_NET
#define CONFIG_MVGBE				/* Enable Marvell GbE Driver */
#define CONFIG_MVGBE_PORTS	{1}		/* enable port 0 only */
#define CONFIG_SKIP_LOCAL_MAC_RANDOMIZATION	/* don't randomize MAC */
#define CONFIG_PHY_BASE_ADR	0x8
#define CONFIG_RESET_PHY_R	/* use reset_phy() to init mv8831116 PHY */
#define CONFIG_NETCONSOLE	/* include NetConsole support   */
#define	CONFIG_MII		/* expose smi ove miiphy interface */
#define CONFIG_SYS_FAULT_ECHO_LINK_DOWN	/* detect link using phy */
#define CONFIG_ENV_OVERWRITE	/* ethaddr can be reprogrammed */
#endif

/*
 * IDE
 */
#ifdef CONFIG_CMD_IDE
#define __io
#define CONFIG_IDE_PREINIT
#define CONFIG_DOS_PARTITION
#define CONFIG_CMD_EXT2
/* ED Mini V has an IDE-compatible SATA connector for port 1 */
#define CONFIG_MVSATA_IDE
#define CONFIG_MVSATA_IDE_USE_PORT1
/* Needs byte-swapping for ATA data register */
#define CONFIG_IDE_SWAP_IO
/* Data, registers and alternate blocks are at the same offset */
#define CONFIG_SYS_ATA_DATA_OFFSET	(0x0100)
#define CONFIG_SYS_ATA_REG_OFFSET	(0x0100)
#define CONFIG_SYS_ATA_ALT_OFFSET	(0x0100)
/* Each 8-bit ATA register is aligned to a 4-bytes address */
#define CONFIG_SYS_ATA_STRIDE		4
/* Controller supports 48-bits LBA addressing */
#define CONFIG_LBA48
/* A single bus, a single device */
#define CONFIG_SYS_IDE_MAXBUS		1
#define CONFIG_SYS_IDE_MAXDEVICE	1
/* ATA registers base is at SATA controller base */
#define CONFIG_SYS_ATA_BASE_ADDR	ORION5X_SATA_BASE
/* ATA bus 0 is orion5x port 1 on ED Mini V2 */
#define CONFIG_SYS_ATA_IDE0_OFFSET	ORION5X_SATA_PORT1_OFFSET
/* end of IDE defines */
#endif /* CMD_IDE */

/*
 * Common USB/EHCI configuration
 */
#ifdef CONFIG_CMD_USB
#define CONFIG_USB_EHCI		/* Enable EHCI USB support */
#define CONFIG_USB_EHCI_MARVELL
#define ORION5X_USB20_HOST_PORT_BASE ORION5X_USB20_PORT0_BASE
#define CONFIG_USB_STORAGE
#define CONFIG_DOS_PARTITION
#define CONFIG_ISO_PARTITION
#define CONFIG_SUPPORT_VFAT
#endif /* CONFIG_CMD_USB */

/*
 * I2C related stuff
 */
#ifdef CONFIG_CMD_I2C
#define CONFIG_I2C_MVTWSI
#define CONFIG_I2C_MVTWSI_BASE		ORION5X_TWSI_BASE
#define CONFIG_SYS_I2C_SLAVE		0x0
#define CONFIG_SYS_I2C_SPEED		100000
#endif

/*
 *  Environment variables configurations
 */
#define CONFIG_ENV_IS_IN_FLASH		1
#define CONFIG_ENV_SECT_SIZE		0x2000	/* 16K */
#define CONFIG_ENV_SIZE			0x2000
#define CONFIG_ENV_OFFSET		0x4000	/* env starts here */

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN	(1024 * 128) /* 128kB for malloc() */

/*
 * Other required minimal configurations
 */
#define CONFIG_CONSOLE_INFO_QUIET	/* some code reduction */
#define CONFIG_ARCH_CPU_INIT		/* call arch_cpu_init() */
#define CONFIG_ARCH_MISC_INIT		/* call arch_misc_init() */
#define CONFIG_DISPLAY_CPUINFO		/* Display cpu info */
#define CONFIG_NR_DRAM_BANKS		1

#define CONFIG_STACKSIZE		0x00100000
#define CONFIG_SYS_LOAD_ADDR		0x00800000
#define CONFIG_SYS_MEMTEST_START	0x00400000
#define CONFIG_SYS_MEMTEST_END		0x007fffff
#define CONFIG_SYS_RESET_ADDRESS	0xffff0000
#define CONFIG_SYS_MAXARGS		16

/* Use the HUSH parser */
#define CONFIG_SYS_HUSH_PARSER

/* Enable command line editing */
#define CONFIG_CMDLINE_EDITING

/* provide extensive help */
#define CONFIG_SYS_LONGHELP

/* additions for new relocation code, must be added to all boards */
#define CONFIG_SYS_SDRAM_BASE		0
#define CONFIG_SYS_INIT_SP_ADDR	\
	(CONFIG_SYS_SDRAM_BASE + 0x1000 - GENERATED_GBL_DATA_SIZE)

#endif /* _CONFIG_EDMINIV2_H */
