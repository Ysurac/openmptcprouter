/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2004
 * Mark Jonas, Freescale Semiconductor, mark.jonas@motorola.com.
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
#include <pci.h>
#include <asm/processor.h>
#include <libfdt.h>
#include <netdev.h>

#if defined(CONFIG_LITE5200B)
#include "mt46v32m16.h"
#else
# if defined(CONFIG_MPC5200_DDR)
#  include "mt46v16m16-75.h"
# else
#include "mt48lc16m16a2-75.h"
# endif
#endif

#ifdef CONFIG_LITE5200B_PM
/* u-boot part of low-power mode implementation */
#define SAVED_ADDR (*(void **)0x00000000)
#define PSC2_4 0x02

void lite5200b_wakeup(void)
{
	unsigned char wakeup_pin;
	void (*linux_wakeup)(void);

	/* check PSC2_4, if it's down "QT" is signaling we have a wakeup
	 * from low power mode */
	*(vu_char *)MPC5XXX_WU_GPIO_ENABLE = PSC2_4;
	__asm__ volatile ("sync");

	wakeup_pin = *(vu_char *)MPC5XXX_WU_GPIO_DATA_I;
	if (wakeup_pin & PSC2_4)
		return;

	/* acknowledge to "QT"
	 * by holding pin at 1 for 10 uS */
	*(vu_char *)MPC5XXX_WU_GPIO_DIR = PSC2_4;
	__asm__ volatile ("sync");
	*(vu_char *)MPC5XXX_WU_GPIO_DATA_O = PSC2_4;
	__asm__ volatile ("sync");
	udelay(10);

	/* put ram out of self-refresh */
	*(vu_long *)MPC5XXX_SDRAM_CTRL |= 0x80000000;	/* mode_en */
	__asm__ volatile ("sync");
	*(vu_long *)MPC5XXX_SDRAM_CTRL |= 0x50000000;	/* cke ref_en */
	__asm__ volatile ("sync");
	*(vu_long *)MPC5XXX_SDRAM_CTRL &= ~0x80000000;	/* !mode_en */
	__asm__ volatile ("sync");
	udelay(10); /* wait a bit */

	/* jump back to linux kernel code */
	linux_wakeup = SAVED_ADDR;
	printf("\n\nLooks like we just woke, transferring control to 0x%08lx\n",
			(unsigned long)linux_wakeup);
	linux_wakeup();
}
#else
#define lite5200b_wakeup()
#endif

#ifndef CONFIG_SYS_RAMBOOT
static void sdram_start (int hi_addr)
{
	long hi_addr_bit = hi_addr ? 0x01000000 : 0;

	/* unlock mode register */
	*(vu_long *)MPC5XXX_SDRAM_CTRL = SDRAM_CONTROL | 0x80000000 | hi_addr_bit;
	__asm__ volatile ("sync");

	/* precharge all banks */
	*(vu_long *)MPC5XXX_SDRAM_CTRL = SDRAM_CONTROL | 0x80000002 | hi_addr_bit;
	__asm__ volatile ("sync");

#if SDRAM_DDR
	/* set mode register: extended mode */
	*(vu_long *)MPC5XXX_SDRAM_MODE = SDRAM_EMODE;
	__asm__ volatile ("sync");

	/* set mode register: reset DLL */
	*(vu_long *)MPC5XXX_SDRAM_MODE = SDRAM_MODE | 0x04000000;
	__asm__ volatile ("sync");
#endif

	/* precharge all banks */
	*(vu_long *)MPC5XXX_SDRAM_CTRL = SDRAM_CONTROL | 0x80000002 | hi_addr_bit;
	__asm__ volatile ("sync");

	/* auto refresh */
	*(vu_long *)MPC5XXX_SDRAM_CTRL = SDRAM_CONTROL | 0x80000004 | hi_addr_bit;
	__asm__ volatile ("sync");

	/* set mode register */
	*(vu_long *)MPC5XXX_SDRAM_MODE = SDRAM_MODE;
	__asm__ volatile ("sync");

	/* normal operation */
	*(vu_long *)MPC5XXX_SDRAM_CTRL = SDRAM_CONTROL | hi_addr_bit;
	__asm__ volatile ("sync");
}
#endif

/*
 * ATTENTION: Although partially referenced initdram does NOT make real use
 *            use of CONFIG_SYS_SDRAM_BASE. The code does not work if CONFIG_SYS_SDRAM_BASE
 *            is something else than 0x00000000.
 */

phys_size_t initdram (int board_type)
{
	ulong dramsize = 0;
	ulong dramsize2 = 0;
	uint svr, pvr;

#ifndef CONFIG_SYS_RAMBOOT
	ulong test1, test2;

	/* setup SDRAM chip selects */
	*(vu_long *)MPC5XXX_SDRAM_CS0CFG = 0x0000001e;/* 2G at 0x0 */
	*(vu_long *)MPC5XXX_SDRAM_CS1CFG = 0x80000000;/* disabled */
	__asm__ volatile ("sync");

	/* setup config registers */
	*(vu_long *)MPC5XXX_SDRAM_CONFIG1 = SDRAM_CONFIG1;
	*(vu_long *)MPC5XXX_SDRAM_CONFIG2 = SDRAM_CONFIG2;
	__asm__ volatile ("sync");

#if SDRAM_DDR
	/* set tap delay */
	*(vu_long *)MPC5XXX_CDM_PORCFG = SDRAM_TAPDELAY;
	__asm__ volatile ("sync");
#endif

	/* find RAM size using SDRAM CS0 only */
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
	if (dramsize < (1 << 20)) {
		dramsize = 0;
	}

	/* set SDRAM CS0 size according to the amount of RAM found */
	if (dramsize > 0) {
		*(vu_long *)MPC5XXX_SDRAM_CS0CFG = 0x13 + __builtin_ffs(dramsize >> 20) - 1;
	} else {
		*(vu_long *)MPC5XXX_SDRAM_CS0CFG = 0; /* disabled */
	}

	/* let SDRAM CS1 start right after CS0 */
	*(vu_long *)MPC5XXX_SDRAM_CS1CFG = dramsize + 0x0000001e;/* 2G */

	/* find RAM size using SDRAM CS1 only */
	if (!dramsize)
		sdram_start(0);
	test2 = test1 = get_ram_size((long *)(CONFIG_SYS_SDRAM_BASE + dramsize), 0x80000000);
	if (!dramsize) {
		sdram_start(1);
		test2 = get_ram_size((long *)(CONFIG_SYS_SDRAM_BASE + dramsize), 0x80000000);
	}
	if (test1 > test2) {
		sdram_start(0);
		dramsize2 = test1;
	} else {
		dramsize2 = test2;
	}

	/* memory smaller than 1MB is impossible */
	if (dramsize2 < (1 << 20)) {
		dramsize2 = 0;
	}

	/* set SDRAM CS1 size according to the amount of RAM found */
	if (dramsize2 > 0) {
		*(vu_long *)MPC5XXX_SDRAM_CS1CFG = dramsize
			| (0x13 + __builtin_ffs(dramsize2 >> 20) - 1);
	} else {
		*(vu_long *)MPC5XXX_SDRAM_CS1CFG = dramsize; /* disabled */
	}

#else /* CONFIG_SYS_RAMBOOT */

	/* retrieve size of memory connected to SDRAM CS0 */
	dramsize = *(vu_long *)MPC5XXX_SDRAM_CS0CFG & 0xFF;
	if (dramsize >= 0x13) {
		dramsize = (1 << (dramsize - 0x13)) << 20;
	} else {
		dramsize = 0;
	}

	/* retrieve size of memory connected to SDRAM CS1 */
	dramsize2 = *(vu_long *)MPC5XXX_SDRAM_CS1CFG & 0xFF;
	if (dramsize2 >= 0x13) {
		dramsize2 = (1 << (dramsize2 - 0x13)) << 20;
	} else {
		dramsize2 = 0;
	}

#endif /* CONFIG_SYS_RAMBOOT */

	/*
	 * On MPC5200B we need to set the special configuration delay in the
	 * DDR controller. Please refer to Freescale's AN3221 "MPC5200B SDRAM
	 * Initialization and Configuration", 3.3.1 SDelay--MBAR + 0x0190:
	 *
	 * "The SDelay should be written to a value of 0x00000004. It is
	 * required to account for changes caused by normal wafer processing
	 * parameters."
	 */
	svr = get_svr();
	pvr = get_pvr();
	if ((SVR_MJREV(svr) >= 2) &&
	    (PVR_MAJ(pvr) == 1) && (PVR_MIN(pvr) == 4)) {

		*(vu_long *)MPC5XXX_SDRAM_SDELAY = 0x04;
		__asm__ volatile ("sync");
	}

	lite5200b_wakeup();

	return dramsize + dramsize2;
}

int checkboard (void)
{
#if defined (CONFIG_LITE5200B)
	puts ("Board: Freescale Lite5200B\n");
#else
	puts ("Board: Motorola MPC5200 (IceCube)\n");
#endif
	return 0;
}

void flash_preinit(void)
{
	/*
	 * Now, when we are in RAM, enable flash write
	 * access for detection process.
	 * Note that CS_BOOT cannot be cleared when
	 * executing in flash.
	 */
	*(vu_long *)MPC5XXX_BOOTCS_CFG &= ~0x1; /* clear RO */
}

void flash_afterinit(ulong size)
{
	if (size == 0x800000) { /* adjust mapping */
		*(vu_long *)MPC5XXX_BOOTCS_START = *(vu_long *)MPC5XXX_CS0_START =
			START_REG(CONFIG_SYS_BOOTCS_START | size);
		*(vu_long *)MPC5XXX_BOOTCS_STOP = *(vu_long *)MPC5XXX_CS0_STOP =
			STOP_REG(CONFIG_SYS_BOOTCS_START | size, size);
	}
}

#ifdef	CONFIG_PCI
static struct pci_controller hose;

extern void pci_mpc5xxx_init(struct pci_controller *);

void pci_init_board(void)
{
	pci_mpc5xxx_init(&hose);
}
#endif

#if defined(CONFIG_CMD_IDE) && defined(CONFIG_IDE_RESET)

void init_ide_reset (void)
{
	debug ("init_ide_reset\n");

	/* Configure PSC1_4 as GPIO output for ATA reset */
	*(vu_long *) MPC5XXX_WU_GPIO_ENABLE |= GPIO_PSC1_4;
	*(vu_long *) MPC5XXX_WU_GPIO_DIR    |= GPIO_PSC1_4;
	/* Deassert reset */
	*(vu_long *) MPC5XXX_WU_GPIO_DATA_O   |= GPIO_PSC1_4;
}

void ide_set_reset (int idereset)
{
	debug ("ide_reset(%d)\n", idereset);

	if (idereset) {
		*(vu_long *) MPC5XXX_WU_GPIO_DATA_O &= ~GPIO_PSC1_4;
		/* Make a delay. MPC5200 spec says 25 usec min */
		udelay(500000);
	} else {
		*(vu_long *) MPC5XXX_WU_GPIO_DATA_O |=  GPIO_PSC1_4;
	}
}
#endif

#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP)
void
ft_board_setup(void *blob, bd_t *bd)
{
	ft_cpu_setup(blob, bd);
}
#endif

int board_eth_init(bd_t *bis)
{
	cpu_eth_init(bis); /* Built in FEC comes first */
	return pci_eth_init(bis);
}
