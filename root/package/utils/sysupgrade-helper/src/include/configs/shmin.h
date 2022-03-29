/*
 * Configuation settings for shmin (T-SH7706LAN, T-SH7706LSR)
 *
 * Copyright (C) 2010, 2011 Nobuhiro Iwamatsu <iwamatsu@nigauri.org>
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

#ifndef __SHMIN_H
#define __SHMIN_H

#define CONFIG_SH		1
#define CONFIG_SH3		1
#define CONFIG_CPU_SH7706	1
/* T-SH7706LAN */
#define CONFIG_SHMIN		1
/* T-SH7706LSR*/
/* #define CONFIG_T_SH7706LSR	1 */

#define CONFIG_CMD_FLASH
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_SDRAM
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_NFS
#define CONFIG_CMD_ENV
#define CONFIG_CMD_SAVEENV

#define CONFIG_BAUDRATE		115200
#define CONFIG_BOOTARGS		"console=ttySC0,115200"

/*
 * This board has original boot loader. If you write u-boot to 0x0,
 * you should set undef.
 */
#define CONFIG_VERSION_VARIABLE
#undef  CONFIG_SHOW_BOOT_PROGRESS

/* system */
#define SHMIN_SDRAM_BASE		(0x8C000000)
#define SHMIN_FLASH_BASE_1		(0xA0000000)

#define CONFIG_SYS_TEXT_BASE	0x8DFB0000
#define CONFIG_SYS_LONGHELP		/* undef to save memory	*/
#define CONFIG_SYS_PROMPT	"=> "	/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE	256	/* Buffer size for input from the Console */
#define CONFIG_SYS_PBSIZE	256	/* Buffer size for Console output */
#define CONFIG_SYS_MAXARGS	16	/* max args accepted for monitor commands */
/* Buffer size for Boot Arguments passed to kernel */
#define CONFIG_SYS_BARGSIZE	512
/* List of legal baudrate settings for this board */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600,14400,19200,38400,57600,115200 }

/* SCIF */
#define CONFIG_SCIF_CONSOLE	1
#define CONFIG_CONS_SCIF0	1

/* memory */
#define CONFIG_SYS_SDRAM_BASE		SHMIN_SDRAM_BASE
#define CONFIG_SYS_SDRAM_SIZE		(32 * 1024 * 1024)
#define CONFIG_SYS_MEMTEST_START	SHMIN_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + CONFIG_SYS_SDRAM_SIZE - (256 * 1024))

#define CONFIG_SYS_LOAD_ADDR		(CONFIG_SYS_SDRAM_BASE + 1 * 1024 * 1024)
#define CONFIG_SYS_MONITOR_BASE		(SHMIN_FLASH_BASE_1 + CONFIG_ENV_SECT_SIZE)
#define CONFIG_SYS_MONITOR_LEN		(128 * 1024)
#define CONFIG_SYS_MALLOC_LEN		(256 * 1024)
#define CONFIG_SYS_GBL_DATA_SIZE	256
#define CONFIG_SYS_BOOTMAPSZ		(8 * 1024 * 1024)

/* FLASH */
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_FLASH_CFI_DRIVER
#undef  CONFIG_SYS_FLASH_QUIET_TEST
#define CONFIG_SYS_FLASH_EMPTY_INFO	/* print 'E' for empty sector on flinfo */
#define CONFIG_SYS_FLASH_BASE		SHMIN_FLASH_BASE_1
#define CONFIG_SYS_MAX_FLASH_SECT 11
#define CONFIG_SYS_MAX_FLASH_BANKS	1

#define CONFIG_FLASH_CFI_LEGACY
#define CONFIG_SYS_ATMEL_BASE		CONFIG_SYS_FLASH_BASE
#define CONFIG_SYS_ATMEL_TOTALSECT	CONFIG_SYS_MAX_FLASH_SECT
#define CONFIG_SYS_ATMEL_REGION		4
#define CONFIG_SYS_ATMEL_SECT		{1, 2, 1, 7}
#define CONFIG_SYS_ATMEL_SECTSZ		{0x4000, 0x2000, 0x8000, 0x10000}

#define CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_SECT_SIZE	(64 * 1024)
#define CONFIG_ENV_SIZE		CONFIG_ENV_SECT_SIZE

#ifdef CONFIG_T_SH7706LSR
#define CONFIG_ENV_ADDR		(SHMIN_FLASH_BASE_1 + 70000)
#else
#define CONFIG_ENV_ADDR		(CONFIG_SYS_MONITOR_BASE + CONFIG_SYS_MONITOR_LEN)
#endif

#define CONFIG_SYS_FLASH_ERASE_TOUT	120000
#define CONFIG_SYS_FLASH_WRITE_TOUT	500

/* Board Clock */
#ifdef CONFIG_T_SH7706LSR
#define CONFIG_SYS_CLK_FREQ 40000000
#else
#define CONFIG_SYS_CLK_FREQ 33333333
#endif /* CONFIG_T_SH7706LSR */
#define CONFIG_SYS_TMU_CLK_DIV	4
#define CONFIG_SYS_HZ	1000

/* Network device */
#define CONFIG_DRIVER_NE2000
#define CONFIG_DRIVER_NE2000_BASE   (0xb0000300)

#endif	/* __SHMIN_H */
