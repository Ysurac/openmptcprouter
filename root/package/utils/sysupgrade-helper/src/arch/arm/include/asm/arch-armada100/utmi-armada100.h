/*
 * (C) Copyright 2012
 * eInfochips Ltd. <www.einfochips.com>
 * Written-by: Ajay Bhargav <ajay.bhargav@einfochips.com>
 *
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef __UTMI_ARMADA100__
#define __UTMI_ARMADA100__

#define UTMI_PHY_BASE		0xD4206000

/* utmi_ctrl - bits */
#define INPKT_DELAY_SOF		(1 << 28)
#define PLL_PWR_UP		2
#define PHY_PWR_UP		1

/* utmi_pll - bits */
#define PLL_FBDIV_MASK		0x00000FF0
#define PLL_FBDIV		4
#define PLL_REFDIV_MASK		0x0000000F
#define PLL_REFDIV		0
#define PLL_READY		0x800000
#define VCOCAL_START		(1 << 21)

#define N_DIVIDER		0xEE
#define M_DIVIDER		0x0B

/* utmi_tx - bits */
#define CK60_PHSEL		17
#define PHSEL_VAL		0x4
#define RCAL_START		(1 << 12)

/*
 * USB PHY registers
 * Refer Datasheet Appendix A.21
 */
struct armd1usb_phy_reg {
	u32 utmi_rev;	/* USB PHY Revision */
	u32 utmi_ctrl;	/* USB PHY Control register */
	u32 utmi_pll;	/* PLL register */
	u32 utmi_tx;	/* Tx register */
	u32 utmi_rx;	/* Rx register */
	u32 utmi_ivref;	/* IVREF register */
	u32 utmi_tst_g0;	/* Test group 0 register */
	u32 utmi_tst_g1;	/* Test group 1 register */
	u32 utmi_tst_g2;	/* Test group 2 register */
	u32 utmi_tst_g3;	/* Test group 3 register */
	u32 utmi_tst_g4;	/* Test group 4 register */
	u32 utmi_tst_g5;	/* Test group 5 register */
	u32 utmi_reserve;	/* Reserve Register */
	u32 utmi_usb_int;	/* USB interuppt register */
	u32 utmi_dbg_ctl;	/* Debug control register */
	u32 utmi_otg_addon;	/* OTG addon register */
};

int utmi_init(void);

#endif /* __UTMI_ARMADA100__ */
