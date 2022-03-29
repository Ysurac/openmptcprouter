/*
 * Copyright 2008-2009 Freescale Semiconductor, Inc.
 *	Dave Liu <daveliu@freescale.com>
 *
 * calculate the organization and timing parameter
 * from ddr3 spd, please refer to the spec
 * JEDEC standard No.21-C 4_01_02_11R18.pdf
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
 */

#include <common.h>
#include <asm/fsl_ddr_sdram.h>

#include "ddr.h"

/*
 * Calculate the Density of each Physical Rank.
 * Returned size is in bytes.
 *
 * each rank size =
 * sdram capacity(bit) / 8 * primary bus width / sdram width
 *
 * where: sdram capacity  = spd byte4[3:0]
 *        primary bus width = spd byte8[2:0]
 *        sdram width = spd byte7[2:0]
 *
 * SPD byte4 - sdram density and banks
 *	bit[3:0]	size(bit)	size(byte)
 *	0000		256Mb		32MB
 *	0001		512Mb		64MB
 *	0010		1Gb		128MB
 *	0011		2Gb		256MB
 *	0100		4Gb		512MB
 *	0101		8Gb		1GB
 *	0110		16Gb		2GB
 *
 * SPD byte8 - module memory bus width
 * 	bit[2:0]	primary bus width
 *	000		8bits
 * 	001		16bits
 * 	010		32bits
 * 	011		64bits
 *
 * SPD byte7 - module organiztion
 * 	bit[2:0]	sdram device width
 * 	000		4bits
 * 	001		8bits
 * 	010		16bits
 * 	011		32bits
 *
 */
static unsigned long long
compute_ranksize(const ddr3_spd_eeprom_t *spd)
{
	unsigned long long bsize;

	int nbit_sdram_cap_bsize = 0;
	int nbit_primary_bus_width = 0;
	int nbit_sdram_width = 0;

	if ((spd->density_banks & 0xf) < 7)
		nbit_sdram_cap_bsize = (spd->density_banks & 0xf) + 28;
	if ((spd->bus_width & 0x7) < 4)
		nbit_primary_bus_width = (spd->bus_width & 0x7) + 3;
	if ((spd->organization & 0x7) < 4)
		nbit_sdram_width = (spd->organization & 0x7) + 2;

	bsize = 1ULL << (nbit_sdram_cap_bsize - 3
		    + nbit_primary_bus_width - nbit_sdram_width);

	debug("DDR: DDR III rank density = 0x%16llx\n", bsize);

	return bsize;
}

/*
 * ddr_compute_dimm_parameters for DDR3 SPD
 *
 * Compute DIMM parameters based upon the SPD information in spd.
 * Writes the results to the dimm_params_t structure pointed by pdimm.
 *
 */
unsigned int
ddr_compute_dimm_parameters(const ddr3_spd_eeprom_t *spd,
			     dimm_params_t *pdimm,
			     unsigned int dimm_number)
{
	unsigned int retval;
	unsigned int mtb_ps;
	int i;

	if (spd->mem_type) {
		if (spd->mem_type != SPD_MEMTYPE_DDR3) {
			printf("DIMM %u: is not a DDR3 SPD.\n", dimm_number);
			return 1;
		}
	} else {
		memset(pdimm, 0, sizeof(dimm_params_t));
		return 1;
	}

	retval = ddr3_spd_check(spd);
	if (retval) {
		printf("DIMM %u: failed checksum\n", dimm_number);
		return 2;
	}

	/*
	 * The part name in ASCII in the SPD EEPROM is not null terminated.
	 * Guarantee null termination here by presetting all bytes to 0
	 * and copying the part name in ASCII from the SPD onto it
	 */
	memset(pdimm->mpart, 0, sizeof(pdimm->mpart));
	if ((spd->info_size_crc & 0xF) > 1)
		memcpy(pdimm->mpart, spd->mpart, sizeof(pdimm->mpart) - 1);

	/* DIMM organization parameters */
	pdimm->n_ranks = ((spd->organization >> 3) & 0x7) + 1;
	pdimm->rank_density = compute_ranksize(spd);
	pdimm->capacity = pdimm->n_ranks * pdimm->rank_density;
	pdimm->primary_sdram_width = 1 << (3 + (spd->bus_width & 0x7));
	if ((spd->bus_width >> 3) & 0x3)
		pdimm->ec_sdram_width = 8;
	else
		pdimm->ec_sdram_width = 0;
	pdimm->data_width = pdimm->primary_sdram_width
			  + pdimm->ec_sdram_width;

	/* These are the types defined by the JEDEC DDR3 SPD spec */
	pdimm->mirrored_dimm = 0;
	pdimm->registered_dimm = 0;
	switch (spd->module_type & DDR3_SPD_MODULETYPE_MASK) {
	case DDR3_SPD_MODULETYPE_RDIMM:
	case DDR3_SPD_MODULETYPE_MINI_RDIMM:
	case DDR3_SPD_MODULETYPE_72B_SO_RDIMM:
		/* Registered/buffered DIMMs */
		pdimm->registered_dimm = 1;
		for (i = 0; i < 16; i += 2) {
			u8 rcw = spd->mod_section.registered.rcw[i/2];
			pdimm->rcw[i]   = (rcw >> 0) & 0x0F;
			pdimm->rcw[i+1] = (rcw >> 4) & 0x0F;
		}
		break;

	case DDR3_SPD_MODULETYPE_UDIMM:
	case DDR3_SPD_MODULETYPE_SO_DIMM:
	case DDR3_SPD_MODULETYPE_MICRO_DIMM:
	case DDR3_SPD_MODULETYPE_MINI_UDIMM:
	case DDR3_SPD_MODULETYPE_MINI_CDIMM:
	case DDR3_SPD_MODULETYPE_72B_SO_UDIMM:
	case DDR3_SPD_MODULETYPE_72B_SO_CDIMM:
	case DDR3_SPD_MODULETYPE_LRDIMM:
	case DDR3_SPD_MODULETYPE_16B_SO_DIMM:
	case DDR3_SPD_MODULETYPE_32B_SO_DIMM:
		/* Unbuffered DIMMs */
		if (spd->mod_section.unbuffered.addr_mapping & 0x1)
			pdimm->mirrored_dimm = 1;
		break;

	default:
		printf("unknown module_type 0x%02X\n", spd->module_type);
		return 1;
	}

	/* SDRAM device parameters */
	pdimm->n_row_addr = ((spd->addressing >> 3) & 0x7) + 12;
	pdimm->n_col_addr = (spd->addressing & 0x7) + 9;
	pdimm->n_banks_per_sdram_device = 8 << ((spd->density_banks >> 4) & 0x7);

	/*
	 * The SPD spec has not the ECC bit,
	 * We consider the DIMM as ECC capability
	 * when the extension bus exist
	 */
	if (pdimm->ec_sdram_width)
		pdimm->edc_config = 0x02;
	else
		pdimm->edc_config = 0x00;

	/*
	 * The SPD spec has not the burst length byte
	 * but DDR3 spec has nature BL8 and BC4,
	 * BL8 -bit3, BC4 -bit2
	 */
	pdimm->burst_lengths_bitmask = 0x0c;
	pdimm->row_density = __ilog2(pdimm->rank_density);

	/* MTB - medium timebase
	 * The unit in the SPD spec is ns,
	 * We convert it to ps.
	 * eg: MTB = 0.125ns (125ps)
	 */
	mtb_ps = (spd->mtb_dividend * 1000) /spd->mtb_divisor;
	pdimm->mtb_ps = mtb_ps;

	/*
	 * sdram minimum cycle time
	 * we assume the MTB is 0.125ns
	 * eg:
	 * tCK_min=15 MTB (1.875ns) ->DDR3-1066
	 *        =12 MTB (1.5ns) ->DDR3-1333
	 *        =10 MTB (1.25ns) ->DDR3-1600
	 */
	pdimm->tCKmin_X_ps = spd->tCK_min * mtb_ps;

	/*
	 * CAS latency supported
	 * bit4 - CL4
	 * bit5 - CL5
	 * bit18 - CL18
	 */
	pdimm->caslat_X  = ((spd->caslat_msb << 8) | spd->caslat_lsb) << 4;

	/*
	 * min CAS latency time
	 * eg: tAA_min =
	 * DDR3-800D	100 MTB (12.5ns)
	 * DDR3-1066F	105 MTB (13.125ns)
	 * DDR3-1333H	108 MTB (13.5ns)
	 * DDR3-1600H	90 MTB (11.25ns)
	 */
	pdimm->tAA_ps = spd->tAA_min * mtb_ps;

	/*
	 * min write recovery time
	 * eg:
	 * tWR_min = 120 MTB (15ns) -> all speed grades.
	 */
	pdimm->tWR_ps = spd->tWR_min * mtb_ps;

	/*
	 * min RAS to CAS delay time
	 * eg: tRCD_min =
	 * DDR3-800	100 MTB (12.5ns)
	 * DDR3-1066F	105 MTB (13.125ns)
	 * DDR3-1333H	108 MTB (13.5ns)
	 * DDR3-1600H	90 MTB (11.25)
	 */
	pdimm->tRCD_ps = spd->tRCD_min * mtb_ps;

	/*
	 * min row active to row active delay time
	 * eg: tRRD_min =
	 * DDR3-800(1KB page)	80 MTB (10ns)
	 * DDR3-1333(1KB page)	48 MTB (6ns)
	 */
	pdimm->tRRD_ps = spd->tRRD_min * mtb_ps;

	/*
	 * min row precharge delay time
	 * eg: tRP_min =
	 * DDR3-800D	100 MTB (12.5ns)
	 * DDR3-1066F	105 MTB (13.125ns)
	 * DDR3-1333H	108 MTB (13.5ns)
	 * DDR3-1600H	90 MTB (11.25ns)
	 */
	pdimm->tRP_ps = spd->tRP_min * mtb_ps;

	/* min active to precharge delay time
	 * eg: tRAS_min =
	 * DDR3-800D	300 MTB (37.5ns)
	 * DDR3-1066F	300 MTB (37.5ns)
	 * DDR3-1333H	288 MTB (36ns)
	 * DDR3-1600H	280 MTB (35ns)
	 */
	pdimm->tRAS_ps = (((spd->tRAS_tRC_ext & 0xf) << 8) | spd->tRAS_min_lsb)
			* mtb_ps;
	/*
	 * min active to actice/refresh delay time
	 * eg: tRC_min =
	 * DDR3-800D	400 MTB (50ns)
	 * DDR3-1066F	405 MTB (50.625ns)
	 * DDR3-1333H	396 MTB (49.5ns)
	 * DDR3-1600H	370 MTB (46.25ns)
	 */
	pdimm->tRC_ps = (((spd->tRAS_tRC_ext & 0xf0) << 4) | spd->tRC_min_lsb)
			* mtb_ps;
	/*
	 * min refresh recovery delay time
	 * eg: tRFC_min =
	 * 512Mb	720 MTB (90ns)
	 * 1Gb		880 MTB (110ns)
	 * 2Gb		1280 MTB (160ns)
	 */
	pdimm->tRFC_ps = ((spd->tRFC_min_msb << 8) | spd->tRFC_min_lsb)
			* mtb_ps;
	/*
	 * min internal write to read command delay time
	 * eg: tWTR_min = 40 MTB (7.5ns) - all speed bins.
	 * tWRT is at least 4 mclk independent of operating freq.
	 */
	pdimm->tWTR_ps = spd->tWTR_min * mtb_ps;

	/*
	 * min internal read to precharge command delay time
	 * eg: tRTP_min = 40 MTB (7.5ns) - all speed bins.
	 * tRTP is at least 4 mclk independent of operating freq.
	 */
	pdimm->tRTP_ps = spd->tRTP_min * mtb_ps;

	/*
	 * Average periodic refresh interval
	 * tREFI = 7.8 us at normal temperature range
	 *       = 3.9 us at ext temperature range
	 */
	pdimm->refresh_rate_ps = 7800000;

	/*
	 * min four active window delay time
	 * eg: tFAW_min =
	 * DDR3-800(1KB page)	320 MTB (40ns)
	 * DDR3-1066(1KB page)	300 MTB (37.5ns)
	 * DDR3-1333(1KB page)	240 MTB (30ns)
	 * DDR3-1600(1KB page)	240 MTB (30ns)
	 */
	pdimm->tFAW_ps = (((spd->tFAW_msb & 0xf) << 8) | spd->tFAW_min)
			* mtb_ps;

	return 0;
}
