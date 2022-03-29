/*
 * (C) Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * Copyright (C) 2004-2006 Freescale Semiconductor, Inc.
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
 *
 * Based on the MPC83xx code.
 */

#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/processor.h>

DECLARE_GLOBAL_DATA_PTR;

static int spmf_mult[] = {
	68, 1, 12, 16,
	20, 24, 28, 32,
	36, 40, 44, 48,
	52, 56, 60, 64
};

static int cpmf_mult[][2] = {
	{0, 1}, {0, 1}, /* 0 and 1 are not valid */
	{1, 1}, {3, 2},
	{2, 1}, {5, 2},
	{3, 1}, {7, 2},
	{0, 1}, {0, 1}, /* and all above 7 are not valid too */
	{0, 1}, {0, 1},
	{0, 1}, {0, 1},
	{0, 1}, {0, 1}
};

static int sys_dividors[][2] = {
	{2, 1}, {5, 2}, {3, 1}, {7, 2}, {4, 1},
	{9, 2}, {5, 1}, {7, 1}, {6, 1}, {8, 1},
	{9, 1}, {11, 1}, {10, 1}, {12, 1}, {13, 1},
	{15, 1}, {14, 1}, {16, 1}, {17, 1}, {19, 1},
	{18, 1}, {20, 1}, {21, 1}, {23, 1}, {22, 1},
	{24, 1}, {25, 1}, {27, 1}, {26, 1}, {28, 1},
	{29, 1}, {31, 1}, {30, 1}, {32, 1}, {33, 1}
};

int get_clocks (void)
{
	volatile immap_t *im = (immap_t *) CONFIG_SYS_IMMR;
	u8 spmf;
	u8 cpmf;
	u8 sys_div;
	u8 ips_div;
	u8 pci_div;
	u32 ref_clk = CONFIG_SYS_MPC512X_CLKIN;
	u32 spll;
	u32 sys_clk;
	u32 core_clk;
	u32 csb_clk;
	u32 ips_clk;
	u32 pci_clk;
	u32 reg;

	reg = in_be32(&im->sysconf.immrbar);
	if ((reg & IMMRBAR_BASE_ADDR) != (u32) im)
		return -1;

	reg = in_be32(&im->clk.spmr);
	spmf = (reg & SPMR_SPMF) >> SPMR_SPMF_SHIFT;
	spll = ref_clk * spmf_mult[spmf];

	reg = in_be32(&im->clk.scfr[1]);
	sys_div = (reg & SCFR2_SYS_DIV) >> SCFR2_SYS_DIV_SHIFT;
	sys_clk = (spll * sys_dividors[sys_div][1]) / sys_dividors[sys_div][0];

	csb_clk = sys_clk / 2;

	reg = in_be32(&im->clk.spmr);
	cpmf = (reg & SPMR_CPMF) >> SPMR_CPMF_SHIFT;
	core_clk = (csb_clk * cpmf_mult[cpmf][0]) / cpmf_mult[cpmf][1];

	reg = in_be32(&im->clk.scfr[0]);
	ips_div = (reg & SCFR1_IPS_DIV_MASK) >> SCFR1_IPS_DIV_SHIFT;
	if (ips_div != 0) {
		ips_clk = csb_clk / ips_div;
	} else {
		/* in case we cannot get a sane IPS divisor, fail gracefully */
		ips_clk = 0;
	}

	reg = in_be32(&im->clk.scfr[0]);
	pci_div = (reg & SCFR1_PCI_DIV_MASK) >> SCFR1_PCI_DIV_SHIFT;
	if (pci_div != 0) {
		pci_clk = csb_clk / pci_div;
	} else {
		/* in case we cannot get a sane IPS divisor, fail gracefully */
		pci_clk = 333333;
	}

	gd->ips_clk = ips_clk;
	gd->pci_clk = pci_clk;
	gd->csb_clk = csb_clk;
	gd->cpu_clk = core_clk;
	gd->bus_clk = csb_clk;
	return 0;

}

/********************************************
 * get_bus_freq
 * return system bus freq in Hz
 *********************************************/
ulong get_bus_freq (ulong dummy)
{
	return gd->csb_clk;
}

int do_clocks (cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	char buf[32];

	printf("Clock configuration:\n");
	printf("  CPU:                 %-4s MHz\n", strmhz(buf, gd->cpu_clk));
	printf("  Coherent System Bus: %-4s MHz\n", strmhz(buf, gd->csb_clk));
	printf("  IPS Bus:             %-4s MHz\n", strmhz(buf, gd->ips_clk));
	printf("  PCI:                 %-4s MHz\n", strmhz(buf, gd->pci_clk));
	printf("  DDR:                 %-4s MHz\n", strmhz(buf, 2*gd->csb_clk));
	return 0;
}

U_BOOT_CMD(clocks, 1, 0, do_clocks,
	"print clock configuration",
	"    clocks"
);

int prt_mpc512x_clks (void)
{
	do_clocks (NULL, 0, 0, NULL);
	return (0);
}
