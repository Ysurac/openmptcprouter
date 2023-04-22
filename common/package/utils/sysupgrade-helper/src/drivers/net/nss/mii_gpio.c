/*
 *
 * mii_gpio.c
 *	MII phy control operations.
 *
 * Copyright (c) 2012 - 2013 The Linux Foundation. All rights reserved.*
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
#include <asm/arch-ipq806x/gpio.h>
#include <asm/arch-ipq806x/iomap.h>
#include <asm/io.h>

#define MII_PREAMBLE_LENGTH	32
#define MII_OPCODE_LENGTH	4
#define MII_PHY_ADDR_LENGTH	5
#define MII_REG_ADDR_LENGTH	5
#define MII_HEADER_LENGTH	(MII_OPCODE_LENGTH + MII_PHY_ADDR_LENGTH + MII_REG_ADDR_LENGTH)
/*
 * REG_DATA_LENGTH includes 2 bit TA
 */
#define SYSTEM_FREQ 		100
#define MII_REG_DATA_LENGTH	18
#define MII_OPCODE_WRITE	0x05
#define MII_OPCODE_READ		0x06
#define MII_DUTY_CYCLE		100
#define MII_DUTY_CYCLE_TIME	((((SYSTEM_FREQ / 1000000) * MII_DUTY_CYCLE / 1000) + 1) * 2)
#define MII_DATA_SETUP_TIME	((SYSTEM_FREQ / 100000000) + 1)

/*
 * GPIO Port and Pin numbers for driving MDC and MDIO
 */
#define GPIO_0			(1<<0)
#define GPIO_1			(1<<1)
#define MII_MDIO_PIN		GPIO_0
#define MII_MDC_PIN		GPIO_1
#define SET_HIGH		1
#define SET_LOW			0
#define GPIO_GET		1
#define GPIO_SET		0
#define GPIO_OUT		1
#define GPIO_IN			0

/*
 * Routines to drive the MDC/MDIO GPIO pins
 */
static inline uint32_t gmac_handle_gpio(uint32_t get, uint32_t pin, uint32_t hi_lo)
{
	uint32_t addr = GPIO_IN_OUT_ADDR(pin);
        uint32_t val = readl(addr);

	if (get)
		return val & (1<< GPIO_IN);

        val &= (~(1 << GPIO_OUT));
        if (hi_lo)
                val |= (1 << GPIO_OUT);
        writel(val, addr);

	return 0;
}


static inline void mii_set_pin_high(uint32_t pin)
{
	gmac_handle_gpio(GPIO_SET,pin, SET_HIGH);
}

static inline void mii_set_pin_low(uint32_t pin)
{
	gmac_handle_gpio(GPIO_SET,pin, SET_LOW);
}

static inline void mii_set_pin_dir_out(uint32_t pin)
{
	gpio_tlmm_config(pin, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_12MA, GPIO_ENABLE);
}

static inline void mii_set_pin_dir_in(uint32_t pin)
{
	 gpio_tlmm_config(pin, 1, GPIO_INPUT, GPIO_NO_PULL, GPIO_12MA, GPIO_ENABLE);
}

static inline void mii_set_pin(uint32_t pin, uint32_t value)
{
	gmac_handle_gpio(GPIO_SET, pin, value);

}

static inline uint32_t mii_get_pin(uint32_t pin)
{
	return gmac_handle_gpio(GPIO_GET, pin, 0);
}

/* will be removed after bring up */
/*
#define bit_reverse(val1, val2, shift) \
{ \
        asm volatile ( \
        "       bfrvrs          %[out], %[in], #"D(shift)"      \n\t" \
                :[out] "=&d" (val1) \
                :[in] "r" (val2) \
        ); \
}
*/

static void bit_reverse(unsigned int val1, unsigned int *val2, unsigned int shift)
{
	unsigned int j, i;

	for (i = 0, j = 31; i < 32 ; i++, j--) {
	        if ((val1 >> i) & 1)
	        	*val2 |= 1<<j;
		else
			*val2 |= 0<<j;
	}
	*val2 >>= shift;
}

/*
 * mii_read_reg()
 *	Read a register from an external PHY/Switch
 */


uint16_t ipq_mii_read_reg(uint32_t phy, uint32_t reg)
{
	uint32_t sz = MII_PREAMBLE_LENGTH;
	uint32_t val1 = reg | (phy << MII_PHY_ADDR_LENGTH) |
			(MII_OPCODE_READ << (MII_PHY_ADDR_LENGTH + MII_REG_ADDR_LENGTH));
	uint32_t val = 0;

	bit_reverse(val1, &val, 32 - MII_HEADER_LENGTH);

	mii_set_pin_high(MII_MDIO_PIN);
	mii_set_pin_high(MII_MDC_PIN);

	mii_set_pin_dir_out(MII_MDIO_PIN);
	mii_set_pin_dir_out(MII_MDC_PIN);

	/*
	 * Send preamble
	 */
	do {
		mii_set_pin_low(MII_MDC_PIN);
		__udelay(MII_DUTY_CYCLE_TIME);
		mii_set_pin_high(MII_MDC_PIN);
		__udelay(MII_DUTY_CYCLE_TIME);
	} while (--sz);

	/*
	 * send read command
	 */
	sz = MII_HEADER_LENGTH;

	do {
		mii_set_pin_low(MII_MDC_PIN);
		mii_set_pin(MII_MDIO_PIN, val & 0x1);
		__udelay(MII_DUTY_CYCLE_TIME);
		mii_set_pin_high(MII_MDC_PIN);
		__udelay(MII_DUTY_CYCLE_TIME);
		val >>= 1;
	} while (--sz);

	/*
	 * read register contents
	 */
	sz = MII_REG_DATA_LENGTH;
	val = 0;
	mii_set_pin_dir_in(MII_MDIO_PIN);

	do {
		mii_set_pin_low(MII_MDC_PIN);
		val <<= 1;
		__udelay(MII_DUTY_CYCLE_TIME);
		val |= mii_get_pin(MII_MDIO_PIN);
		mii_set_pin_high(MII_MDC_PIN);
		__udelay(MII_DUTY_CYCLE_TIME);

	} while (--sz);

	return val;
}

/*
 * mii_write_reg()
 *	Write a register of an external PHY/Switch
 */
void ipq_mii_write_reg(uint32_t phy, uint32_t reg, uint16_t data)
{
	uint32_t sz = MII_PREAMBLE_LENGTH;
	uint32_t val1 = reg | (phy << MII_PHY_ADDR_LENGTH) |
			(MII_OPCODE_WRITE << (MII_PHY_ADDR_LENGTH + MII_REG_ADDR_LENGTH));
	uint32_t val = 0;
	val1 = (val << MII_REG_DATA_LENGTH) | (1 << (MII_REG_DATA_LENGTH - 1)) | data;


	bit_reverse(val1, &val, 32 - MII_HEADER_LENGTH - MII_REG_DATA_LENGTH);

	mii_set_pin_high(MII_MDIO_PIN);
	mii_set_pin_high(MII_MDC_PIN);

	mii_set_pin_dir_out(MII_MDIO_PIN);
	mii_set_pin_dir_out(MII_MDC_PIN);

	/*
	 * Send preamble
	 */
	do {
		mii_set_pin_low(MII_MDC_PIN);
		__udelay(MII_DUTY_CYCLE_TIME);
		mii_set_pin_high(MII_MDC_PIN);
		__udelay(MII_DUTY_CYCLE_TIME);
	} while (--sz);

	/*
	 * send command and data
	 */
	sz = (MII_HEADER_LENGTH + MII_REG_DATA_LENGTH);

	do {
		mii_set_pin_low(MII_MDC_PIN);
		mii_set_pin(MII_MDIO_PIN, val & 0x1);
		__udelay(MII_DUTY_CYCLE_TIME);
		mii_set_pin_high(MII_MDC_PIN);
		__udelay(MII_DUTY_CYCLE_TIME);
		val >>= 1;
	} while (--sz);
}

