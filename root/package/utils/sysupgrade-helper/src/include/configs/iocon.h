/*
 * (C) Copyright 2010
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
#define CONFIG_IOCON		1	/*  on a IoCon board */

#define	CONFIG_SYS_TEXT_BASE	0xFFFC0000

/*
 * Include common defines/options for all AMCC eval boards
 */
#define CONFIG_HOSTNAME		iocon
#define CONFIG_IDENT_STRING	" iocon 0.04"
#include "amcc-common.h"

#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_BOARD_EARLY_INIT_R
#define CONFIG_LAST_STAGE_INIT

#define CONFIG_SYS_CLK_FREQ	33333333 /* external frequency to pll   */

/*
 * Configure PLL
 */
#define PLLMR0_DEFAULT PLLMR0_266_133_66
#define PLLMR1_DEFAULT PLLMR1_266_133_66

#undef CONFIG_ZERO_BOOTDELAY_CHECK	/* ignore keypress on bootdelay==0 */
#define CONFIG_AUTOBOOT_KEYED		/* use key strings to stop autoboot */
#define CONFIG_AUTOBOOT_STOP_STR " "

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
#define CONFIG_CONS_INDEX		1	/* Use UART0 */
#undef  CONFIG_SYS_EXT_SERIAL_CLOCK		/* external serial clock */
#undef  CONFIG_SYS_405_UART_ERRATA_59		/* 405GP/CR Rev. D silicon */
#define CONFIG_SYS_BASE_BAUD		691200

/*
 * I2C stuff
 */
#define CONFIG_SYS_I2C_SPEED		400000

/* enable I2C and select the hardware/software driver */
#undef  CONFIG_HARD_I2C			/* I2C with hardware support	*/
#define CONFIG_SOFT_I2C		1	/* I2C bit-banged		*/

/*
 * Software (bit-bang) I2C driver configuration
 */

#ifndef __ASSEMBLY__
void fpga_gpio_set(int pin);
void fpga_gpio_clear(int pin);
int fpga_gpio_get(int pin);
#endif

#define I2C_ACTIVE	{ }
#define I2C_TRISTATE	{ }
#define I2C_READ	fpga_gpio_get(0x0040) ? 1 : 0
#define I2C_SDA(bit)	if (bit) fpga_gpio_set(0x0040); \
			else fpga_gpio_clear(0x0040)
#define I2C_SCL(bit)	if (bit) fpga_gpio_set(0x0020); \
			else fpga_gpio_clear(0x0020)
#define I2C_DELAY	udelay(25)	/* 1/4 I2C clock duration */

/*
 * OSD hardware
 */
#define CONFIG_SYS_MPC92469AC
#define CONFIG_SYS_CH7301

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
{ GPIO_BASE, GPIO_IN,  GPIO_SEL,  GPIO_OUT_NO_CHG }, /* GPIO6	TS4 */ \
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
#define CONFIG_SYS_INIT_RAM_END	CONFIG_SYS_OCM_DATA_SIZE /* End of used area */

#define CONFIG_SYS_GBL_DATA_SIZE	128  /* size/bytes res'd for init data*/
#define CONFIG_SYS_GBL_DATA_OFFSET \
	(CONFIG_SYS_INIT_RAM_END - CONFIG_SYS_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

/*
 * External Bus Controller (EBC) Setup
 */

/* Memory Bank 0 (NOR-FLASH) initialization */
#define CONFIG_SYS_EBC_PB0AP		0xa382a880
#define CONFIG_SYS_EBC_PB0CR		0xFC0DA000

/* Memory Bank 1 (NVRAM) initializatio */
#define CONFIG_SYS_EBC_PB1AP		0x92015480
#define CONFIG_SYS_EBC_PB1CR		0xFB858000

/* Memory Bank 2 (FPGA0) initialization */
#define CONFIG_SYS_FPGA0_BASE		0x7f100000
#define CONFIG_SYS_EBC_PB2AP		0x02825080
#define CONFIG_SYS_EBC_PB2CR		(CONFIG_SYS_FPGA0_BASE | 0x1a000)

#define CONFIG_SYS_FPGA_BASE(k)		CONFIG_SYS_FPGA0_BASE
#define CONFIG_SYS_FPGA_DONE(k)		0x0010

#define CONFIG_SYS_FPGA_COUNT		1

/* Memory Bank 3 (Latches) initialization */
#define CONFIG_SYS_LATCH_BASE		0x7f200000
#define CONFIG_SYS_EBC_PB3AP		0x02025080
#define CONFIG_SYS_EBC_PB3CR		0x7f21a000

#define CONFIG_SYS_LATCH0_RESET		0xffef
#define CONFIG_SYS_LATCH0_BOOT		0xffff
#define CONFIG_SYS_LATCH1_RESET		0xffff
#define CONFIG_SYS_LATCH1_BOOT		0xffff

/*
 * OSD Setup
 */
#define CONFIG_SYS_MPC92469AC
#define CONFIG_SYS_CH7301
#define CONFIG_SYS_OSD_SCREENS		CONFIG_SYS_FPGA_COUNT

#endif	/* __CONFIG_H */
