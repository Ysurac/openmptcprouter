/*
 * Configuation settings for the Motorola MC5275EVB board.
 *
 * By Arthur Shipkowski <art@videon-central.com>
 * Copyright (C) 2005 Videon Central, Inc.
 *
 * Based off of M5272C3 board code by Josef Baumgartner
 * <josef.baumgartner@telex.de>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * board/config.h - configuration options, board specific
 */

#ifndef _M5275EVB_H
#define _M5275EVB_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_MCF52x2			/* define processor family */
#define CONFIG_M5275			/* define processor type */
#define CONFIG_M5275EVB			/* define board type */

#define CONFIG_MCFTMR

#define CONFIG_MCFUART
#define CONFIG_SYS_UART_PORT		(0)
#define CONFIG_BAUDRATE		115200

/* Configuration for environment
 * Environment is embedded in u-boot in the second sector of the flash
 */
#ifndef CONFIG_MONITOR_IS_IN_RAM
#define CONFIG_ENV_OFFSET		0x4000
#define CONFIG_ENV_SECT_SIZE	0x2000
#define CONFIG_ENV_IS_IN_FLASH	1
#else
#define CONFIG_ENV_ADDR		0xffe04000
#define CONFIG_ENV_SECT_SIZE	0x2000
#define CONFIG_ENV_IS_IN_FLASH	1
#endif

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME

/* Available command configuration */
#include <config_cmd_default.h>

#define CONFIG_CMD_CACHE
#define CONFIG_CMD_PING
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET
#define CONFIG_CMD_ELF
#define CONFIG_CMD_FLASH
#define CONFIG_CMD_I2C
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_DHCP

#undef CONFIG_CMD_LOADS
#undef CONFIG_CMD_LOADB

#define CONFIG_MCFFEC
#ifdef CONFIG_MCFFEC
#define CONFIG_MII		1
#define CONFIG_MII_INIT		1
#define CONFIG_SYS_DISCOVER_PHY
#define CONFIG_SYS_RX_ETH_BUFFER	8
#define CONFIG_SYS_FAULT_ECHO_LINK_DOWN
#define CONFIG_SYS_FEC0_PINMUX		0
#define CONFIG_SYS_FEC0_MIIBASE	CONFIG_SYS_FEC0_IOBASE
#define CONFIG_SYS_FEC1_PINMUX		0
#define CONFIG_SYS_FEC1_MIIBASE	CONFIG_SYS_FEC1_IOBASE
#define MCFFEC_TOUT_LOOP	50000
#define CONFIG_HAS_ETH1
/* If CONFIG_SYS_DISCOVER_PHY is not defined - hardcoded */
#ifndef CONFIG_SYS_DISCOVER_PHY
#define FECDUPLEX		FULL
#define FECSPEED		_100BASET
#else
#ifndef CONFIG_SYS_FAULT_ECHO_LINK_DOWN
#define CONFIG_SYS_FAULT_ECHO_LINK_DOWN
#endif
#endif
#endif

/* I2C */
#define CONFIG_FSL_I2C
#define CONFIG_HARD_I2C		/* I2C with hw support */
#undef CONFIG_SOFT_I2C
#define CONFIG_SYS_I2C_SPEED		80000
#define CONFIG_SYS_I2C_SLAVE		0x7F
#define CONFIG_SYS_I2C_OFFSET		0x00000300
#define CONFIG_SYS_IMMR		CONFIG_SYS_MBAR
#define CONFIG_SYS_I2C_PINMUX_REG	(gpio_reg->par_feci2c)
#define CONFIG_SYS_I2C_PINMUX_CLR	(0xFFF0)
#define CONFIG_SYS_I2C_PINMUX_SET	(0x000F)

#define CONFIG_SYS_PROMPT		"-> "
#define CONFIG_SYS_LONGHELP		/* undef to save memory	*/

#if (CONFIG_CMD_KGDB)
#	define CONFIG_SYS_CBSIZE	1024
#else
#	define CONFIG_SYS_CBSIZE	256
#endif
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS		16
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

#define CONFIG_SYS_LOAD_ADDR		0x800000

#define CONFIG_BOOTDELAY	5
#define CONFIG_BOOTCOMMAND	"bootm ffe40000"
#define CONFIG_SYS_MEMTEST_START	0x400
#define CONFIG_SYS_MEMTEST_END		0x380000

#ifdef CONFIG_MCFFEC
#	define CONFIG_NET_RETRY_COUNT	5
#	define CONFIG_OVERWRITE_ETHADDR_ONCE
#endif				/* FEC_ENET */

#define CONFIG_EXTRA_ENV_SETTINGS		\
	"netdev=eth0\0"				\
	"loadaddr=10000\0"			\
	"uboot=u-boot.bin\0"			\
	"load=tftp ${loadaddr} ${uboot}\0"	\
	"upd=run load; run prog\0"		\
	"prog=prot off ffe00000 ffe3ffff;"	\
	"era ffe00000 ffe3ffff;"		\
	"cp.b ${loadaddr} ffe00000 ${filesize};"\
	"save\0"				\
	""

#define CONFIG_SYS_HZ			1000
#define CONFIG_SYS_CLK			150000000

/*
 * Low Level Configuration Settings
 * (address mappings, register initial values, etc.)
 * You should know what you are doing if you make changes here.
 */

#define CONFIG_SYS_MBAR		0x40000000

/*-----------------------------------------------------------------------
 * Definitions for initial stack pointer and data area (in DPRAM)
 */
#define CONFIG_SYS_INIT_RAM_ADDR	0x20000000
#define CONFIG_SYS_INIT_RAM_SIZE	0x10000	/* Size of used area in internal SRAM */
#define CONFIG_SYS_GBL_DATA_OFFSET	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

/*-----------------------------------------------------------------------
 * Start addresses for the final memory configuration
 * (Set up by the startup code)
 * Please note that CONFIG_SYS_SDRAM_BASE _must_ start at 0
 */
#define CONFIG_SYS_SDRAM_BASE		0x00000000
#define CONFIG_SYS_SDRAM_SIZE		16	/* SDRAM size in MB */
#define CONFIG_SYS_FLASH_BASE		CONFIG_SYS_CS0_BASE

#ifdef CONFIG_MONITOR_IS_IN_RAM
#define CONFIG_SYS_MONITOR_BASE	0x20000
#else
#define CONFIG_SYS_MONITOR_BASE	(CONFIG_SYS_FLASH_BASE + 0x400)
#endif

#define CONFIG_SYS_MONITOR_LEN		0x20000
#define CONFIG_SYS_MALLOC_LEN		(256 << 10)
#define CONFIG_SYS_BOOTPARAMS_LEN	64*1024

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization ??
 */
#define CONFIG_SYS_BOOTMAPSZ		(CONFIG_SYS_SDRAM_BASE + (CONFIG_SYS_SDRAM_SIZE << 20))
#define CONFIG_SYS_BOOTM_LEN		(CONFIG_SYS_SDRAM_SIZE << 20)

/*-----------------------------------------------------------------------
 * FLASH organization
 */
#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CONFIG_SYS_MAX_FLASH_SECT	11	/* max number of sectors on one chip */
#define CONFIG_SYS_FLASH_ERASE_TOUT	1000

#define CONFIG_SYS_FLASH_CFI		1
#define CONFIG_FLASH_CFI_DRIVER	1
#define CONFIG_SYS_FLASH_SIZE		0x200000

/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CONFIG_SYS_CACHELINE_SIZE	16

#define ICACHE_STATUS			(CONFIG_SYS_INIT_RAM_ADDR + \
					 CONFIG_SYS_INIT_RAM_SIZE - 8)
#define DCACHE_STATUS			(CONFIG_SYS_INIT_RAM_ADDR + \
					 CONFIG_SYS_INIT_RAM_SIZE - 4)
#define CONFIG_SYS_ICACHE_INV		(CF_CACR_CINV | CF_CACR_INVI)
#define CONFIG_SYS_CACHE_ACR0		(CONFIG_SYS_SDRAM_BASE | \
					 CF_ADDRMASK(CONFIG_SYS_SDRAM_SIZE) | \
					 CF_ACR_EN | CF_ACR_SM_ALL)
#define CONFIG_SYS_CACHE_ICACR		(CF_CACR_CENB | CF_CACR_CINV | \
					 CF_CACR_DISD | CF_CACR_INVI | \
					 CF_CACR_CEIB | CF_CACR_DCM | \
					 CF_CACR_EUSP)

/*-----------------------------------------------------------------------
 * Memory bank definitions
 */
#define CONFIG_SYS_CS0_BASE		0xffe00000
#define CONFIG_SYS_CS0_CTRL		0x00001980
#define CONFIG_SYS_CS0_MASK		0x001F0001

#define CONFIG_SYS_CS1_BASE		0x30000000
#define CONFIG_SYS_CS1_CTRL		0x00001900
#define CONFIG_SYS_CS1_MASK		0x00070001

/*-----------------------------------------------------------------------
 * Port configuration
 */
#define CONFIG_SYS_FECI2C		0x0FA0

#endif	/* _M5275EVB_H */
