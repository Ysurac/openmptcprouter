/*
 * mcf5282.h -- Definitions for Motorola Coldfire 5282
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

/****************************************************************************/
#ifndef	m5282_h
#define	m5282_h

/*********************************************************************
* PLL Clock Module
*********************************************************************/
/* Bit definitions and macros for PLL_SYNCR */
#define PLL_SYNCR_LOLRE			(0x8000)
#define PLL_SYNCR_MFD2			(0x4000)
#define PLL_SYNCR_MFD1			(0x2000)
#define PLL_SYNCR_MFD0			(0x1000)
#define PLL_SYNCR_LOCRE			(0x0800)
#define PLL_SYNCR_RFC2			(0x0400)
#define PLL_SYNCR_RFC1			(0x0200)
#define PLL_SYNCR_RFC0			(0x0100)
#define PLL_SYNCR_LOCEN			(0x0080)
#define PLL_SYNCR_DISCLK		(0x0040)
#define PLL_SYNCR_FWKUP			(0x0020)
#define PLL_SYNCR_STPMD1		(0x0008)
#define PLL_SYNCR_STPMD0		(0x0004)

/* Bit definitions and macros for PLL_SYNSR */
#define PLL_SYNSR_MODE			(0x0080)
#define PLL_SYNSR_PLLSEL		(0x0040)
#define PLL_SYNSR_PLLREF		(0x0020)
#define PLL_SYNSR_LOCKS			(0x0010)
#define PLL_SYNSR_LOCK			(0x0008)
#define PLL_SYNSR_LOCS			(0x0004)

/*********************************************************************
* Interrupt Controller (INTC)
*********************************************************************/
#define INT0_LO_RSVD0			(0)
#define INT0_LO_EPORT1			(1)
#define INT0_LO_EPORT2			(2)
#define INT0_LO_EPORT3			(3)
#define INT0_LO_EPORT4			(4)
#define INT0_LO_EPORT5			(5)
#define INT0_LO_EPORT6			(6)
#define INT0_LO_EPORT7			(7)
#define INT0_LO_SCM_SWT1		(8)
#define INT0_LO_DMA_00			(9)
#define INT0_LO_DMA_01			(10)
#define INT0_LO_DMA_02			(11)
#define INT0_LO_DMA_03			(12)
#define INT0_LO_UART0			(13)
#define INT0_LO_UART1			(14)
#define INT0_LO_UART2			(15)
#define INT0_LO_RSVD1			(16)
#define INT0_LO_I2C			(17)
#define INT0_LO_QSPI			(18)
#define INT0_LO_DTMR0			(19)
#define INT0_LO_DTMR1			(20)
#define INT0_LO_DTMR2			(21)
#define INT0_LO_DTMR3			(22)
#define INT0_LO_FEC_TXF			(23)
#define INT0_LO_FEC_TXB			(24)
#define INT0_LO_FEC_UN			(25)
#define INT0_LO_FEC_RL			(26)
#define INT0_LO_FEC_RXF			(27)
#define INT0_LO_FEC_RXB			(28)
#define INT0_LO_FEC_MII			(29)
#define INT0_LO_FEC_LC			(30)
#define INT0_LO_FEC_HBERR		(31)
#define INT0_HI_FEC_GRA			(32)
#define INT0_HI_FEC_EBERR		(33)
#define INT0_HI_FEC_BABT		(34)
#define INT0_HI_FEC_BABR		(35)
#define INT0_HI_PMM_LVDF		(36)
#define INT0_HI_QADC_CF1		(37)
#define INT0_HI_QADC_CF2		(38)
#define INT0_HI_QADC_PF1		(39)
#define INT0_HI_QADC_PF2		(40)
#define INT0_HI_GPTA_TOF		(41)
#define INT0_HI_GPTA_PAIF		(42)
#define INT0_HI_GPTA_PAOVF		(43)
#define INT0_HI_GPTA_C0F		(44)
#define INT0_HI_GPTA_C1F		(45)
#define INT0_HI_GPTA_C2F		(46)
#define INT0_HI_GPTA_C3F		(47)
#define INT0_HI_GPTB_TOF		(48)
#define INT0_HI_GPTB_PAIF		(49)
#define INT0_HI_GPTB_PAOVF		(50)
#define INT0_HI_GPTB_C0F		(51)
#define INT0_HI_GPTB_C1F		(52)
#define INT0_HI_GPTB_C2F		(53)
#define INT0_HI_GPTB_C3F		(54)
#define INT0_HI_PIT0			(55)
#define INT0_HI_PIT1			(56)
#define INT0_HI_PIT2			(57)
#define INT0_HI_PIT3			(58)
#define INT0_HI_CFM_CBEIF		(59)
#define INT0_HI_CFM_CCIF		(60)
#define INT0_HI_CFM_PVIF		(61)
#define INT0_HI_CFM_AEIF		(62)

/*
 * Size of internal RAM
 */

#define INT_RAM_SIZE	65536

/* General Purpose I/O Module GPIO */

#define MCFGPIO_PORTA		(*(vu_char *) (CONFIG_SYS_MBAR+0x100000))
#define MCFGPIO_PORTB		(*(vu_char *) (CONFIG_SYS_MBAR+0x100001))
#define MCFGPIO_PORTC		(*(vu_char *) (CONFIG_SYS_MBAR+0x100002))
#define MCFGPIO_PORTD		(*(vu_char *) (CONFIG_SYS_MBAR+0x100003))
#define MCFGPIO_PORTE		(*(vu_char *) (CONFIG_SYS_MBAR+0x100004))
#define MCFGPIO_PORTF		(*(vu_char *) (CONFIG_SYS_MBAR+0x100005))
#define MCFGPIO_PORTG		(*(vu_char *) (CONFIG_SYS_MBAR+0x100006))
#define MCFGPIO_PORTH		(*(vu_char *) (CONFIG_SYS_MBAR+0x100007))
#define MCFGPIO_PORTJ		(*(vu_char *) (CONFIG_SYS_MBAR+0x100008))
#define MCFGPIO_PORTDD		(*(vu_char *) (CONFIG_SYS_MBAR+0x100009))
#define MCFGPIO_PORTEH		(*(vu_char *) (CONFIG_SYS_MBAR+0x10000A))
#define MCFGPIO_PORTEL		(*(vu_char *) (CONFIG_SYS_MBAR+0x10000B))
#define MCFGPIO_PORTAS		(*(vu_char *) (CONFIG_SYS_MBAR+0x10000C))
#define MCFGPIO_PORTQS		(*(vu_char *) (CONFIG_SYS_MBAR+0x10000D))
#define MCFGPIO_PORTSD		(*(vu_char *) (CONFIG_SYS_MBAR+0x10000E))
#define MCFGPIO_PORTTC		(*(vu_char *) (CONFIG_SYS_MBAR+0x10000F))
#define MCFGPIO_PORTTD		(*(vu_char *) (CONFIG_SYS_MBAR+0x100010))
#define MCFGPIO_PORTUA		(*(vu_char *) (CONFIG_SYS_MBAR+0x100011))

#define MCFGPIO_DDRA		(*(vu_char *) (CONFIG_SYS_MBAR+0x100014))
#define MCFGPIO_DDRB		(*(vu_char *) (CONFIG_SYS_MBAR+0x100015))
#define MCFGPIO_DDRC		(*(vu_char *) (CONFIG_SYS_MBAR+0x100016))
#define MCFGPIO_DDRD		(*(vu_char *) (CONFIG_SYS_MBAR+0x100017))
#define MCFGPIO_DDRE		(*(vu_char *) (CONFIG_SYS_MBAR+0x100018))
#define MCFGPIO_DDRF		(*(vu_char *) (CONFIG_SYS_MBAR+0x100019))
#define MCFGPIO_DDRG		(*(vu_char *) (CONFIG_SYS_MBAR+0x10001A))
#define MCFGPIO_DDRH		(*(vu_char *) (CONFIG_SYS_MBAR+0x10001B))
#define MCFGPIO_DDRJ		(*(vu_char *) (CONFIG_SYS_MBAR+0x10001C))
#define MCFGPIO_DDRDD		(*(vu_char *) (CONFIG_SYS_MBAR+0x10001D))
#define MCFGPIO_DDREH		(*(vu_char *) (CONFIG_SYS_MBAR+0x10001E))
#define MCFGPIO_DDREL		(*(vu_char *) (CONFIG_SYS_MBAR+0x10001F))
#define MCFGPIO_DDRAS		(*(vu_char *) (CONFIG_SYS_MBAR+0x100020))
#define MCFGPIO_DDRQS		(*(vu_char *) (CONFIG_SYS_MBAR+0x100021))
#define MCFGPIO_DDRSD		(*(vu_char *) (CONFIG_SYS_MBAR+0x100022))
#define MCFGPIO_DDRTC		(*(vu_char *) (CONFIG_SYS_MBAR+0x100023))
#define MCFGPIO_DDRTD		(*(vu_char *) (CONFIG_SYS_MBAR+0x100024))
#define MCFGPIO_DDRUA		(*(vu_char *) (CONFIG_SYS_MBAR+0x100025))

#define MCFGPIO_PORTAP		(*(vu_char *) (CONFIG_SYS_MBAR+0x100028))
#define MCFGPIO_PORTBP		(*(vu_char *) (CONFIG_SYS_MBAR+0x100029))
#define MCFGPIO_PORTCP		(*(vu_char *) (CONFIG_SYS_MBAR+0x10002A))
#define MCFGPIO_PORTDP		(*(vu_char *) (CONFIG_SYS_MBAR+0x10002B))
#define MCFGPIO_PORTEP		(*(vu_char *) (CONFIG_SYS_MBAR+0x10002C))
#define MCFGPIO_PORTFP		(*(vu_char *) (CONFIG_SYS_MBAR+0x10002D))
#define MCFGPIO_PORTGP		(*(vu_char *) (CONFIG_SYS_MBAR+0x10002E))
#define MCFGPIO_PORTHP		(*(vu_char *) (CONFIG_SYS_MBAR+0x10002F))
#define MCFGPIO_PORTJP		(*(vu_char *) (CONFIG_SYS_MBAR+0x100030))
#define MCFGPIO_PORTDDP		(*(vu_char *) (CONFIG_SYS_MBAR+0x100031))
#define MCFGPIO_PORTEHP		(*(vu_char *) (CONFIG_SYS_MBAR+0x100032))
#define MCFGPIO_PORTELP		(*(vu_char *) (CONFIG_SYS_MBAR+0x100033))
#define MCFGPIO_PORTASP		(*(vu_char *) (CONFIG_SYS_MBAR+0x100034))
#define MCFGPIO_PORTQSP		(*(vu_char *) (CONFIG_SYS_MBAR+0x100035))
#define MCFGPIO_PORTSDP		(*(vu_char *) (CONFIG_SYS_MBAR+0x100036))
#define MCFGPIO_PORTTCP		(*(vu_char *) (CONFIG_SYS_MBAR+0x100037))
#define MCFGPIO_PORTTDP		(*(vu_char *) (CONFIG_SYS_MBAR+0x100038))
#define MCFGPIO_PORTUAP		(*(vu_char *) (CONFIG_SYS_MBAR+0x100039))

#define MCFGPIO_SETA		(*(vu_char *) (CONFIG_SYS_MBAR+0x100028))
#define MCFGPIO_SETB		(*(vu_char *) (CONFIG_SYS_MBAR+0x100029))
#define MCFGPIO_SETC		(*(vu_char *) (CONFIG_SYS_MBAR+0x10002A))
#define MCFGPIO_SETD		(*(vu_char *) (CONFIG_SYS_MBAR+0x10002B))
#define MCFGPIO_SETE		(*(vu_char *) (CONFIG_SYS_MBAR+0x10002C))
#define MCFGPIO_SETF		(*(vu_char *) (CONFIG_SYS_MBAR+0x10002D))
#define MCFGPIO_SETG		(*(vu_char *) (CONFIG_SYS_MBAR+0x10002E))
#define MCFGPIO_SETH		(*(vu_char *) (CONFIG_SYS_MBAR+0x10002F))
#define MCFGPIO_SETJ		(*(vu_char *) (CONFIG_SYS_MBAR+0x100030))
#define MCFGPIO_SETDD		(*(vu_char *) (CONFIG_SYS_MBAR+0x100031))
#define MCFGPIO_SETEH		(*(vu_char *) (CONFIG_SYS_MBAR+0x100032))
#define MCFGPIO_SETEL		(*(vu_char *) (CONFIG_SYS_MBAR+0x100033))
#define MCFGPIO_SETAS		(*(vu_char *) (CONFIG_SYS_MBAR+0x100034))
#define MCFGPIO_SETQS		(*(vu_char *) (CONFIG_SYS_MBAR+0x100035))
#define MCFGPIO_SETSD		(*(vu_char *) (CONFIG_SYS_MBAR+0x100036))
#define MCFGPIO_SETTC		(*(vu_char *) (CONFIG_SYS_MBAR+0x100037))
#define MCFGPIO_SETTD		(*(vu_char *) (CONFIG_SYS_MBAR+0x100038))
#define MCFGPIO_SETUA		(*(vu_char *) (CONFIG_SYS_MBAR+0x100039))

#define MCFGPIO_CLRA		(*(vu_char *) (CONFIG_SYS_MBAR+0x10003C))
#define MCFGPIO_CLRB		(*(vu_char *) (CONFIG_SYS_MBAR+0x10003D))
#define MCFGPIO_CLRC		(*(vu_char *) (CONFIG_SYS_MBAR+0x10003E))
#define MCFGPIO_CLRD		(*(vu_char *) (CONFIG_SYS_MBAR+0x10003F))
#define MCFGPIO_CLRE		(*(vu_char *) (CONFIG_SYS_MBAR+0x100040))
#define MCFGPIO_CLRF		(*(vu_char *) (CONFIG_SYS_MBAR+0x100041))
#define MCFGPIO_CLRG		(*(vu_char *) (CONFIG_SYS_MBAR+0x100042))
#define MCFGPIO_CLRH		(*(vu_char *) (CONFIG_SYS_MBAR+0x100043))
#define MCFGPIO_CLRJ		(*(vu_char *) (CONFIG_SYS_MBAR+0x100044))
#define MCFGPIO_CLRDD		(*(vu_char *) (CONFIG_SYS_MBAR+0x100045))
#define MCFGPIO_CLREH		(*(vu_char *) (CONFIG_SYS_MBAR+0x100046))
#define MCFGPIO_CLREL		(*(vu_char *) (CONFIG_SYS_MBAR+0x100047))
#define MCFGPIO_CLRAS		(*(vu_char *) (CONFIG_SYS_MBAR+0x100048))
#define MCFGPIO_CLRQS		(*(vu_char *) (CONFIG_SYS_MBAR+0x100049))
#define MCFGPIO_CLRSD		(*(vu_char *) (CONFIG_SYS_MBAR+0x10004A))
#define MCFGPIO_CLRTC		(*(vu_char *) (CONFIG_SYS_MBAR+0x10004B))
#define MCFGPIO_CLRTD		(*(vu_char *) (CONFIG_SYS_MBAR+0x10004C))
#define MCFGPIO_CLRUA		(*(vu_char *) (CONFIG_SYS_MBAR+0x10004D))

#define MCFGPIO_PBCDPAR	(*(vu_char *) (CONFIG_SYS_MBAR+0x100050))
#define MCFGPIO_PFPAR		(*(vu_char *) (CONFIG_SYS_MBAR+0x100051))
#define MCFGPIO_PEPAR		(*(vu_short *)(CONFIG_SYS_MBAR+0x100052))
#define MCFGPIO_PJPAR		(*(vu_char *) (CONFIG_SYS_MBAR+0x100054))
#define MCFGPIO_PSDPAR		(*(vu_char *) (CONFIG_SYS_MBAR+0x100055))
#define MCFGPIO_PASPAR		(*(vu_short *)(CONFIG_SYS_MBAR+0x100056))
#define MCFGPIO_PEHLPAR		(*(vu_char *) (CONFIG_SYS_MBAR+0x100058))
#define MCFGPIO_PQSPAR		(*(vu_char *) (CONFIG_SYS_MBAR+0x100059))
#define MCFGPIO_PTCPAR		(*(vu_char *) (CONFIG_SYS_MBAR+0x10005A))
#define MCFGPIO_PTDPAR		(*(vu_char *) (CONFIG_SYS_MBAR+0x10005B))
#define MCFGPIO_PUAPAR		(*(vu_char *) (CONFIG_SYS_MBAR+0x10005C))

/* Bit level definitions and macros */
#define MCFGPIO_PORT7			(0x80)
#define MCFGPIO_PORT6			(0x40)
#define MCFGPIO_PORT5			(0x20)
#define MCFGPIO_PORT4			(0x10)
#define MCFGPIO_PORT3			(0x08)
#define MCFGPIO_PORT2			(0x04)
#define MCFGPIO_PORT1			(0x02)
#define MCFGPIO_PORT0			(0x01)
#define MCFGPIO_PORT(x)			(0x01<<x)

#define MCFGPIO_DDR7			(0x80)
#define MCFGPIO_DDR6			(0x40)
#define MCFGPIO_DDR5			(0x20)
#define MCFGPIO_DDR4			(0x10)
#define MCFGPIO_DDR3			(0x08)
#define MCFGPIO_DDR2			(0x04)
#define MCFGPIO_DDR1			(0x02)
#define MCFGPIO_DDR0			(0x01)
#define MCFGPIO_DDR(x)			(0x01<<x)

#define MCFGPIO_Px7			(0x80)
#define MCFGPIO_Px6			(0x40)
#define MCFGPIO_Px5			(0x20)
#define MCFGPIO_Px4			(0x10)
#define MCFGPIO_Px3			(0x08)
#define MCFGPIO_Px2			(0x04)
#define MCFGPIO_Px1			(0x02)
#define MCFGPIO_Px0			(0x01)
#define MCFGPIO_Px(x)			(0x01<<x)

#define MCFGPIO_PBCDPAR_PBPA		(0x80)
#define MCFGPIO_PBCDPAR_PCDPA		(0x40)

#define MCFGPIO_PEPAR_PEPA7		(0x4000)
#define MCFGPIO_PEPAR_PEPA6		(0x1000)
#define MCFGPIO_PEPAR_PEPA5		(0x0400)
#define MCFGPIO_PEPAR_PEPA4		(0x0100)
#define MCFGPIO_PEPAR_PEPA3		(0x0040)
#define MCFGPIO_PEPAR_PEPA2		(0x0010)
#define MCFGPIO_PEPAR_PEPA1(x)		(((x)&0x3)<<2)
#define MCFGPIO_PEPAR_PEPA0(x)		(((x)&0x3))

#define MCFGPIO_PFPAR_PFPA7		(0x80)
#define MCFGPIO_PFPAR_PFPA6		(0x40)
#define MCFGPIO_PFPAR_PFPA5		(0x20)

#define MCFGPIO_PJPAR_PJPA7		(0x80)
#define MCFGPIO_PJPAR_PJPA6		(0x40)
#define MCFGPIO_PJPAR_PJPA5		(0x20)
#define MCFGPIO_PJPAR_PJPA4		(0x10)
#define MCFGPIO_PJPAR_PJPA3		(0x08)
#define MCFGPIO_PJPAR_PJPA2		(0x04)
#define MCFGPIO_PJPAR_PJPA1		(0x02)
#define MCFGPIO_PJPAR_PJPA0		(0x01)
#define MCFGPIO_PJPAR_PJPA(x)		(0x01<<x)

#define MCFGPIO_PSDPAR_PSDPA		(0x80)

#define MCFGPIO_PASPAR_PASPA5(x)	(((x)&0x3)<<10)
#define MCFGPIO_PASPAR_PASPA4(x)	(((x)&0x3)<<8)
#define MCFGPIO_PASPAR_PASPA3(x)	(((x)&0x3)<<6)
#define MCFGPIO_PASPAR_PASPA2(x)	(((x)&0x3)<<4)
#define MCFGPIO_PASPAR_PASPA1(x)	(((x)&0x3)<<2)
#define MCFGPIO_PASPAR_PASPA0(x)	(((x)&0x3))

#define MCFGPIO_PEHLPAR_PEHPA		(0x80)
#define MCFGPIO_PEHLPAR_PELPA		(0x40)

#define MCFGPIO_PQSPAR_PQSPA6		(0x40)
#define MCFGPIO_PQSPAR_PQSPA5		(0x20)
#define MCFGPIO_PQSPAR_PQSPA4		(0x10)
#define MCFGPIO_PQSPAR_PQSPA3		(0x08)
#define MCFGPIO_PQSPAR_PQSPA2		(0x04)
#define MCFGPIO_PQSPAR_PQSPA1		(0x02)
#define MCFGPIO_PQSPAR_PQSPA0		(0x01)
#define MCFGPIO_PQSPAR_PQSPA(x)		(0x01<<x)

#define MCFGPIO_PTCPAR_PTCPA3(x)	(((x)&0x3)<<6)
#define MCFGPIO_PTCPAR_PTCPA2(x)	(((x)&0x3)<<4)
#define MCFGPIO_PTCPAR_PTCPA1(x)	(((x)&0x3)<<2)
#define MCFGPIO_PTCPAR_PTCPA0(x)	(((x)&0x3))

#define MCFGPIO_PTDPAR_PTDPA3(x)	(((x)&0x3)<<6)
#define MCFGPIO_PTDPAR_PTDPA2(x)	(((x)&0x3)<<4)
#define MCFGPIO_PTDPAR_PTDPA1(x)	(((x)&0x3)<<2)
#define MCFGPIO_PTDPAR_PTDPA0(x)	(((x)&0x3))

#define MCFGPIO_PUAPAR_PUAPA3		(0x08)
#define MCFGPIO_PUAPAR_PUAPA2		(0x04)
#define MCFGPIO_PUAPAR_PUAPA1		(0x02)
#define MCFGPIO_PUAPAR_PUAPA0		(0x01)

/* System Conrol Module SCM */

#define MCFSCM_RAMBAR		(*(vu_long *) (CONFIG_SYS_MBAR+0x00000008))
#define MCFSCM_CRSR		(*(vu_char *) (CONFIG_SYS_MBAR+0x00000010))
#define MCFSCM_CWCR		(*(vu_char *) (CONFIG_SYS_MBAR+0x00000011))
#define MCFSCM_LPICR		(*(vu_char *) (CONFIG_SYS_MBAR+0x00000012))
#define MCFSCM_CWSR		(*(vu_char *) (CONFIG_SYS_MBAR+0x00000013))

#define MCFSCM_MPARK		(*(vu_long *) (CONFIG_SYS_MBAR+0x0000001C))
#define MCFSCM_MPR		(*(vu_char *) (CONFIG_SYS_MBAR+0x00000020))
#define MCFSCM_PACR0		(*(vu_char *) (CONFIG_SYS_MBAR+0x00000024))
#define MCFSCM_PACR1		(*(vu_char *) (CONFIG_SYS_MBAR+0x00000025))
#define MCFSCM_PACR2		(*(vu_char *) (CONFIG_SYS_MBAR+0x00000026))
#define MCFSCM_PACR3		(*(vu_char *) (CONFIG_SYS_MBAR+0x00000027))
#define MCFSCM_PACR4		(*(vu_char *) (CONFIG_SYS_MBAR+0x00000028))
#define MCFSCM_PACR5		(*(vu_char *) (CONFIG_SYS_MBAR+0x0000002A))
#define MCFSCM_PACR6		(*(vu_char *) (CONFIG_SYS_MBAR+0x0000002B))
#define MCFSCM_PACR7		(*(vu_char *) (CONFIG_SYS_MBAR+0x0000002C))
#define MCFSCM_PACR8		(*(vu_char *) (CONFIG_SYS_MBAR+0x0000002E))
#define MCFSCM_GPACR0		(*(vu_char *) (CONFIG_SYS_MBAR+0x00000030))
#define MCFSCM_GPACR1		(*(vu_char *) (CONFIG_SYS_MBAR+0x00000031))

#define MCFSCM_CRSR_EXT		(0x80)
#define MCFSCM_CRSR_CWDR	(0x20)
#define MCFSCM_RAMBAR_BA(x)	((x)&0xFFFF0000)
#define MCFSCM_RAMBAR_BDE	(0x00000200)

/* Reset Controller Module RCM */

#define MCFRESET_RCR		(*(vu_char *) (CONFIG_SYS_MBAR+0x00110000))
#define MCFRESET_RSR		(*(vu_char *) (CONFIG_SYS_MBAR+0x00110001))

#define MCFRESET_RCR_SOFTRST	(0x80)
#define MCFRESET_RCR_FRCRSTOUT	(0x40)
#define MCFRESET_RCR_LVDF	(0x10)
#define MCFRESET_RCR_LVDIE	(0x08)
#define MCFRESET_RCR_LVDRE	(0x04)
#define MCFRESET_RCR_LVDE	(0x01)

#define MCFRESET_RSR_LVD	(0x40)
#define MCFRESET_RSR_SOFT	(0x20)
#define MCFRESET_RSR_WDR	(0x10)
#define MCFRESET_RSR_POR	(0x08)
#define MCFRESET_RSR_EXT	(0x04)
#define MCFRESET_RSR_LOC	(0x02)
#define MCFRESET_RSR_LOL	(0x01)
#define MCFRESET_RSR_ALL	(0x7F)
#define MCFRESET_RCR_SOFTRST	(0x80)
#define MCFRESET_RCR_FRCRSTOUT	(0x40)

/* Chip Configuration Module CCM */

#define MCFCCM_CCR		(*(vu_short *)(CONFIG_SYS_MBAR+0x00110004))
#define MCFCCM_RCON		(*(vu_short *)(CONFIG_SYS_MBAR+0x00110008))
#define MCFCCM_CIR		(*(vu_short *)(CONFIG_SYS_MBAR+0x0011000A))

/* Bit level definitions and macros */
#define MCFCCM_CCR_LOAD		(0x8000)
#define MCFCCM_CCR_MODE(x)	(((x)&0x0007)<<8)
#define MCFCCM_CCR_SZEN		(0x0040)
#define MCFCCM_CCR_PSTEN	(0x0020)
#define MCFCCM_CCR_BME		(0x0008)
#define MCFCCM_CCR_BMT(x)	(((x)&0x0007))

#define MCFCCM_CIR_PIN_MASK	(0xFF00)
#define MCFCCM_CIR_PRN_MASK	(0x00FF)

/* Clock Module */

#define MCFCLOCK_SYNCR		(*(vu_short *)(CONFIG_SYS_MBAR+0x120000))
#define MCFCLOCK_SYNSR		(*(vu_char *) (CONFIG_SYS_MBAR+0x120002))

#define MCFCLOCK_SYNCR_MFD(x)	(((x)&0x0007)<<12)
#define MCFCLOCK_SYNCR_RFD(x)	(((x)&0x0007)<<8)
#define MCFCLOCK_SYNSR_LOCK	0x08

#define MCFSDRAMC_DCR		(*(vu_short *)(CONFIG_SYS_MBAR+0x00000040))
#define MCFSDRAMC_DACR0		(*(vu_long *) (CONFIG_SYS_MBAR+0x00000048))
#define MCFSDRAMC_DMR0		(*(vu_long *) (CONFIG_SYS_MBAR+0x0000004c))
#define MCFSDRAMC_DACR1		(*(vu_long *) (CONFIG_SYS_MBAR+0x00000050))
#define MCFSDRAMC_DMR1		(*(vu_long *) (CONFIG_SYS_MBAR+0x00000054))

#define MCFSDRAMC_DCR_NAM	(0x2000)
#define MCFSDRAMC_DCR_COC	(0x1000)
#define MCFSDRAMC_DCR_IS	(0x0800)
#define MCFSDRAMC_DCR_RTIM_3	(0x0000)
#define MCFSDRAMC_DCR_RTIM_6	(0x0200)
#define MCFSDRAMC_DCR_RTIM_9	(0x0400)
#define MCFSDRAMC_DCR_RC(x)	((x)&0x01FF)

#define MCFSDRAMC_DACR_BASE(x)	((x)&0xFFFC0000)
#define MCFSDRAMC_DACR_RE	(0x00008000)
#define MCFSDRAMC_DACR_CASL(x)	(((x)&0x03)<<12)
#define MCFSDRAMC_DACR_CBM(x)	(((x)&0x07)<<8)
#define MCFSDRAMC_DACR_PS_32	(0x00000000)
#define MCFSDRAMC_DACR_PS_16	(0x00000020)
#define MCFSDRAMC_DACR_PS_8	(0x00000010)
#define MCFSDRAMC_DACR_IP	(0x00000008)
#define MCFSDRAMC_DACR_IMRS	(0x00000040)

#define MCFSDRAMC_DMR_BAM_16M	(0x00FC0000)
#define MCFSDRAMC_DMR_WP	(0x00000100)
#define MCFSDRAMC_DMR_CI	(0x00000040)
#define MCFSDRAMC_DMR_AM	(0x00000020)
#define MCFSDRAMC_DMR_SC	(0x00000010)
#define MCFSDRAMC_DMR_SD	(0x00000008)
#define MCFSDRAMC_DMR_UC	(0x00000004)
#define MCFSDRAMC_DMR_UD	(0x00000002)
#define MCFSDRAMC_DMR_V		(0x00000001)

#define MCFWTM_WCR		(*(vu_short *)(CONFIG_SYS_MBAR+0x00140000))
#define MCFWTM_WMR		(*(vu_short *)(CONFIG_SYS_MBAR+0x00140002))
#define MCFWTM_WCNTR		(*(vu_short *)(CONFIG_SYS_MBAR+0x00140004))
#define MCFWTM_WSR		(*(vu_short *)(CONFIG_SYS_MBAR+0x00140006))

/*********************************************************************
* General Purpose Timer (GPT) Module
*********************************************************************/

#define MCFGPTA_GPTIOS		(*(vu_char *)(CONFIG_SYS_MBAR+0x1A0000))
#define MCFGPTA_GPTCFORC	(*(vu_char *)(CONFIG_SYS_MBAR+0x1A0001))
#define MCFGPTA_GPTOC3M		(*(vu_char *)(CONFIG_SYS_MBAR+0x1A0002))
#define MCFGPTA_GPTOC3D		(*(vu_char *)(CONFIG_SYS_MBAR+0x1A0003))
#define MCFGPTA_GPTCNT		(*(vu_short *)(CONFIG_SYS_MBAR+0x1A0004))
#define MCFGPTA_GPTSCR1		(*(vu_char *)(CONFIG_SYS_MBAR+0x1A0006))
#define MCFGPTA_GPTTOV		(*(vu_char *)(CONFIG_SYS_MBAR+0x1A0008))
#define MCFGPTA_GPTCTL1		(*(vu_char *)(CONFIG_SYS_MBAR+0x1A0009))
#define MCFGPTA_GPTCTL2		(*(vu_char *)(CONFIG_SYS_MBAR+0x1A000B))
#define MCFGPTA_GPTIE		(*(vu_char *)(CONFIG_SYS_MBAR+0x1A000C))
#define MCFGPTA_GPTSCR2		(*(vu_char *)(CONFIG_SYS_MBAR+0x1A000D))
#define MCFGPTA_GPTFLG1		(*(vu_char *)(CONFIG_SYS_MBAR+0x1A000E))
#define MCFGPTA_GPTFLG2		(*(vu_char *)(CONFIG_SYS_MBAR+0x1A000F))
#define MCFGPTA_GPTC0		(*(vu_short *)(CONFIG_SYS_MBAR+0x1A0010))
#define MCFGPTA_GPTC1		(*(vu_short *)(CONFIG_SYS_MBAR+0x1A0012))
#define MCFGPTA_GPTC2		(*(vu_short *)(CONFIG_SYS_MBAR+0x1A0014))
#define MCFGPTA_GPTC3		(*(vu_short *)(CONFIG_SYS_MBAR+0x1A0016))
#define MCFGPTA_GPTPACTL	(*(vu_char *)(CONFIG_SYS_MBAR+0x1A0018))
#define MCFGPTA_GPTPAFLG	(*(vu_char *)(CONFIG_SYS_MBAR+0x1A0019))
#define MCFGPTA_GPTPACNT	(*(vu_short *)(CONFIG_SYS_MBAR+0x1A001A))
#define MCFGPTA_GPTPORT		(*(vu_char *)(CONFIG_SYS_MBAR+0x1A001D))
#define MCFGPTA_GPTDDR		(*(vu_char *)(CONFIG_SYS_MBAR+0x1A001E))

#define MCFGPTB_GPTIOS		(*(vu_char *)(CONFIG_SYS_MBAR+0x1B0000))
#define MCFGPTB_GPTCFORC	(*(vu_char *)(CONFIG_SYS_MBAR+0x1B0001))
#define MCFGPTB_GPTOC3M		(*(vu_char *)(CONFIG_SYS_MBAR+0x1B0002))
#define MCFGPTB_GPTOC3D		(*(vu_char *)(CONFIG_SYS_MBAR+0x1B0003))
#define MCFGPTB_GPTCNT		(*(vu_short *)(CONFIG_SYS_MBAR+0x1B0004))
#define MCFGPTB_GPTSCR1		(*(vu_char *)(CONFIG_SYS_MBAR+0x1B0006))
#define MCFGPTB_GPTTOV		(*(vu_char *)(CONFIG_SYS_MBAR+0x1B0008))
#define MCFGPTB_GPTCTL1		(*(vu_char *)(CONFIG_SYS_MBAR+0x1B0009))
#define MCFGPTB_GPTCTL2		(*(vu_char *)(CONFIG_SYS_MBAR+0x1B000B))
#define MCFGPTB_GPTIE		(*(vu_char *)(CONFIG_SYS_MBAR+0x1B000C))
#define MCFGPTB_GPTSCR2		(*(vu_char *)(CONFIG_SYS_MBAR+0x1B000D))
#define MCFGPTB_GPTFLG1		(*(vu_char *)(CONFIG_SYS_MBAR+0x1B000E))
#define MCFGPTB_GPTFLG2		(*(vu_char *)(CONFIG_SYS_MBAR+0x1B000F))
#define MCFGPTB_GPTC0		(*(vu_short *)(CONFIG_SYS_MBAR+0x1B0010))
#define MCFGPTB_GPTC1		(*(vu_short *)(CONFIG_SYS_MBAR+0x1B0012))
#define MCFGPTB_GPTC2		(*(vu_short *)(CONFIG_SYS_MBAR+0x1B0014))
#define MCFGPTB_GPTC3		(*(vu_short *)(CONFIG_SYS_MBAR+0x1B0016))
#define MCFGPTB_GPTPACTL	(*(vu_char *)(CONFIG_SYS_MBAR+0x1B0018))
#define MCFGPTB_GPTPAFLG	(*(vu_char *)(CONFIG_SYS_MBAR+0x1B0019))
#define MCFGPTB_GPTPACNT	(*(vu_short *)(CONFIG_SYS_MBAR+0x1B001A))
#define MCFGPTB_GPTPORT		(*(vu_char *)(CONFIG_SYS_MBAR+0x1B001D))
#define MCFGPTB_GPTDDR		(*(vu_char *)(CONFIG_SYS_MBAR+0x1B001E))

/* Bit level definitions and macros */
#define MCFGPT_GPTIOS_IOS3		(0x08)
#define MCFGPT_GPTIOS_IOS2		(0x04)
#define MCFGPT_GPTIOS_IOS1		(0x02)
#define MCFGPT_GPTIOS_IOS0		(0x01)

#define MCFGPT_GPTCFORC_FOC3		(0x08)
#define MCFGPT_GPTCFORC_FOC2		(0x04)
#define MCFGPT_GPTCFORC_FOC1		(0x02)
#define MCFGPT_GPTCFORC_FOC0		(0x01)

#define MCFGPT_GPTOC3M_OC3M3		(0x08)
#define MCFGPT_GPTOC3M_OC3M2		(0x04)
#define MCFGPT_GPTOC3M_OC3M1		(0x02)
#define MCFGPT_GPTOC3M_OC3M0		(0x01)

#define MCFGPT_GPTOC3M_OC3D(x)		(((x)&0x04))

#define MCFGPT_GPTSCR1_GPTEN		(0x80)
#define MCFGPT_GPTSCR1_TFFCA		(0x10)

#define MCFGPT_GPTTOV3			(0x08)
#define MCFGPT_GPTTOV2			(0x04)
#define MCFGPT_GPTTOV1			(0x02)
#define MCFGPT_GPTTOV0			(0x01)

#define MCFGPT_GPTCTL_OMOL3(x)		(((x)&0x03)<<6)
#define MCFGPT_GPTCTL_OMOL2(x)		(((x)&0x03)<<4)
#define MCFGPT_GPTCTL_OMOL1(x)		(((x)&0x03)<<2)
#define MCFGPT_GPTCTL_OMOL0(x)		(((x)&0x03))

#define MCFGPT_GPTCTL2_EDG3(x)		(((x)&0x03)<<6)
#define MCFGPT_GPTCTL2_EDG2(x)		(((x)&0x03)<<4)
#define MCFGPT_GPTCTL2_EDG1(x)		(((x)&0x03)<<2)
#define MCFGPT_GPTCTL2_EDG0(x)		(((x)&0x03))

#define MCFGPT_GPTIE_C3I		(0x08)
#define MCFGPT_GPTIE_C2I		(0x04)
#define MCFGPT_GPTIE_C1I		(0x02)
#define MCFGPT_GPTIE_C0I		(0x01)

#define MCFGPT_GPTSCR2_TOI		(0x80)
#define MCFGPT_GPTSCR2_PUPT		(0x20)
#define MCFGPT_GPTSCR2_RDPT		(0x10)
#define MCFGPT_GPTSCR2_TCRE		(0x08)
#define MCFGPT_GPTSCR2_PR(x)		(((x)&0x07))

#define MCFGPT_GPTFLG1_C3F		(0x08)
#define MCFGPT_GPTFLG1_C2F		(0x04)
#define MCFGPT_GPTFLG1_C1F		(0x02)
#define MCFGPT_GPTFLG1_C0F		(0x01)

#define MCFGPT_GPTFLG2_TOF		(0x80)
#define MCFGPT_GPTFLG2_C3F		(0x08)
#define MCFGPT_GPTFLG2_C2F		(0x04)
#define MCFGPT_GPTFLG2_C1F		(0x02)
#define MCFGPT_GPTFLG2_C0F		(0x01)

#define MCFGPT_GPTPACTL_PAE		(0x40)
#define MCFGPT_GPTPACTL_PAMOD		(0x20)
#define MCFGPT_GPTPACTL_PEDGE		(0x10)
#define MCFGPT_GPTPACTL_CLK_PACLK	(0x04)
#define MCFGPT_GPTPACTL_CLK_PACLK256	(0x08)
#define MCFGPT_GPTPACTL_CLK_PACLK65536	(0x0C)
#define MCFGPT_GPTPACTL_CLK(x)		(((x)&0x03)<<2)
#define MCFGPT_GPTPACTL_PAOVI		(0x02)
#define MCFGPT_GPTPACTL_PAI		(0x01)

#define MCFGPT_GPTPAFLG_PAOVF		(0x02)
#define MCFGPT_GPTPAFLG_PAIF		(0x01)

#define MCFGPT_GPTPORT_PORTT3		(0x08)
#define MCFGPT_GPTPORT_PORTT2		(0x04)
#define MCFGPT_GPTPORT_PORTT1		(0x02)
#define MCFGPT_GPTPORT_PORTT0		(0x01)

#define MCFGPT_GPTDDR_DDRT3		(0x08)
#define MCFGPT_GPTDDR_DDRT2		(0x04)
#define MCFGPT_GPTDDR_DDRT1		(0x02)
#define MCFGPT_GPTDDR_DDRT0		(0x01)

/* Coldfire Flash Module CFM */

#define MCFCFM_MCR			(*(vu_short *)(CONFIG_SYS_MBAR+0x1D0000))
#define MCFCFM_MCR_LOCK			(0x0400)
#define MCFCFM_MCR_PVIE			(0x0200)
#define MCFCFM_MCR_AEIE			(0x0100)
#define MCFCFM_MCR_CBEIE		(0x0080)
#define MCFCFM_MCR_CCIE			(0x0040)
#define MCFCFM_MCR_KEYACC		(0x0020)

#define MCFCFM_CLKD			(*(vu_char *)(CONFIG_SYS_MBAR+0x1D0002))

#define MCFCFM_SEC			(*(vu_long*) (CONFIG_SYS_MBAR+0x1D0008))
#define MCFCFM_SEC_KEYEN		(0x80000000)
#define MCFCFM_SEC_SECSTAT		(0x40000000)

#define MCFCFM_PROT			(*(vu_long*) (CONFIG_SYS_MBAR+0x1D0010))
#define MCFCFM_SACC			(*(vu_long*) (CONFIG_SYS_MBAR+0x1D0014))
#define MCFCFM_DACC			(*(vu_long*) (CONFIG_SYS_MBAR+0x1D0018))
#define MCFCFM_USTAT			(*(vu_char*) (CONFIG_SYS_MBAR+0x1D0020))
#define MCFCFM_USTAT_CBEIF		0x80
#define MCFCFM_USTAT_CCIF		0x40
#define MCFCFM_USTAT_PVIOL		0x20
#define MCFCFM_USTAT_ACCERR		0x10
#define MCFCFM_USTAT_BLANK		0x04

#define MCFCFM_CMD			(*(vu_char*) (CONFIG_SYS_MBAR+0x1D0024))
#define MCFCFM_CMD_ERSVER		0x05
#define MCFCFM_CMD_PGERSVER		0x06
#define MCFCFM_CMD_PGM			0x20
#define MCFCFM_CMD_PGERS		0x40
#define MCFCFM_CMD_MASERS		0x41

/****************************************************************************/
#endif				/* m5282_h */
