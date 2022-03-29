/*
 * (C) Copyright 2010
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _PPC405EP_H_
#define _PPC405EP_H_

#define CONFIG_SDRAM_PPC4xx_IBM_SDRAM	/* IBM SDRAM controller */

/* Memory mapped register */
#define CONFIG_SYS_PERIPHERAL_BASE	0xef600000 /* Internal Peripherals */

#define CONFIG_SYS_NS16550_COM1	(CONFIG_SYS_PERIPHERAL_BASE + 0x0300)
#define CONFIG_SYS_NS16550_COM2	(CONFIG_SYS_PERIPHERAL_BASE + 0x0400)

#define GPIO0_BASE		(CONFIG_SYS_PERIPHERAL_BASE + 0x0700)

/* DCR */
#define OCM0_ISCNTL	0x0019	/* OCM I-side control reg */
#define OCM0_DSARC	0x001a	/* OCM D-side address compare */
#define OCM0_DSCNTL	0x001b	/* OCM D-side control */
#define CPC0_PLLMR0	0x00f0	/* PLL mode  register 0	*/
#define CPC0_BOOT	0x00f1	/* Clock status register	*/
#define CPC0_CR1	0x00f2	/* Chip Control 1 register */
#define CPC0_EPCTL	0x00f3	/* EMAC to PHY control register */
#define CPC0_PLLMR1	0x00f4	/* PLL mode  register 1	*/
#define CPC0_UCR	0x00f5	/* UART control register	*/
#define CPC0_SRR	0x00f6	/* Soft Reset register */
#define CPC0_PCI	0x00f9	/* PCI control register	*/

/* Defines for CPC0_EPCTL register */
#define CPC0_EPCTL_E0NFE	0x80000000
#define CPC0_EPCTL_E1NFE	0x40000000

/* Defines for CPC0_PCI Register */
#define CPC0_PCI_SPE		0x00000010	/* PCIINT/WE select	 */
#define CPC0_PCI_HOST_CFG_EN	0x00000008	/* PCI host config Enable */
#define CPC0_PCI_ARBIT_EN	0x00000001	/* PCI Internal Arb Enabled */

/* Defines for CPC0_BOOR Register */
#define CPC0_BOOT_SEP		0x00000002	/* serial EEPROM present */

/* Bit definitions */
#define PLLMR0_CPU_DIV_MASK	0x00300000	/* CPU clock divider */
#define PLLMR0_CPU_DIV_BYPASS	0x00000000
#define PLLMR0_CPU_DIV_2	0x00100000
#define PLLMR0_CPU_DIV_3	0x00200000
#define PLLMR0_CPU_DIV_4	0x00300000

#define PLLMR0_CPU_TO_PLB_MASK	0x00030000	/* CPU:PLB Frequency Divisor */
#define PLLMR0_CPU_PLB_DIV_1	0x00000000
#define PLLMR0_CPU_PLB_DIV_2	0x00010000
#define PLLMR0_CPU_PLB_DIV_3	0x00020000
#define PLLMR0_CPU_PLB_DIV_4	0x00030000

#define PLLMR0_OPB_TO_PLB_MASK	0x00003000	/* OPB:PLB Frequency Divisor */
#define PLLMR0_OPB_PLB_DIV_1	0x00000000
#define PLLMR0_OPB_PLB_DIV_2	0x00001000
#define PLLMR0_OPB_PLB_DIV_3	0x00002000
#define PLLMR0_OPB_PLB_DIV_4	0x00003000

#define PLLMR0_EXB_TO_PLB_MASK	0x00000300	/* External Bus:PLB Divisor */
#define PLLMR0_EXB_PLB_DIV_2	0x00000000
#define PLLMR0_EXB_PLB_DIV_3	0x00000100
#define PLLMR0_EXB_PLB_DIV_4	0x00000200
#define PLLMR0_EXB_PLB_DIV_5	0x00000300

#define PLLMR0_MAL_TO_PLB_MASK	0x00000030	/* MAL:PLB Divisor */
#define PLLMR0_MAL_PLB_DIV_1	0x00000000
#define PLLMR0_MAL_PLB_DIV_2	0x00000010
#define PLLMR0_MAL_PLB_DIV_3	0x00000020
#define PLLMR0_MAL_PLB_DIV_4	0x00000030

#define PLLMR0_PCI_TO_PLB_MASK	0x00000003	/* PCI:PLB Frequency Divisor */
#define PLLMR0_PCI_PLB_DIV_1	0x00000000
#define PLLMR0_PCI_PLB_DIV_2	0x00000001
#define PLLMR0_PCI_PLB_DIV_3	0x00000002
#define PLLMR0_PCI_PLB_DIV_4	0x00000003

#define PLLMR1_SSCS_MASK	0x80000000	/* Select system clock source */
#define PLLMR1_PLLR_MASK	0x40000000	/* PLL reset */
#define PLLMR1_FBMUL_MASK	0x00F00000	/* PLL feedback multiplier value */

#define PLLMR1_FWDVA_MASK	0x00070000	/* PLL forward divider A value */
#define PLLMR1_FWDVB_MASK	0x00007000	/* PLL forward divider B value */
#define PLLMR1_TUNING_MASK	0x000003FF	/* PLL tune bits */

/* Defines for CPC0_PLLMR1 Register fields */
#define PLL_ACTIVE		0x80000000
#define CPC0_PLLMR1_SSCS	0x80000000
#define PLL_RESET		0x40000000
#define CPC0_PLLMR1_PLLR	0x40000000
/* Feedback multiplier */
#define PLL_FBKDIV		0x00F00000
#define CPC0_PLLMR1_FBDV	0x00F00000
#define PLL_FBKDIV_16		0x00000000
#define PLL_FBKDIV_1		0x00100000
#define PLL_FBKDIV_2		0x00200000
#define PLL_FBKDIV_3		0x00300000
#define PLL_FBKDIV_4		0x00400000
#define PLL_FBKDIV_5		0x00500000
#define PLL_FBKDIV_6		0x00600000
#define PLL_FBKDIV_7		0x00700000
#define PLL_FBKDIV_8		0x00800000
#define PLL_FBKDIV_9		0x00900000
#define PLL_FBKDIV_10		0x00A00000
#define PLL_FBKDIV_11		0x00B00000
#define PLL_FBKDIV_12		0x00C00000
#define PLL_FBKDIV_13		0x00D00000
#define PLL_FBKDIV_14		0x00E00000
#define PLL_FBKDIV_15		0x00F00000
/* Forward A divisor */
#define PLL_FWDDIVA		0x00070000
#define CPC0_PLLMR1_FWDVA	0x00070000
#define PLL_FWDDIVA_8		0x00000000
#define PLL_FWDDIVA_7		0x00010000
#define PLL_FWDDIVA_6		0x00020000
#define PLL_FWDDIVA_5		0x00030000
#define PLL_FWDDIVA_4		0x00040000
#define PLL_FWDDIVA_3		0x00050000
#define PLL_FWDDIVA_2		0x00060000
#define PLL_FWDDIVA_1		0x00070000
/* Forward B divisor */
#define PLL_FWDDIVB		0x00007000
#define CPC0_PLLMR1_FWDVB	0x00007000
#define PLL_FWDDIVB_8		0x00000000
#define PLL_FWDDIVB_7		0x00001000
#define PLL_FWDDIVB_6		0x00002000
#define PLL_FWDDIVB_5		0x00003000
#define PLL_FWDDIVB_4		0x00004000
#define PLL_FWDDIVB_3		0x00005000
#define PLL_FWDDIVB_2		0x00006000
#define PLL_FWDDIVB_1		0x00007000
/* PLL tune bits */
#define PLL_TUNE_MASK		0x000003FF
#define PLL_TUNE_2_M_3		0x00000133	/*  2 <= M <= 3 */
#define PLL_TUNE_4_M_6		0x00000134	/*  3 <  M <= 6 */
#define PLL_TUNE_7_M_10		0x00000138	/*  6 <  M <= 10 */
#define PLL_TUNE_11_M_14	0x0000013C	/* 10 <  M <= 14 */
#define PLL_TUNE_15_M_40	0x0000023E	/* 14 <  M <= 40 */
#define PLL_TUNE_VCO_LOW	0x00000000	/* 500MHz <= VCO <=  800MHz */
#define PLL_TUNE_VCO_HI		0x00000080	/* 800MHz <  VCO <= 1000MHz */

/* Defines for CPC0_PLLMR0 Register fields */
/* CPU divisor */
#define PLL_CPUDIV		0x00300000
#define CPC0_PLLMR0_CCDV	0x00300000
#define PLL_CPUDIV_1		0x00000000
#define PLL_CPUDIV_2		0x00100000
#define PLL_CPUDIV_3		0x00200000
#define PLL_CPUDIV_4		0x00300000
/* PLB divisor */
#define PLL_PLBDIV		0x00030000
#define CPC0_PLLMR0_CBDV	0x00030000
#define PLL_PLBDIV_1		0x00000000
#define PLL_PLBDIV_2		0x00010000
#define PLL_PLBDIV_3		0x00020000
#define PLL_PLBDIV_4		0x00030000
/* OPB divisor */
#define PLL_OPBDIV		0x00003000
#define CPC0_PLLMR0_OPDV	0x00003000
#define PLL_OPBDIV_1		0x00000000
#define PLL_OPBDIV_2		0x00001000
#define PLL_OPBDIV_3		0x00002000
#define PLL_OPBDIV_4		0x00003000
/* EBC divisor */
#define PLL_EXTBUSDIV		0x00000300
#define CPC0_PLLMR0_EPDV	0x00000300
#define PLL_EXTBUSDIV_2		0x00000000
#define PLL_EXTBUSDIV_3		0x00000100
#define PLL_EXTBUSDIV_4		0x00000200
#define PLL_EXTBUSDIV_5		0x00000300
/* MAL divisor */
#define PLL_MALDIV		0x00000030
#define CPC0_PLLMR0_MPDV	0x00000030
#define PLL_MALDIV_1		0x00000000
#define PLL_MALDIV_2		0x00000010
#define PLL_MALDIV_3		0x00000020
#define PLL_MALDIV_4		0x00000030
/* PCI divisor */
#define PLL_PCIDIV		0x00000003
#define CPC0_PLLMR0_PPFD	0x00000003
#define PLL_PCIDIV_1		0x00000000
#define PLL_PCIDIV_2		0x00000001
#define PLL_PCIDIV_3		0x00000002
#define PLL_PCIDIV_4		0x00000003

/*
 * PLL settings for 266MHz CPU, 133MHz PLB/SDRAM, 66MHz EBC, 33MHz PCI,
 * assuming a 33.3MHz input clock to the 405EP.
 */
#define PLLMR0_266_133_66	(PLL_CPUDIV_1 | PLL_PLBDIV_2 |     \
				 PLL_OPBDIV_2 | PLL_EXTBUSDIV_2 |  \
				 PLL_MALDIV_1 | PLL_PCIDIV_4)
#define PLLMR1_266_133_66	(PLL_FBKDIV_8  |			\
				 PLL_FWDDIVA_3 | PLL_FWDDIVB_3 |	\
				 PLL_TUNE_15_M_40 | PLL_TUNE_VCO_LOW)

#define PLLMR0_133_66_66_33	(PLL_CPUDIV_1 | PLL_PLBDIV_1 |		\
				 PLL_OPBDIV_2 | PLL_EXTBUSDIV_4 |	\
				 PLL_MALDIV_1 | PLL_PCIDIV_4)
#define PLLMR1_133_66_66_33	(PLL_FBKDIV_4  |			\
				 PLL_FWDDIVA_6 | PLL_FWDDIVB_6 |	\
				 PLL_TUNE_15_M_40 | PLL_TUNE_VCO_LOW)
#define PLLMR0_200_100_50_33	(PLL_CPUDIV_1 | PLL_PLBDIV_2 |		\
				 PLL_OPBDIV_2 | PLL_EXTBUSDIV_3 |	\
				 PLL_MALDIV_1 | PLL_PCIDIV_4)
#define PLLMR1_200_100_50_33	(PLL_FBKDIV_6  |			\
				 PLL_FWDDIVA_4 | PLL_FWDDIVB_4 |	\
				 PLL_TUNE_15_M_40 | PLL_TUNE_VCO_LOW)
#define PLLMR0_266_133_66_33	(PLL_CPUDIV_1 | PLL_PLBDIV_2 |		\
				 PLL_OPBDIV_2 | PLL_EXTBUSDIV_4 |	\
				 PLL_MALDIV_1 | PLL_PCIDIV_4)
#define PLLMR1_266_133_66_33	(PLL_FBKDIV_8  |			\
				 PLL_FWDDIVA_3 | PLL_FWDDIVB_3 |	\
				 PLL_TUNE_15_M_40 | PLL_TUNE_VCO_LOW)
#define PLLMR0_266_66_33_33	(PLL_CPUDIV_1 | PLL_PLBDIV_4 |		\
				 PLL_OPBDIV_2 | PLL_EXTBUSDIV_2 |	\
				 PLL_MALDIV_1 | PLL_PCIDIV_2)
#define PLLMR1_266_66_33_33	(PLL_FBKDIV_8  |			\
				 PLL_FWDDIVA_3 | PLL_FWDDIVB_3 |	\
				 PLL_TUNE_15_M_40 | PLL_TUNE_VCO_LOW)
#define PLLMR0_333_111_55_37	(PLL_CPUDIV_1 | PLL_PLBDIV_3 |		\
				 PLL_OPBDIV_2 | PLL_EXTBUSDIV_2 |	\
				 PLL_MALDIV_1 | PLL_PCIDIV_3)
#define PLLMR1_333_111_55_37	(PLL_FBKDIV_10  |			\
				 PLL_FWDDIVA_3 | PLL_FWDDIVB_3 |	\
				 PLL_TUNE_15_M_40 | PLL_TUNE_VCO_HI)
#define PLLMR0_333_111_55_111	(PLL_CPUDIV_1 | PLL_PLBDIV_3 |		\
				 PLL_OPBDIV_2 | PLL_EXTBUSDIV_2 |	\
				 PLL_MALDIV_1 | PLL_PCIDIV_1)
#define PLLMR1_333_111_55_111	(PLL_FBKDIV_10  |			\
				 PLL_FWDDIVA_3 | PLL_FWDDIVB_3 |	\
				 PLL_TUNE_15_M_40 | PLL_TUNE_VCO_HI)

#endif /* _PPC405EP_H_ */
