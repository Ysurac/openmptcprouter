/*
 * (C) Copyright 2008
 * Texas Instruments, <www.ti.com>
 *
 * Author :
 *      Manikandan Pillai <mani.pillai@ti.com>
 *
 * Derived from Beagle Board and 3430 SDP code by
 *      Richard Woodruff <r-woodruff2@ti.com>
 *      Syed Mohammed Khasim <khasim@ti.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR /PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/mem.h>	/* get mem tables */
#include <asm/arch/sys_proto.h>
#include <i2c.h>
#include <linux/compiler.h>

extern omap3_sysinfo sysinfo;
static struct ctrl *ctrl_base = (struct ctrl *)OMAP34XX_CTRL_BASE;

#ifdef CONFIG_DISPLAY_CPUINFO
static char *rev_s[CPU_3XX_MAX_REV] = {
				"1.0",
				"2.0",
				"2.1",
				"3.0",
				"3.1",
				"UNKNOWN",
				"UNKNOWN",
				"3.1.2"};

/* this is the revision table for 37xx CPUs */
static char *rev_s_37xx[CPU_37XX_MAX_REV] = {
				"1.0",
				"1.1",
				"1.2"};
#endif /* CONFIG_DISPLAY_CPUINFO */

/*****************************************************************
 * dieid_num_r(void) - read and set die ID
 *****************************************************************/
void dieid_num_r(void)
{
	struct ctrl_id *id_base = (struct ctrl_id *)OMAP34XX_ID_L4_IO_BASE;
	char *uid_s, die_id[34];
	u32 id[4];

	memset(die_id, 0, sizeof(die_id));

	uid_s = getenv("dieid#");

	if (uid_s == NULL) {
		id[3] = readl(&id_base->die_id_0);
		id[2] = readl(&id_base->die_id_1);
		id[1] = readl(&id_base->die_id_2);
		id[0] = readl(&id_base->die_id_3);
		sprintf(die_id, "%08x%08x%08x%08x", id[0], id[1], id[2], id[3]);
		setenv("dieid#", die_id);
		uid_s = die_id;
	}

	printf("Die ID #%s\n", uid_s);
}

/******************************************
 * get_cpu_type(void) - extract cpu info
 ******************************************/
u32 get_cpu_type(void)
{
	return readl(&ctrl_base->ctrl_omap_stat);
}

/******************************************
 * get_cpu_id(void) - extract cpu id
 * returns 0 for ES1.0, cpuid otherwise
 ******************************************/
u32 get_cpu_id(void)
{
	struct ctrl_id *id_base;
	u32 cpuid = 0;

	/*
	 * On ES1.0 the IDCODE register is not exposed on L4
	 * so using CPU ID to differentiate between ES1.0 and > ES1.0.
	 */
	__asm__ __volatile__("mrc p15, 0, %0, c0, c0, 0":"=r"(cpuid));
	if ((cpuid & 0xf) == 0x0) {
		return 0;
	} else {
		/* Decode the IDs on > ES1.0 */
		id_base = (struct ctrl_id *) OMAP34XX_ID_L4_IO_BASE;

		cpuid = readl(&id_base->idcode);
	}

	return cpuid;
}

/******************************************
 * get_cpu_family(void) - extract cpu info
 ******************************************/
u32 get_cpu_family(void)
{
	u16 hawkeye;
	u32 cpu_family;
	u32 cpuid = get_cpu_id();

	if (cpuid == 0)
		return CPU_OMAP34XX;

	hawkeye = (cpuid >> HAWKEYE_SHIFT) & 0xffff;
	switch (hawkeye) {
	case HAWKEYE_OMAP34XX:
		cpu_family = CPU_OMAP34XX;
		break;
	case HAWKEYE_AM35XX:
		cpu_family = CPU_AM35XX;
		break;
	case HAWKEYE_OMAP36XX:
		cpu_family = CPU_OMAP36XX;
		break;
	default:
		cpu_family = CPU_OMAP34XX;
	}

	return cpu_family;
}

/******************************************
 * get_cpu_rev(void) - extract version info
 ******************************************/
u32 get_cpu_rev(void)
{
	u32 cpuid = get_cpu_id();

	if (cpuid == 0)
		return CPU_3XX_ES10;
	else
		return (cpuid >> CPU_3XX_ID_SHIFT) & 0xf;
}

/*****************************************************************
 * get_sku_id(void) - read sku_id to get info on max clock rate
 *****************************************************************/
u32 get_sku_id(void)
{
	struct ctrl_id *id_base = (struct ctrl_id *)OMAP34XX_ID_L4_IO_BASE;
	return readl(&id_base->sku_id) & SKUID_CLK_MASK;
}

/***************************************************************************
 *  get_gpmc0_base() - Return current address hardware will be
 *     fetching from. The below effectively gives what is correct, its a bit
 *   mis-leading compared to the TRM.  For the most general case the mask
 *   needs to be also taken into account this does work in practice.
 *   - for u-boot we currently map:
 *       -- 0 to nothing,
 *       -- 4 to flash
 *       -- 8 to enent
 *       -- c to wifi
 ****************************************************************************/
u32 get_gpmc0_base(void)
{
	u32 b;

	b = readl(&gpmc_cfg->cs[0].config7);
	b &= 0x1F;		/* keep base [5:0] */
	b = b << 24;		/* ret 0x0b000000 */
	return b;
}

/*******************************************************************
 * get_gpmc0_width() - See if bus is in x8 or x16 (mainly for nand)
 *******************************************************************/
u32 get_gpmc0_width(void)
{
	return WIDTH_16BIT;
}

/*************************************************************************
 * get_board_rev() - setup to pass kernel board revision information
 * returns:(bit[0-3] sub version, higher bit[7-4] is higher version)
 *************************************************************************/
u32 __weak get_board_rev(void)
{
	return 0x20;
}

/********************************************************
 *  get_base(); get upper addr of current execution
 *******************************************************/
u32 get_base(void)
{
	u32 val;

	__asm__ __volatile__("mov %0, pc \n":"=r"(val)::"memory");
	val &= 0xF0000000;
	val >>= 28;
	return val;
}

/********************************************************
 *  is_running_in_flash() - tell if currently running in
 *  FLASH.
 *******************************************************/
u32 is_running_in_flash(void)
{
	if (get_base() < 4)
		return 1;	/* in FLASH */

	return 0;		/* running in SRAM or SDRAM */
}

/********************************************************
 *  is_running_in_sram() - tell if currently running in
 *  SRAM.
 *******************************************************/
u32 is_running_in_sram(void)
{
	if (get_base() == 4)
		return 1;	/* in SRAM */

	return 0;		/* running in FLASH or SDRAM */
}

/********************************************************
 *  is_running_in_sdram() - tell if currently running in
 *  SDRAM.
 *******************************************************/
u32 is_running_in_sdram(void)
{
	if (get_base() > 4)
		return 1;	/* in SDRAM */

	return 0;		/* running in SRAM or FLASH */
}

/***************************************************************
 *  get_boot_type() - Is this an XIP type device or a stream one
 *  bits 4-0 specify type. Bit 5 says mem/perif
 ***************************************************************/
u32 get_boot_type(void)
{
	return (readl(&ctrl_base->status) & SYSBOOT_MASK);
}

/*************************************************************
 *  get_device_type(): tell if GP/HS/EMU/TST
 *************************************************************/
u32 get_device_type(void)
{
	return ((readl(&ctrl_base->status) & (DEVICE_MASK)) >> 8);
}

#ifdef CONFIG_DISPLAY_CPUINFO
/**
 * Print CPU information
 */
int print_cpuinfo (void)
{
	char *cpu_family_s, *cpu_s, *sec_s, *max_clk;

	switch (get_cpu_family()) {
	case CPU_OMAP34XX:
		cpu_family_s = "OMAP";
		switch (get_cpu_type()) {
		case OMAP3503:
			cpu_s = "3503";
			break;
		case OMAP3515:
			cpu_s = "3515";
			break;
		case OMAP3525:
			cpu_s = "3525";
			break;
		case OMAP3530:
			cpu_s = "3530";
			break;
		default:
			cpu_s = "35XX";
			break;
		}
		if ((get_cpu_rev() >= CPU_3XX_ES31) &&
		    (get_sku_id() == SKUID_CLK_720MHZ))
			max_clk = "720 mHz";
		else
			max_clk = "600 mHz";

		break;
	case CPU_AM35XX:
		cpu_family_s = "AM";
		switch (get_cpu_type()) {
		case AM3505:
			cpu_s = "3505";
			break;
		case AM3517:
			cpu_s = "3517";
			break;
		default:
			cpu_s = "35XX";
			break;
		}
		max_clk = "600 Mhz";
		break;
	case CPU_OMAP36XX:
		cpu_family_s = "OMAP";
		switch (get_cpu_type()) {
		case OMAP3730:
			cpu_s = "3630/3730";
			break;
		default:
			cpu_s = "36XX/37XX";
			break;
		}
		max_clk = "1 Ghz";
		break;
	default:
		cpu_family_s = "OMAP";
		cpu_s = "35XX";
		max_clk = "600 Mhz";
	}

	switch (get_device_type()) {
	case TST_DEVICE:
		sec_s = "TST";
		break;
	case EMU_DEVICE:
		sec_s = "EMU";
		break;
	case HS_DEVICE:
		sec_s = "HS";
		break;
	case GP_DEVICE:
		sec_s = "GP";
		break;
	default:
		sec_s = "?";
	}

	if (CPU_OMAP36XX == get_cpu_family())
		printf("%s%s-%s ES%s, CPU-OPP2, L3-165MHz, Max CPU Clock %s\n",
			cpu_family_s, cpu_s, sec_s,
			rev_s_37xx[get_cpu_rev()], max_clk);
	else
		printf("%s%s-%s ES%s, CPU-OPP2, L3-165MHz, Max CPU Clock %s\n",
			cpu_family_s, cpu_s, sec_s,
			rev_s[get_cpu_rev()], max_clk);

	return 0;
}
#endif	/* CONFIG_DISPLAY_CPUINFO */
