/*
 * (C) Copyright 2003
 * Texas Instruments <www.ti.com>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * (C) Copyright 2002-2004
 * Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
 *
 * (C) Copyright 2004
 * Philippe Robin, ARM Ltd. <philippe.robin@arm.com>
 *
 * Copyright (C) 2007 Sergey Kubushyn <ksi@koi8.net>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/timer_defs.h>
#include <div64.h>

DECLARE_GLOBAL_DATA_PTR;

static struct davinci_timer * const timer =
	(struct davinci_timer *)CONFIG_SYS_TIMERBASE;

#define TIMER_LOAD_VAL	0xffffffff

#define TIM_CLK_DIV	16

int timer_init(void)
{
	/* We are using timer34 in unchained 32-bit mode, full speed */
	writel(0x0, &timer->tcr);
	writel(0x0, &timer->tgcr);
	writel(0x06 | ((TIM_CLK_DIV - 1) << 8), &timer->tgcr);
	writel(0x0, &timer->tim34);
	writel(TIMER_LOAD_VAL, &timer->prd34);
	writel(2 << 22, &timer->tcr);
	gd->timer_rate_hz = CONFIG_SYS_HZ_CLOCK / TIM_CLK_DIV;
	gd->timer_reset_value = 0;

	return(0);
}

/*
 * Get the current 64 bit timer tick count
 */
unsigned long long get_ticks(void)
{
	unsigned long now = readl(&timer->tim34);

	/* increment tbu if tbl has rolled over */
	if (now < gd->tbl)
		gd->tbu++;
	gd->tbl = now;

	return (((unsigned long long)gd->tbu) << 32) | gd->tbl;
}

ulong get_timer(ulong base)
{
	unsigned long long timer_diff;

	timer_diff = get_ticks() - gd->timer_reset_value;

	return lldiv(timer_diff, (gd->timer_rate_hz / CONFIG_SYS_HZ)) - base;
}

void __udelay(unsigned long usec)
{
	unsigned long long endtime;

	endtime = lldiv((unsigned long long)usec * gd->timer_rate_hz,
			1000000UL);
	endtime += get_ticks();

	while (get_ticks() < endtime)
		;
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk(void)
{
	return gd->timer_rate_hz;
}

#ifdef CONFIG_HW_WATCHDOG
static struct davinci_timer * const wdttimer =
	(struct davinci_timer *)CONFIG_SYS_WDTTIMERBASE;

/*
 * See prufw2.pdf for using Timer as a WDT
 */
void davinci_hw_watchdog_enable(void)
{
	writel(0x0, &wdttimer->tcr);
	writel(0x0, &wdttimer->tgcr);
	/* TIMMODE = 2h */
	writel(0x08 | 0x03 | ((TIM_CLK_DIV - 1) << 8), &wdttimer->tgcr);
	writel(CONFIG_SYS_WDT_PERIOD_LOW, &wdttimer->prd12);
	writel(CONFIG_SYS_WDT_PERIOD_HIGH, &wdttimer->prd34);
	writel(2 << 22, &wdttimer->tcr);
	writel(0x0, &wdttimer->tim12);
	writel(0x0, &wdttimer->tim34);
	/* set WDEN bit, WDKEY 0xa5c6 */
	writel(0xa5c64000, &wdttimer->wdtcr);
	/* clear counter register */
	writel(0xda7e4000, &wdttimer->wdtcr);
}

void davinci_hw_watchdog_reset(void)
{
	writel(0xa5c64000, &wdttimer->wdtcr);
	writel(0xda7e4000, &wdttimer->wdtcr);
}
#endif
