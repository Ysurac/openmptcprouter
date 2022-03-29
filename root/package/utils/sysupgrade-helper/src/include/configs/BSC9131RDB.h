/*
 * Copyright 2011-2012 Freescale Semiconductor, Inc.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * BSC9131 RDB board configuration file
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#ifdef CONFIG_BSC9131RDB
#define CONFIG_BSC9131
#define CONFIG_NAND_FSL_IFC
#endif

#ifdef CONFIG_SPIFLASH
#define CONFIG_RAMBOOT_SPIFLASH
#define CONFIG_SYS_RAMBOOT
#define CONFIG_SYS_EXTRA_ENV_RELOC
#define CONFIG_SYS_TEXT_BASE		0x11000000
#define CONFIG_RESET_VECTOR_ADDRESS	0x1107fffc
#endif

#ifndef CONFIG_SYS_MONITOR_BASE
#define CONFIG_SYS_MONITOR_BASE CONFIG_SYS_TEXT_BASE    /* start of monitor */
#endif

/* High Level Configuration Options */
#define CONFIG_BOOKE			/* BOOKE */
#define CONFIG_E500			/* BOOKE e500 family */
#define CONFIG_MPC85xx		/* MPC8540/60/55/41/48/P1020/P2020/P1010,etc*/
#define CONFIG_FSL_IFC			/* Enable IFC Support */

#define CONFIG_FSL_LAW			/* Use common FSL init code */
#define CONFIG_TSEC_ENET
#define CONFIG_ENV_OVERWRITE

#define CONFIG_DDR_CLK_FREQ	66666666 /* DDRCLK on 9131 RDB */
#define CONFIG_SYS_CLK_FREQ	66666666 /* SYSCLK for 9131 RDB */

#define CONFIG_HWCONFIG
/*
 * These can be toggled for performance analysis, otherwise use default.
 */
#define CONFIG_L2_CACHE			/* toggle L2 cache */
#define CONFIG_BTB			/* enable branch predition */

#define CONFIG_SYS_MEMTEST_START	0x01000000	/* memtest works on */
#define CONFIG_SYS_MEMTEST_END		0x01ffffff

/* DDR Setup */
#define CONFIG_FSL_DDR3
#undef CONFIG_SYS_DDR_RAW_TIMING
#undef CONFIG_DDR_SPD
#define CONFIG_SYS_SPD_BUS_NUM		0
#define SPD_EEPROM_ADDRESS		0x52 /* I2C access */

#define CONFIG_MEM_INIT_VALUE		0xDeadBeef

#ifndef __ASSEMBLY__
extern unsigned long get_sdram_size(void);
#endif
#define CONFIG_SYS_SDRAM_SIZE		get_sdram_size() /* DDR size */
#define CONFIG_SYS_DDR_SDRAM_BASE	0x00000000
#define CONFIG_SYS_SDRAM_BASE		CONFIG_SYS_DDR_SDRAM_BASE

#define CONFIG_NUM_DDR_CONTROLLERS	1
#define CONFIG_DIMM_SLOTS_PER_CTLR	1
#define CONFIG_CHIP_SELECTS_PER_CTRL	1

#define CONFIG_SYS_DDR_CS0_BNDS		0x0000003f
#define CONFIG_SYS_DDR_CS0_CONFIG	0x80014302
#define CONFIG_SYS_DDR_CS0_CONFIG_2	0x00000000

#define CONFIG_SYS_DDR_DATA_INIT	0xdeadbeef
#define CONFIG_SYS_DDR_INIT_ADDR	0x00000000
#define CONFIG_SYS_DDR_INIT_EXT_ADDR	0x00000000
#define CONFIG_SYS_DDR_MODE_CONTROL	0x00000000

#define CONFIG_SYS_DDR_ZQ_CONTROL	0x89080600
#define CONFIG_SYS_DDR_SR_CNTR		0x00000000
#define CONFIG_SYS_DDR_RCW_1		0x00000000
#define CONFIG_SYS_DDR_RCW_2		0x00000000
#define CONFIG_SYS_DDR_CONTROL		0xC70C0000	/* Type = DDR3	*/
#define CONFIG_SYS_DDR_CONTROL_2	0x24401000
#define CONFIG_SYS_DDR_TIMING_4		0x00000001
#define CONFIG_SYS_DDR_TIMING_5		0x02401400

#define CONFIG_SYS_DDR_TIMING_3_800		0x00030000
#define CONFIG_SYS_DDR_TIMING_0_800		0x00110104
#define CONFIG_SYS_DDR_TIMING_1_800		0x6f6b8644
#define CONFIG_SYS_DDR_TIMING_2_800		0x0fa888cf
#define CONFIG_SYS_DDR_CLK_CTRL_800		0x03000000
#define CONFIG_SYS_DDR_MODE_1_800		0x00441420
#define CONFIG_SYS_DDR_MODE_2_800		0x8000c000
#define CONFIG_SYS_DDR_INTERVAL_800		0x0c300100
#define CONFIG_SYS_DDR_WRLVL_CONTROL_800	0x8675f608

/*
 * Base addresses -- Note these are effective addresses where the
 * actual resources get mapped (not physical addresses)
 */
/* relocated CCSRBAR */
#define CONFIG_SYS_CCSRBAR	CONFIG_SYS_CCSRBAR_DEFAULT
#define CONFIG_SYS_CCSRBAR_PHYS_LOW	CONFIG_SYS_CCSRBAR_DEFAULT

#define CONFIG_SYS_IMMR		CONFIG_SYS_CCSRBAR	/* PQII uses */
							/* CONFIG_SYS_IMMR */

/*
 * Memory map
 *
 * 0x0000_0000	0x3FFF_FFFF	DDR			1G cacheable
 * 0x8800_0000	0x8810_0000	IFC internal SRAM		1M
 * 0xC100_0000	0xC13F_FFFF	MAPLE-2F		4M
 * 0xC1F0_0000	0xC1F3_FFFF	PA L2 SRAM Region 0	256K
 * 0xC1F8_0000	0xC1F9_FFFF	PA L2 SRAM Region 1	128K
 * 0xFED0_0000	0xFED0_3FFF	SEC Secured RAM		16K
 * 0xFF70_0000	0xFF7F_FFFF	PA CCSR			1M
 * 0xFF80_0000	0xFFFF_FFFF	Boot Page & NAND flash buffer	8M
 *
 */

/*
 * IFC Definitions
 */
#define CONFIG_SYS_NO_FLASH

/* NAND Flash on IFC */
#define CONFIG_SYS_NAND_BASE		0xff800000
#define CONFIG_SYS_NAND_BASE_PHYS	CONFIG_SYS_NAND_BASE

#define CONFIG_SYS_NAND_CSPR	(CSPR_PHYS_ADDR(CONFIG_SYS_NAND_BASE_PHYS) \
				| CSPR_PORT_SIZE_8	/* Port Size = 8 bit*/ \
				| CSPR_MSEL_NAND	/* MSEL = NAND */ \
				| CSPR_V)
#define CONFIG_SYS_NAND_AMASK	IFC_AMASK(64*1024)

#define CONFIG_SYS_NAND_CSOR    (CSOR_NAND_ECC_ENC_EN   /* ECC on encode */ \
				| CSOR_NAND_ECC_DEC_EN  /* ECC on decode */ \
				| CSOR_NAND_ECC_MODE_4  /* 4-bit ECC */ \
				| CSOR_NAND_RAL_2	/* RAL = 2Byes */ \
				| CSOR_NAND_PGS_2K	/* Page Size = 2K */ \
				| CSOR_NAND_SPRZ_64	/* Spare size = 64 */ \
				| CSOR_NAND_PB(64))	/*Pages Per Block = 64*/

/* NAND Flash Timing Params */
#define CONFIG_SYS_NAND_FTIM0		(FTIM0_NAND_TCCST(0x08)  \
					| FTIM0_NAND_TWP(0x06)   \
					| FTIM0_NAND_TWCHT(0x03) \
					| FTIM0_NAND_TWH(0x04))
#define CONFIG_SYS_NAND_FTIM1		(FTIM1_NAND_TADLE(0x18) \
					| FTIM1_NAND_TWBE(0x23) \
					| FTIM1_NAND_TRR(0x08)  \
					| FTIM1_NAND_TRP(0x05))
#define CONFIG_SYS_NAND_FTIM2		(FTIM2_NAND_TRAD(0x08)  \
					| FTIM2_NAND_TREH(0x04) \
					| FTIM2_NAND_TWHRE(0x3f))
#define CONFIG_SYS_NAND_FTIM3		FTIM3_NAND_TWW(0x22)

#define CONFIG_SYS_NAND_BASE_LIST	{ CONFIG_SYS_NAND_BASE }
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_MTD_NAND_VERIFY_WRITE
#define CONFIG_CMD_NAND
#define CONFIG_SYS_NAND_BLOCK_SIZE	(128 * 1024)

#define CONFIG_SYS_NAND_DDR_LAW		11

/* Set up IFC registers for boot location NAND */
#define CONFIG_SYS_CSPR0		CONFIG_SYS_NAND_CSPR
#define CONFIG_SYS_AMASK0		CONFIG_SYS_NAND_AMASK
#define CONFIG_SYS_CSOR0		CONFIG_SYS_NAND_CSOR
#define CONFIG_SYS_CS0_FTIM0		CONFIG_SYS_NAND_FTIM0
#define CONFIG_SYS_CS0_FTIM1		CONFIG_SYS_NAND_FTIM1
#define CONFIG_SYS_CS0_FTIM2		CONFIG_SYS_NAND_FTIM2
#define CONFIG_SYS_CS0_FTIM3		CONFIG_SYS_NAND_FTIM3

#define CONFIG_BOARD_EARLY_INIT_F	/* Call board_pre_init */

#define CONFIG_SYS_INIT_RAM_LOCK
#define CONFIG_SYS_INIT_RAM_ADDR	0xffd00000	/* stack in RAM */
#define CONFIG_SYS_INIT_RAM_END		0x00004000/* End of used area in RAM */

#define CONFIG_SYS_GBL_DATA_OFFSET	(CONFIG_SYS_INIT_RAM_END \
						- GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

#define CONFIG_SYS_MONITOR_LEN		(256 * 1024) /* Reserve 256 kB for Mon*/
#define CONFIG_SYS_MALLOC_LEN		(1024 * 1024)	/* Reserved for malloc*/

/* Serial Port */
#define CONFIG_CONS_INDEX	1
#undef	CONFIG_SERIAL_SOFTWARE_FIFO
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	1
#define CONFIG_SYS_NS16550_CLK		get_bus_freq(0)

#define CONFIG_SYS_CONSOLE_IS_IN_ENV	/* determine from environment */

#define CONFIG_SYS_BAUDRATE_TABLE	\
	{300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200}

#define CONFIG_SYS_NS16550_COM1	(CONFIG_SYS_CCSRBAR+0x4500)

/* Use the HUSH parser */
#define CONFIG_SYS_HUSH_PARSER
#ifdef	CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2 "> "
#endif

/*
 * Pass open firmware flat tree
 */
#define CONFIG_OF_LIBFDT
#define CONFIG_OF_BOARD_SETUP
#define CONFIG_OF_STDOUT_VIA_ALIAS

/* new uImage format support */
#define CONFIG_FIT
#define CONFIG_FIT_VERBOSE	/* enable fit_format_{error,warning}() */

#define CONFIG_FSL_I2C			/* Use FSL common I2C driver */
#define CONFIG_HARD_I2C			/* I2C with hardware support */
#undef CONFIG_SOFT_I2C			/* I2C bit-banged */
#define CONFIG_I2C_MULTI_BUS
#define CONFIG_I2C_CMD_TREE
#define CONFIG_SYS_I2C_SPEED		400000 /* I2C speed and slave address*/
#define CONFIG_SYS_I2C_OFFSET		0x3000

/* I2C EEPROM */
#define CONFIG_CMD_EEPROM
#define CONFIG_SYS_I2C_MULTI_EEPROMS
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN 1
#define CONFIG_SYS_EEPROM_PAGE_WRITE_BITS 3
#define CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS 5

#define CONFIG_CMD_I2C


#define CONFIG_FSL_ESPI
/* eSPI - Enhanced SPI */
#ifdef CONFIG_FSL_ESPI
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_SPANSION
#define CONFIG_CMD_SF
#define CONFIG_SF_DEFAULT_SPEED		10000000
#define CONFIG_SF_DEFAULT_MODE		SPI_MODE_0
#endif

#if defined(CONFIG_TSEC_ENET)

#define CONFIG_MII			/* MII PHY management */
#define CONFIG_MII_DEFAULT_TSEC	1	/* Allow unregistered phys */
#define CONFIG_TSEC1	1
#define CONFIG_TSEC1_NAME	"eTSEC1"
#define CONFIG_TSEC2	1
#define CONFIG_TSEC2_NAME	"eTSEC2"

#define TSEC1_PHY_ADDR		0
#define TSEC2_PHY_ADDR		3

#define TSEC1_FLAGS		(TSEC_GIGABIT | TSEC_REDUCED)
#define TSEC2_FLAGS		(TSEC_GIGABIT | TSEC_REDUCED)

#define TSEC1_PHYIDX		0

#define TSEC2_PHYIDX		0

#define CONFIG_ETHPRIME		"eTSEC1"

#define CONFIG_PHY_GIGE		/* Include GbE speed/duplex detection */

#endif	/* CONFIG_TSEC_ENET */

/*
 * Environment
 */
#if defined(CONFIG_SYS_RAMBOOT)
#if defined(CONFIG_RAMBOOT_SPIFLASH)
#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_SPI_BUS	0
#define CONFIG_ENV_SPI_CS	0
#define CONFIG_ENV_SPI_MAX_HZ	10000000
#define CONFIG_ENV_SPI_MODE	0
#define CONFIG_ENV_OFFSET	0x100000	/* 1MB */
#define CONFIG_ENV_SECT_SIZE	0x10000
#define CONFIG_ENV_SIZE		0x2000
#else
#define CONFIG_ENV_IS_NOWHERE		/* Store ENV in memory only */
#define CONFIG_ENV_ADDR			(CONFIG_SYS_MONITOR_BASE - 0x1000)
#define CONFIG_ENV_SIZE			0x2000
#endif
#else
#define CONFIG_ENV_IS_NOWHERE	1	/* Store ENV in memory only */
#define CONFIG_ENV_ADDR		(CONFIG_SYS_MONITOR_BASE - 0x1000)
#define CONFIG_ENV_SIZE		0x400
#endif

#define CONFIG_LOADS_ECHO		/* echo on for serial download */
#define CONFIG_SYS_LOADS_BAUD_CHANGE		/* allow baudrate change */

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_DHCP
#define CONFIG_CMD_ERRATA
#define CONFIG_CMD_ELF
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT
#define CONFIG_CMD_IRQ
#define CONFIG_CMD_MII
#define CONFIG_DOS_PARTITION
#define CONFIG_CMD_PING
#define CONFIG_CMD_REGINFO
#define CONFIG_CMD_SETEXPR

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP			/* undef to save memory	*/
#define CONFIG_CMDLINE_EDITING			/* Command-line editing */
#define CONFIG_AUTO_COMPLETE			/* add autocompletion support */
#define CONFIG_SYS_LOAD_ADDR	0x2000000	/* default load address */
#define CONFIG_SYS_PROMPT	"=> "		/* Monitor Command Prompt */

#if defined(CONFIG_CMD_KGDB)
#define CONFIG_SYS_CBSIZE	1024		/* Console I/O Buffer Size */
#else
#define CONFIG_SYS_CBSIZE	1024		/* Console I/O Buffer Size */
#endif
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
						/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	16		/* max number of command args */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE/* Boot Argument Buffer Size */
#define CONFIG_SYS_HZ		1000	/* decrementer freq: 1ms ticks */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 64 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTMAPSZ	(64 << 20) /* Initial Memory map for Linux */
#define CONFIG_SYS_BOOTM_LEN	(64 << 20) /* Increase max gunzip size */

#if defined(CONFIG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	230400	/* speed to run kgdb serial port */
#define CONFIG_KGDB_SER_INDEX	2	/* which serial port to use */
#endif

#define CONFIG_USB_EHCI

#ifdef CONFIG_USB_EHCI
#define CONFIG_CMD_USB
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET
#define CONFIG_USB_EHCI_FSL
#define CONFIG_USB_STORAGE
#define CONFIG_HAS_FSL_DR_USB
#endif

/*
 * Environment Configuration
 */

#if defined(CONFIG_TSEC_ENET)
#define CONFIG_HAS_ETH0
#endif

#define CONFIG_HOSTNAME		BSC9131rdb
#define CONFIG_ROOTPATH		"/opt/nfsroot"
#define CONFIG_BOOTFILE		"uImage"
#define CONFIG_UBOOTPATH	"u-boot.bin" /* U-Boot image on TFTP server */

#define CONFIG_BAUDRATE		115200

#define	CONFIG_EXTRA_ENV_SETTINGS				\
	"netdev=eth0\0"						\
	"uboot=" CONFIG_UBOOTPATH "\0"				\
	"loadaddr=1000000\0"			\
	"bootfile=uImage\0"	\
	"consoledev=ttyS0\0"				\
	"ramdiskaddr=2000000\0"			\
	"ramdiskfile=rootfs.ext2.gz.uboot\0"		\
	"fdtaddr=c00000\0"				\
	"fdtfile=bsc9131rdb.dtb\0"		\
	"bdev=sda1\0"	\
	"hwconfig=usb1:dr_mode=host,phy_type=ulpi\0"	\
	"othbootargs=ramdisk_size=600000 \0" \
	"usbext2boot=setenv bootargs root=/dev/ram rw "	\
	"console=$consoledev,$baudrate $othbootargs; "	\
	"usb start;"			\
	"ext2load usb 0:4 $loadaddr $bootfile;"		\
	"ext2load usb 0:4 $fdtaddr $fdtfile;"	\
	"ext2load usb 0:4 $ramdiskaddr $ramdiskfile;"	\
	"bootm $loadaddr $ramdiskaddr $fdtaddr\0"	\

#define CONFIG_RAMBOOTCOMMAND		\
	"setenv bootargs root=/dev/ram rw "	\
	"console=$consoledev,$baudrate $othbootargs; "	\
	"tftp $ramdiskaddr $ramdiskfile;"	\
	"tftp $loadaddr $bootfile;"		\
	"tftp $fdtaddr $fdtfile;"		\
	"bootm $loadaddr $ramdiskaddr $fdtaddr"

#define CONFIG_BOOTCOMMAND CONFIG_RAMBOOTCOMMAND

#endif	/* __CONFIG_H */
