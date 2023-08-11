/*
 * Copyright (c) 2014 The Linux Foundation. All rights reserved.
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

#include <linux/compat.h>
#include <linux/usb/dwc3.h>
#include <linux/usb/msm_usb30.h>

#include "xhci.h"

#define IPQ_XHCI_BASE_1 0x11000000
#define IPQ_XHCI_BASE_2 0x10000000
#define USB30_RESET             0x00903B50
#define USB30_1_RESET           0x00903B58
#define DWC3_SSUSB_REG_GCTL         0xC110
#define DWC3_SSUSB_REG_GUSB2PHYCFG_PHYSOFTRST           (1 << 31)
#define DWC3_SSUSB_REG_GUSB2PHYCFG(n)       (0xC200 + ((n) * 0x16))
#define DWC3_SSUSB_REG_GUSB3PIPECTL_PHYSOFTRST              (1 << 31)
#define DWC3_SSUSB_REG_GUSB3PIPECTL(n)      (0xC2C0 + ((n) * 0x16))
#define DWC3_SSUSB_PHY_RTUNE_RTUNE_CTRL_REG     0x34
#define DWC3_SSUSB_PHY_RTUNE_DEBUG_REG          0x3
#define RX_TERM_VALUE           0
#define RX_EQ_VALUE             4
#define DWC3_SSUSB_PHY_RX_OVRD_IN_HI_REG        0x1006
#define DWC3_SSUSB_PHY_RX_OVRD_IN_HI_RX_EQ_EN       6
#define DWC3_SSUSB_PHY_RX_OVRD_IN_HI_RX_EQ_EN_OVRD  7
#define DWC3_SSUSB_PHY_RX_OVRD_IN_HI_RX_EQ_MASK     0x7
#define DWC3_SSUSB_PHY_RX_OVRD_IN_HI_RX_EQ      8
#define DWC3_SSUSB_PHY_RX_OVRD_IN_HI_RX_EQ_OVRD     11
#define AMPLITUDE_VALUE         110
#define TX_DEEMPH_3_5DB         23
#define DWC3_SSUSB_PHY_TX_OVRD_DRV_LO_REG       0x1002
#define DWC3_SSUSB_PHY_TX_DEEMPH_MASK           0x3F80
#define DWC3_SSUSB_PHY_AMP_MASK             0x7F
#define DWC3_SSUSB_PHY_AMP_EN               (1 << 14)
#define DWC3_SSUSB_REG_GUCTL            0xC12C
#define DWC3_SSUSB_PHY_TX_ALT_BLOCK_REG         0x102D
#define DWC3_SSUSB_PHY_TX_ALT_BLOCK_EN_ALT_BUS      (1 << 7)
#define DWC3_GCTL_U2EXIT_LFPS   (1 << 2)
#define DWC3_GCTL_SOFITPSYNC    (1 << 10)
#define DWC3_GCTL       0xc110
#define DWC3_SSUSB_REG_GUSB2PHYCFG_SUSPENDUSB20         (1 << 6)
#define DWC3_SSUSB_REG_GUSB2PHYCFG_ENBLSLPM         (1 << 8)
#define DWC3_SSUSB_REG_GUSB2PHYCFG_USBTRDTIM(n)         ((n) << 10)
#define DWC3_SSUSB_REG_GUSB3PIPECTL_ELASTIC_BUFFER_MODE         (1 << 0)
#define DWC3_SSUSB_REG_GUSB3PIPECTL_TX_DE_EPPHASIS(n)           ((n) << 1)
#define DWC3_SSUSB_REG_GUSB3PIPECTL_TX_MARGIN(n)            ((n) << 3)
#define DWC3_SSUSB_REG_GUSB3PIPECTL_DELAYP1TRANS            (1 << 18)
#define DWC3_SSUSB_REG_GUSB3PIPECTL_DELAYP1P2P3(n)          ((n) << 19)
#define DWC3_SSUSB_REG_GUSB3PIPECTL_U1U2EXITFAIL_TO_RECOV       (1 << 25)
#define DWC3_SSUSB_REG_GUSB3PIPECTL_REQUEST_P1P2P3          (1 << 24)
#define DWC3_SSUSB_PHY_TX_DEEMPH_SHIFT          7
#define DWC3_SSUSB_XHCI_REV_10          1

extern board_ipq806x_params_t board_params[];
extern board_ipq806x_params_t *gboard_param;

/* Declare global data pointer */
DECLARE_GLOBAL_DATA_PTR;

struct ipq_xhci {
	struct xhci_hccr *hcd;
	struct dwc3 *dwc3_reg;
};
static struct ipq_xhci ipq;

inline int __board_usb_init(int index, enum usb_init_type init)
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

void ipq_reset_usb_phy(struct dwc3 *dwc3_reg)
{
	/* Assert USB3 PHY reset */
	setbits_le32(&dwc3_reg->g_usb3pipectl[0], DWC3_GUSB3PIPECTL_PHYSOFTRST);

	/* Assert USB2 PHY reset */
	setbits_le32(&dwc3_reg->g_usb2phycfg, DWC3_GUSB2PHYCFG_PHYSOFTRST);

	mdelay(100);

	/* Clear USB3 PHY reset */
	clrbits_le32(&dwc3_reg->g_usb3pipectl[0], DWC3_GUSB3PIPECTL_PHYSOFTRST);

	/* Clear USB2 PHY reset */
	clrbits_le32(&dwc3_reg->g_usb2phycfg, DWC3_GUSB2PHYCFG_PHYSOFTRST);

}
static void dwc3_core_soft_reset(struct dwc3 *dwc3_reg)
{
	/* Before Resetting PHY, put Core in Reset */
	setbits_le32(&dwc3_reg->g_ctl, DWC3_GCTL_CORESOFTRESET);

	ipq_reset_usb_phy(dwc3_reg);

	/* After PHYs are stable we can take Core out of reset state */
	clrbits_le32(&dwc3_reg->g_ctl, DWC3_GCTL_CORESOFTRESET);
}

static int dwc3_core_init(struct dwc3 *dwc3_reg)
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

	dwc3_core_soft_reset(dwc3_reg);

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

static u16 dwc3_ipq_ssusb_read_phy_reg(unsigned int  addr, unsigned int ipq_base)
{
	u16 tmp_phy[3], i;
	do {
		for (i = 0; i < 3; i++) {
			writel(addr, ipq_base +
				IPQ_SSUSB_REG_QSCRATCH_SS_CR_PROTOCOL_DATA_IN);
			writel(0x1, ipq_base +
				IPQ_SSUSB_REG_QSCRATCH_SS_CR_PROTOCOL_CAP_ADDR);
			while (0 != readl(ipq_base +
				IPQ_SSUSB_REG_QSCRATCH_SS_CR_PROTOCOL_CAP_ADDR));
			writel(0x1, ipq_base +
				IPQ_SSUSB_REG_QSCRATCH_SS_CR_PROTOCOL_READ);
			while (0 != readl(ipq_base +
				IPQ_SSUSB_REG_QSCRATCH_SS_CR_PROTOCOL_READ));
			tmp_phy[i] = (u16)readl(ipq_base +
				IPQ_SSUSB_REG_QSCRATCH_SS_CR_PROTOCOL_DATA_OUT);
		}
	} while (tmp_phy[1] != tmp_phy[2]);
	return tmp_phy[2];
}

static void dwc3_ipq_ssusb_write_phy_reg(u32 addr, u16 data, unsigned int ipq_base)
{
	writel(addr, ipq_base + IPQ_SSUSB_REG_QSCRATCH_SS_CR_PROTOCOL_DATA_IN);
	writel(0x1, ipq_base + IPQ_SSUSB_REG_QSCRATCH_SS_CR_PROTOCOL_CAP_ADDR);
	while (0 != readl(ipq_base +
		IPQ_SSUSB_REG_QSCRATCH_SS_CR_PROTOCOL_CAP_ADDR));
	writel(data, ipq_base + IPQ_SSUSB_REG_QSCRATCH_SS_CR_PROTOCOL_DATA_IN);
	writel(0x1, ipq_base + IPQ_SSUSB_REG_QSCRATCH_SS_CR_PROTOCOL_CAP_DATA);
	while (0 != readl(ipq_base +
		IPQ_SSUSB_REG_QSCRATCH_SS_CR_PROTOCOL_CAP_DATA));
	writel(0x1, ipq_base + IPQ_SSUSB_REG_QSCRATCH_SS_CR_PROTOCOL_WRITE);
	while (0 != readl(ipq_base +
		IPQ_SSUSB_REG_QSCRATCH_SS_CR_PROTOCOL_WRITE));
}

static void ipq_ssusb_clear_bits32(u32 offset, u32 bits, unsigned int ipq_base)
{
	u32 data;
	data = readl(ipq_base+offset);
	data = data & ~bits;
	writel(data, ipq_base + offset);
}

static void ipq_ssusb_clear_and_set_bits32(u32 offset, u32 clear_bits, u32 set_bits, unsigned int ipq_base)
{
	u32 data;
	data = readl(ipq_base + offset);
	data = (data & ~clear_bits) | set_bits;
	writel(data, ipq_base + offset);
}

static void partial_rx_reset_init(unsigned int ipq_base)
{
        u32 addr = DWC3_SSUSB_PHY_TX_ALT_BLOCK_REG;
        u16 data = dwc3_ipq_ssusb_read_phy_reg(addr, ipq_base);
        data |= DWC3_SSUSB_PHY_TX_ALT_BLOCK_EN_ALT_BUS;
        dwc3_ipq_ssusb_write_phy_reg(addr, data, ipq_base);
        return;
}

static void  uw_ssusb_pre_init(unsigned int ipq_base)
{
	u32  set_bits, tmp;

	/* GCTL Reset ON */
	writel(0x800, ipq_base + DWC3_SSUSB_REG_GCTL);
	/* Config SS PHY CTRL */
	set_bits = 0;
	writel(0x80, ipq_base + IPQ_SS_PHY_CTRL_REG);
	udelay(5);
	ipq_ssusb_clear_bits32(IPQ_SS_PHY_CTRL_REG, 0x80, ipq_base);
	udelay(5);
	/* REF_USE_PAD */
	set_bits = 0x0000000;  /* USE Internal clock */
	set_bits |= IPQ_SSUSB_QSCRATCH_SS_PHY_CTRL_LANE0_PWR_PRESENT;
	set_bits |= IPQ_SSUSB_QSCRATCH_SS_PHY_CTRL_REF_SS_PHY_EN;
	writel(set_bits, ipq_base + IPQ_SS_PHY_CTRL_REG);
	/* Config HS PHY CTRL */
	set_bits = IPQ_SSUSB_REG_QSCRATCH_HS_PHY_CTRL_UTMI_OTG_VBUS_VALID;
	/*
	 * COMMONONN forces xo, bias and pll to stay on during suspend;
	 * Allowing suspend (writing 1) kills Aragorn V1
	 */
	set_bits |= IPQ_SSUSB_REG_QSCRATCH_HS_PHY_CTRL_COMMONONN;
	set_bits |= IPQ_SSUSB_REG_QSCRATCH_HS_PHY_CTRL_USE_CLKCORE;
	set_bits |= IPQ_SSUSB_REG_QSCRATCH_HS_PHY_CTRL_FSEL_VAL;
	/*
	 * If the configuration of clocks is not bypassed in Host mode,
	 * HS PHY suspend needs to be prohibited, otherwise - SS connection fails
	 */
	ipq_ssusb_clear_and_set_bits32(IPQ_SSUSB_REG_QSCRATCH_HS_PHY_CTRL, 0,
					set_bits, ipq_base);
	/* USB2 PHY Reset ON */
	writel(DWC3_SSUSB_REG_GUSB2PHYCFG_PHYSOFTRST, ipq_base +
		DWC3_SSUSB_REG_GUSB2PHYCFG(0));
	/* USB3 PHY Reset ON */
	writel(DWC3_SSUSB_REG_GUSB3PIPECTL_PHYSOFTRST, ipq_base +
		DWC3_SSUSB_REG_GUSB3PIPECTL(0));
	udelay(5);
	/* USB3 PHY Reset OFF */
	ipq_ssusb_clear_bits32(DWC3_SSUSB_REG_GUSB3PIPECTL(0),
				DWC3_SSUSB_REG_GUSB3PIPECTL_PHYSOFTRST, ipq_base);
	ipq_ssusb_clear_bits32(DWC3_SSUSB_REG_GUSB2PHYCFG(0),
				DWC3_GUSB2PHYCFG_PHYSOFTRST, ipq_base);
	udelay(5);
	/* GCTL Reset OFF */
	ipq_ssusb_clear_bits32(DWC3_SSUSB_REG_GCTL, DWC3_GCTL_CORESOFTRESET,
				ipq_base);
	udelay(5);
	if (RX_TERM_VALUE) {
		dwc3_ipq_ssusb_write_phy_reg(DWC3_SSUSB_PHY_RTUNE_RTUNE_CTRL_REG,
						0, ipq_base);
		dwc3_ipq_ssusb_write_phy_reg(DWC3_SSUSB_PHY_RTUNE_DEBUG_REG,
						0x0448, ipq_base);
		dwc3_ipq_ssusb_write_phy_reg(DWC3_SSUSB_PHY_RTUNE_DEBUG_REG,
						RX_TERM_VALUE, ipq_base);
	}
	if (0 != RX_EQ_VALUE) { /* Values from 1 to 7 */
		tmp =0;
		/*
		 * 1. Fixed EQ setting. This can be achieved as follows:
		 * LANE0.RX_OVRD_IN_HI. RX_EQ_EN set to 0 - address 1006 bit 6
		 * LANE0.RX_OVRD_IN_HI.RX_EQ_EN_OVRD set to 1 0- address 1006 bit 7
		 * LANE0.RX_OVRD_IN_HI.RX_EQ set to 4 (also try setting 3 if possible) -
		 * address 1006 bits 10:8 - please make this a variable, if unchanged the section is not executed
		 * LANE0.RX_OVRD_IN_HI.RX_EQ_OVRD set to 1 - address 1006 bit 11
		 */
		tmp = dwc3_ipq_ssusb_read_phy_reg(DWC3_SSUSB_PHY_RX_OVRD_IN_HI_REG,
							ipq_base);
		tmp &= ~((u16)1 << DWC3_SSUSB_PHY_RX_OVRD_IN_HI_RX_EQ_EN);
		tmp |= ((u16)1 << DWC3_SSUSB_PHY_RX_OVRD_IN_HI_RX_EQ_EN_OVRD);
		tmp &= ~((u16) DWC3_SSUSB_PHY_RX_OVRD_IN_HI_RX_EQ_MASK <<
			DWC3_SSUSB_PHY_RX_OVRD_IN_HI_RX_EQ);
		tmp |= RX_EQ_VALUE << DWC3_SSUSB_PHY_RX_OVRD_IN_HI_RX_EQ;
		tmp |= 1 << DWC3_SSUSB_PHY_RX_OVRD_IN_HI_RX_EQ_OVRD;
		dwc3_ipq_ssusb_write_phy_reg(DWC3_SSUSB_PHY_RX_OVRD_IN_HI_REG,
						tmp, ipq_base);
	}
	if ((113 != AMPLITUDE_VALUE) || (21 != TX_DEEMPH_3_5DB)) {
		tmp = dwc3_ipq_ssusb_read_phy_reg(DWC3_SSUSB_PHY_TX_OVRD_DRV_LO_REG,
			ipq_base);
		tmp &= ~DWC3_SSUSB_PHY_TX_DEEMPH_MASK;
		tmp |= (TX_DEEMPH_3_5DB << DWC3_SSUSB_PHY_TX_DEEMPH_SHIFT);
		tmp &= ~DWC3_SSUSB_PHY_AMP_MASK;
		tmp |= AMPLITUDE_VALUE;
		tmp |= DWC3_SSUSB_PHY_AMP_EN;
		dwc3_ipq_ssusb_write_phy_reg(DWC3_SSUSB_PHY_TX_OVRD_DRV_LO_REG,
						tmp, ipq_base);
	}
	ipq_ssusb_clear_and_set_bits32(IPQ_SS_PHY_PARAM_CTRL_1_REG,
					0x7, 0x5, ipq_base);
	/* XHCI REV */
	writel((1 << 2), ipq_base + IPQ_QSCRATCH_GENERAL_CFG);
	writel(0x0c80c010, ipq_base + DWC3_SSUSB_REG_GUCTL);
	partial_rx_reset_init(ipq_base);
	set_bits = 0;
	/* Test  U2EXIT_LFPS */
	set_bits |= IPQ_SSUSB_REG_GCTL_U2EXIT_LFPS;
	ipq_ssusb_clear_and_set_bits32(DWC3_SSUSB_REG_GCTL, 0,
					set_bits, ipq_base);
	set_bits = 0;
	set_bits |= IPQ_SSUSB_REG_GCTL_U2RSTECN;
	set_bits |= IPQ_SSUSB_REG_GCTL_U2EXIT_LFPS;
	ipq_ssusb_clear_and_set_bits32(DWC3_SSUSB_REG_GCTL, 0,
					set_bits, ipq_base);
	writel(DWC3_GCTL_U2EXIT_LFPS | DWC3_GCTL_SOFITPSYNC |
		DWC3_GCTL_PRTCAPDIR(1) |
		DWC3_GCTL_U2RSTECN | DWC3_GCTL_PWRDNSCALE(2),
		ipq_base + DWC3_GCTL);
	writel((IPQ_SSUSB_QSCRATCH_SS_PHY_CTRL_MPLL_MULTI(0x19) |
		IPQ_SSUSB_QSCRATCH_SS_PHY_CTRL_REF_SS_PHY_EN |
		IPQ_SSUSB_QSCRATCH_SS_PHY_CTRL_LANE0_PWR_PRESENT),
		ipq_base + IPQ_SS_PHY_CTRL_REG);
	writel((DWC3_SSUSB_REG_GUSB2PHYCFG_SUSPENDUSB20 |
		DWC3_SSUSB_REG_GUSB2PHYCFG_ENBLSLPM |
		DWC3_SSUSB_REG_GUSB2PHYCFG_USBTRDTIM(9)),
		ipq_base + DWC3_SSUSB_REG_GUSB2PHYCFG(0));
	writel(DWC3_SSUSB_REG_GUSB3PIPECTL_ELASTIC_BUFFER_MODE |
		DWC3_SSUSB_REG_GUSB3PIPECTL_TX_DE_EPPHASIS(1) |
		DWC3_SSUSB_REG_GUSB3PIPECTL_TX_MARGIN(0)|
		DWC3_SSUSB_REG_GUSB3PIPECTL_DELAYP1TRANS |
		DWC3_SSUSB_REG_GUSB3PIPECTL_DELAYP1P2P3(1) |
		DWC3_SSUSB_REG_GUSB3PIPECTL_U1U2EXITFAIL_TO_RECOV |
		DWC3_SSUSB_REG_GUSB3PIPECTL_REQUEST_P1P2P3,
		ipq_base + DWC3_SSUSB_REG_GUSB3PIPECTL(0));
	writel(IPQ_SSUSB_REG_QSCRATCH_SS_PHY_PARAM_CTRL_1_LOS_LEVEL(0x9) |
		IPQ_SSUSB_REG_QSCRATCH_SS_PHY_PARAM_CTRL_1_TX_DEEMPH_3_5DB(0x17) |
		IPQ_SSUSB_REG_QSCRATCH_SS_PHY_PARAM_CTRL_1_TX_DEEMPH_6DB(0x20) |
		IPQ_SSUSB_REG_QSCRATCH_SS_PHY_PARAM_CTRL_1_TX_SWING_FULL(0x6E),
		ipq_base + IPQ_SS_PHY_PARAM_CTRL_1_REG);
	writel(IPQ_SSUSB_REG_QSCRATCH_GENERAL_CFG_XHCI_REV(DWC3_SSUSB_XHCI_REV_10),
		ipq_base + IPQ_QSCRATCH_GENERAL_CFG);
}

static void usb30_common_pre_init(int id, unsigned int ipq_base)
{
	unsigned int reg;
	if (id == 0)
		reg = USB30_RESET;
	else
		reg = USB30_1_RESET;

	writel(IPQ_USB30_RESET_PHY_ASYNC_RESET, reg);
	writel(IPQ_USB30_RESET_POWERON_ASYNC_RESET |
		IPQ_USB30_RESET_PHY_ASYNC_RESET, reg);
	writel(IPQ_USB30_RESET_MOC_UTMI_ASYNC_RESET |
		IPQ_USB30_RESET_POWERON_ASYNC_RESET |
		IPQ_USB30_RESET_PHY_ASYNC_RESET, reg);
	writel(IPQ_USB30_RESET_SLEEP_ASYNC_RESET |
		IPQ_USB30_RESET_MOC_UTMI_ASYNC_RESET |
		IPQ_USB30_RESET_POWERON_ASYNC_RESET |
		IPQ_USB30_RESET_PHY_ASYNC_RESET, reg);
	writel(IPQ_USB30_RESET_MASTER_ASYNC_RESET |
		IPQ_USB30_RESET_SLEEP_ASYNC_RESET |
		IPQ_USB30_RESET_MOC_UTMI_ASYNC_RESET |
		IPQ_USB30_RESET_POWERON_ASYNC_RESET |
		IPQ_USB30_RESET_PHY_ASYNC_RESET, reg);
	if (id == 0) {
		writel(IPQ_USB30_RESET_PORT2_HS_PHY_ASYNC_RESET |
			IPQ_USB30_RESET_MASTER_ASYNC_RESET |
			IPQ_USB30_RESET_SLEEP_ASYNC_RESET |
			IPQ_USB30_RESET_MOC_UTMI_ASYNC_RESET |
			IPQ_USB30_RESET_POWERON_ASYNC_RESET |
			IPQ_USB30_RESET_PHY_ASYNC_RESET, reg);
	}
	udelay(5);
	writel(IPQ_USB30_RESET_MASK & ~(IPQ_USB30_RESET_PHY_ASYNC_RESET), reg);
	writel(IPQ_USB30_RESET_MASK & ~(IPQ_USB30_RESET_POWERON_ASYNC_RESET |
		IPQ_USB30_RESET_PHY_ASYNC_RESET), reg);
	writel(IPQ_USB30_RESET_MASK & ~(IPQ_USB30_RESET_MOC_UTMI_ASYNC_RESET |
		IPQ_USB30_RESET_POWERON_ASYNC_RESET |
		IPQ_USB30_RESET_PHY_ASYNC_RESET), reg);
	writel(IPQ_USB30_RESET_MASK & ~(IPQ_USB30_RESET_SLEEP_ASYNC_RESET |
		IPQ_USB30_RESET_MOC_UTMI_ASYNC_RESET |
		IPQ_USB30_RESET_POWERON_ASYNC_RESET |
		IPQ_USB30_RESET_PHY_ASYNC_RESET), reg);
	writel(IPQ_USB30_RESET_MASK & ~(IPQ_USB30_RESET_MASTER_ASYNC_RESET |
		IPQ_USB30_RESET_SLEEP_ASYNC_RESET|
		IPQ_USB30_RESET_MOC_UTMI_ASYNC_RESET |
		IPQ_USB30_RESET_POWERON_ASYNC_RESET |
		IPQ_USB30_RESET_PHY_ASYNC_RESET), reg);
	if (id == 0) {
		writel(IPQ_USB30_RESET_MASK &
			~(IPQ_USB30_RESET_PORT2_HS_PHY_ASYNC_RESET |
			IPQ_USB30_RESET_MASTER_ASYNC_RESET |
			IPQ_USB30_RESET_SLEEP_ASYNC_RESET |
			IPQ_USB30_RESET_MOC_UTMI_ASYNC_RESET |
			IPQ_USB30_RESET_POWERON_ASYNC_RESET |
			IPQ_USB30_RESET_PHY_ASYNC_RESET), reg);
		reg = IPQ_TCSR_USB_CONTROLLER_TYPE_SEL;
		if (reg) {
			writel(0x3, reg);
		}
	}
	writel((IPQ_SSUSB_REG_QSCRATCH_CGCTL_RAM1112_EN |
		IPQ_SSUSB_REG_QSCRATCH_CGCTL_RAM13_EN),
		ipq_base + IPQ_SSUSB_REG_QSCRATCH_CGCTL);
	writel((IPQ_SSUSB_REG_QSCRATCH_RAM1_RAM13_EN |
		IPQ_SSUSB_REG_QSCRATCH_RAM1RAM12_EN |
		IPQ_SSUSB_REG_QSCRATCH_RAM1_RAM11_EN),
		ipq_base + IPQ_SSUSB_REG_QSCRATCH_RAM1);
}


static int ipq_xhci_core_init(struct ipq_xhci *ipq, unsigned int ipq_base)
{
	int ret = 0;

	/* Configure the usb core clock */
    usb_ss_core_clock_config(0,
		gboard_param->usb_core_mnd_value.m_value,
		gboard_param->usb_core_mnd_value.n_value,
		gboard_param->usb_core_mnd_value.d_value,
		gboard_param->clk_dummy);
	/* Configure the usb core clock */
    usb_ss_utmi_clock_config(0,
		gboard_param->usb_utmi_mnd_value.m_value,
		gboard_param->usb_utmi_mnd_value.n_value,
		gboard_param->usb_utmi_mnd_value.d_value,
		gboard_param->clk_dummy);

	usb30_common_pre_init(0, ipq_base);
	uw_ssusb_pre_init(ipq_base);
	ret = dwc3_core_init(ipq->dwc3_reg);
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
	struct ipq_xhci *ctx = &ipq;
	unsigned int ipq_base;
	int ret = 0;

	if ( index == 0 )
		ipq_base = IPQ_XHCI_BASE_1;
	else
		ipq_base = IPQ_XHCI_BASE_2;

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

	debug("omap-xhci: init hccr %x and hcor %x hc_length %d\n",
	      (uint32_t)*hccr, (uint32_t)*hcor,
	      (uint32_t)HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));

	return ret;
}

void xhci_hcd_stop(int index)
{
	struct ipq_xhci *ctx = &ipq;

	ipq_xhci_core_exit(ctx);
}
