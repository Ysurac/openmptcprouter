/*
 * (C) Copyright 2010, Stefano Babic <sbabic@denx.de>
 *
 * (C) Copyright 2008-2010 Freescale Semiconductor, Inc.
 *
 * Copyright (C) 2007, Guennadi Liakhovetski <lg@denx.de>
 *
 * Configuration for the MX35pdk Freescale board.
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

#include <asm/arch/imx-regs.h>

 /* High Level Configuration Options */
#define CONFIG_ARM1136	/* This is an arm1136 CPU core */
#define CONFIG_MX35
#define CONFIG_MX35_HCLK_FREQ	24000000

#define CONFIG_DISPLAY_CPUINFO

/* Set TEXT at the beginning of the NOR flash */
#define CONFIG_SYS_TEXT_BASE	0xA0000000
#define CONFIG_SYS_CACHELINE_SIZE	32

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_LATE_INIT

#define CONFIG_CMDLINE_TAG		/* enable passing of ATAGs */
#define CONFIG_REVISION_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 1024 * 1024)

/*
 * Hardware drivers
 */
#define CONFIG_HARD_I2C
#define CONFIG_I2C_MXC
#define CONFIG_SYS_I2C_BASE		I2C1_BASE_ADDR
#define CONFIG_SYS_I2C_SPEED		100000
#define CONFIG_MXC_SPI
#define CONFIG_MXC_GPIO


/*
 * PMIC Configs
 */
#define CONFIG_PMIC
#define CONFIG_PMIC_I2C
#define CONFIG_PMIC_FSL
#define CONFIG_SYS_FSL_PMIC_I2C_ADDR	0x08
#define CONFIG_RTC_MC13XXX

/*
 * MFD MC9SDZ60
 */
#define CONFIG_FSL_MC9SDZ60
#define CONFIG_SYS_FSL_MC9SDZ60_I2C_ADDR	0x69

/*
 * UART (console)
 */
#define CONFIG_MXC_UART
#define CONFIG_MXC_UART_BASE	UART1_BASE

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_CONS_INDEX	1
#define CONFIG_BAUDRATE		115200

/*
 * Command definition
 */

#include <config_cmd_default.h>

#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_BOOTP_SUBNETMASK
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_DNS

#define CONFIG_CMD_NAND
#define CONFIG_CMD_CACHE

#define CONFIG_CMD_I2C
#define CONFIG_CMD_SPI
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET
#define CONFIG_NET_RETRY_COUNT	100
#define CONFIG_CMD_DATE

#define CONFIG_BOOTDELAY	3

#define CONFIG_LOADADDR		0x80800000	/* loadaddr env var */

/*
 * Ethernet on the debug board (SMC911)
 */
#define CONFIG_SMC911X
#define CONFIG_SMC911X_16_BIT 1
#define CONFIG_SMC911X_BASE CS5_BASE_ADDR

#define CONFIG_HAS_ETH1
#define CONFIG_ETHPRIME

/*
 * Ethernet on SOC (FEC)
 */
#define CONFIG_FEC_MXC
#define IMX_FEC_BASE	FEC_BASE_ADDR
#define CONFIG_FEC_MXC_PHYADDR	0x1F

#define CONFIG_MII
#define CONFIG_DISCOVER_PHY

#define CONFIG_ARP_TIMEOUT	200UL

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP	/* undef to save memory */
#define CONFIG_SYS_PROMPT	"MX35 U-Boot > "
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_HUSH_PARSER	/* Use the HUSH parser */

#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE	256	/* Console I/O Buffer Size */
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS	16	/* max number of command args */
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE /* Boot Argument Buffer Size */

#define CONFIG_SYS_MEMTEST_START	0	/* memtest works on */
#define CONFIG_SYS_MEMTEST_END		0x10000

#undef	CONFIG_SYS_CLKS_IN_HZ	/* everything, incl board info, in Hz */

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR

#define CONFIG_SYS_HZ				1000


/*
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(128 * 1024)	/* regular stack */

/*
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	2
#define PHYS_SDRAM_1		CSD0_BASE_ADDR
#define PHYS_SDRAM_1_SIZE	(128 * 1024 * 1024)
#define PHYS_SDRAM_2		CSD1_BASE_ADDR
#define PHYS_SDRAM_2_SIZE	(128 * 1024 * 1024)

#define CONFIG_SYS_SDRAM_BASE		CSD0_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_ADDR	(IRAM_BASE_ADDR + 0x10000)
#define CONFIG_SYS_INIT_RAM_SIZE		(IRAM_SIZE / 2)
#define CONFIG_SYS_GBL_DATA_OFFSET	(CONFIG_SYS_INIT_RAM_SIZE - \
					GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SYS_INIT_RAM_ADDR + \
					CONFIG_SYS_GBL_DATA_OFFSET)

/*
 * MTD Command for mtdparts
 */
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE
#define CONFIG_FLASH_CFI_MTD
#define CONFIG_MTD_PARTITIONS
#define MTDIDS_DEFAULT		"nand0=mxc_nand,nor0=physmap-flash.0"
#define MTDPARTS_DEFAULT	"mtdparts=mxc_nand:1m(boot),5m(linux),"	\
				"96m(root),8m(cfg),1938m(user);"	\
				"physmap-flash.0:512k(b),4m(k),30m(u),28m(r)"

/*
 * FLASH and environment organization
 */
#define CONFIG_SYS_FLASH_BASE		CS0_BASE_ADDR
#define CONFIG_SYS_MAX_FLASH_BANKS 1	/* max number of memory banks */
#define CONFIG_SYS_MAX_FLASH_SECT 512	/* max number of sectors on one chip */
/* Monitor at beginning of flash */
#define CONFIG_SYS_MONITOR_BASE	CONFIG_SYS_FLASH_BASE
#define CONFIG_SYS_MONITOR_LEN		(512 * 1024)

#define CONFIG_ENV_SECT_SIZE	(128 * 1024)
#define CONFIG_ENV_SIZE		CONFIG_ENV_SECT_SIZE

/* Address and size of Redundant Environment Sector	*/
#define CONFIG_ENV_OFFSET_REDUND	(CONFIG_ENV_OFFSET + CONFIG_ENV_SIZE)
#define CONFIG_ENV_SIZE_REDUND	CONFIG_ENV_SIZE

#define CONFIG_ENV_ADDR		(CONFIG_SYS_MONITOR_BASE + \
				CONFIG_SYS_MONITOR_LEN)

#define CONFIG_ENV_IS_IN_FLASH

#if defined(CONFIG_FSL_ENV_IN_NAND)
	#define CONFIG_ENV_IS_IN_NAND
	#define CONFIG_ENV_OFFSET       (1024 * 1024)
#endif

/*
 * CFI FLASH driver setup
 */
#define CONFIG_SYS_FLASH_CFI		/* Flash memory is CFI compliant */
#define CONFIG_FLASH_CFI_DRIVER

/* A non-standard buffered write algorithm */
#define CONFIG_FLASH_SPANSION_S29WS_N
#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE	/* faster */
#define CONFIG_SYS_FLASH_PROTECTION	/* Use hardware sector protection */

/*
 * NAND FLASH driver setup
 */
#define CONFIG_NAND_MXC
#define CONFIG_NAND_MXC_V1_1
#define CONFIG_MXC_NAND_REGS_BASE	(NFC_BASE_ADDR)
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		(NFC_BASE_ADDR)
#define CONFIG_MXC_NAND_HWECC
#define CONFIG_SYS_NAND_LARGEPAGE

/*
 * Default environment and default scripts
 * to update uboot and load kernel
 */
#define xstr(s)	str(s)
#define str(s)	#s

#define CONFIG_HOSTNAME "mx35pdk"
#define	CONFIG_EXTRA_ENV_SETTINGS					\
	"netdev=eth1\0"							\
	"ethprime=smc911x\0"						\
	"nfsargs=setenv bootargs root=/dev/nfs rw "			\
		"nfsroot=${serverip}:${rootpath}\0"			\
	"ramargs=setenv bootargs root=/dev/ram rw\0"			\
	"addip_sta=setenv bootargs ${bootargs} "			\
		"ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}"	\
		":${hostname}:${netdev}:off panic=1\0"			\
	"addip_dyn=setenv bootargs ${bootargs} ip=dhcp\0"		\
	"addip=if test -n ${ipdyn};then run addip_dyn;"			\
		"else run addip_sta;fi\0"	\
	"addmtd=setenv bootargs ${bootargs} ${mtdparts}\0"		\
	"addtty=setenv bootargs ${bootargs}"				\
		" console=ttymxc0,${baudrate}\0"			\
	"addmisc=setenv bootargs ${bootargs} ${misc}\0"			\
	"loadaddr=80800000\0"						\
	"kernel_addr_r=80800000\0"					\
	"hostname=" xstr(CONFIG_HOSTNAME) "\0"				\
	"bootfile=" xstr(CONFIG_HOSTNAME) "/uImage\0"			\
	"ramdisk_file=" xstr(CONFIG_HOSTNAME) "/uRamdisk\0"		\
	"flash_self=run ramargs addip addtty addmtd addmisc;"		\
		"bootm ${kernel_addr} ${ramdisk_addr}\0"		\
	"flash_nfs=run nfsargs addip addtty addmtd addmisc;"		\
		"bootm ${kernel_addr}\0"				\
	"net_nfs=tftp ${kernel_addr_r} ${bootfile}; "			\
		"run nfsargs addip addtty addmtd addmisc;"		\
		"bootm ${kernel_addr_r}\0"				\
	"net_self_load=tftp ${kernel_addr_r} ${bootfile};"		\
		"tftp ${ramdisk_addr_r} ${ramdisk_file};\0"		\
	"u-boot=" xstr(CONFIG_HOSTNAME) "/u-boot.bin\0"			\
	"load=tftp ${loadaddr} ${u-boot}\0"				\
	"uboot_addr=" xstr(CONFIG_SYS_MONITOR_BASE) "\0"		\
	"update=protect off ${uboot_addr} +40000;"			\
		"erase ${uboot_addr} +40000;"				\
		"cp.b ${loadaddr} ${uboot_addr} ${filesize}\0"		\
	"upd=if run load;then echo Updating u-boot;if run update;"	\
		"then echo U-Boot updated;"				\
			"else echo Error updating u-boot !;"		\
			"echo Board without bootloader !!;"		\
		"fi;"							\
		"else echo U-Boot not downloaded..exiting;fi\0"		\
	"bootcmd=run net_nfs\0"

#endif				/* __CONFIG_H */
