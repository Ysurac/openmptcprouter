/*
 * (C) Copyright 2012
 * Holger Brunck, Keymile GmbH Hannover, <holger.brunck@keymile.com>
 * Christian Herzig, Keymile AG Switzerland, <christian.herzig@keymile.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/* KMBEC FPGA (PRIO) */
#define CONFIG_SYS_KMBEC_FPGA_BASE	0xE8000000
#define CONFIG_SYS_KMBEC_FPGA_SIZE	64

#if defined CONFIG_KMETER1
#define CONFIG_HOSTNAME		kmeter1
#define CONFIG_KM_BOARD_NAME   "kmeter1"
#define CONFIG_KM_DEF_NETDEV	"netdev=eth2\0"
#elif defined CONFIG_KMCOGE5NE
#define CONFIG_HOSTNAME		kmcoge5ne
#define CONFIG_KM_BOARD_NAME	"kmcoge5ne"
#define CONFIG_KM_DEF_NETDEV	"netdev=eth1\0"
#define CONFIG_CMD_NAND
#define CONFIG_NAND_KMETER1
#define CONFIG_SYS_MAX_NAND_DEVICE		1
#define NAND_MAX_CHIPS				1
#define CONFIG_SYS_NAND_BASE CONFIG_SYS_KMBEC_FPGA_BASE /* PRIO_BASE_ADDRESS */

#define CONFIG_KM_UBI_PARTITION_NAME_BOOT	"ubi0"
#define CONFIG_KM_UBI_PARTITION_NAME_APP	"ubi1"
#define MTDIDS_DEFAULT			"nor0=boot,nand0=app"

#define MTDPARTS_DEFAULT		"mtdparts="			\
	"boot:"								\
		"768k(u-boot),"						\
		"128k(env),"						\
		"128k(envred),"						\
		"-(" CONFIG_KM_UBI_PARTITION_NAME_BOOT ");"		\
	"app:"								\
		"-(" CONFIG_KM_UBI_PARTITION_NAME_APP ");"
#else
#error ("Board not supported")
#endif

/*
 * High Level Configuration Options
 */
#define CONFIG_QE			/* Has QE */
#define CONFIG_MPC8360			/* MPC8360 CPU specific */

#define	CONFIG_SYS_TEXT_BASE	0xF0000000

/* include common defines/options for all 83xx Keymile boards */
#include "km/km83xx-common.h"

/*
 * System IO Setup
 */
#define CONFIG_SYS_SICRH		(SICRH_UC1EOBI | SICRH_UC2E1OBI)

/*
 * Hardware Reset Configuration Word
 */
#define CONFIG_SYS_HRCW_LOW (\
	HRCWL_CSB_TO_CLKIN_4X1 | \
	HRCWL_CORE_TO_CSB_2X1 | \
	HRCWL_CE_PLL_VCO_DIV_2 | \
	HRCWL_CE_TO_PLL_1X6)

#define CONFIG_SYS_HRCW_HIGH (\
	HRCWH_CORE_ENABLE | \
	HRCWH_FROM_0X00000100 | \
	HRCWH_BOOTSEQ_DISABLE | \
	HRCWH_SW_WATCHDOG_DISABLE | \
	HRCWH_ROM_LOC_LOCAL_16BIT | \
	HRCWH_BIG_ENDIAN | \
	HRCWH_LALE_EARLY | \
	HRCWH_LDP_CLEAR)

/**
 * DDR RAM settings
 */
#define CONFIG_SYS_DDR_SDRAM_CFG (\
	SDRAM_CFG_SDRAM_TYPE_DDR2 | \
	SDRAM_CFG_SREN | \
	SDRAM_CFG_HSE)

#define CONFIG_SYS_DDR_SDRAM_CFG2	0x00401000

#ifdef CONFIG_KMCOGE5NE
/**
 * KMCOGE5NE has 512 MB RAM
 */
#define CONFIG_SYS_DDR_CS0_CONFIG (\
	CSCONFIG_EN | \
	CSCONFIG_AP | \
	CSCONFIG_ODT_RD_ONLY_CURRENT | \
	CSCONFIG_BANK_BIT_3 | \
	CSCONFIG_ROW_BIT_13 | \
	CSCONFIG_COL_BIT_10)
#else
#define CONFIG_SYS_DDR_CS0_CONFIG	(CSCONFIG_EN | CSCONFIG_AP | \
					 CSCONFIG_ROW_BIT_13 | \
					 CSCONFIG_COL_BIT_10 | \
					 CSCONFIG_ODT_RD_ONLY_CURRENT)
#endif

#define CONFIG_SYS_DDR_CLK_CNTL (\
	DDR_SDRAM_CLK_CNTL_CLK_ADJUST_05)

#define CONFIG_SYS_DDR_INTERVAL (\
	(0x080 << SDRAM_INTERVAL_BSTOPRE_SHIFT) | \
	(0x203 << SDRAM_INTERVAL_REFINT_SHIFT))

#define CONFIG_SYS_DDR_CS0_BNDS			0x0000007f

#define CONFIG_SYS_DDRCDR (\
	DDRCDR_EN | \
	DDRCDR_Q_DRN)
#define CONFIG_SYS_DDR_MODE		0x47860452
#define CONFIG_SYS_DDR_MODE2		0x8080c000

#define CONFIG_SYS_DDR_TIMING_0 (\
	(2 << TIMING_CFG0_MRS_CYC_SHIFT) | \
	(8 << TIMING_CFG0_ODT_PD_EXIT_SHIFT) | \
	(6 << TIMING_CFG0_PRE_PD_EXIT_SHIFT) | \
	(2 << TIMING_CFG0_ACT_PD_EXIT_SHIFT) | \
	(0 << TIMING_CFG0_WWT_SHIFT) | \
	(0 << TIMING_CFG0_RRT_SHIFT) | \
	(0 << TIMING_CFG0_WRT_SHIFT) | \
	(0 << TIMING_CFG0_RWT_SHIFT))

#define CONFIG_SYS_DDR_TIMING_1	((TIMING_CFG1_CASLAT_50) | \
				 (2 << TIMING_CFG1_WRTORD_SHIFT) | \
				 (2 << TIMING_CFG1_ACTTOACT_SHIFT) | \
				 (3 << TIMING_CFG1_WRREC_SHIFT) | \
				 (7 << TIMING_CFG1_REFREC_SHIFT) | \
				 (3 << TIMING_CFG1_ACTTORW_SHIFT) | \
				 (8 << TIMING_CFG1_ACTTOPRE_SHIFT) | \
				 (3 << TIMING_CFG1_PRETOACT_SHIFT))

#define CONFIG_SYS_DDR_TIMING_2 (\
	(0xa << TIMING_CFG2_FOUR_ACT_SHIFT) | \
	(3 << TIMING_CFG2_CKE_PLS_SHIFT) | \
	(2 << TIMING_CFG2_WR_DATA_DELAY_SHIFT) | \
	(2 << TIMING_CFG2_RD_TO_PRE_SHIFT) | \
	(4 << TIMING_CFG2_WR_LAT_DELAY_SHIFT) | \
	(5 << TIMING_CFG2_CPO_SHIFT) | \
	(0 << TIMING_CFG2_ADD_LAT_SHIFT))

#define CONFIG_SYS_DDR_TIMING_3			0x00000000

/* EEprom support */
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN		2

/*
 * Local Bus Configuration & Clock Setup
 */
#define CONFIG_SYS_LCRR_DBYP		LCRR_DBYP
#define CONFIG_SYS_LCRR_EADC		LCRR_EADC_2
#define CONFIG_SYS_LCRR_CLKDIV		LCRR_CLKDIV_4

/*
 * PAXE on the local bus CS3
 */
#define CONFIG_SYS_PAXE_BASE		0xA0000000
#define CONFIG_SYS_PAXE_SIZE		256

#define CONFIG_SYS_LBLAWBAR3_PRELIM	CONFIG_SYS_PAXE_BASE

#define CONFIG_SYS_LBLAWAR3_PRELIM	0x8000001C /* 512MB window size */

#define CONFIG_SYS_BR3_PRELIM (\
	CONFIG_SYS_PAXE_BASE | \
	(1 << BR_PS_SHIFT) | \
	BR_V)

#define CONFIG_SYS_OR3_PRELIM (\
	MEG_TO_AM(CONFIG_SYS_PAXE_SIZE) | \
	OR_GPCM_CSNT | \
	OR_GPCM_ACS_DIV2 | \
	OR_GPCM_SCY_2 | \
	OR_GPCM_TRLX | \
	OR_GPCM_EAD)

#ifdef CONFIG_KMCOGE5NE
/*
 * BFTIC3 on the local bus CS4
 */
#define CONFIG_SYS_BFTIC3_BASE			0xB0000000
#define CONFIG_SYS_BFTIC3_SIZE			256

#define CONFIG_SYS_BR4_PRELIM (\
	CONFIG_SYS_BFTIC3_BASE |\
	(1 << BR_PS_SHIFT) | \
	BR_V)

#define CONFIG_SYS_OR4_PRELIM (\
	MEG_TO_AM(CONFIG_SYS_BFTIC3_SIZE) |\
	OR_GPCM_CSNT | \
	OR_GPCM_ACS_DIV2 |\
	OR_GPCM_SCY_2 |\
	OR_GPCM_TRLX |\
	OR_GPCM_EAD)
#endif

/*
 * MMU Setup
 */

/* PAXE:  icache cacheable, but dcache-inhibit and guarded */
#define CONFIG_SYS_IBAT5L (\
	CONFIG_SYS_PAXE_BASE | \
	BATL_PP_10 | \
	BATL_MEMCOHERENCE)

#define CONFIG_SYS_IBAT5U (\
	CONFIG_SYS_PAXE_BASE | \
	BATU_BL_256M | \
	BATU_VS | \
	BATU_VP)

#define CONFIG_SYS_DBAT5L (\
	CONFIG_SYS_PAXE_BASE | \
	BATL_PP_10 | \
	BATL_CACHEINHIBIT | \
	BATL_GUARDEDSTORAGE)

#define CONFIG_SYS_DBAT5U	CONFIG_SYS_IBAT5U


#ifdef CONFIG_KMCOGE5NE
/* BFTIC3:  icache cacheable, but dcache-inhibit and guarded */
#define CONFIG_SYS_IBAT6L (\
	CONFIG_SYS_BFTIC3_BASE | \
	BATL_PP_10 | \
	BATL_MEMCOHERENCE)

#define CONFIG_SYS_IBAT6U (\
	CONFIG_SYS_BFTIC3_BASE | \
	BATU_BL_256M | \
	BATU_VS | \
	BATU_VP)

#define CONFIG_SYS_DBAT6L (\
	CONFIG_SYS_BFTIC3_BASE | \
	BATL_PP_10 | \
	BATL_CACHEINHIBIT | \
	BATL_GUARDEDSTORAGE)

#define CONFIG_SYS_DBAT6U	CONFIG_SYS_IBAT6U

/* DDR/LBC SDRAM next 256M: cacheable */
#define CONFIG_SYS_IBAT7L (\
	CONFIG_SYS_SDRAM_BASE2 |\
	BATL_PP_10 |\
	BATL_CACHEINHIBIT |\
	BATL_GUARDEDSTORAGE)

#define CONFIG_SYS_IBAT7U (\
	CONFIG_SYS_SDRAM_BASE2 |\
	BATU_BL_256M |\
	BATU_VS |\
	BATU_VP)
/* enable POST tests */
#define CONFIG_POST (CONFIG_SYS_POST_MEMORY|CONFIG_SYS_POST_MEM_REGIONS)
#define CONFIG_POST_EXTERNAL_WORD_FUNCS /* use own functions, not generic */
#define CPM_POST_WORD_ADDR  CONFIG_SYS_MEMTEST_END
#define CONFIG_TESTPIN_REG  gprt3	/* for kmcoge5ne */
#define CONFIG_TESTPIN_MASK 0x20	/* for kmcoge5ne */
#define CONFIG_CMD_DIAG	/* so that testpin is inquired for POST test */

#else
#define CONFIG_SYS_IBAT6L	(0)
#define CONFIG_SYS_IBAT6U	(0)
#define CONFIG_SYS_IBAT7L	(0)
#define CONFIG_SYS_IBAT7U	(0)
#define CONFIG_SYS_DBAT6L	CONFIG_SYS_IBAT6L
#define CONFIG_SYS_DBAT6U	CONFIG_SYS_IBAT6U
#endif

#define CONFIG_SYS_DBAT7L	CONFIG_SYS_IBAT7L
#define CONFIG_SYS_DBAT7U	CONFIG_SYS_IBAT7U

#endif /* CONFIG */
