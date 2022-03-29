/*
 * (C) Copyright 2003-2007
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * modified for Promess PRO - by Andy Joseph, andy@promessdev.com
 * modified for Promess PRO-Motion - by Robert McCullough, rob@promessdev.com
 * modified by Chris M. Tumas 6/20/06 Change CAS latency to 2 from 3
 * Also changed the refresh for 100MHz operation
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
#include <mpc5xxx.h>
#include <miiphy.h>
#include <libfdt.h>

#if defined(CONFIG_STATUS_LED)
#include <status_led.h>
#endif /* CONFIG_STATUS_LED */

/* Kollmorgen DPR initialization data */
struct init_elem {
	unsigned long addr;
	unsigned len;
	char *data;
	} init_seq[] = {
		{0x500003F2, 2, "\x86\x00"},		/* HW parameter */
		{0x500003F0, 2, "\x00\x00"},
		{0x500003EC, 4, "\x00\x80\xc1\x52"},	/* Magic word */
	};

/*
 * Initialize Kollmorgen DPR
 */
static void kollmorgen_init(void)
{
	unsigned i, j;
	vu_char *p;

	for (i = 0; i < sizeof(init_seq) / sizeof(struct init_elem); ++i) {
		p = (vu_char *)init_seq[i].addr;
		for (j = 0; j < init_seq[i].len; ++j)
			*(p + j) = *(init_seq[i].data + j);
	}

	printf("DPR:   Kollmorgen DPR initialized\n");
}


/*
 * Early board initalization.
 */
int board_early_init_r(void)
{
	/* Now, when we are in RAM, disable Boot Chipselect and enable CS0 */
	*(vu_long *)MPC5XXX_ADDECR &= ~(1 << 25);
	*(vu_long *)MPC5XXX_ADDECR |= (1 << 16);

	/* Initialize Kollmorgen DPR */
	kollmorgen_init();

	return 0;
}


/*
 * Additional PHY intialization. After being reset in mpc5xxx_fec_init_phy(),
 * PHY goes into FX mode.  To take it out of the FX mode and switch into
 * desired TX operation, one needs to clear the FX_SEL bit of Mode Control
 * Register.
 */
void reset_phy(void)
{
	unsigned short mode_control;

	miiphy_read("FEC", CONFIG_PHY_ADDR, 0x15, &mode_control);
	miiphy_write("FEC", CONFIG_PHY_ADDR, 0x15,
			mode_control & 0xfffe);
	return;
}

#ifndef CONFIG_SYS_RAMBOOT
/*
 * Helper function to initialize SDRAM controller.
 */
static void sdram_start(int hi_addr)
{
	long hi_addr_bit = hi_addr ? 0x01000000 : 0;

	/* unlock mode register */
	*(vu_long *)MPC5XXX_SDRAM_CTRL = SDRAM_CONTROL | 0x80000000 |
						hi_addr_bit;

	/* precharge all banks */
	*(vu_long *)MPC5XXX_SDRAM_CTRL = SDRAM_CONTROL | 0x80000002 |
						hi_addr_bit;

	/* auto refresh */
	*(vu_long *)MPC5XXX_SDRAM_CTRL = SDRAM_CONTROL | 0x80000004 |
						hi_addr_bit;

	/* auto refresh, second time */
	*(vu_long *)MPC5XXX_SDRAM_CTRL = SDRAM_CONTROL | 0x80000004 |
						hi_addr_bit;

	/* set mode register */
	*(vu_long *)MPC5XXX_SDRAM_MODE = SDRAM_MODE;

	/* normal operation */
	*(vu_long *)MPC5XXX_SDRAM_CTRL = SDRAM_CONTROL | hi_addr_bit;
}
#endif /* !CONFIG_SYS_RAMBOOT */


/*
 * Initalize SDRAM - configure SDRAM controller, detect memory size.
 */
phys_size_t initdram(int board_type)
{
	ulong dramsize = 0;
#ifndef CONFIG_SYS_RAMBOOT
	ulong test1, test2;

	/* According to AN3221 (MPC5200B SDRAM Initialization and
	 * Configuration), the SDelay register must be written a value of
	 * 0x00000004 as the first step of the SDRAM contorller configuration.
	 */
	*(vu_long *)MPC5XXX_SDRAM_SDELAY = 0x04;

	/* configure SDRAM start/end for detection */
	*(vu_long *)MPC5XXX_SDRAM_CS0CFG = 0x0000001e; /* 2G at 0x0 */
	*(vu_long *)MPC5XXX_SDRAM_CS1CFG = 0x80000000; /* disabled */

	/* setup config registers */
	*(vu_long *)MPC5XXX_SDRAM_CONFIG1 = SDRAM_CONFIG1;
	*(vu_long *)MPC5XXX_SDRAM_CONFIG2 = SDRAM_CONFIG2;

	sdram_start(0);
	test1 = get_ram_size((long *)CONFIG_SYS_SDRAM_BASE, 0x80000000);
	sdram_start(1);
	test2 = get_ram_size((long *)CONFIG_SYS_SDRAM_BASE, 0x80000000);
	if (test1 > test2) {
		sdram_start(0);
		dramsize = test1;
	} else {
		dramsize = test2;
	}

	/* memory smaller than 1MB is impossible */
	if (dramsize < (1 << 20))
		dramsize = 0;

	/* set SDRAM CS0 size according to the amount of RAM found */
	if (dramsize > 0) {
		*(vu_long *)MPC5XXX_SDRAM_CS0CFG = 0x13 +
			__builtin_ffs(dramsize >> 20) - 1;
	} else {
		*(vu_long *)MPC5XXX_SDRAM_CS0CFG = 0; /* disabled */
	}

	/* let SDRAM CS1 start right after CS0 and disable it */
	*(vu_long *) MPC5XXX_SDRAM_CS1CFG = dramsize;

#else /* !CONFIG_SYS_RAMBOOT */
	/* retrieve size of memory connected to SDRAM CS0 */
	dramsize = *(vu_long *)MPC5XXX_SDRAM_CS0CFG & 0xFF;
	if (dramsize >= 0x13)
		dramsize = (1 << (dramsize - 0x13)) << 20;
	else
		dramsize = 0;
#endif /* CONFIG_SYS_RAMBOOT */

	/* return total ram size */
	return dramsize;
}


int checkboard(void)
{
	uchar rev = *(vu_char *)CPLD_REV_REGISTER;
	printf("Board: Promess Motion-PRO board (CPLD rev. 0x%02x)\n", rev);
	return 0;
}


#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP)
void ft_board_setup(void *blob, bd_t *bd)
{
	ft_cpu_setup(blob, bd);
}
#endif /* defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP) */


#if defined(CONFIG_STATUS_LED)
void __led_init(led_id_t regaddr, int state)
{
	*((vu_long *) regaddr) |= ENABLE_GPIO_OUT;

	if (state == STATUS_LED_ON)
		*((vu_long *) regaddr) |= LED_ON;
	else
		*((vu_long *) regaddr) &= ~LED_ON;
}

void __led_set(led_id_t regaddr, int state)
{
	if (state == STATUS_LED_ON)
		*((vu_long *) regaddr) |= LED_ON;
	else
		*((vu_long *) regaddr) &= ~LED_ON;
}

void __led_toggle(led_id_t regaddr)
{
	*((vu_long *) regaddr) ^= LED_ON;
}
#endif /* CONFIG_STATUS_LED */
