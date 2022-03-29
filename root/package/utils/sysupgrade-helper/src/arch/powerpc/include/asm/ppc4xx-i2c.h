/*
 * (C) Copyright 2007-2009
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
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

#ifndef _4xx_i2c_h_
#define _4xx_i2c_h_

#define IIC_OK		0
#define IIC_NOK		1
#define IIC_NOK_LA	2		/* Lost arbitration */
#define IIC_NOK_ICT	3		/* Incomplete transfer */
#define IIC_NOK_XFRA	4		/* Transfer aborted */
#define IIC_NOK_DATA	5		/* No data in buffer */
#define IIC_NOK_TOUT	6		/* Transfer timeout */

#define IIC_TIMEOUT	1		/* 1 second */

#if defined(CONFIG_I2C_MULTI_BUS)
#define I2C_BUS_OFFS	(i2c_bus_num * 0x100)
#else
#define I2C_BUS_OFFS	(0x000)
#endif /* CONFIG_I2C_MULTI_BUS */

#if defined(CONFIG_440EP) || defined(CONFIG_440GR) || \
    defined(CONFIG_440EPX) || defined(CONFIG_440GRX) || \
    defined(CONFIG_460EX) || defined(CONFIG_460GT)
#define I2C_BASE_ADDR	(CONFIG_SYS_PERIPHERAL_BASE + 0x00000700 + I2C_BUS_OFFS)
#elif defined(CONFIG_440) || defined(CONFIG_405EX)
/* all remaining 440 variants */
#define I2C_BASE_ADDR	(CONFIG_SYS_PERIPHERAL_BASE + 0x00000400 + I2C_BUS_OFFS)
#else
/* all 405 variants */
#define I2C_BASE_ADDR	(0xEF600500 + I2C_BUS_OFFS)
#endif

struct ppc4xx_i2c {
	u8 mdbuf;
	u8 res1;
	u8 sdbuf;
	u8 res2;
	u8 lmadr;
	u8 hmadr;
	u8 cntl;
	u8 mdcntl;
	u8 sts;
	u8 extsts;
	u8 lsadr;
	u8 hsadr;
	u8 clkdiv;
	u8 intrmsk;
	u8 xfrcnt;
	u8 xtcntlss;
	u8 directcntl;
	u8 intr;
};

/* MDCNTL Register Bit definition */
#define IIC_MDCNTL_HSCL		0x01
#define IIC_MDCNTL_EUBS		0x02
#define IIC_MDCNTL_EINT		0x04
#define IIC_MDCNTL_ESM		0x08
#define IIC_MDCNTL_FSM		0x10
#define IIC_MDCNTL_EGC		0x20
#define IIC_MDCNTL_FMDB		0x40
#define IIC_MDCNTL_FSDB		0x80

/* CNTL Register Bit definition */
#define IIC_CNTL_PT		0x01
#define IIC_CNTL_READ		0x02
#define IIC_CNTL_CHT		0x04
#define IIC_CNTL_RPST		0x08
/* bit 2/3 for Transfer count*/
#define IIC_CNTL_AMD		0x40
#define IIC_CNTL_HMT		0x80

/* STS Register Bit definition */
#define IIC_STS_PT		0x01
#define IIC_STS_IRQA		0x02
#define IIC_STS_ERR		0x04
#define IIC_STS_SCMP		0x08
#define IIC_STS_MDBF		0x10
#define IIC_STS_MDBS		0x20
#define IIC_STS_SLPR		0x40
#define IIC_STS_SSS		0x80

/* EXTSTS Register Bit definition */
#define IIC_EXTSTS_XFRA		0x01
#define IIC_EXTSTS_ICT		0x02
#define IIC_EXTSTS_LA		0x04

/* XTCNTLSS Register Bit definition */
#define IIC_XTCNTLSS_SRST	0x01
#define IIC_XTCNTLSS_EPI	0x02
#define IIC_XTCNTLSS_SDBF	0x04
#define IIC_XTCNTLSS_SBDD	0x08
#define IIC_XTCNTLSS_SWS	0x10
#define IIC_XTCNTLSS_SWC	0x20
#define IIC_XTCNTLSS_SRS	0x40
#define IIC_XTCNTLSS_SRC	0x80

/* IICx_DIRECTCNTL register */
#define IIC_DIRCNTL_SDAC	0x08
#define IIC_DIRCNTL_SCC		0x04
#define IIC_DIRCNTL_MSDA	0x02
#define IIC_DIRCNTL_MSC		0x01

#define DIRCTNL_FREE(v)		(((v) & 0x0f) == 0x0f)
#endif
