/*
 * (C) Copyright 2010
 * Matthias Weisser <weisserm@arcor.de>
 *
 * Configuation settings for the jadecpu board
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

#define CONFIG_MB86R0x
#define CONFIG_MB86R0x_IOCLK	get_bus_freq(0)
#define CONFIG_SYS_HZ		1000
#define CONFIG_SYS_TEXT_BASE	0x10000000

#define CONFIG_ARM926EJS	1	/* This is an ARM926EJS Core	*/
#undef CONFIG_USE_IRQ			/* we don't need IRQ/FIQ stuff	*/

#define CONFIG_USE_ARCH_MEMCPY
#define CONFIG_USE_ARCH_MEMSET

#define MACH_TYPE_JADECPU	2636

#define CONFIG_MACH_TYPE MACH_TYPE_JADECPU

/*
 * Environment settings
 */
#define CONFIG_EXTRA_ENV_SETTINGS \
	"gs_fast_boot=setenv bootdelay 5\0" \
	"gs_slow_boot=setenv bootdelay 10\0" \
	"bootcmd=dcache off; mw.l 0x40000000 0 1024; usb start;" \
		"fatls usb 0; fatload usb 0 0x40000000 jadecpu-init.bin;" \
		"bootelf 0x40000000\0" \
	""

#define CONFIG_CMDLINE_TAG	1	/* enable passing of ATAGs	*/
#define CONFIG_SETUP_MEMORY_TAGS 1
#define CONFIG_INITRD_TAG	1
#define CONFIG_BOARD_LATE_INIT

/*
 * Compressions
 */
#define CONFIG_LZO

/*
 * Hardware drivers
 */

/*
 * Serial
 */
#define CONFIG_SERIAL_MULTI
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE		(-4)
#define CONFIG_SYS_NS16550_CLK			get_bus_freq(0)
#define CONFIG_SYS_NS16550_COM1			0xfffe1000	/* UART 0 */
#define CONFIG_SYS_NS16550_COM2			0xfff50000	/* UART 2 */
#define CONFIG_SYS_NS16550_COM3			0xfff51000	/* UART 3 */
#define CONFIG_SYS_NS16550_COM4			0xfff43000	/* UART 4 */

#define CONFIG_CONS_INDEX	4

/*
 * Ethernet
 */
#define CONFIG_SMC911X
#define CONFIG_SMC911X_BASE	0x02000000
#define CONFIG_SMC911X_16_BIT

/*
 * Video
 */
#define CONFIG_VIDEO
#define CONFIG_VIDEO_MB86R0xGDC
#define CONFIG_SYS_WHITE_ON_BLACK
#define CONFIG_CFB_CONSOLE
#define CONFIG_SYS_CONSOLE_IS_IN_ENV
#define CONFIG_SYS_CONSOLE_ENV_OVERWRITE
#define CONFIG_VIDEO_LOGO
#define CONFIG_SPLASH_SCREEN
#define CONFIG_SPLASH_SCREEN_ALIGN
#define CONFIG_VIDEO_BMP_LOGO
#define CONFIG_VIDEO_BMP_GZIP
#define CONFIG_VIDEO_BMP_RLE8
#define CONFIG_SYS_VIDEO_LOGO_MAX_SIZE  (800*480 + 256*4 + 10*1024)
#define VIDEO_FB_16BPP_WORD_SWAP
#define VIDEO_KBD_INIT_FCT		0
#define VIDEO_TSTC_FCT			serial_tstc
#define VIDEO_GETC_FCT			serial_getc

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
#undef CONFIG_CMD_BDI
#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_IMLS
#undef CONFIG_CMD_LOADS
#undef CONFIG_CMD_SOURCE
#undef CONFIG_CMD_NFS
#undef CONFIG_CMD_XIMG

#define CONFIG_CMD_BMP
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_ELF
#define CONFIG_CMD_FAT
#define CONFIG_CMD_PING
#define CONFIG_CMD_USB
#define CONFIG_CMD_CACHE

#define CONFIG_SYS_HUSH_PARSER

/* USB */
#define CONFIG_USB_OHCI_NEW
#define CONFIG_SYS_USB_OHCI_REGS_BASE       0xFFF81000
#define CONFIG_SYS_USB_OHCI_SLOT_NAME       "mb86r0x"
#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS  1
#define CONFIG_USB_STORAGE
#define CONFIG_DOS_PARTITION

/* SDRAM */
#define CONFIG_NR_DRAM_BANKS	1
#define PHYS_SDRAM		0x40000000	/* Start address of DDRRAM */
#define PHYS_SDRAM_SIZE	0x08000000	/* 128 megs */

#define CONFIG_SYS_SDRAM_BASE	PHYS_SDRAM
#define CONFIG_SYS_INIT_SP_ADDR	0x01008000

/*
 * FLASH and environment organization
 */
#define CONFIG_SYS_FLASH_BASE		0x10000000
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_MAX_FLASH_SECT	256
#define CONFIG_SYS_MONITOR_BASE	CONFIG_SYS_FLASH_BASE

#define CONFIG_ENV_ADDR		(CONFIG_SYS_FLASH_BASE + 0x00040000)
#define CONFIG_ENV_IS_IN_FLASH		1
#define CONFIG_ENV_SECT_SIZE		(128 * 1024)
#define CONFIG_ENV_SIZE		(128 * 1024)

/*
 * CFI FLASH driver setup
 */
#define CONFIG_SYS_FLASH_CFI		1
#define CONFIG_FLASH_CFI_DRIVER	1
#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE	1	/* ~10x faster */

#define CONFIG_SYS_LOAD_ADDR		0x40000000	/* load address */

#define CONFIG_SYS_MEMTEST_START	(PHYS_SDRAM + (512*1024))
#define CONFIG_SYS_MEMTEST_END		(PHYS_SDRAM + PHYS_SDRAM_SIZE)

#define CONFIG_BAUDRATE		115200

#define CONFIG_SYS_PROMPT	"jade> "
#define CONFIG_SYS_CBSIZE	256
#define CONFIG_SYS_MAXARGS	16
#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE + \
				sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_LONGHELP	1
#define CONFIG_CMDLINE_EDITING	1

#define CONFIG_PREBOOT  ""

#define CONFIG_BOOTDELAY	5
#define CONFIG_AUTOBOOT_KEYED
#define CONFIG_AUTOBOOT_PROMPT "boot in %d s\n", bootdelay
#define CONFIG_AUTOBOOT_DELAY_STR	"delaygs"
#define CONFIG_AUTOBOOT_STOP_STR	"stopgs"

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN	(10 << 20)
#define CONFIG_SYS_MEM_TOP_HIDE	(4 << 20)

#define CONFIG_STACKSIZE	(32*1024)	/* regular stack */

/*
 * Clock reset generator init
 */
#define CONFIG_SYS_CRG_CRHA_INIT		0xffff
#define CONFIG_SYS_CRG_CRPA_INIT		0xffff
#define CONFIG_SYS_CRG_CRPB_INIT		0xfffe
#define CONFIG_SYS_CRG_CRHB_INIT		0xffff
#define CONFIG_SYS_CRG_CRAM_INIT		0xffef

/*
 * Memory controller settings
 */
#define CONFIG_SYS_MEMC_MCFMODE0_INIT	0x00000001	/* 16bit */
#define CONFIG_SYS_MEMC_MCFMODE2_INIT	0x00000001	/* 16bit */
#define CONFIG_SYS_MEMC_MCFMODE4_INIT	0x00000021	/* 16bit, Page*/
#define CONFIG_SYS_MEMC_MCFTIM0_INIT	0x16191008
#define CONFIG_SYS_MEMC_MCFTIM2_INIT	0x03061008
#define CONFIG_SYS_MEMC_MCFTIM4_INIT	0x03061804
#define CONFIG_SYS_MEMC_MCFAREA0_INIT	0x000000c0	/* 0x0c000000 1MB */
#define CONFIG_SYS_MEMC_MCFAREA2_INIT	0x00000020	/* 0x02000000 1MB */
#define CONFIG_SYS_MEMC_MCFAREA4_INIT	0x001f0000	/* 0x10000000 32 MB */

/*
 * DDR2 controller init settings
 */
#define CONFIG_SYS_DDR2_DRIMS_INIT	0x5555
#define CONFIG_SYS_CCNT_CDCRC_INIT_1	0x00000002
#define CONFIG_SYS_CCNT_CDCRC_INIT_2	0x00000003
#define CONFIG_SYS_DDR2_DRIC1_INIT	0x003f
#define CONFIG_SYS_DDR2_DRIC2_INIT	0x0000
#define CONFIG_SYS_DDR2_DRCA_INIT	0xc124	/* 512Mbit DDR2SDRAM x 2 */
#define CONFIG_SYS_DDR2_DRCM_INIT	0x0032
#define CONFIG_SYS_DDR2_DRCST1_INIT	0x3418
#define CONFIG_SYS_DDR2_DRCST2_INIT	0x6e32
#define CONFIG_SYS_DDR2_DRCR_INIT	0x0141
#define CONFIG_SYS_DDR2_DRCF_INIT	0x0002
#define CONFIG_SYS_DDR2_DRASR_INIT	0x0001
#define CONFIG_SYS_DDR2_DROBS_INIT	0x0001
#define CONFIG_SYS_DDR2_DROABA_INIT	0x0103
#define CONFIG_SYS_DDR2_DRIBSODT1_INIT	0x003F
#define CONFIG_SYS_DDR2_DROS_INIT	0x0001

/*
 * DRAM init sequence
 */

/* PALL Command */
#define CONFIG_SYS_DDR2_INIT_DRIC1_1	0x0017
#define CONFIG_SYS_DDR2_INIT_DRIC2_1	0x0400

/* EMR(2) command */
#define CONFIG_SYS_DDR2_INIT_DRIC1_2	0x0006
#define CONFIG_SYS_DDR2_INIT_DRIC2_2	0x0000

/* EMR(3) command */
#define CONFIG_SYS_DDR2_INIT_DRIC1_3	0x0007
#define CONFIG_SYS_DDR2_INIT_DRIC2_3	0x0000

/* EMR(1) command */
#define CONFIG_SYS_DDR2_INIT_DRIC1_4	0x0005
#define CONFIG_SYS_DDR2_INIT_DRIC2_4	0x0000

/* MRS command */
#define CONFIG_SYS_DDR2_INIT_DRIC1_5	0x0004
#define CONFIG_SYS_DDR2_INIT_DRIC2_5	0x0532

/* PALL command */
#define CONFIG_SYS_DDR2_INIT_DRIC1_6	0x0017
#define CONFIG_SYS_DDR2_INIT_DRIC2_6	0x0400

/* REF command 1 */
#define CONFIG_SYS_DDR2_INIT_DRIC1_7	0x000f
#define CONFIG_SYS_DDR2_INIT_DRIC2_7	0x0000

/* MRS command */
#define CONFIG_SYS_DDR2_INIT_DRIC1_8	0x0004
#define CONFIG_SYS_DDR2_INIT_DRIC2_8	0x0432

/* EMR(1) command */
#define CONFIG_SYS_DDR2_INIT_DRIC1_9	0x0005
#define CONFIG_SYS_DDR2_INIT_DRIC2_9	0x0380

/* EMR(1) command */
#define CONFIG_SYS_DDR2_INIT_DRIC1_10	0x0005
#define CONFIG_SYS_DDR2_INIT_DRIC2_10	0x0002

#ifdef CONFIG_USE_IRQ
#error CONFIG_USE_IRQ not supported
#endif

#endif	/* __CONFIG_H */
