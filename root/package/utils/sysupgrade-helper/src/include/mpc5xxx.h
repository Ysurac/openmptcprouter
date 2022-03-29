/*
 * include/asm-ppc/mpc5xxx.h
 *
 * Prototypes, etc. for the Motorola MPC5xxx
 * embedded cpu chips
 *
 * 2003 (c) MontaVista, Software, Inc.
 * Author: Dale Farnsworth <dfarnsworth@mvista.com>
 *
 * 2003 (C) Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
#ifndef __ASMPPC_MPC5XXX_H
#define __ASMPPC_MPC5XXX_H

#include <asm/types.h>

/* Processor name */
#define CPU_ID_STR	"MPC5200"

/* Exception offsets (PowerPC standard) */
#define EXC_OFF_SYS_RESET	0x0100
#define _START_OFFSET		EXC_OFF_SYS_RESET

/* useful macros for manipulating CSx_START/STOP */
#define START_REG(start)	((start) >> 16)
#define STOP_REG(start, size)	(((start) + (size) - 1) >> 16)

/* Internal memory map */

#define MPC5XXX_CS0_START	(CONFIG_SYS_MBAR + 0x0004)
#define MPC5XXX_CS0_STOP	(CONFIG_SYS_MBAR + 0x0008)
#define MPC5XXX_CS1_START	(CONFIG_SYS_MBAR + 0x000c)
#define MPC5XXX_CS1_STOP	(CONFIG_SYS_MBAR + 0x0010)
#define MPC5XXX_CS2_START	(CONFIG_SYS_MBAR + 0x0014)
#define MPC5XXX_CS2_STOP	(CONFIG_SYS_MBAR + 0x0018)
#define MPC5XXX_CS3_START	(CONFIG_SYS_MBAR + 0x001c)
#define MPC5XXX_CS3_STOP	(CONFIG_SYS_MBAR + 0x0020)
#define MPC5XXX_CS4_START	(CONFIG_SYS_MBAR + 0x0024)
#define MPC5XXX_CS4_STOP	(CONFIG_SYS_MBAR + 0x0028)
#define MPC5XXX_CS5_START	(CONFIG_SYS_MBAR + 0x002c)
#define MPC5XXX_CS5_STOP	(CONFIG_SYS_MBAR + 0x0030)
#define MPC5XXX_BOOTCS_START	(CONFIG_SYS_MBAR + 0x004c)
#define MPC5XXX_BOOTCS_STOP	(CONFIG_SYS_MBAR + 0x0050)
#define MPC5XXX_ADDECR		(CONFIG_SYS_MBAR + 0x0054)

#define MPC5XXX_CS6_START	(CONFIG_SYS_MBAR + 0x0058)
#define MPC5XXX_CS6_STOP	(CONFIG_SYS_MBAR + 0x005c)
#define MPC5XXX_CS7_START	(CONFIG_SYS_MBAR + 0x0060)
#define MPC5XXX_CS7_STOP	(CONFIG_SYS_MBAR + 0x0064)
#define MPC5XXX_SDRAM_CS0CFG	(CONFIG_SYS_MBAR + 0x0034)
#define MPC5XXX_SDRAM_CS1CFG	(CONFIG_SYS_MBAR + 0x0038)

#define MPC5XXX_SDRAM		(CONFIG_SYS_MBAR + 0x0100)
#define MPC5XXX_CDM		(CONFIG_SYS_MBAR + 0x0200)
#define MPC5XXX_LPB		(CONFIG_SYS_MBAR + 0x0300)
#define MPC5XXX_ICTL		(CONFIG_SYS_MBAR + 0x0500)
#define MPC5XXX_GPT		(CONFIG_SYS_MBAR + 0x0600)
#define MPC5XXX_GPIO		(CONFIG_SYS_MBAR + 0x0b00)
#define MPC5XXX_WU_GPIO         (CONFIG_SYS_MBAR + 0x0c00)
#define MPC5XXX_PCI		(CONFIG_SYS_MBAR + 0x0d00)
#define MPC5XXX_SPI		(CONFIG_SYS_MBAR + 0x0f00)
#define MPC5XXX_USB		(CONFIG_SYS_MBAR + 0x1000)
#define MPC5XXX_SDMA		(CONFIG_SYS_MBAR + 0x1200)
#define MPC5XXX_XLBARB		(CONFIG_SYS_MBAR + 0x1f00)

#define	MPC5XXX_PSC1		(CONFIG_SYS_MBAR + 0x2000)
#define	MPC5XXX_PSC2		(CONFIG_SYS_MBAR + 0x2200)
#define	MPC5XXX_PSC3		(CONFIG_SYS_MBAR + 0x2400)
#define	MPC5XXX_PSC4		(CONFIG_SYS_MBAR + 0x2600)
#define	MPC5XXX_PSC5		(CONFIG_SYS_MBAR + 0x2800)
#define	MPC5XXX_PSC6		(CONFIG_SYS_MBAR + 0x2c00)

#define	MPC5XXX_FEC		(CONFIG_SYS_MBAR + 0x3000)
#define MPC5XXX_ATA             (CONFIG_SYS_MBAR + 0x3A00)

#define MPC5XXX_I2C1		(CONFIG_SYS_MBAR + 0x3D00)
#define MPC5XXX_I2C2		(CONFIG_SYS_MBAR + 0x3D40)

#define MPC5XXX_SRAM		(CONFIG_SYS_MBAR + 0x8000)
#define MPC5XXX_SRAM_SIZE	(16*1024)

/* SDRAM Controller */
#define MPC5XXX_SDRAM_MODE	(MPC5XXX_SDRAM + 0x0000)
#define MPC5XXX_SDRAM_CTRL	(MPC5XXX_SDRAM + 0x0004)
#define MPC5XXX_SDRAM_CONFIG1	(MPC5XXX_SDRAM + 0x0008)
#define MPC5XXX_SDRAM_CONFIG2	(MPC5XXX_SDRAM + 0x000c)
#define MPC5XXX_SDRAM_SDELAY	(MPC5XXX_SDRAM + 0x0090)

/* Clock Distribution Module */
#define MPC5XXX_CDM_JTAGID	(MPC5XXX_CDM + 0x0000)
#define MPC5XXX_CDM_PORCFG	(MPC5XXX_CDM + 0x0004)
#define MPC5XXX_CDM_BRDCRMB  	(MPC5XXX_CDM + 0x0008)
#define MPC5XXX_CDM_CFG		(MPC5XXX_CDM + 0x000c)
#define MPC5XXX_CDM_48_FDC	(MPC5XXX_CDM + 0x0010)
#define MPC5XXX_CDM_CLK_ENA	(MPC5XXX_CDM + 0x0014)
#define MPC5XXX_CDM_SRESET	(MPC5XXX_CDM + 0x0020)

/* Local Plus Bus interface */
#define MPC5XXX_CS0_CFG		(MPC5XXX_LPB + 0x0000)
#define MPC5XXX_CS1_CFG		(MPC5XXX_LPB + 0x0004)
#define MPC5XXX_CS2_CFG		(MPC5XXX_LPB + 0x0008)
#define MPC5XXX_CS3_CFG		(MPC5XXX_LPB + 0x000c)
#define MPC5XXX_CS4_CFG		(MPC5XXX_LPB + 0x0010)
#define MPC5XXX_CS5_CFG		(MPC5XXX_LPB + 0x0014)
#define MPC5XXX_BOOTCS_CFG	MPC5XXX_CS0_CFG
#define MPC5XXX_CS_CTRL		(MPC5XXX_LPB + 0x0018)
#define MPC5XXX_CS_STATUS	(MPC5XXX_LPB + 0x001c)
#define MPC5XXX_CS6_CFG		(MPC5XXX_LPB + 0x0020)
#define MPC5XXX_CS7_CFG		(MPC5XXX_LPB + 0x0024)
#define MPC5XXX_CS_BURST	(MPC5XXX_LPB + 0x0028)
#define MPC5XXX_CS_DEADCYCLE	(MPC5XXX_LPB + 0x002c)

/* XLB Arbiter registers */
#define MPC5XXX_XLBARB_CFG		(MPC5XXX_XLBARB + 0x40)
#define MPC5XXX_XLBARB_MPRIEN	(MPC5XXX_XLBARB + 0x64)
#define MPC5XXX_XLBARB_MPRIVAL	(MPC5XXX_XLBARB + 0x68)

/* GPIO registers */
#define MPC5XXX_GPS_PORT_CONFIG	(MPC5XXX_GPIO + 0x0000)

/* Standard GPIO registers (simple, output only and simple interrupt */
#define MPC5XXX_GPIO_ENABLE     (MPC5XXX_GPIO + 0x0004)
#define MPC5XXX_GPIO_ODE        (MPC5XXX_GPIO + 0x0008)
#define MPC5XXX_GPIO_DIR        (MPC5XXX_GPIO + 0x000c)
#define MPC5XXX_GPIO_DATA_O     (MPC5XXX_GPIO + 0x0010)
#define MPC5XXX_GPIO_DATA_I     (MPC5XXX_GPIO + 0x0014)
#define MPC5XXX_GPIO_OO_ENABLE  (MPC5XXX_GPIO + 0x0018)
#define MPC5XXX_GPIO_OO_DATA    (MPC5XXX_GPIO + 0x001C)
#define MPC5XXX_GPIO_SI_ENABLE  (MPC5XXX_GPIO + 0x0020)
#define MPC5XXX_GPIO_SI_ODE     (MPC5XXX_GPIO + 0x0024)
#define MPC5XXX_GPIO_SI_DIR     (MPC5XXX_GPIO + 0x0028)
#define MPC5XXX_GPIO_SI_DATA    (MPC5XXX_GPIO + 0x002C)
#define MPC5XXX_GPIO_SI_IEN     (MPC5XXX_GPIO + 0x0030)
#define MPC5XXX_GPIO_SI_ITYPE   (MPC5XXX_GPIO + 0x0034)
#define MPC5XXX_GPIO_SI_MEN     (MPC5XXX_GPIO + 0x0038)
#define MPC5XXX_GPIO_SI_STATUS  (MPC5XXX_GPIO + 0x003C)

/* WakeUp GPIO registers */
#define MPC5XXX_WU_GPIO_ENABLE  (MPC5XXX_WU_GPIO + 0x0000)
#define MPC5XXX_WU_GPIO_ODE     (MPC5XXX_WU_GPIO + 0x0004)
#define MPC5XXX_WU_GPIO_DIR     (MPC5XXX_WU_GPIO + 0x0008)
#define MPC5XXX_WU_GPIO_DATA_O  (MPC5XXX_WU_GPIO + 0x000c)
#define MPC5XXX_WU_GPIO_DATA_I  (MPC5XXX_WU_GPIO + 0x0020)

/* GPIO pins, for Rev.B chip */
#define GPIO_WKUP_7		0x80000000UL
#define GPIO_PSC6_0		0x10000000UL
#define GPIO_PSC3_9		0x04000000UL
#define GPIO_PSC1_4		0x01000000UL
#define GPIO_PSC2_4		0x02000000UL

#define MPC5XXX_GPIO_SIMPLE_PSC6_3   0x20000000UL
#define MPC5XXX_GPIO_SIMPLE_PSC6_2   0x10000000UL
#define MPC5XXX_GPIO_SIMPLE_PSC3_7   0x00002000UL
#define MPC5XXX_GPIO_SIMPLE_PSC3_6   0x00001000UL
#define MPC5XXX_GPIO_SIMPLE_PSC3_3   0x00000800UL
#define MPC5XXX_GPIO_SIMPLE_PSC3_2   0x00000400UL
#define MPC5XXX_GPIO_SIMPLE_PSC3_1   0x00000200UL
#define MPC5XXX_GPIO_SIMPLE_PSC3_0   0x00000100UL
#define MPC5XXX_GPIO_SIMPLE_PSC2_3   0x00000080UL
#define MPC5XXX_GPIO_SIMPLE_PSC2_2   0x00000040UL
#define MPC5XXX_GPIO_SIMPLE_PSC2_1   0x00000020UL
#define MPC5XXX_GPIO_SIMPLE_PSC2_0   0x00000010UL
#define MPC5XXX_GPIO_SIMPLE_PSC1_3   0x00000008UL
#define MPC5XXX_GPIO_SIMPLE_PSC1_2   0x00000004UL
#define MPC5XXX_GPIO_SIMPLE_PSC1_1   0x00000002UL
#define MPC5XXX_GPIO_SIMPLE_PSC1_0   0x00000001UL

#define MPC5XXX_GPIO_SINT_ETH_16     0x80
#define MPC5XXX_GPIO_SINT_ETH_15     0x40
#define MPC5XXX_GPIO_SINT_ETH_14     0x20
#define MPC5XXX_GPIO_SINT_ETH_13     0x10
#define MPC5XXX_GPIO_SINT_USB1_9     0x08
#define MPC5XXX_GPIO_SINT_PSC3_8     0x04
#define MPC5XXX_GPIO_SINT_PSC3_5     0x02
#define MPC5XXX_GPIO_SINT_PSC3_4     0x01

#define MPC5XXX_GPIO_WKUP_7          0x80
#define MPC5XXX_GPIO_WKUP_6          0x40
#define MPC5XXX_GPIO_WKUP_PSC6_1     0x20
#define MPC5XXX_GPIO_WKUP_PSC6_0     0x10
#define MPC5XXX_GPIO_WKUP_ETH17      0x08
#define MPC5XXX_GPIO_WKUP_PSC3_9     0x04
#define MPC5XXX_GPIO_WKUP_PSC2_4     0x02
#define MPC5XXX_GPIO_WKUP_PSC1_4     0x01

/* PCI registers */
#define MPC5XXX_PCI_CMD		(MPC5XXX_PCI + 0x04)
#define MPC5XXX_PCI_CFG		(MPC5XXX_PCI + 0x0c)
#define MPC5XXX_PCI_BAR0	(MPC5XXX_PCI + 0x10)
#define MPC5XXX_PCI_BAR1	(MPC5XXX_PCI + 0x14)
#define MPC5XXX_PCI_GSCR	(MPC5XXX_PCI + 0x60)
#define MPC5XXX_PCI_TBATR0	(MPC5XXX_PCI + 0x64)
#define MPC5XXX_PCI_TBATR1	(MPC5XXX_PCI + 0x68)
#define MPC5XXX_PCI_TCR		(MPC5XXX_PCI + 0x6c)
#define MPC5XXX_PCI_IW0BTAR	(MPC5XXX_PCI + 0x70)
#define MPC5XXX_PCI_IW1BTAR	(MPC5XXX_PCI + 0x74)
#define MPC5XXX_PCI_IW2BTAR	(MPC5XXX_PCI + 0x78)
#define MPC5XXX_PCI_IWCR	(MPC5XXX_PCI + 0x80)
#define MPC5XXX_PCI_ICR		(MPC5XXX_PCI + 0x84)
#define MPC5XXX_PCI_ISR		(MPC5XXX_PCI + 0x88)
#define MPC5XXX_PCI_ARB		(MPC5XXX_PCI + 0x8c)
#define MPC5XXX_PCI_CAR		(MPC5XXX_PCI + 0xf8)

/* Interrupt Controller registers */
#define MPC5XXX_ICTL_PER_MASK	(MPC5XXX_ICTL + 0x0000)
#define MPC5XXX_ICTL_PER_PRIO1	(MPC5XXX_ICTL + 0x0004)
#define MPC5XXX_ICTL_PER_PRIO2	(MPC5XXX_ICTL + 0x0008)
#define MPC5XXX_ICTL_PER_PRIO3	(MPC5XXX_ICTL + 0x000c)
#define MPC5XXX_ICTL_EXT	(MPC5XXX_ICTL + 0x0010)
#define MPC5XXX_ICTL_CRIT	(MPC5XXX_ICTL + 0x0014)
#define MPC5XXX_ICTL_MAIN_PRIO1	(MPC5XXX_ICTL + 0x0018)
#define MPC5XXX_ICTL_MAIN_PRIO2	(MPC5XXX_ICTL + 0x001c)
#define MPC5XXX_ICTL_STS	(MPC5XXX_ICTL + 0x0024)
#define MPC5XXX_ICTL_CRIT_STS	(MPC5XXX_ICTL + 0x0028)
#define MPC5XXX_ICTL_MAIN_STS	(MPC5XXX_ICTL + 0x002c)
#define MPC5XXX_ICTL_PER_STS	(MPC5XXX_ICTL + 0x0030)
#define MPC5XXX_ICTL_BUS_STS	(MPC5XXX_ICTL + 0x0038)

#define NR_IRQS			64

/* IRQ mapping - these are our logical IRQ numbers */
#define MPC5XXX_CRIT_IRQ_NUM	4
#define MPC5XXX_MAIN_IRQ_NUM	17
#define MPC5XXX_SDMA_IRQ_NUM	17
#define MPC5XXX_PERP_IRQ_NUM	23

#define MPC5XXX_CRIT_IRQ_BASE	1
#define MPC5XXX_MAIN_IRQ_BASE	(MPC5XXX_CRIT_IRQ_BASE + MPC5XXX_CRIT_IRQ_NUM)
#define MPC5XXX_SDMA_IRQ_BASE	(MPC5XXX_MAIN_IRQ_BASE + MPC5XXX_MAIN_IRQ_NUM)
#define MPC5XXX_PERP_IRQ_BASE	(MPC5XXX_SDMA_IRQ_BASE + MPC5XXX_SDMA_IRQ_NUM)

#define MPC5XXX_IRQ0			(MPC5XXX_CRIT_IRQ_BASE + 0)
#define MPC5XXX_SLICE_TIMER_0_IRQ	(MPC5XXX_CRIT_IRQ_BASE + 1)
#define MPC5XXX_HI_INT_IRQ		(MPC5XXX_CRIT_IRQ_BASE + 2)
#define MPC5XXX_CCS_IRQ			(MPC5XXX_CRIT_IRQ_BASE + 3)

#define MPC5XXX_IRQ1			(MPC5XXX_MAIN_IRQ_BASE + 1)
#define MPC5XXX_IRQ2			(MPC5XXX_MAIN_IRQ_BASE + 2)
#define MPC5XXX_IRQ3			(MPC5XXX_MAIN_IRQ_BASE + 3)
#define MPC5XXX_RTC_PINT_IRQ		(MPC5XXX_MAIN_IRQ_BASE + 5)
#define MPC5XXX_RTC_SINT_IRQ		(MPC5XXX_MAIN_IRQ_BASE + 6)
#define MPC5XXX_RTC_GPIO_STD_IRQ	(MPC5XXX_MAIN_IRQ_BASE + 7)
#define MPC5XXX_RTC_GPIO_WKUP_IRQ	(MPC5XXX_MAIN_IRQ_BASE + 8)
#define MPC5XXX_TMR0_IRQ		(MPC5XXX_MAIN_IRQ_BASE + 9)
#define MPC5XXX_TMR1_IRQ		(MPC5XXX_MAIN_IRQ_BASE + 10)
#define MPC5XXX_TMR2_IRQ		(MPC5XXX_MAIN_IRQ_BASE + 11)
#define MPC5XXX_TMR3_IRQ		(MPC5XXX_MAIN_IRQ_BASE + 12)
#define MPC5XXX_TMR4_IRQ		(MPC5XXX_MAIN_IRQ_BASE + 13)
#define MPC5XXX_TMR5_IRQ		(MPC5XXX_MAIN_IRQ_BASE + 14)
#define MPC5XXX_TMR6_IRQ		(MPC5XXX_MAIN_IRQ_BASE + 15)
#define MPC5XXX_TMR7_IRQ		(MPC5XXX_MAIN_IRQ_BASE + 16)

#define MPC5XXX_SDMA_IRQ		(MPC5XXX_PERP_IRQ_BASE + 0)
#define MPC5XXX_PSC1_IRQ		(MPC5XXX_PERP_IRQ_BASE + 1)
#define MPC5XXX_PSC2_IRQ		(MPC5XXX_PERP_IRQ_BASE + 2)
#define MPC5XXX_PSC3_IRQ		(MPC5XXX_PERP_IRQ_BASE + 3)
#define MPC5XXX_PSC6_IRQ		(MPC5XXX_PERP_IRQ_BASE + 4)
#define MPC5XXX_IRDA_IRQ		(MPC5XXX_PERP_IRQ_BASE + 4)
#define MPC5XXX_FEC_IRQ			(MPC5XXX_PERP_IRQ_BASE + 5)
#define MPC5XXX_USB_IRQ			(MPC5XXX_PERP_IRQ_BASE + 6)
#define MPC5XXX_ATA_IRQ			(MPC5XXX_PERP_IRQ_BASE + 7)
#define MPC5XXX_PCI_CNTRL_IRQ		(MPC5XXX_PERP_IRQ_BASE + 8)
#define MPC5XXX_PCI_SCIRX_IRQ		(MPC5XXX_PERP_IRQ_BASE + 9)
#define MPC5XXX_PCI_SCITX_IRQ		(MPC5XXX_PERP_IRQ_BASE + 10)
#define MPC5XXX_PSC4_IRQ		(MPC5XXX_PERP_IRQ_BASE + 11)
#define MPC5XXX_PSC5_IRQ		(MPC5XXX_PERP_IRQ_BASE + 12)
#define MPC5XXX_SPI_MODF_IRQ		(MPC5XXX_PERP_IRQ_BASE + 13)
#define MPC5XXX_SPI_SPIF_IRQ		(MPC5XXX_PERP_IRQ_BASE + 14)
#define MPC5XXX_I2C1_IRQ		(MPC5XXX_PERP_IRQ_BASE + 15)
#define MPC5XXX_I2C2_IRQ		(MPC5XXX_PERP_IRQ_BASE + 16)
#define MPC5XXX_MSCAN1_IRQ		(MPC5XXX_PERP_IRQ_BASE + 17)
#define MPC5XXX_MSCAN2_IRQ		(MPC5XXX_PERP_IRQ_BASE + 18)
#define MPC5XXX_IR_RX_IRQ		(MPC5XXX_PERP_IRQ_BASE + 19)
#define MPC5XXX_IR_TX_IRQ		(MPC5XXX_PERP_IRQ_BASE + 20)
#define MPC5XXX_XLB_ARB_IRQ		(MPC5XXX_PERP_IRQ_BASE + 21)
#define MPC5XXX_BDLC_IRQ		(MPC5XXX_PERP_IRQ_BASE + 22)

/* General Purpose Timers registers */
#define MPC5XXX_GPT0_ENABLE		(MPC5XXX_GPT + 0x0)
#define MPC5XXX_GPT0_COUNTER		(MPC5XXX_GPT + 0x4)
#define MPC5XXX_GPT0_STATUS		(MPC5XXX_GPT + 0x0C)
#define MPC5XXX_GPT1_ENABLE		(MPC5XXX_GPT + 0x10)
#define MPC5XXX_GPT1_COUNTER		(MPC5XXX_GPT + 0x14)
#define MPC5XXX_GPT1_STATUS		(MPC5XXX_GPT + 0x1C)
#define MPC5XXX_GPT2_ENABLE		(MPC5XXX_GPT + 0x20)
#define MPC5XXX_GPT2_COUNTER		(MPC5XXX_GPT + 0x24)
#define MPC5XXX_GPT2_STATUS		(MPC5XXX_GPT + 0x2C)
#define MPC5XXX_GPT3_ENABLE		(MPC5XXX_GPT + 0x30)
#define MPC5XXX_GPT3_COUNTER		(MPC5XXX_GPT + 0x34)
#define MPC5XXX_GPT3_STATUS		(MPC5XXX_GPT + 0x3C)
#define MPC5XXX_GPT4_ENABLE		(MPC5XXX_GPT + 0x40)
#define MPC5XXX_GPT4_COUNTER		(MPC5XXX_GPT + 0x44)
#define MPC5XXX_GPT4_STATUS		(MPC5XXX_GPT + 0x4C)
#define MPC5XXX_GPT5_ENABLE		(MPC5XXX_GPT + 0x50)
#define MPC5XXX_GPT5_STATUS		(MPC5XXX_GPT + 0x5C)
#define MPC5XXX_GPT5_COUNTER		(MPC5XXX_GPT + 0x54)
#define MPC5XXX_GPT6_ENABLE		(MPC5XXX_GPT + 0x60)
#define MPC5XXX_GPT6_COUNTER		(MPC5XXX_GPT + 0x64)
#define MPC5XXX_GPT6_STATUS		(MPC5XXX_GPT + 0x6C)
#define MPC5XXX_GPT7_ENABLE		(MPC5XXX_GPT + 0x70)
#define MPC5XXX_GPT7_COUNTER		(MPC5XXX_GPT + 0x74)
#define MPC5XXX_GPT7_STATUS		(MPC5XXX_GPT + 0x7C)

#define MPC5XXX_GPT_GPIO_PIN(status)	((0x00000100 & (status)) >> 8)

#define MPC5XXX_GPT7_PWMCFG		(MPC5XXX_GPT + 0x78)

/* ATA registers */
#define MPC5XXX_ATA_HOST_CONFIG         (MPC5XXX_ATA + 0x0000)
#define MPC5XXX_ATA_PIO1                (MPC5XXX_ATA + 0x0008)
#define MPC5XXX_ATA_PIO2                (MPC5XXX_ATA + 0x000C)
#define MPC5XXX_ATA_SHARE_COUNT         (MPC5XXX_ATA + 0x002C)

/* I2Cn control register bits */
#define I2C_EN		0x80
#define I2C_IEN		0x40
#define I2C_STA		0x20
#define I2C_TX		0x10
#define I2C_TXAK	0x08
#define I2C_RSTA	0x04
#define I2C_INIT_MASK	(I2C_EN | I2C_STA | I2C_TX | I2C_RSTA)

/* I2Cn status register bits */
#define I2C_CF		0x80
#define I2C_AAS		0x40
#define I2C_BB		0x20
#define I2C_AL		0x10
#define I2C_SRW		0x04
#define I2C_IF		0x02
#define I2C_RXAK	0x01

/* SPI control register 1 bits */
#define SPI_CR_LSBFE	0x01
#define SPI_CR_SSOE	0x02
#define SPI_CR_CPHA	0x04
#define SPI_CR_CPOL	0x08
#define SPI_CR_MSTR	0x10
#define SPI_CR_SWOM	0x20
#define SPI_CR_SPE	0x40
#define SPI_CR_SPIE	0x80

/* SPI status register bits */
#define SPI_SR_MODF	0x10
#define SPI_SR_WCOL	0x40
#define SPI_SR_SPIF	0x80

/* SPI port data register bits */
#define SPI_PDR_SS	0x08

/* Programmable Serial Controller (PSC) status register bits */
#define PSC_SR_CDE		0x0080
#define PSC_SR_RXRDY		0x0100
#define PSC_SR_RXFULL		0x0200
#define PSC_SR_TXRDY		0x0400
#define PSC_SR_TXEMP		0x0800
#define PSC_SR_OE		0x1000
#define PSC_SR_PE		0x2000
#define PSC_SR_FE		0x4000
#define PSC_SR_RB		0x8000

/* PSC Command values */
#define PSC_RX_ENABLE		0x0001
#define PSC_RX_DISABLE		0x0002
#define PSC_TX_ENABLE		0x0004
#define PSC_TX_DISABLE		0x0008
#define PSC_SEL_MODE_REG_1	0x0010
#define PSC_RST_RX		0x0020
#define PSC_RST_TX		0x0030
#define PSC_RST_ERR_STAT	0x0040
#define PSC_RST_BRK_CHG_INT	0x0050
#define PSC_START_BRK		0x0060
#define PSC_STOP_BRK		0x0070

/* PSC Rx FIFO status bits */
#define PSC_RX_FIFO_ERR		0x0040
#define PSC_RX_FIFO_UF		0x0020
#define PSC_RX_FIFO_OF		0x0010
#define PSC_RX_FIFO_FR		0x0008
#define PSC_RX_FIFO_FULL	0x0004
#define PSC_RX_FIFO_ALARM	0x0002
#define PSC_RX_FIFO_EMPTY	0x0001

/* PSC interrupt mask bits */
#define PSC_IMR_TXRDY		0x0100
#define PSC_IMR_RXRDY		0x0200
#define PSC_IMR_DB		0x0400
#define PSC_IMR_IPC		0x8000

/* PSC input port change bits */
#define PSC_IPCR_CTS		0x01
#define PSC_IPCR_DCD		0x02

/* PSC mode fields */
#define PSC_MODE_5_BITS		0x00
#define PSC_MODE_6_BITS		0x01
#define PSC_MODE_7_BITS		0x02
#define PSC_MODE_8_BITS		0x03
#define PSC_MODE_PAREVEN	0x00
#define PSC_MODE_PARODD		0x04
#define PSC_MODE_PARFORCE	0x08
#define PSC_MODE_PARNONE	0x10
#define PSC_MODE_ERR		0x20
#define PSC_MODE_FFULL		0x40
#define PSC_MODE_RXRTS		0x80

#define PSC_MODE_ONE_STOP_5_BITS	0x00
#define PSC_MODE_ONE_STOP		0x07
#define PSC_MODE_TWO_STOP		0x0f

/* ATA config fields */
#define MPC5xxx_ATA_HOSTCONF_SMR	0x80000000UL	/* State machine
							   reset */
#define MPC5xxx_ATA_HOSTCONF_FR		0x40000000UL	/* FIFO Reset */
#define MPC5xxx_ATA_HOSTCONF_IE		0x02000000UL	/* Enable interrupt
							   in PIO */
#define MPC5xxx_ATA_HOSTCONF_IORDY	0x01000000UL	/* Drive supports
							   IORDY protocol */

#ifndef __ASSEMBLY__
/* Memory map registers */
struct mpc5xxx_mmap_ctl {
	volatile u32	mbar;
	volatile u32	cs0_start;	/* 0x0004 */
	volatile u32	cs0_stop;
	volatile u32	cs1_start;	/* 0x000c */
	volatile u32	cs1_stop;
	volatile u32	cs2_start;	/* 0x0014 */
	volatile u32	cs2_stop;
	volatile u32	cs3_start;	/* 0x001c */
	volatile u32	cs3_stop;
	volatile u32	cs4_start;	/* 0x0024 */
	volatile u32	cs4_stop;
	volatile u32	cs5_start;	/* 0x002c */
	volatile u32	cs5_stop;
	volatile u32	sdram0;		/* 0x0034 */
	volatile u32	sdram1;		/* 0x0038 */
	volatile u32	dummy1[4];	/* 0x003c */
	volatile u32	boot_start;	/* 0x004c */
	volatile u32	boot_stop;
	volatile u32	ipbi_ws_ctrl;	/* 0x0054 */
	volatile u32	cs6_start;	/* 0x0058 */
	volatile u32	cs6_stop;
	volatile u32	cs7_start;	/* 0x0060 */
	volatile u32	cs7_stop;
};

/* Clock distribution module */
struct mpc5xxx_cdm {
	volatile u32	jtagid;		/* 0x0000 */
	volatile u32	porcfg;
	volatile u32	brdcrmb;	/* 0x0008 */
	volatile u32	cfg;
	volatile u32	fourtyeight_fdc;/* 0x0010 */
	volatile u32	clock_enable;
	volatile u32	system_osc;	/* 0x0018 */
	volatile u32	ccscr;
	volatile u32	sreset;		/* 0x0020 */
	volatile u32	pll_status;
	volatile u32	psc1_mccr;	/* 0x0028 */
	volatile u32	psc2_mccr;
	volatile u32	psc3_mccr;	/* 0x0030 */
	volatile u32	psc6_mccr;
};

/* SDRAM controller */
struct mpc5xxx_sdram {
	volatile u32	mode;
	volatile u32	ctrl;
	volatile u32	config1;
	volatile u32	config2;
	volatile u32	dummy[32];
	volatile u32	sdelay;
};

struct mpc5xxx_lpb {
	volatile u32	cs0_cfg;
	volatile u32	cs1_cfg;
	volatile u32	cs2_cfg;
	volatile u32	cs3_cfg;
	volatile u32	cs4_cfg;
	volatile u32	cs5_cfg;
	volatile u32	cs_ctrl;
	volatile u32	cs_status;
	volatile u32	cs6_cfg;
	volatile u32	cs7_cfg;
	volatile u32	cs_burst;
	volatile u32	cs_deadcycle;
};


struct mpc5xxx_psc {
	volatile u8	mode;		/* PSC + 0x00 */
	volatile u8	reserved0[3];
	union {				/* PSC + 0x04 */
		volatile u16	status;
		volatile u16	clock_select;
	} sr_csr;
#define psc_status	sr_csr.status
#define psc_clock_select sr_csr.clock_select
	volatile u16	reserved1;
	volatile u8	command;	/* PSC + 0x08 */
	volatile u8	reserved2[3];
	union {				/* PSC + 0x0c */
		volatile u8	buffer_8;
		volatile u16	buffer_16;
		volatile u32	buffer_32;
	} buffer;
#define psc_buffer_8	buffer.buffer_8
#define psc_buffer_16	buffer.buffer_16
#define psc_buffer_32	buffer.buffer_32
	union {				/* PSC + 0x10 */
		volatile u8	ipcr;
		volatile u8	acr;
	} ipcr_acr;
#define psc_ipcr	ipcr_acr.ipcr
#define psc_acr		ipcr_acr.acr
	volatile u8	reserved3[3];
	union {				/* PSC + 0x14 */
		volatile u16	isr;
		volatile u16	imr;
	} isr_imr;
#define psc_isr		isr_imr.isr
#define psc_imr		isr_imr.imr
	volatile u16	reserved4;
	volatile u8	ctur;		/* PSC + 0x18 */
	volatile u8	reserved5[3];
	volatile u8	ctlr;		/* PSC + 0x1c */
	volatile u8	reserved6[3];
	volatile u16	ccr;		/* PSC + 0x20 */
	volatile u8	reserved7[14];
	volatile u8	ivr;		/* PSC + 0x30 */
	volatile u8	reserved8[3];
	volatile u8	ip;		/* PSC + 0x34 */
	volatile u8	reserved9[3];
	volatile u8	op1;		/* PSC + 0x38 */
	volatile u8	reserved10[3];
	volatile u8	op0;		/* PSC + 0x3c */
	volatile u8	reserved11[3];
	volatile u32	sicr;		/* PSC + 0x40 */
	volatile u8	ircr1;		/* PSC + 0x44 */
	volatile u8	reserved12[3];
	volatile u8	ircr2;		/* PSC + 0x44 */
	volatile u8	reserved13[3];
	volatile u8	irsdr;		/* PSC + 0x4c */
	volatile u8	reserved14[3];
	volatile u8	irmdr;		/* PSC + 0x50 */
	volatile u8	reserved15[3];
	volatile u8	irfdr;		/* PSC + 0x54 */
	volatile u8	reserved16[3];
	volatile u16	rfnum;		/* PSC + 0x58 */
	volatile u16	reserved17;
	volatile u16	tfnum;		/* PSC + 0x5c */
	volatile u16	reserved18;
	volatile u32	rfdata;		/* PSC + 0x60 */
	volatile u16	rfstat;		/* PSC + 0x64 */
	volatile u16	reserved20;
	volatile u8	rfcntl;		/* PSC + 0x68 */
	volatile u8	reserved21[5];
	volatile u16	rfalarm;	/* PSC + 0x6e */
	volatile u16	reserved22;
	volatile u16	rfrptr;		/* PSC + 0x72 */
	volatile u16	reserved23;
	volatile u16	rfwptr;		/* PSC + 0x76 */
	volatile u16	reserved24;
	volatile u16	rflrfptr;	/* PSC + 0x7a */
	volatile u16	reserved25;
	volatile u16	rflwfptr;	/* PSC + 0x7e */
	volatile u32	tfdata;		/* PSC + 0x80 */
	volatile u16	tfstat;		/* PSC + 0x84 */
	volatile u16	reserved26;
	volatile u8	tfcntl;		/* PSC + 0x88 */
	volatile u8	reserved27[5];
	volatile u16	tfalarm;	/* PSC + 0x8e */
	volatile u16	reserved28;
	volatile u16	tfrptr;		/* PSC + 0x92 */
	volatile u16	reserved29;
	volatile u16	tfwptr;		/* PSC + 0x96 */
	volatile u16	reserved30;
	volatile u16	tflrfptr;	/* PSC + 0x9a */
	volatile u16	reserved31;
	volatile u16	tflwfptr;	/* PSC + 0x9e */
};

struct mpc5xxx_intr {
	volatile u32	per_mask;	/* INTR + 0x00 */
	volatile u32	per_pri1;	/* INTR + 0x04 */
	volatile u32	per_pri2;	/* INTR + 0x08 */
	volatile u32	per_pri3;	/* INTR + 0x0c */
	volatile u32	ctrl;		/* INTR + 0x10 */
	volatile u32	main_mask;	/* INTR + 0x14 */
	volatile u32	main_pri1;	/* INTR + 0x18 */
	volatile u32	main_pri2;	/* INTR + 0x1c */
	volatile u32	reserved1;	/* INTR + 0x20 */
	volatile u32	enc_status;	/* INTR + 0x24 */
	volatile u32	crit_status;	/* INTR + 0x28 */
	volatile u32	main_status;	/* INTR + 0x2c */
	volatile u32	per_status;	/* INTR + 0x30 */
	volatile u32	reserved2;	/* INTR + 0x34 */
	volatile u32	per_error;	/* INTR + 0x38 */
};

struct mpc5xxx_gpio {
	volatile u32 port_config;	/* GPIO + 0x00 */
	volatile u32 simple_gpioe;	/* GPIO + 0x04 */
	volatile u32 simple_ode;	/* GPIO + 0x08 */
	volatile u32 simple_ddr;	/* GPIO + 0x0c */
	volatile u32 simple_dvo;	/* GPIO + 0x10 */
	volatile u32 simple_ival;	/* GPIO + 0x14 */
	volatile u8 outo_gpioe;		/* GPIO + 0x18 */
	volatile u8 reserved1[3];	/* GPIO + 0x19 */
	volatile u8 outo_dvo;		/* GPIO + 0x1c */
	volatile u8 reserved2[3];	/* GPIO + 0x1d */
	volatile u8 sint_gpioe;		/* GPIO + 0x20 */
	volatile u8 reserved3[3];	/* GPIO + 0x21 */
	volatile u8 sint_ode;		/* GPIO + 0x24 */
	volatile u8 reserved4[3];	/* GPIO + 0x25 */
	volatile u8 sint_ddr;		/* GPIO + 0x28 */
	volatile u8 reserved5[3];	/* GPIO + 0x29 */
	volatile u8 sint_dvo;		/* GPIO + 0x2c */
	volatile u8 reserved6[3];	/* GPIO + 0x2d */
	volatile u8 sint_inten;		/* GPIO + 0x30 */
	volatile u8 reserved7[3];	/* GPIO + 0x31 */
	volatile u16 sint_itype;	/* GPIO + 0x34 */
	volatile u16 reserved8;		/* GPIO + 0x36 */
	volatile u8 gpio_control;	/* GPIO + 0x38 */
	volatile u8 reserved9[3];	/* GPIO + 0x39 */
	volatile u8 sint_istat;		/* GPIO + 0x3c */
	volatile u8 sint_ival;		/* GPIO + 0x3d */
	volatile u8 bus_errs;		/* GPIO + 0x3e */
	volatile u8 reserved10;		/* GPIO + 0x3f */
};

struct mpc5xxx_wu_gpio {
	volatile u8 enable;		/* WU_GPIO + 0x00 */
	volatile u8 reserved1[3];	/* WU_GPIO + 0x01 */
	volatile u8 ode;		/* WU_GPIO + 0x04 */
	volatile u8 reserved2[3];	/* WU_GPIO + 0x05 */
	volatile u8 ddr;		/* WU_GPIO + 0x08 */
	volatile u8 reserved3[3];	/* WU_GPIO + 0x09 */
	volatile u8 dvo;		/* WU_GPIO + 0x0c */
	volatile u8 reserved4[3];	/* WU_GPIO + 0x0d */
	volatile u8 inten;		/* WU_GPIO + 0x10 */
	volatile u8 reserved5[3];	/* WU_GPIO + 0x11 */
	volatile u8 iinten;		/* WU_GPIO + 0x14 */
	volatile u8 reserved6[3];	/* WU_GPIO + 0x15 */
	volatile u16 itype;		/* WU_GPIO + 0x18 */
	volatile u8 reserved7[2];	/* WU_GPIO + 0x1a */
	volatile u8 master_enable;	/* WU_GPIO + 0x1c */
	volatile u8 reserved8[3];	/* WU_GPIO + 0x1d */
	volatile u8 ival;		/* WU_GPIO + 0x20 */
	volatile u8 reserved9[3];	/* WU_GPIO + 0x21 */
	volatile u8 status;		/* WU_GPIO + 0x24 */
	volatile u8 reserved10[3];	/* WU_GPIO + 0x25 */
};

struct mpc5xxx_sdma {
	volatile u32 taskBar;		/* SDMA + 0x00 */
	volatile u32 currentPointer;	/* SDMA + 0x04 */
	volatile u32 endPointer;	/* SDMA + 0x08 */
	volatile u32 variablePointer;	/* SDMA + 0x0c */

	volatile u8 IntVect1;		/* SDMA + 0x10 */
	volatile u8 IntVect2;		/* SDMA + 0x11 */
	volatile u16 PtdCntrl;		/* SDMA + 0x12 */

	volatile u32 IntPend;		/* SDMA + 0x14 */
	volatile u32 IntMask;		/* SDMA + 0x18 */

	volatile u16 tcr_0;		/* SDMA + 0x1c */
	volatile u16 tcr_1;		/* SDMA + 0x1e */
	volatile u16 tcr_2;		/* SDMA + 0x20 */
	volatile u16 tcr_3;		/* SDMA + 0x22 */
	volatile u16 tcr_4;		/* SDMA + 0x24 */
	volatile u16 tcr_5;		/* SDMA + 0x26 */
	volatile u16 tcr_6;		/* SDMA + 0x28 */
	volatile u16 tcr_7;		/* SDMA + 0x2a */
	volatile u16 tcr_8;		/* SDMA + 0x2c */
	volatile u16 tcr_9;		/* SDMA + 0x2e */
	volatile u16 tcr_a;		/* SDMA + 0x30 */
	volatile u16 tcr_b;		/* SDMA + 0x32 */
	volatile u16 tcr_c;		/* SDMA + 0x34 */
	volatile u16 tcr_d;		/* SDMA + 0x36 */
	volatile u16 tcr_e;		/* SDMA + 0x38 */
	volatile u16 tcr_f;		/* SDMA + 0x3a */

	volatile u8 IPR0;		/* SDMA + 0x3c */
	volatile u8 IPR1;		/* SDMA + 0x3d */
	volatile u8 IPR2;		/* SDMA + 0x3e */
	volatile u8 IPR3;		/* SDMA + 0x3f */
	volatile u8 IPR4;		/* SDMA + 0x40 */
	volatile u8 IPR5;		/* SDMA + 0x41 */
	volatile u8 IPR6;		/* SDMA + 0x42 */
	volatile u8 IPR7;		/* SDMA + 0x43 */
	volatile u8 IPR8;		/* SDMA + 0x44 */
	volatile u8 IPR9;		/* SDMA + 0x45 */
	volatile u8 IPR10;		/* SDMA + 0x46 */
	volatile u8 IPR11;		/* SDMA + 0x47 */
	volatile u8 IPR12;		/* SDMA + 0x48 */
	volatile u8 IPR13;		/* SDMA + 0x49 */
	volatile u8 IPR14;		/* SDMA + 0x4a */
	volatile u8 IPR15;		/* SDMA + 0x4b */
	volatile u8 IPR16;		/* SDMA + 0x4c */
	volatile u8 IPR17;		/* SDMA + 0x4d */
	volatile u8 IPR18;		/* SDMA + 0x4e */
	volatile u8 IPR19;		/* SDMA + 0x4f */
	volatile u8 IPR20;		/* SDMA + 0x50 */
	volatile u8 IPR21;		/* SDMA + 0x51 */
	volatile u8 IPR22;		/* SDMA + 0x52 */
	volatile u8 IPR23;		/* SDMA + 0x53 */
	volatile u8 IPR24;		/* SDMA + 0x54 */
	volatile u8 IPR25;		/* SDMA + 0x55 */
	volatile u8 IPR26;		/* SDMA + 0x56 */
	volatile u8 IPR27;		/* SDMA + 0x57 */
	volatile u8 IPR28;		/* SDMA + 0x58 */
	volatile u8 IPR29;		/* SDMA + 0x59 */
	volatile u8 IPR30;		/* SDMA + 0x5a */
	volatile u8 IPR31;		/* SDMA + 0x5b */

	volatile u32 res1;		/* SDMA + 0x5c */
	volatile u32 res2;		/* SDMA + 0x60 */
	volatile u32 res3;		/* SDMA + 0x64 */
	volatile u32 MDEDebug;		/* SDMA + 0x68 */
	volatile u32 ADSDebug;		/* SDMA + 0x6c */
	volatile u32 Value1;		/* SDMA + 0x70 */
	volatile u32 Value2;		/* SDMA + 0x74 */
	volatile u32 Control;		/* SDMA + 0x78 */
	volatile u32 Status;		/* SDMA + 0x7c */
	volatile u32 EU00;		/* SDMA + 0x80 */
	volatile u32 EU01;		/* SDMA + 0x84 */
	volatile u32 EU02;		/* SDMA + 0x88 */
	volatile u32 EU03;		/* SDMA + 0x8c */
	volatile u32 EU04;		/* SDMA + 0x90 */
	volatile u32 EU05;		/* SDMA + 0x94 */
	volatile u32 EU06;		/* SDMA + 0x98 */
	volatile u32 EU07;		/* SDMA + 0x9c */
	volatile u32 EU10;		/* SDMA + 0xa0 */
	volatile u32 EU11;		/* SDMA + 0xa4 */
	volatile u32 EU12;		/* SDMA + 0xa8 */
	volatile u32 EU13;		/* SDMA + 0xac */
	volatile u32 EU14;		/* SDMA + 0xb0 */
	volatile u32 EU15;		/* SDMA + 0xb4 */
	volatile u32 EU16;		/* SDMA + 0xb8 */
	volatile u32 EU17;		/* SDMA + 0xbc */
	volatile u32 EU20;		/* SDMA + 0xc0 */
	volatile u32 EU21;		/* SDMA + 0xc4 */
	volatile u32 EU22;		/* SDMA + 0xc8 */
	volatile u32 EU23;		/* SDMA + 0xcc */
	volatile u32 EU24;		/* SDMA + 0xd0 */
	volatile u32 EU25;		/* SDMA + 0xd4 */
	volatile u32 EU26;		/* SDMA + 0xd8 */
	volatile u32 EU27;		/* SDMA + 0xdc */
	volatile u32 EU30;		/* SDMA + 0xe0 */
	volatile u32 EU31;		/* SDMA + 0xe4 */
	volatile u32 EU32;		/* SDMA + 0xe8 */
	volatile u32 EU33;		/* SDMA + 0xec */
	volatile u32 EU34;		/* SDMA + 0xf0 */
	volatile u32 EU35;		/* SDMA + 0xf4 */
	volatile u32 EU36;		/* SDMA + 0xf8 */
	volatile u32 EU37;		/* SDMA + 0xfc */
};

struct mpc5xxx_i2c {
	volatile u32 madr;		/* I2Cn + 0x00 */
	volatile u32 mfdr;		/* I2Cn + 0x04 */
	volatile u32 mcr;		/* I2Cn + 0x08 */
	volatile u32 msr;		/* I2Cn + 0x0C */
	volatile u32 mdr;		/* I2Cn + 0x10 */
};

struct mpc5xxx_spi {
	volatile u8 cr1;		/* SPI + 0x0F00 */
	volatile u8 cr2;		/* SPI + 0x0F01 */
	volatile u8 reserved1[2];
	volatile u8 brr;		/* SPI + 0x0F04 */
	volatile u8 sr;			/* SPI + 0x0F05 */
	volatile u8 reserved2[3];
	volatile u8 dr;			/* SPI + 0x0F09 */
	volatile u8 reserved3[3];
	volatile u8 pdr;		/* SPI + 0x0F0D */
	volatile u8 reserved4[2];
	volatile u8 ddr;		/* SPI + 0x0F10 */
};


struct mpc5xxx_gpt {
	volatile u32 emsr;		/* GPT + Timer# * 0x10 + 0x00 */
	volatile u32 cir;		/* GPT + Timer# * 0x10 + 0x04 */
	volatile u32 pwmcr;		/* GPT + Timer# * 0x10 + 0x08 */
	volatile u32 sr;		/* GPT + Timer# * 0x10 + 0x0c */
};

struct mpc5xxx_gpt_0_7 {
	struct mpc5xxx_gpt gpt0;
	struct mpc5xxx_gpt gpt1;
	struct mpc5xxx_gpt gpt2;
	struct mpc5xxx_gpt gpt3;
	struct mpc5xxx_gpt gpt4;
	struct mpc5xxx_gpt gpt5;
	struct mpc5xxx_gpt gpt6;
	struct mpc5xxx_gpt gpt7;
};

struct mscan_buffer {
	volatile u8  idr[0x8];          /* 0x00 */
	volatile u8  dsr[0x10];         /* 0x08 */
	volatile u8  dlr;               /* 0x18 */
	volatile u8  tbpr;              /* 0x19 */      /* This register is not applicable for receive buffers */
	volatile u16 rsrv1;             /* 0x1A */
	volatile u8  tsrh;              /* 0x1C */
	volatile u8  tsrl;              /* 0x1D */
	volatile u16 rsrv2;             /* 0x1E */
};

struct mpc5xxx_mscan {
	volatile u8  canctl0;           /* MSCAN + 0x00 */
	volatile u8  canctl1;           /* MSCAN + 0x01 */
	volatile u16 rsrv1;             /* MSCAN + 0x02 */
	volatile u8  canbtr0;           /* MSCAN + 0x04 */
	volatile u8  canbtr1;           /* MSCAN + 0x05 */
	volatile u16 rsrv2;             /* MSCAN + 0x06 */
	volatile u8  canrflg;           /* MSCAN + 0x08 */
	volatile u8  canrier;           /* MSCAN + 0x09 */
	volatile u16 rsrv3;             /* MSCAN + 0x0A */
	volatile u8  cantflg;           /* MSCAN + 0x0C */
	volatile u8  cantier;           /* MSCAN + 0x0D */
	volatile u16 rsrv4;             /* MSCAN + 0x0E */
	volatile u8  cantarq;           /* MSCAN + 0x10 */
	volatile u8  cantaak;           /* MSCAN + 0x11 */
	volatile u16 rsrv5;             /* MSCAN + 0x12 */
	volatile u8  cantbsel;          /* MSCAN + 0x14 */
	volatile u8  canidac;           /* MSCAN + 0x15 */
	volatile u16 rsrv6[3];          /* MSCAN + 0x16 */
	volatile u8  canrxerr;          /* MSCAN + 0x1C */
	volatile u8  cantxerr;          /* MSCAN + 0x1D */
	volatile u16 rsrv7;             /* MSCAN + 0x1E */
	volatile u8  canidar0;          /* MSCAN + 0x20 */
	volatile u8  canidar1;          /* MSCAN + 0x21 */
	volatile u16 rsrv8;             /* MSCAN + 0x22 */
	volatile u8  canidar2;          /* MSCAN + 0x24 */
	volatile u8  canidar3;          /* MSCAN + 0x25 */
	volatile u16 rsrv9;             /* MSCAN + 0x26 */
	volatile u8  canidmr0;          /* MSCAN + 0x28 */
	volatile u8  canidmr1;          /* MSCAN + 0x29 */
	volatile u16 rsrv10;            /* MSCAN + 0x2A */
	volatile u8  canidmr2;          /* MSCAN + 0x2C */
	volatile u8  canidmr3;          /* MSCAN + 0x2D */
	volatile u16 rsrv11;            /* MSCAN + 0x2E */
	volatile u8  canidar4;          /* MSCAN + 0x30 */
	volatile u8  canidar5;          /* MSCAN + 0x31 */
	volatile u16 rsrv12;            /* MSCAN + 0x32 */
	volatile u8  canidar6;          /* MSCAN + 0x34 */
	volatile u8  canidar7;          /* MSCAN + 0x35 */
	volatile u16 rsrv13;            /* MSCAN + 0x36 */
	volatile u8  canidmr4;          /* MSCAN + 0x38 */
	volatile u8  canidmr5;          /* MSCAN + 0x39 */
	volatile u16 rsrv14;            /* MSCAN + 0x3A */
	volatile u8  canidmr6;          /* MSCAN + 0x3C */
	volatile u8  canidmr7;          /* MSCAN + 0x3D */
	volatile u16 rsrv15;            /* MSCAN + 0x3E */

	struct mscan_buffer canrxfg;    /* MSCAN + 0x40 */    /* Foreground receive buffer */
	struct mscan_buffer cantxfg;    /* MSCAN + 0x60 */    /* Foreground transmit buffer */
	};

struct mpc5xxx_xlb {
	volatile u8 reserved[0x40];	/* XLB + 0x00 */
	volatile u32 config;		/* XLB + 0x40 */
	volatile u32 version;		/* XLB + 0x44 */
	volatile u32 status;		/* XLB + 0x48 */
	volatile u32 int_enable;	/* XLB + 0x4c */
	volatile u32 addr_capture;	/* XLB + 0x50 */
	volatile u32 bus_sig_capture;	/* XLB + 0x54 */
	volatile u32 addr_timeout;	/* XLB + 0x58 */
	volatile u32 data_timeout;	/* XLB + 0x5c */
	volatile u32 bus_act_timeout;	/* XLB + 0x60 */
	volatile u32 master_pri_enable; /* XLB + 0x64 */
	volatile u32 master_priority;	/* XLB + 0x68 */
	volatile u32 base_address;	/* XLB + 0x6c */
	volatile u32 snoop_window;	/* XLB + 0x70 */
};

/* function prototypes */
void loadtask(int basetask, int tasks);

#endif /* __ASSEMBLY__ */

#endif /* __ASMPPC_MPC5XXX_H */
