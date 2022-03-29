/*
 * (C) Copyright 2000
 * Murray Jensen <Murray.Jensen@cmst.csiro.au>
 *
 * (C) Copyright 2000
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * Configuation settings for the R&S Protocol Board board.
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

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */

#define CONFIG_MPC8260		1	/* This is an MPC8260 CPU	*/
#define CONFIG_RSD_PROTO	1	/* on a R&S Protocol Board      */
#define CONFIG_CPM2		1	/* Has a CPM2 */

#define	CONFIG_SYS_TEXT_BASE	0xff000000
#define	CONFIG_SYS_LDSCRIPT	"board/rsdproto/u-boot.lds"

#define	CONFIG_MISC_INIT_F	1	/* Use misc_init_f()		*/

/*
 * select serial console configuration
 *
 * if either CONFIG_CONS_ON_SMC or CONFIG_CONS_ON_SCC is selected, then
 * CONFIG_CONS_INDEX must be set to the channel number (1-2 for SMC, 1-4
 * for SCC).
 *
 * if CONFIG_CONS_NONE is defined, then the serial console routines must
 * defined elsewhere.
 */
#undef	CONFIG_CONS_ON_SMC		/* define if console on SMC */
#define	CONFIG_CONS_ON_SCC		/* define if console on SCC */
#undef	CONFIG_CONS_NONE		/* define if console on neither */
#define CONFIG_CONS_INDEX	1	/* which SMC/SCC channel for console */

/*
 * select ethernet configuration
 *
 * if either CONFIG_ETHER_ON_SCC or CONFIG_ETHER_ON_FCC is selected, then
 * CONFIG_ETHER_INDEX must be set to the channel number (1-4 for SCC, 1-3
 * for FCC)
 *
 * if CONFIG_ETHER_NONE is defined, then either the ethernet routines must be
 * defined elsewhere (as for the console), or CONFIG_CMD_NET must be unset.
 */
#undef	CONFIG_ETHER_ON_SCC		/* define if ethernet on SCC	*/
#define	CONFIG_ETHER_ON_FCC		/* define if ethernet on FCC	*/
#undef	CONFIG_ETHER_NONE		/* define if ethernet on neither */
#define CONFIG_ETHER_INDEX	2	/* which SCC/FCC channel for ethernet */

#if (CONFIG_ETHER_INDEX == 2)

/*
 * - Rx-CLK is CLK13
 * - Tx-CLK is CLK14
 * - Select bus for bd/buffers (see 28-13)
 * - Enable Full Duplex in FSMR
 */
# define CONFIG_SYS_CMXFCR_MASK2	(CMXFCR_FC2 | CMXFCR_RF2CS_MSK | CMXFCR_TF2CS_MSK)
# define CONFIG_SYS_CMXFCR_VALUE2	(CMXFCR_RF2CS_CLK13 | CMXFCR_TF2CS_CLK14)
# define CONFIG_SYS_CPMFCR_RAMTYPE	(0)
# define CONFIG_SYS_FCC_PSMR		(FCC_PSMR_FDE | FCC_PSMR_LPB)

#endif /* CONFIG_ETHER_INDEX */


/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

/* enable I2C */
#define CONFIG_HARD_I2C		1	/* I2C with hardware support */
#define CONFIG_SYS_I2C_SPEED		50000	/* I2C speed and slave address */
#define CONFIG_SYS_I2C_SLAVE		0x30


/* system clock rate (CLKIN) - equal to the 60x and local bus speed */
#define CONFIG_8260_CLKIN	50000000	/* in Hz */

#define CONFIG_BAUDRATE		115200


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

#undef CONFIG_CMD_KGDB


/* Define this if you want to boot from 0x00000100. If you don't define
 * this, you will need to program the bootloader to 0xfff00000, and
 * get the hardware reset config words at 0xfe000000. The simplest
 * way to do that is to program the bootloader at both addresses.
 * It is suggested that you just let U-Boot live at 0x00000000.
 */
#define CONFIG_SYS_RSD_BOOT_LOW 1

#define CONFIG_BOOTDELAY	5
#define CONFIG_BOOTARGS		"devfs=mount root=ramfs"
#define CONFIG_ETHADDR		08:00:3e:26:0a:5a
#define CONFIG_NETMASK          255.255.0.0

#if defined(CONFIG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	230400		/* speed to run kgdb serial port */
#define CONFIG_KGDB_SER_INDEX	2		/* which serial port to use */
#endif

/*
 * Miscellaneous configurable options
 */
#define	CONFIG_SYS_LONGHELP				/* undef to save memory		*/
#define	CONFIG_SYS_PROMPT		"=> "		/* Monitor Command Prompt	*/
#if defined(CONFIG_CMD_KGDB)
#define	CONFIG_SYS_CBSIZE		1024		/* Console I/O Buffer Size	*/
#else
#define	CONFIG_SYS_CBSIZE		256		/* Console I/O Buffer Size	*/
#endif
#define	CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16) /* Print Buffer Size */
#define	CONFIG_SYS_MAXARGS		16		/* max number of command args	*/
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size	*/

#define CONFIG_SYS_MEMTEST_START	0x00400000	/* memtest works on	*/
#define CONFIG_SYS_MEMTEST_END		0x01c00000	/* 4 ... 28 MB in DRAM	*/

#define	CONFIG_SYS_LOAD_ADDR		0x100000	/* default load address	*/

#define	CONFIG_SYS_HZ			1000		/* decrementer freq: 1 ms ticks	*/

/*
 * Low Level Configuration Settings
 * (address mappings, register initial values, etc.)
 * You should know what you are doing if you make changes here.
 */

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define PHYS_SDRAM_60X		0x00000000 /* SDRAM (60x Bus) */
#define PHYS_SDRAM_60X_SIZE	0x08000000 /* 128 MB */

#define PHYS_SDRAM_LOCAL	0x40000000 /* SDRAM (Local Bus) */
#define PHYS_SDRAM_LOCAL_SIZE	0x04000000 /* 64 MB */

#define PHYS_DPRAM_PCI		0xE8000000 /* DPRAM PPC/PCI */
#define PHYS_DPRAM_PCI_SIZE	0x00020000 /* 128 KB */

/*#define PHYS_DPRAM_PCI_SEM	0x04020000 / * DPRAM PPC/PCI Semaphore */
/*#define PHYS_DPRAM_PCI_SEM_SIZE	0x00000001 / * 1 Byte */

#define PHYS_DPRAM_SHARC	0xE8100000 /* DPRAM PPC/Sharc */
#define PHYS_DPRAM_SHARC_SIZE	0x00040000 /* 256 KB */

/*#define PHYS_DPRAM_SHARC_SEM	0x04140000 / * DPRAM PPC/Sharc Semaphore */
/*#define PHYS_DPRAM_SHARC_SEM_SIZE 0x00000001 / * 1 Byte */

#define PHYS_VIRTEX_REGISTER    0xE8300000 /* FPGA implemented register */
#define PHYS_VIRTEX_REGISTER_SIZE 0x00000100

#define PHYS_USB		0x04200000 /* USB Controller (60x Bus) */
#define PHYS_USB_SIZE		0x00000002 /* 2 Bytes */

#define PHYS_IMMR		0xF0000000 /* Internal Memory Mapped Reg. */

#define PHYS_FLASH		0xFF000000 /* Flash (60x Bus) */
#define PHYS_FLASH_SIZE		0x01000000 /* 16 MB */

#define CONFIG_SYS_IMMR		PHYS_IMMR

/*-----------------------------------------------------------------------
 * Reset Address
 *
 * In order to reset the CPU, U-Boot jumps to a special address which
 * causes a machine check exception. The default address for this is
 * CONFIG_SYS_MONITOR_BASE - sizeof (ulong), which might not always work, eg. when
 * testing the monitor in RAM using a JTAG debugger.
 *
 * Just set CONFIG_SYS_RESET_ADDRESS to an address that you know is sure to
 * cause a bus error on your hardware.
 */
#define CONFIG_SYS_RESET_ADDRESS	0x20000000

/*-----------------------------------------------------------------------
 * Hard Reset Configuration Words
 */

#if defined(CONFIG_SYS_RSD_BOOT_LOW)
#  define  CONFIG_SYS_RSD_HRCW_BOOT_FLAGS  (HRCW_CIP | HRCW_BMS)
#else
#  define  CONFIG_SYS_RSD_HRCW_BOOT_FLAGS  (0)
#endif /* defined(CONFIG_SYS_RSD_BOOT_LOW) */

/* get the HRCW ISB field from CONFIG_SYS_IMMR */
#define CONFIG_SYS_RSD_HRCW_IMMR ( ((CONFIG_SYS_IMMR & 0x10000000) >> 10) |\
			    ((CONFIG_SYS_IMMR & 0x01000000) >> 7)  |\
			    ((CONFIG_SYS_IMMR & 0x00100000) >> 4) )

#define CONFIG_SYS_HRCW_MASTER	(HRCW_L2CPC10 | \
			 HRCW_DPPC11 | \
			 CONFIG_SYS_RSD_HRCW_IMMR |\
			 HRCW_MMR00 | \
			 HRCW_APPC10 | \
			 HRCW_CS10PC00 | \
			 HRCW_MODCK_H0000 |\
			 CONFIG_SYS_RSD_HRCW_BOOT_FLAGS)

/* no slaves */
#define CONFIG_SYS_HRCW_SLAVE1	0
#define CONFIG_SYS_HRCW_SLAVE2	0
#define CONFIG_SYS_HRCW_SLAVE3	0
#define CONFIG_SYS_HRCW_SLAVE4	0
#define CONFIG_SYS_HRCW_SLAVE5	0
#define CONFIG_SYS_HRCW_SLAVE6	0
#define CONFIG_SYS_HRCW_SLAVE7	0

/*-----------------------------------------------------------------------
 * Definitions for initial stack pointer and data area (in DPRAM)
 */
#define CONFIG_SYS_INIT_RAM_ADDR	CONFIG_SYS_IMMR
#define	CONFIG_SYS_INIT_RAM_SIZE	0x4000	/* Size of used area in DPRAM	*/
#define CONFIG_SYS_GBL_DATA_OFFSET	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define	CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

/*-----------------------------------------------------------------------
 * Start addresses for the final memory configuration
 * (Set up by the startup code)
 * Please note that CONFIG_SYS_SDRAM_BASE _must_ start at 0
 * Note also that the logic that sets CONFIG_SYS_RAMBOOT is platform dependend.
 */
#define	CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_60X
#define CONFIG_SYS_FLASH_BASE		PHYS_FLASH
/*#define	CONFIG_SYS_MONITOR_BASE	0x200000 */
#define	CONFIG_SYS_MONITOR_BASE	CONFIG_SYS_FLASH_BASE
#if CONFIG_SYS_MONITOR_BASE < CONFIG_SYS_FLASH_BASE
#define CONFIG_SYS_RAMBOOT
#endif
#define	CONFIG_SYS_MONITOR_LEN		(160 << 10)	/* Reserve 160 kB for Monitor	*/
#define	CONFIG_SYS_MALLOC_LEN		(128 << 10)	/* Reserve 128 kB for malloc()	*/

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define	CONFIG_SYS_BOOTMAPSZ		(8 << 20)	/* Initial Memory map for Linux	*/

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CONFIG_SYS_MAX_FLASH_BANKS	2	/* max number of memory banks		*/
#define CONFIG_SYS_MAX_FLASH_SECT	63	/* max number of sectors on one chip	*/

#define CONFIG_SYS_FLASH_ERASE_TOUT	12000	/* Timeout for Flash Erase (in ms)	*/
#define CONFIG_SYS_FLASH_WRITE_TOUT	3000	/* Timeout for Flash Write (in ms)	*/

/* turn off NVRAM env feature */
#undef CONFIG_NVRAM_ENV

#define	CONFIG_ENV_IS_IN_FLASH	1
#define CONFIG_ENV_ADDR	(PHYS_FLASH + 0x28000)	/* Addr of Environment Sector	*/
#define CONFIG_ENV_SECT_SIZE	0x8000	/* Total Size of Environment Sector	*/

/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CONFIG_SYS_CACHELINE_SIZE	32	/* For MPC8260 CPU			*/
#if defined(CONFIG_CMD_KGDB)
#define CONFIG_SYS_CACHELINE_SHIFT	5	/* log base 2 of the above value	*/
#endif

/*-----------------------------------------------------------------------
 * HIDx - Hardware Implementation-dependent Registers			 2-11
 *-----------------------------------------------------------------------
 * HID0 also contains cache control - initially enable both caches and
 * invalidate contents, then the final state leaves only the instruction
 * cache enabled. Note that Power-On and Hard reset invalidate the caches,
 * but Soft reset does not.
 *
 * HID1 has only read-only information - nothing to set.
 */
#define CONFIG_SYS_HID0_INIT	(HID0_ICE|HID0_DCE|HID0_ICFI|HID0_DCI|HID0_IFEM|HID0_ABE)
#define CONFIG_SYS_HID0_FINAL	(HID0_ICE|HID0_IFEM|HID0_ABE|HID0_EMCP)
#define CONFIG_SYS_HID2	0

/*-----------------------------------------------------------------------
 * RMR - Reset Mode Register
 *-----------------------------------------------------------------------
 */
#define CONFIG_SYS_RMR		0

/*-----------------------------------------------------------------------
 * BCR - Bus Configuration					 4-25
 *-----------------------------------------------------------------------
 */
#define CONFIG_SYS_BCR		0x100c0000

/*-----------------------------------------------------------------------
 * SIUMCR - SIU Module Configuration				 4-31
 *-----------------------------------------------------------------------
 */

#define CONFIG_SYS_SIUMCR	(SIUMCR_DPPC11 | SIUMCR_L2CPC10 | SIUMCR_APPC10 | \
					 SIUMCR_CS10PC01 | SIUMCR_BCTLC01)

/*-----------------------------------------------------------------------
 * SYPCR - System Protection Control				11-9
 * SYPCR can only be written once after reset!
 *-----------------------------------------------------------------------
 * Watchdog & Bus Monitor Timer max, 60x Bus Monitor enable
 */
#define CONFIG_SYS_SYPCR	(SYPCR_SWTC | SYPCR_BMT | SYPCR_PBME | SYPCR_LBME | \
			 SYPCR_SWRI | SYPCR_SWP)

/*-----------------------------------------------------------------------
 * TMCNTSC - Time Counter Status and Control			 4-40
 *-----------------------------------------------------------------------
 * Clear once per Second and Alarm Interrupt Status, Set 32KHz timersclk,
 * and enable Time Counter
 */
#define CONFIG_SYS_TMCNTSC	(TMCNTSC_SEC | TMCNTSC_ALR | TMCNTSC_TCF | TMCNTSC_TCE)

/*-----------------------------------------------------------------------
 * PISCR - Periodic Interrupt Status and Control		 4-42
 *-----------------------------------------------------------------------
 * Clear Periodic Interrupt Status, Set 32KHz timersclk, and enable
 * Periodic timer
 */
#define CONFIG_SYS_PISCR	(PISCR_PS|PISCR_PTF|PISCR_PTE)

/*-----------------------------------------------------------------------
 * SCCR - System Clock Control					 9-8
 *-----------------------------------------------------------------------
 */
#define CONFIG_SYS_SCCR	0x00000000

/*-----------------------------------------------------------------------
 * RCCR - RISC Controller Configuration				13-7
 *-----------------------------------------------------------------------
 */
#define CONFIG_SYS_RCCR	0

/*
 * Init Memory Controller:
 */

#define CONFIG_SYS_PSDMR	0x494D2452
#define CONFIG_SYS_LSDMR	0x49492552

/* Flash */
#define CONFIG_SYS_BR0_PRELIM	(PHYS_FLASH | BRx_V)
#define CONFIG_SYS_OR0_PRELIM	(P2SZ_TO_AM(PHYS_FLASH_SIZE) | \
			 ORxG_BCTLD | \
			 ORxG_SCY_5_CLK)

/* DPRAM to the PCI BUS on the protocol board */
#define CONFIG_SYS_BR1_PRELIM	(PHYS_DPRAM_PCI | BRx_V)
#define CONFIG_SYS_OR1_PRELIM	(P2SZ_TO_AM(PHYS_DPRAM_PCI_SIZE) | \
			 ORxG_ACS_DIV4)

/* 60x Bus SDRAM */
#define CONFIG_SYS_BR2_PRELIM	(PHYS_SDRAM_60X | BRx_MS_SDRAM_P | BRx_V)
#define CONFIG_SYS_OR2_PRELIM	(ORxS_SIZE_TO_AM(PHYS_SDRAM_60X_SIZE) | \
			 ORxS_BPD_4 | \
			 ORxS_ROWST_PBI1_A2 | \
			 ORxS_NUMR_13 | \
			 ORxS_IBID)

/* Virtex-FPGA - Register */
#define CONFIG_SYS_BR3_PRELIM  (PHYS_VIRTEX_REGISTER | BRx_V)
#define CONFIG_SYS_OR3_PRELIM  (ORxS_SIZE_TO_AM(PHYS_VIRTEX_REGISTER_SIZE) | \
			 ORxG_SCY_1_CLK | \
			 ORxG_ACS_DIV2 | \
			 ORxG_CSNT )

/* local bus SDRAM */
#define CONFIG_SYS_BR4_PRELIM	(PHYS_SDRAM_LOCAL | BRx_PS_32 | BRx_MS_SDRAM_L | BRx_V)
#define CONFIG_SYS_OR4_PRELIM	(ORxS_SIZE_TO_AM(PHYS_SDRAM_LOCAL_SIZE) | \
			 ORxS_BPD_4 | \
			 ORxS_ROWST_PBI1_A4 | \
			 ORxS_NUMR_13)

/* DPRAM to the Sharc-Bus on the protocol board */
#define CONFIG_SYS_BR5_PRELIM	(PHYS_DPRAM_SHARC | BRx_V)
#define CONFIG_SYS_OR5_PRELIM	(P2SZ_TO_AM(PHYS_DPRAM_SHARC_SIZE) | \
			 ORxG_ACS_DIV4)

#endif	/* __CONFIG_H */
