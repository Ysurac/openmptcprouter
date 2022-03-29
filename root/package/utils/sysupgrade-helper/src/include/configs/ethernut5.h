/*
 * (C) Copyright 2011
 * egnite GmbH <info@egnite.de>
 *
 * Configuation settings for Ethernut 5 with AT91SAM9XE.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/hardware.h>

/* The first stage boot loader expects u-boot running at this address. */
#define CONFIG_SYS_TEXT_BASE	0x27000000	/* 16MB available */

/* The first stage boot loader takes care of low level initialization. */
#define CONFIG_SKIP_LOWLEVEL_INIT

/* Set our official architecture number. */
#define MACH_TYPE_ETHERNUT5 1971
#define CONFIG_MACH_TYPE MACH_TYPE_ETHERNUT5

/* CPU information */
#define CONFIG_ARM926EJS
#define CONFIG_AT91FAMILY
#define CONFIG_DISPLAY_CPUINFO		/* Display at console. */
#define CONFIG_ARCH_CPU_INIT

/* ARM asynchronous clock */
#define CONFIG_SYS_AT91_SLOW_CLOCK	32768	/* slow clock xtal */
#define CONFIG_SYS_AT91_MAIN_CLOCK	18432000 /* 18.432 MHz crystal */
#define CONFIG_SYS_HZ			1000
#undef CONFIG_USE_IRQ			/* Running w/o interrupts */

/* 32kB internal SRAM */
#define CONFIG_SRAM_BASE	0x00300000 /*AT91SAM9XE_SRAM_BASE */
#define CONFIG_SRAM_SIZE	(32 << 10)
#define CONFIG_STACKSIZE	(CONFIG_SRAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SRAM_BASE + CONFIG_STACKSIZE)

/* 128MB SDRAM in 1 bank */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_SDRAM_BASE		0x20000000
#define CONFIG_SYS_SDRAM_SIZE		(128 << 20)
#define CONFIG_SYS_LOAD_ADDR		CONFIG_SYS_SDRAM_BASE
#define CONFIG_LOADADDR			CONFIG_SYS_LOAD_ADDR
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (1 << 20))
#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_TEXT_BASE \
					- CONFIG_SYS_MALLOC_LEN)

/* 512kB on-chip NOR flash */
# define CONFIG_SYS_MAX_FLASH_BANKS	1
# define CONFIG_SYS_FLASH_BASE		0x00200000 /* AT91SAM9XE_FLASH_BASE */
# define CONFIG_AT91_EFLASH
# define CONFIG_SYS_MAX_FLASH_SECT	32
# define CONFIG_SYS_FLASH_PROTECTION	/* First stage loader in sector 0 */
# define CONFIG_EFLASH_PROTSECTORS	1

/* 512kB DataFlash at NPCS0 */
#define CONFIG_SYS_MAX_DATAFLASH_BANKS	1
#define CONFIG_HAS_DATAFLASH
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_ATMEL
#define CONFIG_ATMEL_DATAFLASH_SPI
#define CONFIG_SYS_DATAFLASH_LOGIC_ADDR_CS0	0xC0000000
#define DATAFLASH_TCSS			(0x1a << 16)
#define DATAFLASH_TCHS			(0x1 << 24)

#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_OFFSET		0x3DE000
#define CONFIG_ENV_SECT_SIZE		(132 << 10)
#define CONFIG_ENV_SIZE			CONFIG_ENV_SECT_SIZE
#define CONFIG_ENV_ADDR			(CONFIG_SYS_DATAFLASH_LOGIC_ADDR_CS0 \
					+ CONFIG_ENV_OFFSET)
#define CONFIG_SYS_MONITOR_BASE		(CONFIG_SYS_DATAFLASH_LOGIC_ADDR_CS0 \
					+ 0x042000)

/* SPI */
#define CONFIG_ATMEL_SPI
#define CONFIG_SYS_SPI_WRITE_TOUT	(5 * CONFIG_SYS_HZ)
#define AT91_SPI_CLK			15000000

/* Serial port */
#define CONFIG_ATMEL_USART
#define CONFIG_USART3			/* USART 3 is DBGU */
#define CONFIG_BAUDRATE			115200
#define CONFIG_USART_BASE		ATMEL_BASE_DBGU
#define	CONFIG_USART_ID			ATMEL_ID_SYS

/* Misc. hardware drivers */
#define CONFIG_AT91_GPIO

/* Command line configuration */
#include <config_cmd_default.h>
#undef CONFIG_CMD_BDI
#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_LOADS

#define CONFIG_CMD_JFFS2
#define CONFIG_CMD_MII
#define CONFIG_CMD_MTDPARTS
#define CONFIG_CMD_NAND
#define CONFIG_CMD_SPI

#ifdef MINIMAL_LOADER
#undef CONFIG_CMD_CONSOLE
#undef CONFIG_CMD_EDITENV
#undef CONFIG_CMD_IMI
#undef CONFIG_CMD_ITEST
#undef CONFIG_CMD_IMLS
#undef CONFIG_CMD_LOADB
#undef CONFIG_CMD_LOADS
#undef CONFIG_CMD_NFS
#undef CONFIG_CMD_SETGETDCR
#undef CONFIG_CMD_XIMG
#else
#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_BSP
#define CONFIG_CMD_CACHE
#define CONFIG_CMD_CDP
#define CONFIG_CMD_DATE
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_DNS
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT
#define CONFIG_CMD_I2C
#define CONFIG_CMD_MMC
#define CONFIG_CMD_PING
#define CONFIG_CMD_RARP
#define CONFIG_CMD_REISER
#define CONFIG_CMD_SAVES
#define CONFIG_CMD_SETEXPR
#define CONFIG_CMD_SF
#define CONFIG_CMD_SNTP
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_CMD_UNZIP
#define CONFIG_CMD_USB
#endif

/* NAND flash */
#ifdef CONFIG_CMD_NAND
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		0x40000000
#define CONFIG_SYS_NAND_DBW_8
#define CONFIG_NAND_ATMEL
/* our ALE is AD21 */
#define CONFIG_SYS_NAND_MASK_ALE	(1 << 21)
/* our CLE is AD22 */
#define CONFIG_SYS_NAND_MASK_CLE	(1 << 22)
#define CONFIG_SYS_NAND_ENABLE_PIN	AT91_PIO_PORTC, 14
#endif

/* JFFS2 */
#ifdef CONFIG_CMD_JFFS2
#define CONFIG_MTD_NAND_ECC_JFFS2
#define CONFIG_JFFS2_CMDLINE
#define CONFIG_JFFS2_NAND
#endif

/* Ethernet */
#define CONFIG_NET_RETRY_COUNT		20
#define CONFIG_MACB
#define CONFIG_RMII
#define CONFIG_PHY_ID			0
#define CONFIG_MACB_SEARCH_PHY

/* MMC */
#ifdef CONFIG_CMD_MMC
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_GENERIC_ATMEL_MCI
#define CONFIG_SYS_MMC_CD_PIN		AT91_PIO_PORTC, 8
#endif

/* USB */
#ifdef CONFIG_CMD_USB
#define CONFIG_USB_ATMEL
#define CONFIG_USB_OHCI_NEW
#define CONFIG_SYS_USB_OHCI_CPU_INIT
#define CONFIG_SYS_USB_OHCI_REGS_BASE	0x00500000
#define CONFIG_SYS_USB_OHCI_SLOT_NAME	"host"
#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS	2
#define CONFIG_USB_STORAGE
#endif

/* RTC */
#if defined(CONFIG_CMD_DATE) || defined(CONFIG_CMD_SNTP)
#define CONFIG_RTC_PCF8563
#define CONFIG_SYS_I2C_RTC_ADDR		0x51
#endif

/* I2C */
#define CONFIG_SYS_MAX_I2C_BUS	1
#define CONFIG_SYS_I2C_SLAVE	0
#define CONFIG_SYS_I2C_SPEED	100000

#define CONFIG_SOFT_I2C
#define I2C_SOFT_DECLARATIONS

#define GPIO_I2C_SCL		AT91_PIO_PORTA, 24
#define GPIO_I2C_SDA		AT91_PIO_PORTA, 23

#define I2C_INIT { \
	at91_set_pio_periph(AT91_PIO_PORTA, 23, 0); \
	at91_set_pio_multi_drive(AT91_PIO_PORTA, 23, 1); \
	at91_set_pio_periph(AT91_PIO_PORTA, 24, 0); \
	at91_set_pio_output(AT91_PIO_PORTA, 24, 0); \
	at91_set_pio_multi_drive(AT91_PIO_PORTA, 24, 1); \
}

#define I2C_ACTIVE	at91_set_pio_output(AT91_PIO_PORTA, 23, 0)
#define I2C_TRISTATE	at91_set_pio_input(AT91_PIO_PORTA, 23, 0)
#define I2C_SCL(bit)	at91_set_pio_value(AT91_PIO_PORTA, 24, bit)
#define I2C_SDA(bit)	at91_set_pio_value(AT91_PIO_PORTA, 23, bit)
#define I2C_DELAY	udelay(100)
#define I2C_READ	at91_get_pio_value(AT91_PIO_PORTA, 23)

/* DHCP/BOOTP options */
#ifdef CONFIG_CMD_DHCP
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME
#define CONFIG_SYS_AUTOLOAD	"n"
#endif

/* File systems */
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#if defined(CONFIG_CMD_MTDPARTS) || defined(CONFIG_CMD_NAND)
#define MTDIDS_DEFAULT		"nand0=atmel_nand"
#define MTDPARTS_DEFAULT	"mtdparts=atmel_nand:-(root)"
#endif
#if defined(CONFIG_CMD_REISER) || defined(CONFIG_CMD_EXT2) || \
	defined(CONFIG_CMD_USB) || defined(CONFIG_MMC)
#define CONFIG_DOS_PARTITION
#endif
#define CONFIG_LZO
#define CONFIG_RBTREE

/* Boot command */
#define CONFIG_BOOTDELAY	3
#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_BOOTCOMMAND	"cp.b 0xC00C6000 ${loadaddr} 0x294000; bootm"
#if defined(CONFIG_CMD_NAND)
#define CONFIG_BOOTARGS		"console=ttyS0,115200 " \
				"root=/dev/mtdblock0 " \
				MTDPARTS_DEFAULT \
				" rw rootfstype=jffs2"
#endif

/* Misc. u-boot settings */
#define CONFIG_SYS_PROMPT		"U-Boot> "
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_CBSIZE		256
#define CONFIG_SYS_MAXARGS		16
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + 16 \
					+ sizeof(CONFIG_SYS_PROMPT))
#define CONFIG_SYS_LONGHELP
#define CONFIG_CMDLINE_EDITING

#endif
