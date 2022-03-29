/*
 * Copyright (c) 2011 The Chromium OS Authors.
 * (C) Copyright 2010,2011 NVIDIA Corporation <www.nvidia.com>
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

#include <common.h>
#include <tps6586x.h>
#include <asm/io.h>
#include <i2c.h>

static int bus_num;		/* I2C bus we are on */
#define I2C_ADDRESS		0x34	/* chip requires this address */
static char inited;		/* 1 if we have been inited */

enum {
	/* Registers that we access */
	SUPPLY_CONTROL1		= 0x20,
	SUPPLY_CONTROL2,
	SM1_VOLTAGE_V1		= 0x23,
	SM1_VOLTAGE_V2,
	SM0_VOLTAGE_V1		= 0x26,
	SM0_VOLTAGE_V2,
	PFM_MODE		= 0x47,

	/* Bits in the supply control registers */
	CTRL_SM1_RAMP		= 0x01,
	CTRL_SM1_SUPPLY2	= 0x02,
	CTRL_SM0_RAMP		= 0x04,
	CTRL_SM0_SUPPLY2	= 0x08,
};

#define MAX_I2C_RETRY	3
int tps6586x_read(int reg)
{
	int	i;
	uchar	data;
	int	retval = -1;
	int	old_bus_num;

	old_bus_num = i2c_get_bus_num();
	i2c_set_bus_num(bus_num);

	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (!i2c_read(I2C_ADDRESS, reg, 1, &data, 1)) {
			retval = (int)data;
			goto exit;
		}

		/* i2c access failed, retry */
		udelay(100);
	}

exit:
	i2c_set_bus_num(old_bus_num);
	debug("pmu_read %x=%x\n", reg, retval);
	if (retval < 0)
		debug("%s: failed to read register %#x: %d\n", __func__, reg,
		      retval);
	return retval;
}

int tps6586x_write(int reg, uchar *data, uint len)
{
	int	i;
	int	retval = -1;
	int	old_bus_num;

	old_bus_num = i2c_get_bus_num();
	i2c_set_bus_num(bus_num);

	for (i = 0; i < MAX_I2C_RETRY; ++i) {
		if (!i2c_write(I2C_ADDRESS, reg, 1, data, len)) {
			retval = 0;
			goto exit;
		}

		/* i2c access failed, retry */
		udelay(100);
	}

exit:
	i2c_set_bus_num(old_bus_num);
	debug("pmu_write %x=%x: ", reg, retval);
	for (i = 0; i < len; i++)
		debug("%x ", data[i]);
	if (retval)
		debug("%s: failed to write register %#x\n", __func__, reg);
	return retval;
}

/*
 * Get current voltage of SM0 and SM1
 *
 * @param sm0	Place to put SM0 voltage
 * @param sm1	Place to put SM1 voltage
 * @return 0 if ok, -1 on error
 */
static int read_voltages(int *sm0, int *sm1)
{
	int ctrl1, ctrl2;
	int is_v2;

	/*
	 * Each vdd has two supply sources, ie, v1 and v2.
	 * The supply control reg1 and reg2 determine the current selection.
	 */
	ctrl1 = tps6586x_read(SUPPLY_CONTROL1);
	ctrl2 = tps6586x_read(SUPPLY_CONTROL2);
	if (ctrl1 == -1 || ctrl2 == -1)
		return -1;

	/* Figure out whether V1 or V2 is selected */
	is_v2 = (ctrl1 | ctrl2) & CTRL_SM0_SUPPLY2;
	*sm0 = tps6586x_read(is_v2 ? SM0_VOLTAGE_V2 : SM0_VOLTAGE_V1);
	*sm1 = tps6586x_read(is_v2 ? SM1_VOLTAGE_V2 : SM1_VOLTAGE_V1);
	if (*sm0 == -1 || *sm1 == -1)
		return -1;

	return 0;
}

static int set_voltage(int reg, int data, int rate)
{
	uchar control_bit;
	uchar buff[3];

	control_bit = (reg == SM0_VOLTAGE_V1 ? CTRL_SM0_RAMP : CTRL_SM1_RAMP);

	/*
	 * Only one supply is needed in u-boot. set both v1 and v2 to
	 * same value.
	 *
	 * When both v1 and v2 are set to same value, we just need to set
	 * control1 reg to trigger the supply selection.
	 */
	buff[0] = buff[1] = (uchar)data;
	buff[2] = rate;

	/* write v1, v2 and rate, then trigger */
	if (tps6586x_write(reg, buff, 3) ||
	    tps6586x_write(SUPPLY_CONTROL1, &control_bit, 1))
		return -1;

	return 0;
}

static int calculate_next_voltage(int voltage, int target, int step)
{
	int diff = voltage < target ? step : -step;

	if (abs(target - voltage) > step)
		voltage += diff;
	else
		voltage = target;

	return voltage;
}

int tps6586x_set_pwm_mode(int mask)
{
	uchar val;
	int ret;

	assert(inited);
	ret = tps6586x_read(PFM_MODE);
	if (ret != -1) {
		val = (uchar)ret;
		val |= mask;

		ret = tps6586x_write(PFM_MODE, &val, 1);
	}

	if (ret == -1)
		debug("%s: Failed to read/write PWM mode reg\n", __func__);

	return ret;
}

int tps6586x_adjust_sm0_sm1(int sm0_target, int sm1_target, int step, int rate,
			    int min_sm0_over_sm1)
{
	int sm0, sm1;
	int bad;

	assert(inited);

	/* get current voltage settings */
	if (read_voltages(&sm0, &sm1)) {
		debug("%s: Cannot read voltage settings\n", __func__);
		return -1;
	}

	/*
	 * if vdd_core < vdd_cpu + rel
	 *    skip
	 *
	 * This condition may happen when system reboots due to kernel crash.
	 */
	if (min_sm0_over_sm1 != -1 && sm0 < sm1 + min_sm0_over_sm1) {
		debug("%s: SM0 is %d, SM1 is %d, but min_sm0_over_sm1 is %d\n",
		      __func__, sm0, sm1, min_sm0_over_sm1);
		return -1;
	}

	/*
	 * Since vdd_core and vdd_cpu may both stand at either greater or less
	 * than their nominal voltage, the adjustment may go either directions.
	 *
	 * Make sure vdd_core is always higher than vdd_cpu with certain margin.
	 * So, find out which vdd to adjust first in each step.
	 *
	 * case 1: both sm0 and sm1 need to move up
	 *              adjust sm0 before sm1
	 *
	 * case 2: both sm0 and sm1 need to move down
	 *              adjust sm1 before sm0
	 *
	 * case 3: sm0 moves down and sm1 moves up
	 *              adjusting either one first is fine.
	 *
	 * Adjust vdd_core and vdd_cpu one step at a time until they reach
	 * their nominal values.
	 */
	bad = 0;
	while (!bad && (sm0 != sm0_target || sm1 != sm1_target)) {
		int adjust_sm0_late = 0; /* flag to adjust vdd_core later */

		debug("%d-%d   %d-%d   ", sm0, sm0_target, sm1, sm1_target);

		if (sm0 != sm0_target) {
			/*
			 * if case 1 and case 3, set new sm0 first.
			 * otherwise, hold down until new sm1 is set.
			 */
			sm0 = calculate_next_voltage(sm0, sm0_target, step);
			if (sm1 < sm1_target)
				bad |= set_voltage(SM0_VOLTAGE_V1, sm0, rate);
			else
				adjust_sm0_late = 1;
		}

		if (sm1 != sm1_target) {
			sm1 = calculate_next_voltage(sm1, sm1_target, step);
			bad |= set_voltage(SM1_VOLTAGE_V1, sm1, rate);
		}

		if (adjust_sm0_late)
			bad |= set_voltage(SM0_VOLTAGE_V1, sm0, rate);
		debug("%d\n", adjust_sm0_late);
	}
	debug("%d-%d   %d-%d   done\n", sm0, sm0_target, sm1, sm1_target);

	return bad ? -1 : 0;
}

int tps6586x_init(int bus)
{
	bus_num = bus;
	inited = 1;

	return 0;
}
