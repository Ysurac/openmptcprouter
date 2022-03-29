/*
 * (C) Copyright 2010
 * Ilko Iliev <iliev@ronetix.at>
 * Asen Dimov <dimov@ronetix.at>
 * Ronetix GmbH <www.ronetix.at>
 *
 * (C) Copyright 2007-2008
 * Stelian Pop <stelian@popies.net>
 * Lead Tech Design <www.leadtechdesign.com>
 *
 * Configuation settings for the PM9G45 board.
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

/*
 * SoC must be defined first, before hardware.h is included.
 * In this case SoC is defined in boards.cfg.
 */
#include <asm/hardware.h>

#define CONFIG_PM9G45		1	/* It's an Ronetix PM9G45 */
#define CONFIG_SYS_AT91_CPU_NAME	"AT91SAM9G45"

#define MACH_TYPE_PM9G45	2672
#define CONFIG_MACH_TYPE	MACH_TYPE_PM9G45

/* ARM asynchronous clock */
#define CONFIG_SYS_AT91_MAIN_CLOCK	12000000 /* from 12 MHz crystal */
#define CONFIG_SYS_AT91_SLOW_CLOCK	32768		/* slow clock xtal */
#define CONFIG_SYS_HZ			1000
#define CONFIG_SYS_TEXT_BASE		0x73f00000

#define CONFIG_ARCH_CPU_INIT

#define CONFIG_CMDLINE_TAG	1	/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS 1
#define CONFIG_INITRD_TAG	1

#define CONFIG_SKIP_LOWLEVEL_INIT
#define CONFIG_BOARD_EARLY_INIT_F

/*
 * Hardware drivers
 */
#define CONFIG_AT91_GPIO	1
#define CONFIG_ATMEL_USART	1
#define CONFIG_USART_BASE		ATMEL_BASE_DBGU
#define	CONFIG_USART_ID			ATMEL_ID_SYS

#define CONFIG_SYS_USE_NANDFLASH	1

/* LED */
#define CONFIG_AT91_LED
#define	CONFIG_RED_LED		AT91_PIO_PORTD, 31 /* this is the user1 led */
#define	CONFIG_GREEN_LED	AT91_PIO_PORTD, 0 /* this is the user2 led */

#define CONFIG_BOOTDELAY	3

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE	1
#define CONFIG_BOOTP_BOOTPATH		1
#define CONFIG_BOOTP_GATEWAY		1
#define CONFIG_BOOTP_HOSTNAME		1

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>
#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_IMLS

#define CONFIG_CMD_CACHE
#define CONFIG_CMD_PING		1
#define CONFIG_CMD_DHCP		1
#define CONFIG_CMD_NAND		1
#define CONFIG_CMD_USB		1

#define CONFIG_CMD_JFFS2		1
#define CONFIG_JFFS2_CMDLINE		1
#define CONFIG_JFFS2_NAND		1
#define CONFIG_JFFS2_DEV		"nand0" /* NAND dev jffs2 lives on */
#define CONFIG_JFFS2_PART_OFFSET	0	/* start of jffs2 partition */
#define CONFIG_JFFS2_PART_SIZE		(256 * 1024 * 1024) /* partition */

/* SDRAM */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM			0x70000000
#define PHYS_SDRAM_SIZE			0x08000000	/* 128 megs */

/* NOR flash, not available */
#define CONFIG_SYS_NO_FLASH		1
#undef CONFIG_CMD_FLASH

/* NAND flash */
#ifdef CONFIG_CMD_NAND
#define CONFIG_NAND_ATMEL
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		0x40000000
#define CONFIG_SYS_NAND_DBW_8		1
/* our ALE is AD21 */
#define CONFIG_SYS_NAND_MASK_ALE	(1 << 21)
/* our CLE is AD22 */
#define CONFIG_SYS_NAND_MASK_CLE	(1 << 22)
#define CONFIG_SYS_NAND_ENABLE_PIN	AT91_PIO_PORTC, 14
#define CONFIG_SYS_NAND_READY_PIN	AT91_PIO_PORTD, 3

#endif

/* Ethernet */
#define CONFIG_MACB			1
#define CONFIG_RMII			1
#define CONFIG_NET_RETRY_COUNT		20
#define CONFIG_RESET_PHY_R		1

/* USB */
#define CONFIG_USB_ATMEL
#define CONFIG_USB_OHCI_NEW		1
#define CONFIG_DOS_PARTITION		1
#define CONFIG_SYS_USB_OHCI_CPU_INIT	1
#define CONFIG_SYS_USB_OHCI_REGS_BASE	0x00700000 /* _UHP_OHCI_BASE */
#define CONFIG_SYS_USB_OHCI_SLOT_NAME	"at91sam9g45"
#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS	2
#define CONFIG_USB_STORAGE		1

/* board specific(not enough SRAM) */
#define CONFIG_AT91SAM9G45_LCD_BASE	PHYS_SDRAM + 0xE00000

#define CONFIG_SYS_LOAD_ADDR		PHYS_SDRAM + 0x2000000 /* load addr */

#define CONFIG_SYS_MEMTEST_START	PHYS_SDRAM
#define CONFIG_SYS_MEMTEST_END		CONFIG_AT91SAM9G45_LCD_BASE

/* bootstrap + u-boot + env + linux in nandflash */
#define CONFIG_ENV_IS_IN_NAND		1
#define CONFIG_ENV_OFFSET		0x60000
#define CONFIG_ENV_OFFSET_REDUND	0x80000
#define CONFIG_ENV_SIZE			0x20000		/* 1 sector = 128 kB */
#define CONFIG_BOOTCOMMAND	"nand read 0x72000000 0x200000 0x200000; bootm"
#define CONFIG_BOOTARGS		"fbcon=rotate:3 console=tty0 " \
				"console=ttyS0,115200 " \
				"root=/dev/mtdblock4 " \
				"mtdparts=atmel_nand:128k(bootstrap)ro," \
				"256k(uboot)ro,1664k(env)," \
				"2M(linux)ro,-(root) rw " \
				"rootfstype=jffs2"

#define CONFIG_BAUDRATE			115200

#define CONFIG_SYS_PROMPT		"U-Boot> "
#define CONFIG_SYS_CBSIZE		256
#define CONFIG_SYS_MAXARGS		16
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_LONGHELP		1
#define CONFIG_CMDLINE_EDITING		1
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_HUSH_PARSER

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN		ROUND(3 * CONFIG_ENV_SIZE + 128*1024,\
					0x1000)

#define CONFIG_SYS_SDRAM_BASE	PHYS_SDRAM
#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_SDRAM_BASE + 0x1000 - \
				GENERATED_GBL_DATA_SIZE)

#define CONFIG_STACKSIZE		(32*1024)	/* regular stack */

#ifdef CONFIG_USE_IRQ
#error CONFIG_USE_IRQ not supported
#endif

#endif
