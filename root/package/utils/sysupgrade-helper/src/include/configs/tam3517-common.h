/*
 * Copyright (C) 2011
 * Stefano Babic, DENX Software Engineering, sbabic@denx.de.
 *
 * Copyright (C) 2009 TechNexion Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef __TAM3517_H
#define __TAM3517_H

/*
 * High Level Configuration Options
 */
#define CONFIG_OMAP		/* in a TI OMAP core */
#define CONFIG_OMAP34XX		/* which is a 34XX */

#define CONFIG_SYS_TEXT_BASE 0x80008000

#define CONFIG_SYS_CACHELINE_SIZE	64

#define CONFIG_EMIF4	/* The chip has EMIF4 controller */

#include <asm/arch/cpu.h>		/* get chip and board defs */
#include <asm/arch/omap3.h>

/*
 * Display CPU and Board information
 */
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

/* Clock Defines */
#define V_OSCK			26000000	/* Clock output from T2 */
#define V_SCLK			(V_OSCK >> 1)

#undef CONFIG_USE_IRQ				/* no support for IRQs */
#define CONFIG_MISC_INIT_R

#define CONFIG_CMDLINE_TAG			/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG

/*
 * Size of malloc() pool
 */
#define CONFIG_ENV_SIZE			(128 << 10)	/* 128 KiB sector */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (128 << 10) + \
					2 * 1024 * 1024)
/*
 * DDR related
 */
#define CONFIG_OMAP3_MICRON_DDR		/* Micron DDR */
#define CONFIG_SYS_CS0_SIZE		(256 * 1024 * 1024)

/*
 * Hardware drivers
 */

/*
 * NS16550 Configuration
 */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
#define CONFIG_SYS_NS16550_CLK		48000000	/* 48MHz (APLL96/2) */

/*
 * select serial console configuration
 */
#define CONFIG_CONS_INDEX		1
#define CONFIG_SYS_NS16550_COM1		OMAP34XX_UART1
#define CONFIG_SERIAL1			/* UART1 */

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{4800, 9600, 19200, 38400, 57600,\
					115200}
#define CONFIG_MMC
#define CONFIG_OMAP_HSMMC
#define CONFIG_GENERIC_MMC
#define CONFIG_DOS_PARTITION

/* EHCI */
#define CONFIG_OMAP3_GPIO_5
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_OMAP
#define CONFIG_USB_ULPI
#define CONFIG_USB_ULPI_VIEWPORT_OMAP
#define CONFIG_OMAP_EHCI_PHY1_RESET_GPIO	25
#define CONFIG_SYS_USB_EHCI_MAX_ROOT_PORTS 3
#define CONFIG_USB_STORAGE

/* #define CONFIG_EHCI_DCACHE */

/* commands to include */
#include <config_cmd_default.h>

#define CONFIG_CMD_CACHE
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_EXT2		/* EXT2 Support			*/
#define CONFIG_CMD_FAT		/* FAT support			*/
#define CONFIG_CMD_GPIO
#define CONFIG_CMD_I2C		/* I2C serial bus support	*/
#define CONFIG_CMD_MII
#define CONFIG_CMD_MMC		/* MMC support			*/
#define CONFIG_CMD_NET
#define CONFIG_CMD_NFS
#define CONFIG_CMD_NAND		/* NAND support			*/
#define CONFIG_CMD_PING
#define CONFIG_CMD_USB

#undef CONFIG_CMD_FLASH		/* only NAND on the SOM */
#undef CONFIG_CMD_IMLS

#define CONFIG_SYS_NO_FLASH
#define CONFIG_HARD_I2C
#define CONFIG_SYS_I2C_SPEED		400000
#define CONFIG_SYS_I2C_SLAVE		1
#define CONFIG_SYS_I2C_BUS		0
#define CONFIG_SYS_I2C_BUS_SELECT	1
#define CONFIG_DRIVER_OMAP34XX_I2C


/*
 * Board NAND Info.
 */
#define CONFIG_SYS_NAND_BASE		NAND_BASE	/* physical address */
							/* to access */
							/* nand at CS0 */

#define CONFIG_SYS_MAX_NAND_DEVICE	1		/* Max number of */
							/* NAND devices */

#define CONFIG_AUTO_COMPLETE

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER		/* use "hush" command parser */
#define CONFIG_CMDLINE_EDITING
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE		512	/* Console I/O Buffer Size */

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS		32	/* max number of command */
						/* args */
/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		(CONFIG_SYS_CBSIZE)
/* memtest works on */
#define CONFIG_SYS_MEMTEST_START	(OMAP34XX_SDRC_CS0)
#define CONFIG_SYS_MEMTEST_END		(OMAP34XX_SDRC_CS0 + \
					0x01F00000) /* 31MB */

#define CONFIG_SYS_LOAD_ADDR		(OMAP34XX_SDRC_CS0) /* default load */
								/* address */

/*
 * AM3517 has 12 GP timers, they can be driven by the system clock
 * (12/13/16.8/19.2/38.4MHz) or by 32KHz clock. We use 13MHz (V_SCLK).
 * This rate is divided by a local divisor.
 */
#define CONFIG_SYS_TIMERBASE		OMAP34XX_GPT2
#define CONFIG_SYS_PTV			2	/* Divisor: 2^(PTV+1) => 8 */
#define CONFIG_SYS_HZ			1000

/*
 * Stack sizes
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(128 << 10)	/* regular stack 128 KiB */

/*
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	2	/* CS1 may or may not be populated */
#define PHYS_SDRAM_1		OMAP34XX_SDRC_CS0
#define PHYS_SDRAM_1_SIZE	(32 << 20)	/* at least 32 MiB */
#define PHYS_SDRAM_2		OMAP34XX_SDRC_CS1

/*
 * FLASH and environment organization
 */

/* **** PISMO SUPPORT *** */

/* Configure the PISMO */
#define PISMO1_NAND_SIZE		GPMC_SIZE_128M

#define CONFIG_NAND_OMAP_GPMC
#define GPMC_NAND_ECC_LP_x16_LAYOUT
#define CONFIG_ENV_IS_IN_NAND
#define SMNAND_ENV_OFFSET		0x180000 /* environment starts here */

/* Redundant Environment */
#define CONFIG_SYS_ENV_SECT_SIZE	(128 << 10)	/* 128 KiB */
#define CONFIG_ENV_OFFSET		SMNAND_ENV_OFFSET
#define CONFIG_ENV_ADDR			SMNAND_ENV_OFFSET
#define CONFIG_ENV_OFFSET_REDUND	(CONFIG_ENV_OFFSET + \
						2 * CONFIG_SYS_ENV_SECT_SIZE)
#define CONFIG_ENV_SIZE_REDUND		CONFIG_ENV_SIZE

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1
#define CONFIG_SYS_INIT_RAM_ADDR	0x4020f800
#define CONFIG_SYS_INIT_RAM_SIZE	0x800
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SYS_INIT_RAM_ADDR + \
					 CONFIG_SYS_INIT_RAM_SIZE - \
					 GENERATED_GBL_DATA_SIZE)

/*
 * ethernet support, EMAC
 *
 */
#define CONFIG_DRIVER_TI_EMAC
#define CONFIG_DRIVER_TI_EMAC_USE_RMII
#define CONFIG_MII
#define CONFIG_EMAC_MDIO_PHY_NUM	0
#define CONFIG_BOOTP_DEFAULT
#define CONFIG_BOOTP_DNS
#define CONFIG_BOOTP_DNS2
#define CONFIG_BOOTP_SEND_HOSTNAME
#define CONFIG_NET_RETRY_COUNT 10

/* Defines for SPL */
#define CONFIG_SPL
#define CONFIG_SPL_CONSOLE
#define CONFIG_SPL_NAND_SIMPLE
#define CONFIG_SPL_NAND_SOFTECC
#define CONFIG_SPL_NAND_WORKSPACE	0x8f07f000 /* below BSS */

#define CONFIG_SPL_LIBCOMMON_SUPPORT
#define CONFIG_SPL_LIBDISK_SUPPORT
#define CONFIG_SPL_I2C_SUPPORT
#define CONFIG_SPL_LIBGENERIC_SUPPORT
#define CONFIG_SPL_SERIAL_SUPPORT
#define CONFIG_SPL_POWER_SUPPORT
#define CONFIG_SPL_NAND_SUPPORT
#define CONFIG_SPL_LDSCRIPT		"$(CPUDIR)/omap-common/u-boot-spl.lds"

#define CONFIG_SPL_TEXT_BASE		0x40200000 /*CONFIG_SYS_SRAM_START*/
#define CONFIG_SPL_MAX_SIZE		(54 * 1024)	/* 8 KB for stack */
#define CONFIG_SPL_STACK		LOW_LEVEL_SRAM_STACK

#define CONFIG_SYS_SPL_MALLOC_START	0x8f000000
#define CONFIG_SYS_SPL_MALLOC_SIZE	0x80000
#define CONFIG_SPL_BSS_START_ADDR	0x8f080000 /* end of RAM */
#define CONFIG_SPL_BSS_MAX_SIZE		0x80000

/* NAND boot config */
#define CONFIG_SYS_NAND_PAGE_COUNT	64
#define CONFIG_SYS_NAND_PAGE_SIZE	2048
#define CONFIG_SYS_NAND_OOBSIZE		64
#define CONFIG_SYS_NAND_BLOCK_SIZE	(128 * 1024)
#define CONFIG_SYS_NAND_5_ADDR_CYCLE
#define CONFIG_SYS_NAND_BAD_BLOCK_POS	0
#define CONFIG_SYS_NAND_ECCPOS		{40, 41, 42, 43, 44, 45, 46, 47,\
					 48, 49, 50, 51, 52, 53, 54, 55,\
					 56, 57, 58, 59, 60, 61, 62, 63}
#define CONFIG_SYS_NAND_ECCSIZE		256
#define CONFIG_SYS_NAND_ECCBYTES	3

#define CONFIG_SYS_NAND_U_BOOT_START	CONFIG_SYS_TEXT_BASE

#define CONFIG_SYS_NAND_U_BOOT_OFFS	0x80000
#define CONFIG_SYS_NAND_U_BOOT_SIZE	0x80000

#define CONFIG_OF_LIBFDT
#define CONFIG_FIT
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_RBTREE
#define CONFIG_LZO
#define CONFIG_MTD_PARTITIONS
#define CONFIG_MTD_DEVICE
#define CONFIG_CMD_MTDPARTS

/* Setup MTD for NAND on the SOM */
#define MTDIDS_DEFAULT		"nand0=omap2-nand.0"
#define MTDPARTS_DEFAULT	"mtdparts=omap2-nand.0:512k(MLO)," \
				"1m(u-boot),256k(env1)," \
				"256k(env2),6m(kernel),-(rootfs)"

#define xstr(s)	str(s)
#define str(s)	#s

#define	CONFIG_TAM3517_SETTINGS						\
	"netdev=eth0\0"							\
	"nandargs=setenv bootargs root=${nandroot} "			\
		"rootfstype=${nandrootfstype}\0"			\
	"nfsargs=setenv bootargs root=/dev/nfs rw "			\
		"nfsroot=${serverip}:${rootpath}\0"			\
	"ramargs=setenv bootargs root=/dev/ram rw\0"			\
	"addip_sta=setenv bootargs ${bootargs} "			\
		"ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}"	\
		":${hostname}:${netdev}:off panic=1\0"			\
	"addip_dyn=setenv bootargs ${bootargs} ip=dhcp\0"		\
	"addip=if test -n ${ipdyn};then run addip_dyn;"			\
		"else run addip_sta;fi\0"				\
	"addmtd=setenv bootargs ${bootargs} ${mtdparts}\0"		\
	"addtty=setenv bootargs ${bootargs}"				\
		" console=ttyO0,${baudrate}\0"				\
	"addmisc=setenv bootargs ${bootargs} ${misc}\0"			\
	"loadaddr=82000000\0"						\
	"kernel_addr_r=82000000\0"					\
	"hostname=" xstr(CONFIG_HOSTNAME) "\0"				\
	"bootfile=" xstr(CONFIG_HOSTNAME) "/uImage\0"			\
	"flash_self=run ramargs addip addtty addmtd addmisc;"		\
		"bootm ${kernel_addr} ${ramdisk_addr}\0"		\
	"flash_nfs=run nfsargs addip addtty addmtd addmisc;"		\
		"bootm ${kernel_addr}\0"				\
	"nandboot=run nandargs addip addtty addmtd addmisc;"		\
		"nand read ${kernel_addr_r} kernel\0"			\
		"bootm ${kernel_addr_r}\0"				\
	"net_nfs=tftp ${kernel_addr_r} ${bootfile}; "			\
		"run nfsargs addip addtty addmtd addmisc;"		\
		"bootm ${kernel_addr_r}\0"				\
	"net_self=if run net_self_load;then "				\
		"run ramargs addip addtty addmtd addmisc;"		\
		"bootm ${kernel_addr_r} ${ramdisk_addr_r};"		\
		"else echo Images not loades;fi\0"			\
	"u-boot=" xstr(CONFIG_HOSTNAME) "/u-boot.img\0"			\
	"load=tftp ${loadaddr} ${u-boot}\0"				\
	"loadmlo=tftp ${loadaddr} ${mlo}\0"				\
	"mlo=" xstr(CONFIG_HOSTNAME) "/MLO\0"				\
	"uboot_addr=0x80000\0"						\
	"update=nandecc sw;nand erase ${uboot_addr} 100000;"		\
		"nand write ${loadaddr} ${uboot_addr} 80000\0"		\
	"updatemlo=nandecc hw;nand erase 0 20000;"			\
		"nand write ${loadaddr} 0 20000\0"			\
	"upd=if run load;then echo Updating u-boot;if run update;"	\
		"then echo U-Boot updated;"				\
			"else echo Error updating u-boot !;"		\
			"echo Board without bootloader !!;"		\
		"fi;"							\
		"else echo U-Boot not downloaded..exiting;fi\0"		\

#endif /* __TAM3517_H */
