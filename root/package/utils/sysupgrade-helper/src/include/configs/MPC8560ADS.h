/*
 * Copyright 2004, 2011 Freescale Semiconductor.
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
 * mpc8560ads board configuration file
 *
 * Please refer to doc/README.mpc85xx for more info.
 *
 * Make sure you change the MAC address and other network params first,
 * search for CONFIG_ETHADDR, CONFIG_SERVERIP, etc in this file.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* High Level Configuration Options */
#define CONFIG_BOOKE		1	/* BOOKE */
#define CONFIG_E500		1	/* BOOKE e500 family */
#define CONFIG_MPC85xx		1	/* MPC8540/MPC8560 */
#define CONFIG_CPM2		1	/* has CPM2 */
#define CONFIG_MPC8560ADS	1	/* MPC8560ADS board specific */
#define CONFIG_MPC8560		1

/*
 * default CCARBAR is at 0xff700000
 * assume U-Boot is less than 0.5MB
 */
#define	CONFIG_SYS_TEXT_BASE	0xfff80000

#define CONFIG_PCI
#define CONFIG_SYS_PCI_64BIT	1	/* enable 64-bit PCI resources */
#define CONFIG_TSEC_ENET		/* tsec ethernet support */
#undef CONFIG_ETHER_ON_FCC             /* cpm FCC ethernet support */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_FSL_LAW		1	/* Use common FSL init code */
#define CONFIG_RESET_PHY_R	1	/* Call reset_phy() */

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
#define CONFIG_SYS_CLK_FREQ	33000000
#endif


/*
 * These can be toggled for performance analysis, otherwise use default.
 */
#define CONFIG_L2_CACHE			/* toggle L2 cache */
#define CONFIG_BTB			/* toggle branch predition */

#define CONFIG_SYS_INIT_DBCR DBCR_IDM		/* Enable Debug Exceptions */

#define CONFIG_SYS_MEMTEST_START	0x00200000	/* memtest region */
#define CONFIG_SYS_MEMTEST_END		0x00400000

#define CONFIG_SYS_CCSRBAR		0xe0000000
#define CONFIG_SYS_CCSRBAR_PHYS_LOW	CONFIG_SYS_CCSRBAR

/* DDR Setup */
#define CONFIG_FSL_DDR1
#define CONFIG_SPD_EEPROM		/* Use SPD EEPROM for DDR setup*/
#define CONFIG_DDR_SPD
#undef CONFIG_FSL_DDR_INTERACTIVE

#define CONFIG_MEM_INIT_VALUE		0xDeadBeef

#define CONFIG_SYS_DDR_SDRAM_BASE	0x00000000	/* DDR is system memory*/
#define CONFIG_SYS_SDRAM_BASE		CONFIG_SYS_DDR_SDRAM_BASE

#define CONFIG_NUM_DDR_CONTROLLERS	1
#define CONFIG_DIMM_SLOTS_PER_CTLR	1
#define CONFIG_CHIP_SELECTS_PER_CTRL	(2 * CONFIG_DIMM_SLOTS_PER_CTLR)

/* I2C addresses of SPD EEPROMs */
#define SPD_EEPROM_ADDRESS	0x51	/* CTLR 0 DIMM 0 */

/* These are used when DDR doesn't use SPD.  */
#define CONFIG_SYS_SDRAM_SIZE	128		/* DDR is 128MB */
#define CONFIG_SYS_DDR_CS0_BNDS	0x00000007	/* 0-128MB */
#define CONFIG_SYS_DDR_CS0_CONFIG	0x80000002
#define CONFIG_SYS_DDR_TIMING_1	0x37344321
#define CONFIG_SYS_DDR_TIMING_2	0x00000800	/* P9-45,may need tuning */
#define CONFIG_SYS_DDR_CONTROL		0xc2000000	/* unbuffered,no DYN_PWR */
#define CONFIG_SYS_DDR_MODE		0x00000062	/* DLL,normal,seq,4/2.5 */
#define CONFIG_SYS_DDR_INTERVAL	0x05200100	/* autocharge,no open page */

/*
 * SDRAM on the Local Bus
 */
#define CONFIG_SYS_LBC_SDRAM_BASE	0xf0000000	/* Localbus SDRAM */
#define CONFIG_SYS_LBC_SDRAM_SIZE	64		/* LBC SDRAM is 64MB */

#define CONFIG_SYS_FLASH_BASE		0xff000000	/* start of FLASH 16M */
#define CONFIG_SYS_BR0_PRELIM		0xff001801	/* port size 32bit */

#define CONFIG_SYS_OR0_PRELIM		0xff006ff7	/* 16MB Flash */
#define CONFIG_SYS_MAX_FLASH_BANKS	1		/* number of banks */
#define CONFIG_SYS_MAX_FLASH_SECT	64		/* sectors per device */
#undef	CONFIG_SYS_FLASH_CHECKSUM
#define CONFIG_SYS_FLASH_ERASE_TOUT	60000	/* Flash Erase Timeout (ms) */
#define CONFIG_SYS_FLASH_WRITE_TOUT	500	/* Flash Write Timeout (ms) */

#define CONFIG_SYS_MONITOR_BASE	CONFIG_SYS_TEXT_BASE	/* start of monitor */

#if (CONFIG_SYS_MONITOR_BASE < CONFIG_SYS_FLASH_BASE)
#define CONFIG_SYS_RAMBOOT
#else
#undef  CONFIG_SYS_RAMBOOT
#endif

#define CONFIG_FLASH_CFI_DRIVER
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_SYS_FLASH_EMPTY_INFO

#undef CONFIG_CLOCKS_IN_MHZ


/*
 * Local Bus Definitions
 */

/*
 * Base Register 2 and Option Register 2 configure SDRAM.
 * The SDRAM base address, CONFIG_SYS_LBC_SDRAM_BASE, is 0xf0000000.
 *
 * For BR2, need:
 *    Base address of 0xf0000000 = BR[0:16] = 1111 0000 0000 0000 0
 *    port-size = 32-bits = BR2[19:20] = 11
 *    no parity checking = BR2[21:22] = 00
 *    SDRAM for MSEL = BR2[24:26] = 011
 *    Valid = BR[31] = 1
 *
 * 0    4    8    12   16   20   24   28
 * 1111 0000 0000 0000 0001 1000 0110 0001 = f0001861
 *
 * FIXME: CONFIG_SYS_LBC_SDRAM_BASE should be masked and OR'ed into
 * FIXME: the top 17 bits of BR2.
 */

#define CONFIG_SYS_BR2_PRELIM		0xf0001861

/*
 * The SDRAM size in MB, CONFIG_SYS_LBC_SDRAM_SIZE, is 64.
 *
 * For OR2, need:
 *    64MB mask for AM, OR2[0:7] = 1111 1100
 *		   XAM, OR2[17:18] = 11
 *    9 columns OR2[19-21] = 010
 *    13 rows   OR2[23-25] = 100
 *    EAD set for extra time OR[31] = 1
 *
 * 0    4    8    12   16   20   24   28
 * 1111 1100 0000 0000 0110 1001 0000 0001 = fc006901
 */

#define CONFIG_SYS_OR2_PRELIM		0xfc006901

#define CONFIG_SYS_LBC_LCRR		0x00030004    /* LB clock ratio reg */
#define CONFIG_SYS_LBC_LBCR		0x00000000    /* LB config reg */
#define CONFIG_SYS_LBC_LSRT		0x20000000    /* LB sdram refresh timer */
#define CONFIG_SYS_LBC_MRTPR		0x20000000    /* LB refresh timer prescal*/

#define CONFIG_SYS_LBC_LSDMR_COMMON	( LSDMR_BSMA1516	\
				| LSDMR_RFCR5		\
				| LSDMR_PRETOACT3	\
				| LSDMR_ACTTORW3	\
				| LSDMR_BL8		\
				| LSDMR_WRC2		\
				| LSDMR_CL3		\
				| LSDMR_RFEN		\
				)

/*
 * SDRAM Controller configuration sequence.
 */
#define CONFIG_SYS_LBC_LSDMR_1	(CONFIG_SYS_LBC_LSDMR_COMMON | LSDMR_OP_PCHALL)
#define CONFIG_SYS_LBC_LSDMR_2	(CONFIG_SYS_LBC_LSDMR_COMMON | LSDMR_OP_ARFRSH)
#define CONFIG_SYS_LBC_LSDMR_3	(CONFIG_SYS_LBC_LSDMR_COMMON | LSDMR_OP_ARFRSH)
#define CONFIG_SYS_LBC_LSDMR_4	(CONFIG_SYS_LBC_LSDMR_COMMON | LSDMR_OP_MRW)
#define CONFIG_SYS_LBC_LSDMR_5	(CONFIG_SYS_LBC_LSDMR_COMMON | LSDMR_OP_NORMAL)


/*
 * 32KB, 8-bit wide for ADS config reg
 */
#define CONFIG_SYS_BR4_PRELIM          0xf8000801
#define CONFIG_SYS_OR4_PRELIM		0xffffe1f1
#define CONFIG_SYS_BCSR		(CONFIG_SYS_BR4_PRELIM & 0xffff8000)

#define CONFIG_SYS_INIT_RAM_LOCK	1
#define CONFIG_SYS_INIT_RAM_ADDR	0xe4010000	/* Initial RAM address */
#define CONFIG_SYS_INIT_RAM_SIZE	0x4000		/* Size of used area in RAM */

#define CONFIG_SYS_GBL_DATA_OFFSET	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

#define CONFIG_SYS_MONITOR_LEN		(256 * 1024)    /* Reserve 256 kB for Mon */
#define CONFIG_SYS_MALLOC_LEN		(128 * 1024)    /* Reserved for malloc */

/* Serial Port */
#define CONFIG_CONS_ON_SCC	/* define if console on SCC */
#undef  CONFIG_CONS_NONE	/* define if console on something else */
#define CONFIG_CONS_INDEX       1  /* which serial channel for console */

#define CONFIG_BAUDRATE		115200

#define CONFIG_SYS_BAUDRATE_TABLE  \
	{300, 600, 1200, 2400, 4800, 9600, 19200, 38400,115200}

/* Use the HUSH parser */
#define CONFIG_SYS_HUSH_PARSER
#ifdef  CONFIG_SYS_HUSH_PARSER
#endif

/* pass open firmware flat tree */
#define CONFIG_OF_LIBFDT		1
#define CONFIG_OF_BOARD_SETUP		1
#define CONFIG_OF_STDOUT_VIA_ALIAS	1

/*
 * I2C
 */
#define CONFIG_FSL_I2C		/* Use FSL common I2C driver */
#define CONFIG_HARD_I2C		/* I2C with hardware support*/
#undef	CONFIG_SOFT_I2C			/* I2C bit-banged */
#define CONFIG_SYS_I2C_SPEED		400000	/* I2C speed and slave address */
#define CONFIG_SYS_I2C_SLAVE		0x7F
#define CONFIG_SYS_I2C_NOPROBES        {0x69}	/* Don't probe these addrs */
#define CONFIG_SYS_I2C_OFFSET		0x3000

/* RapidIO MMU */
#define CONFIG_SYS_RIO_MEM_VIRT	0xc0000000	/* base address */
#define CONFIG_SYS_RIO_MEM_BUS	0xc0000000	/* base address */
#define CONFIG_SYS_RIO_MEM_PHYS	0xc0000000
#define CONFIG_SYS_RIO_MEM_SIZE	0x20000000	/* 128M */

/*
 * General PCI
 * Memory space is mapped 1-1, but I/O space must start from 0.
 */
#define CONFIG_SYS_PCI1_MEM_VIRT	0x80000000
#define CONFIG_SYS_PCI1_MEM_BUS	0x80000000
#define CONFIG_SYS_PCI1_MEM_PHYS	0x80000000
#define CONFIG_SYS_PCI1_MEM_SIZE	0x20000000	/* 512M */
#define CONFIG_SYS_PCI1_IO_VIRT	0xe2000000
#define CONFIG_SYS_PCI1_IO_BUS	0x00000000
#define CONFIG_SYS_PCI1_IO_PHYS	0xe2000000
#define CONFIG_SYS_PCI1_IO_SIZE	0x100000	/* 1M */

#if defined(CONFIG_PCI)

#define CONFIG_PCI_PNP			/* do pci plug-and-play */

#undef CONFIG_EEPRO100
#undef CONFIG_TULIP

#if !defined(CONFIG_PCI_PNP)
    #define PCI_ENET0_IOADDR	0xe0000000
    #define PCI_ENET0_MEMADDR	0xe0000000
    #define PCI_IDSEL_NUMBER	0x0c	/* slot0->3(IDSEL)=12->15 */
#endif

#undef CONFIG_PCI_SCAN_SHOW		/* show pci devices on startup */
#define CONFIG_SYS_PCI_SUBSYS_VENDORID 0x1057  /* Motorola */

#endif	/* CONFIG_PCI */


#ifdef CONFIG_TSEC_ENET

#ifndef CONFIG_MII
#define CONFIG_MII		1	/* MII PHY management */
#endif
#define CONFIG_TSEC1	1
#define CONFIG_TSEC1_NAME	"TSEC0"
#define CONFIG_TSEC2	1
#define CONFIG_TSEC2_NAME	"TSEC1"
#define TSEC1_PHY_ADDR		0
#define TSEC2_PHY_ADDR		1
#define TSEC1_PHYIDX		0
#define TSEC2_PHYIDX		0
#define TSEC1_FLAGS		TSEC_GIGABIT
#define TSEC2_FLAGS		TSEC_GIGABIT

/* Options are: TSEC[0-1] */
#define CONFIG_ETHPRIME		"TSEC0"

#endif /* CONFIG_TSEC_ENET */

#ifdef CONFIG_ETHER_ON_FCC		/* CPM FCC Ethernet */

#undef  CONFIG_ETHER_NONE		/* define if ether on something else */
#define CONFIG_ETHER_INDEX      2       /* which channel for ether */

#if (CONFIG_ETHER_INDEX == 2)
  /*
   * - Rx-CLK is CLK13
   * - Tx-CLK is CLK14
   * - Select bus for bd/buffers
   * - Full duplex
   */
  #define CONFIG_SYS_CMXFCR_MASK2      (CMXFCR_FC2 | CMXFCR_RF2CS_MSK | CMXFCR_TF2CS_MSK)
  #define CONFIG_SYS_CMXFCR_VALUE2     (CMXFCR_RF2CS_CLK13 | CMXFCR_TF2CS_CLK14)
  #define CONFIG_SYS_CPMFCR_RAMTYPE    0
  #define CONFIG_SYS_FCC_PSMR          (FCC_PSMR_FDE)
  #define FETH2_RST		0x01
#elif (CONFIG_ETHER_INDEX == 3)
  /* need more definitions here for FE3 */
  #define FETH3_RST		0x80
#endif					/* CONFIG_ETHER_INDEX */

#ifndef CONFIG_MII
#define CONFIG_MII		1	/* MII PHY management */
#endif

#define CONFIG_BITBANGMII		/* bit-bang MII PHY management */

/*
 * GPIO pins used for bit-banged MII communications
 */
#define MDIO_PORT	2		/* Port C */
#define MDIO_DECLARE	volatile ioport_t *iop = ioport_addr ( \
				(immap_t *) CONFIG_SYS_IMMR, MDIO_PORT )
#define MDC_DECLARE	MDIO_DECLARE

#define MDIO_ACTIVE	(iop->pdir |=  0x00400000)
#define MDIO_TRISTATE	(iop->pdir &= ~0x00400000)
#define MDIO_READ	((iop->pdat &  0x00400000) != 0)

#define MDIO(bit)	if(bit) iop->pdat |=  0x00400000; \
			else	iop->pdat &= ~0x00400000

#define MDC(bit)	if(bit) iop->pdat |=  0x00200000; \
			else	iop->pdat &= ~0x00200000

#define MIIDELAY	udelay(1)

#endif


/*
 * Environment
 */
#ifndef CONFIG_SYS_RAMBOOT
  #define CONFIG_ENV_IS_IN_FLASH	1
  #define CONFIG_ENV_ADDR		(CONFIG_SYS_MONITOR_BASE + 0x40000)
  #define CONFIG_ENV_SECT_SIZE	0x40000	/* 256K(one sector) for env */
  #define CONFIG_ENV_SIZE		0x2000
#else
  #define CONFIG_SYS_NO_FLASH		1	/* Flash is not usable now */
  #define CONFIG_ENV_IS_NOWHERE	1	/* Store ENV in memory only */
  #define CONFIG_ENV_ADDR		(CONFIG_SYS_MONITOR_BASE - 0x1000)
  #define CONFIG_ENV_SIZE		0x2000
#endif

#define CONFIG_LOADS_ECHO	1	/* echo on for serial download */
#define CONFIG_SYS_LOADS_BAUD_CHANGE	1	/* allow baudrate change */

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

#define CONFIG_CMD_PING
#define CONFIG_CMD_I2C
#define CONFIG_CMD_ELF
#define CONFIG_CMD_IRQ
#define CONFIG_CMD_SETEXPR
#define CONFIG_CMD_REGINFO

#if defined(CONFIG_PCI)
    #define CONFIG_CMD_PCI
#endif

#if defined(CONFIG_ETHER_ON_FCC)
    #define CONFIG_CMD_MII
#endif

#if defined(CONFIG_SYS_RAMBOOT)
    #undef CONFIG_CMD_SAVEENV
    #undef CONFIG_CMD_LOADS
#endif


#undef CONFIG_WATCHDOG			/* watchdog disabled */

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP			/* undef to save memory	*/
#define CONFIG_CMDLINE_EDITING			/* Command-line editing */
#define CONFIG_AUTO_COMPLETE			/* add autocompletion support */
#define CONFIG_SYS_LOAD_ADDR	0x1000000	/* default load address */
#define CONFIG_SYS_PROMPT	"=> "		/* Monitor Command Prompt */

#if defined(CONFIG_CMD_KGDB)
    #define CONFIG_SYS_CBSIZE	1024		/* Console I/O Buffer Size */
#else
    #define CONFIG_SYS_CBSIZE	256		/* Console I/O Buffer Size */
#endif

#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16) /* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	16		/* max number of command args */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size */
#define CONFIG_SYS_HZ		1000		/* decrementer freq: 1ms ticks */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 64 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTMAPSZ	(64 << 20)	/* Initial Memory map for Linux*/
#define CONFIG_SYS_BOOTM_LEN	(64 << 20)	/* Increase max gunzip size */

#if defined(CONFIG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	230400	/* speed to run kgdb serial port */
#define CONFIG_KGDB_SER_INDEX	2	/* which serial port to use */
#endif


/*
 * Environment Configuration
 */

/* The mac addresses for all ethernet interface */
#if defined(CONFIG_TSEC_ENET) || defined(CONFIG_ETHER_ON_FCC)
#define CONFIG_HAS_ETH0
#define CONFIG_ETHADDR   00:E0:0C:00:00:FD
#define CONFIG_HAS_ETH1
#define CONFIG_ETH1ADDR  00:E0:0C:00:01:FD
#define CONFIG_HAS_ETH2
#define CONFIG_ETH2ADDR  00:E0:0C:00:02:FD
#define CONFIG_HAS_ETH3
#define CONFIG_ETH3ADDR  00:E0:0C:00:03:FD
#endif

#define CONFIG_IPADDR    192.168.1.253

#define CONFIG_HOSTNAME		unknown
#define CONFIG_ROOTPATH		"/nfsroot"
#define CONFIG_BOOTFILE		"your.uImage"

#define CONFIG_SERVERIP  192.168.1.1
#define CONFIG_GATEWAYIP 192.168.1.1
#define CONFIG_NETMASK   255.255.255.0

#define CONFIG_LOADADDR  200000	/* default location for tftp and bootm */

#define CONFIG_BOOTDELAY 10	/* -1 disables auto-boot */
#undef  CONFIG_BOOTARGS		/* the boot command will set bootargs */

#define CONFIG_BAUDRATE	115200

#define	CONFIG_EXTRA_ENV_SETTINGS				        \
	"netdev=eth0\0"							\
	"consoledev=ttyCPM\0"						\
	"ramdiskaddr=1000000\0"						\
	"ramdiskfile=your.ramdisk.u-boot\0"				\
	"fdtaddr=400000\0"						\
	"fdtfile=mpc8560ads.dtb\0"

#define CONFIG_NFSBOOTCOMMAND	                                        \
	"setenv bootargs root=/dev/nfs rw "				\
		"nfsroot=$serverip:$rootpath "				\
		"ip=$ipaddr:$serverip:$gatewayip:$netmask:$hostname:$netdev:off " \
		"console=$consoledev,$baudrate $othbootargs;"		\
	"tftp $loadaddr $bootfile;"					\
	"tftp $fdtaddr $fdtfile;"					\
	"bootm $loadaddr - $fdtaddr"

#define CONFIG_RAMBOOTCOMMAND \
	"setenv bootargs root=/dev/ram rw "				\
		"console=$consoledev,$baudrate $othbootargs;"		\
	"tftp $ramdiskaddr $ramdiskfile;"				\
	"tftp $loadaddr $bootfile;"					\
	"tftp $fdtaddr $fdtfile;"					\
	"bootm $loadaddr $ramdiskaddr $fdtaddr"

#define CONFIG_BOOTCOMMAND  CONFIG_NFSBOOTCOMMAND

#endif	/* __CONFIG_H */
