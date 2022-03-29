/*
 * Freescale i.MX28 USB PHY Register Definitions
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

#ifndef __REGS_USBPHY_H__
#define __REGS_USBPHY_H__

struct mx28_usbphy_regs {
	mx28_reg_32(hw_usbphy_pwd)
	mx28_reg_32(hw_usbphy_tx)
	mx28_reg_32(hw_usbphy_rx)
	mx28_reg_32(hw_usbphy_ctrl)
	mx28_reg_32(hw_usbphy_status)
	mx28_reg_32(hw_usbphy_debug)
	mx28_reg_32(hw_usbphy_debug0_status)
	mx28_reg_32(hw_usbphy_debug1)
	mx28_reg_32(hw_usbphy_version)
	mx28_reg_32(hw_usbphy_ip)
};

#define	USBPHY_PWD_RXPWDRX				(1 << 20)
#define	USBPHY_PWD_RXPWDDIFF				(1 << 19)
#define	USBPHY_PWD_RXPWD1PT1				(1 << 18)
#define	USBPHY_PWD_RXPWDENV				(1 << 17)
#define	USBPHY_PWD_TXPWDV2I				(1 << 12)
#define	USBPHY_PWD_TXPWDIBIAS				(1 << 11)
#define	USBPHY_PWD_TXPWDFS				(1 << 10)

#define	USBPHY_TX_USBPHY_TX_EDGECTRL_OFFSET		26
#define	USBPHY_TX_USBPHY_TX_EDGECTRL_MASK		(0x7 << 26)
#define	USBPHY_TX_USBPHY_TX_SYNC_INVERT			(1 << 25)
#define	USBPHY_TX_USBPHY_TX_SYNC_MUX			(1 << 24)
#define	USBPHY_TX_TXENCAL45DP				(1 << 21)
#define	USBPHY_TX_TXCAL45DP_OFFSET			16
#define	USBPHY_TX_TXCAL45DP_MASK			(0xf << 16)
#define	USBPHY_TX_TXENCAL45DM				(1 << 13)
#define	USBPHY_TX_TXCAL45DM_OFFSET			8
#define	USBPHY_TX_TXCAL45DM_MASK			(0xf << 8)
#define	USBPHY_TX_D_CAL_OFFSET				0
#define	USBPHY_TX_D_CAL_MASK				0xf

#define	USBPHY_RX_RXDBYPASS				(1 << 22)
#define	USBPHY_RX_DISCONADJ_OFFSET			4
#define	USBPHY_RX_DISCONADJ_MASK			(0x7 << 4)
#define	USBPHY_RX_ENVADJ_OFFSET				0
#define	USBPHY_RX_ENVADJ_MASK				0x7

#define	USBPHY_CTRL_SFTRST				(1 << 31)
#define	USBPHY_CTRL_CLKGATE				(1 << 30)
#define	USBPHY_CTRL_UTMI_SUSPENDM			(1 << 29)
#define	USBPHY_CTRL_HOST_FORCE_LS_SE0			(1 << 28)
#define	USBPHY_CTRL_ENAUTOSET_USBCLKS			(1 << 26)
#define	USBPHY_CTRL_ENAUTOCLR_USBCLKGATE		(1 << 25)
#define	USBPHY_CTRL_FSDLL_RST_EN			(1 << 24)
#define	USBPHY_CTRL_ENVBUSCHG_WKUP			(1 << 23)
#define	USBPHY_CTRL_ENIDCHG_WKUP			(1 << 22)
#define	USBPHY_CTRL_ENDPDMCHG_WKUP			(1 << 21)
#define	USBPHY_CTRL_ENAUTOCLR_PHY_PWD			(1 << 20)
#define	USBPHY_CTRL_ENAUTOCLR_CLKGATE			(1 << 19)
#define	USBPHY_CTRL_ENAUTO_PWRON_PLL			(1 << 18)
#define	USBPHY_CTRL_WAKEUP_IRQ				(1 << 17)
#define	USBPHY_CTRL_ENIRQWAKEUP				(1 << 16)
#define	USBPHY_CTRL_ENUTMILEVEL3			(1 << 15)
#define	USBPHY_CTRL_ENUTMILEVEL2			(1 << 14)
#define	USBPHY_CTRL_DATA_ON_LRADC			(1 << 13)
#define	USBPHY_CTRL_DEVPLUGIN_IRQ			(1 << 12)
#define	USBPHY_CTRL_ENIRQDEVPLUGIN			(1 << 11)
#define	USBPHY_CTRL_RESUME_IRQ				(1 << 10)
#define	USBPHY_CTRL_ENIRQRESUMEDETECT			(1 << 9)
#define	USBPHY_CTRL_RESUMEIRQSTICKY			(1 << 8)
#define	USBPHY_CTRL_ENOTGIDDETECT			(1 << 7)
#define	USBPHY_CTRL_DEVPLUGIN_POLARITY			(1 << 5)
#define	USBPHY_CTRL_ENDEVPLUGINDETECT			(1 << 4)
#define	USBPHY_CTRL_HOSTDISCONDETECT_IRQ		(1 << 3)
#define	USBPHY_CTRL_ENIRQHOSTDISCON			(1 << 2)
#define	USBPHY_CTRL_ENHOSTDISCONDETECT			(1 << 1)

#define	USBPHY_STATUS_RESUME_STATUS			(1 << 10)
#define	USBPHY_STATUS_OTGID_STATUS			(1 << 8)
#define	USBPHY_STATUS_DEVPLUGIN_STATUS			(1 << 6)
#define	USBPHY_STATUS_HOSTDISCONDETECT_STATUS		(1 << 3)

#define	USBPHY_DEBUG_CLKGATE				(1 << 30)
#define	USBPHY_DEBUG_HOST_RESUME_DEBUG			(1 << 29)
#define	USBPHY_DEBUG_SQUELCHRESETLENGTH_OFFSET		25
#define	USBPHY_DEBUG_SQUELCHRESETLENGTH_MASK		(0xf << 25)
#define	USBPHY_DEBUG_ENSQUELCHRESET			(1 << 24)
#define	USBPHY_DEBUG_SQUELCHRESETCOUNT_OFFSET		16
#define	USBPHY_DEBUG_SQUELCHRESETCOUNT_MASK		(0x1f << 16)
#define	USBPHY_DEBUG_ENTX2RXCOUNT			(1 << 12)
#define	USBPHY_DEBUG_TX2RXCOUNT_OFFSET			8
#define	USBPHY_DEBUG_TX2RXCOUNT_MASK			(0xf << 8)
#define	USBPHY_DEBUG_ENHSTPULLDOWN_OFFSET		4
#define	USBPHY_DEBUG_ENHSTPULLDOWN_MASK			(0x3 << 4)
#define	USBPHY_DEBUG_HSTPULLDOWN_OFFSET			2
#define	USBPHY_DEBUG_HSTPULLDOWN_MASK			(0x3 << 2)
#define	USBPHY_DEBUG_DEBUG_INTERFACE_HOLD		(1 << 1)
#define	USBPHY_DEBUG_OTGIDPIDLOCK			(1 << 0)

#define	USBPHY_DEBUG0_STATUS_SQUELCH_COUNT_OFFSET	26
#define	USBPHY_DEBUG0_STATUS_SQUELCH_COUNT_MASK		(0x3f << 26)
#define	USBPHY_DEBUG0_STATUS_UTMI_RXERROR_OFFSET	16
#define	USBPHY_DEBUG0_STATUS_UTMI_RXERROR_MASK		(0x3ff << 16)
#define	USBPHY_DEBUG0_STATUS_LOOP_BACK_OFFSET		0
#define	USBPHY_DEBUG0_STATUS_LOOP_BACK_MASK		0xffff

#define	USBPHY_DEBUG1_ENTAILADJVD_OFFSET		13
#define	USBPHY_DEBUG1_ENTAILADJVD_MASK			(0x3 << 13)
#define	USBPHY_DEBUG1_ENTX2TX				(1 << 12)
#define	USBPHY_DEBUG1_DBG_ADDRESS_OFFSET		0
#define	USBPHY_DEBUG1_DBG_ADDRESS_MASK			0xf

#define	USBPHY_VERSION_MAJOR_MASK			(0xff << 24)
#define	USBPHY_VERSION_MAJOR_OFFSET			24
#define	USBPHY_VERSION_MINOR_MASK			(0xff << 16)
#define	USBPHY_VERSION_MINOR_OFFSET			16
#define	USBPHY_VERSION_STEP_MASK			0xffff
#define	USBPHY_VERSION_STEP_OFFSET			0

#define	USBPHY_IP_DIV_SEL_OFFSET			23
#define	USBPHY_IP_DIV_SEL_MASK				(0x3 << 23)
#define	USBPHY_IP_LFR_SEL_OFFSET			21
#define	USBPHY_IP_LFR_SEL_MASK				(0x3 << 21)
#define	USBPHY_IP_CP_SEL_OFFSET				19
#define	USBPHY_IP_CP_SEL_MASK				(0x3 << 19)
#define	USBPHY_IP_TSTI_TX_DP				(1 << 18)
#define	USBPHY_IP_TSTI_TX_DM				(1 << 17)
#define	USBPHY_IP_ANALOG_TESTMODE			(1 << 16)
#define	USBPHY_IP_EN_USB_CLKS				(1 << 2)
#define	USBPHY_IP_PLL_LOCKED				(1 << 1)
#define	USBPHY_IP_PLL_POWER				(1 << 0)

#endif	/* __REGS_USBPHY_H__ */
