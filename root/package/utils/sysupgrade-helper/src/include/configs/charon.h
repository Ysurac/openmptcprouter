/*
 * (C) Copyright 2003-2005
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2004-2006
 * Martin Krause, TQ-Systems GmbH, martin.krause@tqs.de
 *
 * (C) Copyright 2010
 * Heiko Schocher, DENX Software Engineering, hs@denx.de.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_CHARON_H
#define __CONFIG_CHARON_H

#define CONFIG_CHARON
#define CONFIG_HOSTNAME		charon

#define CONFIG_SYS_GPS_PORT_CONFIG	0x81550414

/* include common defines/options for TQM52xx boards */
#include "TQM5200.h"

/* defines special on charon board */
#undef CONFIG_RTC_MPC5200
#undef CONFIG_CMD_DATE

#undef CUSTOM_ENV_SETTINGS
#define CUSTOM_ENV_SETTINGS					\
	"bootfile=/tftpboot/charon/uImage\0"			\
	"fdt_file=/tftpboot/charon/charon.dtb\0"		\
	"u-boot=/tftpboot/charon/u-boot.bin\0"			\
	"mtdparts=" MTDPARTS_DEFAULT "\0"			\
	"addmtd=setenv bootargs ${bootargs} ${mtdparts}\0"


/* additional features on charon board */
#define CONFIG_RESET_PHY_R

/*
 * I2C configuration
 */
#define CONFIG_I2C_MULTI_BUS

#define CONFIG_SYS_TFP410_ADDR	0x38
#define CONFIG_SYS_TFP410_BUS	0

/*
 * FPGA configuration
 */
#define CONFIG_SYS_CS3_START		0xE8000000
#define CONFIG_SYS_CS3_SIZE		0x80000	/* 512 KByte */

/*
 * CS3 Config Register Init:
 * 	CS3 Enabled
 * 	AddrBus: 8bits
 *  	DataBus: 4bytes
 *  	Multiplexed: Yes
 *  	MuxBank: 00
 */
#define CONFIG_SYS_CS3_CFG		0x00009310

#endif /* __CONFIG_CHARON_H */
