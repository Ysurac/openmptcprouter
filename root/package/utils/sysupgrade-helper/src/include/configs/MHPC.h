/*
 * (C) Copyright 2001
 * Frank Gottschling, ELTEC Elektronik AG, fgottschling@eltec.de
 *
 * (C) Copyright 2001
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * Configuation settings for the miniHiPerCam.
 *
 * -----------------------------------------------------------------
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
#define CONFIG_MPC823		1	/* This is a MPC823 CPU		*/
#define CONFIG_MHPC		1	/* on a miniHiPerCam		*/
#define CONFIG_BOARD_EARLY_INIT_F 1	/* do special hardware init.	*/
#define CONFIG_MISC_INIT_R	1

#define	CONFIG_SYS_TEXT_BASE	0xfe000000

#define CONFIG_8xx_GCLK_FREQ	MPC8XX_SPEED
#undef	CONFIG_8xx_CONS_SMC1
#define CONFIG_8xx_CONS_SMC2	1	/* Console is on SMC2		*/
#undef	CONFIG_8xx_CONS_NONE
#define CONFIG_BAUDRATE		9600
#define CONFIG_BOOTDELAY	5	/* autoboot after 5 seconds	*/

#define CONFIG_CLOCKS_IN_MHZ	1	/* clocks passsed to Linux in MHz */

#define CONFIG_ENV_OVERWRITE	1
#define CONFIG_ETHADDR		00:00:5b:ee:de:ad

#undef	CONFIG_BOOTARGS
#define CONFIG_BOOTCOMMAND	\
	"bootp;"								\
	"setenv bootargs root=/dev/nfs rw nfsroot=${serverip}:${rootpath} "	\
	"ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:${hostname}::off;"	\
	"bootm"

#define CONFIG_LOADS_ECHO	1	/* echo on for serial download	*/
#undef	CONFIG_SYS_LOADS_BAUD_CHANGE		/* don't allow baudrate change	*/

#undef	CONFIG_WATCHDOG			/* watchdog disabled		*/
#define CONFIG_RTC_MPC8xx		/* use internal RTC of MPC8xx	*/

#undef	CONFIG_UCODE_PATCH

/* enable I2C and select the hardware/software driver */
#undef	CONFIG_HARD_I2C			/* I2C with hardware support	*/
#define CONFIG_SOFT_I2C		1	/* I2C bit-banged		*/
/*
 * Software (bit-bang) I2C driver configuration
 */
#define PB_SCL		0x00000020	/* PB 26 */
#define PB_SDA		0x00000010	/* PB 27 */

#define I2C_INIT	(immr->im_cpm.cp_pbdir |=  PB_SCL)
#define I2C_ACTIVE	(immr->im_cpm.cp_pbdir |=  PB_SDA)
#define I2C_TRISTATE	(immr->im_cpm.cp_pbdir &= ~PB_SDA)
#define I2C_READ	((immr->im_cpm.cp_pbdat & PB_SDA) != 0)
#define I2C_SDA(bit)	if(bit) immr->im_cpm.cp_pbdat |=  PB_SDA; \
			else	immr->im_cpm.cp_pbdat &= ~PB_SDA
#define I2C_SCL(bit)	if(bit) immr->im_cpm.cp_pbdat |=  PB_SCL; \
			else	immr->im_cpm.cp_pbdat &= ~PB_SCL
#define I2C_DELAY	udelay(5)	/* 1/4 I2C clock duration */

#define CONFIG_SYS_I2C_SPEED			50000
#define CONFIG_SYS_I2C_SLAVE			0xFE
#define CONFIG_SYS_I2C_EEPROM_ADDR		0x50	/* EEPROM X24C04		*/
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN		1	/* bytes of address		*/
/* mask of address bits that overflow into the "EEPROM chip address"	*/
#define CONFIG_SYS_I2C_EEPROM_ADDR_OVERFLOW	0x07
#define CONFIG_SYS_EEPROM_PAGE_WRITE_BITS	3
#define CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS	10

#define LCD_VIDEO_ADDR		(SDRAM_MAX_SIZE-SDRAM_RES_SIZE)
#define LCD_VIDEO_SIZE		SDRAM_RES_SIZE	/* 2MB */
#define LCD_VIDEO_COLS		640
#define LCD_VIDEO_ROWS		480
#define LCD_VIDEO_FG		255
#define LCD_VIDEO_BG		0

#undef	CONFIG_VIDEO				/* test only ! s.a devices.c and 8xx */
#define CONFIG_CFB_CONSOLE			/* framebuffer console with std input */
#define CONFIG_VIDEO_LOGO

#define VIDEO_KBD_INIT_FCT	0		/* no KBD dev on MHPC - use serial */
#define VIDEO_TSTC_FCT		serial_tstc
#define VIDEO_GETC_FCT		serial_getc

#define CONFIG_BR0_WORKAROUND	1


/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_DATE
#define CONFIG_CMD_EEPROM
#define CONFIG_CMD_ELF
#define CONFIG_CMD_I2C
#define CONFIG_CMD_JFFS2
#define CONFIG_CMD_REGINFO


/*
 * BOOTP options
 */
#define CONFIG_BOOTP_SUBNETMASK
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_BOOTFILESIZE


/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP			/* undef to save memory		*/
#define CONFIG_SYS_PROMPT	"=> "		/* Monitor Command Prompt	*/
#if defined(CONFIG_CMD_KGDB)
#define CONFIG_SYS_CBSIZE	1024		/* Console I/O Buffer Size	*/
#else
#define CONFIG_SYS_CBSIZE	256		/* Console I/O Buffer Size	*/
#endif
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16) /* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	16		/* max number of command args	*/
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size	*/

#define CONFIG_SYS_MEMTEST_START	0x0400000	/* memtest works on	*/
#define CONFIG_SYS_MEMTEST_END		0x0C00000	/* 4 ... 12 MB in DRAM	*/

#define CONFIG_SYS_LOAD_ADDR		0x300000	/* default load address */

#define CONFIG_SYS_HZ			1000		/* decrementer freq: 1 ms ticks */

/*
 * Low Level Configuration Settings
 * (address mappings, register initial values, etc.)
 * You should know what you are doing if you make changes here.
 */

/*-----------------------------------------------------------------------
 * Physical memory map
 */
#define CONFIG_SYS_IMMR		0xFFF00000 /* Internal Memory Mapped Register*/

/*-----------------------------------------------------------------------
 * Definitions for initial stack pointer and data area (in DPRAM)
 */
#define CONFIG_SYS_INIT_RAM_ADDR	CONFIG_SYS_IMMR
#define CONFIG_SYS_INIT_RAM_SIZE	0x2F00	/* Size of used area in DPRAM	*/
#define CONFIG_SYS_GBL_DATA_OFFSET	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

/*-----------------------------------------------------------------------
 * Start addresses for the final memory configuration
 * (Set up by the startup code)
 * Please note that CONFIG_SYS_SDRAM_BASE _must_ start at 0
 */
#define CONFIG_SYS_SDRAM_BASE		0x00000000
#define CONFIG_SYS_FLASH_BASE		0xfe000000

#define CONFIG_SYS_MONITOR_LEN		0x40000		/* Reserve 256 kB for Monitor	*/
#undef	CONFIG_SYS_MONITOR_BASE		    /* to run U-Boot from RAM */
#define CONFIG_SYS_MONITOR_BASE	CONFIG_SYS_FLASH_BASE
#define CONFIG_SYS_MALLOC_LEN		(128 << 10)	/* Reserve 128 kB for malloc()	*/

/*
 * JFFS2 partitions
 *
 */
/* No command line, one static partition, whole device */
#undef CONFIG_CMD_MTDPARTS
#define CONFIG_JFFS2_DEV		"nor0"
#define CONFIG_JFFS2_PART_SIZE		0xFFFFFFFF
#define CONFIG_JFFS2_PART_OFFSET	0x00000000

/* mtdparts command line support */
/* Note: fake mtd_id used, no linux mtd map file */
/*
#define CONFIG_CMD_MTDPARTS
#define MTDIDS_DEFAULT		"nor0=mhpc-0"
#define MTDPARTS_DEFAULT	"mtdparts=mhpc-0:-(jffs2)"
*/

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTMAPSZ		(8 << 20)	/* Initial Memory map- for Linux	*/

/*-----------------------------------------------------------------------
 * FLASH organization
 */
#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks		*/
#define CONFIG_SYS_MAX_FLASH_SECT	64	/* max number of sectors on one chip	*/

#define CONFIG_SYS_FLASH_ERASE_TOUT	120000	/* Timeout for Flash Erase (in ms)	*/
#define CONFIG_SYS_FLASH_WRITE_TOUT	500	/* Timeout for Flash Write (in ms)	*/
#define CONFIG_ENV_IS_IN_FLASH	1
#define CONFIG_ENV_OFFSET		CONFIG_SYS_MONITOR_LEN /* Offset of Environment */
#define CONFIG_ENV_SIZE		0x20000 /* Total Size of Environment	*/

/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CONFIG_SYS_CACHELINE_SIZE	16	/* For all MPC8xx CPUs			*/
#if defined(CONFIG_CMD_KGDB)
#define CONFIG_SYS_CACHELINE_SHIFT	4	/* log base 2 of the above value	*/
#endif

/*-----------------------------------------------------------------------
 * SYPCR - System Protection Control				11-9
 * SYPCR can only be written once after reset!
 *-----------------------------------------------------------------------
 * Software & Bus Monitor Timer max, Bus Monitor enable, SW Watchdog freeze
 */
#if defined(CONFIG_WATCHDOG)
#define CONFIG_SYS_SYPCR	(SYPCR_SWTC | SYPCR_BMT | SYPCR_BME | SYPCR_SWF | \
			 SYPCR_SWE  | SYPCR_SWRI| SYPCR_SWP)
#else
#define CONFIG_SYS_SYPCR	(SYPCR_SWTC | SYPCR_BMT | SYPCR_BME | SYPCR_SWF | \
			 SYPCR_SWP)
#endif

/*-----------------------------------------------------------------------
 * SIUMCR - SIU Module Configuration				11-6
 *-----------------------------------------------------------------------
 * PCMCIA config., multi-function pin tri-state
 */
#define CONFIG_SYS_SIUMCR	(SIUMCR_SEME)

/*-----------------------------------------------------------------------
 * TBSCR - Time Base Status and Control				11-26
 *-----------------------------------------------------------------------
 * Clear Reference Interrupt Status, Timebase freezing enabled
 */
#define CONFIG_SYS_TBSCR	(TBSCR_REFA | TBSCR_REFB | TBSCR_TBE)

/*-----------------------------------------------------------------------
 * PISCR - Periodic Interrupt Status and Control		11-31
 *-----------------------------------------------------------------------
 * Clear Periodic Interrupt Status, Interrupt Timer freezing enabled
 */
#define CONFIG_SYS_PISCR	(PISCR_PS | PISCR_PITF | PISCR_PTE)

/*-----------------------------------------------------------------------
 * RTCSC - Real-Time Clock Status and Control Register		12-18
 *-----------------------------------------------------------------------
 */
#define CONFIG_SYS_RTCSC	(RTCSC_SEC | RTCSC_ALR | RTCSC_RTF| RTCSC_RTE)

/*-----------------------------------------------------------------------
 * PLPRCR - PLL, Low-Power, and Reset Control Register		15-30
 *-----------------------------------------------------------------------
 * Reset PLL lock status sticky bit, timer expired status bit and timer
 * interrupt status bit - leave PLL multiplication factor unchanged !
 */
#define MPC8XX_SPEED	50000000L
#define MPC8XX_XIN	5000000L      /* ref clk */
#define MPC8XX_FACT	(MPC8XX_SPEED/MPC8XX_XIN)
#define CONFIG_SYS_PLPRCR	(((MPC8XX_FACT-1) << PLPRCR_MF_SHIFT) | \
			PLPRCR_SPLSS | PLPRCR_TEXPS | PLPRCR_TMIST)

/*-----------------------------------------------------------------------
 * SCCR - System Clock and reset Control Register		15-27
 *-----------------------------------------------------------------------
 * Set clock output, timebase and RTC source and divider,
 * power management and some other internal clocks
 */

#define SCCR_MASK	(SCCR_RTDIV | SCCR_RTSEL)     /* SCCR_EBDF11 */
#define CONFIG_SYS_SCCR	(SCCR_TBS | SCCR_DFLCD001)


/*-----------------------------------------------------------------------
 * MAMR settings for SDRAM	- 16-14
 * => 0xC080200F
 *-----------------------------------------------------------------------
 * periodic timer for refresh
 */
#define CONFIG_SYS_MAMR_PTA	0xC0
#define CONFIG_SYS_MAMR	((CONFIG_SYS_MAMR_PTA << MAMR_PTA_SHIFT) | MAMR_PTAE | MAMR_G0CLA_A11 | MAMR_TLFA_MSK)

/*
 * BR0 and OR0 (FLASH) used to re-map FLASH
 */

/* allow for max 8 MB of Flash */
#define FLASH_BASE		0xFE000000	/* FLASH bank #0*/
#define FLASH_BASE0_PRELIM	0xFE000000	/* FLASH bank #0*/
#define CONFIG_SYS_REMAP_OR_AM		0xFF800000	/* OR addr mask */
#define CONFIG_SYS_PRELIM_OR_AM	0xFF800000	/* OR addr mask */

#define CONFIG_SYS_OR_TIMING_FLASH	(OR_CSNT_SAM | OR_BI | OR_SCY_8_CLK) /* (OR_CSNT_SAM | OR_ACS_DIV2 | OR_BI | OR_SCY_6_CLK)*/

#define CONFIG_SYS_OR0_REMAP	(CONFIG_SYS_REMAP_OR_AM  | CONFIG_SYS_OR_TIMING_FLASH)
#define CONFIG_SYS_OR0_PRELIM	(CONFIG_SYS_PRELIM_OR_AM | CONFIG_SYS_OR_TIMING_FLASH)
#define CONFIG_SYS_BR0_PRELIM	((FLASH_BASE & BR_BA_MSK) | BR_PS_16 | BR_MS_GPCM | BR_V )

/*
 * BR1 and OR1 (SDRAM)
 */
#define SDRAM_BASE1_PRELIM	0x00000000	/* SDRAM bank #0	*/
#define SDRAM_MAX_SIZE		0x01000000	/* max 16 MB		*/
#define SDRAM_RES_SIZE		0x00200000	/* 2 MB for framebuffer */

/* SDRAM timing: drive GPL5 high on first cycle */
#define CONFIG_SYS_OR_TIMING_SDRAM	(OR_G5LS)

#define CONFIG_SYS_OR1_PRELIM	((~(SDRAM_MAX_SIZE)+1)| CONFIG_SYS_OR_TIMING_SDRAM )
#define CONFIG_SYS_BR1_PRELIM	((SDRAM_BASE1_PRELIM & BR_BA_MSK) | BR_MS_UPMA | BR_V )

/*
 * BR2/OR2 - DIMM
 */
#define CONFIG_SYS_OR2		(OR_ACS_DIV4)
#define CONFIG_SYS_BR2		(BR_MS_UPMA)

/*
 * BR3/OR3 - DIMM
 */
#define CONFIG_SYS_OR3		(OR_ACS_DIV4)
#define CONFIG_SYS_BR3		(BR_MS_UPMA)

/*
 * BR4/OR4
 */
#define CONFIG_SYS_OR4		0
#define CONFIG_SYS_BR4		0

/*
 * BR5/OR5
 */
#define CONFIG_SYS_OR5		0
#define CONFIG_SYS_BR5		0

/*
 * BR6/OR6
 */
#define CONFIG_SYS_OR6		0
#define CONFIG_SYS_BR6		0

/*
 * BR7/OR7
 */
#define CONFIG_SYS_OR7		0
#define CONFIG_SYS_BR7		0


/*-----------------------------------------------------------------------
 * Debug Entry Mode
 *-----------------------------------------------------------------------
 *
 */
#define CONFIG_SYS_DER 0

#endif	/* __CONFIG_H */
