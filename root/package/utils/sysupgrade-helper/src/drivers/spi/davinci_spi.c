/*
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Driver for SPI controller on DaVinci. Based on atmel_spi.c
 * by Atmel Corporation
 *
 * Copyright (C) 2007 Atmel Corporation
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
#include <common.h>
#include <spi.h>
#include <malloc.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include "davinci_spi.h"

void spi_init()
{
	/* do nothing */
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
			unsigned int max_hz, unsigned int mode)
{
	struct davinci_spi_slave	*ds;

	if (!spi_cs_is_valid(bus, cs))
		return NULL;

	ds = malloc(sizeof(*ds));
	if (!ds)
		return NULL;

	ds->slave.bus = bus;
	ds->slave.cs = cs;
	ds->regs = (struct davinci_spi_regs *)CONFIG_SYS_SPI_BASE;
	ds->freq = max_hz;

	return &ds->slave;
}

void spi_free_slave(struct spi_slave *slave)
{
	struct davinci_spi_slave *ds = to_davinci_spi(slave);

	free(ds);
}

int spi_claim_bus(struct spi_slave *slave)
{
	struct davinci_spi_slave *ds = to_davinci_spi(slave);
	unsigned int scalar;

	/* Enable the SPI hardware */
	writel(SPIGCR0_SPIRST_MASK, &ds->regs->gcr0);
	udelay(1000);
	writel(SPIGCR0_SPIENA_MASK, &ds->regs->gcr0);

	/* Set master mode, powered up and not activated */
	writel(SPIGCR1_MASTER_MASK | SPIGCR1_CLKMOD_MASK, &ds->regs->gcr1);

	/* CS, CLK, SIMO and SOMI are functional pins */
	writel((SPIPC0_EN0FUN_MASK | SPIPC0_CLKFUN_MASK |
		SPIPC0_DOFUN_MASK | SPIPC0_DIFUN_MASK), &ds->regs->pc0);

	/* setup format */
	scalar = ((CONFIG_SYS_SPI_CLK / ds->freq) - 1) & 0xFF;

	/*
	 * Use following format:
	 *   character length = 8,
	 *   clock signal delayed by half clk cycle,
	 *   clock low in idle state - Mode 0,
	 *   MSB shifted out first
	 */
	writel(8 | (scalar << SPIFMT_PRESCALE_SHIFT) |
		(1 << SPIFMT_PHASE_SHIFT), &ds->regs->fmt0);

	/*
	 * Including a minor delay. No science here. Should be good even with
	 * no delay
	 */
	writel((50 << SPI_C2TDELAY_SHIFT) |
		(50 << SPI_T2CDELAY_SHIFT), &ds->regs->delay);

	/* default chip select register */
	writel(SPIDEF_CSDEF0_MASK, &ds->regs->def);

	/* no interrupts */
	writel(0, &ds->regs->int0);
	writel(0, &ds->regs->lvl);

	/* enable SPI */
	writel((readl(&ds->regs->gcr1) | SPIGCR1_SPIENA_MASK), &ds->regs->gcr1);

	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	struct davinci_spi_slave *ds = to_davinci_spi(slave);

	/* Disable the SPI hardware */
	writel(SPIGCR0_SPIRST_MASK, &ds->regs->gcr0);
}

/*
 * This functions needs to act like a macro to avoid pipeline reloads in the
 * loops below. Use always_inline. This gains us about 160KiB/s and the bloat
 * appears to be zero bytes (da830).
 */
__attribute__((always_inline))
static inline u32 davinci_spi_xfer_data(struct davinci_spi_slave *ds, u32 data)
{
	u32	buf_reg_val;

	/* send out data */
	writel(data, &ds->regs->dat1);

	/* wait for the data to clock in/out */
	while ((buf_reg_val = readl(&ds->regs->buf)) & SPIBUF_RXEMPTY_MASK)
		;

	return buf_reg_val;
}

static int davinci_spi_read(struct spi_slave *slave, unsigned int len,
			    u8 *rxp, unsigned long flags)
{
	struct davinci_spi_slave *ds = to_davinci_spi(slave);
	unsigned int data1_reg_val;

	/* enable CS hold, CS[n] and clear the data bits */
	data1_reg_val = ((1 << SPIDAT1_CSHOLD_SHIFT) |
			 (slave->cs << SPIDAT1_CSNR_SHIFT));

	/* wait till TXFULL is deasserted */
	while (readl(&ds->regs->buf) & SPIBUF_TXFULL_MASK)
		;

	/* preload the TX buffer to avoid clock starvation */
	writel(data1_reg_val, &ds->regs->dat1);

	/* keep reading 1 byte until only 1 byte left */
	while ((len--) > 1)
		*rxp++ = davinci_spi_xfer_data(ds, data1_reg_val);

	/* clear CS hold when we reach the end */
	if (flags & SPI_XFER_END)
		data1_reg_val &= ~(1 << SPIDAT1_CSHOLD_SHIFT);

	/* read the last byte */
	*rxp = davinci_spi_xfer_data(ds, data1_reg_val);

	return 0;
}

static int davinci_spi_write(struct spi_slave *slave, unsigned int len,
			     const u8 *txp, unsigned long flags)
{
	struct davinci_spi_slave *ds = to_davinci_spi(slave);
	unsigned int data1_reg_val;

	/* enable CS hold and clear the data bits */
	data1_reg_val = ((1 << SPIDAT1_CSHOLD_SHIFT) |
			 (slave->cs << SPIDAT1_CSNR_SHIFT));

	/* wait till TXFULL is deasserted */
	while (readl(&ds->regs->buf) & SPIBUF_TXFULL_MASK)
		;

	/* preload the TX buffer to avoid clock starvation */
	if (len > 2) {
		writel(data1_reg_val | *txp++, &ds->regs->dat1);
		len--;
	}

	/* keep writing 1 byte until only 1 byte left */
	while ((len--) > 1)
		davinci_spi_xfer_data(ds, data1_reg_val | *txp++);

	/* clear CS hold when we reach the end */
	if (flags & SPI_XFER_END)
		data1_reg_val &= ~(1 << SPIDAT1_CSHOLD_SHIFT);

	/* write the last byte */
	davinci_spi_xfer_data(ds, data1_reg_val | *txp);

	return 0;
}

#ifndef CONFIG_SPI_HALF_DUPLEX
static int davinci_spi_read_write(struct spi_slave *slave, unsigned int len,
				  u8 *rxp, const u8 *txp, unsigned long flags)
{
	struct davinci_spi_slave *ds = to_davinci_spi(slave);
	unsigned int data1_reg_val;

	/* enable CS hold and clear the data bits */
	data1_reg_val = ((1 << SPIDAT1_CSHOLD_SHIFT) |
			 (slave->cs << SPIDAT1_CSNR_SHIFT));

	/* wait till TXFULL is deasserted */
	while (readl(&ds->regs->buf) & SPIBUF_TXFULL_MASK)
		;

	/* keep reading and writing 1 byte until only 1 byte left */
	while ((len--) > 1)
		*rxp++ = davinci_spi_xfer_data(ds, data1_reg_val | *txp++);

	/* clear CS hold when we reach the end */
	if (flags & SPI_XFER_END)
		data1_reg_val &= ~(1 << SPIDAT1_CSHOLD_SHIFT);

	/* read and write the last byte */
	*rxp = davinci_spi_xfer_data(ds, data1_reg_val | *txp);

	return 0;
}
#endif

int spi_xfer(struct spi_slave *slave, unsigned int bitlen,
	     const void *dout, void *din, unsigned long flags)
{
	unsigned int len;

	if (bitlen == 0)
		/* Finish any previously submitted transfers */
		goto out;

	/*
	 * It's not clear how non-8-bit-aligned transfers are supposed to be
	 * represented as a stream of bytes...this is a limitation of
	 * the current SPI interface - here we terminate on receiving such a
	 * transfer request.
	 */
	if (bitlen % 8) {
		/* Errors always terminate an ongoing transfer */
		flags |= SPI_XFER_END;
		goto out;
	}

	len = bitlen / 8;

	if (!dout)
		return davinci_spi_read(slave, len, din, flags);
	else if (!din)
		return davinci_spi_write(slave, len, dout, flags);
#ifndef CONFIG_SPI_HALF_DUPLEX
	else
		return davinci_spi_read_write(slave, len, din, dout, flags);
#else
	printf("SPI full duplex transaction requested with "
	       "CONFIG_SPI_HALF_DUPLEX defined.\n");
	flags |= SPI_XFER_END;
#endif

out:
	if (flags & SPI_XFER_END) {
		u8 dummy = 0;
		davinci_spi_write(slave, 1, &dummy, flags);
	}
	return 0;
}

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return bus == 0 && cs == 0;
}

void spi_cs_activate(struct spi_slave *slave)
{
	/* do nothing */
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	/* do nothing */
}
