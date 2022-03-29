/*
 * Copyright 2011,2012 Freescale Semiconductor, Inc.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <netdev.h>
#include <linux/compiler.h>
#include <asm/mmu.h>
#include <asm/processor.h>
#include <asm/cache.h>
#include <asm/immap_85xx.h>
#include <asm/fsl_law.h>
#include <asm/fsl_serdes.h>
#include <asm/fsl_portals.h>
#include <asm/fsl_liodn.h>
#include <fm_eth.h>

extern void pci_of_setup(void *blob, bd_t *bd);

#include "cpld.h"

DECLARE_GLOBAL_DATA_PTR;

int checkboard(void)
{
	u8 sw;
	struct cpu_type *cpu = gd->cpu;
	ccsr_gur_t *gur = (void *)CONFIG_SYS_MPC85xx_GUTS_ADDR;
	unsigned int i;

	printf("Board: %sRDB, ", cpu->name);
	printf("CPLD version: %d.%d ", CPLD_READ(cpld_ver),
			CPLD_READ(cpld_ver_sub));

	sw = CPLD_READ(fbank_sel);
	printf("vBank: %d\n", sw & 0x1);

	/*
	 * Display the RCW, so that no one gets confused as to what RCW
	 * we're actually using for this boot.
	 */
	puts("Reset Configuration Word (RCW):");
	for (i = 0; i < ARRAY_SIZE(gur->rcwsr); i++) {
		u32 rcw = in_be32(&gur->rcwsr[i]);

		if ((i % 4) == 0)
			printf("\n       %08x:", i * 4);
		printf(" %08x", rcw);
	}
	puts("\n");

	/*
	 * Display the actual SERDES reference clocks as configured by the
	 * dip switches on the board.  Note that the SWx registers could
	 * technically be set to force the reference clocks to match the
	 * values that the SERDES expects (or vice versa).  For now, however,
	 * we just display both values and hope the user notices when they
	 * don't match.
	 */
	puts("SERDES Reference Clocks: ");
	sw = in_8(&CPLD_SW(2)) >> 2;
	for (i = 0; i < 2; i++) {
		static const char * const freq[][3] = {{"0", "100", "125"},
						{"100", "156.25", "125"}
		};
		unsigned int clock = (sw >> (2 * i)) & 3;

		printf("Bank%u=%sMhz ", i+1, freq[i][clock]);
	}
	puts("\n");

	return 0;
}

int board_early_init_f(void)
{
	ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);

	/* board only uses the DDR_MCK0/1, so disable the DDR_MCK2/3 */
	setbits_be32(&gur->ddrclkdr, 0x000f000f);

	return 0;
}

int board_early_init_r(void)
{
	const unsigned int flashbase = CONFIG_SYS_FLASH_BASE;
	const u8 flash_esel = find_tlb_idx((void *)flashbase, 1);

	/*
	 * Remap Boot flash + PROMJET region to caching-inhibited
	 * so that flash can be erased properly.
	 */

	/* Flush d-cache and invalidate i-cache of any FLASH data */
	flush_dcache();
	invalidate_icache();

	/* invalidate existing TLB entry for flash + promjet */
	disable_tlb(flash_esel);

	set_tlb(1, flashbase, CONFIG_SYS_FLASH_BASE_PHYS,
			MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
			0, flash_esel, BOOKE_PAGESZ_256M, 1);

	set_liodns();
	setup_portals();

	return 0;
}

unsigned long get_board_sys_clk(unsigned long dummy)
{
	u8 sysclk_conf = CPLD_READ(sysclk_sw1);

	switch (sysclk_conf & 0x7) {
	case CPLD_SYSCLK_83:
		return 83333333;
	case CPLD_SYSCLK_100:
		return 100000000;
	default:
		return 66666666;
	}
}

static const char *serdes_clock_to_string(u32 clock)
{
	switch (clock) {
	case SRDS_PLLCR0_RFCK_SEL_100:
		return "100";
	case SRDS_PLLCR0_RFCK_SEL_125:
		return "125";
	case SRDS_PLLCR0_RFCK_SEL_156_25:
		return "156.25";
	default:
		return "150";
	}
}

#define NUM_SRDS_BANKS	2

int misc_init_r(void)
{
	serdes_corenet_t *regs = (void *)CONFIG_SYS_FSL_CORENET_SERDES_ADDR;
	u32 actual[NUM_SRDS_BANKS];
	unsigned int i;
	u8 sw;
	static const int freq[][3] = {
		{0, SRDS_PLLCR0_RFCK_SEL_100, SRDS_PLLCR0_RFCK_SEL_125},
		{SRDS_PLLCR0_RFCK_SEL_100, SRDS_PLLCR0_RFCK_SEL_156_25,
			SRDS_PLLCR0_RFCK_SEL_125}
	};

	sw = in_8(&CPLD_SW(2)) >> 2;
	for (i = 0; i < NUM_SRDS_BANKS; i++) {
		unsigned int clock = (sw >> (2 * i)) & 3;
		if (clock == 0x3) {
			printf("Warning: SDREFCLK%u switch setting of '11' is "
			       "unsupported\n", i + 1);
			break;
		}
		if (i == 0 && clock == 0)
			puts("Warning: SDREFCLK1 switch setting of"
				"'00' is unsupported\n");
		else
			actual[i] = freq[i][clock];
	}

	for (i = 0; i < NUM_SRDS_BANKS; i++) {
		u32 expected = in_be32(&regs->bank[i].pllcr0);
		expected &= SRDS_PLLCR0_RFCK_SEL_MASK;
		if (expected != actual[i]) {
			printf("Warning: SERDES bank %u expects reference clock"
			       " %sMHz, but actual is %sMHz\n", i + 1,
			       serdes_clock_to_string(expected),
			       serdes_clock_to_string(actual[i]));
		}
	}

	return 0;
}

void ft_board_setup(void *blob, bd_t *bd)
{
	phys_addr_t base;
	phys_size_t size;

	ft_cpu_setup(blob, bd);

	base = getenv_bootm_low();
	size = getenv_bootm_size();

	fdt_fixup_memory(blob, (u64)base, (u64)size);

#if defined(CONFIG_HAS_FSL_DR_USB) || defined(CONFIG_HAS_FSL_MPH_USB)
	fdt_fixup_dr_usb(blob, bd);
#endif

#ifdef CONFIG_PCI
	pci_of_setup(blob, bd);
#endif

	fdt_fixup_liodn(blob);
#ifdef CONFIG_SYS_DPAA_FMAN
	fdt_fixup_fman_ethernet(blob);
#endif
}
