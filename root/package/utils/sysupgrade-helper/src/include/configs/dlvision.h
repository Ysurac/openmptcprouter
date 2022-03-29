/*
 * (C) Copyright 2009
 * Dirk Eibach,  Guntermann & Drunck GmbH, eibach@gdsys.de
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

#define CONFIG_405EP		1	/* this is a PPC405 CPU */
#define CONFIG_4xx		1	/*  member of PPC4xx family */
#define CONFIG_DLVISION	        1	/*  on a Neo board */

#define	CONFIG_SYS_TEXT_BASE	0xFFFC0000

/*
 * Include common defines/options for all AMCC eval boards
 */
#define CONFIG_HOSTNAME		dlvision
#define CONFIG_IDENT_STRING	" dlvision 0.01"
#include "amcc-common.h"

#define CONFIG_BOARD_EARLY_INIT_F	/* call board_early_init_f */
#define CONFIG_MISC_INIT_R		/* call misc_init_r */

#define CONFIG_SYS_CLK_FREQ	33333333 /* external frequency to pll   */

/*
 * Configure PLL
 */
#define PLLMR0_DEFAULT PLLMR0_266_133_66_33
#define PLLMR1_DEFAULT PLLMR1_266_133_66_33

/* new uImage format support */
#define CONFIG_FIT
#define CONFIG_FIT_VERBOSE	/* enable fit_format_{error,warning}() */

#define CONFIG_ENV_IS_IN_FLASH	/* use FLASH for environment vars */

/*
 * Default environment variables
 */
#define	CONFIG_EXTRA_ENV_SETTINGS					\
	CONFIG_AMCC_DEF_ENV						\
	CONFIG_AMCC_DEF_ENV_POWERPC					\
	CONFIG_AMCC_DEF_ENV_NOR_UPD					\
	"kernel_addr=fc000000\0"					\
	"fdt_addr=fc1e0000\0"						\
	"ramdisk_addr=fc200000\0"					\
	""

#define CONFIG_PHY_ADDR		4	/* PHY address			*/
#define CONFIG_HAS_ETH0
#define CONFIG_HAS_ETH1
#define CONFIG_PHY1_ADDR	0xc	/* EMAC1 PHY address		*/
#define CONFIG_PHY_CLK_FREQ    EMAC_STACR_CLK_66MHZ

/*
 * Commands additional to the ones defined in amcc-common.h
 */
#define CONFIG_CMD_CACHE
#undef CONFIG_CMD_EEPROM

/*
 * SDRAM configuration (please see cpu/ppc/sdram.[ch])
 */
#define CONFIG_SDRAM_BANK0	1	/* init onboard SDRAM bank 0 */

/* SDRAM timings used in datasheet */
#define CONFIG_SYS_SDRAM_CL             3	/* CAS latency */
#define CONFIG_SYS_SDRAM_tRP           20	/* PRECHARGE command period */
#define CONFIG_SYS_SDRAM_tRC           66	/* ACTIVE-to-ACTIVE period */
#define CONFIG_SYS_SDRAM_tRCD          20	/* ACTIVE-to-READ delay */
#define CONFIG_SYS_SDRAM_tRFC          66	/* Auto refresh period */

/*
 * If CONFIG_SYS_EXT_SERIAL_CLOCK, then the UART divisor is 1.
 * If CONFIG_SYS_405_UART_ERRATA_59, then UART divisor is 31.
 * Otherwise, UART divisor is determined by CPU Clock and CONFIG_SYS_BASE_BAUD.
 * The Linux BASE_BAUD define should match this configuration.
 *    baseBaud = cpuClock/(uartDivisor*16)
 * If CONFIG_SYS_405_UART_ERRATA_59 and 200MHz CPU clock,
 * set Linux BASE_BAUD to 403200.
 */
#define CONFIG_CONS_INDEX	1	/* Use UART0			*/
#undef  CONFIG_SYS_EXT_SERIAL_CLOCK           /* external serial clock */
#undef  CONFIG_SYS_405_UART_ERRATA_59         /* 405GP/CR Rev. D silicon */
#define CONFIG_SYS_BASE_BAUD		691200

/*
 * I2C stuff
 */
#define CONFIG_SYS_I2C_SPEED		100000	/* I2C speed and slave address*/

/*
 * FLASH organization
 */
#define CONFIG_SYS_FLASH_CFI		/* The flash is CFI compatible	*/
#define CONFIG_FLASH_CFI_DRIVER		/* Use common CFI driver	*/

#define CONFIG_SYS_FLASH_BASE		0xFC000000
#define CONFIG_SYS_FLASH_BANKS_LIST	{ CONFIG_SYS_FLASH_BASE }

#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max num of memory banks */
#define CONFIG_SYS_MAX_FLASH_SECT	512	/* max num of sectors per chip*/

#define CONFIG_SYS_FLASH_ERASE_TOUT	120000	/* Timeout for Flash Erase/ms */
#define CONFIG_SYS_FLASH_WRITE_TOUT	500	/* Timeout for Flash Write/ms */

#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE 1	/* use buff'd writes */
#define CONFIG_SYS_FLASH_PROTECTION	1	/* use hardware flash protect */

#define CONFIG_SYS_FLASH_EMPTY_INFO	/* 'E' for empty sector on flinfo */
#define CONFIG_SYS_FLASH_QUIET_TEST	1	/* no warn upon unknown flash */

#ifdef CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_SECT_SIZE	0x20000	/* size of one complete sector */
#define CONFIG_ENV_ADDR		((-CONFIG_SYS_MONITOR_LEN)-CONFIG_ENV_SECT_SIZE)
#define	CONFIG_ENV_SIZE		0x2000	/* Total Size of Environment Sector */

/* Address and size of Redundant Environment Sector	*/
#define CONFIG_ENV_ADDR_REDUND	(CONFIG_ENV_ADDR-CONFIG_ENV_SECT_SIZE)
#define CONFIG_ENV_SIZE_REDUND	(CONFIG_ENV_SIZE)
#endif

/*
 * PPC405 GPIO Configuration
 */
#define CONFIG_SYS_4xx_GPIO_TABLE { /* GPIO	Alternate1	*/ \
{ \
/* GPIO Core 0 */ \
{ GPIO_BASE, GPIO_OUT, GPIO_ALT1, GPIO_OUT_NO_CHG }, /* GPIO0	PerBLast */ \
{ GPIO_BASE, GPIO_OUT, GPIO_SEL,  GPIO_OUT_NO_CHG }, /* GPIO1	TS1E */ \
{ GPIO_BASE, GPIO_OUT, GPIO_SEL,  GPIO_OUT_NO_CHG }, /* GPIO2	TS2E */ \
{ GPIO_BASE, GPIO_IN,  GPIO_SEL,  GPIO_OUT_NO_CHG }, /* GPIO3	TS1O */ \
{ GPIO_BASE, GPIO_OUT, GPIO_SEL,  GPIO_OUT_NO_CHG }, /* GPIO4	TS2O */ \
{ GPIO_BASE, GPIO_OUT, GPIO_SEL,  GPIO_OUT_1      }, /* GPIO5	TS3 */ \
{ GPIO_BASE, GPIO_OUT, GPIO_SEL,  GPIO_OUT_NO_CHG }, /* GPIO6	TS4 */ \
{ GPIO_BASE, GPIO_OUT, GPIO_SEL,  GPIO_OUT_NO_CHG }, /* GPIO7	TS5 */ \
{ GPIO_BASE, GPIO_OUT, GPIO_SEL,  GPIO_OUT_NO_CHG }, /* GPIO8	TS6 */ \
{ GPIO_BASE, GPIO_OUT, GPIO_SEL,  GPIO_OUT_NO_CHG }, /* GPIO9	TrcClk */ \
{ GPIO_BASE, GPIO_OUT, GPIO_ALT1, GPIO_OUT_NO_CHG }, /* GPIO10	PerCS1 */ \
{ GPIO_BASE, GPIO_OUT, GPIO_ALT1, GPIO_OUT_NO_CHG }, /* GPIO11	PerCS2 */ \
{ GPIO_BASE, GPIO_OUT, GPIO_ALT1, GPIO_OUT_NO_CHG }, /* GPIO12	PerCS3 */ \
{ GPIO_BASE, GPIO_OUT, GPIO_ALT1, GPIO_OUT_NO_CHG }, /* GPIO13	PerCS4 */ \
{ GPIO_BASE, GPIO_OUT, GPIO_ALT1, GPIO_OUT_NO_CHG }, /* GPIO14	PerAddr03 */ \
{ GPIO_BASE, GPIO_OUT, GPIO_ALT1, GPIO_OUT_NO_CHG }, /* GPIO15	PerAddr04 */ \
{ GPIO_BASE, GPIO_OUT, GPIO_ALT1, GPIO_OUT_NO_CHG }, /* GPIO16	PerAddr05 */ \
{ GPIO_BASE, GPIO_IN,  GPIO_ALT1, GPIO_OUT_NO_CHG }, /* GPIO17	IRQ0 */ \
{ GPIO_BASE, GPIO_IN,  GPIO_ALT1, GPIO_OUT_NO_CHG }, /* GPIO18	IRQ1 */ \
{ GPIO_BASE, GPIO_IN,  GPIO_ALT1, GPIO_OUT_NO_CHG }, /* GPIO19	IRQ2 */ \
{ GPIO_BASE, GPIO_IN,  GPIO_SEL,  GPIO_OUT_NO_CHG }, /* GPIO20	IRQ3 */ \
{ GPIO_BASE, GPIO_OUT, GPIO_SEL,  GPIO_OUT_NO_CHG }, /* GPIO21	IRQ4 */ \
{ GPIO_BASE, GPIO_OUT, GPIO_SEL,  GPIO_OUT_NO_CHG }, /* GPIO22	IRQ5 */ \
{ GPIO_BASE, GPIO_IN,  GPIO_SEL,  GPIO_OUT_NO_CHG }, /* GPIO23	IRQ6 */ \
{ GPIO_BASE, GPIO_IN,  GPIO_ALT1, GPIO_OUT_NO_CHG }, /* GPIO24	UART0_DCD */ \
{ GPIO_BASE, GPIO_IN,  GPIO_ALT1, GPIO_OUT_NO_CHG }, /* GPIO25	UART0_DSR */ \
{ GPIO_BASE, GPIO_IN,  GPIO_ALT1, GPIO_OUT_NO_CHG }, /* GPIO26	UART0_RI */ \
{ GPIO_BASE, GPIO_OUT, GPIO_ALT1, GPIO_OUT_NO_CHG }, /* GPIO27	UART0_DTR */ \
{ GPIO_BASE, GPIO_IN,  GPIO_ALT1, GPIO_OUT_NO_CHG }, /* GPIO28	UART1_Rx */ \
{ GPIO_BASE, GPIO_OUT, GPIO_ALT1, GPIO_OUT_NO_CHG }, /* GPIO29	UART1_Tx */ \
{ GPIO_BASE, GPIO_OUT, GPIO_SEL,  GPIO_OUT_NO_CHG }, /* GPIO30	RejectPkt0 */ \
{ GPIO_BASE, GPIO_IN,  GPIO_SEL,  GPIO_OUT_NO_CHG }, /* GPIO31	RejectPkt1 */ \
} \
}

/*
 * Definitions for initial stack pointer and data area (in data cache)
 */
/* use on chip memory (OCM) for temperary stack until sdram is tested */
#define CONFIG_SYS_TEMP_STACK_OCM        1

/* On Chip Memory location */
#define CONFIG_SYS_OCM_DATA_ADDR	0xF8000000
#define CONFIG_SYS_OCM_DATA_SIZE	0x1000
#define CONFIG_SYS_INIT_RAM_ADDR	CONFIG_SYS_OCM_DATA_ADDR /* in SDRAM */
#define CONFIG_SYS_INIT_RAM_SIZE	CONFIG_SYS_OCM_DATA_SIZE /* Size of used area */

#define CONFIG_SYS_GBL_DATA_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

/*
 * External Bus Controller (EBC) Setup
 */

/* Memory Bank 0 (NOR-FLASH) initialization */
#define CONFIG_SYS_EBC_PB0AP		0x92015480
/* BAS=0xFC0,BS=64MB,BU=R/W,BW=16bit */
#define CONFIG_SYS_EBC_PB0CR		0xFC0DA000

/* Memory Bank 1 (NVRAM) initializatio */
#define CONFIG_SYS_EBC_PB1AP		0x92015480
/* BAS=0xFF8,BS=4MB,BU=R/W,BW=8bit  */
#define CONFIG_SYS_EBC_PB1CR		0xFB858000

/* Memory Bank 2 (UART) initialization */
#define CONFIG_UART_BASE		0x7f100000
#define CONFIG_SYS_EBC_PB2AP		0x92015480
/* BAS=0x7f1,BS=1MB,BU=R/W,BW=8bit */
#define CONFIG_SYS_EBC_PB2CR		0x7f118000

/* Memory Bank 3 (Latches) initialization */
#define CONFIG_SYS_LATCH_BASE		0x7f200000
#define CONFIG_SYS_EBC_PB3AP		0x92015480
/* BAS=0x7f2,BS=1MB,BU=R/W,BW=16bit */
#define CONFIG_SYS_EBC_PB3CR		0x7f21a000

#endif	/* __CONFIG_H */
