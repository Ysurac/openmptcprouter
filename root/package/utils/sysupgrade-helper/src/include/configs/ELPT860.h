/*
**=====================================================================
**
** Copyright (C) 2000, 2001, 2002, 2003
** The LEOX team <team@leox.org>, http://www.leox.org
**
** LEOX.org is about the development of free hardware and software resources
**   for system on chip.
**
** Description: U-Boot port on the LEOX's ELPT860 CPU board
** ~~~~~~~~~~~
**
**=====================================================================
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License as
** published by the Free Software Foundation; either version 2 of
** the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston,
** MA 02111-1307 USA
**
**=====================================================================
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

#define CONFIG_MPC860		1	/* It's a MPC860, in fact a 860T CPU */
#define CONFIG_MPC860T		1
#define CONFIG_ELPT860		1	/* ...on a LEOX's ELPT860 CPU board */

#define CONFIG_SYS_TEXT_BASE	0x02000000

#define CONFIG_8xx_CONS_SMC1	1	/* Console is on SMC1		    */
#undef	  CONFIG_8xx_CONS_SMC2
#undef	  CONFIG_8xx_CONS_NONE

#define CONFIG_CLOCKS_IN_MHZ	1  /* Clock passed to Linux (<2.4.5) in MHz */
#define CONFIG_8xx_GCLK_FREQ	50000000       /* MPC860T runs at 50MHz */

#define CONFIG_BOOTDELAY	5	/* autoboot after 5 seconds	*/

#define CONFIG_BOARD_EARLY_INIT_F 1	/* Call board_early_init_f	*/
#define CONFIG_RESET_PHY_R	1	/* Call reset_phy()		*/

/* BOOT arguments */
#define CONFIG_PREBOOT							   \
     "echo;"								   \
     "echo Type \"run nfsboot\" to mount root filesystem over NFS;"	   \
     "echo"

#undef	  CONFIG_BOOTARGS

#define CONFIG_EXTRA_ENV_SETTINGS					\
    "ramargs=setenv bootargs root=/dev/ram rw\0"			\
    "rootargs=setenv rootpath /tftp/${ipaddr}\0"			\
    "nfsargs=setenv bootargs root=/dev/nfs rw "				\
	"nfsroot=${serverip}:${rootpath}\0"				\
    "addip=setenv bootargs ${bootargs} "				\
	"ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}"		\
	":${hostname}:eth0:off panic=1\0"				\
    "ramboot=tftp 400000 /home/paugaml/pMulti;"				\
	"run ramargs;bootm\0"						\
    "nfsboot=tftp 400000 /home/paugaml/uImage;"				\
	"run rootargs;run nfsargs;run addip;bootm\0"			\
    ""
#define CONFIG_BOOTCOMMAND	"run ramboot"

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_SUBNETMASK
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_BOOTFILESIZE


#undef	  CONFIG_WATCHDOG		/* watchdog disabled		*/
#undef	  CONFIG_CAN_DRIVER		/* CAN Driver support disabled	*/
#undef	  CONFIG_RTC_MPC8xx		/* internal RTC MPC8xx unused	*/
#define CONFIG_RTC_DS164x	1	/* RTC is a Dallas DS1646	*/

#define CONFIG_LOADS_ECHO	1	/* echo on for serial download	*/
#undef	  CONFIG_SYS_LOADS_BAUD_CHANGE		/* don't allow baudrate change	*/


/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_DATE


/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP			/* undef to save memory		*/
#define CONFIG_SYS_PROMPT     "LEOX_elpt860: " /* Monitor Command Prompt	*/

#if defined(CONFIG_CMD_KGDB)
#  define CONFIG_SYS_CBSIZE	1024		/* Console I/O Buffer Size	*/
#else
#  define CONFIG_SYS_CBSIZE	 256		/* Console I/O Buffer Size	*/
#endif

#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16) /* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	  16		/* max number of command args	*/
#define CONFIG_SYS_BARGSIZE	  CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size	*/

#define CONFIG_SYS_MEMTEST_START	0x00400000	/* memtest works on	*/
#define CONFIG_SYS_MEMTEST_END		0x00C00000	/* 4 ... 12 MB in DRAM	*/

#define CONFIG_SYS_LOAD_ADDR		0x00100000	/* default load address */

#define CONFIG_SYS_HZ		1000		/* decrementer freq: 1 ms ticks */

/*
 * Environment Variables and Storages
 */
#define CONFIG_ENV_OVERWRITE	1  /* Allow Overwrite of serial# & ethaddr */

#undef	  CONFIG_ENV_IS_IN_NVRAM	       /* Environment is in NVRAM	*/
#undef	  CONFIG_ENV_IS_IN_EEPROM	       /* Environment is in I2C EEPROM	*/
#define CONFIG_ENV_IS_IN_FLASH	1      /* Environment is in FLASH	*/

#define CONFIG_BAUDRATE		9600   /* console baudrate = 9600 bps	*/

#define CONFIG_ETHADDR		00:01:77:00:60:40
#define CONFIG_IPADDR		192.168.0.30
#define CONFIG_NETMASK		255.255.255.0

#define CONFIG_SERVERIP		192.168.0.1
#define CONFIG_GATEWAYIP	192.168.0.1

/*
 * Low Level Configuration Settings
 * (address mappings, register initial values, etc.)
 * You should know what you are doing if you make changes here.
 */

/*-----------------------------------------------------------------------
 * Internal Memory Mapped Register
 */
#define CONFIG_SYS_IMMR		0xFF000000

/*-----------------------------------------------------------------------
 * Definitions for initial stack pointer and data area (in DPRAM)
 */
#define CONFIG_SYS_INIT_RAM_ADDR	CONFIG_SYS_IMMR
#define CONFIG_SYS_INIT_RAM_SIZE	0x2F00	/* Size of used area in DPRAM	*/
#define CONFIG_SYS_GBL_DATA_OFFSET    (CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

/*-----------------------------------------------------------------------
 * Start addresses for the final memory configuration
 * (Set up by the startup code)
 * Please note that CONFIG_SYS_SDRAM_BASE _must_ start at 0
 */
#define CONFIG_SYS_SDRAM_BASE		0x00000000
#define CONFIG_SYS_FLASH_BASE		0x02000000
#define CONFIG_SYS_NVRAM_BASE		0x03000000

#if defined(CONFIG_ENV_IS_IN_FLASH)
#  if defined(DEBUG)
#    define CONFIG_SYS_MONITOR_LEN	(320 << 10)  /* Reserve 320 kB for Monitor  */
#  else
#    define CONFIG_SYS_MONITOR_LEN	(256 << 10)  /* Reserve 256 kB for Monitor  */
#  endif
#else
#  if defined(DEBUG)
#    define CONFIG_SYS_MONITOR_LEN	(256 << 10)  /* Reserve 256 kB for Monitor  */
#  else
#    define CONFIG_SYS_MONITOR_LEN	(192 << 10)  /* Reserve 192 kB for Monitor  */
#  endif
#endif

#define CONFIG_SYS_MONITOR_BASE	CONFIG_SYS_FLASH_BASE
#define CONFIG_SYS_MALLOC_LEN		(128 << 10)  /* Reserve 128 kB for malloc() */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTMAPSZ		(8 << 20)    /* Initial Memory map for Linux */

/*-----------------------------------------------------------------------
 * FLASH organization
 */
#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks	     */
#define CONFIG_SYS_MAX_FLASH_SECT	8	/* max number of sectors on one chip */

#define CONFIG_SYS_FLASH_ERASE_TOUT	120000	/* Timeout for Flash Erase (in ms)   */
#define CONFIG_SYS_FLASH_WRITE_TOUT	500	/* Timeout for Flash Write (in ms)   */

#if defined(CONFIG_ENV_IS_IN_FLASH)
#  define CONFIG_ENV_OFFSET	0x10000 /* Offset   of Environment Sector    */
#  define CONFIG_ENV_SIZE		0x10000 /* Total Size of Environment Sector  */
#endif

/*-----------------------------------------------------------------------
 * NVRAM organization
 */
#define CONFIG_SYS_NVRAM_BASE_ADDR	CONFIG_SYS_NVRAM_BASE /* Base address of NVRAM area */
#define CONFIG_SYS_NVRAM_SIZE		((128*1024)-8) /* clock regs resident in the */
					       /*   8 top NVRAM locations    */

#if defined(CONFIG_ENV_IS_IN_NVRAM)
#  define CONFIG_ENV_ADDR		CONFIG_SYS_NVRAM_BASE /* Base address of NVRAM area */
#  define CONFIG_ENV_SIZE		0x4000	/* Total Size of Environment Sector  */
#endif

/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CONFIG_SYS_CACHELINE_SIZE	16	/* For all MPC8xx CPUs		     */

#if defined(CONFIG_CMD_KGDB)
#  define CONFIG_SYS_CACHELINE_SHIFT	 4	/* log base 2 of the above value     */
#endif

/*-----------------------------------------------------------------------
 * SYPCR - System Protection Control				11-9
 * SYPCR can only be written once after reset!
 *-----------------------------------------------------------------------
 * Software & Bus Monitor Timer max, Bus Monitor enable, SW Watchdog freeze
 */
#if defined(CONFIG_WATCHDOG)
#  define CONFIG_SYS_SYPCR	(SYPCR_SWTC | SYPCR_BMT	 | SYPCR_BME | SYPCR_SWF | \
			 SYPCR_SWE  | SYPCR_SWRI | SYPCR_SWP)
#else
#  define CONFIG_SYS_SYPCR	(SYPCR_SWTC | SYPCR_BMT	 | SYPCR_BME | SYPCR_SWF | \
						   SYPCR_SWP)
#endif

/*-----------------------------------------------------------------------
 * SUMCR - SIU Module Configuration				11-6
 *-----------------------------------------------------------------------
 * PCMCIA config., multi-function pin tri-state
 */
#define CONFIG_SYS_SIUMCR	(SIUMCR_DBGC11)

/*-----------------------------------------------------------------------
 * TBSCR - Time Base Status and Control				11-26
 *-----------------------------------------------------------------------
 * Clear Reference Interrupt Status, Timebase freezing enabled
 */
#define CONFIG_SYS_TBSCR	(TBSCR_REFA | TBSCR_REFB | TBSCR_TBF)

/*-----------------------------------------------------------------------
 * RTCSC - Real-Time Clock Status and Control Register		11-27
 *-----------------------------------------------------------------------
 * Once-per-Second Interrupt, Alarm Interrupt, RTC freezing enabled, RTC
 *  enabled
 */
#define CONFIG_SYS_RTCSC	(RTCSC_SEC | RTCSC_ALR | RTCSC_RTF| RTCSC_RTE)

/*-----------------------------------------------------------------------
 * PISCR - Periodic Interrupt Status and Control		11-31
 *-----------------------------------------------------------------------
 * Clear Periodic Interrupt Status, Interrupt Timer freezing enabled
 */
#define CONFIG_SYS_PISCR	(PISCR_PS | PISCR_PITF)

/*-----------------------------------------------------------------------
 * PLPRCR - PLL, Low-Power, and Reset Control Register		15-30
 *-----------------------------------------------------------------------
 * Reset PLL lock status sticky bit, timer expired status bit and timer
 * interrupt status bit - leave PLL multiplication factor unchanged !
 */
#define CONFIG_SYS_PLPRCR	(PLPRCR_SPLSS | PLPRCR_TEXPS | PLPRCR_TMIST)

/*-----------------------------------------------------------------------
 * SCCR - System Clock and reset Control Register		15-27
 *-----------------------------------------------------------------------
 * Set clock output, timebase and RTC source and divider,
 * power management and some other internal clocks
 */
#define SCCR_MASK	 SCCR_EBDF11
#define CONFIG_SYS_SCCR	(SCCR_TBS     | \
			 SCCR_COM00   | SCCR_DFSYNC00 | SCCR_DFBRG00  | \
			 SCCR_DFNL000 | SCCR_DFNH000  | SCCR_DFLCD000 | \
			 SCCR_DFALCD00)

/*-----------------------------------------------------------------------
 * Chip Selects + SDRAM timings + Memory Periodic Timer Prescaler
 *-----------------------------------------------------------------------
 *
 */
#ifdef DEBUG
#  define CONFIG_SYS_DER		0xFFE7400F	/* Debug Enable Register */
#else
#  define CONFIG_SYS_DER		0
#endif

/*
 * Init Memory Controller:
 * ~~~~~~~~~~~~~~~~~~~~~~
 *
 * BR0 and OR0 (FLASH)
 */

#define FLASH_BASE0_PRELIM	CONFIG_SYS_FLASH_BASE	/* FLASH bank #0	  */

/* used to re-map FLASH both when starting from SRAM or FLASH:
 * restrict access enough to keep SRAM working (if any)
 * but not too much to meddle with FLASH accesses
 */
#define CONFIG_SYS_PRELIM_OR_AM	0xFF000000	/* 16 MB between each CSx */

/* FLASH timing: ACS = 11, TRLX = 0, CSNT = 0, SCY = 8, EHTR = 0	  */
#define CONFIG_SYS_OR_TIMING_FLASH	(OR_ACS_DIV2 | OR_BI | OR_SCY_8_CLK)

#define CONFIG_SYS_OR0_PRELIM	(CONFIG_SYS_PRELIM_OR_AM | CONFIG_SYS_OR_TIMING_FLASH)
#define CONFIG_SYS_BR0_PRELIM	((FLASH_BASE0_PRELIM & BR_BA_MSK) | BR_PS_8 | BR_V )

/*
 * BR1 and OR1 (SDRAM)
 *
 */
#define SDRAM_BASE1_PRELIM	CONFIG_SYS_SDRAM_BASE	/* SDRAM bank #0	*/
#define SDRAM_MAX_SIZE		0x02000000	/* 32 MB MAX for CS1	*/

/* SDRAM timing:							*/
#define CONFIG_SYS_OR_TIMING_SDRAM	0x00000000

#define CONFIG_SYS_OR1_PRELIM	((2 * CONFIG_SYS_PRELIM_OR_AM) | CONFIG_SYS_OR_TIMING_SDRAM )
#define CONFIG_SYS_BR1_PRELIM	((SDRAM_BASE1_PRELIM & BR_BA_MSK) | BR_MS_UPMA | BR_V )

/*
 * BR2 and OR2 (NVRAM)
 *
 */
#define NVRAM_BASE1_PRELIM	CONFIG_SYS_NVRAM_BASE	/* NVRAM bank #0	*/
#define NVRAM_MAX_SIZE		0x00020000	/* 128 KB MAX for CS2	*/

#define CONFIG_SYS_OR2_PRELIM		0xFFF80160
#define CONFIG_SYS_BR2_PRELIM	((NVRAM_BASE1_PRELIM & BR_BA_MSK) | BR_PS_8 | BR_V )

/*
 * Memory Periodic Timer Prescaler
 */

/* periodic timer for refresh */
#define CONFIG_SYS_MAMR_PTA		97     /* start with divider for 100 MHz */

/* refresh rate 15.6 us (= 64 ms / 4K = 62.4 / quad bursts) for <= 128 MBit */
#define CONFIG_SYS_MPTPR_2BK_4K	MPTPR_PTP_DIV16	  /* setting for 2 banks */
#define CONFIG_SYS_MPTPR_1BK_4K	MPTPR_PTP_DIV32	  /* setting for 1 bank	 */

/* refresh rate 7.8 us (= 64 ms / 8K = 31.2 / quad bursts) for 256 MBit	 */
#define CONFIG_SYS_MPTPR_2BK_8K	MPTPR_PTP_DIV8	  /* setting for 2 banks */
#define CONFIG_SYS_MPTPR_1BK_8K	MPTPR_PTP_DIV16	  /* setting for 1 bank	 */

/*
 * MAMR settings for SDRAM
 */

/* 8 column SDRAM */
#define CONFIG_SYS_MAMR_8COL	((CONFIG_SYS_MAMR_PTA << MAMR_PTA_SHIFT)  | MAMR_PTAE	    | \
			 MAMR_AMA_TYPE_0 | MAMR_DSA_1_CYCL | MAMR_G0CLA_A11 | \
			 MAMR_RLFA_1X	 | MAMR_WLFA_1X	   | MAMR_TLFA_4X)
/* 9 column SDRAM */
#define CONFIG_SYS_MAMR_9COL	((CONFIG_SYS_MAMR_PTA << MAMR_PTA_SHIFT)  | MAMR_PTAE	    | \
			 MAMR_AMA_TYPE_1 | MAMR_DSA_1_CYCL | MAMR_G0CLA_A10 | \
			 MAMR_RLFA_1X	 | MAMR_WLFA_1X	   | MAMR_TLFA_4X)

#endif	/* __CONFIG_H */
