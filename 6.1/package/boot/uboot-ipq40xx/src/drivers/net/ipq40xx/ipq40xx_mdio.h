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
#ifndef _IPQ40XX_MDIO_H
#define _IPQ40XX_MDIO_H

#define IPQ40XX_MDIO_BASE	0x90000
#define MDIO_CTRL_0_REG		0x40
#define MDIO_CTRL_1_REG		0x44
#define MDIO_CTRL_2_REG		0x48
#define MDIO_CTRL_3_REG		0x4c
#define MDIO_CTRL_4_REG		0x50
#define MDIO_CTRL_4_ACCESS_BUSY		(1 << 16)
#define MDIO_CTRL_4_ACCESS_START	(1 << 8)
#define MDIO_CTRL_4_ACCESS_CODE_READ	0
#define MDIO_CTRL_4_ACCESS_CODE_WRITE	1

#define IPQ40XX_MDIO_RETRY	1000
#define IPQ40XX_MDIO_DELAY	5
#endif /* End _IPQ40XX_MDIO_H */
