/*
 * Copyright (C) Freescale Semiconductor, Inc. 2006.
 *
 * (C) Copyright 2010
 * Heiko Schocher, DENX Software Engineering, hs@denx.de.
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
 * ve8313 board configuration file
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 */
#define CONFIG_E300		1
#define CONFIG_MPC83xx		1
#define CONFIG_MPC831x		1
#define CONFIG_MPC8313		1
#define CONFIG_VE8313		1

#ifndef CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_TEXT_BASE	0xfe000000
#endif

#define CONFIG_PCI		1
#define CONFIG_FSL_ELBC		1

#define CONFIG_BOARD_EARLY_INIT_F	1

/*
 * On-board devices
 *
 */
#define CONFIG_83XX_CLKIN	32000000	/* in Hz */

#define CONFIG_SYS_CLK_FREQ	CONFIG_83XX_CLKIN

#define CONFIG_SYS_IMMR		0xE0000000

#define CONFIG_SYS_MEMTEST_START	0x00001000
#define CONFIG_SYS_MEMTEST_END		0x07000000

#define CONFIG_SYS_ACR_PIPE_DEP		3	/* Arbiter pipeline depth */
#define CONFIG_SYS_ACR_RPTCNT		3	/* Arbiter repeat count */

/*
 * Device configurations
 */

/*
 * DDR Setup
 */
#define CONFIG_SYS_DDR_BASE		0x00000000 /* DDR is system memory*/
#define CONFIG_SYS_SDRAM_BASE		CONFIG_SYS_DDR_BASE
#define CONFIG_SYS_DDR_SDRAM_BASE	CONFIG_SYS_DDR_BASE

/*
 * Manually set up DDR parameters, as this board does not
 * have the SPD connected to I2C.
 */
#define CONFIG_SYS_DDR_SIZE	128	/* MB */
#define CONFIG_SYS_DDR_CS0_CONFIG	(CSCONFIG_EN \
				| CSCONFIG_AP \
				| CSCONFIG_ODT_RD_NEVER \
				| CSCONFIG_ODT_WR_ALL \
				| CSCONFIG_ROW_BIT_13 \
				| CSCONFIG_COL_BIT_10)
				/* 0x80840102 */

#define CONFIG_SYS_DDR_TIMING_3	0x00000000
#define CONFIG_SYS_DDR_TIMING_0	((0 << TIMING_CFG0_RWT_SHIFT) \
				| (0 << TIMING_CFG0_WRT_SHIFT) \
				| (3 << TIMING_CFG0_RRT_SHIFT) \
				| (2 << TIMING_CFG0_WWT_SHIFT) \
				| (7 << TIMING_CFG0_ACT_PD_EXIT_SHIFT) \
				| (2 << TIMING_CFG0_PRE_PD_EXIT_SHIFT) \
				| (8 << TIMING_CFG0_ODT_PD_EXIT_SHIFT) \
				| (2 << TIMING_CFG0_MRS_CYC_SHIFT))
				/* 0x0e720802 */
#define CONFIG_SYS_DDR_TIMING_1	((2 << TIMING_CFG1_PRETOACT_SHIFT) \
				| (6 << TIMING_CFG1_ACTTOPRE_SHIFT) \
				| (2 << TIMING_CFG1_ACTTORW_SHIFT) \
				| (5 << TIMING_CFG1_CASLAT_SHIFT) \
				| (6 << TIMING_CFG1_REFREC_SHIFT) \
				| (2 << TIMING_CFG1_WRREC_SHIFT) \
				| (2 << TIMING_CFG1_ACTTOACT_SHIFT) \
				| (2 << TIMING_CFG1_WRTORD_SHIFT))
				/* 0x26256222 */
#define CONFIG_SYS_DDR_TIMING_2	((0 << TIMING_CFG2_ADD_LAT_SHIFT) \
				| (5 << TIMING_CFG2_CPO_SHIFT) \
				| (2 << TIMING_CFG2_WR_LAT_DELAY_SHIFT) \
				| (1 << TIMING_CFG2_RD_TO_PRE_SHIFT) \
				| (2 << TIMING_CFG2_WR_DATA_DELAY_SHIFT) \
				| (3 << TIMING_CFG2_CKE_PLS_SHIFT) \
				| (7 << TIMING_CFG2_FOUR_ACT_SHIFT))
				/* 0x029028c7 */
#define CONFIG_SYS_DDR_INTERVAL	((0x320 << SDRAM_INTERVAL_REFINT_SHIFT) \
				| (0x2000 << SDRAM_INTERVAL_BSTOPRE_SHIFT))
				/* 0x03202000 */
#define CONFIG_SYS_SDRAM_CFG	(SDRAM_CFG_SREN \
				| SDRAM_CFG_SDRAM_TYPE_DDR2 \
				| SDRAM_CFG_DBW_32)
				/* 0x43080000 */
#define CONFIG_SYS_SDRAM_CFG2	0x00401000
#define CONFIG_SYS_DDR_MODE	((0x4440 << SDRAM_MODE_ESD_SHIFT) \
				| (0x0232 << SDRAM_MODE_SD_SHIFT))
				/* 0x44400232 */
#define CONFIG_SYS_DDR_MODE_2	0x8000C000

#define CONFIG_SYS_DDR_CLK_CNTL	DDR_SDRAM_CLK_CNTL_CLK_ADJUST_05
				/*0x02000000*/
#define CONFIG_SYS_DDRCDR_VALUE	(DDRCDR_EN \
				| DDRCDR_PZ_NOMZ \
				| DDRCDR_NZ_NOMZ \
				| DDRCDR_M_ODR)
				/* 0x73000002 */

/*
 * FLASH on the Local Bus
 */
#define CONFIG_SYS_FLASH_CFI		/* use the Common Flash Interface */
#define CONFIG_FLASH_CFI_DRIVER		/* use the CFI driver */
#define CONFIG_SYS_FLASH_BASE		0xFE000000
#define CONFIG_SYS_FLASH_SIZE		32	/* size in MB */
#define CONFIG_SYS_FLASH_EMPTY_INFO		/* display empty sectors */
#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE	/* buffer up multiple bytes */

#define CONFIG_SYS_NOR_BR_PRELIM	(CONFIG_SYS_FLASH_BASE \
					| BR_PS_16	/* 16 bit */ \
					| BR_MS_GPCM	/* MSEL = GPCM */ \
					| BR_V)		/* valid */
#define CONFIG_SYS_NOR_OR_PRELIM	(MEG_TO_AM(CONFIG_SYS_FLASH_SIZE) \
					| OR_GPCM_CSNT \
					| OR_GPCM_ACS_DIV4 \
					| OR_GPCM_SCY_5 \
					| OR_GPCM_TRLX_SET \
					| OR_GPCM_EAD)
					/* 0xfe000c55 */

#define CONFIG_SYS_LBLAWBAR0_PRELIM	CONFIG_SYS_FLASH_BASE
#define CONFIG_SYS_LBLAWAR0_PRELIM	(LBLAWAR_EN | LBLAWAR_32MB)

#define CONFIG_SYS_MAX_FLASH_BANKS	1		/* number of banks */
#define CONFIG_SYS_MAX_FLASH_SECT	256		/* sectors per dev */

#define CONFIG_SYS_FLASH_ERASE_TOUT	60000	/* Flash Erase Timeout (ms) */
#define CONFIG_SYS_FLASH_WRITE_TOUT	500	/* Flash Write Timeout (ms) */

#define CONFIG_SYS_MONITOR_BASE	CONFIG_SYS_TEXT_BASE	/* start of monitor */

#if (CONFIG_SYS_MONITOR_BASE < CONFIG_SYS_FLASH_BASE)
#define CONFIG_SYS_RAMBOOT
#endif

#define CONFIG_SYS_INIT_RAM_LOCK	1
#define CONFIG_SYS_INIT_RAM_ADDR	0xFD000000 /* Initial RAM address */
#define CONFIG_SYS_INIT_RAM_SIZE	0x1000	/* Size of used area in RAM*/

#define CONFIG_SYS_GBL_DATA_OFFSET	\
			(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

/* CONFIG_SYS_MONITOR_LEN must be a multiple of CONFIG_ENV_SECT_SIZE */
#define CONFIG_SYS_MONITOR_LEN		(384 * 1024)
#define CONFIG_SYS_MALLOC_LEN		(512 * 1024)

/*
 * Local Bus LCRR and LBCR regs
 */
#define CONFIG_SYS_LCRR_EADC	LCRR_EADC_3
#define CONFIG_SYS_LCRR_CLKDIV	LCRR_CLKDIV_2

#define CONFIG_SYS_LBC_LBCR	0x00040000

#define CONFIG_SYS_LBC_MRTPR	0x20000000

/*
 * NAND settings
 */
#define CONFIG_SYS_NAND_BASE		0x61000000
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_MTD_NAND_VERIFY_WRITE
#define CONFIG_CMD_NAND 1
#define CONFIG_NAND_FSL_ELBC 1
#define CONFIG_SYS_NAND_BLOCK_SIZE 16384

#define CONFIG_SYS_NAND_BR_PRELIM	(CONFIG_SYS_NAND_BASE \
					| BR_PS_8		\
					| BR_DECC_CHK_GEN	\
					| BR_MS_FCM		\
					| BR_V)	/* valid */
					/* 0x61000c21 */
#define CONFIG_SYS_NAND_OR_PRELIM	(OR_AM_32KB \
					| OR_FCM_BCTLD \
					| OR_FCM_CHT \
					| OR_FCM_SCY_2 \
					| OR_FCM_RST \
					| OR_FCM_TRLX)
					/* 0xffff90ac */

#define CONFIG_SYS_BR0_PRELIM CONFIG_SYS_NOR_BR_PRELIM
#define CONFIG_SYS_OR0_PRELIM CONFIG_SYS_NOR_OR_PRELIM
#define CONFIG_SYS_BR1_PRELIM CONFIG_SYS_NAND_BR_PRELIM
#define CONFIG_SYS_OR1_PRELIM CONFIG_SYS_NAND_OR_PRELIM

#define CONFIG_SYS_LBLAWBAR1_PRELIM	CONFIG_SYS_NAND_BASE
#define CONFIG_SYS_LBLAWAR1_PRELIM	(LBLAWAR_EN | LBLAWAR_32KB)

#define CONFIG_SYS_NAND_LBLAWBAR_PRELIM CONFIG_SYS_LBLAWBAR1_PRELIM
#define CONFIG_SYS_NAND_LBLAWAR_PRELIM CONFIG_SYS_LBLAWAR1_PRELIM

/* CS2 NvRAM */
#define CONFIG_SYS_BR2_PRELIM	(0x60000000 \
				| BR_PS_8 \
				| BR_V)
				/* 0x60000801 */
#define CONFIG_SYS_OR2_PRELIM	(OR_AM_128KB \
				| OR_GPCM_CSNT \
				| OR_GPCM_XACS \
				| OR_GPCM_SCY_3 \
				| OR_GPCM_TRLX_SET \
				| OR_GPCM_EHTR_SET \
				| OR_GPCM_EAD)
				/* 0xfffe0937 */
/* local bus read write buffer mapping SRAM@0x64000000 */
#define CONFIG_SYS_BR3_PRELIM	(0x62000000 \
				| BR_PS_16 \
				| BR_V)
				/* 0x62001001 */

#define CONFIG_SYS_OR3_PRELIM	(OR_AM_32MB \
				| OR_GPCM_CSNT \
				| OR_GPCM_XACS \
				| OR_GPCM_SCY_15 \
				| OR_GPCM_TRLX_SET \
				| OR_GPCM_EHTR_SET \
				| OR_GPCM_EAD)
				/* 0xfe0009f7 */

/* pass open firmware flat tree */
#define CONFIG_OF_LIBFDT	1
#define CONFIG_OF_BOARD_SETUP	1
#define CONFIG_OF_STDOUT_VIA_ALIAS	1

/*
 * Serial Port
 */
#define CONFIG_CONS_INDEX	1
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	1
#define CONFIG_SYS_NS16550_CLK		get_bus_freq(0)

#define CONFIG_SYS_BAUDRATE_TABLE	\
	{300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 115200}

#define CONFIG_SYS_NS16550_COM1	(CONFIG_SYS_IMMR+0x4500)
#define CONFIG_SYS_NS16550_COM2	(CONFIG_SYS_IMMR+0x4600)

/* Use the HUSH parser */
#define CONFIG_SYS_HUSH_PARSER

#if defined(CONFIG_PCI)
/*
 * General PCI
 * Addresses are mapped 1-1.
 */
#define CONFIG_SYS_PCI1_MEM_BASE	0x80000000
#define CONFIG_SYS_PCI1_MEM_PHYS	CONFIG_SYS_PCI1_MEM_BASE
#define CONFIG_SYS_PCI1_MEM_SIZE	0x10000000	/* 256M */
#define CONFIG_SYS_PCI1_MMIO_BASE	0x90000000
#define CONFIG_SYS_PCI1_MMIO_PHYS	CONFIG_SYS_PCI1_MMIO_BASE
#define CONFIG_SYS_PCI1_MMIO_SIZE	0x10000000	/* 256M */
#define CONFIG_SYS_PCI1_IO_BASE		0x00000000
#define CONFIG_SYS_PCI1_IO_PHYS		0xE2000000
#define CONFIG_SYS_PCI1_IO_SIZE		0x00100000	/* 1M */

#define CONFIG_PCI_PNP		/* do pci plug-and-play */
#define CONFIG_SYS_PCI_SUBSYS_VENDORID 0x1957	/* Freescale */
#endif

/*
 * TSEC
 */
#define CONFIG_TSEC_ENET		/* TSEC ethernet support */


#define CONFIG_TSEC1
#ifdef CONFIG_TSEC1
#define CONFIG_HAS_ETH0
#define CONFIG_TSEC1_NAME	"TSEC1"
#define CONFIG_SYS_TSEC1_OFFSET	0x24000
#define TSEC1_PHY_ADDR		0x01
#define TSEC1_FLAGS		0
#define TSEC1_PHYIDX		0
#endif

/* Options are: TSEC[0-1] */
#define CONFIG_ETHPRIME			"TSEC1"

/*
 * Environment
 */
#define CONFIG_ENV_IS_IN_FLASH	1
#define CONFIG_ENV_ADDR		\
			(CONFIG_SYS_FLASH_BASE + CONFIG_SYS_MONITOR_LEN)
#define CONFIG_ENV_SECT_SIZE	0x20000	/* 128K(one sector) for env */
#define CONFIG_ENV_SIZE		0x4000
/* Address and size of Redundant Environment Sector */
#define CONFIG_ENV_OFFSET_REDUND	\
			(CONFIG_ENV_OFFSET + CONFIG_ENV_SECT_SIZE)
#define CONFIG_ENV_SIZE_REDUND	(CONFIG_ENV_SIZE)

#define CONFIG_LOADS_ECHO	1	/* echo on for serial download */
#define CONFIG_SYS_LOADS_BAUD_CHANGE	1	/* allow baudrate change */

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

#define CONFIG_CMD_DHCP
#define CONFIG_CMD_MII
#define CONFIG_CMD_PING
#define CONFIG_CMD_PCI

#define CONFIG_CMDLINE_EDITING 1
#define CONFIG_AUTO_COMPLETE	/* add autocompletion support   */

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP			/* undef to save memory */
#define CONFIG_SYS_LOAD_ADDR	0x100000	/* default load address */
#define CONFIG_SYS_PROMPT	"=> "		/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE	1024		/* Console I/O Buffer Size */

#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS	16		/* max number of cmd args */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE /* Boot arg Buffer size */
#define CONFIG_SYS_HZ		1000		/* 1ms ticks */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 256 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
				/* Initial Memory map for Linux*/
#define CONFIG_SYS_BOOTMAPSZ	(256 << 20)

/* 0x64050000 */
#define CONFIG_SYS_HRCW_LOW (\
	0x20000000 /* reserved, must be set */ |\
	HRCWL_DDRCM |\
	HRCWL_CSB_TO_CLKIN_4X1 | \
	HRCWL_CORE_TO_CSB_2_5X1)

/* 0xa0600004 */
#define CONFIG_SYS_HRCW_HIGH (HRCWH_PCI_HOST | \
	HRCWH_PCI_ARBITER_ENABLE | \
	HRCWH_CORE_ENABLE | \
	HRCWH_FROM_0X00000100 | \
	HRCWH_BOOTSEQ_DISABLE |\
	HRCWH_SW_WATCHDOG_DISABLE |\
	HRCWH_ROM_LOC_LOCAL_16BIT | \
	HRCWH_TSEC1M_IN_MII | \
	HRCWH_BIG_ENDIAN | \
	HRCWH_LALE_EARLY)

/* System IO Config */
#define CONFIG_SYS_SICRH	(0x01000000 | \
				SICRH_ETSEC2_B | \
				SICRH_ETSEC2_C | \
				SICRH_ETSEC2_D | \
				SICRH_ETSEC2_E | \
				SICRH_ETSEC2_F | \
				SICRH_ETSEC2_G | \
				SICRH_TSOBI1 | \
				SICRH_TSOBI2)
				/* 0x010fff03 */
#define CONFIG_SYS_SICRL	(SICRL_LBC | \
				SICRL_SPI_A | \
				SICRL_SPI_B | \
				SICRL_SPI_C | \
				SICRL_SPI_D | \
				SICRL_ETSEC2_A)
				/* 0x33fc0003) */

#define CONFIG_SYS_HID0_INIT	0x000000000
#define CONFIG_SYS_HID0_FINAL	(HID0_ENABLE_MACHINE_CHECK | \
				 HID0_ENABLE_INSTRUCTION_CACHE)

#define CONFIG_SYS_HID2 HID2_HBE

#define CONFIG_HIGH_BATS	1	/* High BATs supported */

/* DDR @ 0x00000000 */
#define CONFIG_SYS_IBAT0L	(CONFIG_SYS_SDRAM_BASE | BATL_PP_RW)
#define CONFIG_SYS_IBAT0U	(CONFIG_SYS_SDRAM_BASE \
				| BATU_BL_256M \
				| BATU_VS \
				| BATU_VP)

#if defined(CONFIG_PCI)
/* PCI @ 0x80000000 */
#define CONFIG_SYS_IBAT1L	(CONFIG_SYS_PCI1_MEM_BASE | BATL_PP_RW)
#define CONFIG_SYS_IBAT1U	(CONFIG_SYS_PCI1_MEM_BASE \
				| BATU_BL_256M \
				| BATU_VS \
				| BATU_VP)
#define CONFIG_SYS_IBAT2L	(CONFIG_SYS_PCI1_MMIO_BASE \
				| BATL_PP_RW \
				| BATL_CACHEINHIBIT \
				| BATL_GUARDEDSTORAGE)
#define CONFIG_SYS_IBAT2U	(CONFIG_SYS_PCI1_MMIO_BASE \
				| BATU_BL_256M \
				| BATU_VS \
				| BATU_VP)
#else
#define CONFIG_SYS_IBAT1L	(0)
#define CONFIG_SYS_IBAT1U	(0)
#define CONFIG_SYS_IBAT2L	(0)
#define CONFIG_SYS_IBAT2U	(0)
#endif

/* PCI2 not supported on 8313 */
#define CONFIG_SYS_IBAT3L	(0)
#define CONFIG_SYS_IBAT3U	(0)
#define CONFIG_SYS_IBAT4L	(0)
#define CONFIG_SYS_IBAT4U	(0)

/* IMMRBAR @ 0xE0000000, PCI IO @ 0xE2000000 & BCSR @ 0xE2400000 */
#define CONFIG_SYS_IBAT5L	(CONFIG_SYS_IMMR \
				| BATL_PP_RW \
				| BATL_CACHEINHIBIT \
				| BATL_GUARDEDSTORAGE)
#define CONFIG_SYS_IBAT5U	(CONFIG_SYS_IMMR \
				| BATU_BL_256M \
				| BATU_VS \
				| BATU_VP)

/* stack in DCACHE 0xFDF00000 & FLASH @ 0xFE000000 */
#define CONFIG_SYS_IBAT6L	(0xF0000000 | BATL_PP_RW | BATL_GUARDEDSTORAGE)
#define CONFIG_SYS_IBAT6U	(0xF0000000 | BATU_BL_256M | BATU_VS | BATU_VP)

/*  FPGA, SRAM, NAND @ 0x60000000 */
#define CONFIG_SYS_IBAT7L	(0x60000000 | BATL_PP_RW | BATL_GUARDEDSTORAGE)
#define CONFIG_SYS_IBAT7U	(0x60000000 | BATU_BL_256M | BATU_VS | BATU_VP)

#define CONFIG_SYS_DBAT0L	CONFIG_SYS_IBAT0L
#define CONFIG_SYS_DBAT0U	CONFIG_SYS_IBAT0U
#define CONFIG_SYS_DBAT1L	CONFIG_SYS_IBAT1L
#define CONFIG_SYS_DBAT1U	CONFIG_SYS_IBAT1U
#define CONFIG_SYS_DBAT2L	CONFIG_SYS_IBAT2L
#define CONFIG_SYS_DBAT2U	CONFIG_SYS_IBAT2U
#define CONFIG_SYS_DBAT3L	CONFIG_SYS_IBAT3L
#define CONFIG_SYS_DBAT3U	CONFIG_SYS_IBAT3U
#define CONFIG_SYS_DBAT4L	CONFIG_SYS_IBAT4L
#define CONFIG_SYS_DBAT4U	CONFIG_SYS_IBAT4U
#define CONFIG_SYS_DBAT5L	CONFIG_SYS_IBAT5L
#define CONFIG_SYS_DBAT5U	CONFIG_SYS_IBAT5U
#define CONFIG_SYS_DBAT6L	CONFIG_SYS_IBAT6L
#define CONFIG_SYS_DBAT6U	CONFIG_SYS_IBAT6U
#define CONFIG_SYS_DBAT7L	CONFIG_SYS_IBAT7L
#define CONFIG_SYS_DBAT7U	CONFIG_SYS_IBAT7U

#define CONFIG_NETDEV		eth0

#define CONFIG_HOSTNAME		ve8313
#define CONFIG_UBOOTPATH	ve8313/u-boot.bin

#define CONFIG_BOOTDELAY	6	/* -1 disables auto-boot */
#define CONFIG_BAUDRATE		115200

#define XMK_STR(x)	#x
#define MK_STR(x)	XMK_STR(x)

#define CONFIG_EXTRA_ENV_SETTINGS \
	"netdev=" MK_STR(CONFIG_NETDEV) "\0"				\
	"ethprime=" MK_STR(CONFIG_TSEC1_NAME) "\0"			\
	"u-boot=" MK_STR(CONFIG_UBOOTPATH) "\0"				\
	"u-boot_addr_r=100000\0"					\
	"load=tftp ${u-boot_addr_r} ${u-boot}\0"			\
	"update=protect off " MK_STR(CONFIG_SYS_FLASH_BASE) " +${filesize};" \
	"erase " MK_STR(CONFIG_SYS_FLASH_BASE) " +${filesize};"		\
	"cp.b ${u-boot_addr_r} " MK_STR(CONFIG_SYS_FLASH_BASE)		\
	" ${filesize};"							\
	"protect on " MK_STR(CONFIG_SYS_FLASH_BASE) " +${filesize}\0"	\

#undef MK_STR
#undef XMK_STR

#endif	/* __CONFIG_H */
