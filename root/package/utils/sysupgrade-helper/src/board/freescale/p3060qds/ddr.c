/*
 * Copyright 2009-2011 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
 */

#include <common.h>
#include <i2c.h>
#include <hwconfig.h>
#include <asm/mmu.h>
#include <asm/fsl_ddr_sdram.h>
#include <asm/fsl_ddr_dimm_params.h>
#include <asm/fsl_law.h>

#include "p3060qds.h"

/*
 * Fixed sdram init -- doesn't use serial presence detect.
 */

phys_size_t fixed_sdram(void)
{
	int i;
	char buf[32];
	fsl_ddr_cfg_regs_t ddr_cfg_regs;
	phys_size_t ddr_size;
	unsigned int lawbar1_target_id;
	ulong ddr_freq, ddr_freq_mhz;

	ddr_freq = get_ddr_freq(0);
	ddr_freq_mhz = ddr_freq / 1000000;

	printf("Configuring DDR for %s MT/s data rate\n",
				strmhz(buf, ddr_freq));

	for (i = 0; fixed_ddr_parm_0[i].max_freq > 0; i++) {
		if ((ddr_freq_mhz > fixed_ddr_parm_0[i].min_freq) &&
		   (ddr_freq_mhz <= fixed_ddr_parm_0[i].max_freq)) {
			memcpy(&ddr_cfg_regs,
				fixed_ddr_parm_0[i].ddr_settings,
				sizeof(ddr_cfg_regs));
			break;
		}
	}

	if (fixed_ddr_parm_0[i].max_freq == 0)
		panic("Unsupported DDR data rate %s MT/s data rate\n",
			strmhz(buf, ddr_freq));

	ddr_size = (phys_size_t) CONFIG_SYS_SDRAM_SIZE * 1024 * 1024;
	ddr_cfg_regs.ddr_cdr1 = DDR_CDR1_DHC_EN;
	fsl_ddr_set_memctl_regs(&ddr_cfg_regs, 0);

	/*
	 * setup laws for DDR. If not interleaving, presuming half memory on
	 * DDR1 and the other half on DDR2
	 */
	if (fixed_ddr_parm_0[i].ddr_settings->cs[0].config & 0x20000000) {
		if (set_ddr_laws(CONFIG_SYS_DDR_SDRAM_BASE,
				 ddr_size,
				 LAW_TRGT_IF_DDR_INTRLV) < 0) {
			printf("ERROR setting Local Access Windows for DDR\n");
			return 0;
		}
	} else {
		lawbar1_target_id = LAW_TRGT_IF_DDR_1;
		if (set_ddr_laws(CONFIG_SYS_DDR_SDRAM_BASE,
				 ddr_size,
				 lawbar1_target_id) < 0) {
			printf("ERROR setting Local Access Windows for DDR\n");
			return 0;
		}
	}
	return ddr_size;
}

struct board_specific_params {
	u32 n_ranks;
	u32 datarate_mhz_high;
	u32 clk_adjust;
	u32 wrlvl_start;
	u32 cpo;
	u32 write_data_delay;
	u32 force_2T;
};

/*
 * This table contains all valid speeds we want to override with board
 * specific parameters. datarate_mhz_high values need to be in ascending order
 * for each n_ranks group.
 */
static const struct board_specific_params udimm[] = {
	/*
	 * memory controller 0
	 *   num|  hi|  clk| wrlvl | cpo  |wrdata|2T
	 * ranks| mhz|adjst| start |      |delay |
	 */
	{4,   850,    4,     6,   0xff,    2,  0},
	{4,   950,    5,     7,   0xff,    2,  0},
	{4,  1050,    5,     8,   0xff,    2,  0},
	{4,  1250,    5,    10,   0xff,    2,  0},
	{4,  1350,    5,    11,   0xff,    2,  0},
	{4,  1666,    5,    12,   0xff,    2,  0},
	{2,   850,    5,     6,   0xff,    2,  0},
	{2,   950,    5,     7,   0xff,    2,  0},
	{2,  1250,    4,     6,   0xff,    2,  0},
	{2,  1350,    5,     7,   0xff,    2,  0},
	{2,  1666,    5,     8,   0xff,    2,  0},
	{1,   850,    4,     5,   0xff,    2,  0},
	{1,   950,    4,     7,   0xff,    2,  0},
	{1,  1666,    4,     8,   0xff,    2,  0},
	{}
};

static const struct board_specific_params rdimm[] = {
	/*
	 * memory controller 0
	 *   num|  hi|  clk| wrlvl | cpo  |wrdata|2T
	 * ranks| mhz|adjst| start |      |delay |
	 */
	{4,   850,    4,     6,   0xff,    2,  0},
	{4,   950,    5,     7,   0xff,    2,  0},
	{4,  1050,    5,     8,   0xff,    2,  0},
	{4,  1250,    5,    10,   0xff,    2,  0},
	{4,  1350,    5,    11,   0xff,    2,  0},
	{4,  1666,    5,    12,   0xff,    2,  0},
	{2,   850,    4,     6,   0xff,    2,  0},
	{2,  1050,    4,     7,   0xff,    2,  0},
	{2,  1666,    4,     8,   0xff,    2,  0},
	{1,   850,    4,     5,   0xff,    2,  0},
	{1,   950,    4,     7,   0xff,    2,  0},
	{1,  1666,    4,     8,   0xff,    2,  0},
	{}
};

void fsl_ddr_board_options(memctl_options_t *popts,
				dimm_params_t *pdimm,
				unsigned int ctrl_num)
{
	const struct board_specific_params *pbsp, *pbsp_highest = NULL;
	ulong ddr_freq;

	if (ctrl_num) {
		printf("Wrong parameter for controller number %d", ctrl_num);
		return;
	}
	if (!pdimm->n_ranks)
		return;

	if (popts->registered_dimm_en)
		pbsp = rdimm;
	else
		pbsp = udimm;

	/* Get clk_adjust, cpo, write_data_delay,2T, according to the board ddr
	 * freqency and n_banks specified in board_specific_parameters table.
	 */
	ddr_freq = get_ddr_freq(0) / 1000000;
	while (pbsp->datarate_mhz_high) {
		if (pbsp->n_ranks == pdimm->n_ranks) {
			if (ddr_freq <= pbsp->datarate_mhz_high) {
				popts->cpo_override = pbsp->cpo;
				popts->write_data_delay =
					pbsp->write_data_delay;
				popts->clk_adjust = pbsp->clk_adjust;
				popts->wrlvl_start = pbsp->wrlvl_start;
				popts->twoT_en = pbsp->force_2T;
				goto found;
			}
			pbsp_highest = pbsp;
		}
		pbsp++;
	}

	if (pbsp_highest) {
		printf("Error: board specific timing not found "
			"for data rate %lu MT/s!\n"
			"Trying to use the highest speed (%u) parameters\n",
			ddr_freq, pbsp_highest->datarate_mhz_high);
		popts->cpo_override = pbsp_highest->cpo;
		popts->write_data_delay = pbsp_highest->write_data_delay;
		popts->clk_adjust = pbsp_highest->clk_adjust;
		popts->wrlvl_start = pbsp_highest->wrlvl_start;
		popts->twoT_en = pbsp_highest->force_2T;
	} else {
		panic("DIMM is not supported by this board");
	}


found:

	/*
	 * The datasheet of HMT125U7BFR8C-H9 blocks CL=7 as reservered.
	 * However SPD still claims CL=7 is supported. Extensive tests
	 * confirmed this board cannot work stably with CL=7 with this
	 * particular DIMM.
	 */
	if (ddr_freq >= 800 && ddr_freq < 1066 && \
		!strncmp(pdimm[0].mpart, "HMT125U7BFR8C-H9", 16)) {
		popts->cas_latency_override = 1;
		popts->cas_latency_override_value = 8;
		debug("Override CL to 8\n");
	}
	/*
	 * Factors to consider for half-strength driver enable:
	 *	- number of DIMMs installed
	 */
	popts->half_strength_driver_enable = 0;
	/*
	 * Write leveling override
	 */
	popts->wrlvl_override = 1;
	popts->wrlvl_sample = 0xf;

	/*
	 * Rtt and Rtt_WR override
	 */
	popts->rtt_override = 0;

	/* Enable ZQ calibration */
	popts->zq_en = 1;

	/* DHC_EN =1, ODT = 60 Ohm */
	popts->ddr_cdr1 = DDR_CDR1_DHC_EN;
}

phys_size_t initdram(int board_type)
{
	phys_size_t dram_size;

	puts("Initializing....");

	if (fsl_use_spd()) {
		puts("using SPD\n");
		dram_size = fsl_ddr_sdram();
	} else {
		puts("using fixed parameters\n");
		dram_size = fixed_sdram();
	}

	dram_size = setup_ddr_tlbs(dram_size / 0x100000);
	dram_size *= 0x100000;

	debug("    DDR: ");
	return dram_size;
}
