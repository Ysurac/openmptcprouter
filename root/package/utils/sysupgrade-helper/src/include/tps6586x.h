/*
 *  (C) Copyright 2010,2011
 *  NVIDIA Corporation <www.nvidia.com>
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

#ifndef __H_
#define _TPS6586X_H_

enum {
	/* SM0-2 PWM/PFM Mode Selection */
	TPS6586X_PWM_SM0	= 1 << 0,
	TPS6586X_PWM_SM1	= 1 << 1,
	TPS6586X_PWM_SM2	= 1 << 2,
};

/**
 * Enable PWM mode for selected SM0-2
 *
 * @param mask	Mask of synchronous converter to enable (TPS6586X_PWM_...)
 * @return 0 if ok, -1 on error
 */
int tps6586x_set_pwm_mode(int mask);

/**
 * Adjust SM0 and SM1 voltages to the given targets in incremental steps.
 *
 * @param sm0_target	Target voltage for SM0 in 25mW units, 0=725mV, 31=1.5V
 * @param sm1_target	Target voltage for SM1 in 25mW units, 0=725mV, 31=1.5V
 * @param step		Amount to change voltage in each step, in 25mW units
 * @param rate		Slew ratein mV/us: 0=instantly, 1=0.11, 2=0.22,
 *			3=0.44, 4=0.88, 5=1.76, 6=3.52, 7=7.04
 * @param min_sm0_over_sm1	Minimum amount by which sm0 must exceed sm1.
 *			If this condition is not met, no adjustment will be
 *			done and an error will be reported. Use -1 to skip
 *			this check.
 * @return 0 if ok, -1 on error
 */
int tps6586x_adjust_sm0_sm1(int sm0_target, int sm1_target, int step, int rate,
			    int min_sm0_over_sm1);

/**
 * Set up the TPS6586X I2C bus number. This will be used for all operations
 * on the device. This function must be called before using other functions.
 *
 * @param bus	I2C bus number containing the TPS6586X chip
 * @return 0 (always succeeds)
 */
int tps6586x_init(int bus);

#endif	/* _TPS6586X_H_ */
