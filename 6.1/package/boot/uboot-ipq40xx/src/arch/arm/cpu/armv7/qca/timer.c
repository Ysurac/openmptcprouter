/*
 *
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <asm/arch-ipq40xx/iomap.h>
#include <asm/io.h>
#include <common.h>
#include <asm/types.h>
#include <asm/arch-ipq40xx/timer.h>

static unsigned long long timestamp;
static unsigned long long lastinc;

#define GPT_FREQ	48
#define GPT_FREQ_KHZ	(GPT_FREQ * 1000)
#define GPT_FREQ_HZ	(GPT_FREQ_KHZ * 1000)	/* 48 MHz */

/**
 * timer_init - initialize timer
 */
int timer_init(void)
{
	writel(1, GCNT_BASE);
	return 0;
}

/**
 * get_timer - returns time lapsed
 * @base: base/start time
 *
 * Returns time lapsed, since the specified base time value.
 */
ulong get_timer(ulong base)
{
	return get_timer_masked() - base;
}

/*
 * read_counter - returns 64-bit counter value
 * Reads Timer HI and Timer LO value register
 */
static unsigned long long read_counter(void)
{
	unsigned long vect_hi1, vect_hi2;
	unsigned long vect_low;

repeat:
	vect_hi1 = readl(GCNT_CNTCV_HI);
	vect_low = readl(GCNT_CNTCV_LO);
	vect_hi2 = readl(GCNT_CNTCV_HI);

	if (vect_hi1 != vect_hi2)
		goto repeat;

	return ((unsigned long long)vect_hi1 << 32 | vect_low);
}
/**
 * __udelay -  generates micro second delay.
 * @usec: delay duration in microseconds
 *
 * With 32KHz clock, minimum possible delay is 31.25 Micro seconds and
 * its multiples. In Rumi GPT clock is 32 KHz
 */
void __udelay(unsigned long usec)
{
	unsigned long long val;
	unsigned long long now;
	unsigned long long last;
	unsigned long long runcount;

	val = (usec * GPT_FREQ);
	last = read_counter();
	do {
		now = read_counter();
		if (last > now)
			runcount = (GPT_FREQ - last) + now;
		else
			runcount = now - last;
	} while (runcount < val);
}

inline unsigned long long gpt_to_sys_freq(unsigned long long gpt)
{
	/*
	 * get_timer() expects the timer increments to be in terms
	 * of CONFIG_SYS_HZ. Convert GPT timer values to CONFIG_SYS_HZ
	 * units.
	 */
	return (gpt) / (GPT_FREQ_HZ / CONFIG_SYS_HZ);
}

/**
 * get_timer_masked - returns current ticks
 *
 * Returns the current timer ticks, since boot.
 */
ulong get_timer_masked(void)
{
	unsigned long long now;
	unsigned long long counter_val;

	counter_val = read_counter();
	now = gpt_to_sys_freq(counter_val);

	if (lastinc <= now) {	/* normal mode (non roll) */
		/* normal mode */
		timestamp += now - lastinc;
		/* move stamp forward with absolute diff ticks */
	} else {
		/* we have overflow of the count down timer */
		timestamp += now + (TIMER_LOAD_VAL - lastinc);
	}
	lastinc = now;

	return (ulong)timestamp;
}
