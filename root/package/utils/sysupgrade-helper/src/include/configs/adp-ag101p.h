/*
 * Copyright (C) 2011 Andes Technology Corporation
 * Shawn Lin, Andes Technology Corporation <nobuhiro@andestech.com>
 * Macpaul Lin, Andes Technology Corporation <macpaul@andestech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/arch/ag101.h>

/*
 * CPU and Board Configuration Options
 */
#define CONFIG_ADP_AG101P

#define CONFIG_USE_INTERRUPT

#define CONFIG_SKIP_LOWLEVEL_INIT

#ifndef CONFIG_SKIP_LOWLEVEL_INIT
#define CONFIG_MEM_REMAP
#endif

#ifdef CONFIG_SKIP_LOWLEVEL_INIT
#define CONFIG_SYS_TEXT_BASE	0x03200000
#else
#define CONFIG_SYS_TEXT_BASE	0x00000000
#endif

/*
 * Timer
 */

/*
 * According to the discussion in u-boot mailing list before,
 * CONFIG_SYS_HZ at 1000 is mandatory.
 */
#define CONFIG_SYS_HZ		1000
#define CONFIG_SYS_CLK_FREQ	39062500
#define VERSION_CLOCK		CONFIG_SYS_CLK_FREQ

/*
 * Use Externel CLOCK or PCLK
 */
#undef CONFIG_FTRTC010_EXTCLK

#ifndef CONFIG_FTRTC010_EXTCLK
#define CONFIG_FTRTC010_PCLK
#endif

#ifdef CONFIG_FTRTC010_EXTCLK
#define TIMER_CLOCK	32768			/* CONFIG_FTRTC010_EXTCLK */
#else
#define TIMER_CLOCK	CONFIG_SYS_HZ		/* CONFIG_FTRTC010_PCLK */
#endif

#define TIMER_LOAD_VAL	0xffffffff

/*
 * Real Time Clock
 */
#define CONFIG_RTC_FTRTC010

/*
 * Real Time Clock Divider
 * RTC_DIV_COUNT			(OSC_CLK/OSC_5MHZ)
 */
#define OSC_5MHZ			(5*1000000)
#define OSC_CLK				(4*OSC_5MHZ)
#define RTC_DIV_COUNT			(0.5)	/* Why?? */

/*
 * Serial console configuration
 */

/* FTUART is a high speed NS 16C550A compatible UART, addr: 0x99600000 */
#define CONFIG_BAUDRATE			38400
#define CONFIG_CONS_INDEX		1
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_COM1		CONFIG_FTUART010_02_BASE
#define CONFIG_SYS_NS16550_REG_SIZE	-4
#define CONFIG_SYS_NS16550_CLK		((18432000 * 20) / 25)	/* AG101P */

/*
 * Ethernet
 */
#define CONFIG_FTMAC100

#define CONFIG_BOOTDELAY	3

/*
 * SD (MMC) controller
 */
#define CONFIG_MMC
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_DOS_PARTITION
#define CONFIG_FTSDC010
#define CONFIG_FTSDC010_NUMBER		1
#define CONFIG_CMD_FAT

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_CACHE
#define CONFIG_CMD_DATE
#define CONFIG_CMD_PING

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP			/* undef to save memory */
#define CONFIG_SYS_PROMPT	"NDS32 # "	/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE	256		/* Console I/O Buffer Size */

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE	\
	(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)

/* max number of command args */
#define CONFIG_SYS_MAXARGS	16

/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE

/*
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(128 * 1024)	/* regular stack */

/*
 * Size of malloc() pool
 */
/* 512kB is suggested, (CONFIG_ENV_SIZE + 128 * 1024) was not enough */
#define CONFIG_SYS_MALLOC_LEN		(512 << 10)

/*
 * size in bytes reserved for initial data
 */
#define CONFIG_SYS_GBL_DATA_SIZE	128

/*
 * AHB Controller configuration
 */
#define CONFIG_FTAHBC020S

#ifdef CONFIG_FTAHBC020S
#include <faraday/ftahbc020s.h>

/* Address of PHYS_SDRAM_0 before memory remap is at 0x(100)00000 */
#define CONFIG_SYS_FTAHBC020S_SLAVE_BSR_BASE	0x100

/*
 * CONFIG_SYS_FTAHBC020S_SLAVE_BSR_6: this define is used in lowlevel_init.S,
 * hence we cannot use FTAHBC020S_BSR_SIZE(2048) since it will use ffs() wrote
 * in C language.
 */
#define CONFIG_SYS_FTAHBC020S_SLAVE_BSR_6 \
	(FTAHBC020S_SLAVE_BSR_BASE(CONFIG_SYS_FTAHBC020S_SLAVE_BSR_BASE) | \
					FTAHBC020S_SLAVE_BSR_SIZE(0xb))
#endif

/*
 * Watchdog
 */
#define CONFIG_FTWDT010_WATCHDOG

/*
 * PMU Power controller configuration
 */
#define CONFIG_PMU
#define CONFIG_FTPMU010_POWER

#ifdef CONFIG_FTPMU010_POWER
#include <faraday/ftpmu010.h>
#define CONFIG_SYS_FTPMU010_PDLLCR0_HCLKOUTDIS		0x0E
#define CONFIG_SYS_FTPMU010_SDRAMHTC	(FTPMU010_SDRAMHTC_EBICTRL_DCSR  | \
					 FTPMU010_SDRAMHTC_EBIDATA_DCSR  | \
					 FTPMU010_SDRAMHTC_SDRAMCS_DCSR  | \
					 FTPMU010_SDRAMHTC_SDRAMCTL_DCSR | \
					 FTPMU010_SDRAMHTC_CKE_DCSR	 | \
					 FTPMU010_SDRAMHTC_DQM_DCSR	 | \
					 FTPMU010_SDRAMHTC_SDCLK_DCSR)
#endif

/*
 * SDRAM controller configuration
 */
#define CONFIG_FTSDMC021

#ifdef CONFIG_FTSDMC021
#include <faraday/ftsdmc021.h>

#define CONFIG_SYS_FTSDMC021_TP1	(FTSDMC021_TP1_TRAS(2)	|	\
					 FTSDMC021_TP1_TRP(1)	|	\
					 FTSDMC021_TP1_TRCD(1)	|	\
					 FTSDMC021_TP1_TRF(3)	|	\
					 FTSDMC021_TP1_TWR(1)	|	\
					 FTSDMC021_TP1_TCL(2))

#define CONFIG_SYS_FTSDMC021_TP2	(FTSDMC021_TP2_INI_PREC(4) |	\
					 FTSDMC021_TP2_INI_REFT(8) |	\
					 FTSDMC021_TP2_REF_INTV(0x180))

/*
 * CONFIG_SYS_FTSDMC021_CR1: this define is used in lowlevel_init.S,
 * hence we cannot use FTSDMC021_BANK_SIZE(64) since it will use ffs() wrote in
 * C language.
 */
#define CONFIG_SYS_FTSDMC021_CR1	(FTSDMC021_CR1_DDW(2)	 |	\
					 FTSDMC021_CR1_DSZ(3)	 |	\
					 FTSDMC021_CR1_MBW(2)	 |	\
					 FTSDMC021_CR1_BNKSIZE(6))

#define CONFIG_SYS_FTSDMC021_CR2	(FTSDMC021_CR2_IPREC	 |	\
					 FTSDMC021_CR2_IREF	 |	\
					 FTSDMC021_CR2_ISMR)

#define CONFIG_SYS_FTSDMC021_BANK0_BASE	CONFIG_SYS_FTAHBC020S_SLAVE_BSR_BASE
#define CONFIG_SYS_FTSDMC021_BANK0_BSR	(FTSDMC021_BANK_ENABLE	 |	\
					 CONFIG_SYS_FTSDMC021_BANK0_BASE)

#endif

/*
 * Physical Memory Map
 */
#if defined(CONFIG_MEM_REMAP) || defined(CONFIG_SKIP_LOWLEVEL_INIT)
#define PHYS_SDRAM_0		0x00000000	/* SDRAM Bank #1 */
#if defined(CONFIG_MEM_REMAP)
#define PHYS_SDRAM_0_AT_INIT	0x10000000	/* SDRAM Bank #1 before remap*/
#endif
#else	/* !CONFIG_SKIP_LOWLEVEL_INIT && !CONFIG_MEM_REMAP */
#define PHYS_SDRAM_0		0x10000000	/* SDRAM Bank #1 */
#endif

#define CONFIG_NR_DRAM_BANKS	1		/* we have 1 bank of DRAM */
#define PHYS_SDRAM_0_SIZE	0x04000000	/* 64 MB */

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_0

#ifdef CONFIG_MEM_REMAP
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SYS_SDRAM_BASE + 0xA0000 - \
					GENERATED_GBL_DATA_SIZE)
#else
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SYS_SDRAM_BASE + 0x1000 - \
					GENERATED_GBL_DATA_SIZE)
#endif /* CONFIG_MEM_REMAP */

/*
 * Load address and memory test area should agree with
 * arch/nds32/config.mk. Be careful not to overwrite U-boot itself.
 */
#define CONFIG_SYS_LOAD_ADDR		0x300000

/* memtest works on 63 MB in DRAM */
#define CONFIG_SYS_MEMTEST_START	PHYS_SDRAM_0
#define CONFIG_SYS_MEMTEST_END		(PHYS_SDRAM_0 + 0x03F00000)

/*
 * Static memory controller configuration
 */
#define CONFIG_FTSMC020

#ifdef CONFIG_FTSMC020
#include <faraday/ftsmc020.h>

#define CONFIG_SYS_FTSMC020_CONFIGS	{			\
	{ FTSMC020_BANK0_CONFIG, FTSMC020_BANK0_TIMING, },	\
	{ FTSMC020_BANK1_CONFIG, FTSMC020_BANK1_TIMING, },	\
}

#ifndef CONFIG_SKIP_LOWLEVEL_INIT	/* FLASH is on BANK 0 */
#define FTSMC020_BANK0_LOWLV_CONFIG	(FTSMC020_BANK_ENABLE	|	\
					 FTSMC020_BANK_SIZE_32M	|	\
					 FTSMC020_BANK_MBW_32)

#define FTSMC020_BANK0_LOWLV_TIMING	(FTSMC020_TPR_RBE	|	\
					 FTSMC020_TPR_AST(1)	|	\
					 FTSMC020_TPR_CTW(1)	|	\
					 FTSMC020_TPR_ATI(1)	|	\
					 FTSMC020_TPR_AT2(1)	|	\
					 FTSMC020_TPR_WTC(1)	|	\
					 FTSMC020_TPR_AHT(1)	|	\
					 FTSMC020_TPR_TRNA(1))
#endif

/*
 * FLASH on ADP_AG101P is connected to BANK0
 * Just disalbe the other BANK to avoid detection error.
 */
#define FTSMC020_BANK0_CONFIG	(FTSMC020_BANK_ENABLE             |	\
				 FTSMC020_BANK_BASE(PHYS_FLASH_1) |	\
				 FTSMC020_BANK_SIZE_32M           |	\
				 FTSMC020_BANK_MBW_32)

#define FTSMC020_BANK0_TIMING	(FTSMC020_TPR_AST(3)   |	\
				 FTSMC020_TPR_CTW(3)   |	\
				 FTSMC020_TPR_ATI(0xf) |	\
				 FTSMC020_TPR_AT2(3)   |	\
				 FTSMC020_TPR_WTC(3)   |	\
				 FTSMC020_TPR_AHT(3)   |	\
				 FTSMC020_TPR_TRNA(0xf))

#define FTSMC020_BANK1_CONFIG	(0x00)
#define FTSMC020_BANK1_TIMING	(0x00)
#endif /* CONFIG_FTSMC020 */

/*
 * FLASH and environment organization
 */
/* use CFI framework */
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_FLASH_CFI_DRIVER

#define CONFIG_SYS_FLASH_CFI_WIDTH	FLASH_CFI_16BIT
#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE

/* support JEDEC */

/* Do not use CONFIG_FLASH_CFI_LEGACY to detect on board flash */
#ifdef CONFIG_SKIP_LOWLEVEL_INIT
#define PHYS_FLASH_1			0x80400000	/* BANK 1 */
#else	/* !CONFIG_SKIP_LOWLEVEL_INIT */
#ifdef CONFIG_MEM_REMAP
#define PHYS_FLASH_1			0x80000000	/* BANK 0 */
#else
#define PHYS_FLASH_1			0x00000000	/* BANK 0 */
#endif	/* CONFIG_MEM_REMAP */
#endif	/* CONFIG_SKIP_LOWLEVEL_INIT */

#define CONFIG_SYS_FLASH_BASE		PHYS_FLASH_1
#define CONFIG_SYS_FLASH_BANKS_LIST	{ PHYS_FLASH_1, }
#define CONFIG_SYS_MONITOR_BASE		PHYS_FLASH_1

#define CONFIG_SYS_FLASH_ERASE_TOUT	120000	/* TO for Flash Erase (ms) */
#define CONFIG_SYS_FLASH_WRITE_TOUT	500	/* TO for Flash Write (ms) */

/* max number of memory banks */
/*
 * There are 4 banks supported for this Controller,
 * but we have only 1 bank connected to flash on board
 */
#define CONFIG_SYS_MAX_FLASH_BANKS	1

/* max number of sectors on one chip */
#define CONFIG_FLASH_SECTOR_SIZE	(0x10000*2*2)
#define CONFIG_ENV_SECT_SIZE		CONFIG_FLASH_SECTOR_SIZE
#define CONFIG_SYS_MAX_FLASH_SECT	128

/* environments */
#define CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_ADDR			(CONFIG_SYS_MONITOR_BASE + 0x140000)
#define CONFIG_ENV_SIZE			8192
#define CONFIG_ENV_OVERWRITE

#endif	/* __CONFIG_H */
