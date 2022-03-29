/*
 * MPC85xx Internal Memory Map
 *
 * Copyright 2007-2012 Freescale Semiconductor, Inc.
 *
 * Copyright(c) 2002,2003 Motorola Inc.
 * Xianghua Xiao (x.xiao@motorola.com)
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

#ifndef __IMMAP_85xx__
#define __IMMAP_85xx__

#include <asm/types.h>
#include <asm/fsl_dma.h>
#include <asm/fsl_i2c.h>
#include <asm/fsl_ifc.h>
#include <asm/fsl_lbc.h>
#include <asm/fsl_fman.h>

typedef struct ccsr_local {
	u32	ccsrbarh;	/* CCSR Base Addr High */
	u32	ccsrbarl;	/* CCSR Base Addr Low */
	u32	ccsrar;		/* CCSR Attr */
#define CCSRAR_C	0x80000000	/* Commit */
	u8	res1[4];
	u32	altcbarh;	/* Alternate Configuration Base Addr High */
	u32	altcbarl;	/* Alternate Configuration Base Addr Low */
	u32	altcar;		/* Alternate Configuration Attr */
	u8	res2[4];
	u32	bstrh;		/* Boot space translation high */
	u32	bstrl;		/* Boot space translation Low */
	u32	bstrar;		/* Boot space translation attributes */
	u8	res3[0xbd4];
	struct {
		u32	lawbarh;	/* LAWn base addr high */
		u32	lawbarl;	/* LAWn base addr low */
		u32	lawar;		/* LAWn attributes */
		u8	res4[4];
	} law[32];
	u8	res35[0x204];
} ccsr_local_t;

/* Local-Access Registers & ECM Registers */
typedef struct ccsr_local_ecm {
	u32	ccsrbar;	/* CCSR Base Addr */
	u8	res1[4];
	u32	altcbar;	/* Alternate Configuration Base Addr */
	u8	res2[4];
	u32	altcar;		/* Alternate Configuration Attr */
	u8	res3[12];
	u32	bptr;		/* Boot Page Translation */
	u8	res4[3044];
	u32	lawbar0;	/* Local Access Window 0 Base Addr */
	u8	res5[4];
	u32	lawar0;		/* Local Access Window 0 Attrs */
	u8	res6[20];
	u32	lawbar1;	/* Local Access Window 1 Base Addr */
	u8	res7[4];
	u32	lawar1;		/* Local Access Window 1 Attrs */
	u8	res8[20];
	u32	lawbar2;	/* Local Access Window 2 Base Addr */
	u8	res9[4];
	u32	lawar2;		/* Local Access Window 2 Attrs */
	u8	res10[20];
	u32	lawbar3;	/* Local Access Window 3 Base Addr */
	u8	res11[4];
	u32	lawar3;		/* Local Access Window 3 Attrs */
	u8	res12[20];
	u32	lawbar4;	/* Local Access Window 4 Base Addr */
	u8	res13[4];
	u32	lawar4;		/* Local Access Window 4 Attrs */
	u8	res14[20];
	u32	lawbar5;	/* Local Access Window 5 Base Addr */
	u8	res15[4];
	u32	lawar5;		/* Local Access Window 5 Attrs */
	u8	res16[20];
	u32	lawbar6;	/* Local Access Window 6 Base Addr */
	u8	res17[4];
	u32	lawar6;		/* Local Access Window 6 Attrs */
	u8	res18[20];
	u32	lawbar7;	/* Local Access Window 7 Base Addr */
	u8	res19[4];
	u32	lawar7;		/* Local Access Window 7 Attrs */
	u8	res19_8a[20];
	u32	lawbar8;	/* Local Access Window 8 Base Addr */
	u8	res19_8b[4];
	u32	lawar8;		/* Local Access Window 8 Attrs */
	u8	res19_9a[20];
	u32	lawbar9;	/* Local Access Window 9 Base Addr */
	u8	res19_9b[4];
	u32	lawar9;		/* Local Access Window 9 Attrs */
	u8	res19_10a[20];
	u32	lawbar10;	/* Local Access Window 10 Base Addr */
	u8	res19_10b[4];
	u32	lawar10;	/* Local Access Window 10 Attrs */
	u8	res19_11a[20];
	u32	lawbar11;	/* Local Access Window 11 Base Addr */
	u8	res19_11b[4];
	u32	lawar11;	/* Local Access Window 11 Attrs */
	u8	res20[652];
	u32	eebacr;		/* ECM CCB Addr Configuration */
	u8	res21[12];
	u32	eebpcr;		/* ECM CCB Port Configuration */
	u8	res22[3564];
	u32	eedr;		/* ECM Error Detect */
	u8	res23[4];
	u32	eeer;		/* ECM Error Enable */
	u32	eeatr;		/* ECM Error Attrs Capture */
	u32	eeadr;		/* ECM Error Addr Capture */
	u8	res24[492];
} ccsr_local_ecm_t;

/* DDR memory controller registers */
typedef struct ccsr_ddr {
	u32	cs0_bnds;		/* Chip Select 0 Memory Bounds */
	u8	res1[4];
	u32	cs1_bnds;		/* Chip Select 1 Memory Bounds */
	u8	res2[4];
	u32	cs2_bnds;		/* Chip Select 2 Memory Bounds */
	u8	res3[4];
	u32	cs3_bnds;		/* Chip Select 3 Memory Bounds */
	u8	res4[100];
	u32	cs0_config;		/* Chip Select Configuration */
	u32	cs1_config;		/* Chip Select Configuration */
	u32	cs2_config;		/* Chip Select Configuration */
	u32	cs3_config;		/* Chip Select Configuration */
	u8	res4a[48];
	u32	cs0_config_2;		/* Chip Select Configuration 2 */
	u32	cs1_config_2;		/* Chip Select Configuration 2 */
	u32	cs2_config_2;		/* Chip Select Configuration 2 */
	u32	cs3_config_2;		/* Chip Select Configuration 2 */
	u8	res5[48];
	u32	timing_cfg_3;		/* SDRAM Timing Configuration 3 */
	u32	timing_cfg_0;		/* SDRAM Timing Configuration 0 */
	u32	timing_cfg_1;		/* SDRAM Timing Configuration 1 */
	u32	timing_cfg_2;		/* SDRAM Timing Configuration 2 */
	u32	sdram_cfg;		/* SDRAM Control Configuration */
	u32	sdram_cfg_2;		/* SDRAM Control Configuration 2 */
	u32	sdram_mode;		/* SDRAM Mode Configuration */
	u32	sdram_mode_2;		/* SDRAM Mode Configuration 2 */
	u32	sdram_md_cntl;		/* SDRAM Mode Control */
	u32	sdram_interval;		/* SDRAM Interval Configuration */
	u32	sdram_data_init;	/* SDRAM Data initialization */
	u8	res6[4];
	u32	sdram_clk_cntl;		/* SDRAM Clock Control */
	u8	res7[20];
	u32	init_addr;		/* training init addr */
	u32	init_ext_addr;		/* training init extended addr */
	u8	res8_1[16];
	u32	timing_cfg_4;		/* SDRAM Timing Configuration 4 */
	u32	timing_cfg_5;		/* SDRAM Timing Configuration 5 */
	u8	reg8_1a[8];
	u32	ddr_zq_cntl;		/* ZQ calibration control*/
	u32	ddr_wrlvl_cntl;		/* write leveling control*/
	u8	reg8_1aa[4];
	u32	ddr_sr_cntr;		/* self refresh counter */
	u32	ddr_sdram_rcw_1;	/* Control Words 1 */
	u32	ddr_sdram_rcw_2;	/* Control Words 2 */
	u8	reg_1ab[8];
	u32	ddr_wrlvl_cntl_2;	/* write leveling control 2 */
	u32	ddr_wrlvl_cntl_3;	/* write leveling control 3 */
	u8	res8_1b[104];
	u32	sdram_mode_3;		/* SDRAM Mode Configuration 3 */
	u32	sdram_mode_4;		/* SDRAM Mode Configuration 4 */
	u32	sdram_mode_5;		/* SDRAM Mode Configuration 5 */
	u32	sdram_mode_6;		/* SDRAM Mode Configuration 6 */
	u32	sdram_mode_7;		/* SDRAM Mode Configuration 7 */
	u32	sdram_mode_8;		/* SDRAM Mode Configuration 8 */
	u8	res8_1ba[0x908];
	u32	ddr_dsr1;		/* Debug Status 1 */
	u32	ddr_dsr2;		/* Debug Status 2 */
	u32	ddr_cdr1;		/* Control Driver 1 */
	u32	ddr_cdr2;		/* Control Driver 2 */
	u8	res8_1c[200];
	u32	ip_rev1;		/* IP Block Revision 1 */
	u32	ip_rev2;		/* IP Block Revision 2 */
	u32	eor;			/* Enhanced Optimization Register */
	u8	res8_2[252];
	u32	mtcr;			/* Memory Test Control Register */
	u8	res8_3[28];
	u32	mtp1;			/* Memory Test Pattern 1 */
	u32	mtp2;			/* Memory Test Pattern 2 */
	u32	mtp3;			/* Memory Test Pattern 3 */
	u32	mtp4;			/* Memory Test Pattern 4 */
	u32	mtp5;			/* Memory Test Pattern 5 */
	u32	mtp6;			/* Memory Test Pattern 6 */
	u32	mtp7;			/* Memory Test Pattern 7 */
	u32	mtp8;			/* Memory Test Pattern 8 */
	u32	mtp9;			/* Memory Test Pattern 9 */
	u32	mtp10;			/* Memory Test Pattern 10 */
	u8	res8_4[184];
	u32	data_err_inject_hi;	/* Data Path Err Injection Mask High */
	u32	data_err_inject_lo;	/* Data Path Err Injection Mask Low */
	u32	ecc_err_inject;		/* Data Path Err Injection Mask ECC */
	u8	res9[20];
	u32	capture_data_hi;	/* Data Path Read Capture High */
	u32	capture_data_lo;	/* Data Path Read Capture Low */
	u32	capture_ecc;		/* Data Path Read Capture ECC */
	u8	res10[20];
	u32	err_detect;		/* Error Detect */
	u32	err_disable;		/* Error Disable */
	u32	err_int_en;
	u32	capture_attributes;	/* Error Attrs Capture */
	u32	capture_address;	/* Error Addr Capture */
	u32	capture_ext_address;	/* Error Extended Addr Capture */
	u32	err_sbe;		/* Single-Bit ECC Error Management */
	u8	res11[164];
	u32	debug[32];		/* debug_1 to debug_32 */
	u8	res12[128];
} ccsr_ddr_t;

#define DDR_EOR_RD_BDW_OPT_DIS	0x80000000 /* Read BDW Opt. disable */
#define DDR_EOR_ADDR_HASH_EN	0x40000000 /* Address hash enabled */

/* I2C Registers */
typedef struct ccsr_i2c {
	struct fsl_i2c	i2c[1];
	u8	res[4096 - 1 * sizeof(struct fsl_i2c)];
} ccsr_i2c_t;

#if defined(CONFIG_MPC8540) \
	|| defined(CONFIG_MPC8541) \
	|| defined(CONFIG_MPC8548) \
	|| defined(CONFIG_MPC8555)
/* DUART Registers */
typedef struct ccsr_duart {
	u8	res1[1280];
/* URBR1, UTHR1, UDLB1 with the same addr */
	u8	urbr1_uthr1_udlb1;
/* UIER1, UDMB1 with the same addr01 */
	u8	uier1_udmb1;
/* UIIR1, UFCR1, UAFR1 with the same addr */
	u8	uiir1_ufcr1_uafr1;
	u8	ulcr1;		/* UART1 Line Control */
	u8	umcr1;		/* UART1 Modem Control */
	u8	ulsr1;		/* UART1 Line Status */
	u8	umsr1;		/* UART1 Modem Status */
	u8	uscr1;		/* UART1 Scratch */
	u8	res2[8];
	u8	udsr1;		/* UART1 DMA Status */
	u8	res3[239];
/* URBR2, UTHR2, UDLB2 with the same addr */
	u8	urbr2_uthr2_udlb2;
/* UIER2, UDMB2 with the same addr */
	u8	uier2_udmb2;
/* UIIR2, UFCR2, UAFR2 with the same addr */
	u8	uiir2_ufcr2_uafr2;
	u8	ulcr2;		/* UART2 Line Control */
	u8	umcr2;		/* UART2 Modem Control */
	u8	ulsr2;		/* UART2 Line Status */
	u8	umsr2;		/* UART2 Modem Status */
	u8	uscr2;		/* UART2 Scratch */
	u8	res4[8];
	u8	udsr2;		/* UART2 DMA Status */
	u8	res5[2543];
} ccsr_duart_t;
#else /* MPC8560 uses UART on its CPM */
typedef struct ccsr_duart {
	u8 res[4096];
} ccsr_duart_t;
#endif

/* eSPI Registers */
typedef struct ccsr_espi {
	u32	mode;		/* eSPI mode */
	u32	event;		/* eSPI event */
	u32	mask;		/* eSPI mask */
	u32	com;		/* eSPI command */
	u32	tx;		/* eSPI transmit FIFO access */
	u32	rx;		/* eSPI receive FIFO access */
	u8	res1[8];	/* reserved */
	u32	csmode[4];	/* 0x2c: sSPI CS0/1/2/3 mode */
	u8	res2[4048];	/* fill up to 0x1000 */
} ccsr_espi_t;

/* PCI Registers */
typedef struct ccsr_pcix {
	u32	cfg_addr;	/* PCIX Configuration Addr */
	u32	cfg_data;	/* PCIX Configuration Data */
	u32	int_ack;	/* PCIX IRQ Acknowledge */
	u8	res1[3060];
	u32	potar0;		/* PCIX Outbound Transaction Addr 0 */
	u32	potear0;	/* PCIX Outbound Translation Extended Addr 0 */
	u32	powbar0;	/* PCIX Outbound Window Base Addr 0 */
	u32	powbear0;	/* PCIX Outbound Window Base Extended Addr 0 */
	u32	powar0;		/* PCIX Outbound Window Attrs 0 */
	u8	res2[12];
	u32	potar1;		/* PCIX Outbound Transaction Addr 1 */
	u32	potear1;	/* PCIX Outbound Translation Extended Addr 1 */
	u32	powbar1;	/* PCIX Outbound Window Base Addr 1 */
	u32	powbear1;	/* PCIX Outbound Window Base Extended Addr 1 */
	u32	powar1;		/* PCIX Outbound Window Attrs 1 */
	u8	res3[12];
	u32	potar2;		/* PCIX Outbound Transaction Addr 2 */
	u32	potear2;	/* PCIX Outbound Translation Extended Addr 2 */
	u32	powbar2;	/* PCIX Outbound Window Base Addr 2 */
	u32	powbear2;	/* PCIX Outbound Window Base Extended Addr 2 */
	u32	powar2;		/* PCIX Outbound Window Attrs 2 */
	u8	res4[12];
	u32	potar3;		/* PCIX Outbound Transaction Addr 3 */
	u32	potear3;	/* PCIX Outbound Translation Extended Addr 3 */
	u32	powbar3;	/* PCIX Outbound Window Base Addr 3 */
	u32	powbear3;	/* PCIX Outbound Window Base Extended Addr 3 */
	u32	powar3;		/* PCIX Outbound Window Attrs 3 */
	u8	res5[12];
	u32	potar4;		/* PCIX Outbound Transaction Addr 4 */
	u32	potear4;	/* PCIX Outbound Translation Extended Addr 4 */
	u32	powbar4;	/* PCIX Outbound Window Base Addr 4 */
	u32	powbear4;	/* PCIX Outbound Window Base Extended Addr 4 */
	u32	powar4;		/* PCIX Outbound Window Attrs 4 */
	u8	res6[268];
	u32	pitar3;		/* PCIX Inbound Translation Addr 3 */
	u32	pitear3;	/* PCIX Inbound Translation Extended Addr 3 */
	u32	piwbar3;	/* PCIX Inbound Window Base Addr 3 */
	u32	piwbear3;	/* PCIX Inbound Window Base Extended Addr 3 */
	u32	piwar3;		/* PCIX Inbound Window Attrs 3 */
	u8	res7[12];
	u32	pitar2;		/* PCIX Inbound Translation Addr 2 */
	u32	pitear2;	/* PCIX Inbound Translation Extended Addr 2 */
	u32	piwbar2;	/* PCIX Inbound Window Base Addr 2 */
	u32	piwbear2;	/* PCIX Inbound Window Base Extended Addr 2 */
	u32	piwar2;		/* PCIX Inbound Window Attrs 2 */
	u8	res8[12];
	u32	pitar1;		/* PCIX Inbound Translation Addr 1 */
	u32	pitear1;	/* PCIX Inbound Translation Extended Addr 1 */
	u32	piwbar1;	/* PCIX Inbound Window Base Addr 1 */
	u8	res9[4];
	u32	piwar1;		/* PCIX Inbound Window Attrs 1 */
	u8	res10[12];
	u32	pedr;		/* PCIX Error Detect */
	u32	pecdr;		/* PCIX Error Capture Disable */
	u32	peer;		/* PCIX Error Enable */
	u32	peattrcr;	/* PCIX Error Attrs Capture */
	u32	peaddrcr;	/* PCIX Error Addr Capture */
	u32	peextaddrcr;	/* PCIX Error Extended Addr Capture */
	u32	pedlcr;		/* PCIX Error Data Low Capture */
	u32	pedhcr;		/* PCIX Error Error Data High Capture */
	u32	gas_timr;	/* PCIX Gasket Timer */
	u8	res11[476];
} ccsr_pcix_t;

#define PCIX_COMMAND	0x62
#define POWAR_EN	0x80000000
#define POWAR_IO_READ	0x00080000
#define POWAR_MEM_READ	0x00040000
#define POWAR_IO_WRITE	0x00008000
#define POWAR_MEM_WRITE	0x00004000
#define POWAR_MEM_512M	0x0000001c
#define POWAR_IO_1M	0x00000013

#define PIWAR_EN	0x80000000
#define PIWAR_PF	0x20000000
#define PIWAR_LOCAL	0x00f00000
#define PIWAR_READ_SNOOP	0x00050000
#define PIWAR_WRITE_SNOOP	0x00005000
#define PIWAR_MEM_2G		0x0000001e

typedef struct ccsr_gpio {
	u32	gpdir;
	u32	gpodr;
	u32	gpdat;
	u32	gpier;
	u32	gpimr;
	u32	gpicr;
} ccsr_gpio_t;

/* L2 Cache Registers */
typedef struct ccsr_l2cache {
	u32	l2ctl;		/* L2 configuration 0 */
	u8	res1[12];
	u32	l2cewar0;	/* L2 cache external write addr 0 */
	u8	res2[4];
	u32	l2cewcr0;	/* L2 cache external write control 0 */
	u8	res3[4];
	u32	l2cewar1;	/* L2 cache external write addr 1 */
	u8	res4[4];
	u32	l2cewcr1;	/* L2 cache external write control 1 */
	u8	res5[4];
	u32	l2cewar2;	/* L2 cache external write addr 2 */
	u8	res6[4];
	u32	l2cewcr2;	/* L2 cache external write control 2 */
	u8	res7[4];
	u32	l2cewar3;	/* L2 cache external write addr 3 */
	u8	res8[4];
	u32	l2cewcr3;	/* L2 cache external write control 3 */
	u8	res9[180];
	u32	l2srbar0;	/* L2 memory-mapped SRAM base addr 0 */
	u8	res10[4];
	u32	l2srbar1;	/* L2 memory-mapped SRAM base addr 1 */
	u8	res11[3316];
	u32	l2errinjhi;	/* L2 error injection mask high */
	u32	l2errinjlo;	/* L2 error injection mask low */
	u32	l2errinjctl;	/* L2 error injection tag/ECC control */
	u8	res12[20];
	u32	l2captdatahi;	/* L2 error data high capture */
	u32	l2captdatalo;	/* L2 error data low capture */
	u32	l2captecc;	/* L2 error ECC capture */
	u8	res13[20];
	u32	l2errdet;	/* L2 error detect */
	u32	l2errdis;	/* L2 error disable */
	u32	l2errinten;	/* L2 error interrupt enable */
	u32	l2errattr;	/* L2 error attributes capture */
	u32	l2erraddr;	/* L2 error addr capture */
	u8	res14[4];
	u32	l2errctl;	/* L2 error control */
	u8	res15[420];
} ccsr_l2cache_t;

#define MPC85xx_L2CTL_L2E			0x80000000
#define MPC85xx_L2CTL_L2SRAM_ENTIRE		0x00010000
#define MPC85xx_L2ERRDIS_MBECC			0x00000008
#define MPC85xx_L2ERRDIS_SBECC			0x00000004

/* DMA Registers */
typedef struct ccsr_dma {
	u8	res1[256];
	struct fsl_dma dma[4];
	u32	dgsr;		/* DMA General Status */
	u8	res2[11516];
} ccsr_dma_t;

/* tsec */
typedef struct ccsr_tsec {
	u8	res1[16];
	u32	ievent;		/* IRQ Event */
	u32	imask;		/* IRQ Mask */
	u32	edis;		/* Error Disabled */
	u8	res2[4];
	u32	ecntrl;		/* Ethernet Control */
	u32	minflr;		/* Minimum Frame Len */
	u32	ptv;		/* Pause Time Value */
	u32	dmactrl;	/* DMA Control */
	u32	tbipa;		/* TBI PHY Addr */
	u8	res3[88];
	u32	fifo_tx_thr;		/* FIFO transmit threshold */
	u8	res4[8];
	u32	fifo_tx_starve;		/* FIFO transmit starve */
	u32	fifo_tx_starve_shutoff;	/* FIFO transmit starve shutoff */
	u8	res5[96];
	u32	tctrl;		/* TX Control */
	u32	tstat;		/* TX Status */
	u8	res6[4];
	u32	tbdlen;		/* TX Buffer Desc Data Len */
	u8	res7[16];
	u32	ctbptrh;	/* Current TX Buffer Desc Ptr High */
	u32	ctbptr;		/* Current TX Buffer Desc Ptr */
	u8	res8[88];
	u32	tbptrh;		/* TX Buffer Desc Ptr High */
	u32	tbptr;		/* TX Buffer Desc Ptr Low */
	u8	res9[120];
	u32	tbaseh;		/* TX Desc Base Addr High */
	u32	tbase;		/* TX Desc Base Addr */
	u8	res10[168];
	u32	ostbd;		/* Out-of-Sequence(OOS) TX Buffer Desc */
	u32	ostbdp;		/* OOS TX Data Buffer Ptr */
	u32	os32tbdp;	/* OOS 32 Bytes TX Data Buffer Ptr Low */
	u32	os32iptrh;	/* OOS 32 Bytes TX Insert Ptr High */
	u32	os32iptrl;	/* OOS 32 Bytes TX Insert Ptr Low */
	u32	os32tbdr;	/* OOS 32 Bytes TX Reserved */
	u32	os32iil;	/* OOS 32 Bytes TX Insert Idx/Len */
	u8	res11[52];
	u32	rctrl;		/* RX Control */
	u32	rstat;		/* RX Status */
	u8	res12[4];
	u32	rbdlen;		/* RxBD Data Len */
	u8	res13[16];
	u32	crbptrh;	/* Current RX Buffer Desc Ptr High */
	u32	crbptr;		/* Current RX Buffer Desc Ptr */
	u8	res14[24];
	u32	mrblr;		/* Maximum RX Buffer Len */
	u32	mrblr2r3;	/* Maximum RX Buffer Len R2R3 */
	u8	res15[56];
	u32	rbptrh;		/* RX Buffer Desc Ptr High 0 */
	u32	rbptr;		/* RX Buffer Desc Ptr */
	u32	rbptrh1;	/* RX Buffer Desc Ptr High 1 */
	u32	rbptrl1;	/* RX Buffer Desc Ptr Low 1 */
	u32	rbptrh2;	/* RX Buffer Desc Ptr High 2 */
	u32	rbptrl2;	/* RX Buffer Desc Ptr Low 2 */
	u32	rbptrh3;	/* RX Buffer Desc Ptr High 3 */
	u32	rbptrl3;	/* RX Buffer Desc Ptr Low 3 */
	u8	res16[96];
	u32	rbaseh;		/* RX Desc Base Addr High 0 */
	u32	rbase;		/* RX Desc Base Addr */
	u32	rbaseh1;	/* RX Desc Base Addr High 1 */
	u32	rbasel1;	/* RX Desc Base Addr Low 1 */
	u32	rbaseh2;	/* RX Desc Base Addr High 2 */
	u32	rbasel2;	/* RX Desc Base Addr Low 2 */
	u32	rbaseh3;	/* RX Desc Base Addr High 3 */
	u32	rbasel3;	/* RX Desc Base Addr Low 3 */
	u8	res17[224];
	u32	maccfg1;	/* MAC Configuration 1 */
	u32	maccfg2;	/* MAC Configuration 2 */
	u32	ipgifg;		/* Inter Packet Gap/Inter Frame Gap */
	u32	hafdup;		/* Half Duplex */
	u32	maxfrm;		/* Maximum Frame Len */
	u8	res18[12];
	u32	miimcfg;	/* MII Management Configuration */
	u32	miimcom;	/* MII Management Cmd */
	u32	miimadd;	/* MII Management Addr */
	u32	miimcon;	/* MII Management Control */
	u32	miimstat;	/* MII Management Status */
	u32	miimind;	/* MII Management Indicator */
	u8	res19[4];
	u32	ifstat;		/* Interface Status */
	u32	macstnaddr1;	/* Station Addr Part 1 */
	u32	macstnaddr2;	/* Station Addr Part 2 */
	u8	res20[312];
	u32	tr64;		/* TX & RX 64-byte Frame Counter */
	u32	tr127;		/* TX & RX 65-127 byte Frame Counter */
	u32	tr255;		/* TX & RX 128-255 byte Frame Counter */
	u32	tr511;		/* TX & RX 256-511 byte Frame Counter */
	u32	tr1k;		/* TX & RX 512-1023 byte Frame Counter */
	u32	trmax;		/* TX & RX 1024-1518 byte Frame Counter */
	u32	trmgv;		/* TX & RX 1519-1522 byte Good VLAN Frame */
	u32	rbyt;		/* RX Byte Counter */
	u32	rpkt;		/* RX Packet Counter */
	u32	rfcs;		/* RX FCS Error Counter */
	u32	rmca;		/* RX Multicast Packet Counter */
	u32	rbca;		/* RX Broadcast Packet Counter */
	u32	rxcf;		/* RX Control Frame Packet Counter */
	u32	rxpf;		/* RX Pause Frame Packet Counter */
	u32	rxuo;		/* RX Unknown OP Code Counter */
	u32	raln;		/* RX Alignment Error Counter */
	u32	rflr;		/* RX Frame Len Error Counter */
	u32	rcde;		/* RX Code Error Counter */
	u32	rcse;		/* RX Carrier Sense Error Counter */
	u32	rund;		/* RX Undersize Packet Counter */
	u32	rovr;		/* RX Oversize Packet Counter */
	u32	rfrg;		/* RX Fragments Counter */
	u32	rjbr;		/* RX Jabber Counter */
	u32	rdrp;		/* RX Drop Counter */
	u32	tbyt;		/* TX Byte Counter Counter */
	u32	tpkt;		/* TX Packet Counter */
	u32	tmca;		/* TX Multicast Packet Counter */
	u32	tbca;		/* TX Broadcast Packet Counter */
	u32	txpf;		/* TX Pause Control Frame Counter */
	u32	tdfr;		/* TX Deferral Packet Counter */
	u32	tedf;		/* TX Excessive Deferral Packet Counter */
	u32	tscl;		/* TX Single Collision Packet Counter */
	u32	tmcl;		/* TX Multiple Collision Packet Counter */
	u32	tlcl;		/* TX Late Collision Packet Counter */
	u32	txcl;		/* TX Excessive Collision Packet Counter */
	u32	tncl;		/* TX Total Collision Counter */
	u8	res21[4];
	u32	tdrp;		/* TX Drop Frame Counter */
	u32	tjbr;		/* TX Jabber Frame Counter */
	u32	tfcs;		/* TX FCS Error Counter */
	u32	txcf;		/* TX Control Frame Counter */
	u32	tovr;		/* TX Oversize Frame Counter */
	u32	tund;		/* TX Undersize Frame Counter */
	u32	tfrg;		/* TX Fragments Frame Counter */
	u32	car1;		/* Carry One */
	u32	car2;		/* Carry Two */
	u32	cam1;		/* Carry Mask One */
	u32	cam2;		/* Carry Mask Two */
	u8	res22[192];
	u32	iaddr0;		/* Indivdual addr 0 */
	u32	iaddr1;		/* Indivdual addr 1 */
	u32	iaddr2;		/* Indivdual addr 2 */
	u32	iaddr3;		/* Indivdual addr 3 */
	u32	iaddr4;		/* Indivdual addr 4 */
	u32	iaddr5;		/* Indivdual addr 5 */
	u32	iaddr6;		/* Indivdual addr 6 */
	u32	iaddr7;		/* Indivdual addr 7 */
	u8	res23[96];
	u32	gaddr0;		/* Global addr 0 */
	u32	gaddr1;		/* Global addr 1 */
	u32	gaddr2;		/* Global addr 2 */
	u32	gaddr3;		/* Global addr 3 */
	u32	gaddr4;		/* Global addr 4 */
	u32	gaddr5;		/* Global addr 5 */
	u32	gaddr6;		/* Global addr 6 */
	u32	gaddr7;		/* Global addr 7 */
	u8	res24[96];
	u32	pmd0;		/* Pattern Match Data */
	u8	res25[4];
	u32	pmask0;		/* Pattern Mask */
	u8	res26[4];
	u32	pcntrl0;	/* Pattern Match Control */
	u8	res27[4];
	u32	pattrb0;	/* Pattern Match Attrs */
	u32	pattrbeli0;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd1;		/* Pattern Match Data */
	u8	res28[4];
	u32	pmask1;		/* Pattern Mask */
	u8	res29[4];
	u32	pcntrl1;	/* Pattern Match Control */
	u8	res30[4];
	u32	pattrb1;	/* Pattern Match Attrs */
	u32	pattrbeli1;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd2;		/* Pattern Match Data */
	u8	res31[4];
	u32	pmask2;		/* Pattern Mask */
	u8	res32[4];
	u32	pcntrl2;	/* Pattern Match Control */
	u8	res33[4];
	u32	pattrb2;	/* Pattern Match Attrs */
	u32	pattrbeli2;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd3;		/* Pattern Match Data */
	u8	res34[4];
	u32	pmask3;		/* Pattern Mask */
	u8	res35[4];
	u32	pcntrl3;	/* Pattern Match Control */
	u8	res36[4];
	u32	pattrb3;	/* Pattern Match Attrs */
	u32	pattrbeli3;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd4;		/* Pattern Match Data */
	u8	res37[4];
	u32	pmask4;		/* Pattern Mask */
	u8	res38[4];
	u32	pcntrl4;	/* Pattern Match Control */
	u8	res39[4];
	u32	pattrb4;	/* Pattern Match Attrs */
	u32	pattrbeli4;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd5;		/* Pattern Match Data */
	u8	res40[4];
	u32	pmask5;		/* Pattern Mask */
	u8	res41[4];
	u32	pcntrl5;	/* Pattern Match Control */
	u8	res42[4];
	u32	pattrb5;	/* Pattern Match Attrs */
	u32	pattrbeli5;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd6;		/* Pattern Match Data */
	u8	res43[4];
	u32	pmask6;		/* Pattern Mask */
	u8	res44[4];
	u32	pcntrl6;	/* Pattern Match Control */
	u8	res45[4];
	u32	pattrb6;	/* Pattern Match Attrs */
	u32	pattrbeli6;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd7;		/* Pattern Match Data */
	u8	res46[4];
	u32	pmask7;		/* Pattern Mask */
	u8	res47[4];
	u32	pcntrl7;	/* Pattern Match Control */
	u8	res48[4];
	u32	pattrb7;	/* Pattern Match Attrs */
	u32	pattrbeli7;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd8;		/* Pattern Match Data */
	u8	res49[4];
	u32	pmask8;		/* Pattern Mask */
	u8	res50[4];
	u32	pcntrl8;	/* Pattern Match Control */
	u8	res51[4];
	u32	pattrb8;	/* Pattern Match Attrs */
	u32	pattrbeli8;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd9;		/* Pattern Match Data */
	u8	res52[4];
	u32	pmask9;		/* Pattern Mask */
	u8	res53[4];
	u32	pcntrl9;	/* Pattern Match Control */
	u8	res54[4];
	u32	pattrb9;	/* Pattern Match Attrs */
	u32	pattrbeli9;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd10;		/* Pattern Match Data */
	u8	res55[4];
	u32	pmask10;	/* Pattern Mask */
	u8	res56[4];
	u32	pcntrl10;	/* Pattern Match Control */
	u8	res57[4];
	u32	pattrb10;	/* Pattern Match Attrs */
	u32	pattrbeli10;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd11;		/* Pattern Match Data */
	u8	res58[4];
	u32	pmask11;	/* Pattern Mask */
	u8	res59[4];
	u32	pcntrl11;	/* Pattern Match Control */
	u8	res60[4];
	u32	pattrb11;	/* Pattern Match Attrs */
	u32	pattrbeli11;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd12;		/* Pattern Match Data */
	u8	res61[4];
	u32	pmask12;	/* Pattern Mask */
	u8	res62[4];
	u32	pcntrl12;	/* Pattern Match Control */
	u8	res63[4];
	u32	pattrb12;	/* Pattern Match Attrs */
	u32	pattrbeli12;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd13;		/* Pattern Match Data */
	u8	res64[4];
	u32	pmask13;	/* Pattern Mask */
	u8	res65[4];
	u32	pcntrl13;	/* Pattern Match Control */
	u8	res66[4];
	u32	pattrb13;	/* Pattern Match Attrs */
	u32	pattrbeli13;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd14;		/* Pattern Match Data */
	u8	res67[4];
	u32	pmask14;	/* Pattern Mask */
	u8	res68[4];
	u32	pcntrl14;	/* Pattern Match Control */
	u8	res69[4];
	u32	pattrb14;	/* Pattern Match Attrs */
	u32	pattrbeli14;	/* Pattern Match Attrs Extract Len & Idx */
	u32	pmd15;		/* Pattern Match Data */
	u8	res70[4];
	u32	pmask15;	/* Pattern Mask */
	u8	res71[4];
	u32	pcntrl15;	/* Pattern Match Control */
	u8	res72[4];
	u32	pattrb15;	/* Pattern Match Attrs */
	u32	pattrbeli15;	/* Pattern Match Attrs Extract Len & Idx */
	u8	res73[248];
	u32	attr;		/* Attrs */
	u32	attreli;	/* Attrs Extract Len & Idx */
	u8	res74[1024];
} ccsr_tsec_t;

/* PIC Registers */
typedef struct ccsr_pic {
	u8	res1[64];
	u32	ipidr0;		/* Interprocessor IRQ Dispatch 0 */
	u8	res2[12];
	u32	ipidr1;		/* Interprocessor IRQ Dispatch 1 */
	u8	res3[12];
	u32	ipidr2;		/* Interprocessor IRQ Dispatch 2 */
	u8	res4[12];
	u32	ipidr3;		/* Interprocessor IRQ Dispatch 3 */
	u8	res5[12];
	u32	ctpr;		/* Current Task Priority */
	u8	res6[12];
	u32	whoami;		/* Who Am I */
	u8	res7[12];
	u32	iack;		/* IRQ Acknowledge */
	u8	res8[12];
	u32	eoi;		/* End Of IRQ */
	u8	res9[3916];
	u32	frr;		/* Feature Reporting */
	u8	res10[28];
	u32	gcr;		/* Global Configuration */
#define MPC85xx_PICGCR_RST	0x80000000
#define MPC85xx_PICGCR_M	0x20000000
	u8	res11[92];
	u32	vir;		/* Vendor Identification */
	u8	res12[12];
	u32	pir;		/* Processor Initialization */
	u8	res13[12];
	u32	ipivpr0;	/* IPI Vector/Priority 0 */
	u8	res14[12];
	u32	ipivpr1;	/* IPI Vector/Priority 1 */
	u8	res15[12];
	u32	ipivpr2;	/* IPI Vector/Priority 2 */
	u8	res16[12];
	u32	ipivpr3;	/* IPI Vector/Priority 3 */
	u8	res17[12];
	u32	svr;		/* Spurious Vector */
	u8	res18[12];
	u32	tfrr;		/* Timer Frequency Reporting */
	u8	res19[12];
	u32	gtccr0;		/* Global Timer Current Count 0 */
	u8	res20[12];
	u32	gtbcr0;		/* Global Timer Base Count 0 */
	u8	res21[12];
	u32	gtvpr0;		/* Global Timer Vector/Priority 0 */
	u8	res22[12];
	u32	gtdr0;		/* Global Timer Destination 0 */
	u8	res23[12];
	u32	gtccr1;		/* Global Timer Current Count 1 */
	u8	res24[12];
	u32	gtbcr1;		/* Global Timer Base Count 1 */
	u8	res25[12];
	u32	gtvpr1;		/* Global Timer Vector/Priority 1 */
	u8	res26[12];
	u32	gtdr1;		/* Global Timer Destination 1 */
	u8	res27[12];
	u32	gtccr2;		/* Global Timer Current Count 2 */
	u8	res28[12];
	u32	gtbcr2;		/* Global Timer Base Count 2 */
	u8	res29[12];
	u32	gtvpr2;		/* Global Timer Vector/Priority 2 */
	u8	res30[12];
	u32	gtdr2;		/* Global Timer Destination 2 */
	u8	res31[12];
	u32	gtccr3;		/* Global Timer Current Count 3 */
	u8	res32[12];
	u32	gtbcr3;		/* Global Timer Base Count 3 */
	u8	res33[12];
	u32	gtvpr3;		/* Global Timer Vector/Priority 3 */
	u8	res34[12];
	u32	gtdr3;		/* Global Timer Destination 3 */
	u8	res35[268];
	u32	tcr;		/* Timer Control */
	u8	res36[12];
	u32	irqsr0;		/* IRQ_OUT Summary 0 */
	u8	res37[12];
	u32	irqsr1;		/* IRQ_OUT Summary 1 */
	u8	res38[12];
	u32	cisr0;		/* Critical IRQ Summary 0 */
	u8	res39[12];
	u32	cisr1;		/* Critical IRQ Summary 1 */
	u8	res40[188];
	u32	msgr0;		/* Message 0 */
	u8	res41[12];
	u32	msgr1;		/* Message 1 */
	u8	res42[12];
	u32	msgr2;		/* Message 2 */
	u8	res43[12];
	u32	msgr3;		/* Message 3 */
	u8	res44[204];
	u32	mer;		/* Message Enable */
	u8	res45[12];
	u32	msr;		/* Message Status */
	u8	res46[60140];
	u32	eivpr0;		/* External IRQ Vector/Priority 0 */
	u8	res47[12];
	u32	eidr0;		/* External IRQ Destination 0 */
	u8	res48[12];
	u32	eivpr1;		/* External IRQ Vector/Priority 1 */
	u8	res49[12];
	u32	eidr1;		/* External IRQ Destination 1 */
	u8	res50[12];
	u32	eivpr2;		/* External IRQ Vector/Priority 2 */
	u8	res51[12];
	u32	eidr2;		/* External IRQ Destination 2 */
	u8	res52[12];
	u32	eivpr3;		/* External IRQ Vector/Priority 3 */
	u8	res53[12];
	u32	eidr3;		/* External IRQ Destination 3 */
	u8	res54[12];
	u32	eivpr4;		/* External IRQ Vector/Priority 4 */
	u8	res55[12];
	u32	eidr4;		/* External IRQ Destination 4 */
	u8	res56[12];
	u32	eivpr5;		/* External IRQ Vector/Priority 5 */
	u8	res57[12];
	u32	eidr5;		/* External IRQ Destination 5 */
	u8	res58[12];
	u32	eivpr6;		/* External IRQ Vector/Priority 6 */
	u8	res59[12];
	u32	eidr6;		/* External IRQ Destination 6 */
	u8	res60[12];
	u32	eivpr7;		/* External IRQ Vector/Priority 7 */
	u8	res61[12];
	u32	eidr7;		/* External IRQ Destination 7 */
	u8	res62[12];
	u32	eivpr8;		/* External IRQ Vector/Priority 8 */
	u8	res63[12];
	u32	eidr8;		/* External IRQ Destination 8 */
	u8	res64[12];
	u32	eivpr9;		/* External IRQ Vector/Priority 9 */
	u8	res65[12];
	u32	eidr9;		/* External IRQ Destination 9 */
	u8	res66[12];
	u32	eivpr10;	/* External IRQ Vector/Priority 10 */
	u8	res67[12];
	u32	eidr10;		/* External IRQ Destination 10 */
	u8	res68[12];
	u32	eivpr11;	/* External IRQ Vector/Priority 11 */
	u8	res69[12];
	u32	eidr11;		/* External IRQ Destination 11 */
	u8	res70[140];
	u32	iivpr0;		/* Internal IRQ Vector/Priority 0 */
	u8	res71[12];
	u32	iidr0;		/* Internal IRQ Destination 0 */
	u8	res72[12];
	u32	iivpr1;		/* Internal IRQ Vector/Priority 1 */
	u8	res73[12];
	u32	iidr1;		/* Internal IRQ Destination 1 */
	u8	res74[12];
	u32	iivpr2;		/* Internal IRQ Vector/Priority 2 */
	u8	res75[12];
	u32	iidr2;		/* Internal IRQ Destination 2 */
	u8	res76[12];
	u32	iivpr3;		/* Internal IRQ Vector/Priority 3 */
	u8	res77[12];
	u32	iidr3;		/* Internal IRQ Destination 3 */
	u8	res78[12];
	u32	iivpr4;		/* Internal IRQ Vector/Priority 4 */
	u8	res79[12];
	u32	iidr4;		/* Internal IRQ Destination 4 */
	u8	res80[12];
	u32	iivpr5;		/* Internal IRQ Vector/Priority 5 */
	u8	res81[12];
	u32	iidr5;		/* Internal IRQ Destination 5 */
	u8	res82[12];
	u32	iivpr6;		/* Internal IRQ Vector/Priority 6 */
	u8	res83[12];
	u32	iidr6;		/* Internal IRQ Destination 6 */
	u8	res84[12];
	u32	iivpr7;		/* Internal IRQ Vector/Priority 7 */
	u8	res85[12];
	u32	iidr7;		/* Internal IRQ Destination 7 */
	u8	res86[12];
	u32	iivpr8;		/* Internal IRQ Vector/Priority 8 */
	u8	res87[12];
	u32	iidr8;		/* Internal IRQ Destination 8 */
	u8	res88[12];
	u32	iivpr9;		/* Internal IRQ Vector/Priority 9 */
	u8	res89[12];
	u32	iidr9;		/* Internal IRQ Destination 9 */
	u8	res90[12];
	u32	iivpr10;	/* Internal IRQ Vector/Priority 10 */
	u8	res91[12];
	u32	iidr10;		/* Internal IRQ Destination 10 */
	u8	res92[12];
	u32	iivpr11;	/* Internal IRQ Vector/Priority 11 */
	u8	res93[12];
	u32	iidr11;		/* Internal IRQ Destination 11 */
	u8	res94[12];
	u32	iivpr12;	/* Internal IRQ Vector/Priority 12 */
	u8	res95[12];
	u32	iidr12;		/* Internal IRQ Destination 12 */
	u8	res96[12];
	u32	iivpr13;	/* Internal IRQ Vector/Priority 13 */
	u8	res97[12];
	u32	iidr13;		/* Internal IRQ Destination 13 */
	u8	res98[12];
	u32	iivpr14;	/* Internal IRQ Vector/Priority 14 */
	u8	res99[12];
	u32	iidr14;		/* Internal IRQ Destination 14 */
	u8	res100[12];
	u32	iivpr15;	/* Internal IRQ Vector/Priority 15 */
	u8	res101[12];
	u32	iidr15;		/* Internal IRQ Destination 15 */
	u8	res102[12];
	u32	iivpr16;	/* Internal IRQ Vector/Priority 16 */
	u8	res103[12];
	u32	iidr16;		/* Internal IRQ Destination 16 */
	u8	res104[12];
	u32	iivpr17;	/* Internal IRQ Vector/Priority 17 */
	u8	res105[12];
	u32	iidr17;		/* Internal IRQ Destination 17 */
	u8	res106[12];
	u32	iivpr18;	/* Internal IRQ Vector/Priority 18 */
	u8	res107[12];
	u32	iidr18;		/* Internal IRQ Destination 18 */
	u8	res108[12];
	u32	iivpr19;	/* Internal IRQ Vector/Priority 19 */
	u8	res109[12];
	u32	iidr19;		/* Internal IRQ Destination 19 */
	u8	res110[12];
	u32	iivpr20;	/* Internal IRQ Vector/Priority 20 */
	u8	res111[12];
	u32	iidr20;		/* Internal IRQ Destination 20 */
	u8	res112[12];
	u32	iivpr21;	/* Internal IRQ Vector/Priority 21 */
	u8	res113[12];
	u32	iidr21;		/* Internal IRQ Destination 21 */
	u8	res114[12];
	u32	iivpr22;	/* Internal IRQ Vector/Priority 22 */
	u8	res115[12];
	u32	iidr22;		/* Internal IRQ Destination 22 */
	u8	res116[12];
	u32	iivpr23;	/* Internal IRQ Vector/Priority 23 */
	u8	res117[12];
	u32	iidr23;		/* Internal IRQ Destination 23 */
	u8	res118[12];
	u32	iivpr24;	/* Internal IRQ Vector/Priority 24 */
	u8	res119[12];
	u32	iidr24;		/* Internal IRQ Destination 24 */
	u8	res120[12];
	u32	iivpr25;	/* Internal IRQ Vector/Priority 25 */
	u8	res121[12];
	u32	iidr25;		/* Internal IRQ Destination 25 */
	u8	res122[12];
	u32	iivpr26;	/* Internal IRQ Vector/Priority 26 */
	u8	res123[12];
	u32	iidr26;		/* Internal IRQ Destination 26 */
	u8	res124[12];
	u32	iivpr27;	/* Internal IRQ Vector/Priority 27 */
	u8	res125[12];
	u32	iidr27;		/* Internal IRQ Destination 27 */
	u8	res126[12];
	u32	iivpr28;	/* Internal IRQ Vector/Priority 28 */
	u8	res127[12];
	u32	iidr28;		/* Internal IRQ Destination 28 */
	u8	res128[12];
	u32	iivpr29;	/* Internal IRQ Vector/Priority 29 */
	u8	res129[12];
	u32	iidr29;		/* Internal IRQ Destination 29 */
	u8	res130[12];
	u32	iivpr30;	/* Internal IRQ Vector/Priority 30 */
	u8	res131[12];
	u32	iidr30;		/* Internal IRQ Destination 30 */
	u8	res132[12];
	u32	iivpr31;	/* Internal IRQ Vector/Priority 31 */
	u8	res133[12];
	u32	iidr31;		/* Internal IRQ Destination 31 */
	u8	res134[4108];
	u32	mivpr0;		/* Messaging IRQ Vector/Priority 0 */
	u8	res135[12];
	u32	midr0;		/* Messaging IRQ Destination 0 */
	u8	res136[12];
	u32	mivpr1;		/* Messaging IRQ Vector/Priority 1 */
	u8	res137[12];
	u32	midr1;		/* Messaging IRQ Destination 1 */
	u8	res138[12];
	u32	mivpr2;		/* Messaging IRQ Vector/Priority 2 */
	u8	res139[12];
	u32	midr2;		/* Messaging IRQ Destination 2 */
	u8	res140[12];
	u32	mivpr3;		/* Messaging IRQ Vector/Priority 3 */
	u8	res141[12];
	u32	midr3;		/* Messaging IRQ Destination 3 */
	u8	res142[59852];
	u32	ipi0dr0;	/* Processor 0 Interprocessor IRQ Dispatch 0 */
	u8	res143[12];
	u32	ipi0dr1;	/* Processor 0 Interprocessor IRQ Dispatch 1 */
	u8	res144[12];
	u32	ipi0dr2;	/* Processor 0 Interprocessor IRQ Dispatch 2 */
	u8	res145[12];
	u32	ipi0dr3;	/* Processor 0 Interprocessor IRQ Dispatch 3 */
	u8	res146[12];
	u32	ctpr0;		/* Current Task Priority for Processor 0 */
	u8	res147[12];
	u32	whoami0;	/* Who Am I for Processor 0 */
	u8	res148[12];
	u32	iack0;		/* IRQ Acknowledge for Processor 0 */
	u8	res149[12];
	u32	eoi0;		/* End Of IRQ for Processor 0 */
	u8	res150[130892];
} ccsr_pic_t;

/* CPM Block */
#ifndef CONFIG_CPM2
typedef struct ccsr_cpm {
	u8 res[262144];
} ccsr_cpm_t;
#else
/*
 * DPARM
 * General SIU
 */
typedef struct ccsr_cpm_siu {
	u8	res1[80];
	u32	smaer;
	u32	smser;
	u32	smevr;
	u8	res2[4];
	u32	lmaer;
	u32	lmser;
	u32	lmevr;
	u8	res3[2964];
} ccsr_cpm_siu_t;

/* IRQ Controller */
typedef struct ccsr_cpm_intctl {
	u16	sicr;
	u8	res1[2];
	u32	sivec;
	u32	sipnrh;
	u32	sipnrl;
	u32	siprr;
	u32	scprrh;
	u32	scprrl;
	u32	simrh;
	u32	simrl;
	u32	siexr;
	u8	res2[88];
	u32	sccr;
	u8	res3[124];
} ccsr_cpm_intctl_t;

/* input/output port */
typedef struct ccsr_cpm_iop {
	u32	pdira;
	u32	ppara;
	u32	psora;
	u32	podra;
	u32	pdata;
	u8	res1[12];
	u32	pdirb;
	u32	pparb;
	u32	psorb;
	u32	podrb;
	u32	pdatb;
	u8	res2[12];
	u32	pdirc;
	u32	pparc;
	u32	psorc;
	u32	podrc;
	u32	pdatc;
	u8	res3[12];
	u32	pdird;
	u32	ppard;
	u32	psord;
	u32	podrd;
	u32	pdatd;
	u8	res4[12];
} ccsr_cpm_iop_t;

/* CPM timers */
typedef struct ccsr_cpm_timer {
	u8	tgcr1;
	u8	res1[3];
	u8	tgcr2;
	u8	res2[11];
	u16	tmr1;
	u16	tmr2;
	u16	trr1;
	u16	trr2;
	u16	tcr1;
	u16	tcr2;
	u16	tcn1;
	u16	tcn2;
	u16	tmr3;
	u16	tmr4;
	u16	trr3;
	u16	trr4;
	u16	tcr3;
	u16	tcr4;
	u16	tcn3;
	u16	tcn4;
	u16	ter1;
	u16	ter2;
	u16	ter3;
	u16	ter4;
	u8	res3[608];
} ccsr_cpm_timer_t;

/* SDMA */
typedef struct ccsr_cpm_sdma {
	u8	sdsr;
	u8	res1[3];
	u8	sdmr;
	u8	res2[739];
} ccsr_cpm_sdma_t;

/* FCC1 */
typedef struct ccsr_cpm_fcc1 {
	u32	gfmr;
	u32	fpsmr;
	u16	ftodr;
	u8	res1[2];
	u16	fdsr;
	u8	res2[2];
	u16	fcce;
	u8	res3[2];
	u16	fccm;
	u8	res4[2];
	u8	fccs;
	u8	res5[3];
	u8	ftirr_phy[4];
} ccsr_cpm_fcc1_t;

/* FCC2 */
typedef struct ccsr_cpm_fcc2 {
	u32	gfmr;
	u32	fpsmr;
	u16	ftodr;
	u8	res1[2];
	u16	fdsr;
	u8	res2[2];
	u16	fcce;
	u8	res3[2];
	u16	fccm;
	u8	res4[2];
	u8	fccs;
	u8	res5[3];
	u8	ftirr_phy[4];
} ccsr_cpm_fcc2_t;

/* FCC3 */
typedef struct ccsr_cpm_fcc3 {
	u32	gfmr;
	u32	fpsmr;
	u16	ftodr;
	u8	res1[2];
	u16	fdsr;
	u8	res2[2];
	u16	fcce;
	u8	res3[2];
	u16	fccm;
	u8	res4[2];
	u8	fccs;
	u8	res5[3];
	u8	res[36];
} ccsr_cpm_fcc3_t;

/* FCC1 extended */
typedef struct ccsr_cpm_fcc1_ext {
	u32	firper;
	u32	firer;
	u32	firsr_h;
	u32	firsr_l;
	u8	gfemr;
	u8	res[15];

} ccsr_cpm_fcc1_ext_t;

/* FCC2 extended */
typedef struct ccsr_cpm_fcc2_ext {
	u32	firper;
	u32	firer;
	u32	firsr_h;
	u32	firsr_l;
	u8	gfemr;
	u8	res[31];
} ccsr_cpm_fcc2_ext_t;

/* FCC3 extended */
typedef struct ccsr_cpm_fcc3_ext {
	u8	gfemr;
	u8	res[47];
} ccsr_cpm_fcc3_ext_t;

/* TC layers */
typedef struct ccsr_cpm_tmp1 {
	u8	res[496];
} ccsr_cpm_tmp1_t;

/* BRGs:5,6,7,8 */
typedef struct ccsr_cpm_brg2 {
	u32	brgc5;
	u32	brgc6;
	u32	brgc7;
	u32	brgc8;
	u8	res[608];
} ccsr_cpm_brg2_t;

/* I2C */
typedef struct ccsr_cpm_i2c {
	u8	i2mod;
	u8	res1[3];
	u8	i2add;
	u8	res2[3];
	u8	i2brg;
	u8	res3[3];
	u8	i2com;
	u8	res4[3];
	u8	i2cer;
	u8	res5[3];
	u8	i2cmr;
	u8	res6[331];
} ccsr_cpm_i2c_t;

/* CPM core */
typedef struct ccsr_cpm_cp {
	u32	cpcr;
	u32	rccr;
	u8	res1[14];
	u16	rter;
	u8	res2[2];
	u16	rtmr;
	u16	rtscr;
	u8	res3[2];
	u32	rtsr;
	u8	res4[12];
} ccsr_cpm_cp_t;

/* BRGs:1,2,3,4 */
typedef struct ccsr_cpm_brg1 {
	u32	brgc1;
	u32	brgc2;
	u32	brgc3;
	u32	brgc4;
} ccsr_cpm_brg1_t;

/* SCC1-SCC4 */
typedef struct ccsr_cpm_scc {
	u32	gsmrl;
	u32	gsmrh;
	u16	psmr;
	u8	res1[2];
	u16	todr;
	u16	dsr;
	u16	scce;
	u8	res2[2];
	u16	sccm;
	u8	res3;
	u8	sccs;
	u8	res4[8];
} ccsr_cpm_scc_t;

typedef struct ccsr_cpm_tmp2 {
	u8	res[32];
} ccsr_cpm_tmp2_t;

/* SPI */
typedef struct ccsr_cpm_spi {
	u16	spmode;
	u8	res1[4];
	u8	spie;
	u8	res2[3];
	u8	spim;
	u8	res3[2];
	u8	spcom;
	u8	res4[82];
} ccsr_cpm_spi_t;

/* CPM MUX */
typedef struct ccsr_cpm_mux {
	u8	cmxsi1cr;
	u8	res1;
	u8	cmxsi2cr;
	u8	res2;
	u32	cmxfcr;
	u32	cmxscr;
	u8	res3[2];
	u16	cmxuar;
	u8	res4[16];
} ccsr_cpm_mux_t;

/* SI,MCC,etc */
typedef struct ccsr_cpm_tmp3 {
	u8 res[58592];
} ccsr_cpm_tmp3_t;

typedef struct ccsr_cpm_iram {
	u32	iram[8192];
	u8	res[98304];
} ccsr_cpm_iram_t;

typedef struct ccsr_cpm {
	/* Some references are into the unique & known dpram spaces,
	 * others are from the generic base.
	 */
#define im_dprambase		im_dpram1
	u8			im_dpram1[16*1024];
	u8			res1[16*1024];
	u8			im_dpram2[16*1024];
	u8			res2[16*1024];
	ccsr_cpm_siu_t		im_cpm_siu; /* SIU Configuration */
	ccsr_cpm_intctl_t	im_cpm_intctl; /* IRQ Controller */
	ccsr_cpm_iop_t		im_cpm_iop; /* IO Port control/status */
	ccsr_cpm_timer_t	im_cpm_timer; /* CPM timers */
	ccsr_cpm_sdma_t		im_cpm_sdma; /* SDMA control/status */
	ccsr_cpm_fcc1_t		im_cpm_fcc1;
	ccsr_cpm_fcc2_t		im_cpm_fcc2;
	ccsr_cpm_fcc3_t		im_cpm_fcc3;
	ccsr_cpm_fcc1_ext_t	im_cpm_fcc1_ext;
	ccsr_cpm_fcc2_ext_t	im_cpm_fcc2_ext;
	ccsr_cpm_fcc3_ext_t	im_cpm_fcc3_ext;
	ccsr_cpm_tmp1_t		im_cpm_tmp1;
	ccsr_cpm_brg2_t		im_cpm_brg2;
	ccsr_cpm_i2c_t		im_cpm_i2c;
	ccsr_cpm_cp_t		im_cpm_cp;
	ccsr_cpm_brg1_t		im_cpm_brg1;
	ccsr_cpm_scc_t		im_cpm_scc[4];
	ccsr_cpm_tmp2_t		im_cpm_tmp2;
	ccsr_cpm_spi_t		im_cpm_spi;
	ccsr_cpm_mux_t		im_cpm_mux;
	ccsr_cpm_tmp3_t		im_cpm_tmp3;
	ccsr_cpm_iram_t		im_cpm_iram;
} ccsr_cpm_t;
#endif

#ifdef CONFIG_SYS_SRIO
/* Architectural regsiters */
struct rio_arch {
	u32	didcar;	/* Device Identity CAR */
	u32	dicar;	/* Device Information CAR */
	u32	aidcar;	/* Assembly Identity CAR */
	u32	aicar;	/* Assembly Information CAR */
	u32	pefcar;	/* Processing Element Features CAR */
	u8	res0[4];
	u32	socar;	/* Source Operations CAR */
	u32	docar;	/* Destination Operations CAR */
	u8	res1[32];
	u32	mcsr;	/* Mailbox CSR */
	u32	pwdcsr;	/* Port-Write and Doorbell CSR */
	u8	res2[4];
	u32	pellccsr;	/* Processing Element Logic Layer CCSR */
	u8	res3[12];
	u32	lcsbacsr;	/* Local Configuration Space BACSR */
	u32	bdidcsr;	/* Base Device ID CSR */
	u8	res4[4];
	u32	hbdidlcsr;	/* Host Base Device ID Lock CSR */
	u32	ctcsr;	/* Component Tag CSR */
};

/* Extended Features Space: 1x/4x LP-Serial Port registers */
struct rio_lp_serial_port {
	u32	plmreqcsr;	/* Port Link Maintenance Request CSR */
	u32	plmrespcsr;	/* Port Link Maintenance Response CS */
	u32	plascsr;	/* Port Local Ackid Status CSR */
	u8	res0[12];
	u32	pescsr;	/* Port Error and Status CSR */
	u32	pccsr;	/* Port Control CSR */
};

/* Extended Features Space: 1x/4x LP-Serial registers */
struct rio_lp_serial {
	u32	pmbh0csr;	/* Port Maintenance Block Header 0 CSR */
	u8	res0[28];
	u32	pltoccsr;	/* Port Link Time-out CCSR */
	u32	prtoccsr;	/* Port Response Time-out CCSR */
	u8	res1[20];
	u32	pgccsr;	/* Port General CSR */
	struct rio_lp_serial_port	port[CONFIG_SYS_FSL_SRIO_MAX_PORTS];
};

/* Logical error reporting registers */
struct rio_logical_err {
	u32	erbh;	/* Error Reporting Block Header Register */
	u8	res0[4];
	u32	ltledcsr;	/* Logical/Transport layer error DCSR */
	u32	ltleecsr;	/* Logical/Transport layer error ECSR */
	u8	res1[4];
	u32	ltlaccsr;	/* Logical/Transport layer ACCSR */
	u32	ltldidccsr;	/* Logical/Transport layer DID CCSR */
	u32	ltlcccsr;	/* Logical/Transport layer control CCSR */
};

/* Physical error reporting port registers */
struct rio_phys_err_port {
	u32	edcsr;	/* Port error detect CSR */
	u32	erecsr;	/* Port error rate enable CSR */
	u32	ecacsr;	/* Port error capture attributes CSR */
	u32	pcseccsr0;	/* Port packet/control symbol ECCSR 0 */
	u32	peccsr[3];	/* Port error capture CSR */
	u8	res0[12];
	u32	ercsr;	/* Port error rate CSR */
	u32	ertcsr;	/* Port error rate threshold CSR */
	u8	res1[16];
};

/* Physical error reporting registers */
struct rio_phys_err {
	struct rio_phys_err_port	port[CONFIG_SYS_FSL_SRIO_MAX_PORTS];
};

/* Implementation Space: General Port-Common */
struct rio_impl_common {
	u8	res0[4];
	u32	llcr;	/* Logical Layer Configuration Register */
	u8	res1[8];
	u32	epwisr;	/* Error / Port-Write Interrupt SR */
	u8	res2[12];
	u32	lretcr;	/* Logical Retry Error Threshold CR */
	u8	res3[92];
	u32	pretcr;	/* Physical Retry Erorr Threshold CR */
	u8	res4[124];
};

/* Implementation Space: Port Specific */
struct rio_impl_port_spec {
	u32	adidcsr;	/* Port Alt. Device ID CSR */
	u8	res0[28];
	u32	ptaacr;	/* Port Pass-Through/Accept-All CR */
	u32	lopttlcr;
	u8	res1[8];
	u32	iecsr;	/* Port Implementation Error CSR */
	u8	res2[12];
	u32	pcr;		/* Port Phsyical Configuration Register */
	u8	res3[20];
	u32	slcsr;	/* Port Serial Link CSR */
	u8	res4[4];
	u32	sleicr;	/* Port Serial Link Error Injection */
	u32	a0txcr;	/* Port Arbitration 0 Tx CR */
	u32	a1txcr;	/* Port Arbitration 1 Tx CR */
	u32	a2txcr;	/* Port Arbitration 2 Tx CR */
	u32	mreqtxbacr[3];	/* Port Request Tx Buffer ACR */
	u32	mrspfctxbacr;	/* Port Response/Flow Control Tx Buffer ACR */
};

/* Implementation Space: register */
struct rio_implement {
	struct rio_impl_common	com;
	struct rio_impl_port_spec	port[CONFIG_SYS_FSL_SRIO_MAX_PORTS];
};

/* Revision Control Register */
struct rio_rev_ctrl {
	u32	ipbrr[2];	/* IP Block Revision Register */
};

struct rio_atmu_row {
	u32	rowtar; /* RapidIO Outbound Window TAR */
	u32	rowtear; /* RapidIO Outbound Window TEAR */
	u32	rowbar;
	u8	res0[4];
	u32	rowar; /* RapidIO Outbound Attributes Register */
	u32	rowsr[3]; /* Port RapidIO outbound window segment register */
};

struct rio_atmu_riw {
	u32	riwtar; /* RapidIO Inbound Window Translation AR */
	u8	res0[4];
	u32	riwbar; /* RapidIO Inbound Window Base AR */
	u8	res1[4];
	u32	riwar; /* RapidIO Inbound Attributes Register */
	u8	res2[12];
};

/* ATMU window registers */
struct rio_atmu_win {
	struct rio_atmu_row	outbw[CONFIG_SYS_FSL_SRIO_OB_WIN_NUM];
	u8	res0[64];
	struct rio_atmu_riw	inbw[CONFIG_SYS_FSL_SRIO_IB_WIN_NUM];
};

struct rio_atmu {
	struct rio_atmu_win	port[CONFIG_SYS_FSL_SRIO_MAX_PORTS];
};

#ifdef CONFIG_SYS_FSL_RMU
struct rio_msg {
	u32	omr; /* Outbound Mode Register */
	u32	osr; /* Outbound Status Register */
	u32	eodqdpar; /* Extended Outbound DQ DPAR */
	u32	odqdpar; /* Outbound Descriptor Queue DPAR */
	u32	eosar; /* Extended Outbound Unit Source AR */
	u32	osar; /* Outbound Unit Source AR */
	u32	odpr; /* Outbound Destination Port Register */
	u32	odatr; /* Outbound Destination Attributes Register */
	u32	odcr; /* Outbound Doubleword Count Register */
	u32	eodqepar; /* Extended Outbound DQ EPAR */
	u32	odqepar; /* Outbound Descriptor Queue EPAR */
	u32	oretr; /* Outbound Retry Error Threshold Register */
	u32	omgr; /* Outbound Multicast Group Register */
	u32	omlr; /* Outbound Multicast List Register */
	u8	res0[40];
	u32	imr;	 /* Outbound Mode Register */
	u32	isr; /* Inbound Status Register */
	u32	eidqdpar; /* Extended Inbound Descriptor Queue DPAR */
	u32	idqdpar; /* Inbound Descriptor Queue DPAR */
	u32	eifqepar; /* Extended Inbound Frame Queue EPAR */
	u32	ifqepar; /* Inbound Frame Queue EPAR */
	u32	imirir; /* Inbound Maximum Interrutp RIR */
	u8	res1[4];
	u32 eihqepar; /* Extended inbound message header queue EPAR */
	u32 ihqepar; /* Inbound message header queue EPAR */
	u8	res2[120];
};

struct rio_dbell {
	u32	odmr; /* Outbound Doorbell Mode Register */
	u32	odsr; /* Outbound Doorbell Status Register */
	u8	res0[16];
	u32	oddpr; /* Outbound Doorbell Destination Port */
	u32	oddatr; /* Outbound Doorbell Destination AR */
	u8	res1[12];
	u32	oddretr; /* Outbound Doorbell Retry Threshold CR */
	u8	res2[48];
	u32	idmr; /* Inbound Doorbell Mode Register */
	u32	idsr;	 /* Inbound Doorbell Status Register */
	u32	iedqdpar; /* Extended Inbound Doorbell Queue DPAR */
	u32	iqdpar; /* Inbound Doorbell Queue DPAR */
	u32	iedqepar; /* Extended Inbound Doorbell Queue EPAR */
	u32	idqepar; /* Inbound Doorbell Queue EPAR */
	u32	idmirir; /* Inbound Doorbell Max Interrupt RIR */
};

struct rio_pw {
	u32	pwmr; /* Port-Write Mode Register */
	u32	pwsr; /* Port-Write Status Register */
	u32	epwqbar; /* Extended Port-Write Queue BAR */
	u32	pwqbar; /* Port-Write Queue Base Address Register */
};
#endif

/* RapidIO Registers */
struct ccsr_rio {
	struct rio_arch	arch;
	u8	res0[144];
	struct rio_lp_serial	lp_serial;
	u8	res1[1152];
	struct rio_logical_err	logical_err;
	u8	res2[32];
	struct rio_phys_err	phys_err;
	u8	res3[63808];
	struct rio_implement	impl;
	u8	res4[2552];
	struct rio_rev_ctrl	rev;
	struct rio_atmu	atmu;
#ifdef CONFIG_SYS_FSL_RMU
	u8	res5[8192];
	struct rio_msg	msg[CONFIG_SYS_FSL_SRIO_MSG_UNIT_NUM];
	u8	res6[512];
	struct rio_dbell	dbell;
	u8	res7[100];
	struct rio_pw	pw;
#endif
};
#endif

/* Quick Engine Block Pin Muxing Registers */
typedef struct par_io {
	u32	cpodr;
	u32	cpdat;
	u32	cpdir1;
	u32	cpdir2;
	u32	cppar1;
	u32	cppar2;
	u8	res[8];
} par_io_t;

#ifdef CONFIG_SYS_FSL_CPC
/*
 * Define a single offset that is the start of all the CPC register
 * blocks - if there is more than one CPC, we expect these to be
 * contiguous 4k regions
 */

typedef struct cpc_corenet {
	u32 	cpccsr0;	/* Config/status reg */
	u32	res1;
	u32	cpccfg0;	/* Configuration register */
	u32	res2;
	u32	cpcewcr0;	/* External Write reg 0 */
	u32	cpcewabr0;	/* External write base reg 0 */
	u32	res3[2];
	u32	cpcewcr1;	/* External Write reg 1 */
	u32	cpcewabr1;	/* External write base reg 1 */
	u32	res4[54];
	u32	cpcsrcr1;	/* SRAM control reg 1 */
	u32	cpcsrcr0;	/* SRAM control reg 0 */
	u32	res5[62];
	struct {
		u32	id;	/* partition ID */
		u32	res;
		u32	alloc;	/* partition allocation */
		u32	way;	/* partition way */
	} partition_regs[16];
	u32	res6[704];
	u32	cpcerrinjhi;	/* Error injection high */
	u32	cpcerrinjlo;	/* Error injection lo */
	u32	cpcerrinjctl;	/* Error injection control */
	u32	res7[5];
	u32	cpccaptdatahi;	/* capture data high */
	u32	cpccaptdatalo;	/* capture data low */
	u32	cpcaptecc;	/* capture ECC */
	u32	res8[5];
	u32	cpcerrdet;	/* error detect */
	u32	cpcerrdis;	/* error disable */
	u32	cpcerrinten;	/* errir interrupt enable */
	u32	cpcerrattr;	/* error attribute */
	u32	cpcerreaddr;	/* error extended address */
	u32	cpcerraddr;	/* error address */
	u32	cpcerrctl;	/* error control */
	u32	res9[41];	/* pad out to 4k */
	u32	cpchdbcr0;	/* hardware debug control register 0 */
	u32	res10[63];	/* pad out to 4k */
} cpc_corenet_t;

#define CPC_CSR0_CE	0x80000000	/* Cache Enable */
#define CPC_CSR0_PE	0x40000000	/* Enable ECC */
#define CPC_CSR0_FI	0x00200000	/* Cache Flash Invalidate */
#define CPC_CSR0_WT	0x00080000	/* Write-through mode */
#define CPC_CSR0_FL	0x00000800	/* Hardware cache flush */
#define CPC_CSR0_LFC	0x00000400	/* Cache Lock Flash Clear */
#define CPC_CFG0_SZ_MASK	0x00003fff
#define CPC_CFG0_SZ_K(x)	((x & CPC_CFG0_SZ_MASK) << 6)
#define CPC_CFG0_NUM_WAYS(x)	(((x >> 14) & 0x1f) + 1)
#define CPC_CFG0_LINE_SZ(x)	((((x >> 23) & 0x3) + 1) * 32)
#define CPC_SRCR1_SRBARU_MASK	0x0000ffff
#define CPC_SRCR1_SRBARU(x)	(((unsigned long long)x >> 32) \
				 & CPC_SRCR1_SRBARU_MASK)
#define	CPC_SRCR0_SRBARL_MASK	0xffff8000
#define CPC_SRCR0_SRBARL(x)	(x & CPC_SRCR0_SRBARL_MASK)
#define CPC_SRCR0_INTLVEN	0x00000100
#define CPC_SRCR0_SRAMSZ_1_WAY	0x00000000
#define CPC_SRCR0_SRAMSZ_2_WAY	0x00000002
#define CPC_SRCR0_SRAMSZ_4_WAY	0x00000004
#define CPC_SRCR0_SRAMSZ_8_WAY	0x00000006
#define CPC_SRCR0_SRAMSZ_16_WAY	0x00000008
#define CPC_SRCR0_SRAMSZ_32_WAY	0x0000000a
#define CPC_SRCR0_SRAMEN	0x00000001
#define	CPC_ERRDIS_TMHITDIS  	0x00000080	/* multi-way hit disable */
#define CPC_HDBCR0_CDQ_SPEC_DIS	0x08000000
#define CPC_HDBCR0_TAG_ECC_SCRUB_DIS	0x01000000
#define CPC_HDBCR0_DATA_ECC_SCRUB_DIS	0x00400000
#endif /* CONFIG_SYS_FSL_CPC */

/* Global Utilities Block */
#ifdef CONFIG_FSL_CORENET
typedef struct ccsr_gur {
	u32	porsr1;		/* POR status */
	u8	res1[28];
	u32	gpporcr1;	/* General-purpose POR configuration */
	u8	res2[12];
	u32	gpiocr;		/* GPIO control */
	u8	res3[12];
	u32	gpoutdr;	/* General-purpose output data */
	u8	res4[12];
	u32	gpindr;		/* General-purpose input data */
	u8	res5[12];
	u32	alt_pmuxcr;	/* Alt function signal multiplex control */
	u8	res6[12];
	u32	devdisr;	/* Device disable control */
#define FSL_CORENET_DEVDISR_PCIE1	0x80000000
#define FSL_CORENET_DEVDISR_PCIE2	0x40000000
#define FSL_CORENET_DEVDISR_PCIE3	0x20000000
#define FSL_CORENET_DEVDISR_PCIE4	0x10000000
#define FSL_CORENET_DEVDISR_RMU		0x08000000
#define FSL_CORENET_DEVDISR_SRIO1	0x04000000
#define FSL_CORENET_DEVDISR_SRIO2	0x02000000
#define FSL_CORENET_DEVDISR_DMA1	0x00400000
#define FSL_CORENET_DEVDISR_DMA2	0x00200000
#define FSL_CORENET_DEVDISR_DDR1	0x00100000
#define FSL_CORENET_DEVDISR_DDR2	0x00080000
#define FSL_CORENET_DEVDISR_DBG		0x00010000
#define FSL_CORENET_DEVDISR_NAL		0x00008000
#define FSL_CORENET_DEVDISR_SATA1	0x00004000
#define FSL_CORENET_DEVDISR_SATA2	0x00002000
#define FSL_CORENET_DEVDISR_ELBC	0x00001000
#define FSL_CORENET_DEVDISR_USB1	0x00000800
#define FSL_CORENET_DEVDISR_USB2	0x00000400
#define FSL_CORENET_DEVDISR_ESDHC	0x00000100
#define FSL_CORENET_DEVDISR_GPIO	0x00000080
#define FSL_CORENET_DEVDISR_ESPI	0x00000040
#define FSL_CORENET_DEVDISR_I2C1	0x00000020
#define FSL_CORENET_DEVDISR_I2C2	0x00000010
#define FSL_CORENET_DEVDISR_DUART1	0x00000002
#define FSL_CORENET_DEVDISR_DUART2	0x00000001
	u32	devdisr2;	/* Device disable control 2 */
#define FSL_CORENET_DEVDISR2_PME	0x80000000
#define FSL_CORENET_DEVDISR2_SEC	0x40000000
#define FSL_CORENET_DEVDISR2_QMBM	0x08000000
#define FSL_CORENET_DEVDISR2_FM1	0x02000000
#define FSL_CORENET_DEVDISR2_10GEC1	0x01000000
#define FSL_CORENET_DEVDISR2_DTSEC1_1	0x00800000
#define FSL_CORENET_DEVDISR2_DTSEC1_2	0x00400000
#define FSL_CORENET_DEVDISR2_DTSEC1_3	0x00200000
#define FSL_CORENET_DEVDISR2_DTSEC1_4	0x00100000
#define FSL_CORENET_DEVDISR2_DTSEC1_5	0x00080000
#define FSL_CORENET_DEVDISR2_FM2	0x00020000
#define FSL_CORENET_DEVDISR2_10GEC2	0x00010000
#define FSL_CORENET_DEVDISR2_DTSEC2_1	0x00008000
#define FSL_CORENET_DEVDISR2_DTSEC2_2	0x00004000
#define FSL_CORENET_DEVDISR2_DTSEC2_3	0x00002000
#define FSL_CORENET_DEVDISR2_DTSEC2_4	0x00001000
#define FSL_CORENET_NUM_DEVDISR		2
	u8	res7[8];
	u32	powmgtcsr;	/* Power management status & control */
	u8	res8[12];
	u32	coredisru;	/* uppper portion for support of 64 cores */
	u32	coredisrl;	/* lower portion for support of 64 cores */
	u8	res9[8];
	u32	pvr;		/* Processor version */
	u32	svr;		/* System version */
	u8	res10[8];
	u32	rstcr;		/* Reset control */
	u32	rstrqpblsr;	/* Reset request preboot loader status */
	u8	res11[8];
	u32	rstrqmr1;	/* Reset request mask */
	u8	res12[4];
	u32	rstrqsr1;	/* Reset request status */
	u8	res13[4];
	u8	res14[4];
	u32	rstrqwdtmrl;	/* Reset request WDT mask */
	u8	res15[4];
	u32	rstrqwdtsrl;	/* Reset request WDT status */
	u8	res16[4];
	u32	brrl;		/* Boot release */
	u8	res17[24];
	u32	rcwsr[16];	/* Reset control word status */
#define FSL_CORENET_RCWSR4_SRDS_PRTCL		0xfc000000
#define FSL_CORENET_RCWSR5_DDR_SYNC		0x00000080
#define FSL_CORENET_RCWSR5_DDR_SYNC_SHIFT		 7
#define FSL_CORENET_RCWSR5_SRDS_EN		0x00002000
#define FSL_CORENET_RCWSRn_SRDS_LPD_B2		0x3c000000 /* bits 162..165 */
#define FSL_CORENET_RCWSRn_SRDS_LPD_B3		0x003c0000 /* bits 170..173 */
#define FSL_CORENET_RCWSR7_MCK_TO_PLAT_RAT	0x00400000
#define FSL_CORENET_RCWSR8_HOST_AGT_B1		0x00e00000
#define FSL_CORENET_RCWSR8_HOST_AGT_B2		0x00100000
#define FSL_CORENET_RCWSR11_EC1			0x00c00000 /* bits 360..361 */
#if defined(CONFIG_PPC_P4080) || defined(CONFIG_PPC_P3060)
#define FSL_CORENET_RCWSR11_EC1_FM1_DTSEC1		0x00000000
#define FSL_CORENET_RCWSR11_EC1_FM1_USB1		0x00800000
#define FSL_CORENET_RCWSR11_EC2			0x001c0000 /* bits 363..365 */
#define FSL_CORENET_RCWSR11_EC2_FM2_DTSEC1		0x00000000
#define FSL_CORENET_RCWSR11_EC2_FM1_DTSEC2		0x00080000
#define FSL_CORENET_RCWSR11_EC2_USB2			0x00100000
#endif
#if defined(CONFIG_PPC_P2040) || defined(CONFIG_PPC_P2041) \
	|| defined(CONFIG_PPC_P3041) || defined(CONFIG_PPC_P5020)
#define FSL_CORENET_RCWSR11_EC1_FM1_DTSEC4_RGMII	0x00000000
#define FSL_CORENET_RCWSR11_EC1_FM1_DTSEC4_MII		0x00800000
#define FSL_CORENET_RCWSR11_EC1_FM1_DTSEC4_NONE		0x00c00000
#define FSL_CORENET_RCWSR11_EC2			0x00180000 /* bits 363..364 */
#define FSL_CORENET_RCWSR11_EC2_FM1_DTSEC5_RGMII	0x00000000
#define FSL_CORENET_RCWSR11_EC2_FM1_DTSEC5_MII		0x00100000
#define FSL_CORENET_RCWSR11_EC2_FM1_DTSEC5_NONE		0x00180000
#endif
	u8	res18[192];
	u32	scratchrw[4];	/* Scratch Read/Write */
	u8	res19[240];
	u32	scratchw1r[4];	/* Scratch Read (Write once) */
	u8	res20[240];
	u32	scrtsr[8];	/* Core reset status */
	u8	res21[224];
	u32	pex1liodnr;	/* PCI Express 1 LIODN */
	u32	pex2liodnr;	/* PCI Express 2 LIODN */
	u32	pex3liodnr;	/* PCI Express 3 LIODN */
	u32	pex4liodnr;	/* PCI Express 4 LIODN */
	u32	rio1liodnr;	/* RIO 1 LIODN */
	u32	rio2liodnr;	/* RIO 2 LIODN */
	u32	rio3liodnr;	/* RIO 3 LIODN */
	u32	rio4liodnr;	/* RIO 4 LIODN */
	u32	usb1liodnr;	/* USB 1 LIODN */
	u32	usb2liodnr;	/* USB 2 LIODN */
	u32	usb3liodnr;	/* USB 3 LIODN */
	u32	usb4liodnr;	/* USB 4 LIODN */
	u32	sdmmc1liodnr;	/* SD/MMC 1 LIODN */
	u32	sdmmc2liodnr;	/* SD/MMC 2 LIODN */
	u32	sdmmc3liodnr;	/* SD/MMC 3 LIODN */
	u32	sdmmc4liodnr;	/* SD/MMC 4 LIODN */
	u32	rio1maintliodnr;/* RIO 1 Maintenance LIODN */
	u32	rio2maintliodnr;/* RIO 2 Maintenance LIODN */
	u32	rio3maintliodnr;/* RIO 3 Maintenance LIODN */
	u32	rio4maintliodnr;/* RIO 4 Maintenance LIODN */
	u32	sata1liodnr;	/* SATA 1 LIODN */
	u32	sata2liodnr;	/* SATA 2 LIODN */
	u32	sata3liodnr;	/* SATA 3 LIODN */
	u32	sata4liodnr;	/* SATA 4 LIODN */
	u8	res22[32];
	u32	dma1liodnr;	/* DMA 1 LIODN */
	u32	dma2liodnr;	/* DMA 2 LIODN */
	u32	dma3liodnr;	/* DMA 3 LIODN */
	u32	dma4liodnr;	/* DMA 4 LIODN */
	u8	res23[48];
	u8	res24[64];
	u32	pblsr;		/* Preboot loader status */
	u32	pamubypenr;	/* PAMU bypass enable */
	u32	dmacr1;		/* DMA control */
	u8	res25[4];
	u32	gensr1;		/* General status */
	u8	res26[12];
	u32	gencr1;		/* General control */
	u8	res27[12];
	u8	res28[4];
	u32	cgensrl;	/* Core general status */
	u8	res29[8];
	u8	res30[4];
	u32	cgencrl;	/* Core general control */
	u8	res31[184];
	u32	sriopstecr;	/* SRIO prescaler timer enable control */
	u32	dcsrcr;		/* DCSR Control register */
	u8	res32[1784];
	u32	pmuxcr;		/* Pin multiplexing control */
	u8	res33[60];
	u32	iovselsr;	/* I/O voltage selection status */
	u8	res34[28];
	u32	ddrclkdr;	/* DDR clock disable */
	u8	res35;
	u32	elbcclkdr;	/* eLBC clock disable */
	u8	res36[20];
	u32	sdhcpcr;	/* eSDHC polarity configuration */
	u8	res37[380];
} ccsr_gur_t;

#define FSL_CORENET_DCSR_SZ_MASK	0x00000003
#define FSL_CORENET_DCSR_SZ_4M		0x0
#define FSL_CORENET_DCSR_SZ_1G		0x3

/*
 * On p4080 we have an LIODN for msg unit (rmu) but not maintenance
 * everything after has RMan thus msg unit LIODN is used for maintenance
 */
#define rmuliodnr rio1maintliodnr

typedef struct ccsr_clk {
	u32	clkc0csr;	/* Core 0 Clock control/status */
	u8	res1[0x1c];
	u32	clkc1csr;	/* Core 1 Clock control/status */
	u8	res2[0x1c];
	u32	clkc2csr;	/* Core 2 Clock control/status */
	u8	res3[0x1c];
	u32	clkc3csr;	/* Core 3 Clock control/status */
	u8	res4[0x1c];
	u32	clkc4csr;	/* Core 4 Clock control/status */
	u8	res5[0x1c];
	u32	clkc5csr;	/* Core 5 Clock control/status */
	u8	res6[0x1c];
	u32	clkc6csr;	/* Core 6 Clock control/status */
	u8	res7[0x1c];
	u32	clkc7csr;	/* Core 7 Clock control/status */
	u8	res8[0x71c];
	u32	pllc1gsr;	/* Cluster PLL 1 General Status */
	u8	res10[0x1c];
	u32	pllc2gsr;	/* Cluster PLL 2 General Status */
	u8	res11[0x1c];
	u32	pllc3gsr;	/* Cluster PLL 3 General Status */
	u8	res12[0x1c];
	u32	pllc4gsr;	/* Cluster PLL 4 General Status */
	u8	res13[0x39c];
	u32	pllpgsr;	/* Platform PLL General Status */
	u8	res14[0x1c];
	u32	plldgsr;	/* DDR PLL General Status */
	u8	res15[0x3dc];
} ccsr_clk_t;

typedef struct ccsr_rcpm {
	u8	res1[4];
	u32	cdozsrl;	/* Core Doze Status */
	u8	res2[4];
	u32	cdozcrl;	/* Core Doze Control */
	u8	res3[4];
	u32	cnapsrl;	/* Core Nap Status */
	u8	res4[4];
	u32	cnapcrl;	/* Core Nap Control */
	u8	res5[4];
	u32	cdozpsrl;	/* Core Doze Previous Status */
	u8	res6[4];
	u32	cdozpcrl;	/* Core Doze Previous Control */
	u8	res7[4];
	u32	cwaitsrl;	/* Core Wait Status */
	u8	res8[8];
	u32	powmgtcsr;	/* Power Mangement Control & Status */
	u8	res9[12];
	u32	ippdexpcr0;	/* IP Powerdown Exception Control 0 */
	u8	res10[12];
	u8	res11[4];
	u32	cpmimrl;	/* Core PM IRQ Masking */
	u8	res12[4];
	u32	cpmcimrl;	/* Core PM Critical IRQ Masking */
	u8	res13[4];
	u32	cpmmcimrl;	/* Core PM Machine Check IRQ Masking */
	u8	res14[4];
	u32	cpmnmimrl;	/* Core PM NMI Masking */
	u8	res15[4];
	u32	ctbenrl;	/* Core Time Base Enable */
	u8	res16[4];
	u32	ctbclkselrl;	/* Core Time Base Clock Select */
	u8	res17[4];
	u32	ctbhltcrl;	/* Core Time Base Halt Control */
	u8	res18[0xf68];
} ccsr_rcpm_t;

#else
typedef struct ccsr_gur {
	u32	porpllsr;	/* POR PLL ratio status */
#ifdef CONFIG_MPC8536
#define MPC85xx_PORPLLSR_DDR_RATIO	0x3e000000
#define MPC85xx_PORPLLSR_DDR_RATIO_SHIFT	25
#else
#ifdef CONFIG_BSC9131
#define MPC85xx_PORPLLSR_DDR_RATIO	0x00003f00
#else
#define MPC85xx_PORPLLSR_DDR_RATIO	0x00003e00
#endif
#define MPC85xx_PORPLLSR_DDR_RATIO_SHIFT	9
#endif
#define MPC85xx_PORPLLSR_QE_RATIO	0x3e000000
#define MPC85xx_PORPLLSR_QE_RATIO_SHIFT		25
#define MPC85xx_PORPLLSR_PLAT_RATIO	0x0000003e
#define MPC85xx_PORPLLSR_PLAT_RATIO_SHIFT	1
	u32	porbmsr;	/* POR boot mode status */
#define MPC85xx_PORBMSR_HA		0x00070000
#define MPC85xx_PORBMSR_HA_SHIFT	16
	u32	porimpscr;	/* POR I/O impedance status & control */
	u32	pordevsr;	/* POR I/O device status regsiter */
#if defined(CONFIG_P1017) || defined(CONFIG_P1023)
#define MPC85xx_PORDEVSR_SGMII1_DIS	0x10000000
#define MPC85xx_PORDEVSR_SGMII2_DIS	0x08000000
#define MPC85xx_PORDEVSR_TSEC1_PRTC	0x02000000
#else
#define MPC85xx_PORDEVSR_SGMII1_DIS	0x20000000
#define MPC85xx_PORDEVSR_SGMII2_DIS	0x10000000
#endif
#define MPC85xx_PORDEVSR_SGMII3_DIS	0x08000000
#define MPC85xx_PORDEVSR_SGMII4_DIS	0x04000000
#define MPC85xx_PORDEVSR_SRDS2_IO_SEL	0x38000000
#define MPC85xx_PORDEVSR_PCI1		0x00800000
#if defined(CONFIG_P1013) || defined(CONFIG_P1022)
#define MPC85xx_PORDEVSR_IO_SEL		0x007c0000
#define MPC85xx_PORDEVSR_IO_SEL_SHIFT	18
#elif defined(CONFIG_P1017) || defined(CONFIG_P1023)
#define MPC85xx_PORDEVSR_IO_SEL		0x00600000
#define MPC85xx_PORDEVSR_IO_SEL_SHIFT	21
#else
#if defined(CONFIG_P1010)
#define MPC85xx_PORDEVSR_IO_SEL		0x00600000
#define MPC85xx_PORDEVSR_IO_SEL_SHIFT	21
#else
#define MPC85xx_PORDEVSR_IO_SEL		0x00780000
#define MPC85xx_PORDEVSR_IO_SEL_SHIFT	19
#endif /* if defined(CONFIG_P1010) */
#endif
#define MPC85xx_PORDEVSR_PCI2_ARB	0x00040000
#define MPC85xx_PORDEVSR_PCI1_ARB	0x00020000
#define MPC85xx_PORDEVSR_PCI1_PCI32	0x00010000
#define MPC85xx_PORDEVSR_PCI1_SPD	0x00008000
#define MPC85xx_PORDEVSR_PCI2_SPD	0x00004000
#define MPC85xx_PORDEVSR_DRAM_RTYPE	0x00000060
#define MPC85xx_PORDEVSR_RIO_CTLS	0x00000008
#define MPC85xx_PORDEVSR_RIO_DEV_ID	0x00000007
	u32	pordbgmsr;	/* POR debug mode status */
	u32	pordevsr2;	/* POR I/O device status 2 */
/* The 8544 RM says this is bit 26, but it's really bit 24 */
#define MPC85xx_PORDEVSR2_SEC_CFG	0x00000080
	u8	res1[8];
	u32	gpporcr;	/* General-purpose POR configuration */
	u8	res2[12];
#if defined(CONFIG_MPC8536)
	u32	gencfgr;	/* General Configuration Register */
#define MPC85xx_GENCFGR_SDHC_WP_INV	0x20000000
#else
	u32	gpiocr;		/* GPIO control */
#endif
	u8	res3[12];
#if defined(CONFIG_MPC8569)
	u32	plppar1;	/* Platform port pin assignment 1 */
	u32	plppar2;	/* Platform port pin assignment 2 */
	u32	plpdir1;	/* Platform port pin direction 1 */
	u32	plpdir2;	/* Platform port pin direction 2 */
#else
	u32	gpoutdr;	/* General-purpose output data */
	u8	res4[12];
#endif
	u32	gpindr;		/* General-purpose input data */
	u8	res5[12];
	u32	pmuxcr;		/* Alt. function signal multiplex control */
#if defined(CONFIG_P1010) || defined(CONFIG_P1014)
#define MPC85xx_PMUXCR_TSEC1_0_1588		0x40000000
#define MPC85xx_PMUXCR_TSEC1_0_RES		0xC0000000
#define MPC85xx_PMUXCR_TSEC1_1_1588_TRIG	0x10000000
#define MPC85xx_PMUXCR_TSEC1_1_GPIO_12		0x20000000
#define MPC85xx_PMUXCR_TSEC1_1_RES		0x30000000
#define MPC85xx_PMUXCR_TSEC1_2_DMA		0x04000000
#define MPC85xx_PMUXCR_TSEC1_2_GPIO		0x08000000
#define MPC85xx_PMUXCR_TSEC1_2_RES		0x0C000000
#define MPC85xx_PMUXCR_TSEC1_3_RES		0x01000000
#define MPC85xx_PMUXCR_TSEC1_3_GPIO_15		0x02000000
#define MPC85xx_PMUXCR_IFC_ADDR16_SDHC		0x00400000
#define MPC85xx_PMUXCR_IFC_ADDR16_USB		0x00800000
#define MPC85xx_PMUXCR_IFC_ADDR16_IFC_CS2	0x00C00000
#define MPC85xx_PMUXCR_IFC_ADDR17_18_SDHC	0x00100000
#define MPC85xx_PMUXCR_IFC_ADDR17_18_USB	0x00200000
#define MPC85xx_PMUXCR_IFC_ADDR17_18_DMA	0x00300000
#define MPC85xx_PMUXCR_IFC_ADDR19_SDHC_DATA	0x00040000
#define MPC85xx_PMUXCR_IFC_ADDR19_USB		0x00080000
#define MPC85xx_PMUXCR_IFC_ADDR19_DMA		0x000C0000
#define MPC85xx_PMUXCR_IFC_ADDR20_21_SDHC_DATA	0x00010000
#define MPC85xx_PMUXCR_IFC_ADDR20_21_USB	0x00020000
#define MPC85xx_PMUXCR_IFC_ADDR20_21_RES	0x00030000
#define MPC85xx_PMUXCR_IFC_ADDR22_SDHC		0x00004000
#define MPC85xx_PMUXCR_IFC_ADDR22_USB		0x00008000
#define MPC85xx_PMUXCR_IFC_ADDR22_RES		0x0000C000
#define MPC85xx_PMUXCR_IFC_ADDR23_SDHC		0x00001000
#define MPC85xx_PMUXCR_IFC_ADDR23_USB		0x00002000
#define MPC85xx_PMUXCR_IFC_ADDR23_RES		0x00003000
#define MPC85xx_PMUXCR_IFC_ADDR24_SDHC		0x00000400
#define MPC85xx_PMUXCR_IFC_ADDR24_USB		0x00000800
#define MPC85xx_PMUXCR_IFC_ADDR24_RES		0x00000C00
#define MPC85xx_PMUXCR_IFC_PAR_PERR_RES		0x00000300
#define MPC85xx_PMUXCR_IFC_PAR_PERR_USB		0x00000200
#define MPC85xx_PMUXCR_LCLK_RES			0x00000040
#define MPC85xx_PMUXCR_LCLK_USB			0x00000080
#define MPC85xx_PMUXCR_LCLK_IFC_CS3		0x000000C0
#define MPC85xx_PMUXCR_SPI_RES			0x00000030
#define MPC85xx_PMUXCR_SPI_GPIO			0x00000020
#define MPC85xx_PMUXCR_CAN1_UART		0x00000004
#define MPC85xx_PMUXCR_CAN1_TDM			0x00000008
#define MPC85xx_PMUXCR_CAN1_RES			0x0000000C
#define MPC85xx_PMUXCR_CAN2_UART		0x00000001
#define MPC85xx_PMUXCR_CAN2_TDM			0x00000002
#define MPC85xx_PMUXCR_CAN2_RES			0x00000003
#endif
#if defined(CONFIG_P1017) || defined(CONFIG_P1023)
#define MPC85xx_PMUXCR_TSEC1_1		0x10000000
#else
#define MPC85xx_PMUXCR_SD_DATA		0x80000000
#define MPC85xx_PMUXCR_SDHC_CD		0x40000000
#define MPC85xx_PMUXCR_SDHC_WP		0x20000000
#define MPC85xx_PMUXCR_ELBC_OFF_USB2_ON	0x01000000
#define MPC85xx_PMUXCR_TDM_ENA		0x00800000
#define MPC85xx_PMUXCR_QE0		0x00008000
#define MPC85xx_PMUXCR_QE1		0x00004000
#define MPC85xx_PMUXCR_QE2		0x00002000
#define MPC85xx_PMUXCR_QE3		0x00001000
#define MPC85xx_PMUXCR_QE4		0x00000800
#define MPC85xx_PMUXCR_QE5		0x00000400
#define MPC85xx_PMUXCR_QE6		0x00000200
#define MPC85xx_PMUXCR_QE7		0x00000100
#define MPC85xx_PMUXCR_QE8		0x00000080
#define MPC85xx_PMUXCR_QE9		0x00000040
#define MPC85xx_PMUXCR_QE10		0x00000020
#define MPC85xx_PMUXCR_QE11		0x00000010
#define MPC85xx_PMUXCR_QE12		0x00000008
#endif
#if defined(CONFIG_P1013) || defined(CONFIG_P1022)
#define MPC85xx_PMUXCR_TDM_MASK		0x0001cc00
#define MPC85xx_PMUXCR_TDM		0x00014800
#define MPC85xx_PMUXCR_SPI_MASK		0x00600000
#define MPC85xx_PMUXCR_SPI		0x00000000
#endif
#if defined(CONFIG_BSC9131)
#define MPC85xx_PMUXCR_TSEC2_DMA_GPIO_IRQ	0x40000000
#define MPC85xx_PMUXCR_TSEC2_USB		0xC0000000
#define MPC85xx_PMUXCR_TSEC2_1588_PPS		0x10000000
#define MPC85xx_PMUXCR_TSEC2_1588_RSVD		0x30000000
#define MPC85xx_PMUXCR_IFC_AD_GPIO		0x04000000
#define MPC85xx_PMUXCR_IFC_AD_GPIO_MASK		0x0C000000
#define MPC85xx_PMUXCR_IFC_AD15_GPIO		0x01000000
#define MPC85xx_PMUXCR_IFC_AD15_TIMER2		0x02000000
#define MPC85xx_PMUXCR_IFC_AD16_GPO8		0x00400000
#define MPC85xx_PMUXCR_IFC_AD16_MSRCID0		0x00800000
#define MPC85xx_PMUXCR_IFC_AD17_GPO		0x00100000
#define MPC85xx_PMUXCR_IFC_AD17_GPO_MASK	0x00300000
#define MPC85xx_PMUXCR_IFC_AD17_MSRCID_DSP	0x00200000
#define MPC85xx_PMUXCR_IFC_CS2_GPO65		0x00040000
#define MPC85xx_PMUXCR_IFC_CS2_DSP_TDI		0x00080000
#define MPC85xx_PMUXCR_SDHC_USIM		0x00010000
#define MPC85xx_PMUXCR_SDHC_TDM_RFS_RCK		0x00020000
#define MPC85xx_PMUXCR_SDHC_GPIO77		0x00030000
#define MPC85xx_PMUXCR_SDHC_RESV		0x00004000
#define MPC85xx_PMUXCR_SDHC_TDM_TXD_RXD		0x00008000
#define MPC85xx_PMUXCR_SDHC_GPIO_TIMER4		0x0000C000
#define MPC85xx_PMUXCR_USB_CLK_UART_SIN		0x00001000
#define MPC85xx_PMUXCR_USB_CLK_GPIO69		0x00002000
#define MPC85xx_PMUXCR_USB_CLK_TIMER3		0x00003000
#define MPC85xx_PMUXCR_USB_UART_GPIO0		0x00000400
#define MPC85xx_PMUXCR_USB_RSVD			0x00000C00
#define MPC85xx_PMUXCR_USB_GPIO62_TRIG_IN	0x00000800
#define MPC85xx_PMUXCR_USB_D1_2_IIC2_SDA_SCL	0x00000100
#define MPC85xx_PMUXCR_USB_D1_2_GPIO71_72	0x00000200
#define MPC85xx_PMUXCR_USB_D1_2_RSVD		0x00000300
#define MPC85xx_PMUXCR_USB_DIR_GPIO2		0x00000040
#define MPC85xx_PMUXCR_USB_DIR_TIMER1		0x00000080
#define MPC85xx_PMUXCR_USB_DIR_MCP_B		0x000000C0
#define MPC85xx_PMUXCR_SPI1_UART3		0x00000010
#define MPC85xx_PMUXCR_SPI1_SIM			0x00000020
#define MPC85xx_PMUXCR_SPI1_CKSTP_IN_GPO74	0x00000030
#define MPC85xx_PMUXCR_SPI1_CS2_CKSTP_OUT_B	0x00000004
#define MPC85xx_PMUXCR_SPI1_CS2_dbg_adi1_rxen	0x00000008
#define MPC85xx_PMUXCR_SPI1_CS2_GPO75		0x0000000C
#define MPC85xx_PMUXCR_SPI1_CS3_ANT_TCXO_PWM	0x00000001
#define MPC85xx_PMUXCR_SPI1_CS3_dbg_adi2_rxen	0x00000002
#define MPC85xx_PMUXCR_SPI1_CS3_GPO76		0x00000003
#endif
	u32	pmuxcr2;	/* Alt. function signal multiplex control 2 */
#if defined(CONFIG_P1010) || defined(CONFIG_P1014)
#define MPC85xx_PMUXCR2_UART_GPIO		0x40000000
#define MPC85xx_PMUXCR2_UART_TDM		0x80000000
#define MPC85xx_PMUXCR2_UART_RES		0xC0000000
#define MPC85xx_PMUXCR2_IRQ2_TRIG_IN		0x10000000
#define MPC85xx_PMUXCR2_IRQ2_RES		0x30000000
#define MPC85xx_PMUXCR2_IRQ3_SRESET		0x04000000
#define MPC85xx_PMUXCR2_IRQ3_RES		0x0C000000
#define MPC85xx_PMUXCR2_GPIO01_DRVVBUS		0x01000000
#define MPC85xx_PMUXCR2_GPIO01_RES		0x03000000
#define MPC85xx_PMUXCR2_GPIO23_CKSTP		0x00400000
#define MPC85xx_PMUXCR2_GPIO23_RES		0x00800000
#define MPC85xx_PMUXCR2_GPIO23_USB		0x00C00000
#define MPC85xx_PMUXCR2_GPIO4_MCP		0x00100000
#define MPC85xx_PMUXCR2_GPIO4_RES		0x00200000
#define MPC85xx_PMUXCR2_GPIO4_CLK_OUT		0x00300000
#define MPC85xx_PMUXCR2_GPIO5_UDE		0x00040000
#define MPC85xx_PMUXCR2_GPIO5_RES		0x00080000
#define MPC85xx_PMUXCR2_READY_ASLEEP		0x00020000
#define MPC85xx_PMUXCR2_DDR_ECC_MUX		0x00010000
#define MPC85xx_PMUXCR2_DEBUG_PORT_EXPOSE	0x00008000
#define MPC85xx_PMUXCR2_POST_EXPOSE		0x00004000
#define MPC85xx_PMUXCR2_DEBUG_MUX_SEL_USBPHY	0x00002000
#define MPC85xx_PMUXCR2_PLL_LKDT_EXPOSE		0x00001000
#endif
#if defined(CONFIG_P1013) || defined(CONFIG_P1022)
#define MPC85xx_PMUXCR2_ETSECUSB_MASK	0x001f8000
#define MPC85xx_PMUXCR2_USB		0x00150000
#endif
#if defined(CONFIG_BSC9131)
#define MPC85xx_PMUXCR2_UART_CTS_B0_SIM_PD		0X40000000
#define MPC85xx_PMUXCR2_UART_CTS_B0_DSP_TMS		0X80000000
#define MPC85xx_PMUXCR2_UART_CTS_B0_GPIO42		0xC0000000
#define MPC85xx_PMUXCR2_UART_RTS_B0_PWM2		0x10000000
#define MPC85xx_PMUXCR2_UART_RTS_B0_DSP_TCK		0x20000000
#define MPC85xx_PMUXCR2_UART_RTS_B0_GPIO43		0x30000000
#define MPC85xx_PMUXCR2_UART_CTS_B1_SIM_PD		0x04000000
#define MPC85xx_PMUXCR2_UART_CTS_B1_SRESET_B		0x08000000
#define MPC85xx_PMUXCR2_UART_CTS_B1_GPIO44		0x0C000000
#define MPC85xx_PMUXCR2_UART_RTS_B1_PPS_LED		0x01000000
#define MPC85xx_PMUXCR2_UART_RTS_B1_RSVD		0x02000000
#define MPC85xx_PMUXCR2_UART_RTS_B1_GPIO45		0x03000000
#define MPC85xx_PMUXCR2_TRIG_OUT_ASLEEP			0x00400000
#define MPC85xx_PMUXCR2_TRIG_OUT_DSP_TRST_B		0x00800000
#define MPC85xx_PMUXCR2_ANT1_TIMER5			0x00100000
#define MPC85xx_PMUXCR2_ANT1_TSEC_1588			0x00200000
#define MPC85xx_PMUXCR2_ANT1_GPIO95_19			0x00300000
#define MPC85xx_PMUXCR2_ANT1_TX_RX_FRAME_MAX3_LOCK	0x00040000
#define MPC85xx_PMUXCR2_ANT1_TX_RX_FRAME_RSVD		0x00080000
#define MPC85xx_PMUXCR2_ANT1_TX_RX_FRAME_GPIO80_20	0x000C0000
#define MPC85xx_PMUXCR2_ANT1_DIO0_3_SPI3_CS0		0x00010000
#define MPC85xx_PMUXCR2_ANT1_DIO0_3_ANT2_DO_3		0x00020000
#define MPC85xx_PMUXCR2_ANT1_DIO0_3_GPIO81_84		0x00030000
#define MPC85xx_PMUXCR2_ANT1_DIO4_7_SPI4		0x00004000
#define MPC85xx_PMUXCR2_ANT1_DIO4_7_ANT2_DO4_7		0x00008000
#define MPC85xx_PMUXCR2_ANT1_DIO4_7_GPIO85_88		0x0000C000
#define MPC85xx_PMUXCR2_ANT1_DIO8_9_MAX2_1_LOCK		0x00001000
#define MPC85xx_PMUXCR2_ANT1_DIO8_9_ANT2_DO8_9		0x00002000
#define MPC85xx_PMUXCR2_ANT1_DIO8_9_GPIO21_22		0x00003000
#define MPC85xx_PMUXCR2_ANT1_DIO10_11_TIMER6_7		0x00000400
#define MPC85xx_PMUXCR2_ANT1_DIO10_11_ANT2_DO10_11	0x00000800
#define MPC85xx_PMUXCR2_ANT1_DIO10_11_GPIO23_24		0x00000C00
#define MPC85xx_PMUXCR2_ANT2_RSVD			0x00000100
#define MPC85xx_PMUXCR2_ANT2_GPO90_91_DMA		0x00000300
#define MPC85xx_PMUXCR2_ANT2_ENABLE_DIO0_10_USB		0x00000040
#define MPC85xx_PMUXCR2_ANT2_ENABLE_DIO0_10_GPIO	0x000000C0
#define MPC85xx_PMUXCR2_ANT2_DIO11_RSVD			0x00000010
#define MPC85xx_PMUXCR2_ANT2_DIO11_TIMER8		0x00000020
#define MPC85xx_PMUXCR2_ANT2_DIO11_GPIO61		0x00000030
#define MPC85xx_PMUXCR2_ANT3_AGC_GPO53			0x00000004
#define MPC85xx_PMUXCR2_ANT3_DO_TDM			0x00000001
#define MPC85xx_PMUXCR2_ANT3_DO_GPIO46_49		0x00000002
	u32	pmuxcr3;

#define MPC85xx_PMUXCR3_ANT3_DO4_5_TDM			0x40000000
#define MPC85xx_PMUXCR3_ANT3_DO4_5_GPIO_50_51		0x80000000
#define MPC85xx_PMUXCR3_ANT3_DO6_7_TRIG_IN_SRESET_B	0x10000000
#define MPC85xx_PMUXCR3_ANT3_DO6_7_GPIO_52_53		0x20000000
#define MPC85xx_PMUXCR3_ANT3_DO8_MCP_B			0x04000000
#define MPC85xx_PMUXCR3_ANT3_DO8_GPIO54			0x08000000
#define MPC85xx_PMUXCR3_ANT3_DO9_10_CKSTP_IN_OUT	0x01000000
#define MPC85xx_PMUXCR3_ANT3_DO9_10_GPIO55_56		0x02000000
#define MPC85xx_PMUXCR3_ANT3_DO11_IRQ_OUT		0x00400000
#define MPC85xx_PMUXCR3_ANT3_DO11_GPIO57		0x00800000
#define MPC85xx_PMUXCR3_SPI2_CS2_GPO93			0x00100000
#define MPC85xx_PMUXCR3_SPI2_CS3_GPO94			0x00040000
#define MPC85xx_PMUXCR3_ANT2_AGC_RSVD			0x00010000
#define MPC85xx_PMUXCR3_ANT2_GPO89			0x00030000
	u32 pmuxcr4;
#else
	u8	res6[8];
#endif
	u32	devdisr;	/* Device disable control */
#define MPC85xx_DEVDISR_PCI1		0x80000000
#define MPC85xx_DEVDISR_PCI2		0x40000000
#define MPC85xx_DEVDISR_PCIE		0x20000000
#define MPC85xx_DEVDISR_LBC		0x08000000
#define MPC85xx_DEVDISR_PCIE2		0x04000000
#define MPC85xx_DEVDISR_PCIE3		0x02000000
#define MPC85xx_DEVDISR_SEC		0x01000000
#define MPC85xx_DEVDISR_SRIO		0x00080000
#define MPC85xx_DEVDISR_RMSG		0x00040000
#define MPC85xx_DEVDISR_DDR		0x00010000
#define MPC85xx_DEVDISR_CPU		0x00008000
#define MPC85xx_DEVDISR_CPU0		MPC85xx_DEVDISR_CPU
#define MPC85xx_DEVDISR_TB		0x00004000
#define MPC85xx_DEVDISR_TB0		MPC85xx_DEVDISR_TB
#define MPC85xx_DEVDISR_CPU1		0x00002000
#define MPC85xx_DEVDISR_TB1		0x00001000
#define MPC85xx_DEVDISR_DMA		0x00000400
#define MPC85xx_DEVDISR_TSEC1		0x00000080
#define MPC85xx_DEVDISR_TSEC2		0x00000040
#define MPC85xx_DEVDISR_TSEC3		0x00000020
#define MPC85xx_DEVDISR_TSEC4		0x00000010
#define MPC85xx_DEVDISR_I2C		0x00000004
#define MPC85xx_DEVDISR_DUART		0x00000002
	u8	res7[12];
	u32	powmgtcsr;	/* Power management status & control */
	u8	res8[12];
	u32	mcpsumr;	/* Machine check summary */
	u8	res9[12];
	u32	pvr;		/* Processor version */
	u32	svr;		/* System version */
	u8	res10[8];
	u32	rstcr;		/* Reset control */
#if defined(CONFIG_MPC8568)||defined(CONFIG_MPC8569)
	u8	res11a[76];
	par_io_t qe_par_io[7];
	u8	res11b[1600];
#elif defined(CONFIG_P1012) || defined(CONFIG_P1016) || \
      defined(CONFIG_P1021) || defined(CONFIG_P1025)
	u8      res11a[12];
	u32     iovselsr;
	u8      res11b[60];
	par_io_t qe_par_io[3];
	u8      res11c[1496];
#else
	u8	res11a[1868];
#endif
	u32	clkdvdr;	/* Clock Divide register */
	u8	res12[1532];
	u32	clkocr;		/* Clock out select */
	u8	res13[12];
	u32	ddrdllcr;	/* DDR DLL control */
	u8	res14[12];
	u32	lbcdllcr;	/* LBC DLL control */
#if defined(CONFIG_BSC9131)
	u8	res15[12];
	u32	halt_req_mask;
#define HALTED_TO_HALT_REQ_MASK_0	0x80000000
	u8	res18[232];
#else
	u8	res15[248];
#endif
	u32	lbiuiplldcr0;	/* LBIU PLL Debug Reg 0 */
	u32	lbiuiplldcr1;	/* LBIU PLL Debug Reg 1 */
	u32	ddrioovcr;	/* DDR IO Override Control */
	u32	tsec12ioovcr;	/* eTSEC 1/2 IO override control */
	u32	tsec34ioovcr;	/* eTSEC 3/4 IO override control */
	u8      res16[52];
	u32	sdhcdcr;	/* SDHC debug control register */
	u8      res17[61592];
} ccsr_gur_t;
#endif

#define SDHCDCR_CD_INV		0x80000000 /* invert SDHC card detect */

typedef struct serdes_corenet {
	struct {
		u32	rstctl;	/* Reset Control Register */
#define SRDS_RSTCTL_RST		0x80000000
#define SRDS_RSTCTL_RSTDONE	0x40000000
#define SRDS_RSTCTL_RSTERR	0x20000000
#define SRDS_RSTCTL_SDPD	0x00000020
		u32	pllcr0; /* PLL Control Register 0 */
#define SRDS_PLLCR0_RFCK_SEL_MASK	0x30000000
#define SRDS_PLLCR0_RFCK_SEL_100	0x00000000
#define SRDS_PLLCR0_RFCK_SEL_125	0x10000000
#define SRDS_PLLCR0_RFCK_SEL_156_25	0x20000000
#define SRDS_PLLCR0_RFCK_SEL_150	0x30000000
#define SRDS_PLLCR0_FRATE_SEL_MASK	0x00030000
#define SRDS_PLLCR0_FRATE_SEL_5		0x00000000
#define SRDS_PLLCR0_FRATE_SEL_6_25	0x00010000
		u32	pllcr1; /* PLL Control Register 1 */
#define SRDS_PLLCR1_PLL_BWSEL	0x08000000
		u32	res[5];
	} bank[3];
	u32	res1[12];
	u32	srdstcalcr;	/* TX Calibration Control */
	u32	res2[3];
	u32	srdsrcalcr;	/* RX Calibration Control */
	u32	res3[3];
	u32	srdsgr0;	/* General Register 0 */
	u32	res4[11];
	u32	srdspccr0;	/* Protocol Converter Config 0 */
	u32	srdspccr1;	/* Protocol Converter Config 1 */
	u32	srdspccr2;	/* Protocol Converter Config 2 */
#define SRDS_PCCR2_RST_XGMII1		0x00800000
#define SRDS_PCCR2_RST_XGMII2		0x00400000
	u32	res5[197];
	struct {
		u32	gcr0;	/* General Control Register 0 */
#define SRDS_GCR0_RRST			0x00400000
#define SRDS_GCR0_1STLANE		0x00010000
		u32	gcr1;	/* General Control Register 1 */
#define SRDS_GCR1_REIDL_CTL_MASK	0x001f0000
#define SRDS_GCR1_REIDL_CTL_PCIE	0x00100000
#define SRDS_GCR1_REIDL_CTL_SRIO	0x00000000
#define SRDS_GCR1_REIDL_CTL_SGMII	0x00040000
#define SRDS_GCR1_OPAD_CTL		0x04000000
		u32	res1[4];
		u32	tecr0;	/* TX Equalization Control Reg 0 */
#define SRDS_TECR0_TEQ_TYPE_MASK	0x30000000
#define SRDS_TECR0_TEQ_TYPE_2LVL	0x10000000
		u32	res3;
		u32	ttlcr0;	/* Transition Tracking Loop Ctrl 0 */
#define SRDS_TTLCR0_FLT_SEL_MASK	0x3f000000
#define SRDS_TTLCR0_FLT_SEL_750PPM	0x03000000
#define SRDS_TTLCR0_PM_DIS		0x00004000
		u32	res4[7];
	} lane[24];
	u32 res6[384];
} serdes_corenet_t;

enum {
	FSL_SRDS_B1_LANE_A = 0,
	FSL_SRDS_B1_LANE_B = 1,
	FSL_SRDS_B1_LANE_C = 2,
	FSL_SRDS_B1_LANE_D = 3,
	FSL_SRDS_B1_LANE_E = 4,
	FSL_SRDS_B1_LANE_F = 5,
	FSL_SRDS_B1_LANE_G = 6,
	FSL_SRDS_B1_LANE_H = 7,
	FSL_SRDS_B1_LANE_I = 8,
	FSL_SRDS_B1_LANE_J = 9,
	FSL_SRDS_B2_LANE_A = 16,
	FSL_SRDS_B2_LANE_B = 17,
	FSL_SRDS_B2_LANE_C = 18,
	FSL_SRDS_B2_LANE_D = 19,
	FSL_SRDS_B3_LANE_A = 20,
	FSL_SRDS_B3_LANE_B = 21,
	FSL_SRDS_B3_LANE_C = 22,
	FSL_SRDS_B3_LANE_D = 23,
};

/* Security Engine Block (MS = Most Sig., LS = Least Sig.) */
#if CONFIG_SYS_FSL_SEC_COMPAT >= 4
typedef struct ccsr_sec {
	u32	res0;
	u32	mcfgr;		/* Master CFG Register */
	u8	res1[0x8];
	struct {
		u32	ms;	/* Job Ring LIODN Register, MS */
		u32	ls;	/* Job Ring LIODN Register, LS */
	} jrliodnr[4];
	u8	res2[0x30];
	struct {
		u32	ms;	/* RTIC LIODN Register, MS */
		u32	ls;	/* RTIC LIODN Register, LS */
	} rticliodnr[4];
	u8	res3[0x1c];
	u32	decorr;		/* DECO Request Register */
	struct {
		u32	ms;	/* DECO LIODN Register, MS */
		u32	ls;	/* DECO LIODN Register, LS */
	} decoliodnr[5];
	u8	res4[0x58];
	u32	dar;		/* DECO Avail Register */
	u32	drr;		/* DECO Reset Register */
	u8	res5[0xe78];
	u32	crnr_ms;	/* CHA Revision Number Register, MS */
	u32	crnr_ls;	/* CHA Revision Number Register, LS */
	u32	ctpr_ms;	/* Compile Time Parameters Register, MS */
	u32	ctpr_ls;	/* Compile Time Parameters Register, LS */
	u8	res6[0x10];
	u32	far_ms;		/* Fault Address Register, MS */
	u32	far_ls;		/* Fault Address Register, LS */
	u32	falr;		/* Fault Address LIODN Register */
	u32	fadr;		/* Fault Address Detail Register */
	u8	res7[0x4];
	u32	csta;		/* CAAM Status Register */
	u8	res8[0x8];
	u32	rvid;		/* Run Time Integrity Checking Version ID Reg.*/
	u32	ccbvid;		/* CHA Cluster Block Version ID Register */
	u32	chavid_ms;	/* CHA Version ID Register, MS */
	u32	chavid_ls;	/* CHA Version ID Register, LS */
	u32	chanum_ms;	/* CHA Number Register, MS */
	u32	chanum_ls;	/* CHA Number Register, LS */
	u32	secvid_ms;	/* SEC Version ID Register, MS */
	u32	secvid_ls;	/* SEC Version ID Register, LS */
	u8	res9[0x6020];
	u32	qilcr_ms;	/* Queue Interface LIODN CFG Register, MS */
	u32	qilcr_ls;	/* Queue Interface LIODN CFG Register, LS */
	u8	res10[0x8fd8];
} ccsr_sec_t;

#define SEC_CTPR_MS_AXI_LIODN		0x08000000
#define SEC_CTPR_MS_QI			0x02000000
#define SEC_RVID_MA			0x0f000000
#define SEC_CHANUM_MS_JRNUM_MASK	0xf0000000
#define SEC_CHANUM_MS_JRNUM_SHIFT	28
#define SEC_CHANUM_MS_DECONUM_MASK	0x0f000000
#define SEC_CHANUM_MS_DECONUM_SHIFT	24
#endif

typedef struct ccsr_qman {
	struct {
		u32	qcsp_lio_cfg;	/* 0x0 - SW Portal n LIO cfg */
		u32	qcsp_io_cfg;	/* 0x4 - SW Portal n IO cfg */
		u32	res;
		u32	qcsp_dd_cfg;	/* 0xc - SW Portal n Dynamic Debug cfg */
	} qcsp[32];

	/* Not actually reserved, but irrelevant to u-boot */
	u8	res[0xbf8 - 0x200];
	u32	ip_rev_1;
	u32	ip_rev_2;
	u32	fqd_bare;	/* FQD Extended Base Addr Register */
	u32	fqd_bar;	/* FQD Base Addr Register */
	u8	res1[0x8];
	u32	fqd_ar;		/* FQD Attributes Register */
	u8	res2[0xc];
	u32	pfdr_bare;	/* PFDR Extended Base Addr Register */
	u32	pfdr_bar;	/* PFDR Base Addr Register */
	u8	res3[0x8];
	u32	pfdr_ar;	/* PFDR Attributes Register */
	u8	res4[0x4c];
	u32	qcsp_bare;	/* QCSP Extended Base Addr Register */
	u32	qcsp_bar;	/* QCSP Base Addr Register */
	u8	res5[0x78];
	u32	ci_sched_cfg;	/* Initiator Scheduling Configuration */
	u32	srcidr;		/* Source ID Register */
	u32	liodnr;		/* LIODN Register */
	u8	res6[4];
	u32	ci_rlm_cfg;	/* Initiator Read Latency Monitor Cfg */
	u32	ci_rlm_avg;	/* Initiator Read Latency Monitor Avg */
	u8	res7[0x2e8];
} ccsr_qman_t;

typedef struct ccsr_bman {
	/* Not actually reserved, but irrelevant to u-boot */
	u8	res[0xbf8];
	u32	ip_rev_1;
	u32	ip_rev_2;
	u32	fbpr_bare;	/* FBPR Extended Base Addr Register */
	u32	fbpr_bar;	/* FBPR Base Addr Register */
	u8	res1[0x8];
	u32	fbpr_ar;	/* FBPR Attributes Register */
	u8	res2[0xf0];
	u32	srcidr;		/* Source ID Register */
	u32	liodnr;		/* LIODN Register */
	u8	res7[0x2f4];
} ccsr_bman_t;

typedef struct ccsr_pme {
	u8	res0[0x804];
	u32	liodnbr;	/* LIODN Base Register */
	u8	res1[0x1f8];
	u32	srcidr;		/* Source ID Register */
	u8	res2[8];
	u32	liodnr;		/* LIODN Register */
	u8	res3[0x1e8];
	u32	pm_ip_rev_1;	/* PME IP Block Revision Reg 1*/
	u32	pm_ip_rev_2;	/* PME IP Block Revision Reg 1*/
	u8	res4[0x400];
} ccsr_pme_t;

typedef struct ccsr_usb_phy {
	u8	res0[0x18];
	u32	usb_enable_override;
	u8	res[0xe4];
} ccsr_usb_phy_t;
#define CONFIG_SYS_FSL_USB_ENABLE_OVERRIDE 1

#ifdef CONFIG_SYS_FSL_RAID_ENGINE
struct ccsr_raide {
	u8	res0[0x543];
	u32	liodnbr;			/* LIODN Base Register */
	u8	res1[0xab8];
	struct {
		struct {
			u32	cfg0;		/* cfg register 0 */
			u32	cfg1;		/* cfg register 1 */
			u8	res1[0x3f8];
		} ring[2];
		u8	res[0x800];
	} jq[2];
};
#endif

#ifdef CONFIG_SYS_DPAA_RMAN
struct ccsr_rman {
	u8	res0[0xf64];
	u32	mmliodnbr;	/* Message Manager LIODN Base Register */
	u32	mmitar;		/* RMAN Inbound Translation Address Register */
	u32	mmitdr;		/* RMAN Inbound Translation Data Register */
	u8	res4[0x1f090];
};
#endif

#ifdef CONFIG_FSL_CORENET
#define CONFIG_SYS_FSL_CORENET_CCM_OFFSET	0x0000
#define CONFIG_SYS_MPC85xx_DDR_OFFSET		0x8000
#define CONFIG_SYS_MPC85xx_DDR2_OFFSET		0x9000
#define CONFIG_SYS_FSL_CORENET_CLK_OFFSET	0xE1000
#define CONFIG_SYS_FSL_CORENET_RCPM_OFFSET	0xE2000
#define CONFIG_SYS_FSL_CORENET_SERDES_OFFSET	0xEA000
#define CONFIG_SYS_FSL_CPC_OFFSET		0x10000
#define CONFIG_SYS_MPC85xx_DMA1_OFFSET		0x100000
#define CONFIG_SYS_MPC85xx_DMA2_OFFSET		0x101000
#define CONFIG_SYS_MPC85xx_DMA_OFFSET		CONFIG_SYS_MPC85xx_DMA1_OFFSET
#define CONFIG_SYS_MPC85xx_ESPI_OFFSET		0x110000
#define CONFIG_SYS_MPC85xx_ESDHC_OFFSET		0x114000
#define CONFIG_SYS_MPC85xx_LBC_OFFSET		0x124000
#define CONFIG_SYS_MPC85xx_GPIO_OFFSET		0x130000
#define CONFIG_SYS_FSL_CORENET_RMAN_OFFSET	0x1e0000
#define CONFIG_SYS_MPC85xx_PCIE1_OFFSET		0x200000
#define CONFIG_SYS_MPC85xx_PCIE2_OFFSET		0x201000
#define CONFIG_SYS_MPC85xx_PCIE3_OFFSET		0x202000
#define CONFIG_SYS_MPC85xx_PCIE4_OFFSET		0x203000
#define CONFIG_SYS_MPC85xx_USB1_OFFSET		0x210000
#define CONFIG_SYS_MPC85xx_USB2_OFFSET		0x211000
#define CONFIG_SYS_MPC85xx_USB_OFFSET		CONFIG_SYS_MPC85xx_USB1_OFFSET
#define CONFIG_SYS_MPC85xx_USB1_PHY_OFFSET 0x214000
#define CONFIG_SYS_MPC85xx_USB2_PHY_OFFSET 0x214100
#define CONFIG_SYS_MPC85xx_SATA1_OFFSET		0x220000
#define CONFIG_SYS_MPC85xx_SATA2_OFFSET		0x221000
#define CONFIG_SYS_FSL_SEC_OFFSET		0x300000
#define CONFIG_SYS_FSL_CORENET_PME_OFFSET	0x316000
#define CONFIG_SYS_FSL_QMAN_OFFSET		0x318000
#define CONFIG_SYS_FSL_BMAN_OFFSET		0x31a000
#define CONFIG_SYS_FSL_RAID_ENGINE_OFFSET	0x320000
#define CONFIG_SYS_FSL_FM1_OFFSET		0x400000
#define CONFIG_SYS_FSL_FM1_RX0_1G_OFFSET	0x488000
#define CONFIG_SYS_FSL_FM1_RX1_1G_OFFSET	0x489000
#define CONFIG_SYS_FSL_FM1_RX2_1G_OFFSET	0x48a000
#define CONFIG_SYS_FSL_FM1_RX3_1G_OFFSET	0x48b000
#define CONFIG_SYS_FSL_FM1_RX4_1G_OFFSET	0x48c000
#define CONFIG_SYS_FSL_FM1_RX0_10G_OFFSET	0x490000
#define CONFIG_SYS_FSL_FM1_DTSEC1_OFFSET	0x4e0000
#define CONFIG_SYS_FSL_FM2_OFFSET		0x500000
#define CONFIG_SYS_FSL_FM2_RX0_1G_OFFSET	0x588000
#define CONFIG_SYS_FSL_FM2_RX1_1G_OFFSET	0x589000
#define CONFIG_SYS_FSL_FM2_RX2_1G_OFFSET	0x58a000
#define CONFIG_SYS_FSL_FM2_RX3_1G_OFFSET	0x58b000
#define CONFIG_SYS_FSL_FM2_RX4_1G_OFFSET	0x58c000
#define CONFIG_SYS_FSL_FM2_RX0_10G_OFFSET	0x590000
#else
#define CONFIG_SYS_MPC85xx_ECM_OFFSET		0x0000
#define CONFIG_SYS_MPC85xx_DDR_OFFSET		0x2000
#define CONFIG_SYS_MPC85xx_LBC_OFFSET		0x5000
#define CONFIG_SYS_MPC85xx_DDR2_OFFSET		0x6000
#define CONFIG_SYS_MPC85xx_ESPI_OFFSET		0x7000
#define CONFIG_SYS_MPC85xx_PCI1_OFFSET		0x8000
#define CONFIG_SYS_MPC85xx_PCIX_OFFSET		0x8000
#define CONFIG_SYS_MPC85xx_PCI2_OFFSET		0x9000
#define CONFIG_SYS_MPC85xx_PCIX2_OFFSET		0x9000
#define CONFIG_SYS_MPC85xx_PCIE1_OFFSET         0xa000
#define CONFIG_SYS_MPC85xx_PCIE2_OFFSET         0x9000
#if defined(CONFIG_MPC8572) || defined(CONFIG_P2020)
#define CONFIG_SYS_MPC85xx_PCIE3_OFFSET         0x8000
#else
#define CONFIG_SYS_MPC85xx_PCIE3_OFFSET         0xb000
#endif
#define CONFIG_SYS_MPC85xx_GPIO_OFFSET		0xF000
#define CONFIG_SYS_MPC85xx_SATA1_OFFSET		0x18000
#define CONFIG_SYS_MPC85xx_SATA2_OFFSET		0x19000
#define CONFIG_SYS_MPC85xx_IFC_OFFSET		0x1e000
#define CONFIG_SYS_MPC85xx_L2_OFFSET		0x20000
#define CONFIG_SYS_MPC85xx_DMA_OFFSET		0x21000
#define CONFIG_SYS_MPC85xx_USB_OFFSET		0x22000
#define CONFIG_SYS_MPC85xx_USB2_OFFSET		0x23000
#ifdef CONFIG_TSECV2
#define CONFIG_SYS_TSEC1_OFFSET			0xB0000
#else
#define CONFIG_SYS_TSEC1_OFFSET			0x24000
#endif
#define CONFIG_SYS_MDIO1_OFFSET			0x24000
#define CONFIG_SYS_MPC85xx_ESDHC_OFFSET		0x2e000
#define CONFIG_SYS_MPC85xx_SERDES2_OFFSET	0xE3100
#define CONFIG_SYS_MPC85xx_SERDES1_OFFSET	0xE3000
#define CONFIG_SYS_SNVS_OFFSET			0xE6000
#define CONFIG_SYS_SFP_OFFSET			0xE7000
#define CONFIG_SYS_MPC85xx_CPM_OFFSET		0x80000
#define CONFIG_SYS_FSL_QMAN_OFFSET		0x88000
#define CONFIG_SYS_FSL_BMAN_OFFSET		0x8a000
#define CONFIG_SYS_FSL_FM1_OFFSET		0x100000
#define CONFIG_SYS_FSL_FM1_RX0_1G_OFFSET	0x188000
#define CONFIG_SYS_FSL_FM1_RX1_1G_OFFSET	0x189000
#define CONFIG_SYS_FSL_FM1_DTSEC1_OFFSET	0x1e0000
#endif

#define CONFIG_SYS_MPC85xx_PIC_OFFSET		0x40000
#define CONFIG_SYS_MPC85xx_GUTS_OFFSET		0xE0000
#define CONFIG_SYS_FSL_SRIO_OFFSET		0xC0000

#define CONFIG_SYS_FSL_CPC_ADDR	\
	(CONFIG_SYS_CCSRBAR + CONFIG_SYS_FSL_CPC_OFFSET)
#define CONFIG_SYS_FSL_QMAN_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_FSL_QMAN_OFFSET)
#define CONFIG_SYS_FSL_BMAN_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_FSL_BMAN_OFFSET)
#define CONFIG_SYS_FSL_CORENET_PME_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_FSL_CORENET_PME_OFFSET)
#define CONFIG_SYS_FSL_RAID_ENGINE_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_FSL_RAID_ENGINE_OFFSET)
#define CONFIG_SYS_FSL_CORENET_RMAN_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_FSL_CORENET_RMAN_OFFSET)
#define CONFIG_SYS_MPC85xx_GUTS_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_GUTS_OFFSET)
#define CONFIG_SYS_FSL_CORENET_CCM_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_FSL_CORENET_CCM_OFFSET)
#define CONFIG_SYS_FSL_CORENET_CLK_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_FSL_CORENET_CLK_OFFSET)
#define CONFIG_SYS_FSL_CORENET_RCPM_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_FSL_CORENET_RCPM_OFFSET)
#define CONFIG_SYS_MPC85xx_ECM_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_ECM_OFFSET)
#define CONFIG_SYS_MPC85xx_DDR_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_DDR_OFFSET)
#define CONFIG_SYS_MPC85xx_DDR2_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_DDR2_OFFSET)
#define CONFIG_SYS_LBC_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_LBC_OFFSET)
#define CONFIG_SYS_IFC_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_IFC_OFFSET)
#define CONFIG_SYS_MPC85xx_ESPI_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_ESPI_OFFSET)
#define CONFIG_SYS_MPC85xx_PCIX_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_PCIX_OFFSET)
#define CONFIG_SYS_MPC85xx_PCIX2_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_PCIX2_OFFSET)
#define CONFIG_SYS_MPC85xx_GPIO_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_GPIO_OFFSET)
#define CONFIG_SYS_MPC85xx_SATA1_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_SATA1_OFFSET)
#define CONFIG_SYS_MPC85xx_SATA2_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_SATA2_OFFSET)
#define CONFIG_SYS_MPC85xx_L2_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_L2_OFFSET)
#define CONFIG_SYS_MPC85xx_DMA_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_DMA_OFFSET)
#define CONFIG_SYS_MPC85xx_ESDHC_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_ESDHC_OFFSET)
#define CONFIG_SYS_MPC8xxx_PIC_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_PIC_OFFSET)
#define CONFIG_SYS_MPC85xx_CPM_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_CPM_OFFSET)
#define CONFIG_SYS_MPC85xx_SERDES1_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_SERDES1_OFFSET)
#define CONFIG_SYS_MPC85xx_SERDES2_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_SERDES2_OFFSET)
#define CONFIG_SYS_FSL_CORENET_SERDES_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_FSL_CORENET_SERDES_OFFSET)
#define CONFIG_SYS_MPC85xx_USB_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_USB_OFFSET)
#define CONFIG_SYS_MPC85xx_USB1_PHY_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_USB1_PHY_OFFSET)
#define CONFIG_SYS_MPC85xx_USB2_PHY_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_USB2_PHY_OFFSET)
#define CONFIG_SYS_FSL_SEC_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_FSL_SEC_OFFSET)
#define CONFIG_SYS_FSL_FM1_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_FSL_FM1_OFFSET)
#define CONFIG_SYS_FSL_FM1_DTSEC1_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_FSL_FM1_DTSEC1_OFFSET)
#define CONFIG_SYS_FSL_FM2_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_FSL_FM2_OFFSET)
#define CONFIG_SYS_FSL_SRIO_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_FSL_SRIO_OFFSET)

#define CONFIG_SYS_PCI1_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_PCI1_OFFSET)
#define CONFIG_SYS_PCI2_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_PCI2_OFFSET)
#define CONFIG_SYS_PCIE1_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_PCIE1_OFFSET)
#define CONFIG_SYS_PCIE2_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_PCIE2_OFFSET)
#define CONFIG_SYS_PCIE3_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_PCIE3_OFFSET)
#define CONFIG_SYS_PCIE4_ADDR \
	(CONFIG_SYS_IMMR + CONFIG_SYS_MPC85xx_PCIE4_OFFSET)

#define TSEC_BASE_ADDR		(CONFIG_SYS_IMMR + CONFIG_SYS_TSEC1_OFFSET)
#define MDIO_BASE_ADDR		(CONFIG_SYS_IMMR + CONFIG_SYS_MDIO1_OFFSET)

#endif /*__IMMAP_85xx__*/
