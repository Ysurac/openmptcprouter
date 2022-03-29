/*
 * ColdFire Internal Memory Map and Defines
 *
 * Copyright (C) 2004-2007 Freescale Semiconductor, Inc.
 * TsiChung Liew (Tsi-Chung.Liew@freescale.com)
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

#ifndef __IMMAP_H
#define __IMMAP_H

#if defined(CONFIG_MCF520x)
#include <asm/immap_520x.h>
#include <asm/m520x.h>

#define CONFIG_SYS_FEC0_IOBASE		(MMAP_FEC0)
#define CONFIG_SYS_UART_BASE		(MMAP_UART0 + (CONFIG_SYS_UART_PORT * 0x4000))

/* Timer */
#ifdef CONFIG_MCFTMR
#define CONFIG_SYS_UDELAY_BASE		(MMAP_DTMR0)
#define CONFIG_SYS_TMR_BASE		(MMAP_DTMR1)
#define CONFIG_SYS_TMRPND_REG		(((volatile int0_t *)(CONFIG_SYS_INTR_BASE))->iprh0)
#define CONFIG_SYS_TMRINTR_NO		(INT0_HI_DTMR1)
#define CONFIG_SYS_TMRINTR_MASK		(INTC_IPRH_INT33)
#define CONFIG_SYS_TMRINTR_PEND		(CONFIG_SYS_TMRINTR_MASK)
#define CONFIG_SYS_TMRINTR_PRI		(6)
#define CONFIG_SYS_TIMER_PRESCALER	(((gd->bus_clk / 1000000) - 1) << 8)
#endif

#ifdef CONFIG_MCFPIT
#define CONFIG_SYS_UDELAY_BASE		(MMAP_PIT0)
#define CONFIG_SYS_PIT_BASE		(MMAP_PIT1)
#define CONFIG_SYS_PIT_PRESCALE	(6)
#endif

#define CONFIG_SYS_INTR_BASE		(MMAP_INTC0)
#define CONFIG_SYS_NUM_IRQS		(128)
#endif				/* CONFIG_M520x */

#ifdef CONFIG_M52277
#include <asm/immap_5227x.h>
#include <asm/m5227x.h>

#define CONFIG_SYS_UART_BASE		(MMAP_UART0 + (CONFIG_SYS_UART_PORT * 0x4000))

#define CONFIG_SYS_MCFRTC_BASE		(MMAP_RTC)

#ifdef CONFIG_LCD
#define	CONFIG_SYS_LCD_BASE		(MMAP_LCD)
#endif

/* Timer */
#ifdef CONFIG_MCFTMR
#define CONFIG_SYS_UDELAY_BASE		(MMAP_DTMR0)
#define CONFIG_SYS_TMR_BASE		(MMAP_DTMR1)
#define CONFIG_SYS_TMRPND_REG		(((volatile int0_t *)(CONFIG_SYS_INTR_BASE))->iprh0)
#define CONFIG_SYS_TMRINTR_NO		(INT0_HI_DTMR1)
#define CONFIG_SYS_TMRINTR_MASK	(INTC_IPRH_INT33)
#define CONFIG_SYS_TMRINTR_PEND	(CONFIG_SYS_TMRINTR_MASK)
#define CONFIG_SYS_TMRINTR_PRI		(6)
#define CONFIG_SYS_TIMER_PRESCALER	(((gd->bus_clk / 1000000) - 1) << 8)
#endif

#ifdef CONFIG_MCFPIT
#define CONFIG_SYS_UDELAY_BASE		(MMAP_PIT0)
#define CONFIG_SYS_PIT_BASE		(MMAP_PIT1)
#define CONFIG_SYS_PIT_PRESCALE	(6)
#endif

#define CONFIG_SYS_INTR_BASE		(MMAP_INTC0)
#define CONFIG_SYS_NUM_IRQS		(128)
#endif				/* CONFIG_M52277 */

#ifdef CONFIG_M5235
#include <asm/immap_5235.h>
#include <asm/m5235.h>

#define CONFIG_SYS_FEC0_IOBASE		(MMAP_FEC)
#define CONFIG_SYS_UART_BASE		(MMAP_UART0 + (CONFIG_SYS_UART_PORT * 0x40))

/* Timer */
#ifdef CONFIG_MCFTMR
#define CONFIG_SYS_UDELAY_BASE		(MMAP_DTMR0)
#define CONFIG_SYS_TMR_BASE		(MMAP_DTMR3)
#define CONFIG_SYS_TMRPND_REG		(((volatile int0_t *)(CONFIG_SYS_INTR_BASE))->iprl0)
#define CONFIG_SYS_TMRINTR_NO		(INT0_LO_DTMR3)
#define CONFIG_SYS_TMRINTR_MASK	(INTC_IPRL_INT22)
#define CONFIG_SYS_TMRINTR_PEND	(CONFIG_SYS_TMRINTR_MASK)
#define CONFIG_SYS_TMRINTR_PRI		(0x1E)		/* Level must include inorder to work */
#define CONFIG_SYS_TIMER_PRESCALER	(((gd->bus_clk / 1000000) - 1) << 8)
#endif

#ifdef CONFIG_MCFPIT
#define CONFIG_SYS_UDELAY_BASE		(MMAP_PIT0)
#define CONFIG_SYS_PIT_BASE		(MMAP_PIT1)
#define CONFIG_SYS_PIT_PRESCALE	(6)
#endif

#define CONFIG_SYS_INTR_BASE		(MMAP_INTC0)
#define CONFIG_SYS_NUM_IRQS		(128)
#endif				/* CONFIG_M5235 */

#ifdef CONFIG_M5249
#include <asm/immap_5249.h>
#include <asm/m5249.h>

#define CONFIG_SYS_UART_BASE		(MMAP_UART0 + (CONFIG_SYS_UART_PORT * 0x40))

#define CONFIG_SYS_INTR_BASE		(MMAP_INTC)
#define CONFIG_SYS_NUM_IRQS		(64)

/* Timer */
#ifdef CONFIG_MCFTMR
#define CONFIG_SYS_UDELAY_BASE		(MMAP_DTMR0)
#define CONFIG_SYS_TMR_BASE		(MMAP_DTMR1)
#define CONFIG_SYS_TMRPND_REG		(mbar_readLong(MCFSIM_IPR))
#define CONFIG_SYS_TMRINTR_NO		(31)
#define CONFIG_SYS_TMRINTR_MASK	(0x00000400)
#define CONFIG_SYS_TMRINTR_PEND	(CONFIG_SYS_TMRINTR_MASK)
#define CONFIG_SYS_TMRINTR_PRI		(MCFSIM_ICR_AUTOVEC | MCFSIM_ICR_LEVEL7 | MCFSIM_ICR_PRI3)
#define CONFIG_SYS_TIMER_PRESCALER	(((gd->bus_clk / 2000000) - 1) << 8)
#endif
#endif				/* CONFIG_M5249 */

#ifdef CONFIG_M5253
#include <asm/immap_5253.h>
#include <asm/m5249.h>
#include <asm/m5253.h>

#define CONFIG_SYS_UART_BASE		(MMAP_UART0 + (CONFIG_SYS_UART_PORT * 0x40))

#define CONFIG_SYS_INTR_BASE		(MMAP_INTC)
#define CONFIG_SYS_NUM_IRQS		(64)

/* Timer */
#ifdef CONFIG_MCFTMR
#define CONFIG_SYS_UDELAY_BASE		(MMAP_DTMR0)
#define CONFIG_SYS_TMR_BASE		(MMAP_DTMR1)
#define CONFIG_SYS_TMRPND_REG		(mbar_readLong(MCFSIM_IPR))
#define CONFIG_SYS_TMRINTR_NO		(27)
#define CONFIG_SYS_TMRINTR_MASK	(0x00000400)
#define CONFIG_SYS_TMRINTR_PEND	(CONFIG_SYS_TMRINTR_MASK)
#define CONFIG_SYS_TMRINTR_PRI		(MCFSIM_ICR_AUTOVEC | MCFSIM_ICR_LEVEL3 | MCFSIM_ICR_PRI3)
#define CONFIG_SYS_TIMER_PRESCALER	(((gd->bus_clk / 2000000) - 1) << 8)
#endif
#endif				/* CONFIG_M5253 */

#ifdef CONFIG_M5271
#include <asm/immap_5271.h>
#include <asm/m5271.h>

#define CONFIG_SYS_FEC0_IOBASE		(MMAP_FEC)
#define CONFIG_SYS_UART_BASE		(MMAP_UART0 + (CONFIG_SYS_UART_PORT * 0x40))

/* Timer */
#ifdef CONFIG_MCFTMR
#define CONFIG_SYS_UDELAY_BASE		(MMAP_DTMR0)
#define CONFIG_SYS_TMR_BASE		(MMAP_DTMR3)
#define CONFIG_SYS_TMRPND_REG		(((volatile int0_t *)(CONFIG_SYS_INTR_BASE))->iprl0)
#define CONFIG_SYS_TMRINTR_NO		(INT0_LO_DTMR3)
#define CONFIG_SYS_TMRINTR_MASK	(INTC_IPRL_INT22)
#define CONFIG_SYS_TMRINTR_PEND	(CONFIG_SYS_TMRINTR_MASK)
#define CONFIG_SYS_TMRINTR_PRI		(0x1E) /* Interrupt level 3, priority 6 */
#define CONFIG_SYS_TIMER_PRESCALER	(((gd->bus_clk / 1000000) - 1) << 8)
#endif

#define CONFIG_SYS_INTR_BASE		(MMAP_INTC0)
#define CONFIG_SYS_NUM_IRQS		(128)
#endif				/* CONFIG_M5271 */

#ifdef CONFIG_M5272
#include <asm/immap_5272.h>
#include <asm/m5272.h>

#define CONFIG_SYS_FEC0_IOBASE		(MMAP_FEC)
#define CONFIG_SYS_UART_BASE		(MMAP_UART0 + (CONFIG_SYS_UART_PORT * 0x40))

#define CONFIG_SYS_INTR_BASE		(MMAP_INTC)
#define CONFIG_SYS_NUM_IRQS		(64)

/* Timer */
#ifdef CONFIG_MCFTMR
#define CONFIG_SYS_UDELAY_BASE		(MMAP_TMR0)
#define CONFIG_SYS_TMR_BASE		(MMAP_TMR3)
#define CONFIG_SYS_TMRPND_REG		(((volatile intctrl_t *)(CONFIG_SYS_INTR_BASE))->int_isr)
#define CONFIG_SYS_TMRINTR_NO		(INT_TMR3)
#define CONFIG_SYS_TMRINTR_MASK	(INT_ISR_INT24)
#define CONFIG_SYS_TMRINTR_PEND	(0)
#define CONFIG_SYS_TMRINTR_PRI		(INT_ICR1_TMR3PI | INT_ICR1_TMR3IPL(5))
#define CONFIG_SYS_TIMER_PRESCALER	(((gd->bus_clk / 1000000) - 1) << 8)
#endif
#endif				/* CONFIG_M5272 */

#ifdef CONFIG_M5275
#include <asm/immap_5275.h>
#include <asm/m5275.h>

#define CONFIG_SYS_FEC0_IOBASE		(MMAP_FEC0)
#define CONFIG_SYS_FEC1_IOBASE		(MMAP_FEC1)
#define CONFIG_SYS_UART_BASE		(MMAP_UART0 + (CONFIG_SYS_UART_PORT * 0x40))

#define CONFIG_SYS_INTR_BASE		(MMAP_INTC0)
#define CONFIG_SYS_NUM_IRQS		(192)

/* Timer */
#ifdef CONFIG_MCFTMR
#define CONFIG_SYS_UDELAY_BASE		(MMAP_DTMR0)
#define CONFIG_SYS_TMR_BASE		(MMAP_DTMR3)
#define CONFIG_SYS_TMRPND_REG		(((volatile int0_t *)(CONFIG_SYS_INTR_BASE))->iprl0)
#define CONFIG_SYS_TMRINTR_NO		(INT0_LO_DTMR3)
#define CONFIG_SYS_TMRINTR_MASK	(INTC_IPRL_INT22)
#define CONFIG_SYS_TMRINTR_PEND	(CONFIG_SYS_TMRINTR_MASK)
#define CONFIG_SYS_TMRINTR_PRI		(0x1E)
#define CONFIG_SYS_TIMER_PRESCALER	(((gd->bus_clk / 1000000) - 1) << 8)
#endif
#endif				/* CONFIG_M5275 */

#ifdef CONFIG_M5282
#include <asm/immap_5282.h>
#include <asm/m5282.h>

#define CONFIG_SYS_FEC0_IOBASE		(MMAP_FEC)
#define CONFIG_SYS_UART_BASE		(MMAP_UART0 + (CONFIG_SYS_UART_PORT * 0x40))

#define CONFIG_SYS_INTR_BASE		(MMAP_INTC0)
#define CONFIG_SYS_NUM_IRQS		(128)

/* Timer */
#ifdef CONFIG_MCFTMR
#define CONFIG_SYS_UDELAY_BASE		(MMAP_DTMR0)
#define CONFIG_SYS_TMR_BASE		(MMAP_DTMR3)
#define CONFIG_SYS_TMRPND_REG		(((volatile int0_t *)(CONFIG_SYS_INTR_BASE))->iprl0)
#define CONFIG_SYS_TMRINTR_NO		(INT0_LO_DTMR3)
#define CONFIG_SYS_TMRINTR_MASK	(1 << INT0_LO_DTMR3)
#define CONFIG_SYS_TMRINTR_PEND	(CONFIG_SYS_TMRINTR_MASK)
#define CONFIG_SYS_TMRINTR_PRI		(0x1E)		/* Level must include inorder to work */
#define CONFIG_SYS_TIMER_PRESCALER	(((gd->bus_clk / 1000000) - 1) << 8)
#endif
#endif				/* CONFIG_M5282 */

#if defined(CONFIG_MCF5301x)
#include <asm/immap_5301x.h>
#include <asm/m5301x.h>

#define CONFIG_SYS_FEC0_IOBASE		(MMAP_FEC0)
#define CONFIG_SYS_FEC1_IOBASE		(MMAP_FEC1)
#define CONFIG_SYS_UART_BASE		(MMAP_UART0 + (CONFIG_SYS_UART_PORT * 0x4000))

#define CONFIG_SYS_MCFRTC_BASE		(MMAP_RTC)

/* Timer */
#ifdef CONFIG_MCFTMR
#define CONFIG_SYS_UDELAY_BASE		(MMAP_DTMR0)
#define CONFIG_SYS_TMR_BASE		(MMAP_DTMR1)
#define CONFIG_SYS_TMRPND_REG		(((volatile int0_t *)(CONFIG_SYS_INTR_BASE))->iprh0)
#define CONFIG_SYS_TMRINTR_NO		(INT0_HI_DTMR1)
#define CONFIG_SYS_TMRINTR_MASK		(INTC_IPRH_INT33)
#define CONFIG_SYS_TMRINTR_PEND		(CONFIG_SYS_TMRINTR_MASK)
#define CONFIG_SYS_TMRINTR_PRI		(6)
#define CONFIG_SYS_TIMER_PRESCALER	(((gd->bus_clk / 1000000) - 1) << 8)
#endif

#ifdef CONFIG_MCFPIT
#define CONFIG_SYS_UDELAY_BASE		(MMAP_PIT0)
#define CONFIG_SYS_PIT_BASE		(MMAP_PIT1)
#define CONFIG_SYS_PIT_PRESCALE	(6)
#endif

#define CONFIG_SYS_INTR_BASE		(MMAP_INTC0)
#define CONFIG_SYS_NUM_IRQS		(128)
#endif				/* CONFIG_M5301x */

#if defined(CONFIG_M5329) || defined(CONFIG_M5373)
#include <asm/immap_5329.h>
#include <asm/m5329.h>

#define CONFIG_SYS_FEC0_IOBASE		(MMAP_FEC)
#define CONFIG_SYS_UART_BASE		(MMAP_UART0 + (CONFIG_SYS_UART_PORT * 0x4000))
#define CONFIG_SYS_MCFRTC_BASE		(MMAP_RTC)

/* Timer */
#ifdef CONFIG_MCFTMR
#define CONFIG_SYS_UDELAY_BASE		(MMAP_DTMR0)
#define CONFIG_SYS_TMR_BASE		(MMAP_DTMR1)
#define CONFIG_SYS_TMRPND_REG		(((volatile int0_t *)(CONFIG_SYS_INTR_BASE))->iprh0)
#define CONFIG_SYS_TMRINTR_NO		(INT0_HI_DTMR1)
#define CONFIG_SYS_TMRINTR_MASK	(INTC_IPRH_INT33)
#define CONFIG_SYS_TMRINTR_PEND	(CONFIG_SYS_TMRINTR_MASK)
#define CONFIG_SYS_TMRINTR_PRI		(6)
#define CONFIG_SYS_TIMER_PRESCALER	(((gd->bus_clk / 1000000) - 1) << 8)
#endif

#ifdef CONFIG_MCFPIT
#define CONFIG_SYS_UDELAY_BASE		(MMAP_PIT0)
#define CONFIG_SYS_PIT_BASE		(MMAP_PIT1)
#define CONFIG_SYS_PIT_PRESCALE	(6)
#endif

#define CONFIG_SYS_INTR_BASE		(MMAP_INTC0)
#define CONFIG_SYS_NUM_IRQS		(128)
#endif				/* CONFIG_M5329 && CONFIG_M5373 */

#if defined(CONFIG_M54451) || defined(CONFIG_M54455)
#include <asm/immap_5445x.h>
#include <asm/m5445x.h>

#define CONFIG_SYS_FEC0_IOBASE		(MMAP_FEC0)
#if defined(CONFIG_M54455EVB)
#define CONFIG_SYS_FEC1_IOBASE		(MMAP_FEC1)
#endif

#define CONFIG_SYS_UART_BASE		(MMAP_UART0 + (CONFIG_SYS_UART_PORT * 0x4000))

#define CONFIG_SYS_MCFRTC_BASE		(MMAP_RTC)

/* Timer */
#ifdef CONFIG_MCFTMR
#define CONFIG_SYS_UDELAY_BASE		(MMAP_DTMR0)
#define CONFIG_SYS_TMR_BASE		(MMAP_DTMR1)
#define CONFIG_SYS_TMRPND_REG		(((volatile int0_t *)(CONFIG_SYS_INTR_BASE))->iprh0)
#define CONFIG_SYS_TMRINTR_NO		(INT0_HI_DTMR1)
#define CONFIG_SYS_TMRINTR_MASK	(INTC_IPRH_INT33)
#define CONFIG_SYS_TMRINTR_PEND	(CONFIG_SYS_TMRINTR_MASK)
#define CONFIG_SYS_TMRINTR_PRI		(6)
#define CONFIG_SYS_TIMER_PRESCALER	(((gd->bus_clk / 1000000) - 1) << 8)
#endif

#ifdef CONFIG_MCFPIT
#define CONFIG_SYS_UDELAY_BASE		(MMAP_PIT0)
#define CONFIG_SYS_PIT_BASE		(MMAP_PIT1)
#define CONFIG_SYS_PIT_PRESCALE	(6)
#endif

#define CONFIG_SYS_INTR_BASE		(MMAP_INTC0)
#define CONFIG_SYS_NUM_IRQS		(128)

#ifdef CONFIG_PCI
#define CONFIG_SYS_PCI_BAR0		(CONFIG_SYS_MBAR)
#define CONFIG_SYS_PCI_BAR5		(CONFIG_SYS_SDRAM_BASE)
#define CONFIG_SYS_PCI_TBATR0		(CONFIG_SYS_MBAR)
#define CONFIG_SYS_PCI_TBATR5		(CONFIG_SYS_SDRAM_BASE)
#endif
#endif				/* CONFIG_M54451 || CONFIG_M54455 */

#ifdef CONFIG_M547x
#include <asm/immap_547x_8x.h>
#include <asm/m547x_8x.h>

#ifdef CONFIG_FSLDMAFEC
#define CONFIG_SYS_FEC0_IOBASE		(MMAP_FEC0)
#define CONFIG_SYS_FEC1_IOBASE		(MMAP_FEC1)

#define FEC0_RX_TASK		0
#define FEC0_TX_TASK		1
#define FEC0_RX_PRIORITY	6
#define FEC0_TX_PRIORITY	7
#define FEC0_RX_INIT		16
#define FEC0_TX_INIT		17
#define FEC1_RX_TASK		2
#define FEC1_TX_TASK		3
#define FEC1_RX_PRIORITY	6
#define FEC1_TX_PRIORITY	7
#define FEC1_RX_INIT		30
#define FEC1_TX_INIT		31
#endif

#define CONFIG_SYS_UART_BASE		(MMAP_UART0 + (CONFIG_SYS_UART_PORT * 0x100))

#ifdef CONFIG_SLTTMR
#define CONFIG_SYS_UDELAY_BASE		(MMAP_SLT1)
#define CONFIG_SYS_TMR_BASE		(MMAP_SLT0)
#define CONFIG_SYS_TMRPND_REG		(((volatile int0_t *)(CONFIG_SYS_INTR_BASE))->iprh0)
#define CONFIG_SYS_TMRINTR_NO		(INT0_HI_SLT0)
#define CONFIG_SYS_TMRINTR_MASK	(INTC_IPRH_INT54)
#define CONFIG_SYS_TMRINTR_PEND	(CONFIG_SYS_TMRINTR_MASK)
#define CONFIG_SYS_TMRINTR_PRI		(0x1E)
#define CONFIG_SYS_TIMER_PRESCALER	(gd->bus_clk / 1000000)
#endif

#define CONFIG_SYS_INTR_BASE		(MMAP_INTC0)
#define CONFIG_SYS_NUM_IRQS		(128)

#ifdef CONFIG_PCI
#define CONFIG_SYS_PCI_BAR0		(0x40000000)
#define CONFIG_SYS_PCI_BAR1		(CONFIG_SYS_SDRAM_BASE)
#define CONFIG_SYS_PCI_TBATR0		(CONFIG_SYS_MBAR)
#define CONFIG_SYS_PCI_TBATR1		(CONFIG_SYS_SDRAM_BASE)
#endif
#endif				/* CONFIG_M547x */

#ifdef CONFIG_M548x
#include <asm/immap_547x_8x.h>
#include <asm/m547x_8x.h>

#ifdef CONFIG_FSLDMAFEC
#define CONFIG_SYS_FEC0_IOBASE		(MMAP_FEC0)
#define CONFIG_SYS_FEC1_IOBASE		(MMAP_FEC1)

#define FEC0_RX_TASK		0
#define FEC0_TX_TASK		1
#define FEC0_RX_PRIORITY	6
#define FEC0_TX_PRIORITY	7
#define FEC0_RX_INIT		16
#define FEC0_TX_INIT		17
#define FEC1_RX_TASK		2
#define FEC1_TX_TASK		3
#define FEC1_RX_PRIORITY	6
#define FEC1_TX_PRIORITY	7
#define FEC1_RX_INIT		30
#define FEC1_TX_INIT		31
#endif

#define CONFIG_SYS_UART_BASE		(MMAP_UART0 + (CONFIG_SYS_UART_PORT * 0x100))

/* Timer */
#ifdef CONFIG_SLTTMR
#define CONFIG_SYS_UDELAY_BASE		(MMAP_SLT1)
#define CONFIG_SYS_TMR_BASE		(MMAP_SLT0)
#define CONFIG_SYS_TMRPND_REG		(((volatile int0_t *)(CONFIG_SYS_INTR_BASE))->iprh0)
#define CONFIG_SYS_TMRINTR_NO		(INT0_HI_SLT0)
#define CONFIG_SYS_TMRINTR_MASK	(INTC_IPRH_INT54)
#define CONFIG_SYS_TMRINTR_PEND	(CONFIG_SYS_TMRINTR_MASK)
#define CONFIG_SYS_TMRINTR_PRI		(0x1E)
#define CONFIG_SYS_TIMER_PRESCALER	(gd->bus_clk / 1000000)
#endif

#define CONFIG_SYS_INTR_BASE		(MMAP_INTC0)
#define CONFIG_SYS_NUM_IRQS		(128)

#ifdef CONFIG_PCI
#define CONFIG_SYS_PCI_BAR0		(CONFIG_SYS_MBAR)
#define CONFIG_SYS_PCI_BAR1		(CONFIG_SYS_SDRAM_BASE)
#define CONFIG_SYS_PCI_TBATR0		(CONFIG_SYS_MBAR)
#define CONFIG_SYS_PCI_TBATR1		(CONFIG_SYS_SDRAM_BASE)
#endif
#endif				/* CONFIG_M548x */

#endif				/* __IMMAP_H */
