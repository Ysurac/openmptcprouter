/*
 * da8xx.h -- TI's DA8xx platform specific usb wrapper definitions.
 *
 * Author: Ajay Kumar Gupta <ajay.gupta@ti.com>
 *
 * Based on drivers/usb/musb/davinci.h
 *
 * Copyright (C) 2009 Texas Instruments Incorporated
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef __DA8XX_MUSB_H__
#define __DA8XX_MUSB_H__

#include <asm/arch/hardware.h>
#include <asm/arch/gpio.h>
#include "musb_core.h"

/* Base address of da8xx usb0 wrapper */
#define DA8XX_USB_OTG_BASE  0x01E00000

/* Base address of da8xx musb core */
#define DA8XX_USB_OTG_CORE_BASE (DA8XX_USB_OTG_BASE + 0x400)

/* Timeout for DA8xx usb module */
#define DA8XX_USB_OTG_TIMEOUT 0x3FFFFFF

/*
 * DA8xx platform USB wrapper register overlay.
 */
struct da8xx_usb_regs {
	dv_reg	revision;
	dv_reg	control;
	dv_reg 	status;
	dv_reg 	emulation;
	dv_reg 	mode;
	dv_reg 	autoreq;
	dv_reg 	srpfixtime;
	dv_reg 	teardown;
	dv_reg 	intsrc;
	dv_reg 	intsrc_set;
	dv_reg 	intsrc_clr;
	dv_reg 	intmsk;
	dv_reg 	intmsk_set;
	dv_reg 	intmsk_clr;
	dv_reg 	intsrcmsk;
	dv_reg 	eoi;
	dv_reg 	intvector;
	dv_reg 	grndis_size[4];
};

#define da8xx_usb_regs ((struct da8xx_usb_regs *)DA8XX_USB_OTG_BASE)

/* DA8XX interrupt bits definitions */
#define DA8XX_USB_TX_ENDPTS_MASK  0x1f	/* ep0 + 4 tx */
#define DA8XX_USB_RX_ENDPTS_MASK  0x1e	/* 4 rx */
#define DA8XX_USB_TXINT_SHIFT	  0
#define DA8XX_USB_RXINT_SHIFT	  8

#define DA8XX_USB_USBINT_MASK	  0x01ff0000	/* 8 Mentor, DRVVBUS */
#define DA8XX_USB_TXINT_MASK \
		(DA8XX_USB_TX_ENDPTS_MASK << DA8XX_USB_TXINT_SHIFT)
#define DA8XX_USB_RXINT_MASK \
		(DA8XX_USB_RX_ENDPTS_MASK << DA8XX_USB_RXINT_SHIFT)

/* DA8xx CFGCHIP2 (USB 2.0 PHY Control) register bits */
#define CFGCHIP2_PHYCLKGD	(1 << 17)
#define CFGCHIP2_VBUSSENSE	(1 << 16)
#define CFGCHIP2_RESET		(1 << 15)
#define CFGCHIP2_OTGMODE	(3 << 13)
#define CFGCHIP2_NO_OVERRIDE	(0 << 13)
#define CFGCHIP2_FORCE_HOST	(1 << 13)
#define CFGCHIP2_FORCE_DEVICE 	(2 << 13)
#define CFGCHIP2_FORCE_HOST_VBUS_LOW (3 << 13)
#define CFGCHIP2_USB1PHYCLKMUX	(1 << 12)
#define CFGCHIP2_USB2PHYCLKMUX	(1 << 11)
#define CFGCHIP2_PHYPWRDN	(1 << 10)
#define CFGCHIP2_OTGPWRDN	(1 << 9)
#define CFGCHIP2_DATPOL 	(1 << 8)
#define CFGCHIP2_USB1SUSPENDM	(1 << 7)
#define CFGCHIP2_PHY_PLLON	(1 << 6)	/* override PLL suspend */
#define CFGCHIP2_SESENDEN	(1 << 5)	/* Vsess_end comparator */
#define CFGCHIP2_VBDTCTEN	(1 << 4)	/* Vbus comparator */
#define CFGCHIP2_REFFREQ	(0xf << 0)
#define CFGCHIP2_REFFREQ_12MHZ	(1 << 0)
#define CFGCHIP2_REFFREQ_24MHZ	(2 << 0)
#define CFGCHIP2_REFFREQ_48MHZ	(3 << 0)

#define DA8XX_USB_VBUS_GPIO	(1 << 15)
#endif	/* __DA8XX_MUSB_H__ */
