/*
 * Copyright (C) 2009 Texas Instruments Incorporated
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
#ifndef _GPIO_DEFS_H_
#define _GPIO_DEFS_H_

#ifndef CONFIG_SOC_DA8XX
#define DAVINCI_GPIO_BINTEN	0x01C67008
#define DAVINCI_GPIO_BANK01	0x01C67010
#define DAVINCI_GPIO_BANK23	0x01C67038
#define DAVINCI_GPIO_BANK45	0x01C67060
#define DAVINCI_GPIO_BANK67	0x01C67088

#else /* CONFIG_SOC_DA8XX */
#define DAVINCI_GPIO_BINTEN	0x01E26008
#define DAVINCI_GPIO_BANK01	0x01E26010
#define DAVINCI_GPIO_BANK23	0x01E26038
#define DAVINCI_GPIO_BANK45	0x01E26060
#define DAVINCI_GPIO_BANK67	0x01E26088
#define DAVINCI_GPIO_BANK8	0x01E260B0
#endif /* CONFIG_SOC_DA8XX */

struct davinci_gpio {
	unsigned int dir;
	unsigned int out_data;
	unsigned int set_data;
	unsigned int clr_data;
	unsigned int in_data;
	unsigned int set_rising;
	unsigned int clr_rising;
	unsigned int set_falling;
	unsigned int clr_falling;
	unsigned int intstat;
};

struct davinci_gpio_bank {
	int num_gpio;
	unsigned int irq_num;
	unsigned int irq_mask;
	unsigned long *in_use;
	unsigned long base;
};

#define davinci_gpio_bank01 ((struct davinci_gpio *)DAVINCI_GPIO_BANK01)
#define davinci_gpio_bank23 ((struct davinci_gpio *)DAVINCI_GPIO_BANK23)
#define davinci_gpio_bank45 ((struct davinci_gpio *)DAVINCI_GPIO_BANK45)
#define davinci_gpio_bank67 ((struct davinci_gpio *)DAVINCI_GPIO_BANK67)
#define davinci_gpio_bank8 ((struct davinci_gpio *)DAVINCI_GPIO_BANK8)

#define gpio_status()		gpio_info()
#define GPIO_NAME_SIZE		20
#define MAX_NUM_GPIOS		144
#define GPIO_BANK(gp)		(davinci_gpio_bank01 + ((gp) >> 5))
#define GPIO_BIT(gp)		((gp) & 0x1F)

void gpio_info(void);

#endif
