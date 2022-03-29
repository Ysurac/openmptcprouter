/*
 * Configuation settings for the Renesas R7780MP board
 *
 * Copyright (C) 2007,2008 Nobuhiro Iwamatsu <iwamatsu@nigauri.org>
 * Copyright (C) 2008 Yusuke Goda <goda.yusuke@renesas.com>
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

#ifndef __R7780RP_H
#define __R7780RP_H

#undef DEBUG
#define CONFIG_SH		1
#define CONFIG_SH4A		1
#define CONFIG_CPU_SH7780	1
#define CONFIG_R7780MP		1
#define CONFIG_SYS_R7780MP_OLD_FLASH	1
#define __LITTLE_ENDIAN__ 1

/*
 * Command line configuration.
 */
#define CONFIG_CMD_SDRAM
#define CONFIG_CMD_FLASH
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_PCI
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_SAVEENV
#define CONFIG_CMD_NFS
#define CONFIG_CMD_IDE
#define CONFIG_CMD_EXT2
#define CONFIG_DOS_PARTITION

#define CONFIG_SCIF_CONSOLE	1
#define CONFIG_BAUDRATE		115200
#define CONFIG_CONS_SCIF0	1

#define CONFIG_BOOTDELAY	3
#define CONFIG_BOOTARGS		"console=ttySC0,115200"
#define CONFIG_ENV_OVERWRITE	1

/* check for keypress on bootdelay==0 */
/*#define CONFIG_ZERO_BOOTDELAY_CHECK*/

#define CONFIG_SYS_TEXT_BASE		0x0FFC0000
#define CONFIG_SYS_SDRAM_BASE		(0x08000000)
#define CONFIG_SYS_SDRAM_SIZE		(128 * 1024 * 1024)

#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_PROMPT		"=> "
#define CONFIG_SYS_CBSIZE		256
#define CONFIG_SYS_PBSIZE		256
#define CONFIG_SYS_MAXARGS		16
#define CONFIG_SYS_BARGSIZE	512

#define CONFIG_SYS_MEMTEST_START	(CONFIG_SYS_SDRAM_BASE)
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_TEXT_BASE - 0x100000)

/* Flash board support */
#define CONFIG_SYS_FLASH_BASE		(0xA0000000)
#ifdef CONFIG_SYS_R7780MP_OLD_FLASH
/* NOR Flash (S29PL127J60TFI130) */
# define CONFIG_SYS_FLASH_CFI_WIDTH	FLASH_CFI_32BIT
# define CONFIG_SYS_MAX_FLASH_BANKS	(2)
# define CONFIG_SYS_MAX_FLASH_SECT	270
# define CONFIG_SYS_FLASH_BANKS_LIST	{ CONFIG_SYS_FLASH_BASE,\
				CONFIG_SYS_FLASH_BASE + 0x100000,\
				CONFIG_SYS_FLASH_BASE + 0x400000,\
				CONFIG_SYS_FLASH_BASE + 0x700000, }
#else /* CONFIG_SYS_R7780MP_OLD_FLASH */
/* NOR Flash (Spantion S29GL256P) */
# define CONFIG_SYS_MAX_FLASH_BANKS	(1)
# define CONFIG_SYS_MAX_FLASH_SECT		256
# define CONFIG_SYS_FLASH_BANKS_LIST	{ CONFIG_SYS_FLASH_BASE }
#endif /* CONFIG_SYS_R7780MP_OLD_FLASH */

#define CONFIG_SYS_LOAD_ADDR		(CONFIG_SYS_SDRAM_BASE + 4 * 1024 * 1024)
/* Address of u-boot image in Flash */
#define CONFIG_SYS_MONITOR_BASE	(CONFIG_SYS_FLASH_BASE)
#define CONFIG_SYS_MONITOR_LEN		(256 * 1024)
/* Size of DRAM reserved for malloc() use */
#define CONFIG_SYS_MALLOC_LEN		(1204 * 1024)

#define CONFIG_SYS_BOOTMAPSZ		(8 * 1024 * 1024)
#define CONFIG_SYS_RX_ETH_BUFFER	(8)

#define CONFIG_SYS_FLASH_CFI
#define CONFIG_FLASH_CFI_DRIVER
#undef CONFIG_SYS_FLASH_CFI_BROKEN_TABLE
#undef  CONFIG_SYS_FLASH_QUIET_TEST
/* print 'E' for empty sector on flinfo */
#define CONFIG_SYS_FLASH_EMPTY_INFO

#define CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_SECT_SIZE	(256 * 1024)
#define CONFIG_ENV_SIZE		(CONFIG_ENV_SECT_SIZE)
#define CONFIG_ENV_ADDR		(CONFIG_SYS_MONITOR_BASE + CONFIG_SYS_MONITOR_LEN)
#define CONFIG_SYS_FLASH_ERASE_TOUT	120000
#define CONFIG_SYS_FLASH_WRITE_TOUT	500

/* Board Clock */
#define CONFIG_SYS_CLK_FREQ	33333333
#define CONFIG_SYS_TMU_CLK_DIV		4
#define CONFIG_SYS_HZ		1000

/* PCI Controller */
#if defined(CONFIG_CMD_PCI)
#define CONFIG_PCI
#define CONFIG_SH4_PCI
#define CONFIG_SH7780_PCI
#define CONFIG_SH7780_PCI_LSR	0x07f00001
#define CONFIG_SH7780_PCI_LAR	CONFIG_SYS_SDRAM_SIZE
#define CONFIG_SH7780_PCI_BAR	CONFIG_SYS_SDRAM_SIZE
#define CONFIG_PCI_PNP
#define CONFIG_PCI_SCAN_SHOW	1
#define __io
#define __mem_pci

#define CONFIG_PCI_MEM_BUS	0xFD000000	/* Memory space base addr */
#define CONFIG_PCI_MEM_PHYS	CONFIG_PCI_MEM_BUS
#define CONFIG_PCI_MEM_SIZE	0x01000000	/* Size of Memory window */

#define CONFIG_PCI_IO_BUS	0xFE200000	/* IO space base address */
#define CONFIG_PCI_IO_PHYS	CONFIG_PCI_IO_BUS
#define CONFIG_PCI_IO_SIZE	0x00200000	/* Size of IO window */
#define CONFIG_PCI_SYS_PHYS CONFIG_SYS_SDRAM_BASE
#define CONFIG_PCI_SYS_BUS  CONFIG_SYS_SDRAM_BASE
#define CONFIG_PCI_SYS_SIZE CONFIG_SYS_SDRAM_SIZE
#endif /* CONFIG_CMD_PCI */

#if defined(CONFIG_CMD_NET)
/*
#define CONFIG_RTL8169
*/
/* AX88796L Support(NE2000 base chip) */
#define CONFIG_DRIVER_AX88796L
#define CONFIG_DRIVER_NE2000_BASE	0xA4100000
#endif

/* Compact flash Support */
#if defined(CONFIG_CMD_IDE)
#define CONFIG_IDE_RESET        1
#define CONFIG_SYS_PIO_MODE            1
#define CONFIG_SYS_IDE_MAXBUS          1   /* IDE bus */
#define CONFIG_SYS_IDE_MAXDEVICE       1
#define CONFIG_SYS_ATA_BASE_ADDR       0xb4000000
#define CONFIG_SYS_ATA_STRIDE          2               /* 1bit shift */
#define CONFIG_SYS_ATA_DATA_OFFSET     0x1000          /* data reg offset */
#define CONFIG_SYS_ATA_REG_OFFSET      0x1000          /* reg offset */
#define CONFIG_SYS_ATA_ALT_OFFSET      0x800           /* alternate register offset */
#define CONFIG_IDE_SWAP_IO
#endif /* CONFIG_CMD_IDE */

#endif /* __R7780RP_H */
