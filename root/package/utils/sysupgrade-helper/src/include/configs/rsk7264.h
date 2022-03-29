/*
 * Configuation settings for the Renesas RSK2+SH7264 board
 *
 * Copyright (C) 2011 Renesas Electronics Europe Ltd.
 * Copyright (C) 2008 Nobuhiro Iwamatsu
 * Copyright (C) 2008 Renesas Solutions Corp.
 *
 * This file is released under the terms of GPL v2 and any later version.
 * See the file COPYING in the root directory of the source tree for details.
 */

#ifndef __RSK7264_H
#define __RSK7264_H

#undef DEBUG
#define CONFIG_SH		1
#define CONFIG_SH2		1
#define CONFIG_SH2A		1
#define CONFIG_CPU_SH7264	1
#define CONFIG_RSK7264		1

#ifndef _CONFIG_CMD_DEFAULT_H
# include <config_cmd_default.h>
#endif

#define CONFIG_BAUDRATE		115200
#define CONFIG_BOOTARGS		"console=ttySC3,115200"
#define CONFIG_BOOTDELAY	3
#define CONFIG_SYS_BAUDRATE_TABLE	{ CONFIG_BAUDRATE }

#define CONFIG_SYS_LONGHELP	1	/* undef to save memory	*/
#define CONFIG_SYS_PROMPT	"=> "	/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE	256	/* Buffer size for input from the Console */
#define CONFIG_SYS_PBSIZE	256	/* Buffer size for Console output */
#define CONFIG_SYS_MAXARGS	16	/* max args accepted for monitor commands */

/* Serial */
#define CONFIG_SCIF_CONSOLE	1
#define CONFIG_CONS_SCIF3	1

/* Memory */
/* u-boot relocated to top 256KB of ram */
#define CONFIG_SYS_TEXT_BASE		0x0CFC0000
#define CONFIG_SYS_SDRAM_BASE		0x0C000000
#define CONFIG_SYS_SDRAM_SIZE		(64 * 1024 * 1024)

#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_TEXT_BASE - 0x100000)
#define CONFIG_SYS_MALLOC_LEN		(256 * 1024)
#define CONFIG_SYS_MONITOR_LEN		(128 * 1024)
#define CONFIG_SYS_LOAD_ADDR		(CONFIG_SYS_SDRAM_BASE + 4*1024*1024)

/* Flash */
#define CONFIG_FLASH_CFI_DRIVER
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_SYS_FLASH_CFI_WIDTH 	FLASH_CFI_16BIT
#define CONFIG_SYS_FLASH_BASE		0x20000000 /* Non-cached */
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_MAX_FLASH_SECT	512

#define CONFIG_ENV_IS_IN_FLASH	1
#define CONFIG_ENV_OFFSET	(128 * 1024)
#define CONFIG_ENV_ADDR		(CONFIG_SYS_FLASH_BASE + CONFIG_ENV_OFFSET)
#define CONFIG_ENV_SECT_SIZE	(128 * 1024)
#define CONFIG_ENV_SIZE		CONFIG_ENV_SECT_SIZE

/* Board Clock */
#define CONFIG_SYS_CLK_FREQ	36000000
#define CMT_CLK_DIVIDER		32	/* 8 (default), 32, 128 or 512 */
#define CONFIG_SYS_HZ		(CONFIG_SYS_CLK_FREQ / CMT_CLK_DIVIDER)

/* Network interface */
#define CONFIG_SMC911X
#define CONFIG_SMC911X_16_BIT
#define CONFIG_SMC911X_BASE	0x28000000

#endif	/* __RSK7264_H */
