/*
 * (C) Copyright 2009 Freescale Semiconductor, Inc.
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

#ifndef __ASM_ARCH_MX5_IMX_REGS_H__
#define __ASM_ARCH_MX5_IMX_REGS_H__

#if defined(CONFIG_MX51)
#define IRAM_BASE_ADDR		0x1FFE0000	/* internal ram */
#define IPU_SOC_BASE_ADDR	0x40000000
#define IPU_SOC_OFFSET		0x1E000000
#define SPBA0_BASE_ADDR         0x70000000
#define AIPS1_BASE_ADDR         0x73F00000
#define AIPS2_BASE_ADDR         0x83F00000
#define CSD0_BASE_ADDR          0x90000000
#define CSD1_BASE_ADDR          0xA0000000
#define NFC_BASE_ADDR_AXI       0xCFFF0000
#define CS1_BASE_ADDR           0xB8000000
#elif defined(CONFIG_MX53)
#define IPU_SOC_BASE_ADDR	0x18000000
#define IPU_SOC_OFFSET		0x06000000
#define SPBA0_BASE_ADDR         0x50000000
#define AIPS1_BASE_ADDR         0x53F00000
#define AIPS2_BASE_ADDR         0x63F00000
#define CSD0_BASE_ADDR          0x70000000
#define CSD1_BASE_ADDR          0xB0000000
#define NFC_BASE_ADDR_AXI       0xF7FF0000
#define IRAM_BASE_ADDR          0xF8000000
#define CS1_BASE_ADDR           0xF4000000
#define SATA_BASE_ADDR		0x10000000
#else
#error "CPU_TYPE not defined"
#endif

#define IRAM_SIZE		0x00020000	/* 128 KB */

/*
 * SPBA global module enabled #0
 */
#define MMC_SDHC1_BASE_ADDR	(SPBA0_BASE_ADDR + 0x00004000)
#define MMC_SDHC2_BASE_ADDR	(SPBA0_BASE_ADDR + 0x00008000)
#define UART3_BASE		(SPBA0_BASE_ADDR + 0x0000C000)
#define CSPI1_BASE_ADDR 	(SPBA0_BASE_ADDR + 0x00010000)
#define SSI2_BASE_ADDR		(SPBA0_BASE_ADDR + 0x00014000)
#define MMC_SDHC3_BASE_ADDR	(SPBA0_BASE_ADDR + 0x00020000)
#define MMC_SDHC4_BASE_ADDR	(SPBA0_BASE_ADDR + 0x00024000)
#define SPDIF_BASE_ADDR		(SPBA0_BASE_ADDR + 0x00028000)
#define ATA_DMA_BASE_ADDR	(SPBA0_BASE_ADDR + 0x00030000)
#define SLIM_DMA_BASE_ADDR	(SPBA0_BASE_ADDR + 0x00034000)
#define HSI2C_DMA_BASE_ADDR	(SPBA0_BASE_ADDR + 0x00038000)
#define SPBA_CTRL_BASE_ADDR	(SPBA0_BASE_ADDR + 0x0003C000)

/*
 * AIPS 1
 */
#define OTG_BASE_ADDR		(AIPS1_BASE_ADDR + 0x00080000)
#define GPIO1_BASE_ADDR		(AIPS1_BASE_ADDR + 0x00084000)
#define GPIO2_BASE_ADDR		(AIPS1_BASE_ADDR + 0x00088000)
#define GPIO3_BASE_ADDR		(AIPS1_BASE_ADDR + 0x0008C000)
#define GPIO4_BASE_ADDR		(AIPS1_BASE_ADDR + 0x00090000)
#define KPP_BASE_ADDR		(AIPS1_BASE_ADDR + 0x00094000)
#define WDOG1_BASE_ADDR		(AIPS1_BASE_ADDR + 0x00098000)
#define WDOG2_BASE_ADDR		(AIPS1_BASE_ADDR + 0x0009C000)
#define GPT1_BASE_ADDR		(AIPS1_BASE_ADDR + 0x000A0000)
#define SRTC_BASE_ADDR		(AIPS1_BASE_ADDR + 0x000A4000)
#define IOMUXC_BASE_ADDR	(AIPS1_BASE_ADDR + 0x000A8000)
#define EPIT1_BASE_ADDR		(AIPS1_BASE_ADDR + 0x000AC000)
#define EPIT2_BASE_ADDR		(AIPS1_BASE_ADDR + 0x000B0000)
#define PWM1_BASE_ADDR		(AIPS1_BASE_ADDR + 0x000B4000)
#define PWM2_BASE_ADDR		(AIPS1_BASE_ADDR + 0x000B8000)
#define UART1_BASE		(AIPS1_BASE_ADDR + 0x000BC000)
#define UART2_BASE		(AIPS1_BASE_ADDR + 0x000C0000)
#define SRC_BASE_ADDR		(AIPS1_BASE_ADDR + 0x000D0000)
#define CCM_BASE_ADDR		(AIPS1_BASE_ADDR + 0x000D4000)
#define GPC_BASE_ADDR		(AIPS1_BASE_ADDR + 0x000D8000)

#if defined(CONFIG_MX53)
#define GPIO5_BASE_ADDR         (AIPS1_BASE_ADDR + 0x000DC000)
#define GPIO6_BASE_ADDR         (AIPS1_BASE_ADDR + 0x000E0000)
#define GPIO7_BASE_ADDR         (AIPS1_BASE_ADDR + 0x000E4000)
#define UART4_BASE_ADDR         (AIPS1_BASE_ADDR + 0x000F0000)
#endif
/*
 * AIPS 2
 */
#define PLL1_BASE_ADDR		(AIPS2_BASE_ADDR + 0x00080000)
#define PLL2_BASE_ADDR		(AIPS2_BASE_ADDR + 0x00084000)
#define PLL3_BASE_ADDR		(AIPS2_BASE_ADDR + 0x00088000)
#ifdef	CONFIG_MX53
#define PLL4_BASE_ADDR		(AIPS2_BASE_ADDR + 0x0008c000)
#endif
#define AHBMAX_BASE_ADDR	(AIPS2_BASE_ADDR + 0x00094000)
#define IIM_BASE_ADDR		(AIPS2_BASE_ADDR + 0x00098000)
#define CSU_BASE_ADDR		(AIPS2_BASE_ADDR + 0x0009C000)
#define ARM_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000A0000)
#define OWIRE_BASE_ADDR 	(AIPS2_BASE_ADDR + 0x000A4000)
#define FIRI_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000A8000)
#define CSPI2_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000AC000)
#define SDMA_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000B0000)
#define SCC_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000B4000)
#define ROMCP_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000B8000)
#define RTIC_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000BC000)
#define CSPI3_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000C0000)
#define I2C2_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000C4000)
#define I2C1_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000C8000)
#define SSI1_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000CC000)
#define AUDMUX_BASE_ADDR	(AIPS2_BASE_ADDR + 0x000D0000)
#define M4IF_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000D8000)
#define ESDCTL_BASE_ADDR	(AIPS2_BASE_ADDR + 0x000D9000)
#define WEIM_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000DA000)
#define NFC_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000DB000)
#define EMI_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000DBF00)
#define MIPI_HSC_BASE_ADDR	(AIPS2_BASE_ADDR + 0x000DC000)
#define ATA_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000E0000)
#define SIM_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000E4000)
#define SSI3BASE_ADDR		(AIPS2_BASE_ADDR + 0x000E8000)
#define FEC_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000EC000)
#define TVE_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000F0000)
#define VPU_BASE_ADDR		(AIPS2_BASE_ADDR + 0x000F4000)
#define SAHARA_BASE_ADDR	(AIPS2_BASE_ADDR + 0x000F8000)

#if defined(CONFIG_MX53)
#define UART5_BASE_ADDR         (AIPS2_BASE_ADDR + 0x00090000)
#endif

/*
 * WEIM CSnGCR1
 */
#define CSEN		1
#define SWR		(1 << 1)
#define SRD		(1 << 2)
#define MUM		(1 << 3)
#define WFL		(1 << 4)
#define RFL		(1 << 5)
#define CRE		(1 << 6)
#define CREP		(1 << 7)
#define BL(x)		(((x) & 0x7) << 8)
#define WC		(1 << 11)
#define BCD(x)		(((x) & 0x3) << 12)
#define BCS(x)		(((x) & 0x3) << 14)
#define DSZ(x)		(((x) & 0x7) << 16)
#define SP		(1 << 19)
#define CSREC(x)	(((x) & 0x7) << 20)
#define AUS		(1 << 23)
#define GBC(x)		(((x) & 0x7) << 24)
#define WP		(1 << 27)
#define PSZ(x)		(((x) & 0x0f << 28)

/*
 * WEIM CSnGCR2
 */
#define ADH(x)		(((x) & 0x3))
#define DAPS(x)		(((x) & 0x0f << 4)
#define DAE		(1 << 8)
#define DAP		(1 << 9)
#define MUX16_BYP	(1 << 12)

/*
 * WEIM CSnRCR1
 */
#define RCSN(x)		(((x) & 0x7))
#define RCSA(x)		(((x) & 0x7) << 4)
#define OEN(x)		(((x) & 0x7) << 8)
#define OEA(x)		(((x) & 0x7) << 12)
#define RADVN(x)	(((x) & 0x7) << 16)
#define RAL		(1 << 19)
#define RADVA(x)	(((x) & 0x7) << 20)
#define RWSC(x)		(((x) & 0x3f) << 24)

/*
 * WEIM CSnRCR2
 */
#define RBEN(x)		(((x) & 0x7))
#define RBE		(1 << 3)
#define RBEA(x)		(((x) & 0x7) << 4)
#define RL(x)		(((x) & 0x3) << 8)
#define PAT(x)		(((x) & 0x7) << 12)
#define APR		(1 << 15)

/*
 * WEIM CSnWCR1
 */
#define WCSN(x)		(((x) & 0x7))
#define WCSA(x)		(((x) & 0x7) << 3)
#define WEN(x)		(((x) & 0x7) << 6)
#define WEA(x)		(((x) & 0x7) << 9)
#define WBEN(x)		(((x) & 0x7) << 12)
#define WBEA(x)		(((x) & 0x7) << 15)
#define WADVN(x)	(((x) & 0x7) << 18)
#define WADVA(x)	(((x) & 0x7) << 21)
#define WWSC(x)		(((x) & 0x3f) << 24)
#define WBED1		(1 << 30)
#define WAL		(1 << 31)

/*
 * WEIM CSnWCR2
 */
#define WBED		1

/*
 * WEIM WCR
 */
#define BCM		1
#define GBCD(x)		(((x) & 0x3) << 1)
#define INTEN		(1 << 4)
#define INTPOL		(1 << 5)
#define WDOG_EN		(1 << 8)
#define WDOG_LIMIT(x)	(((x) & 0x3) << 9)

#define CS0_128					0
#define CS0_64M_CS1_64M				1
#define CS0_64M_CS1_32M_CS2_32M			2
#define CS0_32M_CS1_32M_CS2_32M_CS3_32M		3

/*
 * CSPI register definitions
 */
#define MXC_ECSPI
#define MXC_CSPICTRL_EN		(1 << 0)
#define MXC_CSPICTRL_MODE	(1 << 1)
#define MXC_CSPICTRL_XCH	(1 << 2)
#define MXC_CSPICTRL_CHIPSELECT(x)	(((x) & 0x3) << 12)
#define MXC_CSPICTRL_BITCOUNT(x)	(((x) & 0xfff) << 20)
#define MXC_CSPICTRL_PREDIV(x)	(((x) & 0xF) << 12)
#define MXC_CSPICTRL_POSTDIV(x)	(((x) & 0xF) << 8)
#define MXC_CSPICTRL_SELCHAN(x)	(((x) & 0x3) << 18)
#define MXC_CSPICTRL_MAXBITS	0xfff
#define MXC_CSPICTRL_TC		(1 << 7)
#define MXC_CSPICTRL_RXOVF	(1 << 6)
#define MXC_CSPIPERIOD_32KHZ	(1 << 15)
#define MAX_SPI_BYTES	32

/* Bit position inside CTRL register to be associated with SS */
#define MXC_CSPICTRL_CHAN	18

/* Bit position inside CON register to be associated with SS */
#define MXC_CSPICON_POL		4
#define MXC_CSPICON_PHA		0
#define MXC_CSPICON_SSPOL	12
#define MXC_SPI_BASE_ADDRESSES \
	CSPI1_BASE_ADDR, \
	CSPI2_BASE_ADDR, \
	CSPI3_BASE_ADDR,

/*
 * Number of GPIO pins per port
 */
#define GPIO_NUM_PIN            32

#define IIM_SREV	0x24
#define ROM_SI_REV	0x48

#define NFC_BUF_SIZE	0x1000

/* M4IF */
#define M4IF_FBPM0	0x40
#define M4IF_FIDBP	0x48

/* Assuming 24MHz input clock with doubler ON */
/*                            MFI         PDF */
#define DP_OP_864	((8 << 4) + ((1 - 1)  << 0))
#define DP_MFD_864	(180 - 1) /* PL Dither mode */
#define DP_MFN_864	180
#define DP_MFN_800_DIT	60 /* PL Dither mode */

#define DP_OP_850	((8 << 4) + ((1 - 1)  << 0))
#define DP_MFD_850	(48 - 1)
#define DP_MFN_850	41

#define DP_OP_800	((8 << 4) + ((1 - 1)  << 0))
#define DP_MFD_800	(3 - 1)
#define DP_MFN_800	1

#define DP_OP_700	((7 << 4) + ((1 - 1)  << 0))
#define DP_MFD_700	(24 - 1)
#define DP_MFN_700	7

#define DP_OP_665	((6 << 4) + ((1 - 1)  << 0))
#define DP_MFD_665	(96 - 1)
#define DP_MFN_665	89

#define DP_OP_532	((5 << 4) + ((1 - 1)  << 0))
#define DP_MFD_532	(24 - 1)
#define DP_MFN_532	13

#define DP_OP_400	((8 << 4) + ((2 - 1)  << 0))
#define DP_MFD_400	(3 - 1)
#define DP_MFN_400	1

#define DP_OP_216	((6 << 4) + ((3 - 1)  << 0))
#define DP_MFD_216	(4 - 1)
#define DP_MFN_216	3

#define CHIP_REV_1_0            0x10
#define CHIP_REV_1_1            0x11
#define CHIP_REV_2_0            0x20
#define CHIP_REV_2_5		0x25
#define CHIP_REV_3_0            0x30

#define BOARD_REV_1_0           0x0
#define BOARD_REV_2_0           0x1

#define IMX_IIM_BASE            (IIM_BASE_ADDR)

#if !(defined(__KERNEL_STRICT_NAMES) || defined(__ASSEMBLY__))
#include <asm/types.h>

#define __REG(x)	(*((volatile u32 *)(x)))
#define __REG16(x)	(*((volatile u16 *)(x)))
#define __REG8(x)	(*((volatile u8 *)(x)))

struct clkctl {
	u32	ccr;
	u32	ccdr;
	u32	csr;
	u32	ccsr;
	u32	cacrr;
	u32	cbcdr;
	u32	cbcmr;
	u32	cscmr1;
	u32	cscmr2;
	u32	cscdr1;
	u32	cs1cdr;
	u32	cs2cdr;
	u32	cdcdr;
	u32	chsccdr;
	u32	cscdr2;
	u32	cscdr3;
	u32	cscdr4;
	u32	cwdr;
	u32	cdhipr;
	u32	cdcr;
	u32	ctor;
	u32	clpcr;
	u32	cisr;
	u32	cimr;
	u32	ccosr;
	u32	cgpr;
	u32	ccgr0;
	u32	ccgr1;
	u32	ccgr2;
	u32	ccgr3;
	u32	ccgr4;
	u32	ccgr5;
	u32	ccgr6;
#if defined(CONFIG_MX53)
	u32	ccgr7;
#endif
	u32	cmeor;
};

/* DPLL registers */
struct dpll {
	u32	dp_ctl;
	u32	dp_config;
	u32	dp_op;
	u32	dp_mfd;
	u32	dp_mfn;
	u32	dp_mfn_minus;
	u32	dp_mfn_plus;
	u32	dp_hfs_op;
	u32	dp_hfs_mfd;
	u32	dp_hfs_mfn;
	u32	dp_mfn_togc;
	u32	dp_destat;
};
/* WEIM registers */
struct weim {
	u32	cs0gcr1;
	u32	cs0gcr2;
	u32	cs0rcr1;
	u32	cs0rcr2;
	u32	cs0wcr1;
	u32	cs0wcr2;
	u32	cs1gcr1;
	u32	cs1gcr2;
	u32	cs1rcr1;
	u32	cs1rcr2;
	u32	cs1wcr1;
	u32	cs1wcr2;
	u32	cs2gcr1;
	u32	cs2gcr2;
	u32	cs2rcr1;
	u32	cs2rcr2;
	u32	cs2wcr1;
	u32	cs2wcr2;
	u32	cs3gcr1;
	u32	cs3gcr2;
	u32	cs3rcr1;
	u32	cs3rcr2;
	u32	cs3wcr1;
	u32	cs3wcr2;
	u32	cs4gcr1;
	u32	cs4gcr2;
	u32	cs4rcr1;
	u32	cs4rcr2;
	u32	cs4wcr1;
	u32	cs4wcr2;
	u32	cs5gcr1;
	u32	cs5gcr2;
	u32	cs5rcr1;
	u32	cs5rcr2;
	u32	cs5wcr1;
	u32	cs5wcr2;
	u32	wcr;
	u32	wiar;
	u32	ear;
};

#if defined(CONFIG_MX51)
struct iomuxc {
	u32	gpr0;
	u32	gpr1;
	u32	omux0;
	u32	omux1;
	u32	omux2;
	u32	omux3;
	u32	omux4;
};
#elif defined(CONFIG_MX53)
struct iomuxc {
	u32	gpr0;
	u32	gpr1;
	u32	gpr2;
	u32	omux0;
	u32	omux1;
	u32	omux2;
	u32	omux3;
	u32	omux4;
};
#endif

/* System Reset Controller (SRC) */
struct src {
	u32	scr;
	u32	sbmr;
	u32	srsr;
	u32	reserved1[2];
	u32	sisr;
	u32	simr;
};

/* CSPI registers */
struct cspi_regs {
	u32 rxdata;
	u32 txdata;
	u32 ctrl;
	u32 cfg;
	u32 intr;
	u32 dma;
	u32 stat;
	u32 period;
};

struct iim_regs {
	u32	stat;
	u32	statm;
	u32     err;
	u32	emask;
	u32	fctl;
	u32	ua;
	u32	la;
	u32	sdat;
	u32	prev;
	u32	srev;
	u32	preg_p;
	u32	scs0;
	u32	scs1;
	u32	scs2;
	u32	scs3;
	u32	res0[0x1f1];
	struct fuse_bank {
		u32	fuse_regs[0x20];
		u32	fuse_rsvd[0xe0];
	} bank[4];
};

struct fuse_bank0_regs {
	u32	fuse0_23[24];
	u32	gp[8];
};

struct fuse_bank1_regs {
	u32	fuse0_8[9];
	u32	mac_addr[6];
	u32	fuse15_31[0x11];
};

#endif /* __ASSEMBLER__*/

#endif				/* __ASM_ARCH_MX5_IMX_REGS_H__ */
