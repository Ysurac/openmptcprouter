/*
 * (C) Copyright 2005
 * Matthias Fuchs, esd gmbh germany, matthias.fuchs@esd-electronics.com
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

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */

#define CONFIG_405GP		1	/* This is a PPC405 CPU		*/
#define CONFIG_4xx		1	/* ...member of PPC4xx family	*/

#define	CONFIG_SYS_TEXT_BASE	0xFFFC0000

#define CONFIG_BOARD_EARLY_INIT_F 1	/* call board_early_init_f()	*/

#define CONFIG_SYS_CLK_FREQ	33330000 /* external frequency to pll	*/

#define CONFIG_BAUDRATE		9600
#define CONFIG_BOOTDELAY	3	/* autoboot after 3 seconds	*/

#undef	CONFIG_BOOTARGS
#undef	CONFIG_BOOTCOMMAND

#define CONFIG_PREBOOT                  /* enable preboot variable      */

#define CONFIG_LOADS_ECHO	1	/* echo on for serial download	*/
#define CONFIG_SYS_LOADS_BAUD_CHANGE	1	/* allow baudrate change	*/

#define CONFIG_MII		1	/* MII PHY management		*/
#define CONFIG_PHY_ADDR		0	/* PHY address			*/

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME


/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_PCI
#define CONFIG_CMD_IRQ
#define CONFIG_CMD_ELF
#define CONFIG_CMD_I2C
#define CONFIG_CMD_BSP
#define CONFIG_CMD_EEPROM

#undef CONFIG_CMD_NET
#undef CONFIG_CMD_NFS

#undef	CONFIG_WATCHDOG			/* watchdog disabled		*/

#define CONFIG_SDRAM_BANK0	1	/* init onboard SDRAM bank 0	*/

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP			/* undef to save memory		*/
#define CONFIG_SYS_PROMPT	"=> "		/* Monitor Command Prompt	*/

#undef	CONFIG_SYS_HUSH_PARSER			/* use "hush" command parser	*/

#if defined(CONFIG_CMD_KGDB)
#define CONFIG_SYS_CBSIZE	1024		/* Console I/O Buffer Size	*/
#else
#define CONFIG_SYS_CBSIZE	256		/* Console I/O Buffer Size	*/
#endif
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16) /* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	16		/* max number of command args	*/
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size	*/

#define CONFIG_SYS_DEVICE_NULLDEV	1	/* include nulldev device	*/

#define CONFIG_SYS_CONSOLE_INFO_QUIET	1	/* don't print console @ startup*/

#define CONFIG_AUTO_COMPLETE	1       /* add autocompletion support   */

#define CONFIG_SYS_MEMTEST_START	0x0400000	/* memtest works on	*/
#define CONFIG_SYS_MEMTEST_END		0x0C00000	/* 4 ... 12 MB in DRAM	*/

#define CONFIG_CONS_INDEX	2	/* Use UART1			*/
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	1
#define CONFIG_SYS_NS16550_CLK		get_serial_clock()

#undef	CONFIG_SYS_EXT_SERIAL_CLOCK	       /* no external serial clock used */
#define CONFIG_SYS_BASE_BAUD	    691200

/* The following table includes the supported baudrates */
#define CONFIG_SYS_BAUDRATE_TABLE	\
	{ 300, 600, 1200, 2400, 4800, 9600, 19200, 38400,     \
	 57600, 115200, 230400, 460800, 921600 }

#define CONFIG_SYS_LOAD_ADDR	0x100000	/* default load address */
#define CONFIG_SYS_EXTBDINFO	1		/* To use extended board_into (bd_t) */

#define CONFIG_SYS_HZ		1000		/* decrementer freq: 1 ms ticks */

#define CONFIG_LOOPW            1       /* enable loopw command         */

#define CONFIG_ZERO_BOOTDELAY_CHECK	/* check for keypress on bootdelay==0 */

#define CONFIG_VERSION_VARIABLE 1	/* include version env variable */

#define CONFIG_SYS_RX_ETH_BUFFER	16	/* use 16 rx buffer on 405 emac */

/*-----------------------------------------------------------------------
 * PCI stuff
 *-----------------------------------------------------------------------
 */
#define PCI_HOST_ADAPTER 0              /* configure as pci adapter     */
#define PCI_HOST_FORCE  1               /* configure as pci host        */
#define PCI_HOST_AUTO   2               /* detected via arbiter enable  */

#define CONFIG_PCI			/* include pci support	        */
#define CONFIG_PCI_HOST	PCI_HOST_AUTO   /* select pci host function     */
#define CONFIG_PCI_PNP			/* do pci plug-and-play         */
					/* resource configuration       */

#define CONFIG_PCI_SCAN_SHOW            /* print pci devices @ startup  */

#define CONFIG_PCI_CONFIG_HOST_BRIDGE 1 /* don't skip host bridge config*/

#define CONFIG_PCI_BOOTDELAY    0       /* enable pci bootdelay variable*/

#define CONFIG_SYS_PCI_SUBSYS_VENDORID 0x12FE  /* PCI Vendor ID: esd gmbh      */
#define CONFIG_SYS_PCI_SUBSYS_DEVICEID 0x040b  /* PCI Device ID: CPCI-2DP      */
#define CONFIG_SYS_PCI_CLASSCODE       0x0280	/* PCI Class Code: Network/Other*/

#define CONFIG_SYS_PCI_PTM1LA  (bd->bi_memstart) /* point to sdram               */
#define CONFIG_SYS_PCI_PTM1MS  (~(bd->bi_memsize - 1) | 1) /* memsize, enable hard-wired to 1 */
#define CONFIG_SYS_PCI_PTM1PCI 0x00000000      /* Host: use this pci address   */
#define CONFIG_SYS_PCI_PTM2LA	0xef000000	/* point to internal regs + PB0/1 */
#define CONFIG_SYS_PCI_PTM2MS  0xff000001      /* 16MB, enable                  */
#define CONFIG_SYS_PCI_PTM2PCI 0x00000000      /* Host: use this pci address   */

/*-----------------------------------------------------------------------
 * Start addresses for the final memory configuration
 * (Set up by the startup code)
 * Please note that CONFIG_SYS_SDRAM_BASE _must_ start at 0
 */
#define CONFIG_SYS_SDRAM_BASE		0x00000000
#define CONFIG_SYS_FLASH_BASE		0xFFFC0000
#define CONFIG_SYS_MONITOR_BASE	CONFIG_SYS_FLASH_BASE
#define CONFIG_SYS_MONITOR_LEN		(256 * 1024)	/* Reserve 256 kB for Monitor	*/
#define CONFIG_SYS_MALLOC_LEN		(128 * 1024)	/* Reserve 128 kB for malloc()	*/

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTMAPSZ		(8 << 20)	/* Initial Memory map for Linux */
/*-----------------------------------------------------------------------
 * FLASH organization
 */
#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks		*/
#define CONFIG_SYS_MAX_FLASH_SECT	256	/* max number of sectors on one chip	*/

#define CONFIG_SYS_FLASH_ERASE_TOUT	120000	/* Timeout for Flash Erase (in ms)	*/
#define CONFIG_SYS_FLASH_WRITE_TOUT	500	/* Timeout for Flash Write (in ms)	*/

#define CONFIG_SYS_FLASH_WORD_SIZE	unsigned short	/* flash word size (width)	*/
#define CONFIG_SYS_FLASH_ADDR0		0x5555	/* 1st address for flash config cycles	*/
#define CONFIG_SYS_FLASH_ADDR1		0x2AAA	/* 2nd address for flash config cycles	*/

#define CONFIG_SYS_FLASH_READ0		0x0000	/* 0 is standard			*/
#define CONFIG_SYS_FLASH_READ1		0x0001	/* 1 is standard			*/
#define CONFIG_SYS_FLASH_READ2		0x0002	/* 2 is standard			*/

#define CONFIG_SYS_FLASH_EMPTY_INFO		/* print 'E' for empty sector on flinfo */

#define CONFIG_ENV_IS_IN_EEPROM	1	/* use EEPROM for environment vars */
#define CONFIG_ENV_OFFSET		0x000	/* environment starts at the beginning of the EEPROM */
#define CONFIG_ENV_SIZE		0x400	/* 1024 bytes may be used for env vars */

/*-----------------------------------------------------------------------
 * I2C EEPROM (CAT24WC16) for environment
 */
#define CONFIG_HARD_I2C			/* I2c with hardware support */
#define CONFIG_PPC4XX_I2C		/* use PPC4xx driver		*/
#define CONFIG_SYS_I2C_SPEED		400000	/* I2C speed and slave address */
#define CONFIG_SYS_I2C_SLAVE		0x7F

#define CONFIG_SYS_I2C_EEPROM_ADDR	0x50	/* EEPROM CAT28WC08		*/
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN 1	/* Bytes of address		*/
/* mask of address bits that overflow into the "EEPROM chip address"	*/
#define CONFIG_SYS_I2C_EEPROM_ADDR_OVERFLOW	0x07
#define CONFIG_SYS_EEPROM_PAGE_WRITE_BITS 4	/* The Catalyst CAT24WC08 has	*/
					/* 16 byte page write mode using*/
					/* last 4 bits of the address	*/
#define CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS	10   /* and takes up to 10 msec */

#define CONFIG_SYS_EEPROM_WREN         1

/*
 * Init Memory Controller:
 *
 * BR0/1 and OR0/1 (FLASH)
 */
#define FLASH_BASE0_PRELIM	0xFFE00000	/* FLASH bank #0	*/
#define FLASH_BASE1_PRELIM	0               /* FLASH bank #1	*/

/*-----------------------------------------------------------------------
 * External Bus Controller (EBC) Setup
 */

/* Memory Bank 0 (Flash Bank 0) initialization					*/
#define CONFIG_SYS_EBC_PB0AP		0x92015480
#define CONFIG_SYS_EBC_PB0CR		0xFFC5A000  /* BAS=0xFFC,BS=4MB,BU=R/W,BW=16bit */

/* Memory Bank 2 (PB0) initialization					*/
#define CONFIG_SYS_EBC_PB2AP		0x03004580  /* TWT=6,WBN=1,TH=2,RE=1,SOR=1 */
#define CONFIG_SYS_EBC_PB2CR		0xEF018000  /* BAS=0xF00,BS=1MB,BU=R/W,BW=8bit	*/

/* Memory Bank 3 (PB1) initialization				*/
#define CONFIG_SYS_EBC_PB3AP		0x03004580  /* TWT=6,WBN=1,TH=2,RE=1,SOR=1 */
#define CONFIG_SYS_EBC_PB3CR		0xEF118000  /* BAS=0xF01,BS=1MB,BU=R/W,BW=8bit */

/*-----------------------------------------------------------------------
 * Definitions for initial stack pointer and data area (in data cache)
 */
#define CONFIG_SYS_INIT_DCACHE_CS	7	/* use cs # 7 for data cache memory    */

#define CONFIG_SYS_INIT_RAM_ADDR	0x40000000  /* use data cache		       */
#define CONFIG_SYS_INIT_RAM_SIZE	0x2000	/* Size of used area in RAM	       */
#define CONFIG_SYS_GBL_DATA_OFFSET     (CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

/*-----------------------------------------------------------------------
 * GPIO definitions
 */
#define CONFIG_SYS_EEPROM_WP		(0x80000000 >> 13)   /* GPIO13 */
#define CONFIG_SYS_SELF_RST		(0x80000000 >> 14)   /* GPIO14 */
#define CONFIG_SYS_PB_LED		(0x80000000 >> 16)   /* GPIO16 */
#define CONFIG_SYS_INTA_FAKE		(0x80000000 >> 23)   /* GPIO23 */

#endif	/* __CONFIG_H */
