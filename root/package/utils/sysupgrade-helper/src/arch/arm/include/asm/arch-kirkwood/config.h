/*
 * (C) Copyright 2011
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Lei Wen <leiwen@marvell.com>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

/*
 * This file should be included in board config header file.
 *
 * It supports common definitions for Kirkwood platform
 */

#ifndef _KW_CONFIG_H
#define _KW_CONFIG_H

#if defined (CONFIG_KW88F6281)
#include <asm/arch/kw88f6281.h>
#elif defined (CONFIG_KW88F6192)
#include <asm/arch/kw88f6192.h>
#else
#error "SOC Name not defined"
#endif /* CONFIG_KW88F6281 */

#include <asm/arch/kirkwood.h>
#define CONFIG_ARM926EJS	1	/* Basic Architecture */
#define CONFIG_SYS_CACHELINE_SIZE	32
				/* default Dcache Line length for kirkwood */
#define CONFIG_MD5	/* get_random_hex on krikwood needs MD5 support */
#define CONFIG_KIRKWOOD_EGIGA_INIT	/* Enable GbePort0/1 for kernel */
#define CONFIG_KIRKWOOD_RGMII_PAD_1V8	/* Set RGMII Pad voltage to 1.8V */
#define CONFIG_KIRKWOOD_PCIE_INIT       /* Enable PCIE Port0 for kernel */

/*
 * By default kwbimage.cfg from board specific folder is used
 * If for some board, different configuration file need to be used,
 * CONFIG_SYS_KWD_CONFIG should be defined in board specific header file
 */
#ifndef CONFIG_SYS_KWD_CONFIG
#define	CONFIG_SYS_KWD_CONFIG	$(SRCTREE)/$(CONFIG_BOARDDIR)/kwbimage.cfg
#endif /* CONFIG_SYS_KWD_CONFIG */

/* Kirkwood has 2k of Security SRAM, use it for SP */
#define CONFIG_SYS_INIT_SP_ADDR		0xC8012000
#define CONFIG_NR_DRAM_BANKS_MAX	2

#define CONFIG_I2C_MVTWSI_BASE	KW_TWSI_BASE
#define MV_UART_CONSOLE_BASE	KW_UART0_BASE
#define MV_SATA_BASE		KW_SATA_BASE
#define MV_SATA_PORT0_OFFSET	KW_SATA_PORT0_OFFSET
#define MV_SATA_PORT1_OFFSET	KW_SATA_PORT1_OFFSET

/*
 * NAND configuration
 */
#ifdef CONFIG_CMD_NAND
#define CONFIG_NAND_KIRKWOOD
#define CONFIG_SYS_NAND_BASE		0xD8000000	/* MV_DEFADR_NANDF */
#define NAND_ALLOW_ERASE_ALL		1
#endif

/*
 * SPI Flash configuration
 */
#ifdef CONFIG_CMD_SF
#define CONFIG_HARD_SPI			1
#define CONFIG_KIRKWOOD_SPI		1
#ifndef CONFIG_ENV_SPI_BUS
# define CONFIG_ENV_SPI_BUS		0
#endif
#ifndef CONFIG_ENV_SPI_CS
# define CONFIG_ENV_SPI_CS		0
#endif
#ifndef CONFIG_ENV_SPI_MAX_HZ
# define CONFIG_ENV_SPI_MAX_HZ		50000000
#endif
#endif

/*
 * Ethernet Driver configuration
 */
#ifdef CONFIG_CMD_NET
#define CONFIG_CMD_MII
#define CONFIG_NETCONSOLE	/* include NetConsole support   */
#define CONFIG_MII		/* expose smi ove miiphy interface */
#define CONFIG_MVGBE		/* Enable Marvell Gbe Controller Driver */
#define CONFIG_SYS_FAULT_ECHO_LINK_DOWN	/* detect link using phy */
#define CONFIG_ENV_OVERWRITE	/* ethaddr can be reprogrammed */
#define CONFIG_RESET_PHY_R	/* use reset_phy() to init mv8831116 PHY */
#endif /* CONFIG_CMD_NET */

/*
 * USB/EHCI
 */
#ifdef CONFIG_CMD_USB
#define CONFIG_USB_EHCI_MARVELL
#define CONFIG_EHCI_IS_TDI
#endif /* CONFIG_CMD_USB */

/*
 * IDE Support on SATA ports
 */
#ifdef CONFIG_CMD_IDE
#define __io
#define CONFIG_CMD_EXT2
#define CONFIG_MVSATA_IDE
#define CONFIG_IDE_PREINIT
#define CONFIG_MVSATA_IDE_USE_PORT1
/* Needs byte-swapping for ATA data register */
#define CONFIG_IDE_SWAP_IO
/* Data, registers and alternate blocks are at the same offset */
#define CONFIG_SYS_ATA_DATA_OFFSET	(0x0100)
#define CONFIG_SYS_ATA_REG_OFFSET	(0x0100)
#define CONFIG_SYS_ATA_ALT_OFFSET	(0x0100)
/* Each 8-bit ATA register is aligned to a 4-bytes address */
#define CONFIG_SYS_ATA_STRIDE		4
/* Controller supports 48-bits LBA addressing */
#define CONFIG_LBA48
/* CONFIG_CMD_IDE requires some #defines for ATA registers */
#define CONFIG_SYS_IDE_MAXBUS		2
#define CONFIG_SYS_IDE_MAXDEVICE	2
/* ATA registers base is at SATA controller base */
#define CONFIG_SYS_ATA_BASE_ADDR	MV_SATA_BASE
#endif /* CONFIG_CMD_IDE */

/*
 * I2C related stuff
 */
#ifdef CONFIG_CMD_I2C
#ifndef CONFIG_SOFT_I2C
#define CONFIG_I2C_MVTWSI
#endif
#define CONFIG_SYS_I2C_SLAVE		0x0
#define CONFIG_SYS_I2C_SPEED		100000
#endif

#endif /* _KW_CONFIG_H */
