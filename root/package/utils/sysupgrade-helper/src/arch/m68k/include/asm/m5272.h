/*
 * mcf5272.h -- Definitions for Motorola Coldfire 5272
 *
 * Based on mcf5272sim.h of uCLinux distribution:
 *      (C) Copyright 1999, Greg Ungerer (gerg@snapgear.com)
 *      (C) Copyright 2000, Lineo Inc. (www.lineo.com)
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

#ifndef	mcf5272_h
#define	mcf5272_h
/****************************************************************************/

/*
 * Size of internal RAM
 */

#define INT_RAM_SIZE 4096

#define GPIO_PACNT_PA15MSK		(0xC0000000)
#define GPIO_PACNT_DGNT1		(0x40000000)
#define GPIO_PACNT_PA14MSK		(0x30000000)
#define GPIO_PACNT_DREQ1		(0x10000000)
#define GPIO_PACNT_PA13MSK		(0x0C000000)
#define GPIO_PACNT_DFSC3		(0x04000000)
#define GPIO_PACNT_PA12MSK		(0x03000000)
#define GPIO_PACNT_DFSC2		(0x01000000)
#define GPIO_PACNT_PA11MSK		(0x00C00000)
#define GPIO_PACNT_QSPI_CS1		(0x00800000)
#define GPIO_PACNT_PA10MSK		(0x00300000)
#define GPIO_PACNT_DREQ0		(0x00100000)
#define GPIO_PACNT_PA9MSK		(0x000C0000)
#define GPIO_PACNT_DGNT0		(0x00040000)
#define GPIO_PACNT_PA8MSK		(0x00030000)
#define GPIO_PACNT_FSC0			(0x00010000)
#define GPIO_PACNT_FSR0			(0x00010000)
#define GPIO_PACNT_PA7MSK		(0x0000C000)
#define GPIO_PACNT_DOUT3		(0x00008000)
#define GPIO_PACNT_QSPI_CS3		(0x00004000)
#define GPIO_PACNT_PA6MSK		(0x00003000)
#define GPIO_PACNT_USB_RXD		(0x00001000)
#define GPIO_PACNT_PA5MSK		(0x00000C00)
#define GPIO_PACNT_USB_TXEN		(0x00000400)
#define GPIO_PACNT_PA4MSK		(0x00000300)
#define GPIO_PACNT_USB_SUSP		(0x00000100)
#define GPIO_PACNT_PA3MSK		(0x000000C0)
#define GPIO_PACNT_USB_TN		(0x00000040)
#define GPIO_PACNT_PA2MSK		(0x00000030)
#define GPIO_PACNT_USB_RN		(0x00000010)
#define GPIO_PACNT_PA1MSK		(0x0000000C)
#define GPIO_PACNT_USB_RP		(0x00000004)
#define GPIO_PACNT_PA0MSK		(0x00000003)
#define GPIO_PACNT_USB_TP		(0x00000001)

#define GPIO_PBCNT_PB15MSK		(0xC0000000)
#define GPIO_PBCNT_E_MDC		(0x40000000)
#define GPIO_PBCNT_PB14MSK		(0x30000000)
#define GPIO_PBCNT_E_RXER		(0x10000000)
#define GPIO_PBCNT_PB13MSK		(0x0C000000)
#define GPIO_PBCNT_E_RXD1		(0x04000000)
#define GPIO_PBCNT_PB12MSK		(0x03000000)
#define GPIO_PBCNT_E_RXD2		(0x01000000)
#define GPIO_PBCNT_PB11MSK		(0x00C00000)
#define GPIO_PBCNT_E_RXD3		(0x00400000)
#define GPIO_PBCNT_PB10MSK		(0x00300000)
#define GPIO_PBCNT_E_TXD1		(0x00100000)
#define GPIO_PBCNT_PB9MSK		(0x000C0000)
#define GPIO_PBCNT_E_TXD2		(0x00040000)
#define GPIO_PBCNT_PB8MSK		(0x00030000)
#define GPIO_PBCNT_E_TXD3		(0x00010000)
#define GPIO_PBCNT_PB7MSK		(0x0000C000)
#define GPIO_PBCNT_TOUT0		(0x00004000)
#define GPIO_PBCNT_PB6MSK		(0x00003000)
#define GPIO_PBCNT_TA			(0x00001000)
#define GPIO_PBCNT_PB4MSK		(0x00000300)
#define GPIO_PBCNT_URT0_CLK		(0x00000100)
#define GPIO_PBCNT_PB3MSK		(0x000000C0)
#define GPIO_PBCNT_URT0_RTS		(0x00000040)
#define GPIO_PBCNT_PB2MSK		(0x00000030)
#define GPIO_PBCNT_URT0_CTS		(0x00000010)
#define GPIO_PBCNT_PB1MSK		(0x0000000C)
#define GPIO_PBCNT_URT0_RXD		(0x00000004)
#define GPIO_PBCNT_URT0_TIN2		(0x00000004)
#define GPIO_PBCNT_PB0MSK		(0x00000003)
#define GPIO_PBCNT_URT0_TXD		(0x00000001)

#define GPIO_PDCNT_PD7MSK		(0x0000C000)
#define GPIO_PDCNT_TIN1			(0x00008000)
#define GPIO_PDCNT_PWM_OUT2		(0x00004000)
#define GPIO_PDCNT_PD6MSK		(0x00003000)
#define GPIO_PDCNT_TOUT1		(0x00002000)
#define GPIO_PDCNT_PWM_OUT1		(0x00001000)
#define GPIO_PDCNT_PD5MSK		(0x00000C00)
#define GPIO_PDCNT_INT4			(0x00000C00)
#define GPIO_PDCNT_DIN3			(0x00000800)
#define GPIO_PDCNT_PD4MSK		(0x00000300)
#define GPIO_PDCNT_URT1_TXD		(0x00000200)
#define GPIO_PDCNT_DOUT0		(0x00000100)
#define GPIO_PDCNT_PD3MSK		(0x000000C0)
#define GPIO_PDCNT_INT5			(0x000000C0)
#define GPIO_PDCNT_URT1_RTS		(0x00000080)
#define GPIO_PDCNT_PD2MSK		(0x00000030)
#define GPIO_PDCNT_QSPI_CS2		(0x00000030)
#define GPIO_PDCNT_URT1_CTS		(0x00000020)
#define GPIO_PDCNT_PD1MSK		(0x0000000C)
#define GPIO_PDCNT_URT1_RXD		(0x00000008)
#define GPIO_PDCNT_URT1_TIN3		(0x00000008)
#define GPIO_PDCNT_DIN0			(0x00000004)
#define GPIO_PDCNT_PD0MSK		(0x00000003)
#define GPIO_PDCNT_URT1_CLK		(0x00000002)
#define GPIO_PDCNT_DCL0			(0x00000001)

#define INT_RSVD0			(0)
#define INT_INT1			(1)
#define INT_INT2			(2)
#define INT_INT3			(3)
#define INT_INT4			(4)
#define INT_TMR0			(5)
#define INT_TMR1			(6)
#define INT_TMR2			(7)
#define INT_TMR3			(8)
#define INT_UART1			(9)
#define INT_UART2			(10)
#define INT_PLIP			(11)
#define INT_PLIA			(12)
#define INT_USB0			(13)
#define INT_USB1			(14)
#define INT_USB2			(15)
#define INT_USB3			(16)
#define INT_USB4			(17)
#define INT_USB5			(18)
#define INT_USB6			(19)
#define INT_USB7			(20)
#define INT_DMA				(21)
#define INT_ERX				(22)
#define INT_ETX				(23)
#define INT_ENTC			(24)
#define INT_QSPI			(25)
#define INT_INT5			(26)
#define INT_INT6			(27)
#define INT_SWTO			(28)

#define INT_ICR1_TMR0MASK		(0x000F000)
#define INT_ICR1_TMR0PI			(0x0008000)
#define INT_ICR1_TMR0IPL(x)		(((x)&0x7)<<12)
#define INT_ICR1_TMR1MASK		(0x0000F00)
#define INT_ICR1_TMR1PI			(0x0000800)
#define INT_ICR1_TMR1IPL(x)		(((x)&0x7)<<8)
#define INT_ICR1_TMR2MASK		(0x00000F0)
#define INT_ICR1_TMR2PI			(0x0000080)
#define INT_ICR1_TMR2IPL(x)		(((x)&0x7)<<4)
#define INT_ICR1_TMR3MASK		(0x000000F)
#define INT_ICR1_TMR3PI			(0x0000008)
#define INT_ICR1_TMR3IPL(x)		(((x)&0x7))

#define INT_ISR_INT31			(0x80000000)
#define INT_ISR_INT30			(0x40000000)
#define INT_ISR_INT29			(0x20000000)
#define INT_ISR_INT28			(0x10000000)
#define INT_ISR_INT27			(0x08000000)
#define INT_ISR_INT26			(0x04000000)
#define INT_ISR_INT25			(0x02000000)
#define INT_ISR_INT24			(0x01000000)
#define INT_ISR_INT23			(0x00800000)
#define INT_ISR_INT22			(0x00400000)
#define INT_ISR_INT21			(0x00200000)
#define INT_ISR_INT20			(0x00100000)
#define INT_ISR_INT19			(0x00080000)
#define INT_ISR_INT18			(0x00040000)
#define INT_ISR_INT17			(0x00020000)
#define INT_ISR_INT16			(0x00010000)
#define INT_ISR_INT15			(0x00008000)
#define INT_ISR_INT14			(0x00004000)
#define INT_ISR_INT13			(0x00002000)
#define INT_ISR_INT12			(0x00001000)
#define INT_ISR_INT11			(0x00000800)
#define INT_ISR_INT10			(0x00000400)
#define INT_ISR_INT9			(0x00000200)
#define INT_ISR_INT8			(0x00000100)
#define INT_ISR_INT7			(0x00000080)
#define INT_ISR_INT6			(0x00000040)
#define INT_ISR_INT5			(0x00000020)
#define INT_ISR_INT4			(0x00000010)
#define INT_ISR_INT3			(0x00000008)
#define INT_ISR_INT2			(0x00000004)
#define INT_ISR_INT1			(0x00000002)
#define INT_ISR_INT0			(0x00000001)

#endif				/* mcf5272_h */
