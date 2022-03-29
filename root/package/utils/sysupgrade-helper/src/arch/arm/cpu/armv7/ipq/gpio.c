/*
 * Copyright (c) 2011-2015, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <asm/arch-ipq806x/iomap.h>
#include <asm/arch-ipq806x/gpio.h>
#include <asm/io.h>

/*******************************************************
Function description: configure GPIO functinality
Arguments :
unsigned int gpio - Gpio number
unsigned int func - Functionality number
unsigned int dir  - direction 0- i/p, 1- o/p
unsigned int pull - pull up/down, no pull range(0-3)
unsigned int drvstr - range (0 - 7)-> (2- 16)MA steps of 2
unsigned int oe - 0 - Disable, 1- Enable.

Return : None
*******************************************************/


void gpio_tlmm_config(unsigned int gpio, unsigned int func,
                      unsigned int out, unsigned int pull,
                      unsigned int drvstr, unsigned int oe)
{
        unsigned int val = 0;
        val |= pull;
        val |= func << 2;
        val |= drvstr << 6;
        val |= oe << 9;
        unsigned int *addr = (unsigned int *)GPIO_CONFIG_ADDR(gpio);
        writel(val, addr);

	/* Output value is only relevant if GPIO has been configured for fixed
	 * output setting - i.e. func == 0 */
	if (func == 0) {
		addr = (unsigned int *)GPIO_IN_OUT_ADDR(gpio);
		val = readl(addr);
		val |= out << 1;
		writel(val, addr);
	}

        return;
}

int gpio_get_value(unsigned int gpio)
{
	unsigned int addr = GPIO_IN_OUT_ADDR(gpio);
	unsigned int val = readl(addr);

	return (val & (1 << GPIO_INPUT));
}

void gpio_set_value(unsigned int gpio, int what)
{
	unsigned int addr = GPIO_IN_OUT_ADDR(gpio);
	unsigned int val = readl(addr);

	val &= (~(1 << GPIO_OUTPUT));

	if (what)
		val |= (1 << GPIO_OUTPUT);

	writel(val, addr);
}

