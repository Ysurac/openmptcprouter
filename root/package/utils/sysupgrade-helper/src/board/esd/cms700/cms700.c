/*
 * (C) Copyright 2005-2007
 * Matthias Fuchs, esd gmbh germany, matthias.fuchs@esd-electronics.com
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/processor.h>
#include <asm/io.h>
#include <command.h>
#include <malloc.h>

DECLARE_GLOBAL_DATA_PTR;

extern void lxt971_no_sleep(void);

int board_early_init_f (void)
{
	/*
	 * IRQ 0-15  405GP internally generated; active high; level sensitive
	 * IRQ 16    405GP internally generated; active low; level sensitive
	 * IRQ 17-24 RESERVED
	 * IRQ 25 (EXT IRQ 0) CAN0; active low; level sensitive
	 * IRQ 26 (EXT IRQ 1) SER0 ; active low; level sensitive
	 * IRQ 27 (EXT IRQ 2) SER1; active low; level sensitive
	 * IRQ 28 (EXT IRQ 3) FPGA 0; active low; level sensitive
	 * IRQ 29 (EXT IRQ 4) FPGA 1; active low; level sensitive
	 * IRQ 30 (EXT IRQ 5) PCI INTA; active low; level sensitive
	 * IRQ 31 (EXT IRQ 6) COMPACT FLASH; active high; level sensitive
	 */
	mtdcr(UIC0SR, 0xFFFFFFFF);       /* clear all ints */
	mtdcr(UIC0ER, 0x00000000);       /* disable all ints */
	mtdcr(UIC0CR, 0x00000000);       /* set all to be non-critical*/
	mtdcr(UIC0PR, 0xFFFFFF80);       /* set int polarities */
	mtdcr(UIC0TR, 0x10000000);       /* set int trigger levels */
	mtdcr(UIC0VCR, 0x00000001);      /* set vect base=0,INT0 highest priority*/
	mtdcr(UIC0SR, 0xFFFFFFFF);       /* clear all ints */

	/*
	 * EBC Configuration Register: set ready timeout to 512 ebc-clks -> ca. 15 us
	 */
	mtebc (EBC0_CFG, 0xa8400000); /* ebc always driven */

	/*
	 * Reset CPLD via GPIO12 (CS3) pin
	 */
	out_be32((void *)GPIO0_OR, in_be32((void *)GPIO0_OR) & ~CONFIG_SYS_PLD_RESET);
	udelay(1000); /* wait 1ms */
	out_be32((void *)GPIO0_OR, in_be32((void *)GPIO0_OR) | CONFIG_SYS_PLD_RESET);
	udelay(1000); /* wait 1ms */

	return 0;
}

int misc_init_r (void)
{
	/* adjust flash start and offset */
	gd->bd->bi_flashstart = 0 - gd->bd->bi_flashsize;
	gd->bd->bi_flashoffset = 0;

	/*
	 * Setup and enable EEPROM write protection
	 */
	out_be32((void *)GPIO0_OR, in_be32((void *)GPIO0_OR) | CONFIG_SYS_EEPROM_WP);

	return (0);
}


/*
 * Check Board Identity:
 */
#define LED_REG (CONFIG_SYS_PLD_BASE + 0x1000)
int checkboard (void)
{
	char str[64];
	int flashcnt;
	int delay;

	puts ("Board: ");

	if (getenv_f("serial#", str, sizeof(str))  == -1) {
		puts ("### No HW ID - assuming CMS700");
	} else {
		puts(str);
	}

	printf(" (PLD-Version=%02d)\n",
	       in_8((void *)(CONFIG_SYS_PLD_BASE + 0x1001)));

	/*
	 * Flash LEDs
	 */
	for (flashcnt = 0; flashcnt < 3; flashcnt++) {
		out_8((void *)LED_REG, 0x00); /* LEDs off */
		for (delay = 0; delay < 100; delay++)
			udelay(1000);
		out_8((void *)LED_REG, 0x0f); /* LEDs on */
		for (delay = 0; delay < 50; delay++)
			udelay(1000);
	}
	out_8((void *)LED_REG, 0x70);

	return 0;
}

/* ------------------------------------------------------------------------- */

#if defined(CONFIG_SYS_EEPROM_WREN)
/* Input: <dev_addr>  I2C address of EEPROM device to enable.
 *         <state>     -1: deliver current state
 *	               0: disable write
 *		       1: enable write
 *  Returns:           -1: wrong device address
 *                      0: dis-/en- able done
 *		     0/1: current state if <state> was -1.
 */
int eeprom_write_enable (unsigned dev_addr, int state)
{
	if (CONFIG_SYS_I2C_EEPROM_ADDR != dev_addr) {
		return -1;
	} else {
		switch (state) {
		case 1:
			/* Enable write access, clear bit GPIO_SINT2. */
			out_be32((void *)GPIO0_OR, in_be32((void *)GPIO0_OR) & ~CONFIG_SYS_EEPROM_WP);
			state = 0;
			break;
		case 0:
			/* Disable write access, set bit GPIO_SINT2. */
			out_be32((void *)GPIO0_OR, in_be32((void *)GPIO0_OR) | CONFIG_SYS_EEPROM_WP);
			state = 0;
			break;
		default:
			/* Read current status back. */
			state = (0 == (in_be32((void *)GPIO0_OR) & CONFIG_SYS_EEPROM_WP));
			break;
		}
	}
	return state;
}

int do_eep_wren (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int query = argc == 1;
	int state = 0;

	if (query) {
		/* Query write access state. */
		state = eeprom_write_enable (CONFIG_SYS_I2C_EEPROM_ADDR, -1);
		if (state < 0) {
			puts ("Query of write access state failed.\n");
		} else {
			printf ("Write access for device 0x%0x is %sabled.\n",
				CONFIG_SYS_I2C_EEPROM_ADDR, state ? "en" : "dis");
			state = 0;
		}
	} else {
		if ('0' == argv[1][0]) {
			/* Disable write access. */
			state = eeprom_write_enable (CONFIG_SYS_I2C_EEPROM_ADDR, 0);
		} else {
			/* Enable write access. */
			state = eeprom_write_enable (CONFIG_SYS_I2C_EEPROM_ADDR, 1);
		}
		if (state < 0) {
			puts ("Setup of write access state failed.\n");
		}
	}

	return state;
}

U_BOOT_CMD(eepwren,	2,	0,	do_eep_wren,
	"Enable / disable / query EEPROM write access",
	""
);
#endif /* #if defined(CONFIG_SYS_EEPROM_WREN) */

/* ------------------------------------------------------------------------- */

void reset_phy(void)
{
#ifdef CONFIG_LXT971_NO_SLEEP

	/*
	 * Disable sleep mode in LXT971
	 */
	lxt971_no_sleep();
#endif
}
