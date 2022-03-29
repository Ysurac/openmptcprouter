/*
 * Pinmux configurations for the DA850 SoCs
 *
 * Copyright (C) 2011 OMICRON electronics GmbH
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

#include <common.h>
#include <asm/arch/davinci_misc.h>
#include <asm/arch/hardware.h>
#include <asm/arch/pinmux_defs.h>

/* SPI pin muxer settings */
const struct pinmux_config spi1_pins_base[] = {
	{ pinmux(5), 1, 2 }, /* SPI1_CLK */
	{ pinmux(5), 1, 4 }, /* SPI1_SOMI */
	{ pinmux(5), 1, 5 }, /* SPI1_SIMO */
};

const struct pinmux_config spi1_pins_scs0[] = {
	{ pinmux(5), 1, 1 }, /* SPI1_SCS[0] */
};

/* UART pin muxer settings */
const struct pinmux_config uart1_pins_txrx[] = {
	{ pinmux(4), 2, 6 }, /* UART1_RXD */
	{ pinmux(4), 2, 7 }, /* UART1_TXD */
};

const struct pinmux_config uart2_pins_txrx[] = {
	{ pinmux(4), 2, 4 }, /* UART2_RXD */
	{ pinmux(4), 2, 5 }, /* UART2_TXD */
};

const struct pinmux_config uart2_pins_rtscts[] = {
	{ pinmux(0), 4, 6 }, /* UART2_RTS */
	{ pinmux(0), 4, 7 }, /* UART2_CTS */
};

/* EMAC pin muxer settings*/
const struct pinmux_config emac_pins_rmii[] = {
	{ pinmux(14), 8, 2 }, /* RMII_TXD[1] */
	{ pinmux(14), 8, 3 }, /* RMII_TXD[0] */
	{ pinmux(14), 8, 4 }, /* RMII_TXEN */
	{ pinmux(14), 8, 5 }, /* RMII_RXD[1] */
	{ pinmux(14), 8, 6 }, /* RMII_RXD[0] */
	{ pinmux(14), 8, 7 }, /* RMII_RXER */
	{ pinmux(15), 8, 1 }, /* RMII_CRS_DV */
};

const struct pinmux_config emac_pins_mii[] = {
	{ pinmux(2), 8, 1 }, /* MII_TXEN */
	{ pinmux(2), 8, 2 }, /* MII_TXCLK */
	{ pinmux(2), 8, 3 }, /* MII_COL */
	{ pinmux(2), 8, 4 }, /* MII_TXD[3] */
	{ pinmux(2), 8, 5 }, /* MII_TXD[2] */
	{ pinmux(2), 8, 6 }, /* MII_TXD[1] */
	{ pinmux(2), 8, 7 }, /* MII_TXD[0] */
	{ pinmux(3), 8, 0 }, /* MII_RXCLK */
	{ pinmux(3), 8, 1 }, /* MII_RXDV */
	{ pinmux(3), 8, 2 }, /* MII_RXER */
	{ pinmux(3), 8, 3 }, /* MII_CRS */
	{ pinmux(3), 8, 4 }, /* MII_RXD[3] */
	{ pinmux(3), 8, 5 }, /* MII_RXD[2] */
	{ pinmux(3), 8, 6 }, /* MII_RXD[1] */
	{ pinmux(3), 8, 7 }, /* MII_RXD[0] */
};

const struct pinmux_config emac_pins_mdio[] = {
	{ pinmux(4), 8, 0 }, /* MDIO_CLK */
	{ pinmux(4), 8, 1 }, /* MDIO_D */
};

/* I2C pin muxer settings */
const struct pinmux_config i2c0_pins[] = {
	{ pinmux(4), 2, 2 }, /* I2C0_SCL */
	{ pinmux(4), 2, 3 }, /* I2C0_SDA */
};

const struct pinmux_config i2c1_pins[] = {
	{ pinmux(4), 4, 4 }, /* I2C1_SCL */
	{ pinmux(4), 4, 5 }, /* I2C1_SDA */
};

/* EMIFA pin muxer settings */
const struct pinmux_config emifa_pins_cs2[] = {
	{ pinmux(7), 1, 0 }, /* EMA_CS2 */
};

const struct pinmux_config emifa_pins_cs3[] = {
	{ pinmux(7), 1, 1 }, /* EMA_CS[3] */
};

const struct pinmux_config emifa_pins_cs4[] = {
	{ pinmux(7), 1, 2 }, /* EMA_CS[4] */
};

const struct pinmux_config emifa_pins_nand[] = {
	{ pinmux(7), 1, 4 },  /* EMA_WE */
	{ pinmux(7), 1, 5 },  /* EMA_OE */
	{ pinmux(9), 1, 0 },  /* EMA_D[7] */
	{ pinmux(9), 1, 1 },  /* EMA_D[6] */
	{ pinmux(9), 1, 2 },  /* EMA_D[5] */
	{ pinmux(9), 1, 3 },  /* EMA_D[4] */
	{ pinmux(9), 1, 4 },  /* EMA_D[3] */
	{ pinmux(9), 1, 5 },  /* EMA_D[2] */
	{ pinmux(9), 1, 6 },  /* EMA_D[1] */
	{ pinmux(9), 1, 7 },  /* EMA_D[0] */
	{ pinmux(12), 1, 5 }, /* EMA_A[2] */
	{ pinmux(12), 1, 6 }, /* EMA_A[1] */
};

/* NOR pin muxer settings */
const struct pinmux_config emifa_pins_nor[] = {
	{ pinmux(5), 1, 6 },  /* EMA_BA[1] */
	{ pinmux(6), 1, 6 },  /* EMA_WAIT[1] */
	{ pinmux(7), 1, 4 },  /* EMA_WE */
	{ pinmux(7), 1, 5 },  /* EMA_OE */
	{ pinmux(8), 1, 0 },  /* EMA_D[15] */
	{ pinmux(8), 1, 1 },  /* EMA_D[14] */
	{ pinmux(8), 1, 2 },  /* EMA_D[13] */
	{ pinmux(8), 1, 3 },  /* EMA_D[12] */
	{ pinmux(8), 1, 4 },  /* EMA_D[11] */
	{ pinmux(8), 1, 5 },  /* EMA_D[10] */
	{ pinmux(8), 1, 6 },  /* EMA_D[9] */
	{ pinmux(8), 1, 7 },  /* EMA_D[8] */
	{ pinmux(9), 1, 0 },  /* EMA_D[7] */
	{ pinmux(9), 1, 1 },  /* EMA_D[6] */
	{ pinmux(9), 1, 2 },  /* EMA_D[5] */
	{ pinmux(9), 1, 3 },  /* EMA_D[4] */
	{ pinmux(9), 1, 4 },  /* EMA_D[3] */
	{ pinmux(9), 1, 5 },  /* EMA_D[2] */
	{ pinmux(9), 1, 6 },  /* EMA_D[1] */
	{ pinmux(9), 1, 7 },  /* EMA_D[0] */
	{ pinmux(10), 1, 1 }, /* EMA_A[22] */
	{ pinmux(10), 1, 2 }, /* EMA_A[21] */
	{ pinmux(10), 1, 3 }, /* EMA_A[20] */
	{ pinmux(10), 1, 4 }, /* EMA_A[19] */
	{ pinmux(10), 1, 5 }, /* EMA_A[18] */
	{ pinmux(10), 1, 6 }, /* EMA_A[17] */
	{ pinmux(10), 1, 7 }, /* EMA_A[16] */
	{ pinmux(11), 1, 0 }, /* EMA_A[15] */
	{ pinmux(11), 1, 1 }, /* EMA_A[14] */
	{ pinmux(11), 1, 2 }, /* EMA_A[13] */
	{ pinmux(11), 1, 3 }, /* EMA_A[12] */
	{ pinmux(11), 1, 4 }, /* EMA_A[11] */
	{ pinmux(11), 1, 5 }, /* EMA_A[10] */
	{ pinmux(11), 1, 6 }, /* EMA_A[9] */
	{ pinmux(11), 1, 7 }, /* EMA_A[8] */
	{ pinmux(12), 1, 0 }, /* EMA_A[7] */
	{ pinmux(12), 1, 1 }, /* EMA_A[6] */
	{ pinmux(12), 1, 2 }, /* EMA_A[5] */
	{ pinmux(12), 1, 3 }, /* EMA_A[4] */
	{ pinmux(12), 1, 4 }, /* EMA_A[3] */
	{ pinmux(12), 1, 5 }, /* EMA_A[2] */
	{ pinmux(12), 1, 6 }, /* EMA_A[1] */
	{ pinmux(12), 1, 7 }, /* EMA_A[0] */
};
