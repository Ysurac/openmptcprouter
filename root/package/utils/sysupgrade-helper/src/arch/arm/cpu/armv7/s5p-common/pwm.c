/*
 * Copyright (C) 2011 Samsung Electronics
 *
 * Donghwa Lee <dh09.lee@samsung.com>
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
#include <errno.h>
#include <pwm.h>
#include <asm/io.h>
#include <asm/arch/pwm.h>
#include <asm/arch/clk.h>

int pwm_enable(int pwm_id)
{
	const struct s5p_timer *pwm =
			(struct s5p_timer *)samsung_get_base_timer();
	unsigned long tcon;

	tcon = readl(&pwm->tcon);
	tcon |= TCON_START(pwm_id);

	writel(tcon, &pwm->tcon);

	return 0;
}

void pwm_disable(int pwm_id)
{
	const struct s5p_timer *pwm =
			(struct s5p_timer *)samsung_get_base_timer();
	unsigned long tcon;

	tcon = readl(&pwm->tcon);
	tcon &= ~TCON_START(pwm_id);

	writel(tcon, &pwm->tcon);
}

static unsigned long pwm_calc_tin(int pwm_id, unsigned long freq)
{
	unsigned long tin_parent_rate;
	unsigned int div;

	tin_parent_rate = get_pwm_clk();

	for (div = 2; div <= 16; div *= 2) {
		if ((tin_parent_rate / (div << 16)) < freq)
			return tin_parent_rate / div;
	}

	return tin_parent_rate / 16;
}

#define NS_IN_HZ (1000000000UL)

int pwm_config(int pwm_id, int duty_ns, int period_ns)
{
	const struct s5p_timer *pwm =
			(struct s5p_timer *)samsung_get_base_timer();
	unsigned int offset;
	unsigned long tin_rate;
	unsigned long tin_ns;
	unsigned long period;
	unsigned long tcon;
	unsigned long tcnt;
	unsigned long tcmp;

	/*
	 * We currently avoid using 64bit arithmetic by using the
	 * fact that anything faster than 1GHz is easily representable
	 * by 32bits.
	 */
	if (period_ns > NS_IN_HZ || duty_ns > NS_IN_HZ)
		return -ERANGE;

	if (duty_ns > period_ns)
		return -EINVAL;

	period = NS_IN_HZ / period_ns;

	/* Check to see if we are changing the clock rate of the PWM */
	tin_rate = pwm_calc_tin(pwm_id, period);

	tin_ns = NS_IN_HZ / tin_rate;
	tcnt = period_ns / tin_ns;

	/* Note, counters count down */
	tcmp = duty_ns / tin_ns;
	tcmp = tcnt - tcmp;

	/*
	 * the pwm hw only checks the compare register after a decrement,
	 * so the pin never toggles if tcmp = tcnt
	 */
	if (tcmp == tcnt)
		tcmp--;

	if (tcmp < 0)
		tcmp = 0;

	/* Update the PWM register block. */
	offset = pwm_id * 3;
	if (pwm_id < 4) {
		writel(tcnt, &pwm->tcntb0 + offset);
		writel(tcmp, &pwm->tcmpb0 + offset);
	}

	tcon = readl(&pwm->tcon);
	tcon |= TCON_UPDATE(pwm_id);
	if (pwm_id < 4)
		tcon |= TCON_AUTO_RELOAD(pwm_id);
	else
		tcon |= TCON4_AUTO_RELOAD;
	writel(tcon, &pwm->tcon);

	tcon &= ~TCON_UPDATE(pwm_id);
	writel(tcon, &pwm->tcon);

	return 0;
}

int pwm_init(int pwm_id, int div, int invert)
{
	u32 val;
	const struct s5p_timer *pwm =
			(struct s5p_timer *)samsung_get_base_timer();
	unsigned long timer_rate_hz;
	unsigned int offset, prescaler;

	/*
	 * Timer Freq(HZ) =
	 *	PWM_CLK / { (prescaler_value + 1) * (divider_value) }
	 */

	val = readl(&pwm->tcfg0);
	if (pwm_id < 2) {
		prescaler = PRESCALER_0;
		val &= ~0xff;
		val |= (prescaler & 0xff);
	} else {
		prescaler = PRESCALER_1;
		val &= ~(0xff << 8);
		val |= (prescaler & 0xff) << 8;
	}
	writel(val, &pwm->tcfg0);
	val = readl(&pwm->tcfg1);
	val &= ~(0xf << MUX_DIV_SHIFT(pwm_id));
	val |= (div & 0xf) << MUX_DIV_SHIFT(pwm_id);
	writel(val, &pwm->tcfg1);

	timer_rate_hz = get_pwm_clk() / ((prescaler + 1) *
			(div + 1));

	timer_rate_hz = timer_rate_hz / 100;

	/* set count value */
	offset = pwm_id * 3;
	writel(timer_rate_hz, &pwm->tcntb0 + offset);

	val = readl(&pwm->tcon) & ~(0xf << TCON_OFFSET(pwm_id));
	if (invert && (pwm_id < 4))
		val |= TCON_INVERTER(pwm_id);
	writel(val, &pwm->tcon);

	pwm_enable(pwm_id);

	return 0;
}
