/*
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
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

#include <common.h>
#include <asm/io.h>
#include <usb.h>
#include "ehci.h"
#include "ehci-core.h"
#include <asm/arch/cpu.h>

#if defined(CONFIG_KIRKWOOD)
#include <asm/arch/kirkwood.h>
#elif defined(CONFIG_ORION5X)
#include <asm/arch/orion5x.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

#define rdl(off)	readl(MVUSB0_BASE + (off))
#define wrl(off, val)	writel((val), MVUSB0_BASE + (off))

#define USB_WINDOW_CTRL(i)	(0x320 + ((i) << 4))
#define USB_WINDOW_BASE(i)	(0x324 + ((i) << 4))
#define USB_TARGET_DRAM		0x0

/*
 * USB 2.0 Bridge Address Decoding registers setup
 */
static void usb_brg_adrdec_setup(void)
{
	int i;
	u32 size, base, attrib;

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {

		/* Enable DRAM bank */
		switch (i) {
		case 0:
			attrib = MVUSB0_CPU_ATTR_DRAM_CS0;
			break;
		case 1:
			attrib = MVUSB0_CPU_ATTR_DRAM_CS1;
			break;
		case 2:
			attrib = MVUSB0_CPU_ATTR_DRAM_CS2;
			break;
		case 3:
			attrib = MVUSB0_CPU_ATTR_DRAM_CS3;
			break;
		default:
			/* invalide bank, disable access */
			attrib = 0;
			break;
		}

		size = gd->bd->bi_dram[i].size;
		base = gd->bd->bi_dram[i].start;
		if ((size) && (attrib))
			wrl(USB_WINDOW_CTRL(i),
				MVCPU_WIN_CTRL_DATA(size, USB_TARGET_DRAM,
					attrib, MVCPU_WIN_ENABLE));
		else
			wrl(USB_WINDOW_CTRL(i), MVCPU_WIN_DISABLE);

		wrl(USB_WINDOW_BASE(i), base);
	}
}

/*
 * Create the appropriate control structures to manage
 * a new EHCI host controller.
 */
int ehci_hcd_init(void)
{
	usb_brg_adrdec_setup();

	hccr = (struct ehci_hccr *)(MVUSB0_BASE + 0x100);
	hcor = (struct ehci_hcor *)((uint32_t) hccr
			+ HC_LENGTH(ehci_readl(&hccr->cr_capbase)));

	debug("ehci-marvell: init hccr %x and hcor %x hc_length %d\n",
		(uint32_t)hccr, (uint32_t)hcor,
		(uint32_t)HC_LENGTH(ehci_readl(&hccr->cr_capbase)));

	return 0;
}

/*
 * Destroy the appropriate control structures corresponding
 * the the EHCI host controller.
 */
int ehci_hcd_stop(void)
{
	return 0;
}
