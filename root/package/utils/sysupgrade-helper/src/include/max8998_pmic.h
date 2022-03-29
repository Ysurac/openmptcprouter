/*
 *  Copyright (C) 2011 Samsung Electronics
 *  Lukasz Majewski <l.majewski@samsung.com>
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

#ifndef __MAX8998_PMIC_H_
#define __MAX8998_PMIC_H_

/* MAX 8998 registers */
enum {
	MAX8998_REG_IRQ1,
	MAX8998_REG_IRQ2,
	MAX8998_REG_IRQ3,
	MAX8998_REG_IRQ4,
	MAX8998_REG_IRQM1,
	MAX8998_REG_IRQM2,
	MAX8998_REG_IRQM3,
	MAX8998_REG_IRQM4,
	MAX8998_REG_STATUS1,
	MAX8998_REG_STATUS2,
	MAX8998_REG_STATUSM1,
	MAX8998_REG_STATUSM2,
	MAX8998_REG_CHGR1,
	MAX8998_REG_CHGR2,
	MAX8998_REG_LDO_ACTIVE_DISCHARGE1,
	MAX8998_REG_LDO_ACTIVE_DISCHARGE2,
	MAX8998_REG_BUCK_ACTIVE_DISCHARGE3,
	MAX8998_REG_ONOFF1,
	MAX8998_REG_ONOFF2,
	MAX8998_REG_ONOFF3,
	MAX8998_REG_ONOFF4,
	MAX8998_REG_BUCK1_VOLTAGE1,
	MAX8998_REG_BUCK1_VOLTAGE2,
	MAX8998_REG_BUCK1_VOLTAGE3,
	MAX8998_REG_BUCK1_VOLTAGE4,
	MAX8998_REG_BUCK2_VOLTAGE1,
	MAX8998_REG_BUCK2_VOLTAGE2,
	MAX8998_REG_BUCK3,
	MAX8998_REG_BUCK4,
	MAX8998_REG_LDO2_LDO3,
	MAX8998_REG_LDO4,
	MAX8998_REG_LDO5,
	MAX8998_REG_LDO6,
	MAX8998_REG_LDO7,
	MAX8998_REG_LDO8_LDO9,
	MAX8998_REG_LDO10_LDO11,
	MAX8998_REG_LDO12,
	MAX8998_REG_LDO13,
	MAX8998_REG_LDO14,
	MAX8998_REG_LDO15,
	MAX8998_REG_LDO16,
	MAX8998_REG_LDO17,
	MAX8998_REG_BKCHR,
	MAX8998_REG_LBCNFG1,
	MAX8998_REG_LBCNFG2,
	PMIC_NUM_OF_REGS,
};

#define MAX8998_LDO3		(1 << 2)
#define MAX8998_LDO4		(1 << 1)
#define MAX8998_LDO8		(1 << 5)
#define MAX8998_SAFEOUT1	(1 << 4)

#define MAX8998_I2C_ADDR        (0xCC >> 1)

enum { LDO_OFF, LDO_ON };

#endif /* __MAX8998_PMIC_H_ */
