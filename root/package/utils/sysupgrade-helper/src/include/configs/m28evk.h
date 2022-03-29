/*
 * Copyright (C) 2011 Marek Vasut <marek.vasut@gmail.com>
 * on behalf of DENX Software Engineering GmbH
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
#ifndef __M28EVK_CONFIG_H__
#define __M28EVK_CONFIG_H__

#include <asm/arch/regs-base.h>

/*
 * SoC configurations
 */
#define	CONFIG_MX28				/* i.MX28 SoC */
#define	CONFIG_MXS_GPIO				/* GPIO control */
#define	CONFIG_SYS_HZ		1000		/* Ticks per second */

/*
 * Define M28EVK machine type by hand until it lands in mach-types
 */
#define	MACH_TYPE_M28EVK	3613

#define	CONFIG_MACH_TYPE	MACH_TYPE_M28EVK

#define	CONFIG_SYS_NO_FLASH
#define	CONFIG_SYS_ICACHE_OFF
#define	CONFIG_SYS_DCACHE_OFF
#define	CONFIG_BOARD_EARLY_INIT_F
#define	CONFIG_ARCH_MISC_INIT

/*
 * SPL
 */
#define	CONFIG_SPL
#define	CONFIG_SPL_NO_CPU_SUPPORT_CODE
#define	CONFIG_SPL_START_S_PATH		"arch/arm/cpu/arm926ejs/mx28"
#define	CONFIG_SPL_LDSCRIPT	"arch/arm/cpu/arm926ejs/mx28/u-boot-spl.lds"
#define	CONFIG_SPL_LIBCOMMON_SUPPORT
#define	CONFIG_SPL_LIBGENERIC_SUPPORT
#define	CONFIG_SPL_GPIO_SUPPORT

/*
 * U-Boot Commands
 */
#include <config_cmd_default.h>
#define	CONFIG_DISPLAY_CPUINFO
#define	CONFIG_DOS_PARTITION

#define	CONFIG_CMD_CACHE
#define	CONFIG_CMD_DATE
#define	CONFIG_CMD_DHCP
#define	CONFIG_CMD_EEPROM
#define	CONFIG_CMD_EXT2
#define	CONFIG_CMD_FAT
#define	CONFIG_CMD_GPIO
#define	CONFIG_CMD_I2C
#define	CONFIG_CMD_MII
#define	CONFIG_CMD_MMC
#define	CONFIG_CMD_NAND
#define	CONFIG_CMD_NET
#define	CONFIG_CMD_NFS
#define	CONFIG_CMD_PING
#define	CONFIG_CMD_SETEXPR
#define	CONFIG_CMD_SF
#define	CONFIG_CMD_SPI
#define	CONFIG_CMD_USB

/*
 * Memory configurations
 */
#define	CONFIG_NR_DRAM_BANKS		1		/* 1 bank of DRAM */
#define	PHYS_SDRAM_1			0x40000000	/* Base address */
#define	PHYS_SDRAM_1_SIZE		0x20000000	/* Max 512 MB RAM */
#define	CONFIG_STACKSIZE		(128 * 1024)	/* 128 KB stack */
#define	CONFIG_SYS_MALLOC_LEN		0x00400000	/* 4 MB for malloc */
#define	CONFIG_SYS_GBL_DATA_SIZE	128		/* Initial data */
#define	CONFIG_SYS_MEMTEST_START	0x40000000	/* Memtest start adr */
#define	CONFIG_SYS_MEMTEST_END		0x40400000	/* 4 MB RAM test */
#define	CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1
/* Point initial SP in SRAM so SPL can use it too. */

#define CONFIG_SYS_INIT_RAM_ADDR	0x00000000
#define CONFIG_SYS_INIT_RAM_SIZE	(128 * 1024)

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)
/*
 * We need to sacrifice first 4 bytes of RAM here to avoid triggering some
 * strange BUG in ROM corrupting first 4 bytes of RAM when loading U-Boot
 * binary. In case there was more of this mess, 0x100 bytes are skipped.
 */
#define	CONFIG_SYS_TEXT_BASE		0x40000100

/*
 * U-Boot general configurations
 */
#define	CONFIG_SYS_LONGHELP
#define	CONFIG_SYS_PROMPT	"=> "
#define	CONFIG_SYS_CBSIZE	1024		/* Console I/O buffer size */
#define	CONFIG_SYS_PBSIZE	\
	(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
						/* Print buffer size */
#define	CONFIG_SYS_MAXARGS	32		/* Max number of command args */
#define	CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE
						/* Boot argument buffer size */
#define	CONFIG_VERSION_VARIABLE			/* U-BOOT version */
#define	CONFIG_AUTO_COMPLETE			/* Command auto complete */
#define	CONFIG_CMDLINE_EDITING			/* Command history etc */
#define	CONFIG_SYS_HUSH_PARSER

/*
 * Serial Driver
 */
#define	CONFIG_PL011_SERIAL
#define	CONFIG_PL011_CLOCK		24000000
#define	CONFIG_PL01x_PORTS		{ (void *)MXS_UARTDBG_BASE }
#define	CONFIG_CONS_INDEX		0
#define	CONFIG_BAUDRATE			115200	/* Default baud rate */

/*
 * MMC Driver
 */
#ifdef	CONFIG_CMD_MMC
#define	CONFIG_MMC
#define	CONFIG_MMC_BOUNCE_BUFFER
#define	CONFIG_GENERIC_MMC
#define	CONFIG_MXS_MMC
#endif

/*
 * APBH DMA
 */
#define CONFIG_APBH_DMA

/*
 * NAND
 */
#define	CONFIG_ENV_SIZE			(16 * 1024)
#ifdef	CONFIG_CMD_NAND
#define	CONFIG_NAND_MXS
#define	CONFIG_SYS_MAX_NAND_DEVICE	1
#define	CONFIG_SYS_NAND_BASE		0x60000000
#define	CONFIG_SYS_NAND_5_ADDR_CYCLE

/* Environment is in NAND */
#define	CONFIG_ENV_IS_IN_NAND
#define	CONFIG_ENV_SIZE_REDUND		CONFIG_ENV_SIZE
#define	CONFIG_ENV_SECT_SIZE		(128 * 1024)
#define	CONFIG_ENV_RANGE		(512 * 1024)
#define	CONFIG_ENV_OFFSET		0x300000
#define	CONFIG_ENV_OFFSET_REDUND	\
		(CONFIG_ENV_OFFSET + CONFIG_ENV_RANGE)

#define	CONFIG_CMD_UBI
#define	CONFIG_CMD_UBIFS
#define	CONFIG_CMD_MTDPARTS
#define	CONFIG_RBTREE
#define	CONFIG_LZO
#define	CONFIG_MTD_DEVICE
#define	CONFIG_MTD_PARTITIONS
#define	MTDIDS_DEFAULT			"nand0=gpmi-nand.0"
#define	MTDPARTS_DEFAULT			\
	"mtdparts=gpmi-nand.0:"			\
		"3m(bootloader)ro,"		\
		"512k(environment),"		\
		"512k(redundant-environment),"	\
		"4m(kernel),"			\
		"-(filesystem)"
#else
#define	CONFIG_ENV_IS_NOWHERE
#endif

/*
 * Ethernet on SOC (FEC)
 */
#ifdef	CONFIG_CMD_NET
#define	CONFIG_ETHPRIME			"FEC0"
#define	CONFIG_FEC_MXC
#define	CONFIG_FEC_MXC_MULTI
#define	CONFIG_MII
#define	CONFIG_DISCOVER_PHY
#define	CONFIG_FEC_XCV_TYPE		RMII
#endif

/*
 * I2C
 */
#ifdef	CONFIG_CMD_I2C
#define	CONFIG_I2C_MXS
#define	CONFIG_HARD_I2C
#define	CONFIG_SYS_I2C_SPEED		400000
#endif

/*
 * EEPROM
 */
#ifdef	CONFIG_CMD_EEPROM
#define	CONFIG_SYS_I2C_MULTI_EEPROMS
#define	CONFIG_SYS_I2C_EEPROM_ADDR_LEN	2
#endif

/*
 * RTC
 */
#ifdef	CONFIG_CMD_DATE
/* Use the internal RTC in the MXS chip */
#define	CONFIG_RTC_INTERNAL
#ifdef	CONFIG_RTC_INTERNAL
#define	CONFIG_RTC_MXS
#else
#define	CONFIG_RTC_M41T62
#define	CONFIG_SYS_I2C_RTC_ADDR		0x68
#define	CONFIG_SYS_M41T11_BASE_YEAR	2000
#endif
#endif

/*
 * USB
 */
#ifdef	CONFIG_CMD_USB
#define	CONFIG_USB_EHCI
#define	CONFIG_USB_EHCI_MXS
#define	CONFIG_EHCI_MXS_PORT		1
#define	CONFIG_EHCI_IS_TDI
#define	CONFIG_USB_STORAGE
#endif

/*
 * SPI
 */
#ifdef	CONFIG_CMD_SPI
#define	CONFIG_HARD_SPI
#define	CONFIG_MXS_SPI
#define	CONFIG_SPI_HALF_DUPLEX
#define	CONFIG_DEFAULT_SPI_BUS		2
#define	CONFIG_DEFAULT_SPI_MODE		SPI_MODE_0

/* SPI FLASH */
#ifdef	CONFIG_CMD_SF
#define	CONFIG_SPI_FLASH
#define	CONFIG_SPI_FLASH_STMICRO
#define	CONFIG_SF_DEFAULT_CS		2
#define	CONFIG_SF_DEFAULT_MODE		SPI_MODE_0
#define	CONFIG_SF_DEFAULT_SPEED		24000000

#define	CONFIG_ENV_SPI_CS		0
#define	CONFIG_ENV_SPI_BUS		2
#define	CONFIG_ENV_SPI_MAX_HZ		24000000
#define	CONFIG_ENV_SPI_MODE		SPI_MODE_0
#endif
#endif

/*
 * Boot Linux
 */
#define	CONFIG_CMDLINE_TAG
#define	CONFIG_SETUP_MEMORY_TAGS
#define	CONFIG_BOOTDELAY	3
#define	CONFIG_BOOTFILE		"uImage"
#define	CONFIG_BOOTARGS		"console=ttyAM0,115200n8 "
#define	CONFIG_BOOTCOMMAND	"run bootcmd_net"
#define	CONFIG_LOADADDR		0x42000000
#define	CONFIG_SYS_LOAD_ADDR	CONFIG_LOADADDR
#define	CONFIG_OF_LIBFDT

/*
 * Extra Environments
 */
#define	CONFIG_EXTRA_ENV_SETTINGS					\
	"update_nand_full_filename=u-boot.nand\0"			\
	"update_nand_firmware_filename=u-boot.sb\0"			\
	"update_sd_firmware_filename=u-boot.sd\0"			\
	"update_nand_firmware_maxsz=0x100000\0"				\
	"update_nand_stride=0x40\0"	/* MX28 datasheet ch. 12.12 */	\
	"update_nand_count=0x4\0"	/* MX28 datasheet ch. 12.12 */	\
	"update_nand_get_fcb_size="	/* Get size of FCB blocks */	\
		"nand device 0 ; "					\
		"nand info ; "						\
		"setexpr fcb_sz ${update_nand_stride} * ${update_nand_count};" \
		"setexpr update_nand_fcb ${fcb_sz} * ${nand_writesize}\0" \
	"update_nand_full="		/* Update FCB, DBBT and FW */	\
		"if tftp ${update_nand_full_filename} ; then "		\
		"run update_nand_get_fcb_size ; "			\
		"nand scrub -y 0x0 ${filesize} ; "			\
		"nand write.raw ${loadaddr} 0x0 ${update_nand_fcb} ; "	\
		"setexpr update_off ${loadaddr} + ${update_nand_fcb} ; " \
		"setexpr update_sz ${filesize} - ${update_nand_fcb} ; " \
		"nand write ${update_off} ${update_nand_fcb} ${update_sz} ; " \
		"fi\0"							\
	"update_nand_firmware="		/* Update only firmware */	\
		"if tftp ${update_nand_firmware_filename} ; then "	\
		"run update_nand_get_fcb_size ; "			\
		"setexpr fcb_sz ${update_nand_fcb} * 2 ; " /* FCB + DBBT */ \
		"setexpr fw_sz ${update_nand_firmware_maxsz} * 2 ; "	\
		"setexpr fw_off ${fcb_sz} + ${update_nand_firmware_maxsz};" \
		"nand erase ${fcb_sz} ${fw_sz} ; "			\
		"nand write ${loadaddr} ${fcb_sz} ${filesize} ; "	\
		"nand write ${loadaddr} ${fw_off} ${filesize} ; "	\
		"fi\0"							\
	"update_sd_firmware="		/* Update the SD firmware partition */ \
		"if mmc rescan ; then "					\
		"if tftp ${update_sd_firmware_filename} ; then "	\
		"setexpr fw_sz ${filesize} / 0x200 ; "	/* SD block size */ \
		"setexpr fw_sz ${fw_sz} + 1 ; "				\
		"mmc write ${loadaddr} 0x800 ${fw_sz} ; "		\
		"fi ; "							\
		"fi\0"

#endif /* __M28EVK_CONFIG_H__ */
