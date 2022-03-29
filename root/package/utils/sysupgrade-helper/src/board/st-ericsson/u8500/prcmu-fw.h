/*
 * Copyright (C) 2009 ST-Ericsson SA
 *
 * Copied from the Linux version:
 * Author: Kumar Sanghvi <kumar.sanghvi@stericsson.com>
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
#ifndef __MACH_PRCMU_FW_V1_H
#define __MACH_PRCMU_FW_V1_H

#define AP_EXECUTE	2
#define I2CREAD		1
#define I2C_WR_OK	1
#define I2C_RD_OK	2
#define I2CWRITE	0

#define _PRCMU_TCDM_BASE    U8500_PRCMU_TCDM_BASE
#define PRCM_XP70_CUR_PWR_STATE (_PRCMU_TCDM_BASE + 0xFFC)      /* 4 BYTES */

#define PRCM_REQ_MB5        (_PRCMU_TCDM_BASE + 0xE44)    /* 4 bytes  */
#define PRCM_ACK_MB5        (_PRCMU_TCDM_BASE + 0xDF4)    /* 4 bytes */

/* Mailbox 5 Requests */
#define PRCM_REQ_MB5_I2COPTYPE_REG	(PRCM_REQ_MB5 + 0x0)
#define PRCM_REQ_MB5_BIT_FIELDS		(PRCM_REQ_MB5 + 0x1)
#define PRCM_REQ_MB5_I2CSLAVE		(PRCM_REQ_MB5 + 0x2)
#define PRCM_REQ_MB5_I2CVAL		(PRCM_REQ_MB5 + 0x3)

/* Mailbox 5 ACKs */
#define PRCM_ACK_MB5_STATUS	(PRCM_ACK_MB5 + 0x1)
#define PRCM_ACK_MB5_SLAVE	(PRCM_ACK_MB5 + 0x2)
#define PRCM_ACK_MB5_VAL	(PRCM_ACK_MB5 + 0x3)

#define LOW_POWER_WAKEUP	1
#define EXE_WAKEUP		0

#define REQ_MB5			5

extern int prcmu_i2c_read(u8 reg, u16 slave);
extern int prcmu_i2c_write(u8 reg, u16 slave, u8 reg_data);

#endif /* __MACH_PRCMU_FW_V1_H */
