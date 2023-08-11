/*
 * Copyright (c) 2015 The Linux Foundation. All rights reserved.

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

#ifndef _QCA8033_PHY_H_
#define _QCA8033_PHY_H_

#define QCA8033_PHY_CONTROL			0
#define QCA8033_PHY_STATUS			1
#define QCA8033_PHY_ID1				2
#define QCA8033_PHY_ID2				3
#define QCA8033_AUTONEG_ADVERT			4
#define QCA8033_LINK_PARTNER_ABILITY		5
#define QCA8033_AUTONEG_EXPANSION		6
#define QCA8033_NEXT_PAGE_TRANSMIT		7
#define QCA8033_LINK_PARTNER_NEXT_PAGE		8
#define QCA8033_1000BASET_CONTROL		9
#define QCA8033_1000BASET_STATUS		10
#define QCA8033_MMD_CTRL_REG			13
#define QCA8033_MMD_DATA_REG			14
#define QCA8033_EXTENDED_STATUS			15
#define QCA8033_PHY_SPEC_CONTROL		16
#define QCA8033_PHY_SPEC_STATUS			17
#define QCA8033_PHY_INTR_MASK			18
#define QCA8033_PHY_INTR_STATUS			19
#define QCA8033_PHY_CDT_CONTROL			22
#define QCA8033_PHY_CDT_STATUS			28
#define QCA8033_DEBUG_PORT_ADDRESS		29
#define QCA8033_DEBUG_PORT_DATA			30

 /* Phy Specific status fields offset:17 */
 /* 1=Speed & Duplex resolved */
#define QCA8033_STATUS_LINK_PASS	0x0400
#define QCA8033_STATUS_RESOVLED		0x0800
 /* 1=Duplex 0=Half Duplex */
#define QCA8033_STATUS_FULL_DUPLEX	0x2000
 /* Speed, bits 14:15 */
#define QCA8033_STATUS_SPEED		0xC000
#define QCA8033_STATUS_SPEED_MASK	0xC000
  /* 00=10Mbs */
#define QCA8033_STATUS_SPEED_10MBS	0x0000
 /* 01=100Mbs */
#define QCA8033_STATUS_SPEED_100MBS	0x4000

 /* 10=1000Mbs */
#define QCA8033_STATUS_SPEED_1000MBS	0x8000

#endif				/* _QCA8033_PHY_H_ */
