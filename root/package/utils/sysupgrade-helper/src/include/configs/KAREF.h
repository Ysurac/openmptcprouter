/*
 * (C) Copyright 2004 Sandburst Corporation
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

/************************************************************************
 * KAMINOREFDES.h - configuration for the Sandburst Kamino Reference
 *		    design.
 ***********************************************************************/

/*
 * $Id: KAREF.h,v 1.6 2005/06/03 15:05:25 tsawyer Exp $
 *
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*-----------------------------------------------------------------------
 * High Level Configuration Options
 *----------------------------------------------------------------------*/
#define CONFIG_KAREF	     1		/* Board is Kamino Ref Variant */
#define CONFIG_440GX		  1	     /* Specifc GX support	*/
#define CONFIG_440		  1	     /* ... PPC440 family	*/
#define CONFIG_4xx		  1	     /* ... PPC4xx family	*/
#define CONFIG_BOARD_EARLY_INIT_F 1	     /* Call board_pre_init	*/
#define CONFIG_MISC_INIT_F	  1	     /* Call board misc_init_f	*/
#define CONFIG_MISC_INIT_R	  1	     /* Call board misc_init_r	*/

#define	CONFIG_SYS_TEXT_BASE	0xFFF80000

#undef	CONFIG_SYS_DRAM_TEST			     /* Disable-takes long time!*/
#define CONFIG_SYS_CLK_FREQ	  66666666   /* external freq to pll	*/

#define CONFIG_VERY_BIG_RAM 1
#define CONFIG_VERSION_VARIABLE

#define CONFIG_IDENT_STRING " Sandburst Kamino Reference Design"

/*-----------------------------------------------------------------------
 * Base addresses -- Note these are effective addresses where the
 * actual resources get mapped (not physical addresses)
 *----------------------------------------------------------------------*/
#define CONFIG_SYS_SDRAM_BASE	       0x00000000    /* _must_ be 0		*/
#define CONFIG_SYS_FLASH_BASE	       0xfff80000    /* start of FLASH		*/
#define CONFIG_SYS_MONITOR_BASE       0xfff80000    /* start of monitor	*/
#define CONFIG_SYS_PCI_MEMBASE	       0x80000000    /* mapped pci memory	*/
#define CONFIG_SYS_ISRAM_BASE	       0xc0000000    /* internal SRAM		*/
#define CONFIG_SYS_PCI_BASE	       0xd0000000    /* internal PCI regs	*/

#define CONFIG_SYS_NVRAM_BASE_ADDR   (CONFIG_SYS_PERIPHERAL_BASE + 0x08000000)
#define CONFIG_SYS_KAREF_FPGA_BASE   (CONFIG_SYS_PERIPHERAL_BASE + 0x08200000)
#define CONFIG_SYS_OFEM_FPGA_BASE    (CONFIG_SYS_PERIPHERAL_BASE + 0x08400000)
#define CONFIG_SYS_BME32_BASE	      (CONFIG_SYS_PERIPHERAL_BASE + 0x08500000)
#define CONFIG_SYS_GPIO_BASE	      (CONFIG_SYS_PERIPHERAL_BASE + 0x00000700)

/* Here for completeness */
#define CONFIG_SYS_OFEMAC_BASE	      (CONFIG_SYS_PERIPHERAL_BASE + 0x08600000)

/*-----------------------------------------------------------------------
 * Initial RAM & stack pointer (placed in internal SRAM)
 *----------------------------------------------------------------------*/
#define CONFIG_SYS_TEMP_STACK_OCM    1
#define CONFIG_SYS_OCM_DATA_ADDR     CONFIG_SYS_ISRAM_BASE
#define CONFIG_SYS_INIT_RAM_ADDR     CONFIG_SYS_ISRAM_BASE /* Initial RAM address	*/
#define CONFIG_SYS_INIT_RAM_SIZE      0x2000	     /* Size of used area in RAM */

#define CONFIG_SYS_GBL_DATA_OFFSET   (CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET    (CONFIG_SYS_GBL_DATA_OFFSET - 0x4)

#define CONFIG_SYS_MONITOR_LEN	      (256 * 1024)   /* Rsrv 256kB for Mon	*/
#define CONFIG_SYS_MALLOC_LEN	      (128 * 1024)   /* Rsrv 128kB for malloc	*/

/*-----------------------------------------------------------------------
 * Serial Port
 *----------------------------------------------------------------------*/
#define CONFIG_CONS_INDEX	1	/* Use UART0			*/
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	1
#define CONFIG_SYS_NS16550_CLK		get_serial_clock()
#define CONFIG_SERIAL_MULTI   1
#define CONFIG_BAUDRATE	      9600

#define CONFIG_SYS_BAUDRATE_TABLE  \
    {300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200}

/*-----------------------------------------------------------------------
 * NVRAM/RTC
 *
 * NOTE: Upper 8 bytes of NVRAM is where the RTC registers are located.
 * The DS1743 code assumes this condition (i.e. -- it assumes the base
 * address for the RTC registers is:
 *
 *	CONFIG_SYS_NVRAM_BASE_ADDR + CONFIG_SYS_NVRAM_SIZE
 *
 *----------------------------------------------------------------------*/
#define CONFIG_SYS_NVRAM_SIZE	      (0x2000 - 8)   /* NVRAM size(8k)- RTC regs*/
#define CONFIG_RTC_DS174x     1		     /* DS1743 RTC		*/

/*-----------------------------------------------------------------------
 * FLASH related
 *----------------------------------------------------------------------*/
#define CONFIG_SYS_MAX_FLASH_BANKS   1		     /* number of banks		*/
#define CONFIG_SYS_MAX_FLASH_SECT    8		     /* sectors per device	*/

#undef	CONFIG_SYS_FLASH_CHECKSUM
#define CONFIG_SYS_FLASH_ERASE_TOUT  120000	     /* Flash Erase TO (in ms)	 */
#define CONFIG_SYS_FLASH_WRITE_TOUT  500	     /* Flash Write TO(in ms)	 */

/*-----------------------------------------------------------------------
 * DDR SDRAM
 *----------------------------------------------------------------------*/
#define CONFIG_SPD_EEPROM     1		     /* Use SPD EEPROM for setup*/
#define SPD_EEPROM_ADDRESS    {0x53}	     /* SPD i2c spd addresses	*/

/*-----------------------------------------------------------------------
 * I2C
 *----------------------------------------------------------------------*/
#define CONFIG_HARD_I2C	      1		     /* I2C hardware support	*/
#undef	CONFIG_SOFT_I2C			     /* I2C !bit-banged		*/
#define CONFIG_PPC4XX_I2C		/* use PPC4xx driver		*/
#define CONFIG_SYS_I2C_SPEED	      400000	     /* I2C speed 400kHz	*/
#define CONFIG_SYS_I2C_SLAVE	      0x7F	     /* I2C slave address	*/
#define CONFIG_SYS_I2C_NOPROBES      {0x69}	     /* Don't probe these addrs */
#define CONFIG_I2C_BUS1	      1		     /* Include i2c bus 1 supp	*/


/*-----------------------------------------------------------------------
 * Environment
 *----------------------------------------------------------------------*/
#define CONFIG_ENV_IS_IN_NVRAM   1		     /* Environment uses NVRAM	*/
#undef	CONFIG_ENV_IS_IN_FLASH		     /* ... not in flash	*/
#undef	CONFIG_ENV_IS_IN_EEPROM		     /* ... not in EEPROM	*/
#define CONFIG_ENV_OVERWRITE  1		     /* allow env overwrite	*/

#define CONFIG_ENV_SIZE	      0x1000	     /* Size of Env vars	*/
#define CONFIG_ENV_ADDR	      (CONFIG_SYS_NVRAM_BASE_ADDR)

#define CONFIG_BOOTDELAY      5		    /* 5 second autoboot */

#define CONFIG_LOADS_ECHO     1		     /* echo on for serial dnld */
#define CONFIG_SYS_LOADS_BAUD_CHANGE 1		     /* allow baudrate change	*/

/*-----------------------------------------------------------------------
 * Networking
 *----------------------------------------------------------------------*/
#define CONFIG_PPC4xx_EMAC
#define CONFIG_MII	      1		     /* MII PHY management	*/
#define CONFIG_PHY_ADDR	      0xff	     /* no phy on EMAC0		*/
#define CONFIG_PHY1_ADDR      0xff	     /* no phy on EMAC1		*/
#define CONFIG_PHY2_ADDR      0x08	     /* PHY addr, MGMT, EMAC2	*/
#define CONFIG_PHY3_ADDR      0x18	     /* PHY addr, LCL, EMAC3	*/
#define CONFIG_HAS_ETH0
#define CONFIG_HAS_ETH1
#define CONFIG_HAS_ETH2
#define CONFIG_HAS_ETH3
#define CONFIG_PHY_RESET      1              /* reset phy upon startup  */
#define CONFIG_CIS8201_PHY    1		     /* RGMII mode for Cicada	*/
#define CONFIG_CIS8201_SHORT_ETCH 1	     /* Use short etch mode	*/
#define CONFIG_PHY_GIGE	      1		     /* GbE speed/duplex detect */
#define CONFIG_PHY_RESET_DELAY 1000
#define CONFIG_NETMASK	      255.255.0.0
#define CONFIG_ETHADDR	      00:00:00:00:00:00 /* No EMAC 0 support	*/
#define CONFIG_ETH1ADDR	      00:00:00:00:00:00 /* No EMAC 1 support	*/
#define CONFIG_SYS_RX_ETH_BUFFER     32	     /* #eth rx buff & descrs	*/


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

#define CONFIG_CMD_PCI
#define CONFIG_CMD_IRQ
#define CONFIG_CMD_I2C
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_DATE
#define CONFIG_CMD_BEDBUG
#define CONFIG_CMD_PING
#define CONFIG_CMD_DIAG
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET
#define CONFIG_CMD_ELF
#define CONFIG_CMD_IDE
#define CONFIG_CMD_FAT


/* Include NetConsole support */
#define CONFIG_NETCONSOLE

/* Include auto complete with tabs */
#define CONFIG_AUTO_COMPLETE 1
#define CONFIG_SYS_ALT_MEMTEST	     1	     /* use real memory test	 */

#define CONFIG_SYS_LONGHELP			     /* undef to save memory	*/
#define CONFIG_SYS_PROMPT	      "KaRefDes=> "  /* Monitor Command Prompt	*/

#define CONFIG_SYS_HUSH_PARSER	       1	     /* HUSH for ext'd cli	*/


/*-----------------------------------------------------------------------
 * Console Buffer
 *----------------------------------------------------------------------*/
#if defined(CONFIG_CMD_KGDB)
#define CONFIG_SYS_CBSIZE	      1024	     /* Console I/O Buffer Size */
#else
#define CONFIG_SYS_CBSIZE	      256	     /* Console I/O Buffer Size */
#endif
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
					     /* Print Buffer Size	*/
#define CONFIG_SYS_MAXARGS	      16	     /* max number of cmd args	*/
#define CONFIG_SYS_BARGSIZE	      CONFIG_SYS_CBSIZE     /* Boot Arg Buffer Size	*/

/*-----------------------------------------------------------------------
 * Memory Test
 *----------------------------------------------------------------------*/
#define CONFIG_SYS_MEMTEST_START     0x0400000	     /* memtest works on	*/
#define CONFIG_SYS_MEMTEST_END	      0x0C00000	     /* 4 ... 12 MB in DRAM	*/

/*-----------------------------------------------------------------------
 * Compact Flash (in true IDE mode)
 *----------------------------------------------------------------------*/
#undef	CONFIG_IDE_8xx_DIRECT		/* no pcmcia interface required */
#undef	CONFIG_IDE_LED			/* no led for ide supported	*/

#define CONFIG_IDE_RESET		/* reset for ide supported	*/
#define CONFIG_SYS_IDE_MAXBUS		1	/* max. 1 IDE busses	*/
#define CONFIG_SYS_IDE_MAXDEVICE	(CONFIG_SYS_IDE_MAXBUS*1) /* max. 1 drives per IDE bus */

#define CONFIG_SYS_ATA_BASE_ADDR	0xF0000000
#define CONFIG_SYS_ATA_IDE0_OFFSET	0x0000
#define CONFIG_SYS_ATA_DATA_OFFSET	0x0000	 /* Offset for data I/O */
#define CONFIG_SYS_ATA_REG_OFFSET	0x0000	 /* Offset for normal register accesses*/
#define CONFIG_SYS_ATA_ALT_OFFSET	0x100000 /* Offset for alternate registers */

#define CONFIG_SYS_ATA_STRIDE		2	 /* Directly connected CF, needs a stride
					    to get to the correct offset */
#define CONFIG_DOS_PARTITION  1		     /* Include dos partition	*/

/*-----------------------------------------------------------------------
 * PCI
 *----------------------------------------------------------------------*/
/* General PCI */
#define CONFIG_PCI			     /* include pci support	*/
#define CONFIG_PCI_PNP			     /* do pci plug-and-play	*/
#define CONFIG_PCI_SCAN_SHOW		     /* show pci devices	*/
#define CONFIG_SYS_PCI_TARGBASE      (CONFIG_SYS_PCI_MEMBASE)

/* Board-specific PCI */
#define CONFIG_SYS_PCI_TARGET_INIT		     /* let board init pci target*/

#define CONFIG_SYS_PCI_SUBSYS_VENDORID 0x17BA	     /* Sandburst */
#define CONFIG_SYS_PCI_SUBSYS_DEVICEID 0xcafe	     /* Whatever */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTMAPSZ		(8 << 20) /* Initial Memory map for Linux */

#if defined(CONFIG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE  230400	     /* kgdb serial port baud	*/
#define CONFIG_KGDB_SER_INDEX 2		     /* kgdb serial port	*/
#endif

/*-----------------------------------------------------------------------
 * Miscellaneous configurable options
 *----------------------------------------------------------------------*/
#undef CONFIG_WATCHDOG			     /* watchdog disabled	*/
#define CONFIG_SYS_LOAD_ADDR	      0x8000000	     /* default load address	*/
#define CONFIG_SYS_EXTBDINFO	      1		     /* use extended board_info */

#define CONFIG_SYS_HZ		      100	     /* decr freq: 1 ms ticks	*/


#endif	/* __CONFIG_H */
