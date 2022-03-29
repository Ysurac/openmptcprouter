/*
 * (C) Copyright 2003-2007
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * Based on PRO Motion board config file by Andy Joseph, andy@promessdev.com
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

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 */

/* CPU and board */
#define CONFIG_MPC5xxx		1	/* This is an MPC5xxx CPU */
#define CONFIG_MPC5200		1	/* More exactly a MPC5200 */
#define CONFIG_MOTIONPRO	1	/* ... on Promess Motion-PRO board */

#define CONFIG_HIGH_BATS	1	/* High BATs supported */

#define	CONFIG_SYS_TEXT_BASE	0xfff00000

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

#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_BEDBUG
#define CONFIG_CMD_DATE
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_DTT
#define CONFIG_CMD_EEPROM
#define CONFIG_CMD_ELF
#define CONFIG_CMD_FAT
#define CONFIG_CMD_I2C
#define CONFIG_CMD_IDE
#define CONFIG_CMD_IMMAP
#define CONFIG_CMD_JFFS2
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_REGINFO

/*
 * Serial console configuration
 */
#define CONFIG_PSC_CONSOLE	1	/* console is on PSC1 */
#define CONFIG_NETCONSOLE	1	/* network console */
#define CONFIG_BAUDRATE		115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200, 230400 }

/*
 * Ethernet configuration
 */
#define CONFIG_MPC5xxx_FEC	1
#define CONFIG_MPC5xxx_FEC_MII100
#define CONFIG_PHY_ADDR		0x2
#define CONFIG_PHY_TYPE		0x79c874
#define CONFIG_RESET_PHY_R	1

/*
 * Autobooting
 */
#define CONFIG_BOOTDELAY	2	/* autoboot after 2 seconds */
#define CONFIG_AUTOBOOT_KEYED
#define CONFIG_AUTOBOOT_STOP_STR	"\x1b\x1b"
#define DEBUG_BOOTKEYS		0
#undef CONFIG_AUTOBOOT_DELAY_STR
#undef CONFIG_BOOTARGS
#define CONFIG_AUTOBOOT_PROMPT	"Autobooting in %d seconds, "		\
				"press \"<Esc><Esc>\" to stop\n", bootdelay

#define CONFIG_CMDLINE_EDITING		1	/* add command line history	*/
#define	CONFIG_SYS_HUSH_PARSER		1	/* use "hush" command parser	*/

#define CONFIG_ETHADDR		00:50:C2:40:10:00
#define CONFIG_OVERWRITE_ETHADDR_ONCE	1
#define CONFIG_VERSION_VARIABLE	1	/* include version env variable */

/*
 * Default environment settings
 */
#define CONFIG_EXTRA_ENV_SETTINGS					\
	"netdev=eth0\0"							\
	"hostname=motionpro\0"						\
	"netmask=255.255.0.0\0"						\
	"ipaddr=192.168.160.22\0"					\
	"serverip=192.168.1.1\0"					\
	"gatewayip=192.168.1.1\0"					\
	"console=ttyPSC0,115200\0"					\
	"u-boot_addr=400000\0"						\
	"kernel_addr=400000\0"						\
	"fdt_addr=700000\0"						\
	"ramdisk_addr=800000\0"						\
	"multi_image_addr=800000\0"					\
	"rootpath=/opt/eldk/ppc_6xx\0"					\
	"u-boot=motionpro/u-boot.bin\0"					\
	"bootfile=motionpro/uImage\0"					\
	"fdt_file=motionpro/motionpro.dtb\0"				\
	"ramdisk_file=motionpro/uRamdisk\0"				\
	"multi_image_file=kernel+initrd+dtb.img\0"			\
	"load=tftp ${u-boot_addr} ${u-boot}\0"				\
	"update=prot off fff00000 +${filesize};"			\
		"era fff00000 +${filesize}; "				\
		"cp.b ${u-boot_addr} fff00000 ${filesize};"		\
		"prot on fff00000 +${filesize}\0"			\
	"ramargs=setenv bootargs root=/dev/ram rw\0"			\
	"nfsargs=setenv bootargs root=/dev/nfs rw "			\
		"nfsroot=${serverip}:${rootpath}\0"			\
	"fat_args=setenv bootargs rw\0"					\
	"addmtd=setenv bootargs ${bootargs} ${mtdparts}\0"		\
	"addip=setenv bootargs ${bootargs} "				\
		"ip=${ipaddr}:${serverip}:${gatewayip}:"		\
		"${netmask}:${hostname}:${netdev}:off panic=1 "		\
		"console=${console}\0"					\
	"net_nfs=tftp ${kernel_addr} ${bootfile}; "			\
		"tftp ${fdt_addr} ${fdt_file}; run nfsargs addip; "	\
		"bootm ${kernel_addr} - ${fdt_addr}\0"			\
	"net_self=tftp ${kernel_addr} ${bootfile}; "			\
		"tftp ${fdt_addr} ${fdt_file}; "			\
		"tftp ${ramdisk_addr} ${ramdisk_file}; "		\
		"run ramargs addip; "					\
		"bootm ${kernel_addr} ${ramdisk_addr} ${fdt_addr}\0"	\
	"fat_multi=run fat_args addip; fatload ide 0:1 "		\
		"${multi_image_addr} ${multi_image_file}; "		\
		"bootm ${multi_image_addr}\0"				\
	""
#define CONFIG_BOOTCOMMAND	"run net_nfs"

/*
 * do board-specific init
 */
#define CONFIG_BOARD_EARLY_INIT_R	1

/*
 * Low level configuration
 */

/*
 * Clock configuration: SYS_XTALIN = 33MHz
 */
#define CONFIG_SYS_MPC5XXX_CLKIN	33000000

/*
 * Set IPB speed to 100MHz
 */
#define CONFIG_SYS_IPBCLK_EQUALS_XLBCLK

/*
 * Memory map
 */
/*
 * Warning!!! with the current BestComm Task, MBAR MUST BE set to 0xf0000000.
 * Setting MBAR to otherwise will cause system hang when using SmartDMA such
 * as network commands.
 */
#define CONFIG_SYS_MBAR			0xf0000000
#define CONFIG_SYS_SDRAM_BASE		0x00000000

/*
 * If building for running out of SDRAM, then MBAR has been set up beforehand
 * (e.g., by the BDI). Otherwise we must specify the default boot-up value of
 * MBAR, as given in the doccumentation.
 */
#if CONFIG_SYS_TEXT_BASE == 0x00100000
#define CONFIG_SYS_DEFAULT_MBAR	0xf0000000
#else /* CONFIG_SYS_TEXT_BASE != 0x00100000 */
#define CONFIG_SYS_DEFAULT_MBAR	0x80000000
#define CONFIG_SYS_LOWBOOT		1
#endif /* CONFIG_SYS_TEXT_BASE == 0x00100000 */

/* Use SRAM until RAM will be available */
#define CONFIG_SYS_INIT_RAM_ADDR	MPC5XXX_SRAM
#define CONFIG_SYS_INIT_RAM_SIZE	MPC5XXX_SRAM_SIZE

#define CONFIG_SYS_GBL_DATA_OFFSET	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

#define CONFIG_SYS_MONITOR_BASE	CONFIG_SYS_TEXT_BASE
#if (CONFIG_SYS_MONITOR_BASE < CONFIG_SYS_FLASH_BASE)
#define CONFIG_SYS_RAMBOOT		1
#endif

#define CONFIG_SYS_MONITOR_LEN		(256 << 10)	/* 256 kB for Monitor */
#define CONFIG_SYS_MALLOC_LEN		(1024 << 10)	/* 1 MiB for malloc() */
#define CONFIG_SYS_BOOTMAPSZ		(8 << 20)	/* initial mem map for Linux */

/*
 * Chip selects configuration
 */
/* Boot Chipselect */
#define CONFIG_SYS_BOOTCS_START	CONFIG_SYS_FLASH_BASE
#define CONFIG_SYS_BOOTCS_SIZE		CONFIG_SYS_FLASH_SIZE
#define CONFIG_SYS_BOOTCS_CFG		0x00045D00

/* Flash memory addressing */
#define CONFIG_SYS_CS0_START		CONFIG_SYS_FLASH_BASE
#define CONFIG_SYS_CS0_SIZE		CONFIG_SYS_FLASH_SIZE
#define CONFIG_SYS_CS0_CFG		CONFIG_SYS_BOOTCS_CFG

/* Dual Port SRAM -- Kollmorgen Drive memory addressing */
#define CONFIG_SYS_CS1_START		0x50000000
#define CONFIG_SYS_CS1_SIZE		0x10000
#define CONFIG_SYS_CS1_CFG		0x05055800

/* Local register access */
#define CONFIG_SYS_CS2_START		0x50010000
#define CONFIG_SYS_CS2_SIZE		0x10000
#define CONFIG_SYS_CS2_CFG		0x05055800

/* Anybus CompactCom Module memory addressing */
#define CONFIG_SYS_CS3_START		0x50020000
#define CONFIG_SYS_CS3_SIZE		0x10000
#define CONFIG_SYS_CS3_CFG		0x05055800

/* No burst and dead cycle = 2 for all CSs */
#define CONFIG_SYS_CS_BURST		0x00000000
#define CONFIG_SYS_CS_DEADCYCLE	0x22222222

/*
 * SDRAM configuration
 */
/* 2 x MT48LC16M16A2BG-75 IT:D, CASL 3, 32 bit data bus */
#define SDRAM_CONFIG1		0x62322900
#define SDRAM_CONFIG2		0x88c70000
#define SDRAM_CONTROL		0x504f0000
#define SDRAM_MODE		0x00cd0000

/*
 * Flash configuration
 */
#define CONFIG_SYS_FLASH_CFI		1	/* Flash is CFI conformant */
#define CONFIG_FLASH_CFI_DRIVER	1
#define CONFIG_SYS_FLASH_BASE		0xff000000
#define CONFIG_SYS_FLASH_SIZE		0x01000000
#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max num of memory banks */
#define CONFIG_SYS_FLASH_BANKS_LIST	{ CONFIG_SYS_FLASH_BASE }
#define CONFIG_SYS_MAX_FLASH_SECT	128	/* max num of sects on one chip */
#define CONFIG_FLASH_16BIT		/* Flash is 16-bit */

/*
 * MTD configuration
 */
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE		/* needed for mtdparts commands */
#define CONFIG_FLASH_CFI_MTD
#define MTDIDS_DEFAULT		"nor0=motionpro-0"
#define MTDPARTS_DEFAULT	"mtdparts=motionpro-0:"			  \
					"13m(fs),2m(kernel),256k(uboot)," \
					"128k(env),128k(redund_env),"	  \
					"128k(dtb),-(user_data)"

/*
 * IDE/ATA configuration
 */
#define CONFIG_SYS_ATA_BASE_ADDR	MPC5XXX_ATA
#define CONFIG_SYS_IDE_MAXBUS		1
#define CONFIG_SYS_IDE_MAXDEVICE	1
#define CONFIG_IDE_PREINIT

#define CONFIG_SYS_ATA_DATA_OFFSET	0x0060
#define CONFIG_SYS_ATA_REG_OFFSET	CONFIG_SYS_ATA_DATA_OFFSET
#define CONFIG_SYS_ATA_STRIDE		4
#define CONFIG_DOS_PARTITION

/*
 * I2C configuration
 */
#define CONFIG_HARD_I2C		1	/* I2C with hardware support */
#define CONFIG_SYS_I2C_MODULE		2	/* select I2C module #2 */
#define CONFIG_SYS_I2C_SPEED		100000	/* 100 kHz */
#define CONFIG_SYS_I2C_SLAVE		0x7F

/*
 * EEPROM configuration
 */
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN		1
#define CONFIG_SYS_EEPROM_PAGE_WRITE_BITS	1	/* 2 bytes per write cycle */
#define CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS	5	/* 2ms/cycle + 3ms extra */
#define CONFIG_SYS_I2C_MULTI_EEPROMS		1	/* 2 EEPROMs (addr:50,52) */

/*
 * RTC configuration
 */
#define CONFIG_RTC_DS1337	1
#define CONFIG_SYS_I2C_RTC_ADDR	0x68

/*
 * Status LED configuration
 */
#define CONFIG_STATUS_LED		/* Status LED enabled */
#define CONFIG_BOARD_SPECIFIC_LED

#define ENABLE_GPIO_OUT		0x00000024
#define LED_ON			0x00000010

#ifndef __ASSEMBLY__
/*
 * In case of Motion-PRO, a LED is identified by its corresponding
 * GPT Enable and Mode Select Register.
 */
typedef volatile unsigned long * led_id_t;

extern void __led_init(led_id_t id, int state);
extern void __led_toggle(led_id_t id);
extern void __led_set(led_id_t id, int state);
#endif /* __ASSEMBLY__ */

/*
 * Temperature sensor
 */
#define CONFIG_DTT_LM75		1
#define CONFIG_DTT_SENSORS	{ 0x49 }

/*
 * Environment settings
 */
#define CONFIG_ENV_IS_IN_FLASH	1
/* This has to be a multiple of the Flash sector size */
#define CONFIG_ENV_ADDR		(CONFIG_SYS_MONITOR_BASE + CONFIG_SYS_MONITOR_LEN)
#define CONFIG_ENV_SIZE		0x1000
#define CONFIG_ENV_SECT_SIZE	0x20000

/* Configuration of redundant environment */
#define CONFIG_ENV_ADDR_REDUND	(CONFIG_ENV_ADDR + CONFIG_ENV_SECT_SIZE)
#define CONFIG_ENV_SIZE_REDUND	(CONFIG_ENV_SIZE)

/*
 * Pin multiplexing configuration
 */

/* PSC1: UART1
 * PSC2: GPIO (default)
 * PSC3: GPIO (default)
 * USB: 2xUART4/5
 * Ethernet: Ethernet 100Mbit with MD
 * Timer: CAN2/GPIO
 * PSC6/IRDA: GPIO (default)
 */
#define CONFIG_SYS_GPS_PORT_CONFIG	0x1105a004

/*
 * Motion-PRO's CPLD revision control register
 */
#define CPLD_REV_REGISTER	(CONFIG_SYS_CS2_START + 0x06)

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP			/* undef to save memory    */
#define CONFIG_SYS_PROMPT		"=> "	/* Monitor Command Prompt   */
#define CONFIG_SYS_CBSIZE		1024	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)	/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS		16		/* max number of command args */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size */

#define CONFIG_SYS_MEMTEST_START	0x00100000	/* memtest works on */
#define CONFIG_SYS_MEMTEST_END		0x03e00000	/* 1 ... 62 MiB in DRAM */
#define CONFIG_SYS_ALT_MEMTEST

#define CONFIG_SYS_LOAD_ADDR		0x200000	/* default kernel load addr */

#define CONFIG_SYS_HZ			1000	/* decrementer freq: 1 ms ticks */

/*
 * Various low-level settings
 */
#define CONFIG_SYS_HID0_INIT		HID0_ICE | HID0_ICFI
#define CONFIG_SYS_HID0_FINAL		HID0_ICE

#define CONFIG_SYS_CACHELINE_SIZE	32	/* For MPC5xxx CPUs */

/* Not needed for MPC 5xxx U-Boot, but used by tools/updater */
#define CONFIG_SYS_RESET_ADDRESS	0xfff00100

/* pass open firmware flat tree */
#define CONFIG_OF_LIBFDT	1
#define CONFIG_OF_BOARD_SETUP	1

#define OF_CPU			"PowerPC,5200@0"
#define OF_SOC			"soc5200@f0000000"
#define OF_TBCLK		(bd->bi_busfreq / 4)
#define OF_STDOUT_PATH		"/soc5200@f0000000/serial@2000"

#endif /* __CONFIG_H */
