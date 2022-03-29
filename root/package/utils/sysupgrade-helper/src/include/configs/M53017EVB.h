/*
 * Configuation settings for the Freescale MCF53017EVB.
 *
 * Copyright (C) 2004-2008 Freescale Semiconductor, Inc.
 * TsiChung Liew (Tsi-Chung.Liew@freescale.com)
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

#ifndef _M53017EVB_H
#define _M53017EVB_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_MCF5301x		/* define processor family */
#define CONFIG_M53015		/* define processor type */

#define CONFIG_MCFUART
#define CONFIG_SYS_UART_PORT		(0)
#define CONFIG_BAUDRATE			115200

#undef CONFIG_WATCHDOG
#define CONFIG_WATCHDOG_TIMEOUT		5000

/* Command line configuration */
#include <config_cmd_default.h>

#define CONFIG_CMD_CACHE
#define CONFIG_CMD_DATE
#define CONFIG_CMD_ELF
#define CONFIG_CMD_FLASH
#undef CONFIG_CMD_I2C
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_MISC
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_REGINFO

#define CONFIG_SYS_UNIFY_CACHE

#define CONFIG_MCFFEC
#ifdef CONFIG_MCFFEC
#	define CONFIG_MII		1
#	define CONFIG_MII_INIT		1
#	define CONFIG_SYS_DISCOVER_PHY
#	define CONFIG_SYS_RX_ETH_BUFFER	8
#	define CONFIG_SYS_TX_ETH_BUFFER	8
#	define CONFIG_SYS_FEC_BUF_USE_SRAM
#	define CONFIG_SYS_FAULT_ECHO_LINK_DOWN
#	define CONFIG_HAS_ETH1

#	define CONFIG_SYS_FEC0_PINMUX	0
#	define CONFIG_SYS_FEC0_MIIBASE	CONFIG_SYS_FEC0_IOBASE
#	define CONFIG_SYS_FEC1_PINMUX	0
#	define CONFIG_SYS_FEC1_MIIBASE	CONFIG_SYS_FEC1_IOBASE
#	define MCFFEC_TOUT_LOOP		50000

#	define CONFIG_BOOTARGS		"root=/dev/mtdblock3 rw rootfstype=jffs2"

/* If CONFIG_SYS_DISCOVER_PHY is not defined - hardcoded */
#	ifndef CONFIG_SYS_DISCOVER_PHY
#		define FECDUPLEX	FULL
#		define FECSPEED		_100BASET
#	else
#		ifndef CONFIG_SYS_FAULT_ECHO_LINK_DOWN
#			define CONFIG_SYS_FAULT_ECHO_LINK_DOWN
#		endif
#	endif			/* CONFIG_SYS_DISCOVER_PHY */
#endif

#define CONFIG_MCFRTC
#undef RTC_DEBUG
#define CONFIG_SYS_RTC_CNT		(0x8000)
#define CONFIG_SYS_RTC_SETUP		(RTC_OCEN_OSCBYP | RTC_OCEN_CLKEN)

/* Timer */
#define CONFIG_MCFTMR
#undef CONFIG_MCFPIT

/* I2C */
#define CONFIG_FSL_I2C
#define CONFIG_HARD_I2C			/* I2C with hw support */
#undef CONFIG_SOFT_I2C			/* I2C bit-banged */
#define CONFIG_SYS_I2C_SPEED		80000
#define CONFIG_SYS_I2C_SLAVE		0x7F
#define CONFIG_SYS_I2C_OFFSET		0x58000
#define CONFIG_SYS_IMMR			CONFIG_SYS_MBAR

#define CONFIG_BOOTDELAY		1	/* autoboot after 5 seconds */
#define CONFIG_UDP_CHECKSUM

#ifdef CONFIG_MCFFEC
#	define CONFIG_ETHADDR	00:e0:0c:bc:e5:60
#	define CONFIG_ETH1ADDR	00:e0:0c:bc:e5:61
#	define CONFIG_IPADDR	192.162.1.2
#	define CONFIG_NETMASK	255.255.255.0
#	define CONFIG_SERVERIP	192.162.1.1
#	define CONFIG_GATEWAYIP	192.162.1.1
#	define CONFIG_OVERWRITE_ETHADDR_ONCE
#endif				/* FEC_ENET */

#define CONFIG_HOSTNAME		M53017
#define CONFIG_EXTRA_ENV_SETTINGS		\
	"netdev=eth0\0"				\
	"loadaddr=40010000\0"			\
	"u-boot=u-boot.bin\0"			\
	"load=tftp ${loadaddr) ${u-boot}\0"	\
	"upd=run load; run prog\0"		\
	"prog=prot off 0 3ffff;"		\
	"era 0 3ffff;"				\
	"cp.b ${loadaddr} 0 ${filesize};"	\
	"save\0"				\
	""

#define CONFIG_PRAM		512	/* 512 KB */
#define CONFIG_SYS_PROMPT	"-> "
#define CONFIG_SYS_LONGHELP	/* undef to save memory */

#ifdef CONFIG_CMD_KGDB
#	define CONFIG_SYS_CBSIZE	1024	/* Console I/O Buffer Size */
#else
#	define CONFIG_SYS_CBSIZE	256	/* Console I/O Buffer Size */
#endif

#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)	/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	16		/* max number of cmd args */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE	/* Boot Arg Buf Sz */
#define CONFIG_SYS_LOAD_ADDR	0x40010000

#define CONFIG_SYS_HZ		1000
#define CONFIG_SYS_CLK		80000000
#define CONFIG_SYS_CPU_CLK	CONFIG_SYS_CLK * 3

#define CONFIG_SYS_MBAR		0xFC000000

/*
 * Low Level Configuration Settings
 * (address mappings, register initial values, etc.)
 * You should know what you are doing if you make changes here.
 */
/*
 * Definitions for initial stack pointer and data area (in DPRAM)
 */
#define CONFIG_SYS_INIT_RAM_ADDR	0x80000000
#define CONFIG_SYS_INIT_RAM_SIZE		0x20000	/* Size of used area in internal SRAM */
#define CONFIG_SYS_INIT_RAM_CTRL	0x221
#define CONFIG_SYS_GBL_DATA_OFFSET	((CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE) - 0x10)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

/*
 * Start addresses for the final memory configuration
 * (Set up by the startup code)
 * Please note that CONFIG_SYS_SDRAM_BASE _must_ start at 0
 */
#define CONFIG_SYS_SDRAM_BASE		0x40000000
#define CONFIG_SYS_SDRAM_SIZE		64	/* SDRAM size in MB */
#define CONFIG_SYS_SDRAM_CFG1		0x43711630
#define CONFIG_SYS_SDRAM_CFG2		0x56670000
#define CONFIG_SYS_SDRAM_CTRL		0xE1092000
#define CONFIG_SYS_SDRAM_EMOD		0x80010000
#define CONFIG_SYS_SDRAM_MODE		0x00CD0000

#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE + 0x400
#define CONFIG_SYS_MEMTEST_END		((CONFIG_SYS_SDRAM_SIZE - 3) << 20)

#define CONFIG_SYS_MONITOR_BASE		(CONFIG_SYS_FLASH_BASE + 0x400)
#define CONFIG_SYS_MONITOR_LEN		(256 << 10)	/* Reserve 256 kB for Monitor */

#define CONFIG_SYS_BOOTPARAMS_LEN	64*1024
#define CONFIG_SYS_MALLOC_LEN		(128 << 10)	/* Reserve 128 kB for malloc() */

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
#define CONFIG_SYS_FLASH_CFI
#ifdef CONFIG_SYS_FLASH_CFI
#	define CONFIG_FLASH_CFI_DRIVER		1
#	define CONFIG_SYS_FLASH_USE_BUFFER_WRITE	1
#	define CONFIG_FLASH_SPANSION_S29WS_N	1
#	define CONFIG_SYS_FLASH_SIZE		0x1000000	/* Max size that the board might have */
#	define CONFIG_SYS_FLASH_CFI_WIDTH	FLASH_CFI_16BIT
#	define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks */
#	define CONFIG_SYS_MAX_FLASH_SECT	137	/* max number of sectors on one chip */
#	define CONFIG_SYS_FLASH_PROTECTION	/* "Real" (hardware) sectors protection */
#endif

#define CONFIG_SYS_FLASH_BASE		CONFIG_SYS_CS0_BASE

/* Configuration for environment
 * Environment is embedded in u-boot in the second sector of the flash
 */
#define CONFIG_ENV_OFFSET		0x8000
#define CONFIG_ENV_SIZE			0x1000
#define CONFIG_ENV_SECT_SIZE		0x8000
#define CONFIG_ENV_IS_IN_FLASH		1

/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CONFIG_SYS_CACHELINE_SIZE	16

#define ICACHE_STATUS			(CONFIG_SYS_INIT_RAM_ADDR + \
					 CONFIG_SYS_INIT_RAM_SIZE - 8)
#define DCACHE_STATUS			(CONFIG_SYS_INIT_RAM_ADDR + \
					 CONFIG_SYS_INIT_RAM_SIZE - 4)
#define CONFIG_SYS_ICACHE_INV		(CF_CACR_CINVA)
#define CONFIG_SYS_CACHE_ACR0		(CONFIG_SYS_SDRAM_BASE | \
					 CF_ADDRMASK(CONFIG_SYS_SDRAM_SIZE) | \
					 CF_ACR_EN | CF_ACR_SM_ALL)
#define CONFIG_SYS_CACHE_ICACR		(CF_CACR_EC | CF_CACR_CINVA | \
					 CF_CACR_DCM_P)

/*-----------------------------------------------------------------------
 * Chipselect bank definitions
 */
/*
 * CS0 - NOR Flash
 * CS1 - Ext SRAM
 * CS2 - Available
 * CS3 - Available
 * CS4 - Available
 * CS5 - Available
 */
#define CONFIG_SYS_CS0_BASE		0
#define CONFIG_SYS_CS0_MASK		0x00FF0001
#define CONFIG_SYS_CS0_CTRL		0x00001FA0

#define CONFIG_SYS_CS1_BASE		0xC0000000
#define CONFIG_SYS_CS1_MASK		0x00070001
#define CONFIG_SYS_CS1_CTRL		0x00001FA0

#endif				/* _M53017EVB_H */
