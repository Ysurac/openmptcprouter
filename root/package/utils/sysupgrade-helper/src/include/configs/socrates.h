/*
 * (C) Copyright 2008
 * Sergei Poselenov, Emcraft Systems, sposelenov@emcraft.com.
 *
 * Wolfgang Denk <wd@denx.de>
 * Copyright 2004 Freescale Semiconductor.
 * (C) Copyright 2002,2003 Motorola,Inc.
 * Xianghua Xiao <X.Xiao@motorola.com>
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
 * Socrates
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* new uImage format support */
#define CONFIG_FIT		1
#define CONFIG_OF_LIBFDT	1
#define CONFIG_FIT_VERBOSE	1 /* enable fit_format_{error,warning}() */

/* High Level Configuration Options */
#define CONFIG_BOOKE		1	/* BOOKE			*/
#define CONFIG_E500		1	/* BOOKE e500 family		*/
#define CONFIG_MPC85xx		1	/* MPC8540/60/55/41		*/
#define CONFIG_MPC8544		1
#define CONFIG_SOCRATES		1

#define	CONFIG_SYS_TEXT_BASE	0xfff80000

#define CONFIG_PCI

#define CONFIG_TSEC_ENET		/* tsec ethernet support	*/

#define CONFIG_MISC_INIT_R	1	/* Call misc_init_r		*/
#define CONFIG_BOARD_EARLY_INIT_R 1	/* Call board_early_init_r	*/

#define CONFIG_FSL_LAW		1	/* Use common FSL init code */

/*
 * Only possible on E500 Version 2 or newer cores.
 */
#define CONFIG_ENABLE_36BIT_PHYS	1

/*
 * sysclk for MPC85xx
 *
 * Two valid values are:
 *    33000000
 *    66000000
 *
 * Most PCI cards are still 33Mhz, so in the presence of PCI, 33MHz
 * is likely the desired value here, so that is now the default.
 * The board, however, can run at 66MHz.  In any event, this value
 * must match the settings of some switches.  Details can be found
 * in the README.mpc85xxads.
 */

#ifndef CONFIG_SYS_CLK_FREQ
#define CONFIG_SYS_CLK_FREQ	66666666
#endif

/*
 * These can be toggled for performance analysis, otherwise use default.
 */
#define CONFIG_L2_CACHE			/* toggle L2 cache		*/
#define CONFIG_BTB			/* toggle branch predition	*/

#define CONFIG_SYS_INIT_DBCR DBCR_IDM		/* Enable Debug Exceptions	*/

#undef	CONFIG_SYS_DRAM_TEST			/* memory test, takes time	*/
#define CONFIG_SYS_MEMTEST_START	0x00400000
#define CONFIG_SYS_MEMTEST_END		0x00C00000

#define CONFIG_SYS_CCSRBAR		0xE0000000
#define CONFIG_SYS_CCSRBAR_PHYS_LOW	CONFIG_SYS_CCSRBAR

/* DDR Setup */
#define CONFIG_FSL_DDR2
#undef CONFIG_FSL_DDR_INTERACTIVE
#define CONFIG_SPD_EEPROM		/* Use SPD EEPROM for DDR setup */
#define CONFIG_DDR_SPD

#undef CONFIG_ECC_INIT_VIA_DDRCONTROLLER	/* DDR controller or DMA? */
#define CONFIG_MEM_INIT_VALUE	0xDeadBeef

#define CONFIG_SYS_DDR_SDRAM_BASE	0x00000000
#define CONFIG_SYS_SDRAM_BASE		CONFIG_SYS_DDR_SDRAM_BASE
#define CONFIG_VERY_BIG_RAM

#define CONFIG_NUM_DDR_CONTROLLERS	1
#define CONFIG_DIMM_SLOTS_PER_CTLR	1
#define CONFIG_CHIP_SELECTS_PER_CTRL	2

/* I2C addresses of SPD EEPROMs */
#define SPD_EEPROM_ADDRESS	0x50	/* CTLR 0 DIMM 0 */

#define CONFIG_DDR_DEFAULT_CL	30		/* CAS latency 3	*/

/* Hardcoded values, to use instead of SPD */
#define CONFIG_SYS_DDR_CS0_BNDS		0x0000000f
#define CONFIG_SYS_DDR_CS0_CONFIG		0x80010102
#define CONFIG_SYS_DDR_TIMING_0		0x00260802
#define CONFIG_SYS_DDR_TIMING_1		0x3935D322
#define CONFIG_SYS_DDR_TIMING_2		0x14904CC8
#define CONFIG_SYS_DDR_MODE			0x00480432
#define CONFIG_SYS_DDR_INTERVAL		0x030C0100
#define CONFIG_SYS_DDR_CONFIG_2		0x04400000
#define CONFIG_SYS_DDR_CONFIG			0xC3008000
#define CONFIG_SYS_DDR_CLK_CONTROL		0x03800000
#define CONFIG_SYS_SDRAM_SIZE			256 /* in Megs */

/*
 * Flash on the LocalBus
 */
#define CONFIG_SYS_LBC_CACHE_BASE	0xf0000000	/* Localbus cacheable	 */

#define CONFIG_SYS_FLASH0		0xFE000000
#define CONFIG_SYS_FLASH1		0xFC000000
#define CONFIG_SYS_FLASH_BANKS_LIST	{ CONFIG_SYS_FLASH1, CONFIG_SYS_FLASH0 }

#define CONFIG_SYS_LBC_FLASH_BASE	CONFIG_SYS_FLASH1	/* Localbus flash start	*/
#define CONFIG_SYS_FLASH_BASE		CONFIG_SYS_LBC_FLASH_BASE /* start of FLASH	*/

#define CONFIG_SYS_BR0_PRELIM		0xfe001001	/* port size 16bit	*/
#define CONFIG_SYS_OR0_PRELIM		0xfe000030	/* 32MB Flash		*/
#define CONFIG_SYS_BR1_PRELIM		0xfc001001	/* port size 16bit	*/
#define CONFIG_SYS_OR1_PRELIM		0xfe000030	/* 32MB Flash		*/

#define CONFIG_SYS_FLASH_CFI				/* flash is CFI compat.	*/
#define CONFIG_FLASH_CFI_DRIVER			/* Use common CFI driver*/

#define CONFIG_SYS_MAX_FLASH_BANKS	2		/* number of banks	*/
#define CONFIG_SYS_MAX_FLASH_SECT	256		/* sectors per device	*/
#undef	CONFIG_SYS_FLASH_CHECKSUM
#define CONFIG_SYS_FLASH_ERASE_TOUT	60000	/* Flash Erase Timeout (ms)	*/
#define CONFIG_SYS_FLASH_WRITE_TOUT	500	/* Flash Write Timeout (ms)	*/

#define CONFIG_SYS_MONITOR_BASE	CONFIG_SYS_TEXT_BASE	/* start of monitor	*/

#define CONFIG_SYS_LBC_LCRR		0x00030004    /* LB clock ratio reg	*/
#define CONFIG_SYS_LBC_LBCR		0x00000000    /* LB config reg		*/
#define CONFIG_SYS_LBC_LSRT		0x20000000    /* LB sdram refresh timer	*/
#define CONFIG_SYS_LBC_MRTPR		0x20000000    /* LB refresh timer presc.*/

#define CONFIG_SYS_INIT_RAM_LOCK	1
#define CONFIG_SYS_INIT_RAM_ADDR	0xe4010000	/* Initial RAM address	*/
#define CONFIG_SYS_INIT_RAM_SIZE	0x4000		/* Size used area in RAM*/

#define CONFIG_SYS_GBL_DATA_OFFSET	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

#define CONFIG_SYS_MONITOR_LEN		(384 * 1024)	/* Reserve 384KiB for Mon */
#define CONFIG_SYS_MALLOC_LEN		(4 << 20)	/* Reserve 4 MB for malloc */

/* FPGA and NAND */
#define CONFIG_SYS_FPGA_BASE		0xc0000000
#define CONFIG_SYS_FPGA_SIZE		0x00100000	/* 1 MB		*/
#define CONFIG_SYS_HMI_BASE		0xc0010000
#define CONFIG_SYS_BR3_PRELIM		0xc0001881	/* UPMA, 32-bit */
#define CONFIG_SYS_OR3_PRELIM		0xfff00000	/* 1 MB 	*/

#define CONFIG_SYS_NAND_BASE		(CONFIG_SYS_FPGA_BASE + 0x70)
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_CMD_NAND

/* LIME GDC */
#define CONFIG_SYS_LIME_BASE		0xc8000000
#define CONFIG_SYS_LIME_SIZE		0x04000000	/* 64 MB	*/
#define CONFIG_SYS_BR2_PRELIM		0xc80018a1	/* UPMB, 32-bit	*/
#define CONFIG_SYS_OR2_PRELIM		0xfc000000	/* 64 MB	*/

#define CONFIG_VIDEO
#define CONFIG_VIDEO_MB862xx
#define CONFIG_VIDEO_MB862xx_ACCEL
#define CONFIG_CFB_CONSOLE
#define CONFIG_VIDEO_LOGO
#define CONFIG_VIDEO_BMP_LOGO
#define CONFIG_CONSOLE_EXTRA_INFO
#define VIDEO_FB_16BPP_PIXEL_SWAP
#define VIDEO_FB_16BPP_WORD_SWAP
#define CONFIG_VGA_AS_SINGLE_DEVICE
#define CONFIG_SYS_CONSOLE_IS_IN_ENV
#define CONFIG_VIDEO_SW_CURSOR
#define CONFIG_SPLASH_SCREEN
#define CONFIG_VIDEO_BMP_GZIP
#define CONFIG_SYS_VIDEO_LOGO_MAX_SIZE	(2 << 20)	/* decompressed img */

/* SDRAM Clock frequency, 100MHz (0x0000) or 133MHz (0x10000) */
#define CONFIG_SYS_MB862xx_CCF		0x10000
/* SDRAM parameter */
#define CONFIG_SYS_MB862xx_MMR		0x4157BA63

/* Serial Port */

#define CONFIG_CONS_INDEX     1
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	1
#define CONFIG_SYS_NS16550_CLK		get_bus_freq(0)

#define CONFIG_SYS_NS16550_COM1	(CONFIG_SYS_CCSRBAR+0x4500)
#define CONFIG_SYS_NS16550_COM2	(CONFIG_SYS_CCSRBAR+0x4600)

#define CONFIG_BAUDRATE         115200

#define CONFIG_SYS_BAUDRATE_TABLE  \
	{300, 600, 1200, 2400, 4800, 9600, 19200, 38400,115200}

#define CONFIG_CMDLINE_EDITING	1	/* add command line history	*/
#define CONFIG_AUTO_COMPLETE	1	/* add autocompletion support */
#define CONFIG_SYS_HUSH_PARSER		1	/* Use the HUSH parser		*/


/*
 * I2C
 */
#define CONFIG_FSL_I2C		/* Use FSL common I2C driver */
#define CONFIG_HARD_I2C			/* I2C with hardware support	*/
#undef	CONFIG_SOFT_I2C			/* I2C bit-banged		*/
#define CONFIG_SYS_I2C_SPEED		102124	/* I2C speed and slave address	*/
#define CONFIG_SYS_I2C_SLAVE		0x7F
#define CONFIG_SYS_I2C_OFFSET		0x3000

#define CONFIG_I2C_MULTI_BUS
#define CONFIG_SYS_I2C2_OFFSET		0x3100

/* I2C RTC */
#define CONFIG_RTC_RX8025		/* Use Epson rx8025 rtc via i2c	*/
#define CONFIG_SYS_I2C_RTC_ADDR	0x32	/* at address 0x32		*/

/* I2C W83782G HW-Monitoring IC */
#define CONFIG_SYS_I2C_W83782G_ADDR	0x28	/* W83782G address 		*/

/* I2C temp sensor */
/* Socrates uses Maxim's	DS75, which is compatible with LM75 */
#define CONFIG_DTT_LM75		1
#define CONFIG_DTT_SENSORS	{4}		/* Sensor addresses	*/
#define CONFIG_SYS_DTT_MAX_TEMP	125
#define CONFIG_SYS_DTT_LOW_TEMP	-55
#define CONFIG_SYS_DTT_HYSTERESIS	3
#define CONFIG_SYS_EEPROM_PAGE_WRITE_BITS	4

/*
 * General PCI
 * Memory space is mapped 1-1.
 */
#define CONFIG_SYS_PCI_PHYS		0x80000000	/* 1G PCI TLB */

/* PCI is clocked by the external source at 33 MHz */
#define CONFIG_PCI_CLK_FREQ	33000000
#define CONFIG_SYS_PCI1_MEM_BASE	0x80000000
#define CONFIG_SYS_PCI1_MEM_PHYS	CONFIG_SYS_PCI1_MEM_BASE
#define CONFIG_SYS_PCI1_MEM_SIZE	0x20000000	/* 512M			*/
#define CONFIG_SYS_PCI1_IO_BASE	0xE2000000
#define CONFIG_SYS_PCI1_IO_PHYS	CONFIG_SYS_PCI1_IO_BASE
#define CONFIG_SYS_PCI1_IO_SIZE	0x01000000	/* 16M			*/

#if defined(CONFIG_PCI)
#define CONFIG_PCI_PNP			/* do pci plug-and-play		*/
#undef CONFIG_PCI_SCAN_SHOW		/* show pci devices on startup	*/
#endif	/* CONFIG_PCI */


#define CONFIG_MII		1	/* MII PHY management */
#define CONFIG_TSEC1	1
#define CONFIG_TSEC1_NAME	"TSEC0"
#define CONFIG_TSEC3	1
#define CONFIG_TSEC3_NAME	"TSEC1"
#undef CONFIG_MPC85XX_FEC

#define TSEC1_PHY_ADDR		0
#define TSEC3_PHY_ADDR		1

#define TSEC1_PHYIDX		0
#define TSEC3_PHYIDX		0
#define TSEC1_FLAGS		TSEC_GIGABIT
#define TSEC3_FLAGS		TSEC_GIGABIT

/* Options are: TSEC[0,1] */
#define CONFIG_ETHPRIME		"TSEC0"
#define CONFIG_PHY_GIGE		1	/* Include GbE speed/duplex detection */

#define CONFIG_HAS_ETH0
#define CONFIG_HAS_ETH1

/*
 * Environment
 */
#define CONFIG_ENV_IS_IN_FLASH	1
#define CONFIG_ENV_SECT_SIZE	0x20000 /* 128K(one sector) for env	*/
#define CONFIG_ENV_ADDR		(CONFIG_SYS_MONITOR_BASE - CONFIG_ENV_SECT_SIZE)
#define CONFIG_ENV_SIZE		0x4000
#define CONFIG_ENV_ADDR_REDUND	(CONFIG_ENV_ADDR-CONFIG_ENV_SECT_SIZE)
#define CONFIG_ENV_SIZE_REDUND	(CONFIG_ENV_SIZE)

#define CONFIG_LOADS_ECHO	1	/* echo on for serial download	*/
#define CONFIG_SYS_LOADS_BAUD_CHANGE	1	/* allow baudrate change	*/

#define	CONFIG_TIMESTAMP		/* Print image info with ts	*/


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

#define CONFIG_CMD_BMP
#define CONFIG_CMD_DATE
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_DTT
#undef CONFIG_CMD_EEPROM
#define CONFIG_CMD_EXT2		/* EXT2 Support			*/
#define CONFIG_CMD_I2C
#define CONFIG_CMD_SDRAM
#define CONFIG_CMD_MII
#undef CONFIG_CMD_NFS
#define CONFIG_CMD_PING
#define CONFIG_CMD_SNTP
#define CONFIG_CMD_USB
#define CONFIG_CMD_REGINFO

#if defined(CONFIG_PCI)
    #define CONFIG_CMD_PCI
#endif

#undef CONFIG_WATCHDOG			/* watchdog disabled		*/

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP			/* undef to save memory		*/
#define CONFIG_SYS_LOAD_ADDR	0x2000000	/* default load address		*/
#define CONFIG_SYS_PROMPT	"=> "		/* Monitor Command Prompt	*/

#if defined(CONFIG_CMD_KGDB)
    #define CONFIG_SYS_CBSIZE	1024		/* Console I/O Buffer Size	*/
#else
    #define CONFIG_SYS_CBSIZE	256		/* Console I/O Buffer Size	*/
#endif

#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16) /* Print Buf Size	*/
#define CONFIG_SYS_MAXARGS	16		/* max number of command args	*/
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size	*/
#define CONFIG_SYS_HZ		1000		/* decrementer freq: 1ms ticks	*/

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTMAPSZ	(8 << 20)	/* Initial Memory map for Linux	*/

#if defined(CONFIG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	230400	/* speed to run kgdb serial port*/
#define CONFIG_KGDB_SER_INDEX	2	/* which serial port to use	*/
#endif


#define CONFIG_LOADADDR	 200000		/* default addr for tftp & bootm*/

#define CONFIG_BOOTDELAY 1		/* -1 disables auto-boot	*/

#define CONFIG_PREBOOT	"echo;"	\
	"echo Welcome on the ABB Socrates Board;" \
	"echo"

#undef	CONFIG_BOOTARGS		/* the boot command will set bootargs	*/

#define	CONFIG_EXTRA_ENV_SETTINGS					\
	"netdev=eth0\0"							\
	"consdev=ttyS0\0"						\
	"uboot_file=/home/tftp/syscon3/u-boot.bin\0"			\
	"bootfile=/home/tftp/syscon3/uImage\0"				\
	"fdt_file=/home/tftp/syscon3/socrates.dtb\0"			\
	"initrd_file=/home/tftp/syscon3/uinitrd.gz\0"			\
	"uboot_addr=FFFA0000\0"						\
	"kernel_addr=FE000000\0"					\
	"fdt_addr=FE1E0000\0"						\
	"ramdisk_addr=FE200000\0"					\
	"fdt_addr_r=B00000\0"						\
	"kernel_addr_r=200000\0"					\
	"ramdisk_addr_r=400000\0"					\
	"rootpath=/opt/eldk/ppc_85xxDP\0"				\
	"ramargs=setenv bootargs root=/dev/ram rw\0"			\
	"nfsargs=setenv bootargs root=/dev/nfs rw "			\
		"nfsroot=$serverip:$rootpath\0"				\
	"addcons=setenv bootargs $bootargs "				\
		"console=$consdev,$baudrate\0"				\
	"addip=setenv bootargs $bootargs "				\
		"ip=$ipaddr:$serverip:$gatewayip:$netmask"		\
		":$hostname:$netdev:off panic=1\0"			\
	"boot_nor=run ramargs addcons;"					\
		"bootm ${kernel_addr} ${ramdisk_addr} ${fdt_addr}\0"	\
	"net_nfs=tftp ${kernel_addr_r} ${bootfile}; "			\
		"tftp ${fdt_addr_r} ${fdt_file}; "			\
		"run nfsargs addip addcons;"				\
		"bootm ${kernel_addr_r} - ${fdt_addr_r}\0"		\
	"update_uboot=tftp 100000 ${uboot_file};"			\
		"protect off fffa0000 ffffffff;"			\
		"era fffa0000 ffffffff;"				\
		"cp.b 100000 fffa0000 ${filesize};"			\
		"setenv filesize;saveenv\0"				\
	"update_kernel=tftp 100000 ${bootfile};"			\
		"era fe000000 fe1dffff;"				\
		"cp.b 100000 fe000000 ${filesize};"			\
		"setenv filesize;saveenv\0"				\
	"update_fdt=tftp 100000 ${fdt_file};" 				\
		"era fe1e0000 fe1fffff;"				\
		"cp.b 100000 fe1e0000 ${filesize};"			\
		"setenv filesize;saveenv\0"				\
	"update_initrd=tftp 100000 ${initrd_file};" 			\
		"era fe200000 fe9fffff;"				\
		"cp.b 100000 fe200000 ${filesize};"			\
		"setenv filesize;saveenv\0"				\
	"clean_data=era fea00000 fff5ffff\0"				\
	"usbargs=setenv bootargs root=/dev/sda1 rw\0" 			\
	"load_usb=usb start;" 						\
		"ext2load usb 0:1 ${kernel_addr_r} /boot/uImage\0"	\
	"boot_usb=run load_usb usbargs addcons;"			\
		"bootm ${kernel_addr_r} - ${fdt_addr};"			\
		"bootm ${kernel_addr} ${ramdisk_addr} ${fdt_addr}\0"	\
	""
#define CONFIG_BOOTCOMMAND	"run boot_nor"

/* pass open firmware flat tree */
#define CONFIG_OF_LIBFDT	1
#define CONFIG_OF_BOARD_SETUP	1

/* USB support */
#define CONFIG_USB_OHCI_NEW		1
#define CONFIG_PCI_OHCI			1
#define CONFIG_PCI_OHCI_DEVNO		3 /* Number in PCI list */
#define CONFIG_PCI_EHCI_DEVNO		(CONFIG_PCI_OHCI_DEVNO / 2)
#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS	15
#define CONFIG_SYS_USB_OHCI_SLOT_NAME		"ohci_pci"
#define CONFIG_SYS_OHCI_SWAP_REG_ACCESS	1
#define CONFIG_DOS_PARTITION		1
#define CONFIG_USB_STORAGE		1

#endif	/* __CONFIG_H */
