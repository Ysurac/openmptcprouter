/*
 * Copyright (C) 2010 Dirk Behme <dirk.behme@googlemail.com>
 *
 * Driver for McSPI controller on OMAP3. Based on davinci_spi.c
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Copyright (C) 2007 Atmel Corporation
 *
 * Parts taken from linux/drivers/spi/omap2_mcspi.c
 * Copyright (C) 2005, 2006 Nokia Corporation
 *
 * Modified by Ruslan Araslanov <ruslan.araslanov@vitecmm.com>
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
 *
 */

#include <common.h>
#include <spi.h>
#include <malloc.h>
#include <asm/io.h>
#include "omap3_spi.h"

#define WORD_LEN	8
#define SPI_WAIT_TIMEOUT 3000000;

static void spi_reset(struct omap3_spi_slave *ds)
{
	unsigned int tmp;

	writel(OMAP3_MCSPI_SYSCONFIG_SOFTRESET, &ds->regs->sysconfig);
	do {
		tmp = readl(&ds->regs->sysstatus);
	} while (!(tmp & OMAP3_MCSPI_SYSSTATUS_RESETDONE));

	writel(OMAP3_MCSPI_SYSCONFIG_AUTOIDLE |
				 OMAP3_MCSPI_SYSCONFIG_ENAWAKEUP |
				 OMAP3_MCSPI_SYSCONFIG_SMARTIDLE,
				 &ds->regs->sysconfig);

	writel(OMAP3_MCSPI_WAKEUPENABLE_WKEN, &ds->regs->wakeupenable);
}

void spi_init()
{
	/* do nothing */
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
				  unsigned int max_hz, unsigned int mode)
{
	struct omap3_spi_slave	*ds;

	ds = malloc(sizeof(struct omap3_spi_slave));
	if (!ds) {
		printf("SPI error: malloc of SPI structure failed\n");
		return NULL;
	}

	/*
	 * OMAP3 McSPI (MultiChannel SPI) has 4 busses (modules)
	 * with different number of chip selects (CS, channels):
	 * McSPI1 has 4 CS (bus 0, cs 0 - 3)
	 * McSPI2 has 2 CS (bus 1, cs 0 - 1)
	 * McSPI3 has 2 CS (bus 2, cs 0 - 1)
	 * McSPI4 has 1 CS (bus 3, cs 0)
	 */

	switch (bus) {
	case 0:
		ds->regs = (struct mcspi *)OMAP3_MCSPI1_BASE;
		break;
	case 1:
		ds->regs = (struct mcspi *)OMAP3_MCSPI2_BASE;
		break;
	case 2:
		ds->regs = (struct mcspi *)OMAP3_MCSPI3_BASE;
		break;
	case 3:
		ds->regs = (struct mcspi *)OMAP3_MCSPI4_BASE;
		break;
	default:
		printf("SPI error: unsupported bus %i. \
			Supported busses 0 - 3\n", bus);
		return NULL;
	}
	ds->slave.bus = bus;

	if (((bus == 0) && (cs > 3)) ||
			((bus == 1) && (cs > 1)) ||
			((bus == 2) && (cs > 1)) ||
			((bus == 3) && (cs > 0))) {
		printf("SPI error: unsupported chip select %i \
			on bus %i\n", cs, bus);
		return NULL;
	}
	ds->slave.cs = cs;

	if (max_hz > OMAP3_MCSPI_MAX_FREQ) {
		printf("SPI error: unsupported frequency %i Hz. \
			Max frequency is 48 Mhz\n", max_hz);
		return NULL;
	}
	ds->freq = max_hz;

	if (mode > SPI_MODE_3) {
		printf("SPI error: unsupported SPI mode %i\n", mode);
		return NULL;
	}
	ds->mode = mode;

	return &ds->slave;
}

void spi_free_slave(struct spi_slave *slave)
{
	struct omap3_spi_slave *ds = to_omap3_spi(slave);

	free(ds);
}

int spi_claim_bus(struct spi_slave *slave)
{
	struct omap3_spi_slave *ds = to_omap3_spi(slave);
	unsigned int conf, div = 0;

	/* McSPI global module configuration */

	/*
	 * setup when switching from (reset default) slave mode
	 * to single-channel master mode
	 */
	spi_reset(ds);
	conf = readl(&ds->regs->modulctrl);
	conf &= ~(OMAP3_MCSPI_MODULCTRL_STEST | OMAP3_MCSPI_MODULCTRL_MS);
	conf |= OMAP3_MCSPI_MODULCTRL_SINGLE;
	writel(conf, &ds->regs->modulctrl);

	/* McSPI individual channel configuration */

	/* Calculate clock divisor. Valid range: 0x0 - 0xC ( /1 - /4096 ) */
	if (ds->freq) {
		while (div <= 0xC && (OMAP3_MCSPI_MAX_FREQ / (1 << div))
					 > ds->freq)
			div++;
	} else
		div = 0xC;

	conf = readl(&ds->regs->channel[ds->slave.cs].chconf);

	/* standard 4-wire master mode:	SCK, MOSI/out, MISO/in, nCS
	 * REVISIT: this controller could support SPI_3WIRE mode.
	 */
	conf &= ~(OMAP3_MCSPI_CHCONF_IS|OMAP3_MCSPI_CHCONF_DPE1);
	conf |= OMAP3_MCSPI_CHCONF_DPE0;

	/* wordlength */
	conf &= ~OMAP3_MCSPI_CHCONF_WL_MASK;
	conf |= (WORD_LEN - 1) << 7;

	/* set chipselect polarity; manage with FORCE */
	if (!(ds->mode & SPI_CS_HIGH))
		conf |= OMAP3_MCSPI_CHCONF_EPOL; /* active-low; normal */
	else
		conf &= ~OMAP3_MCSPI_CHCONF_EPOL;

	/* set clock divisor */
	conf &= ~OMAP3_MCSPI_CHCONF_CLKD_MASK;
	conf |= div << 2;

	/* set SPI mode 0..3 */
	if (ds->mode & SPI_CPOL)
		conf |= OMAP3_MCSPI_CHCONF_POL;
	else
		conf &= ~OMAP3_MCSPI_CHCONF_POL;
	if (ds->mode & SPI_CPHA)
		conf |= OMAP3_MCSPI_CHCONF_PHA;
	else
		conf &= ~OMAP3_MCSPI_CHCONF_PHA;

	/* Transmit & receive mode */
	conf &= ~OMAP3_MCSPI_CHCONF_TRM_MASK;

	writel(conf, &ds->regs->channel[ds->slave.cs].chconf);

	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	struct omap3_spi_slave *ds = to_omap3_spi(slave);

	/* Reset the SPI hardware */
	spi_reset(ds);
}

int omap3_spi_write(struct spi_slave *slave, unsigned int len, const u8 *txp,
		    unsigned long flags)
{
	struct omap3_spi_slave *ds = to_omap3_spi(slave);
	int i;
	int timeout = SPI_WAIT_TIMEOUT;
	int chconf = readl(&ds->regs->channel[ds->slave.cs].chconf);

	if (flags & SPI_XFER_BEGIN)
		writel(OMAP3_MCSPI_CHCTRL_EN,
		       &ds->regs->channel[ds->slave.cs].chctrl);

	chconf &= ~OMAP3_MCSPI_CHCONF_TRM_MASK;
	chconf |= OMAP3_MCSPI_CHCONF_TRM_TX_ONLY;
	chconf |= OMAP3_MCSPI_CHCONF_FORCE;
	writel(chconf, &ds->regs->channel[ds->slave.cs].chconf);

	for (i = 0; i < len; i++) {
		/* wait till TX register is empty (TXS == 1) */
		while (!(readl(&ds->regs->channel[ds->slave.cs].chstat) &
			 OMAP3_MCSPI_CHSTAT_TXS)) {
			if (--timeout <= 0) {
				printf("SPI TXS timed out, status=0x%08x\n",
				       readl(&ds->regs->channel[ds->slave.cs].chstat));
				return -1;
			}
		}
		/* Write the data */
		writel(txp[i], &ds->regs->channel[ds->slave.cs].tx);
	}

	if (flags & SPI_XFER_END) {
		/* wait to finish of transfer */
		while (!(readl(&ds->regs->channel[ds->slave.cs].chstat) &
			 OMAP3_MCSPI_CHSTAT_EOT));

		chconf &= ~OMAP3_MCSPI_CHCONF_FORCE;
		writel(chconf, &ds->regs->channel[ds->slave.cs].chconf);

		writel(0, &ds->regs->channel[ds->slave.cs].chctrl);
	}
	return 0;
}

int omap3_spi_read(struct spi_slave *slave, unsigned int len, u8 *rxp,
		   unsigned long flags)
{
	struct omap3_spi_slave *ds = to_omap3_spi(slave);
	int i;
	int timeout = SPI_WAIT_TIMEOUT;
	int chconf = readl(&ds->regs->channel[ds->slave.cs].chconf);

	if (flags & SPI_XFER_BEGIN)
		writel(OMAP3_MCSPI_CHCTRL_EN,
		       &ds->regs->channel[ds->slave.cs].chctrl);

	chconf &= ~OMAP3_MCSPI_CHCONF_TRM_MASK;
	chconf |= OMAP3_MCSPI_CHCONF_TRM_RX_ONLY;
	chconf |= OMAP3_MCSPI_CHCONF_FORCE;
	writel(chconf, &ds->regs->channel[ds->slave.cs].chconf);

	writel(0, &ds->regs->channel[ds->slave.cs].tx);

	for (i = 0; i < len; i++) {
		/* Wait till RX register contains data (RXS == 1) */
		while (!(readl(&ds->regs->channel[ds->slave.cs].chstat) &
			 OMAP3_MCSPI_CHSTAT_RXS)) {
			if (--timeout <= 0) {
				printf("SPI RXS timed out, status=0x%08x\n",
				       readl(&ds->regs->channel[ds->slave.cs].chstat));
				return -1;
			}
		}
		/* Read the data */
		rxp[i] = readl(&ds->regs->channel[ds->slave.cs].rx);
	}

	if (flags & SPI_XFER_END) {
		chconf &= ~OMAP3_MCSPI_CHCONF_FORCE;
		writel(chconf, &ds->regs->channel[ds->slave.cs].chconf);

		writel(0, &ds->regs->channel[ds->slave.cs].chctrl);
	}

	return 0;
}

/*McSPI Transmit Receive Mode*/
int omap3_spi_txrx(struct spi_slave *slave,
		unsigned int len, const u8 *txp, u8 *rxp, unsigned long flags)
{
	struct omap3_spi_slave *ds = to_omap3_spi(slave);
	int timeout = SPI_WAIT_TIMEOUT;
	int chconf = readl(&ds->regs->channel[ds->slave.cs].chconf);
	int irqstatus = readl(&ds->regs->irqstatus);
	int i=0;

	/*Enable SPI channel*/
	if (flags & SPI_XFER_BEGIN)
		writel(OMAP3_MCSPI_CHCTRL_EN,
		       &ds->regs->channel[ds->slave.cs].chctrl);

	/*set TRANSMIT-RECEIVE Mode*/
	chconf &= ~OMAP3_MCSPI_CHCONF_TRM_MASK;
	chconf |= OMAP3_MCSPI_CHCONF_FORCE;
	writel(chconf, &ds->regs->channel[ds->slave.cs].chconf);

	/*Shift in and out 1 byte at time*/
	for (i=0; i < len; i++){
		/* Write: wait for TX empty (TXS == 1)*/
		irqstatus |= (1<< (4*(ds->slave.bus)));
		while (!(readl(&ds->regs->channel[ds->slave.cs].chstat) &
			 OMAP3_MCSPI_CHSTAT_TXS)) {
			if (--timeout <= 0) {
				printf("SPI TXS timed out, status=0x%08x\n",
				       readl(&ds->regs->channel[ds->slave.cs].chstat));
				return -1;
			}
		}
		/* Write the data */
		writel(txp[i], &ds->regs->channel[ds->slave.cs].tx);

		/*Read: wait for RX containing data (RXS == 1)*/
		while (!(readl(&ds->regs->channel[ds->slave.cs].chstat) &
			 OMAP3_MCSPI_CHSTAT_RXS)) {
			if (--timeout <= 0) {
				printf("SPI RXS timed out, status=0x%08x\n",
				       readl(&ds->regs->channel[ds->slave.cs].chstat));
				return -1;
			}
		}
		/* Read the data */
		rxp[i] = readl(&ds->regs->channel[ds->slave.cs].rx);
	}

	/*if transfer must be terminated disable the channel*/
	if (flags & SPI_XFER_END) {
		chconf &= ~OMAP3_MCSPI_CHCONF_FORCE;
		writel(chconf, &ds->regs->channel[ds->slave.cs].chconf);

		writel(0, &ds->regs->channel[ds->slave.cs].chctrl);
	}

	return 0;
}

int spi_xfer(struct spi_slave *slave, unsigned int bitlen,
	     const void *dout, void *din, unsigned long flags)
{
	struct omap3_spi_slave *ds = to_omap3_spi(slave);
	unsigned int	len;
	const u8	*txp = dout;
	u8		*rxp = din;
	int ret = -1;

	if (bitlen % 8)
		return -1;

	len = bitlen / 8;

	if (bitlen == 0) {	 /* only change CS */
		int chconf = readl(&ds->regs->channel[ds->slave.cs].chconf);

		if (flags & SPI_XFER_BEGIN) {
			writel(OMAP3_MCSPI_CHCTRL_EN,
			       &ds->regs->channel[ds->slave.cs].chctrl);
			chconf |= OMAP3_MCSPI_CHCONF_FORCE;
			writel(chconf,
			       &ds->regs->channel[ds->slave.cs].chconf);
		}
		if (flags & SPI_XFER_END) {
			chconf &= ~OMAP3_MCSPI_CHCONF_FORCE;
			writel(chconf,
			       &ds->regs->channel[ds->slave.cs].chconf);
			writel(0, &ds->regs->channel[ds->slave.cs].chctrl);
		}
		ret = 0;
	} else {
		if (dout != NULL && din != NULL)
			ret = omap3_spi_txrx(slave, len, txp, rxp, flags);
		else if (dout != NULL)
			ret = omap3_spi_write(slave, len, txp, flags);
		else if (din != NULL)
			ret = omap3_spi_read(slave, len, rxp, flags);
	}
	return ret;
}

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return 1;
}

void spi_cs_activate(struct spi_slave *slave)
{
}

void spi_cs_deactivate(struct spi_slave *slave)
{
}
