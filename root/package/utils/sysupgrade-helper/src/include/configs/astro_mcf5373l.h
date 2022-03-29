/*
 * Configuration settings for the Sentec Cobra Board.
 *
 * (C) Copyright 2003 Josef Baumgartner <josef.baumgartner@telex.de>
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
 * configuration for ASTRO "Urmel" board.
 * Originating from Cobra5272 configuration, messed up by
 * Wolfgang Wegner <w.wegner@astro-kom.de>
 * Please do not bother the original author with bug reports
 * concerning this file.
 */

#ifndef _CONFIG_ASTRO_MCF5373L_H
#define _CONFIG_ASTRO_MCF5373L_H

/*
 * set the card type to actually compile for; either of
 * the possibilities listed below has to be used!
 */
#define CONFIG_ASTRO_V532	1

#if CONFIG_ASTRO_V532
#define ASTRO_ID	0xF8
#elif CONFIG_ASTRO_V512
#define ASTRO_ID	0xFA
#elif CONFIG_ASTRO_TWIN7S2
#define ASTRO_ID	0xF9
#elif CONFIG_ASTRO_V912
#define ASTRO_ID	0xFC
#elif CONFIG_ASTRO_COFDMDUOS2
#define ASTRO_ID	0xFB
#else
#error No card type defined!
#endif

/*
 * Define processor
 * possible values for Urmel board: only Coldfire M5373 processor supported
 * (please do not change)
 */

/* it seems not clear yet which processor defines we should use */
#define CONFIG_MCF537x			/* define processor family */
#define CONFIG_MCF532x			/* define processor family */
#define CONFIG_M5373			/* define processor type */
#define CONFIG_ASTRO5373L		/* define board type */

/* Command line configuration */
#include <config_cmd_default.h>

/*
 * CONFIG_RAM defines if u-boot is loaded via BDM (or started from
 * a different bootloader that has already performed RAM setup) or
 * started directly from flash, which is the regular case for production
 * boards.
 */
#ifdef CONFIG_RAM
#define CONFIG_MONITOR_IS_IN_RAM
#define CONFIG_SYS_TEXT_BASE		0x40020000
#define ENABLE_JFFS	0
#else
#define CONFIG_SYS_TEXT_BASE		0x00000000
#define ENABLE_JFFS	1
#endif

/* Define which commmands should be available at u-boot command prompt */

#define CONFIG_CMD_CACHE
#define CONFIG_CMD_DATE
#define CONFIG_CMD_ELF
#define CONFIG_CMD_FLASH
#define CONFIG_CMD_I2C
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_MISC
#define CONFIG_CMD_XIMG
#undef CONFIG_CMD_NET
#undef CONFIG_CMD_NFS
#if ENABLE_JFFS
#define CONFIG_CMD_JFFS2
#endif
#define CONFIG_CMD_REGINFO
#define CONFIG_CMD_LOADS
#define CONFIG_CMD_LOADB
#define CONFIG_CMD_FPGA
#define CONFIG_CMDLINE_EDITING

#define CONFIG_SYS_HUSH_PARSER

#define CONFIG_MCFRTC
#undef RTC_DEBUG

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

/*
 * Defines processor clock - important for correct timings concerning serial
 * interface etc.
 * CONFIG_SYS_HZ gives unit: 1000 -> 1 Hz ^= 1000 ms
 */

#define CONFIG_SYS_HZ			1000
#define CONFIG_SYS_CLK			80000000
#define CONFIG_SYS_CPU_CLK		(CONFIG_SYS_CLK * 3)
#define CONFIG_SYS_SDRAM_SIZE		32		/* SDRAM size in MB */

#define CONFIG_SYS_CORE_SRAM_SIZE	0x8000
#define CONFIG_SYS_CORE_SRAM		0x80000000

#define CONFIG_SYS_UNIFY_CACHE

/*
 * Define baudrate for UART1 (console output, tftp, ...)
 * default value of CONFIG_BAUDRATE for Sentec board: 19200 baud
 * CONFIG_SYS_BAUDRATE_TABLE defines values that can be selected
 * in u-boot command interface
 */

#define CONFIG_BAUDRATE		115200

#define CONFIG_MCFUART
#define CONFIG_SYS_UART_PORT		(2)
#define CONFIG_SYS_UART2_ALT3_GPIO

/*
 * Watchdog configuration; Watchdog is disabled for running from RAM
 * and set to highest possible value else. Beware there is no check
 * in the watchdog code to validate the timeout value set here!
 */

#ifndef CONFIG_MONITOR_IS_IN_RAM
#define CONFIG_WATCHDOG
#define CONFIG_WATCHDOG_TIMEOUT 3355	/* timeout in milliseconds */
#endif

/*
 * Configuration for environment
 * Environment is located in the last sector of the flash
 */

#ifndef CONFIG_MONITOR_IS_IN_RAM
#define CONFIG_ENV_OFFSET		0x1FF8000
#define CONFIG_ENV_SECT_SIZE		0x8000
#define CONFIG_ENV_IS_IN_FLASH		1
#else
/*
 * environment in RAM - This is used to use a single PC-based application
 * to load an image, load U-Boot, load an environment and then start U-Boot
 * to execute the commands from the environment. Feedback is done via setting
 * and reading memory locations.
 */
#define CONFIG_ENV_ADDR		0x40060000
#define CONFIG_ENV_SECT_SIZE	0x8000
#define CONFIG_ENV_IS_IN_FLASH	1
#endif

/* here we put our FPGA configuration... */
#define CONFIG_MISC_INIT_R	1

/* Define user parameters that have to be customized most likely */

/* AUTOBOOT settings - booting images automatically by u-boot after power on */

/*
 * used for autoboot, delay in seconds u-boot will wait before starting
 * defined (auto-)boot command, setting to -1 disables delay, setting to
 * 0 will too prevent access to u-boot command interface: u-boot then has
 * to be reflashed
 * beware - watchdog is not serviced during autoboot delay time!
 */
#ifdef CONFIG_MONITOR_IS_IN_RAM
#define CONFIG_BOOTDELAY	1
#else
#define CONFIG_BOOTDELAY	1
#endif

/*
 * The following settings will be contained in the environment block ; if you
 * want to use a neutral environment all those settings can be manually set in
 * u-boot: 'set' command
 */

#define _QUOTEME(x)	#x
#define QUOTEME(x)	_QUOTEME(x)

#define CONFIG_EXTRA_ENV_SETTINGS			\
	"loaderversion=11\0"				\
	"card_id="QUOTEME(ASTRO_ID)"\0"			\
	"alterafile=0\0"				\
	"xilinxfile=0\0"				\
	"xilinxload=imxtract 0x540000 $xilinxfile 0x41000000&&"\
		"fpga load 0 0x41000000 $filesize\0" \
	"alteraload=imxtract 0x6c0000 $alterafile 0x41000000&&"\
		"fpga load 1 0x41000000 $filesize\0" \
	"env_default=1\0"				\
	"env_check=if test $env_default -eq 1;"\
		" then setenv env_default 0;saveenv;fi\0"

/*
 * "update" is a non-standard command that has to be supplied
 * by external update.c; This is not included in mainline because
 * it needs non-blocking CFI routines.
 */
#ifdef CONFIG_MONITOR_IS_IN_RAM
#define CONFIG_BOOTCOMMAND	""	/* no autoboot in this case */
#else
#if CONFIG_ASTRO_V532
#define CONFIG_BOOTCOMMAND	"protect off 0x80000 0x1ffffff;run env_check;"\
				"run xilinxload&&run alteraload&&bootm 0x80000;"\
				"update;reset"
#else
#define CONFIG_BOOTCOMMAND	"protect off 0x80000 0x1ffffff;run env_check;"\
				"run xilinxload&&bootm 0x80000;update;reset"
#endif
#endif

/* default bootargs that are considered during boot */
#define CONFIG_BOOTARGS		" console=ttyS2,115200 rootfstype=romfs"\
				" loaderversion=$loaderversion"

#define CONFIG_SYS_PROMPT	"URMEL > "

/* default RAM address for user programs */
#define CONFIG_SYS_LOAD_ADDR	0x20000

#define CONFIG_SYS_LONGHELP

#if (CONFIG_COMMANDS & CONFIG_CMD_KGDB)
#define CONFIG_SYS_CBSIZE		1024
#else
#define CONFIG_SYS_CBSIZE		256
#endif
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS		16
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

#define CONFIG_FPGA_COUNT	1
#define CONFIG_FPGA
#define	CONFIG_FPGA_XILINX
#define	CONFIG_FPGA_SPARTAN3
#define CONFIG_FPGA_ALTERA
#define CONFIG_FPGA_CYCLON2
#define CONFIG_SYS_FPGA_PROG_FEEDBACK
#define CONFIG_SYS_FPGA_WAIT		1000

/* End of user parameters to be customized */

/* Defines memory range for test */

#define CONFIG_SYS_MEMTEST_START	0x40020000
#define CONFIG_SYS_MEMTEST_END		0x41ffffff

/*
 * Low Level Configuration Settings
 * (address mappings, register initial values, etc.)
 * You should know what you are doing if you make changes here.
 */

/* Base register address */

#define CONFIG_SYS_MBAR		0xFC000000	/* Register Base Addrs */

/* System Conf. Reg. & System Protection Reg. */

#define CONFIG_SYS_SCR		0x0003;
#define CONFIG_SYS_SPR		0xffff;

/*
 * Definitions for initial stack pointer and data area (in internal SRAM)
 */
#define CONFIG_SYS_INIT_RAM_ADDR	0x80000000
#define CONFIG_SYS_INIT_RAM_SIZE		0x8000
#define CONFIG_SYS_INIT_RAM_CTRL	0x221
#define CONFIG_SYS_GBL_DATA_OFFSET	(CONFIG_SYS_INIT_RAM_SIZE - \
					 GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

/*
 * Start addresses for the final memory configuration
 * (Set up by the startup code)
 * for MCF5373, the allowable range is 0x40000000 to 0x7FF00000
 */
#define CONFIG_SYS_SDRAM_BASE		0x40000000

/*
 * Chipselect bank definitions
 *
 * CS0 - Flash 32MB (first 16MB)
 * CS1 - Flash 32MB (second half)
 * CS2 - FPGA
 * CS3 - FPGA
 * CS4 - unused
 * CS5 - unused
 */
#define CONFIG_SYS_CS0_BASE		0
#define CONFIG_SYS_CS0_MASK		0x00ff0001
#define CONFIG_SYS_CS0_CTRL		0x00001fc0

#define CONFIG_SYS_CS1_BASE		0x01000000
#define CONFIG_SYS_CS1_MASK		0x00ff0001
#define CONFIG_SYS_CS1_CTRL		0x00001fc0

#define CONFIG_SYS_CS2_BASE		0x20000000
#define CONFIG_SYS_CS2_MASK		0x00ff0001
#define CONFIG_SYS_CS2_CTRL		0x0000fec0

#define CONFIG_SYS_CS3_BASE		0x21000000
#define CONFIG_SYS_CS3_MASK		0x00ff0001
#define CONFIG_SYS_CS3_CTRL		0x0000fec0

#define CONFIG_SYS_FLASH_BASE		0x00000000

#ifdef	CONFIG_MONITOR_IS_IN_RAM
#define CONFIG_SYS_MONITOR_BASE		CONFIG_SYS_TEXT_BASE
#else
/* This is mainly used during relocation in start.S */
#define CONFIG_SYS_MONITOR_BASE		(CONFIG_SYS_FLASH_BASE + 0x400)
#endif
/* Reserve 256 kB for Monitor */
#define CONFIG_SYS_MONITOR_LEN		(256 << 10)

#define CONFIG_SYS_BOOTPARAMS_LEN	(64 * 1024)
/* Reserve 128 kB for malloc() */
#define CONFIG_SYS_MALLOC_LEN		(128 << 10)

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization ??
 */
#define CONFIG_SYS_BOOTMAPSZ		(CONFIG_SYS_SDRAM_BASE + \
						(CONFIG_SYS_SDRAM_SIZE << 20))

/* FLASH organization */
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_MAX_FLASH_SECT	259
#define CONFIG_SYS_FLASH_ERASE_TOUT	1000

#define CONFIG_SYS_FLASH_CFI		1
#define CONFIG_FLASH_CFI_DRIVER		1
#define CONFIG_SYS_FLASH_SIZE		0x2000000
#define CONFIG_SYS_FLASH_PROTECTION	1
#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE	1
#define CONFIG_SYS_FLASH_CFI_NONBLOCK	1

#if ENABLE_JFFS
/* JFFS Partition offset set */
#define CONFIG_SYS_JFFS2_FIRST_BANK    0
#define CONFIG_SYS_JFFS2_NUM_BANKS     1
/* 512k reserved for u-boot */
#define CONFIG_SYS_JFFS2_FIRST_SECTOR  0x40
#endif

/* Cache Configuration */
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

#endif	/* _CONFIG_ASTRO_MCF5373L_H */
