/*
 * (C) Copyright 2010 Linaro
 * Matt Waddel, <matt.waddel@linaro.org>
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
#ifndef _SYSCTRL_H_
#define _SYSCTRL_H_

/* System controller (SP810) register definitions */
#define SP810_TIMER0_ENSEL	(1 << 15)
#define SP810_TIMER1_ENSEL	(1 << 17)
#define SP810_TIMER2_ENSEL	(1 << 19)
#define SP810_TIMER3_ENSEL	(1 << 21)

struct sysctrl {
	u32 scctrl;		/* 0x000 */
	u32 scsysstat;
	u32 scimctrl;
	u32 scimstat;
	u32 scxtalctrl;
	u32 scpllctrl;
	u32 scpllfctrl;
	u32 scperctrl0;
	u32 scperctrl1;
	u32 scperen;
	u32 scperdis;
	u32 scperclken;
	u32 scperstat;
	u32 res1[0x006];
	u32 scflashctrl;	/* 0x04c */
	u32 res2[0x3a4];
	u32 scsysid0;		/* 0xee0 */
	u32 scsysid1;
	u32 scsysid2;
	u32 scsysid3;
	u32 scitcr;
	u32 scitir0;
	u32 scitir1;
	u32 scitor;
	u32 sccntctrl;
	u32 sccntdata;
	u32 sccntstep;
	u32 res3[0x32];
	u32 scperiphid0;	/* 0xfe0 */
	u32 scperiphid1;
	u32 scperiphid2;
	u32 scperiphid3;
	u32 scpcellid0;
	u32 scpcellid1;
	u32 scpcellid2;
	u32 scpcellid3;
};
#endif /* _SYSCTRL_H_ */
