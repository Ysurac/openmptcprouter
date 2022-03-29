/*
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Prafulla Wadaskar <prafulla@marvell.com>
 *
 * (C) Copyright 2009
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
 *
 * (C) Copyright 2011-2012
 * Holger Brunck, Keymile GmbH Hannover, holger.brunck@keymile.com
 * Valentin Longchamp, Keymile AG, valentin.longchamp@keymile.com
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

/*
 * for linking errors see
 * http://lists.denx.de/pipermail/u-boot/2009-July/057350.html
 */

#ifndef _CONFIG_KM_KIRKWOOD_H
#define _CONFIG_KM_KIRKWOOD_H

/* KM_KIRKWOOD */
#if defined(CONFIG_KM_KIRKWOOD)
#define CONFIG_IDENT_STRING		"\nKeymile Kirkwood"
#define CONFIG_HOSTNAME			km_kirkwood
#define CONFIG_KM_DISABLE_PCIE
#define KM_IVM_BUS			"pca9544a:70:9"	/* I2C2 (Mux-Port 1)*/

/* KM_KIRKWOOD_PCI */
#elif defined(CONFIG_KM_KIRKWOOD_PCI)
#define CONFIG_IDENT_STRING		"\nKeymile Kirkwood PCI"
#define CONFIG_HOSTNAME			km_kirkwood_pci
#define KM_IVM_BUS			"pca9544a:70:9"	/* I2C2 (Mux-Port 1)*/
#define CONFIG_KM_FPGA_CONFIG

/* KM_NUSA */
#elif defined(CONFIG_KM_NUSA)
#define KM_IVM_BUS			"pca9547:70:9"	/* I2C2 (Mux-Port 1)*/
#define CONFIG_IDENT_STRING		"\nKeymile NUSA"
#define CONFIG_HOSTNAME			kmnusa
#undef CONFIG_SYS_KWD_CONFIG
#define CONFIG_SYS_KWD_CONFIG \
		$(SRCTREE)/$(CONFIG_BOARDDIR)/kwbimage_128M16_1.cfg
#define CONFIG_KM_ENV_IS_IN_SPI_NOR
#define CONFIG_KM_FPGA_CONFIG
#define CONFIG_KM_PIGGY4_88E6352

/* KM_MGCOGE3UN */
#elif defined(CONFIG_KM_MGCOGE3UN)
#define CONFIG_IDENT_STRING		"\nKeymile COGE3UN"
#define CONFIG_HOSTNAME			mgcoge3un
#define KM_IVM_BUS			"pca9547:70:9" /* I2C2 (Mux-Port 1)*/
#undef CONFIG_SYS_KWD_CONFIG
#define CONFIG_SYS_KWD_CONFIG \
		$(SRCTREE)/$(CONFIG_BOARDDIR)/kwbimage-memphis.cfg
#define CONFIG_KM_BOARD_EXTRA_ENV	"waitforne=true\0"
#define CONFIG_PIGGY_MAC_ADRESS_OFFSET  3
#define CONFIG_KM_DISABLE_PCIE
#define CONFIG_KM_PIGGY4_88E6061

/* KMCOGE5UN */
#elif defined(CONFIG_KM_COGE5UN)
#define CONFIG_IDENT_STRING		"\nKeymile COGE5UN"
#define KM_IVM_BUS			"pca9547:70:9"	/* I2C2 (Mux-Port 1)*/
#undef	CONFIG_SYS_KWD_CONFIG
#define CONFIG_SYS_KWD_CONFIG \
		$(SRCTREE)/$(CONFIG_BOARDDIR)/kwbimage_256M8_1.cfg
#define CONFIG_KM_ENV_IS_IN_SPI_NOR
#define CONFIG_PIGGY_MAC_ADRESS_OFFSET	3
#define CONFIG_HOSTNAME			kmcoge5un
#define CONFIG_KM_DISABLE_PCIE
#define CONFIG_KM_PIGGY4_88E6352

/* KM_PORTL2 */
#elif defined(CONFIG_KM_PORTL2)
#define CONFIG_IDENT_STRING		"\nKeymile Port-L2"
#define CONFIG_HOSTNAME			portl2
#define KM_IVM_BUS			"pca9544a:70:9" /* I2C2 (Mux-Port 1)*/
#define CONFIG_KM_PIGGY4_88E6061

#else
#error ("Board unsupported")
#endif

/* include common defines/options for all arm based Keymile boards */
#include "km/km_arm.h"

#ifndef CONFIG_KM_ENV_IS_IN_SPI_NOR
#define KM_ENV_BUS	"pca9544a:70:d"	/* I2C2 (Mux-Port 5)*/
#endif

#if defined(CONFIG_KM_PIGGY4_88E6352)
/*
 * Some keymile boards like mgcoge5un & nusa1 have their PIGGY4 connected via
 * an Marvell 88E6352 simple switch.
 * In this case we have to change the default settings for the etherent mac.
 * There is NO ethernet phy. The ARM and Switch are conencted directly over
 * RGMII in MAC-MAC mode
 * In this case 1GBit full duplex and autoneg off
 */
#define PORT_SERIAL_CONTROL_VALUE		( \
	MVGBE_FORCE_LINK_PASS			    | \
	MVGBE_DIS_AUTO_NEG_FOR_DUPLX		| \
	MVGBE_DIS_AUTO_NEG_FOR_FLOW_CTRL	| \
	MVGBE_ADV_NO_FLOW_CTRL			    | \
	MVGBE_FORCE_FC_MODE_NO_PAUSE_DIS_TX	| \
	MVGBE_FORCE_BP_MODE_NO_JAM		    | \
	(1 << 9) /* Reserved bit has to be 1 */	| \
	MVGBE_DO_NOT_FORCE_LINK_FAIL		| \
	MVGBE_DIS_AUTO_NEG_SPEED_GMII		| \
	MVGBE_DTE_ADV_0				        | \
	MVGBE_MIIPHY_MAC_MODE			    | \
	MVGBE_AUTO_NEG_NO_CHANGE		    | \
	MVGBE_MAX_RX_PACKET_1552BYTE		| \
	MVGBE_CLR_EXT_LOOPBACK			    | \
	MVGBE_SET_FULL_DUPLEX_MODE		    | \
	MVGBE_EN_FLOW_CTRL_TX_RX_IN_FULL_DUPLEX	|\
	MVGBE_SET_GMII_SPEED_TO_1000	    |\
	MVGBE_SET_MII_SPEED_TO_100)

#endif

#ifdef CONFIG_KM_PIGGY4_88E6061
/*
 * Some keymile boards like mgcoge3un have their PIGGY4 connected via
 * an Marvell 88E6061 simple switch.
 * In this case we have to change the default settings for the
 * ethernet phy connected to the kirkwood.
 * In this case 100MB full duplex and autoneg off
 */
#define PORT_SERIAL_CONTROL_VALUE		( \
	MVGBE_FORCE_LINK_PASS			| \
	MVGBE_DIS_AUTO_NEG_FOR_DUPLX		| \
	MVGBE_DIS_AUTO_NEG_FOR_FLOW_CTRL	| \
	MVGBE_ADV_NO_FLOW_CTRL			| \
	MVGBE_FORCE_FC_MODE_NO_PAUSE_DIS_TX	| \
	MVGBE_FORCE_BP_MODE_NO_JAM		| \
	(1 << 9) /* Reserved bit has to be 1 */	| \
	MVGBE_DO_NOT_FORCE_LINK_FAIL		| \
	MVGBE_DIS_AUTO_NEG_SPEED_GMII		| \
	MVGBE_DTE_ADV_0				| \
	MVGBE_MIIPHY_MAC_MODE			| \
	MVGBE_AUTO_NEG_NO_CHANGE		| \
	MVGBE_MAX_RX_PACKET_1552BYTE		| \
	MVGBE_CLR_EXT_LOOPBACK			| \
	MVGBE_SET_FULL_DUPLEX_MODE		| \
	MVGBE_DIS_FLOW_CTRL_TX_RX_IN_FULL_DUPLEX	|\
	MVGBE_SET_GMII_SPEED_TO_10_100	|\
	MVGBE_SET_MII_SPEED_TO_100)
#endif

#ifdef CONFIG_KM_DISABLE_PCI
#undef  CONFIG_KIRKWOOD_PCIE_INIT
#endif


#endif /* _CONFIG_KM_KIRKWOOD */
