/*----------------------------------------------------------------------------+
|   This source code is dual-licensed.  You may use it under the terms of the
|   GNU General Public License version 2, or under the license below.
|
|	This source code has been made available to you by IBM on an AS-IS
|	basis.	Anyone receiving this source is licensed under IBM
|	copyrights to use it in any way he or she deems fit, including
|	copying it, modifying it, compiling it, and redistributing it either
|	with or without modifications.	No license under IBM patents or
|	patent applications is to be implied by the copyright license.
|
|	Any user of this software should understand that IBM cannot provide
|	technical support for this software and will not be responsible for
|	any consequences resulting from the use of this software.
|
|	Any person who transfers this source code or any derivative work
|	must include the IBM copyright notice, this paragraph, and the
|	preceding two paragraphs in the transferred software.
|
|	COPYRIGHT   I B M   CORPORATION 1999
|	LICENSED MATERIAL  -  PROGRAM PROPERTY OF I B M
+----------------------------------------------------------------------------*/

/*
 * (C) Copyright 2006
 * Sylvie Gohl,             AMCC/IBM, gohl.sylvie@fr.ibm.com
 * Jacqueline Pira-Ferriol, AMCC/IBM, jpira-ferriol@fr.ibm.com
 * Thierry Roman,           AMCC/IBM, thierry_roman@fr.ibm.com
 * Alain Saurel,            AMCC/IBM, alain.saurel@fr.ibm.com
 * Robert Snyder,           AMCC/IBM, rob.snyder@fr.ibm.com
 *
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __PPC440_H__
#define __PPC440_H__

#define CONFIG_SYS_DCACHE_SIZE		(32 << 10)	/* For AMCC 440 CPUs */

/*
 * DCRs & Related
 */

/* Memory mapped registers */
#define PCIL0_CFGADR	(CONFIG_SYS_PCI_BASE + 0x0ec00000)
#define PCIL0_CFGDATA	(CONFIG_SYS_PCI_BASE + 0x0ec00004)
#define PCIL0_CFGBASE	(CONFIG_SYS_PCI_BASE + 0x0ec80000)
#define PCIL0_IOBASE	(CONFIG_SYS_PCI_BASE + 0x08000000)

/* DCR registers */

/* CPR register declarations */
#define CPR0_PLLC	0x0040
#define CPR0_PLLD	0x0060
#define CPR0_PRIMAD0	0x0080
#define CPR0_PRIMBD0	0x00a0
#define CPR0_OPBD0	0x00c0
#define CPR0_PERD	0x00e0
#define CPR0_MALD	0x0100
#define CPR0_SPCID	0x0120
#define CPR0_ICFG	0x0140

/* SDR register definations */
#define SDR0_SDSTP0	0x0020
#define SDR0_SDSTP1	0x0021
#define SDR0_PINSTP	0x0040
#define SDR0_SDCS0	0x0060
#define SDR0_ECID0	0x0080
#define SDR0_ECID1	0x0081
#define SDR0_ECID2	0x0082
#define SDR0_ECID3	0x0083
#define SDR0_DDR0	0x00e1
#define SDR0_EBC	0x0100
#define SDR0_UART0	0x0120
#define SDR0_UART1	0x0121
#define SDR0_UART2	0x0122
#define SDR0_UART3	0x0123
#define SDR0_CP440	0x0180
#define SDR0_XCR	0x01c0
#define SDR0_XCR0	0x01c0
#define SDR0_XPLLC	0x01c1
#define SDR0_XPLLD	0x01c2
#define SDR0_SRST	0x0200
#define SDR0_SRST0	SDR0_SRST
#define SDR0_SRST1	0x0201
#define SDR0_AMP0	0x0240
#define SDR0_AMP1	0x0241
#define SDR0_USB0	0x0320
#define SDR0_CUST0	0x4000
#define SDR0_CUST1	0x4002
#define SDR0_CUST2	0x4004
#define SDR0_CUST3	0x4006
#define SDR0_PFC0	0x4100
#define SDR0_PFC1	0x4101
#define SDR0_PFC2   	0x4102
#define SDR0_PFC4	0x4104
#define SDR0_MFR	0x4300

#define SDR0_DDR0_DDRM_DECODE(n)	((((u32)(n)) >> 29) & 0x03)

#define SDR0_PCI0_PAE_MASK		(0x80000000 >> 0)
#define SDR0_XCR0_PAE_MASK		(0x80000000 >> 0)

#define SDR0_PFC0_GEIE_MASK		0x00003e00
#define SDR0_PFC0_GEIE_TRE		0x00003e00
#define SDR0_PFC0_GEIE_NOTRE		0x00000000
#define SDR0_PFC0_TRE_MASK		(0x80000000 >> 23)
#define SDR0_PFC0_TRE_DISABLE		0x00000000
#define SDR0_PFC0_TRE_ENABLE		(0x80000000 >> 23)

/*
 * Core Configuration/MMU configuration for 440
 */
#define CCR0_DAPUIB		0x00100000
#define CCR0_DTB		0x00008000

#define SDR0_SDCS_SDD		(0x80000000 >> 31)

/* todo: move this code from macro offsets to struct */
#define PCIL0_VENDID		(PCIL0_CFGBASE + PCI_VENDOR_ID )
#define PCIL0_DEVID		(PCIL0_CFGBASE + PCI_DEVICE_ID )
#define PCIL0_CMD		(PCIL0_CFGBASE + PCI_COMMAND )
#define PCIL0_STATUS		(PCIL0_CFGBASE + PCI_STATUS )
#define PCIL0_REVID		(PCIL0_CFGBASE + PCI_REVISION_ID )
#define PCIL0_CLS		(PCIL0_CFGBASE + PCI_CLASS_CODE)
#define PCIL0_CACHELS		(PCIL0_CFGBASE + PCI_CACHE_LINE_SIZE )
#define PCIL0_LATTIM		(PCIL0_CFGBASE + PCI_LATENCY_TIMER )
#define PCIL0_HDTYPE		(PCIL0_CFGBASE + PCI_HEADER_TYPE )
#define PCIL0_BIST		(PCIL0_CFGBASE + PCI_BIST )
#define PCIL0_BAR0		(PCIL0_CFGBASE + PCI_BASE_ADDRESS_0 )
#define PCIL0_BAR1		(PCIL0_CFGBASE + PCI_BASE_ADDRESS_1 )
#define PCIL0_BAR2		(PCIL0_CFGBASE + PCI_BASE_ADDRESS_2 )
#define PCIL0_BAR3		(PCIL0_CFGBASE + PCI_BASE_ADDRESS_3 )
#define PCIL0_BAR4		(PCIL0_CFGBASE + PCI_BASE_ADDRESS_4 )
#define PCIL0_BAR5		(PCIL0_CFGBASE + PCI_BASE_ADDRESS_5 )
#define PCIL0_CISPTR		(PCIL0_CFGBASE + PCI_CARDBUS_CIS )
#define PCIL0_SBSYSVID		(PCIL0_CFGBASE + PCI_SUBSYSTEM_VENDOR_ID )
#define PCIL0_SBSYSID		(PCIL0_CFGBASE + PCI_SUBSYSTEM_ID )
#define PCIL0_EROMBA		(PCIL0_CFGBASE + PCI_ROM_ADDRESS )
#define PCIL0_CAP		(PCIL0_CFGBASE + PCI_CAPABILITY_LIST )
#define PCIL0_RES0		(PCIL0_CFGBASE + 0x0035 )
#define PCIL0_RES1		(PCIL0_CFGBASE + 0x0036 )
#define PCIL0_RES2		(PCIL0_CFGBASE + 0x0038 )
#define PCIL0_INTLN		(PCIL0_CFGBASE + PCI_INTERRUPT_LINE )
#define PCIL0_INTPN		(PCIL0_CFGBASE + PCI_INTERRUPT_PIN )

#define PCIL0_MINGNT		(PCIL0_CFGBASE + PCI_MIN_GNT )
#define PCIL0_MAXLTNCY		(PCIL0_CFGBASE + PCI_MAX_LAT )

#define PCIL0_POM0LAL		(PCIL0_CFGBASE + 0x0068)
#define PCIL0_POM0LAH		(PCIL0_CFGBASE + 0x006c)
#define PCIL0_POM0SA		(PCIL0_CFGBASE + 0x0070)
#define PCIL0_POM0PCIAL		(PCIL0_CFGBASE + 0x0074)
#define PCIL0_POM0PCIAH		(PCIL0_CFGBASE + 0x0078)
#define PCIL0_POM1LAL		(PCIL0_CFGBASE + 0x007c)
#define PCIL0_POM1LAH		(PCIL0_CFGBASE + 0x0080)
#define PCIL0_POM1SA		(PCIL0_CFGBASE + 0x0084)
#define PCIL0_POM1PCIAL		(PCIL0_CFGBASE + 0x0088)
#define PCIL0_POM1PCIAH		(PCIL0_CFGBASE + 0x008c)
#define PCIL0_POM2SA		(PCIL0_CFGBASE + 0x0090)

#define PCIL0_PIM0SA		(PCIL0_CFGBASE + 0x0098)
#define PCIL0_PIM0LAL		(PCIL0_CFGBASE + 0x009c)
#define PCIL0_PIM0LAH		(PCIL0_CFGBASE + 0x00a0)
#define PCIL0_PIM1SA		(PCIL0_CFGBASE + 0x00a4)
#define PCIL0_PIM1LAL		(PCIL0_CFGBASE + 0x00a8)
#define PCIL0_PIM1LAH		(PCIL0_CFGBASE + 0x00ac)
#define PCIL0_PIM2SA		(PCIL0_CFGBASE + 0x00b0)
#define PCIL0_PIM2LAL		(PCIL0_CFGBASE + 0x00b4)
#define PCIL0_PIM2LAH		(PCIL0_CFGBASE + 0x00b8)

#define PCIL0_STS		(PCIL0_CFGBASE + 0x00e0)

#endif	/* __PPC440_H__ */
