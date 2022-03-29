/*
 * Copyright (C) 2004-2006 Atmel Corporation
 *
 * Modified to support C structur SoC access by
 * Andreas Bießmann <biessmann@corscience.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <common.h>
#include <watchdog.h>

#include <asm/io.h>
#include <asm/arch/clk.h>
#include <asm/arch/hardware.h>

#include "atmel_usart.h"

DECLARE_GLOBAL_DATA_PTR;

void serial_setbrg(void)
{
	atmel_usart3_t *usart = (atmel_usart3_t *)CONFIG_USART_BASE;
	unsigned long divisor;
	unsigned long usart_hz;

	/*
	 *              Master Clock
	 * Baud Rate = --------------
	 *                16 * CD
	 */
	usart_hz = get_usart_clk_rate(CONFIG_USART_ID);
	divisor = (usart_hz / 16 + gd->baudrate / 2) / gd->baudrate;
	writel(USART3_BF(CD, divisor), &usart->brgr);
}

int serial_init(void)
{
	atmel_usart3_t *usart = (atmel_usart3_t *)CONFIG_USART_BASE;

	writel(USART3_BIT(RSTRX) | USART3_BIT(RSTTX), &usart->cr);

	serial_setbrg();

	writel(USART3_BIT(RXEN) | USART3_BIT(TXEN), &usart->cr);
	writel((USART3_BF(USART_MODE, USART3_USART_MODE_NORMAL)
			   | USART3_BF(USCLKS, USART3_USCLKS_MCK)
			   | USART3_BF(CHRL, USART3_CHRL_8)
			   | USART3_BF(PAR, USART3_PAR_NONE)
			   | USART3_BF(NBSTOP, USART3_NBSTOP_1)),
			   &usart->mr);

	return 0;
}

void serial_putc(char c)
{
	atmel_usart3_t *usart = (atmel_usart3_t *)CONFIG_USART_BASE;

	if (c == '\n')
		serial_putc('\r');

	while (!(readl(&usart->csr) & USART3_BIT(TXRDY)));
	writel(c, &usart->thr);
}

void serial_puts(const char *s)
{
	while (*s)
		serial_putc(*s++);
}

int serial_getc(void)
{
	atmel_usart3_t *usart = (atmel_usart3_t *)CONFIG_USART_BASE;

	while (!(readl(&usart->csr) & USART3_BIT(RXRDY)))
		 WATCHDOG_RESET();
	return readl(&usart->rhr);
}

int serial_tstc(void)
{
	atmel_usart3_t *usart = (atmel_usart3_t *)CONFIG_USART_BASE;
	return (readl(&usart->csr) & USART3_BIT(RXRDY)) != 0;
}
