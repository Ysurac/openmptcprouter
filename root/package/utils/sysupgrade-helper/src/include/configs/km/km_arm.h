/*
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Prafulla Wadaskar <prafulla@marvell.com>
 *
 * (C) Copyright 2009
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
 *
 * (C) Copyright 2010-2011
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

/*
 * for linking errors see
 * http://lists.denx.de/pipermail/u-boot/2009-July/057350.html
 */

#ifndef _CONFIG_KM_ARM_H
#define _CONFIG_KM_ARM_H

/* We got removed from Linux mach-types.h */
#define MACH_TYPE_KM_KIRKWOOD          2255

/*
 * High Level Configuration Options (easy to change)
 */
#define CONFIG_MARVELL
#define CONFIG_FEROCEON_88FR131		/* CPU Core subversion */
#define CONFIG_KIRKWOOD			/* SOC Family Name */
#define CONFIG_KW88F6281		/* SOC Name */
#define CONFIG_MACH_KM_KIRKWOOD		/* Machine type */

#define CONFIG_MACH_TYPE	MACH_TYPE_KM_KIRKWOOD

/* include common defines/options for all Keymile boards */
#include "keymile-common.h"

#define CONFIG_CMD_NAND
#define CONFIG_CMD_SF
#define CONFIG_SOFT_I2C		/* I2C bit-banged	*/

#if defined CONFIG_KM_ENV_IS_IN_SPI_NOR
#define CONFIG_ENV_SPI_BUS		0
#define CONFIG_ENV_SPI_CS		0
#define CONFIG_ENV_SPI_MAX_HZ		5000000
#define CONFIG_ENV_SPI_MODE		SPI_MODE_3
#endif

#include "asm/arch/config.h"

#define CONFIG_SYS_TEXT_BASE	0x07d00000	/* code address before reloc */
#define CONFIG_SYS_MEMTEST_START 0x00400000	/* 4M */
#define CONFIG_SYS_MEMTEST_END	0x007fffff	/*(_8M -1) */
#define CONFIG_SYS_LOAD_ADDR	0x00800000	/* default load adr- 8M */

/* pseudo-non volatile RAM [hex] */
#define CONFIG_KM_PNVRAM	0x80000
/* physical RAM MTD size [hex] */
#define CONFIG_KM_PHRAM		0x17F000

#define CONFIG_KM_CRAMFS_ADDR	0x2400000
#define CONFIG_KM_KERNEL_ADDR	0x2000000	/* 4096KBytes */

/* architecture specific default bootargs */
#define CONFIG_KM_DEF_BOOT_ARGS_CPU					\
		"bootcountaddr=${bootcountaddr} ${mtdparts}"		\
		" boardid=0x${IVM_BoardId} hwkey=0x${IVM_HWKey}"

#define CONFIG_KM_DEF_ENV_CPU						\
	"boot=bootm ${load_addr_r} - -\0"				\
	"cramfsloadfdt=true\0"						\
	"u-boot="xstr(CONFIG_HOSTNAME) "/u-boot.kwb\0"			\
	CONFIG_KM_UPDATE_UBOOT						\
	""

#define CONFIG_SKIP_LOWLEVEL_INIT	/* disable board lowlevel_init */
#define CONFIG_MISC_INIT_R

/*
 * NS16550 Configuration
 */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
#define CONFIG_SYS_NS16550_CLK		CONFIG_SYS_TCLK
#define CONFIG_SYS_NS16550_COM1		KW_UART0_BASE
#define CONFIG_SYS_NS16550_COM2		KW_UART1_BASE

/*
 * Serial Port configuration
 * The following definitions let you select what serial you want to use
 * for your console driver.
 */

#define CONFIG_CONS_INDEX	1	/* Console on UART0 */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_BOOTMAPSZ	(8 << 20)	/* Initial Memmap for Linux */
#define CONFIG_CMDLINE_TAG		/* enable passing of ATAGs  */
#define CONFIG_INITRD_TAG		/* enable INITRD tag */
#define CONFIG_SETUP_MEMORY_TAGS	/* enable memory tag */

/*
 * Commands configuration
 */
#define CONFIG_CMD_ELF
#define CONFIG_CMD_MTDPARTS
#define CONFIG_CMD_NFS

/*
 * Without NOR FLASH we need this
 */
#define CONFIG_SYS_NO_FLASH
#undef CONFIG_CMD_FLASH
#undef CONFIG_CMD_IMLS

/*
 * NAND Flash configuration
 */
#define CONFIG_SYS_MAX_NAND_DEVICE	1

#define BOOTFLASH_START		0x0

/* Kirkwood has two serial IF */
#if (CONFIG_CONS_INDEX == 2)
#define CONFIG_KM_CONSOLE_TTY	"ttyS1"
#else
#define CONFIG_KM_CONSOLE_TTY	"ttyS0"
#endif

/*
 * Other required minimal configurations
 */
#define CONFIG_CONSOLE_INFO_QUIET	/* some code reduction */
#define CONFIG_ARCH_CPU_INIT		/* call arch_cpu_init() */
#define CONFIG_ARCH_MISC_INIT		/* call arch_misc_init() */
#define CONFIG_DISPLAY_CPUINFO		/* Display cpu info */
#define CONFIG_NR_DRAM_BANKS	4
#define CONFIG_STACKSIZE	0x00100000	/* regular stack- 1M */
#define CONFIG_SYS_RESET_ADDRESS 0xffff0000	/* Rst Vector Adr */

/*
 * Ethernet Driver configuration
 */
#define CONFIG_NETCONSOLE	/* include NetConsole support   */
#define CONFIG_MII		/* expose smi ove miiphy interface */
#define CONFIG_CMD_MII		/* to debug mdio phy config */
#define CONFIG_MVGBE		/* Enable Marvell Gbe Controller Driver */
#define CONFIG_SYS_FAULT_ECHO_LINK_DOWN	/* detect link using phy */
#define CONFIG_MVGBE_PORTS	{1, 0}	/* enable port 0 only */
#define CONFIG_PHY_BASE_ADR	0
#define CONFIG_ENV_OVERWRITE	/* ethaddr can be reprogrammed */

/*
 * UBI related stuff
 */
#define CONFIG_SYS_USE_UBI

/*
 * I2C related stuff
 */
#define	CONFIG_KIRKWOOD_GPIO		/* Enable GPIO Support */
#if defined(CONFIG_SOFT_I2C)
#ifndef __ASSEMBLY__
#include <asm/arch-kirkwood/gpio.h>
extern void __set_direction(unsigned pin, int high);
void set_sda(int state);
void set_scl(int state);
int get_sda(void);
int get_scl(void);
#define KM_KIRKWOOD_SDA_PIN	8
#define KM_KIRKWOOD_SCL_PIN	9
#define KM_KIRKWOOD_SOFT_I2C_GPIOS	0x0300
#define KM_KIRKWOOD_ENV_WP	38

#define I2C_ACTIVE	__set_direction(KM_KIRKWOOD_SDA_PIN, 0)
#define I2C_TRISTATE	__set_direction(KM_KIRKWOOD_SDA_PIN, 1)
#define I2C_READ	(kw_gpio_get_value(KM_KIRKWOOD_SDA_PIN) ? 1 : 0)
#define I2C_SDA(bit)	kw_gpio_set_value(KM_KIRKWOOD_SDA_PIN, bit)
#define I2C_SCL(bit)	kw_gpio_set_value(KM_KIRKWOOD_SCL_PIN, bit)
#endif

#define I2C_DELAY	udelay(1)
#define I2C_SOFT_DECLARATIONS

#endif

/* EEprom support 24C128, 24C256 valid for environment eeprom */
#define CONFIG_SYS_I2C_MULTI_EEPROMS
#define CONFIG_SYS_EEPROM_PAGE_WRITE_ENABLE
#define CONFIG_SYS_EEPROM_PAGE_WRITE_BITS	6 /* 64 Byte write page */
#define CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS	10

#define CONFIG_SYS_I2C_EEPROM_ADDR	0x50
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN	2

/*
 *  Environment variables configurations
 */
#if defined CONFIG_KM_ENV_IS_IN_SPI_NOR
#define CONFIG_ENV_IS_IN_SPI_FLASH  /* use SPI-Flash for environment vars */
#define CONFIG_ENV_OFFSET		0xc0000     /* no bracets! */
#define CONFIG_ENV_SIZE			0x02000     /* Size of Environment */
#define CONFIG_ENV_SECT_SIZE		0x10000
#define CONFIG_ENV_OFFSET_REDUND	(CONFIG_ENV_OFFSET + \
					CONFIG_ENV_SECT_SIZE)
#define CONFIG_ENV_TOTAL_SIZE		0x20000     /* no bracets! */
#else
#define CONFIG_ENV_IS_IN_EEPROM		/* use EEPROM for environment vars */
#define CONFIG_SYS_DEF_EEPROM_ADDR	0x50
#define CONFIG_ENV_EEPROM_IS_ON_I2C
#define CONFIG_SYS_EEPROM_WREN
#define CONFIG_ENV_OFFSET		0x0 /* no bracets! */
#define CONFIG_ENV_SIZE			(0x2000 - CONFIG_ENV_OFFSET)
#define CONFIG_I2C_ENV_EEPROM_BUS	KM_ENV_BUS "\0"
#define CONFIG_ENV_OFFSET_REDUND	0x2000 /* no bracets! */
#define CONFIG_ENV_SIZE_REDUND		(CONFIG_ENV_SIZE)
#endif

#define CONFIG_SYS_REDUNDAND_ENVIRONMENT

#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_STMICRO

/* SPI bus claim MPP configuration */
#define CONFIG_SYS_KW_SPI_MPP	0x0

#define FLASH_GPIO_PIN			0x00010000
#define KM_FLASH_GPIO_PIN	16

#ifndef MTDIDS_DEFAULT
# define MTDIDS_DEFAULT		"nand0=orion_nand"
#endif /* MTDIDS_DEFAULT */

#ifndef MTDPARTS_DEFAULT
# define MTDPARTS_DEFAULT	"mtdparts="			\
	"orion_nand:"						\
		"-(" CONFIG_KM_UBI_PARTITION_NAME_BOOT ");"
#endif /* MTDPARTS_DEFAULT */

#define	CONFIG_KM_UPDATE_UBOOT						\
	"update="							\
		"sf probe 0;sf erase 0 +${filesize};"			\
		"sf write ${load_addr_r} 0 ${filesize};\0"

#if defined CONFIG_KM_ENV_IS_IN_SPI_NOR
#define CONFIG_KM_NEW_ENV						\
	"newenv=sf probe 0;"						\
		"sf erase " xstr(CONFIG_ENV_OFFSET) " "			\
		xstr(CONFIG_ENV_TOTAL_SIZE)"\0"
#else
#define CONFIG_KM_NEW_ENV						\
	"newenv=setenv addr 0x100000 && "				\
		"i2c dev 1; mw.b ${addr} 0 4 && "			\
		"eeprom write " xstr(CONFIG_SYS_DEF_EEPROM_ADDR)	\
		" ${addr} " xstr(CONFIG_ENV_OFFSET) " 4 && "		\
		"eeprom write " xstr(CONFIG_SYS_DEF_EEPROM_ADDR)	\
		" ${addr} " xstr(CONFIG_ENV_OFFSET_REDUND) " 4\0"
#endif

/*
 * Default environment variables
 */
#define CONFIG_EXTRA_ENV_SETTINGS					\
	CONFIG_KM_DEF_ENV						\
	CONFIG_KM_NEW_ENV						\
	"arch=arm\0"							\
	"EEprom_ivm=" KM_IVM_BUS "\0"					\
	""

#if defined(CONFIG_SYS_NO_FLASH)
#undef	CONFIG_FLASH_CFI_MTD
#undef	CONFIG_JFFS2_CMDLINE
#endif

/* additions for new relocation code, must be added to all boards */
#define CONFIG_SYS_SDRAM_BASE		0x00000000
/* Do early setups now in board_init_f() */
#define CONFIG_BOARD_EARLY_INIT_F

/*
 * resereved pram area at the end of memroy [hex]
 * 8Mbytes for switch + 4Kbytes for bootcount
 */
#define CONFIG_KM_RESERVED_PRAM 0x801000
/* address for the bootcount (taken from end of RAM) */
#define BOOTCOUNT_ADDR          (CONFIG_KM_RESERVED_PRAM)

/* enable POST tests */
#define CONFIG_POST	(CONFIG_SYS_POST_MEM_REGIONS)
#define CONFIG_POST_SKIP_ENV_FLAGS
#define CONFIG_POST_EXTERNAL_WORD_FUNCS
#define CONFIG_CMD_DIAG

/* we do the whole PCIe FPGA config stuff here */
#define	BOARD_LATE_INIT

#endif /* _CONFIG_KM_ARM_H */
