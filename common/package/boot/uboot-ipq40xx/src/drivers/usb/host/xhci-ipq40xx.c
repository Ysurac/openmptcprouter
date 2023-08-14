/*
 * Copyright (c) 2014, 2015 The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <usb.h>
#include <asm-generic/errno.h>
#include <linux/usb/dwc3.h>
#include "xhci.h"

#define IPQ_XHCI_COUNT		0x2
#define IPQ_XHCI_BASE_1 	0x8A00000
#define IPQ_XHCI_BASE_2 	0x6000000
#define GCC_USB_RST_CTRL_1	0x181E038
#define GCC_USB_RST_CTRL_2	0x181E01C

struct ipq_xhci {
	struct xhci_hccr *hcd;
	struct dwc3 *dwc3_reg;
	unsigned int *gcc_rst_ctrl;
};
static struct ipq_xhci ipq[IPQ_XHCI_COUNT];

int __board_usb_init(int index, enum usb_init_type init)
{
	return 0;
}

int board_usb_init(int index, enum usb_init_type init)
	__attribute__((weak, alias("__board_usb_init")));

static void dwc3_set_mode(struct dwc3 *dwc3_reg, u32 mode)
{
	clrsetbits_le32(&dwc3_reg->g_ctl,
			DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG),
			DWC3_GCTL_PRTCAPDIR(mode));
}

void ipq_reset_usb_phy(unsigned int *gcc_rst_ctrl)
{
	/* assert HS PHY POR reset */
	setbits_le32(gcc_rst_ctrl, 0x10);
	mdelay(10);

	/* assert HS PHY SRIF reset */
	setbits_le32(gcc_rst_ctrl, 0x4);
	mdelay(10);

	/* deassert HS PHY SRIF reset and program HS PHY registers */
	clrbits_le32(gcc_rst_ctrl, 0x4);
	mdelay(10);

	/* de-assert USB3 HS PHY POR reset */
	clrbits_le32(gcc_rst_ctrl, 0x10);
	mdelay(10);

	if (gcc_rst_ctrl == (unsigned int*)GCC_USB_RST_CTRL_1) {
		/* assert SS PHY POR reset */
		setbits_le32(gcc_rst_ctrl, 0x20);
		mdelay(10);

		/* deassert SS PHY POR reset */
		clrbits_le32(gcc_rst_ctrl, 0x20);
	}
}

void dwc3_reset_usb_phy(struct dwc3 *dwc3_reg, unsigned int *gcc_rst_ctrl)
{
	/* Assert USB3 PHY reset */
	setbits_le32(&dwc3_reg->g_usb3pipectl[0], DWC3_GUSB3PIPECTL_PHYSOFTRST);

	/* Assert USB2 PHY reset */
	setbits_le32(&dwc3_reg->g_usb2phycfg, DWC3_GUSB2PHYCFG_PHYSOFTRST);

	mdelay(100);

	ipq_reset_usb_phy(gcc_rst_ctrl);

	mdelay(100);

	/* Clear USB3 PHY reset */
	clrbits_le32(&dwc3_reg->g_usb3pipectl[0], DWC3_GUSB3PIPECTL_PHYSOFTRST);

	/* Clear USB2 PHY reset */
	clrbits_le32(&dwc3_reg->g_usb2phycfg, DWC3_GUSB2PHYCFG_PHYSOFTRST);
}

static void dwc3_core_soft_reset(struct dwc3 *dwc3_reg, unsigned int *gcc_rst_ctrl)
{
	/* Before Resetting PHY, put Core in Reset */
	setbits_le32(&dwc3_reg->g_ctl, DWC3_GCTL_CORESOFTRESET);

	dwc3_reset_usb_phy(dwc3_reg, gcc_rst_ctrl);

	/* After PHYs are stable we can take Core out of reset state */
	clrbits_le32(&dwc3_reg->g_ctl, DWC3_GCTL_CORESOFTRESET);
}

static int dwc3_core_init(struct dwc3 *dwc3_reg, unsigned int *gcc_rst_ctrl)
{
	u32 reg;
	u32 revision;
	unsigned int dwc3_hwparams1;

	revision = readl(&dwc3_reg->g_snpsid);
	/* This should read as U3 followed by revision number */
	if ((revision & DWC3_GSNPSID_MASK) != 0x55330000) {
		puts("this is not a DesignWare USB3 DRD Core\n");
		return -1;
	}

	dwc3_core_soft_reset(dwc3_reg, gcc_rst_ctrl);

	dwc3_hwparams1 = readl(&dwc3_reg->g_hwparams1);

	reg = readl(&dwc3_reg->g_ctl);
	reg &= ~DWC3_GCTL_SCALEDOWN_MASK;
	reg &= ~DWC3_GCTL_DISSCRAMBLE;
	switch (DWC3_GHWPARAMS1_EN_PWROPT(dwc3_hwparams1)) {
	case DWC3_GHWPARAMS1_EN_PWROPT_CLK:
		reg &= ~DWC3_GCTL_DSBLCLKGTNG;
		break;
	default:
		debug("No power optimization available\n");
	}

	/*
	 * WORKAROUND: DWC3 revisions <1.90a have a bug
	 * where the device can fail to connect at SuperSpeed
	 * and falls back to high-speed mode which causes
	 * the device to enter a Connect/Disconnect loop
	 */
	if ((revision & DWC3_REVISION_MASK) < 0x190a)
		reg |= DWC3_GCTL_U2RSTECN;

	writel(reg, &dwc3_reg->g_ctl);

	return 0;
}

static int ipq_xhci_core_init(struct ipq_xhci *ipq, unsigned int ipq_base)
{
	int ret = 0;

	ret = dwc3_core_init(ipq->dwc3_reg, ipq->gcc_rst_ctrl);
	if (ret) {
		debug("%s:failed to initialize core\n", __func__);
		return ret;
	}

	/* We are hard-coding DWC3 core to Host Mode */
	dwc3_set_mode(ipq->dwc3_reg, DWC3_GCTL_PRTCAP_HOST);

	return ret;
}

static void ipq_xhci_core_exit(struct ipq_xhci *ipq)
{

}

int xhci_hcd_init(int index, struct xhci_hccr **hccr, struct xhci_hcor **hcor)
{
	struct ipq_xhci *ctx;
	unsigned int ipq_base;
	int ret = 0;

	if (index >= IPQ_XHCI_COUNT)
		return -1;

	if (index == 0) {
		ctx = &ipq[0];
		ipq_base = IPQ_XHCI_BASE_1;
		ctx->gcc_rst_ctrl = (unsigned int*)GCC_USB_RST_CTRL_1;
	} else {
		ctx = &ipq[1];
		ipq_base = IPQ_XHCI_BASE_2;
		ctx->gcc_rst_ctrl = (unsigned int*)GCC_USB_RST_CTRL_2;
	}

	ctx->hcd = (struct xhci_hccr *)ipq_base;
	ctx->dwc3_reg = (struct dwc3 *)((char *)(ctx->hcd) + DWC3_REG_OFFSET);

	ret = board_usb_init(index, USB_INIT_HOST);
	if (ret != 0) {
		puts("Failed to initialize board for USB\n");
		return ret;
	}

	ret = ipq_xhci_core_init(ctx, ipq_base);
	if (ret < 0) {
		puts("Failed to initialize xhci\n");
		return ret;
	}

	*hccr = (struct xhci_hccr *)(ipq_base);
	*hcor = (struct xhci_hcor *)((uint32_t) *hccr
				+ HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));

	debug("ipq-xhci: init hccr %x and hcor %x hc_length %d\n",
	      (uint32_t)*hccr, (uint32_t)*hcor,
	      (uint32_t)HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));

	return ret;
}

void xhci_hcd_stop(int index)
{
	if (index >= IPQ_XHCI_COUNT) {
		debug("ipq-xhci: index greater than xhci count\n");
		return;
	}

	ipq_xhci_core_exit(&ipq[index]);
}
