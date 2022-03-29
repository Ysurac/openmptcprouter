/*
 * Freescale i.MX28 RTC Register Definitions
 *
 * Copyright (C) 2011 Marek Vasut <marek.vasut@gmail.com>
 * on behalf of DENX Software Engineering GmbH
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#ifndef __MX28_REGS_RTC_H__
#define __MX28_REGS_RTC_H__

#include <asm/arch/regs-common.h>

#ifndef	__ASSEMBLY__
struct mx28_rtc_regs {
	mx28_reg_32(hw_rtc_ctrl)
	mx28_reg_32(hw_rtc_stat)
	mx28_reg_32(hw_rtc_milliseconds)
	mx28_reg_32(hw_rtc_seconds)
	mx28_reg_32(hw_rtc_rtc_alarm)
	mx28_reg_32(hw_rtc_watchdog)
	mx28_reg_32(hw_rtc_persistent0)
	mx28_reg_32(hw_rtc_persistent1)
	mx28_reg_32(hw_rtc_persistent2)
	mx28_reg_32(hw_rtc_persistent3)
	mx28_reg_32(hw_rtc_persistent4)
	mx28_reg_32(hw_rtc_persistent5)
	mx28_reg_32(hw_rtc_debug)
	mx28_reg_32(hw_rtc_version)
};
#endif

#define	RTC_CTRL_SFTRST				(1 << 31)
#define	RTC_CTRL_CLKGATE			(1 << 30)
#define	RTC_CTRL_SUPPRESS_COPY2ANALOG		(1 << 6)
#define	RTC_CTRL_FORCE_UPDATE			(1 << 5)
#define	RTC_CTRL_WATCHDOGEN			(1 << 4)
#define	RTC_CTRL_ONEMSEC_IRQ			(1 << 3)
#define	RTC_CTRL_ALARM_IRQ			(1 << 2)
#define	RTC_CTRL_ONEMSEC_IRQ_EN			(1 << 1)
#define	RTC_CTRL_ALARM_IRQ_EN			(1 << 0)

#define	RTC_STAT_RTC_PRESENT			(1 << 31)
#define	RTC_STAT_ALARM_PRESENT			(1 << 30)
#define	RTC_STAT_WATCHDOG_PRESENT		(1 << 29)
#define	RTC_STAT_XTAL32000_PRESENT		(1 << 28)
#define	RTC_STAT_XTAL32768_PRESENT		(1 << 27)
#define	RTC_STAT_STALE_REGS_MASK		(0xff << 16)
#define	RTC_STAT_STALE_REGS_OFFSET		16
#define	RTC_STAT_NEW_REGS_MASK			(0xff << 8)
#define	RTC_STAT_NEW_REGS_OFFSET		8

#define	RTC_MILLISECONDS_COUNT_MASK		0xffffffff
#define	RTC_MILLISECONDS_COUNT_OFFSET		0

#define	RTC_SECONDS_COUNT_MASK			0xffffffff
#define	RTC_SECONDS_COUNT_OFFSET		0

#define	RTC_ALARM_VALUE_MASK			0xffffffff
#define	RTC_ALARM_VALUE_OFFSET			0

#define	RTC_WATCHDOG_COUNT_MASK			0xffffffff
#define	RTC_WATCHDOG_COUNT_OFFSET		0

#define	RTC_PERSISTENT0_ADJ_POSLIMITBUCK_MASK	(0xf << 28)
#define	RTC_PERSISTENT0_ADJ_POSLIMITBUCK_OFFSET	28
#define	RTC_PERSISTENT0_ADJ_POSLIMITBUCK_2V83	(0x0 << 28)
#define	RTC_PERSISTENT0_ADJ_POSLIMITBUCK_2V78	(0x1 << 28)
#define	RTC_PERSISTENT0_ADJ_POSLIMITBUCK_2V73	(0x2 << 28)
#define	RTC_PERSISTENT0_ADJ_POSLIMITBUCK_2V68	(0x3 << 28)
#define	RTC_PERSISTENT0_ADJ_POSLIMITBUCK_2V62	(0x4 << 28)
#define	RTC_PERSISTENT0_ADJ_POSLIMITBUCK_2V57	(0x5 << 28)
#define	RTC_PERSISTENT0_ADJ_POSLIMITBUCK_2V52	(0x6 << 28)
#define	RTC_PERSISTENT0_ADJ_POSLIMITBUCK_2V48	(0x7 << 28)
#define	RTC_PERSISTENT0_EXTERNAL_RESET		(1 << 21)
#define	RTC_PERSISTENT0_THERMAL_RESET		(1 << 20)
#define	RTC_PERSISTENT0_ENABLE_LRADC_PWRUP	(1 << 18)
#define	RTC_PERSISTENT0_AUTO_RESTART		(1 << 17)
#define	RTC_PERSISTENT0_DISABLE_PSWITCH		(1 << 16)
#define	RTC_PERSISTENT0_LOWERBIAS_MASK		(0xf << 14)
#define	RTC_PERSISTENT0_LOWERBIAS_OFFSET	14
#define	RTC_PERSISTENT0_LOWERBIAS_NOMINAL	(0x0 << 14)
#define	RTC_PERSISTENT0_LOWERBIAS_M25P		(0x1 << 14)
#define	RTC_PERSISTENT0_LOWERBIAS_M50P		(0x3 << 14)
#define	RTC_PERSISTENT0_DISABLE_XTALOK		(1 << 13)
#define	RTC_PERSISTENT0_MSEC_RES_MASK		(0x1f << 8)
#define	RTC_PERSISTENT0_MSEC_RES_OFFSET		8
#define	RTC_PERSISTENT0_MSEC_RES_1MS		(0x01 << 8)
#define	RTC_PERSISTENT0_MSEC_RES_2MS		(0x02 << 8)
#define	RTC_PERSISTENT0_MSEC_RES_4MS		(0x04 << 8)
#define	RTC_PERSISTENT0_MSEC_RES_8MS		(0x08 << 8)
#define	RTC_PERSISTENT0_MSEC_RES_16MS		(0x10 << 8)
#define	RTC_PERSISTENT0_ALARM_WAKE		(1 << 7)
#define	RTC_PERSISTENT0_XTAL32_FREQ		(1 << 6)
#define	RTC_PERSISTENT0_XTAL32KHZ_PWRUP		(1 << 5)
#define	RTC_PERSISTENT0_XTAL24KHZ_PWRUP		(1 << 4)
#define	RTC_PERSISTENT0_LCK_SECS		(1 << 3)
#define	RTC_PERSISTENT0_ALARM_EN		(1 << 2)
#define	RTC_PERSISTENT0_ALARM_WAKE_EN		(1 << 1)
#define	RTC_PERSISTENT0_CLOCKSOURCE		(1 << 0)

#define	RTC_PERSISTENT1_GENERAL_MASK		0xffffffff
#define	RTC_PERSISTENT1_GENERAL_OFFSET		0
#define	RTC_PERSISTENT1_GENERAL_OTG_ALT_ROLE	0x0080
#define	RTC_PERSISTENT1_GENERAL_OTG_HNP		0x0100
#define	RTC_PERSISTENT1_GENERAL_USB_LPM		0x0200
#define	RTC_PERSISTENT1_GENERAL_SKIP_CHECKDISK	0x0400
#define	RTC_PERSISTENT1_GENERAL_USB_BOOT_PLAYER	0x0800
#define	RTC_PERSISTENT1_GENERAL_ENUM_500MA_2X	0x1000

#define	RTC_PERSISTENT2_GENERAL_MASK		0xffffffff
#define	RTC_PERSISTENT2_GENERAL_OFFSET		0

#define	RTC_PERSISTENT3_GENERAL_MASK		0xffffffff
#define	RTC_PERSISTENT3_GENERAL_OFFSET		0

#define	RTC_PERSISTENT4_GENERAL_MASK		0xffffffff
#define	RTC_PERSISTENT4_GENERAL_OFFSET		0

#define	RTC_PERSISTENT5_GENERAL_MASK		0xffffffff
#define	RTC_PERSISTENT5_GENERAL_OFFSET		0

#define	RTC_DEBUG_WATCHDOG_RESET_MASK		(1 << 1)
#define	RTC_DEBUG_WATCHDOG_RESET		(1 << 0)

#define	RTC_VERSION_MAJOR_MASK			(0xff << 24)
#define	RTC_VERSION_MAJOR_OFFSET		24
#define	RTC_VERSION_MINOR_MASK			(0xff << 16)
#define	RTC_VERSION_MINOR_OFFSET		16
#define	RTC_VERSION_STEP_MASK			0xffff
#define	RTC_VERSION_STEP_OFFSET			0

#endif	/* __MX28_REGS_RTC_H__ */
