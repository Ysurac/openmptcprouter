/*
 * Copyright 2011-2012 Freescale Semiconductor, Inc.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/mmu.h>
#include <asm/immap_85xx.h>
#include <asm/processor.h>
#include <asm/fsl_ddr_sdram.h>
#include <asm/fsl_ddr_dimm_params.h>
#include <asm/io.h>
#include <asm/fsl_law.h>

DECLARE_GLOBAL_DATA_PTR;

#ifndef CONFIG_SYS_DDR_RAW_TIMING
#define CONFIG_SYS_DRAM_SIZE	1024

fsl_ddr_cfg_regs_t ddr_cfg_regs_800 = {
	.cs[0].bnds = CONFIG_SYS_DDR_CS0_BNDS,
	.cs[0].config = CONFIG_SYS_DDR_CS0_CONFIG,
	.cs[0].config_2 = CONFIG_SYS_DDR_CS0_CONFIG_2,
	.timing_cfg_3 = CONFIG_SYS_DDR_TIMING_3_800,
	.timing_cfg_0 = CONFIG_SYS_DDR_TIMING_0_800,
	.timing_cfg_1 = CONFIG_SYS_DDR_TIMING_1_800,
	.timing_cfg_2 = CONFIG_SYS_DDR_TIMING_2_800,
	.ddr_sdram_cfg = CONFIG_SYS_DDR_CONTROL,
	.ddr_sdram_cfg_2 = CONFIG_SYS_DDR_CONTROL_2,
	.ddr_sdram_mode = CONFIG_SYS_DDR_MODE_1_800,
	.ddr_sdram_mode_2 = CONFIG_SYS_DDR_MODE_2_800,
	.ddr_sdram_md_cntl = CONFIG_SYS_DDR_MODE_CONTROL,
	.ddr_sdram_interval = CONFIG_SYS_DDR_INTERVAL_800,
	.ddr_data_init = CONFIG_MEM_INIT_VALUE,
	.ddr_sdram_clk_cntl = CONFIG_SYS_DDR_CLK_CTRL_800,
	.ddr_init_addr = CONFIG_SYS_DDR_INIT_ADDR,
	.ddr_init_ext_addr = CONFIG_SYS_DDR_INIT_EXT_ADDR,
	.timing_cfg_4 = CONFIG_SYS_DDR_TIMING_4,
	.timing_cfg_5 = CONFIG_SYS_DDR_TIMING_5,
	.ddr_zq_cntl = CONFIG_SYS_DDR_ZQ_CONTROL,
	.ddr_wrlvl_cntl = CONFIG_SYS_DDR_WRLVL_CONTROL_800,
	.ddr_sr_cntr = CONFIG_SYS_DDR_SR_CNTR,
	.ddr_sdram_rcw_1 = CONFIG_SYS_DDR_RCW_1,
	.ddr_sdram_rcw_2 = CONFIG_SYS_DDR_RCW_2
};

fixed_ddr_parm_t fixed_ddr_parm_0[] = {
	{750, 850, &ddr_cfg_regs_800},
	{0, 0, NULL}
};

unsigned long get_sdram_size(void)
{
	return get_ram_size(CONFIG_SYS_DDR_SDRAM_BASE, CONFIG_SYS_DRAM_SIZE);
}

/*
 * Fixed sdram init -- doesn't use serial presence detect.
 */
phys_size_t fixed_sdram(void)
{
	int i;
	char buf[32];
	fsl_ddr_cfg_regs_t ddr_cfg_regs;
	phys_size_t ddr_size;
	ulong ddr_freq, ddr_freq_mhz;

	ddr_freq = get_ddr_freq(0);
	ddr_freq_mhz = ddr_freq / 1000000;

	printf("Configuring DDR for %s MT/s data rate\n",
				strmhz(buf, ddr_freq));

	for (i = 0; fixed_ddr_parm_0[i].max_freq > 0; i++) {
		if ((ddr_freq_mhz > fixed_ddr_parm_0[i].min_freq) &&
		   (ddr_freq_mhz <= fixed_ddr_parm_0[i].max_freq)) {
			memcpy(&ddr_cfg_regs, fixed_ddr_parm_0[i].ddr_settings,
							sizeof(ddr_cfg_regs));
			break;
		}
	}

	if (fixed_ddr_parm_0[i].max_freq == 0) {
		panic("Unsupported DDR data rate %s MT/s data rate\n",
					strmhz(buf, ddr_freq));
	}

	ddr_size = (phys_size_t) CONFIG_SYS_SDRAM_SIZE * 1024 * 1024;
	fsl_ddr_set_memctl_regs(&ddr_cfg_regs, 0);

	if (set_ddr_laws(CONFIG_SYS_DDR_SDRAM_BASE, ddr_size,
					LAW_TRGT_IF_DDR_1) < 0) {
		printf("ERROR setting Local Access Windows for DDR\n");
		return 0;
	}

	return ddr_size;
}

#else /* CONFIG_SYS_DDR_RAW_TIMING */
/* Micron MT41J256M8HX-15E */
dimm_params_t ddr_raw_timing = {
	.n_ranks = 1,
	.rank_density = 1073741824u,
	.capacity = 1073741824u,
	.primary_sdram_width = 32,
	.ec_sdram_width = 0,
	.registered_dimm = 0,
	.mirrored_dimm = 0,
	.n_row_addr = 15,
	.n_col_addr = 10,
	.n_banks_per_sdram_device = 8,
	.edc_config = 0,
	.burst_lengths_bitmask = 0x0c,

	.tCKmin_X_ps = 1870,
	.caslat_X = 0x1e << 4,	/* 5,6,7,8 */
	.tAA_ps = 13125,
	.tWR_ps = 15000,
	.tRCD_ps = 13125,
	.tRRD_ps = 7500,
	.tRP_ps = 13125,
	.tRAS_ps = 37500,
	.tRC_ps = 50625,
	.tRFC_ps = 160000,
	.tWTR_ps = 7500,
	.tRTP_ps = 7500,
	.refresh_rate_ps = 7800000,
	.tFAW_ps = 37500,
};

int fsl_ddr_get_dimm_params(dimm_params_t *pdimm,
		unsigned int controller_number,
		unsigned int dimm_number)
{
	const char dimm_model[] = "Fixed DDR on board";

	if ((controller_number == 0) && (dimm_number == 0)) {
		memcpy(pdimm, &ddr_raw_timing, sizeof(dimm_params_t));
		memset(pdimm->mpart, 0, sizeof(pdimm->mpart));
		memcpy(pdimm->mpart, dimm_model, sizeof(dimm_model) - 1);
	}

	return 0;
}

void fsl_ddr_board_options(memctl_options_t *popts,
				dimm_params_t *pdimm,
				unsigned int ctrl_num)
{
	int i;
	popts->clk_adjust = 6;
	popts->cpo_override = 0x1f;
	popts->write_data_delay = 2;
	popts->half_strength_driver_enable = 1;
	/* Write leveling override */
	popts->wrlvl_en = 1;
	popts->wrlvl_override = 1;
	popts->wrlvl_sample = 0xf;
	popts->wrlvl_start = 0x8;
	popts->trwt_override = 1;
	popts->trwt = 0;

	for (i = 0; i < CONFIG_CHIP_SELECTS_PER_CTRL; i++) {
		popts->cs_local_opts[i].odt_rd_cfg = FSL_DDR_ODT_NEVER;
		popts->cs_local_opts[i].odt_wr_cfg = FSL_DDR_ODT_CS;
	}
}

#endif /* CONFIG_SYS_DDR_RAW_TIMING */
