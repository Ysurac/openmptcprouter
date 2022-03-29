/*
 *
 * (c) 2007 Pengutronix, Sascha Hauer <s.hauer@pengutronix.de>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __ASM_ARCH_MX31_IMX_REGS_H
#define __ASM_ARCH_MX31_IMX_REGS_H

#if !(defined(__KERNEL_STRICT_NAMES) || defined(__ASSEMBLY__))
#include <asm/types.h>

/* Clock control module registers */
struct clock_control_regs {
	u32 ccmr;
	u32 pdr0;
	u32 pdr1;
	u32 rcsr;
	u32 mpctl;
	u32 upctl;
	u32 spctl;
	u32 cosr;
	u32 cgr0;
	u32 cgr1;
	u32 cgr2;
	u32 wimr0;
	u32 ldc;
	u32 dcvr0;
	u32 dcvr1;
	u32 dcvr2;
	u32 dcvr3;
	u32 ltr0;
	u32 ltr1;
	u32 ltr2;
	u32 ltr3;
	u32 ltbr0;
	u32 ltbr1;
	u32 pmcr0;
	u32 pmcr1;
	u32 pdr2;
};

struct cspi_regs {
	u32 rxdata;
	u32 txdata;
	u32 ctrl;
	u32 intr;
	u32 dma;
	u32 stat;
	u32 period;
	u32 test;
};

/* Watchdog Timer (WDOG) registers */
#define WDOG_ENABLE	(1 << 2)
#define WDOG_WT_SHIFT	8
#define WDOG_WDZST	(1 << 0)

struct wdog_regs {
	u16 wcr;	/* Control */
	u16 wsr;	/* Service */
	u16 wrsr;	/* Reset Status */
};

/* IIM Control Registers */
struct iim_regs {
	u32 iim_stat;
	u32 iim_statm;
	u32 iim_err;
	u32 iim_emask;
	u32 iim_fctl;
	u32 iim_ua;
	u32 iim_la;
	u32 iim_sdat;
	u32 iim_prev;
	u32 iim_srev;
	u32 iim_prog_p;
	u32 iim_scs0;
	u32 iim_scs1;
	u32 iim_scs2;
	u32 iim_scs3;
};

struct iomuxc_regs {
	u32 unused1;
	u32 unused2;
	u32 gpr;
};

struct mx3_cpu_type {
	u8 srev;
	u32 v;
};

#define IOMUX_PADNUM_MASK	0x1ff
#define IOMUX_PIN(gpionum, padnum) ((padnum) & IOMUX_PADNUM_MASK)

/*
 * various IOMUX pad functions
 */
enum iomux_pad_config {
	PAD_CTL_NOLOOPBACK	= 0x0 << 9,
	PAD_CTL_LOOPBACK	= 0x1 << 9,
	PAD_CTL_PKE_NONE	= 0x0 << 8,
	PAD_CTL_PKE_ENABLE	= 0x1 << 8,
	PAD_CTL_PUE_KEEPER	= 0x0 << 7,
	PAD_CTL_PUE_PUD		= 0x1 << 7,
	PAD_CTL_100K_PD		= 0x0 << 5,
	PAD_CTL_100K_PU		= 0x1 << 5,
	PAD_CTL_47K_PU		= 0x2 << 5,
	PAD_CTL_22K_PU		= 0x3 << 5,
	PAD_CTL_HYS_CMOS	= 0x0 << 4,
	PAD_CTL_HYS_SCHMITZ	= 0x1 << 4,
	PAD_CTL_ODE_CMOS	= 0x0 << 3,
	PAD_CTL_ODE_OpenDrain	= 0x1 << 3,
	PAD_CTL_DRV_NORMAL	= 0x0 << 1,
	PAD_CTL_DRV_HIGH	= 0x1 << 1,
	PAD_CTL_DRV_MAX		= 0x2 << 1,
	PAD_CTL_SRE_SLOW	= 0x0 << 0,
	PAD_CTL_SRE_FAST	= 0x1 << 0
};

/*
 * This enumeration is constructed based on the Section
 * "sw_pad_ctl & sw_mux_ctl details" of the MX31 IC Spec. Each enumerated
 * value is constructed based on the rules described above.
 */

enum iomux_pins {
	MX31_PIN_TTM_PAD	= IOMUX_PIN(0xff,   0),
	MX31_PIN_CSPI3_SPI_RDY	= IOMUX_PIN(0xff,   1),
	MX31_PIN_CSPI3_SCLK	= IOMUX_PIN(0xff,   2),
	MX31_PIN_CSPI3_MISO	= IOMUX_PIN(0xff,   3),
	MX31_PIN_CSPI3_MOSI	= IOMUX_PIN(0xff,   4),
	MX31_PIN_CLKSS		= IOMUX_PIN(0xff,   5),
	MX31_PIN_CE_CONTROL	= IOMUX_PIN(0xff,   6),
	MX31_PIN_ATA_RESET_B	= IOMUX_PIN(95,     7),
	MX31_PIN_ATA_DMACK	= IOMUX_PIN(94,     8),
	MX31_PIN_ATA_DIOW	= IOMUX_PIN(93,     9),
	MX31_PIN_ATA_DIOR	= IOMUX_PIN(92,    10),
	MX31_PIN_ATA_CS1	= IOMUX_PIN(91,    11),
	MX31_PIN_ATA_CS0	= IOMUX_PIN(90,    12),
	MX31_PIN_SD1_DATA3	= IOMUX_PIN(63,    13),
	MX31_PIN_SD1_DATA2	= IOMUX_PIN(62,    14),
	MX31_PIN_SD1_DATA1	= IOMUX_PIN(61,    15),
	MX31_PIN_SD1_DATA0	= IOMUX_PIN(60,    16),
	MX31_PIN_SD1_CLK	= IOMUX_PIN(59,    17),
	MX31_PIN_SD1_CMD	= IOMUX_PIN(58,    18),
	MX31_PIN_D3_SPL		= IOMUX_PIN(0xff,  19),
	MX31_PIN_D3_CLS		= IOMUX_PIN(0xff,  20),
	MX31_PIN_D3_REV		= IOMUX_PIN(0xff,  21),
	MX31_PIN_CONTRAST	= IOMUX_PIN(0xff,  22),
	MX31_PIN_VSYNC3		= IOMUX_PIN(0xff,  23),
	MX31_PIN_READ		= IOMUX_PIN(0xff,  24),
	MX31_PIN_WRITE		= IOMUX_PIN(0xff,  25),
	MX31_PIN_PAR_RS		= IOMUX_PIN(0xff,  26),
	MX31_PIN_SER_RS		= IOMUX_PIN(89,    27),
	MX31_PIN_LCS1		= IOMUX_PIN(88,    28),
	MX31_PIN_LCS0		= IOMUX_PIN(87,    29),
	MX31_PIN_SD_D_CLK	= IOMUX_PIN(86,    30),
	MX31_PIN_SD_D_IO	= IOMUX_PIN(85,    31),
	MX31_PIN_SD_D_I		= IOMUX_PIN(84,    32),
	MX31_PIN_DRDY0		= IOMUX_PIN(0xff,  33),
	MX31_PIN_FPSHIFT	= IOMUX_PIN(0xff,  34),
	MX31_PIN_HSYNC		= IOMUX_PIN(0xff,  35),
	MX31_PIN_VSYNC0		= IOMUX_PIN(0xff,  36),
	MX31_PIN_LD17		= IOMUX_PIN(0xff,  37),
	MX31_PIN_LD16		= IOMUX_PIN(0xff,  38),
	MX31_PIN_LD15		= IOMUX_PIN(0xff,  39),
	MX31_PIN_LD14		= IOMUX_PIN(0xff,  40),
	MX31_PIN_LD13		= IOMUX_PIN(0xff,  41),
	MX31_PIN_LD12		= IOMUX_PIN(0xff,  42),
	MX31_PIN_LD11		= IOMUX_PIN(0xff,  43),
	MX31_PIN_LD10		= IOMUX_PIN(0xff,  44),
	MX31_PIN_LD9		= IOMUX_PIN(0xff,  45),
	MX31_PIN_LD8		= IOMUX_PIN(0xff,  46),
	MX31_PIN_LD7		= IOMUX_PIN(0xff,  47),
	MX31_PIN_LD6		= IOMUX_PIN(0xff,  48),
	MX31_PIN_LD5		= IOMUX_PIN(0xff,  49),
	MX31_PIN_LD4		= IOMUX_PIN(0xff,  50),
	MX31_PIN_LD3		= IOMUX_PIN(0xff,  51),
	MX31_PIN_LD2		= IOMUX_PIN(0xff,  52),
	MX31_PIN_LD1		= IOMUX_PIN(0xff,  53),
	MX31_PIN_LD0		= IOMUX_PIN(0xff,  54),
	MX31_PIN_USBH2_DATA1	= IOMUX_PIN(0xff,  55),
	MX31_PIN_USBH2_DATA0	= IOMUX_PIN(0xff,  56),
	MX31_PIN_USBH2_NXT	= IOMUX_PIN(0xff,  57),
	MX31_PIN_USBH2_STP	= IOMUX_PIN(0xff,  58),
	MX31_PIN_USBH2_DIR	= IOMUX_PIN(0xff,  59),
	MX31_PIN_USBH2_CLK	= IOMUX_PIN(0xff,  60),
	MX31_PIN_USBOTG_DATA7	= IOMUX_PIN(0xff,  61),
	MX31_PIN_USBOTG_DATA6	= IOMUX_PIN(0xff,  62),
	MX31_PIN_USBOTG_DATA5	= IOMUX_PIN(0xff,  63),
	MX31_PIN_USBOTG_DATA4	= IOMUX_PIN(0xff,  64),
	MX31_PIN_USBOTG_DATA3	= IOMUX_PIN(0xff,  65),
	MX31_PIN_USBOTG_DATA2	= IOMUX_PIN(0xff,  66),
	MX31_PIN_USBOTG_DATA1	= IOMUX_PIN(0xff,  67),
	MX31_PIN_USBOTG_DATA0	= IOMUX_PIN(0xff,  68),
	MX31_PIN_USBOTG_NXT	= IOMUX_PIN(0xff,  69),
	MX31_PIN_USBOTG_STP	= IOMUX_PIN(0xff,  70),
	MX31_PIN_USBOTG_DIR	= IOMUX_PIN(0xff,  71),
	MX31_PIN_USBOTG_CLK	= IOMUX_PIN(0xff,  72),
	MX31_PIN_USB_BYP	= IOMUX_PIN(31,    73),
	MX31_PIN_USB_OC		= IOMUX_PIN(30,    74),
	MX31_PIN_USB_PWR	= IOMUX_PIN(29,    75),
	MX31_PIN_SJC_MOD	= IOMUX_PIN(0xff,  76),
	MX31_PIN_DE_B		= IOMUX_PIN(0xff,  77),
	MX31_PIN_TRSTB		= IOMUX_PIN(0xff,  78),
	MX31_PIN_TDO		= IOMUX_PIN(0xff,  79),
	MX31_PIN_TDI		= IOMUX_PIN(0xff,  80),
	MX31_PIN_TMS		= IOMUX_PIN(0xff,  81),
	MX31_PIN_TCK		= IOMUX_PIN(0xff,  82),
	MX31_PIN_RTCK		= IOMUX_PIN(0xff,  83),
	MX31_PIN_KEY_COL7	= IOMUX_PIN(57,    84),
	MX31_PIN_KEY_COL6	= IOMUX_PIN(56,    85),
	MX31_PIN_KEY_COL5	= IOMUX_PIN(55,    86),
	MX31_PIN_KEY_COL4	= IOMUX_PIN(54,    87),
	MX31_PIN_KEY_COL3	= IOMUX_PIN(0xff,  88),
	MX31_PIN_KEY_COL2	= IOMUX_PIN(0xff,  89),
	MX31_PIN_KEY_COL1	= IOMUX_PIN(0xff,  90),
	MX31_PIN_KEY_COL0	= IOMUX_PIN(0xff,  91),
	MX31_PIN_KEY_ROW7	= IOMUX_PIN(53,    92),
	MX31_PIN_KEY_ROW6	= IOMUX_PIN(52,    93),
	MX31_PIN_KEY_ROW5	= IOMUX_PIN(51,    94),
	MX31_PIN_KEY_ROW4	= IOMUX_PIN(50,    95),
	MX31_PIN_KEY_ROW3	= IOMUX_PIN(0xff,  96),
	MX31_PIN_KEY_ROW2	= IOMUX_PIN(0xff,  97),
	MX31_PIN_KEY_ROW1	= IOMUX_PIN(0xff,  98),
	MX31_PIN_KEY_ROW0	= IOMUX_PIN(0xff,  99),
	MX31_PIN_BATT_LINE	= IOMUX_PIN(49,   100),
	MX31_PIN_CTS2		= IOMUX_PIN(0xff, 101),
	MX31_PIN_RTS2		= IOMUX_PIN(0xff, 102),
	MX31_PIN_TXD2		= IOMUX_PIN(28,   103),
	MX31_PIN_RXD2		= IOMUX_PIN(27,   104),
	MX31_PIN_DTR_DCE2	= IOMUX_PIN(48,   105),
	MX31_PIN_DCD_DTE1	= IOMUX_PIN(47,   106),
	MX31_PIN_RI_DTE1	= IOMUX_PIN(46,   107),
	MX31_PIN_DSR_DTE1	= IOMUX_PIN(45,   108),
	MX31_PIN_DTR_DTE1	= IOMUX_PIN(44,   109),
	MX31_PIN_DCD_DCE1	= IOMUX_PIN(43,   110),
	MX31_PIN_RI_DCE1	= IOMUX_PIN(42,   111),
	MX31_PIN_DSR_DCE1	= IOMUX_PIN(41,   112),
	MX31_PIN_DTR_DCE1	= IOMUX_PIN(40,   113),
	MX31_PIN_CTS1		= IOMUX_PIN(39,   114),
	MX31_PIN_RTS1		= IOMUX_PIN(38,   115),
	MX31_PIN_TXD1		= IOMUX_PIN(37,   116),
	MX31_PIN_RXD1		= IOMUX_PIN(36,   117),
	MX31_PIN_CSPI2_SPI_RDY	= IOMUX_PIN(0xff, 118),
	MX31_PIN_CSPI2_SCLK	= IOMUX_PIN(0xff, 119),
	MX31_PIN_CSPI2_SS2	= IOMUX_PIN(0xff, 120),
	MX31_PIN_CSPI2_SS1	= IOMUX_PIN(0xff, 121),
	MX31_PIN_CSPI2_SS0	= IOMUX_PIN(0xff, 122),
	MX31_PIN_CSPI2_MISO	= IOMUX_PIN(0xff, 123),
	MX31_PIN_CSPI2_MOSI	= IOMUX_PIN(0xff, 124),
	MX31_PIN_CSPI1_SPI_RDY	= IOMUX_PIN(0xff, 125),
	MX31_PIN_CSPI1_SCLK	= IOMUX_PIN(0xff, 126),
	MX31_PIN_CSPI1_SS2	= IOMUX_PIN(0xff, 127),
	MX31_PIN_CSPI1_SS1	= IOMUX_PIN(0xff, 128),
	MX31_PIN_CSPI1_SS0	= IOMUX_PIN(0xff, 129),
	MX31_PIN_CSPI1_MISO	= IOMUX_PIN(0xff, 130),
	MX31_PIN_CSPI1_MOSI	= IOMUX_PIN(0xff, 131),
	MX31_PIN_SFS6		= IOMUX_PIN(26,   132),
	MX31_PIN_SCK6		= IOMUX_PIN(25,   133),
	MX31_PIN_SRXD6		= IOMUX_PIN(24,   134),
	MX31_PIN_STXD6		= IOMUX_PIN(23,   135),
	MX31_PIN_SFS5		= IOMUX_PIN(0xff, 136),
	MX31_PIN_SCK5		= IOMUX_PIN(0xff, 137),
	MX31_PIN_SRXD5		= IOMUX_PIN(22,   138),
	MX31_PIN_STXD5		= IOMUX_PIN(21,   139),
	MX31_PIN_SFS4		= IOMUX_PIN(0xff, 140),
	MX31_PIN_SCK4		= IOMUX_PIN(0xff, 141),
	MX31_PIN_SRXD4		= IOMUX_PIN(20,   142),
	MX31_PIN_STXD4		= IOMUX_PIN(19,   143),
	MX31_PIN_SFS3		= IOMUX_PIN(0xff, 144),
	MX31_PIN_SCK3		= IOMUX_PIN(0xff, 145),
	MX31_PIN_SRXD3		= IOMUX_PIN(18,   146),
	MX31_PIN_STXD3		= IOMUX_PIN(17,   147),
	MX31_PIN_I2C_DAT	= IOMUX_PIN(0xff, 148),
	MX31_PIN_I2C_CLK	= IOMUX_PIN(0xff, 149),
	MX31_PIN_CSI_PIXCLK	= IOMUX_PIN(83,   150),
	MX31_PIN_CSI_HSYNC	= IOMUX_PIN(82,   151),
	MX31_PIN_CSI_VSYNC	= IOMUX_PIN(81,   152),
	MX31_PIN_CSI_MCLK	= IOMUX_PIN(80,   153),
	MX31_PIN_CSI_D15	= IOMUX_PIN(79,   154),
	MX31_PIN_CSI_D14	= IOMUX_PIN(78,   155),
	MX31_PIN_CSI_D13	= IOMUX_PIN(77,   156),
	MX31_PIN_CSI_D12	= IOMUX_PIN(76,   157),
	MX31_PIN_CSI_D11	= IOMUX_PIN(75,   158),
	MX31_PIN_CSI_D10	= IOMUX_PIN(74,   159),
	MX31_PIN_CSI_D9		= IOMUX_PIN(73,   160),
	MX31_PIN_CSI_D8		= IOMUX_PIN(72,   161),
	MX31_PIN_CSI_D7		= IOMUX_PIN(71,   162),
	MX31_PIN_CSI_D6		= IOMUX_PIN(70,   163),
	MX31_PIN_CSI_D5		= IOMUX_PIN(69,   164),
	MX31_PIN_CSI_D4		= IOMUX_PIN(68,   165),
	MX31_PIN_M_GRANT	= IOMUX_PIN(0xff, 166),
	MX31_PIN_M_REQUEST	= IOMUX_PIN(0xff, 167),
	MX31_PIN_PC_POE		= IOMUX_PIN(0xff, 168),
	MX31_PIN_PC_RW_B	= IOMUX_PIN(0xff, 169),
	MX31_PIN_IOIS16		= IOMUX_PIN(0xff, 170),
	MX31_PIN_PC_RST		= IOMUX_PIN(0xff, 171),
	MX31_PIN_PC_BVD2	= IOMUX_PIN(0xff, 172),
	MX31_PIN_PC_BVD1	= IOMUX_PIN(0xff, 173),
	MX31_PIN_PC_VS2		= IOMUX_PIN(0xff, 174),
	MX31_PIN_PC_VS1		= IOMUX_PIN(0xff, 175),
	MX31_PIN_PC_PWRON	= IOMUX_PIN(0xff, 176),
	MX31_PIN_PC_READY	= IOMUX_PIN(0xff, 177),
	MX31_PIN_PC_WAIT_B	= IOMUX_PIN(0xff, 178),
	MX31_PIN_PC_CD2_B	= IOMUX_PIN(0xff, 179),
	MX31_PIN_PC_CD1_B	= IOMUX_PIN(0xff, 180),
	MX31_PIN_D0		= IOMUX_PIN(0xff, 181),
	MX31_PIN_D1		= IOMUX_PIN(0xff, 182),
	MX31_PIN_D2		= IOMUX_PIN(0xff, 183),
	MX31_PIN_D3		= IOMUX_PIN(0xff, 184),
	MX31_PIN_D4		= IOMUX_PIN(0xff, 185),
	MX31_PIN_D5		= IOMUX_PIN(0xff, 186),
	MX31_PIN_D6		= IOMUX_PIN(0xff, 187),
	MX31_PIN_D7		= IOMUX_PIN(0xff, 188),
	MX31_PIN_D8		= IOMUX_PIN(0xff, 189),
	MX31_PIN_D9		= IOMUX_PIN(0xff, 190),
	MX31_PIN_D10		= IOMUX_PIN(0xff, 191),
	MX31_PIN_D11		= IOMUX_PIN(0xff, 192),
	MX31_PIN_D12		= IOMUX_PIN(0xff, 193),
	MX31_PIN_D13		= IOMUX_PIN(0xff, 194),
	MX31_PIN_D14		= IOMUX_PIN(0xff, 195),
	MX31_PIN_D15		= IOMUX_PIN(0xff, 196),
	MX31_PIN_NFRB		= IOMUX_PIN(16,   197),
	MX31_PIN_NFCE_B		= IOMUX_PIN(15,   198),
	MX31_PIN_NFWP_B		= IOMUX_PIN(14,   199),
	MX31_PIN_NFCLE		= IOMUX_PIN(13,   200),
	MX31_PIN_NFALE		= IOMUX_PIN(12,   201),
	MX31_PIN_NFRE_B		= IOMUX_PIN(11,   202),
	MX31_PIN_NFWE_B		= IOMUX_PIN(10,   203),
	MX31_PIN_SDQS3		= IOMUX_PIN(0xff, 204),
	MX31_PIN_SDQS2		= IOMUX_PIN(0xff, 205),
	MX31_PIN_SDQS1		= IOMUX_PIN(0xff, 206),
	MX31_PIN_SDQS0		= IOMUX_PIN(0xff, 207),
	MX31_PIN_SDCLK_B	= IOMUX_PIN(0xff, 208),
	MX31_PIN_SDCLK		= IOMUX_PIN(0xff, 209),
	MX31_PIN_SDCKE1		= IOMUX_PIN(0xff, 210),
	MX31_PIN_SDCKE0		= IOMUX_PIN(0xff, 211),
	MX31_PIN_SDWE		= IOMUX_PIN(0xff, 212),
	MX31_PIN_CAS		= IOMUX_PIN(0xff, 213),
	MX31_PIN_RAS		= IOMUX_PIN(0xff, 214),
	MX31_PIN_RW		= IOMUX_PIN(0xff, 215),
	MX31_PIN_BCLK		= IOMUX_PIN(0xff, 216),
	MX31_PIN_LBA		= IOMUX_PIN(0xff, 217),
	MX31_PIN_ECB		= IOMUX_PIN(0xff, 218),
	MX31_PIN_CS5		= IOMUX_PIN(0xff, 219),
	MX31_PIN_CS4		= IOMUX_PIN(0xff, 220),
	MX31_PIN_CS3		= IOMUX_PIN(0xff, 221),
	MX31_PIN_CS2		= IOMUX_PIN(0xff, 222),
	MX31_PIN_CS1		= IOMUX_PIN(0xff, 223),
	MX31_PIN_CS0		= IOMUX_PIN(0xff, 224),
	MX31_PIN_OE		= IOMUX_PIN(0xff, 225),
	MX31_PIN_EB1		= IOMUX_PIN(0xff, 226),
	MX31_PIN_EB0		= IOMUX_PIN(0xff, 227),
	MX31_PIN_DQM3		= IOMUX_PIN(0xff, 228),
	MX31_PIN_DQM2		= IOMUX_PIN(0xff, 229),
	MX31_PIN_DQM1		= IOMUX_PIN(0xff, 230),
	MX31_PIN_DQM0		= IOMUX_PIN(0xff, 231),
	MX31_PIN_SD31		= IOMUX_PIN(0xff, 232),
	MX31_PIN_SD30		= IOMUX_PIN(0xff, 233),
	MX31_PIN_SD29		= IOMUX_PIN(0xff, 234),
	MX31_PIN_SD28		= IOMUX_PIN(0xff, 235),
	MX31_PIN_SD27		= IOMUX_PIN(0xff, 236),
	MX31_PIN_SD26		= IOMUX_PIN(0xff, 237),
	MX31_PIN_SD25		= IOMUX_PIN(0xff, 238),
	MX31_PIN_SD24		= IOMUX_PIN(0xff, 239),
	MX31_PIN_SD23		= IOMUX_PIN(0xff, 240),
	MX31_PIN_SD22		= IOMUX_PIN(0xff, 241),
	MX31_PIN_SD21		= IOMUX_PIN(0xff, 242),
	MX31_PIN_SD20		= IOMUX_PIN(0xff, 243),
	MX31_PIN_SD19		= IOMUX_PIN(0xff, 244),
	MX31_PIN_SD18		= IOMUX_PIN(0xff, 245),
	MX31_PIN_SD17		= IOMUX_PIN(0xff, 246),
	MX31_PIN_SD16		= IOMUX_PIN(0xff, 247),
	MX31_PIN_SD15		= IOMUX_PIN(0xff, 248),
	MX31_PIN_SD14		= IOMUX_PIN(0xff, 249),
	MX31_PIN_SD13		= IOMUX_PIN(0xff, 250),
	MX31_PIN_SD12		= IOMUX_PIN(0xff, 251),
	MX31_PIN_SD11		= IOMUX_PIN(0xff, 252),
	MX31_PIN_SD10		= IOMUX_PIN(0xff, 253),
	MX31_PIN_SD9		= IOMUX_PIN(0xff, 254),
	MX31_PIN_SD8		= IOMUX_PIN(0xff, 255),
	MX31_PIN_SD7		= IOMUX_PIN(0xff, 256),
	MX31_PIN_SD6		= IOMUX_PIN(0xff, 257),
	MX31_PIN_SD5		= IOMUX_PIN(0xff, 258),
	MX31_PIN_SD4		= IOMUX_PIN(0xff, 259),
	MX31_PIN_SD3		= IOMUX_PIN(0xff, 260),
	MX31_PIN_SD2		= IOMUX_PIN(0xff, 261),
	MX31_PIN_SD1		= IOMUX_PIN(0xff, 262),
	MX31_PIN_SD0		= IOMUX_PIN(0xff, 263),
	MX31_PIN_SDBA0		= IOMUX_PIN(0xff, 264),
	MX31_PIN_SDBA1		= IOMUX_PIN(0xff, 265),
	MX31_PIN_A25		= IOMUX_PIN(0xff, 266),
	MX31_PIN_A24		= IOMUX_PIN(0xff, 267),
	MX31_PIN_A23		= IOMUX_PIN(0xff, 268),
	MX31_PIN_A22		= IOMUX_PIN(0xff, 269),
	MX31_PIN_A21		= IOMUX_PIN(0xff, 270),
	MX31_PIN_A20		= IOMUX_PIN(0xff, 271),
	MX31_PIN_A19		= IOMUX_PIN(0xff, 272),
	MX31_PIN_A18		= IOMUX_PIN(0xff, 273),
	MX31_PIN_A17		= IOMUX_PIN(0xff, 274),
	MX31_PIN_A16		= IOMUX_PIN(0xff, 275),
	MX31_PIN_A14		= IOMUX_PIN(0xff, 276),
	MX31_PIN_A15		= IOMUX_PIN(0xff, 277),
	MX31_PIN_A13		= IOMUX_PIN(0xff, 278),
	MX31_PIN_A12		= IOMUX_PIN(0xff, 279),
	MX31_PIN_A11		= IOMUX_PIN(0xff, 280),
	MX31_PIN_MA10		= IOMUX_PIN(0xff, 281),
	MX31_PIN_A10		= IOMUX_PIN(0xff, 282),
	MX31_PIN_A9		= IOMUX_PIN(0xff, 283),
	MX31_PIN_A8		= IOMUX_PIN(0xff, 284),
	MX31_PIN_A7		= IOMUX_PIN(0xff, 285),
	MX31_PIN_A6		= IOMUX_PIN(0xff, 286),
	MX31_PIN_A5		= IOMUX_PIN(0xff, 287),
	MX31_PIN_A4		= IOMUX_PIN(0xff, 288),
	MX31_PIN_A3		= IOMUX_PIN(0xff, 289),
	MX31_PIN_A2		= IOMUX_PIN(0xff, 290),
	MX31_PIN_A1		= IOMUX_PIN(0xff, 291),
	MX31_PIN_A0		= IOMUX_PIN(0xff, 292),
	MX31_PIN_VPG1		= IOMUX_PIN(0xff, 293),
	MX31_PIN_VPG0		= IOMUX_PIN(0xff, 294),
	MX31_PIN_DVFS1		= IOMUX_PIN(0xff, 295),
	MX31_PIN_DVFS0		= IOMUX_PIN(0xff, 296),
	MX31_PIN_VSTBY		= IOMUX_PIN(0xff, 297),
	MX31_PIN_POWER_FAIL	= IOMUX_PIN(0xff, 298),
	MX31_PIN_CKIL		= IOMUX_PIN(0xff, 299),
	MX31_PIN_BOOT_MODE4	= IOMUX_PIN(0xff, 300),
	MX31_PIN_BOOT_MODE3	= IOMUX_PIN(0xff, 301),
	MX31_PIN_BOOT_MODE2	= IOMUX_PIN(0xff, 302),
	MX31_PIN_BOOT_MODE1	= IOMUX_PIN(0xff, 303),
	MX31_PIN_BOOT_MODE0	= IOMUX_PIN(0xff, 304),
	MX31_PIN_CLKO		= IOMUX_PIN(0xff, 305),
	MX31_PIN_POR_B		= IOMUX_PIN(0xff, 306),
	MX31_PIN_RESET_IN_B	= IOMUX_PIN(0xff, 307),
	MX31_PIN_CKIH		= IOMUX_PIN(0xff, 308),
	MX31_PIN_SIMPD0		= IOMUX_PIN(35,   309),
	MX31_PIN_SRX0		= IOMUX_PIN(34,   310),
	MX31_PIN_STX0		= IOMUX_PIN(33,   311),
	MX31_PIN_SVEN0		= IOMUX_PIN(32,   312),
	MX31_PIN_SRST0		= IOMUX_PIN(67,   313),
	MX31_PIN_SCLK0		= IOMUX_PIN(66,   314),
	MX31_PIN_GPIO3_1	= IOMUX_PIN(65,   315),
	MX31_PIN_GPIO3_0	= IOMUX_PIN(64,   316),
	MX31_PIN_GPIO1_6	= IOMUX_PIN(6,    317),
	MX31_PIN_GPIO1_5	= IOMUX_PIN(5,    318),
	MX31_PIN_GPIO1_4	= IOMUX_PIN(4,    319),
	MX31_PIN_GPIO1_3	= IOMUX_PIN(3,    320),
	MX31_PIN_GPIO1_2	= IOMUX_PIN(2,    321),
	MX31_PIN_GPIO1_1	= IOMUX_PIN(1,    322),
	MX31_PIN_GPIO1_0	= IOMUX_PIN(0,    323),
	MX31_PIN_PWMO		= IOMUX_PIN(9,    324),
	MX31_PIN_WATCHDOG_RST	= IOMUX_PIN(0xff, 325),
	MX31_PIN_COMPARE	= IOMUX_PIN(8,    326),
	MX31_PIN_CAPTURE	= IOMUX_PIN(7,    327),
};

/*
 * various IOMUX general purpose functions
 */
enum iomux_gp_func {
	MUX_PGP_FIRI			= 1 << 0,
	MUX_DDR_MODE			= 1 << 1,
	MUX_PGP_CSPI_BB			= 1 << 2,
	MUX_PGP_ATA_1			= 1 << 3,
	MUX_PGP_ATA_2			= 1 << 4,
	MUX_PGP_ATA_3			= 1 << 5,
	MUX_PGP_ATA_4			= 1 << 6,
	MUX_PGP_ATA_5			= 1 << 7,
	MUX_PGP_ATA_6			= 1 << 8,
	MUX_PGP_ATA_7			= 1 << 9,
	MUX_PGP_ATA_8			= 1 << 10,
	MUX_PGP_UH2			= 1 << 11,
	MUX_SDCTL_CSD0_SEL		= 1 << 12,
	MUX_SDCTL_CSD1_SEL		= 1 << 13,
	MUX_CSPI1_UART3			= 1 << 14,
	MUX_EXTDMAREQ2_MBX_SEL		= 1 << 15,
	MUX_TAMPER_DETECT_EN		= 1 << 16,
	MUX_PGP_USB_4WIRE		= 1 << 17,
	MUX_PGP_USB_COMMON		= 1 << 18,
	MUX_SDHC_MEMSTICK1		= 1 << 19,
	MUX_SDHC_MEMSTICK2		= 1 << 20,
	MUX_PGP_SPLL_BYP		= 1 << 21,
	MUX_PGP_UPLL_BYP		= 1 << 22,
	MUX_PGP_MSHC1_CLK_SEL		= 1 << 23,
	MUX_PGP_MSHC2_CLK_SEL		= 1 << 24,
	MUX_CSPI3_UART5_SEL		= 1 << 25,
	MUX_PGP_ATA_9			= 1 << 26,
	MUX_PGP_USB_SUSPEND		= 1 << 27,
	MUX_PGP_USB_OTG_LOOPBACK	= 1 << 28,
	MUX_PGP_USB_HS1_LOOPBACK	= 1 << 29,
	MUX_PGP_USB_HS2_LOOPBACK	= 1 << 30,
	MUX_CLKO_DDR_MODE		= 1 << 31,
};

/* Bit definitions for RCSR register in CCM */
#define CCM_RCSR_NF16B	(1 << 31)
#define CCM_RCSR_NFMS	(1 << 30)

/* WEIM CS control registers */
struct mx31_weim_cscr {
	u32 upper;
	u32 lower;
	u32 additional;
	u32 reserved;
};

struct mx31_weim {
	struct mx31_weim_cscr cscr[6];
};

/* ESD control registers */
struct esdc_regs {
	u32 ctl0;
	u32 cfg0;
	u32 ctl1;
	u32 cfg1;
	u32 misc;
	u32 dly[5];
	u32 dlyl;
};

#endif

#define __REG(x)     (*((volatile u32 *)(x)))
#define __REG16(x)   (*((volatile u16 *)(x)))
#define __REG8(x)    (*((volatile u8 *)(x)))

#define CCM_BASE	0x53f80000
#define CCM_CCMR	(CCM_BASE + 0x00)
#define CCM_PDR0	(CCM_BASE + 0x04)
#define CCM_PDR1	(CCM_BASE + 0x08)
#define CCM_RCSR	(CCM_BASE + 0x0c)
#define CCM_MPCTL	(CCM_BASE + 0x10)
#define CCM_UPCTL	(CCM_BASE + 0x14)
#define CCM_SPCTL	(CCM_BASE + 0x18)
#define CCM_COSR	(CCM_BASE + 0x1C)
#define CCM_CGR0	(CCM_BASE + 0x20)
#define CCM_CGR1	(CCM_BASE + 0x24)
#define CCM_CGR2	(CCM_BASE + 0x28)

#define CCMR_MDS	(1 << 7)
#define CCMR_SBYCS	(1 << 4)
#define CCMR_MPE	(1 << 3)
#define CCMR_PRCS_MASK	(3 << 1)
#define CCMR_FPM	(1 << 1)
#define CCMR_CKIH	(2 << 1)

#define MX31_IIM_BASE_ADDR	0x5001C000

#define PDR0_CSI_PODF(x)	(((x) & 0x1ff) << 23)
#define PDR0_PER_PODF(x)	(((x) & 0x1f) << 16)
#define PDR0_HSP_PODF(x)	(((x) & 0x7) << 11)
#define PDR0_NFC_PODF(x)	(((x) & 0x7) << 8)
#define PDR0_IPG_PODF(x)	(((x) & 0x3) << 6)
#define PDR0_MAX_PODF(x)	(((x) & 0x7) << 3)
#define PDR0_MCU_PODF(x)	((x) & 0x7)

#define PLL_PD(x)		(((x) & 0xf) << 26)
#define PLL_MFD(x)		(((x) & 0x3ff) << 16)
#define PLL_MFI(x)		(((x) & 0xf) << 10)
#define PLL_MFN(x)		(((x) & 0x3ff) << 0)

#define GET_PDR0_CSI_PODF(x)	(((x) >> 23) & 0x1ff)
#define GET_PDR0_PER_PODF(x)	(((x) >> 16) & 0x1f)
#define GET_PDR0_HSP_PODF(x)	(((x) >> 11) & 0x7)
#define GET_PDR0_NFC_PODF(x)	(((x) >> 8) & 0x7)
#define GET_PDR0_IPG_PODF(x)	(((x) >> 6) & 0x3)
#define GET_PDR0_MAX_PODF(x)	(((x) >> 3) & 0x7)
#define GET_PDR0_MCU_PODF(x)	((x) & 0x7)

#define GET_PLL_PD(x)		(((x) >> 26) & 0xf)
#define GET_PLL_MFD(x)		(((x) >> 16) & 0x3ff)
#define GET_PLL_MFI(x)		(((x) >> 10) & 0xf)
#define GET_PLL_MFN(x)		(((x) >> 0) & 0x3ff)


#define WEIM_ESDCTL0	0xB8001000
#define WEIM_ESDCFG0	0xB8001004
#define WEIM_ESDCTL1	0xB8001008
#define WEIM_ESDCFG1	0xB800100C
#define WEIM_ESDMISC	0xB8001010

#define UART1_BASE	0x43F90000
#define UART2_BASE	0x43F94000
#define UART3_BASE	0x5000C000
#define UART4_BASE	0x43FB0000
#define UART5_BASE	0x43FB4000

#define I2C1_BASE_ADDR          0x43f80000
#define I2C1_CLK_OFFSET		26
#define I2C2_BASE_ADDR          0x43F98000
#define I2C2_CLK_OFFSET		28
#define I2C3_BASE_ADDR          0x43f84000
#define I2C3_CLK_OFFSET		30

#define ESDCTL_SDE			(1 << 31)
#define ESDCTL_CMD_RW			(0 << 28)
#define ESDCTL_CMD_PRECHARGE		(1 << 28)
#define ESDCTL_CMD_AUTOREFRESH		(2 << 28)
#define ESDCTL_CMD_LOADMODEREG		(3 << 28)
#define ESDCTL_CMD_MANUALREFRESH	(4 << 28)
#define ESDCTL_ROW_13			(2 << 24)
#define ESDCTL_ROW(x)			((x) << 24)
#define ESDCTL_COL_9			(1 << 20)
#define ESDCTL_COL(x)			((x) << 20)
#define ESDCTL_DSIZ(x)			((x) << 16)
#define ESDCTL_SREFR(x)			((x) << 13)
#define ESDCTL_PWDT(x)			((x) << 10)
#define ESDCTL_FP(x)			((x) << 8)
#define ESDCTL_BL(x)			((x) << 7)
#define ESDCTL_PRCT(x)			((x) << 0)

#define ESDCTL_BASE_ADDR	0xB8001000

/* 13 fields of the upper CS control register */
#define CSCR_U(sp, wp, bcd, bcs, psz, pme, sync, dol, \
		cnc, wsc, ew, wws, edc) \
	((sp) << 31 | (wp) << 30 | (bcd) << 28 | (psz) << 22 | (pme) << 21 |\
	 (sync) << 20 | (dol) << 16 | (cnc) << 14 | (wsc) << 8 | (ew) << 7 |\
	 (wws) << 4 | (edc) << 0)
/* 12 fields of the lower CS control register */
#define CSCR_L(oea, oen, ebwa, ebwn, \
		csa, ebc, dsz, csn, psr, cre, wrap, csen) \
	((oea) << 28 | (oen) << 24 | (ebwa) << 20 | (ebwn) << 16 |\
	 (csa) << 12 | (ebc) << 11 | (dsz) << 8 | (csn) << 4 |\
	 (psr) << 3 | (cre) << 2 | (wrap) << 1 | (csen) << 0)
/* 14 fields of the additional CS control register */
#define CSCR_A(ebra, ebrn, rwa, rwn, mum, lah, lbn, lba, dww, dct, \
		wwu, age, cnc2, fce) \
	((ebra) << 28 | (ebrn) << 24 | (rwa) << 20 | (rwn) << 16 |\
	 (mum) << 15 | (lah) << 13 | (lbn) << 10 | (lba) << 8 |\
	 (dww) << 6 | (dct) << 4 | (wwu) << 3 |\
	 (age) << 2 | (cnc2) << 1 | (fce) << 0)

#define WEIM_BASE	0xb8002000

#define IOMUXC_BASE	0x43FAC000
#define IOMUXC_SW_MUX_CTL(x)	(IOMUXC_BASE + 0xc + (x) * 4)
#define IOMUXC_SW_PAD_CTL(x)	(IOMUXC_BASE + 0x154 + (x) * 4)

#define IPU_BASE		0x53fc0000
#define IPU_CONF		IPU_BASE

#define IPU_CONF_PXL_ENDIAN	(1<<8)
#define IPU_CONF_DU_EN		(1<<7)
#define IPU_CONF_DI_EN		(1<<6)
#define IPU_CONF_ADC_EN		(1<<5)
#define IPU_CONF_SDC_EN		(1<<4)
#define IPU_CONF_PF_EN		(1<<3)
#define IPU_CONF_ROT_EN		(1<<2)
#define IPU_CONF_IC_EN		(1<<1)
#define IPU_CONF_SCI_EN		(1<<0)

#define ARM_PPMRR		0x40000015

#define WDOG_BASE		0x53FDC000

/*
 * GPIO
 */
#define GPIO1_BASE_ADDR	0x53FCC000
#define GPIO2_BASE_ADDR	0x53FD0000
#define GPIO3_BASE_ADDR	0x53FA4000
#define GPIO_DR		0x00000000	/* data register */
#define GPIO_GDIR	0x00000004	/* direction register */
#define GPIO_PSR	0x00000008	/* pad status register */

/*
 * Signal Multiplexing (IOMUX)
 */

/* bits in the SW_MUX_CTL registers */
#define MUX_CTL_OUT_GPIO_DR	(0 << 4)
#define MUX_CTL_OUT_FUNC	(1 << 4)
#define MUX_CTL_OUT_ALT1	(2 << 4)
#define MUX_CTL_OUT_ALT2	(3 << 4)
#define MUX_CTL_OUT_ALT3	(4 << 4)
#define MUX_CTL_OUT_ALT4	(5 << 4)
#define MUX_CTL_OUT_ALT5	(6 << 4)
#define MUX_CTL_OUT_ALT6	(7 << 4)
#define MUX_CTL_IN_NONE		(0 << 0)
#define MUX_CTL_IN_GPIO		(1 << 0)
#define MUX_CTL_IN_FUNC		(2 << 0)
#define MUX_CTL_IN_ALT1		(4 << 0)
#define MUX_CTL_IN_ALT2		(8 << 0)

#define MUX_CTL_FUNC		(MUX_CTL_OUT_FUNC | MUX_CTL_IN_FUNC)
#define MUX_CTL_ALT1		(MUX_CTL_OUT_ALT1 | MUX_CTL_IN_ALT1)
#define MUX_CTL_ALT2		(MUX_CTL_OUT_ALT2 | MUX_CTL_IN_ALT2)
#define MUX_CTL_GPIO		(MUX_CTL_OUT_GPIO_DR | MUX_CTL_IN_GPIO)

/* Register offsets based on IOMUXC_BASE */
/* 0x00 .. 0x7b */
#define MUX_CTL_CSPI3_MISO		0x0c
#define MUX_CTL_CSPI3_SCLK		0x0d
#define MUX_CTL_CSPI3_SPI_RDY	0x0e
#define MUX_CTL_CSPI3_MOSI		0x13

#define MUX_CTL_SD1_DATA1	0x18
#define MUX_CTL_SD1_DATA2	0x19
#define MUX_CTL_SD1_DATA3	0x1a
#define MUX_CTL_SD1_CMD		0x1d
#define MUX_CTL_SD1_CLK		0x1e
#define MUX_CTL_SD1_DATA0	0x1f

#define MUX_CTL_USBH2_DATA1	0x40
#define MUX_CTL_USBH2_DIR	0x44
#define MUX_CTL_USBH2_STP	0x45
#define MUX_CTL_USBH2_NXT	0x46
#define MUX_CTL_USBH2_DATA0	0x47
#define MUX_CTL_USBH2_CLK	0x4B

#define MUX_CTL_TXD2		0x70
#define MUX_CTL_RTS2		0x71
#define MUX_CTL_CTS2		0x72
#define MUX_CTL_RXD2		0x77

#define MUX_CTL_RTS1		0x7c
#define MUX_CTL_CTS1		0x7d
#define MUX_CTL_DTR_DCE1	0x7e
#define MUX_CTL_DSR_DCE1	0x7f
#define MUX_CTL_CSPI2_SCLK	0x80
#define MUX_CTL_CSPI2_SPI_RDY	0x81
#define MUX_CTL_RXD1		0x82
#define MUX_CTL_TXD1		0x83
#define MUX_CTL_CSPI2_MISO	0x84
#define MUX_CTL_CSPI2_SS0	0x85
#define MUX_CTL_CSPI2_SS1	0x86
#define MUX_CTL_CSPI2_SS2	0x87
#define MUX_CTL_CSPI1_SS2	0x88
#define MUX_CTL_CSPI1_SCLK	0x89
#define MUX_CTL_CSPI1_SPI_RDY	0x8a
#define MUX_CTL_CSPI2_MOSI	0x8b
#define MUX_CTL_CSPI1_MOSI	0x8c
#define MUX_CTL_CSPI1_MISO	0x8d
#define MUX_CTL_CSPI1_SS0	0x8e
#define MUX_CTL_CSPI1_SS1	0x8f
#define MUX_CTL_STXD6		0x90
#define MUX_CTL_SRXD6		0x91
#define MUX_CTL_SCK6		0x92
#define MUX_CTL_SFS6		0x93

#define MUX_CTL_STXD3		0x9C
#define MUX_CTL_SRXD3		0x9D
#define MUX_CTL_SCK3		0x9E
#define MUX_CTL_SFS3		0x9F

#define MUX_CTL_NFC_WP		0xD0
#define MUX_CTL_NFC_CE		0xD1
#define MUX_CTL_NFC_RB		0xD2
#define MUX_CTL_NFC_WE		0xD4
#define MUX_CTL_NFC_RE		0xD5
#define MUX_CTL_NFC_ALE		0xD6
#define MUX_CTL_NFC_CLE		0xD7


#define MUX_CTL_CAPTURE		0x150
#define MUX_CTL_COMPARE		0x151

/*
 * Helper macros for the MUX_[contact name]__[pin function] macros
 */
#define IOMUX_MODE_POS 9
#define IOMUX_MODE(contact, mode) (((mode) << IOMUX_MODE_POS) | (contact))

/*
 * These macros can be used in mx31_gpio_mux() and have the form
 * MUX_[contact name]__[pin function]
 */
#define MUX_RXD1__UART1_RXD_MUX	IOMUX_MODE(MUX_CTL_RXD1, MUX_CTL_FUNC)
#define MUX_TXD1__UART1_TXD_MUX	IOMUX_MODE(MUX_CTL_TXD1, MUX_CTL_FUNC)
#define MUX_RTS1__UART1_RTS_B	IOMUX_MODE(MUX_CTL_RTS1, MUX_CTL_FUNC)
#define MUX_CTS1__UART1_CTS_B	IOMUX_MODE(MUX_CTL_CTS1, MUX_CTL_FUNC)

#define MUX_RXD2__UART2_RXD_MUX	IOMUX_MODE(MUX_CTL_RXD2, MUX_CTL_FUNC)
#define MUX_TXD2__UART2_TXD_MUX	IOMUX_MODE(MUX_CTL_TXD2, MUX_CTL_FUNC)
#define MUX_RTS2__UART2_RTS_B	IOMUX_MODE(MUX_CTL_RTS2, MUX_CTL_FUNC)
#define MUX_CTS2__UART2_CTS_B	IOMUX_MODE(MUX_CTL_CTS2, MUX_CTL_FUNC)

#define MUX_CSPI2_SS0__CSPI2_SS0_B IOMUX_MODE(MUX_CTL_CSPI2_SS0, MUX_CTL_FUNC)
#define MUX_CSPI2_SS1__CSPI2_SS1_B IOMUX_MODE(MUX_CTL_CSPI2_SS1, MUX_CTL_FUNC)
#define MUX_CSPI2_SS2__CSPI2_SS2_B IOMUX_MODE(MUX_CTL_CSPI2_SS2, MUX_CTL_FUNC)
#define MUX_CSPI2_MOSI__CSPI2_MOSI IOMUX_MODE(MUX_CTL_CSPI2_MOSI, MUX_CTL_FUNC)
#define MUX_CSPI2_MISO__CSPI2_MISO IOMUX_MODE(MUX_CTL_CSPI2_MISO, MUX_CTL_FUNC)
#define MUX_CSPI2_SPI_RDY__CSPI2_DATAREADY_B \
	IOMUX_MODE(MUX_CTL_CSPI2_SPI_RDY, MUX_CTL_FUNC)
#define MUX_CSPI2_SCLK__CSPI2_CLK IOMUX_MODE(MUX_CTL_CSPI2_SCLK, MUX_CTL_FUNC)

#define MUX_CSPI1_SS0__CSPI1_SS0_B IOMUX_MODE(MUX_CTL_CSPI1_SS0, MUX_CTL_FUNC)
#define MUX_CSPI1_SS1__CSPI1_SS1_B IOMUX_MODE(MUX_CTL_CSPI1_SS1, MUX_CTL_FUNC)
#define MUX_CSPI1_SS2__CSPI1_SS2_B IOMUX_MODE(MUX_CTL_CSPI1_SS2, MUX_CTL_FUNC)
#define MUX_CSPI1_MOSI__CSPI1_MOSI IOMUX_MODE(MUX_CTL_CSPI1_MOSI, MUX_CTL_FUNC)
#define MUX_CSPI1_MISO__CSPI1_MISO IOMUX_MODE(MUX_CTL_CSPI1_MISO, MUX_CTL_FUNC)
#define MUX_CSPI1_SPI_RDY__CSPI1_DATAREADY_B \
	IOMUX_MODE(MUX_CTL_CSPI1_SPI_RDY, MUX_CTL_FUNC)
#define MUX_CSPI1_SCLK__CSPI1_CLK IOMUX_MODE(MUX_CTL_CSPI1_SCLK, MUX_CTL_FUNC)

#define MUX_CSPI2_MOSI__I2C2_SCL IOMUX_MODE(MUX_CTL_CSPI2_MOSI, MUX_CTL_ALT1)
#define MUX_CSPI2_MISO__I2C2_SDA IOMUX_MODE(MUX_CTL_CSPI2_MISO, MUX_CTL_ALT1)

/* PAD control registers for SDR/DDR */
#define IOMUXC_SW_PAD_CTL_SDCKE1_SDCLK_SDCLK_B	(IOMUXC_BASE + 0x26C)
#define IOMUXC_SW_PAD_CTL_CAS_SDWE_SDCKE0	(IOMUXC_BASE + 0x270)
#define IOMUXC_SW_PAD_CTL_BCLK_RW_RAS		(IOMUXC_BASE + 0x274)
#define IOMUXC_SW_PAD_CTL_CS5_ECB_LBA		(IOMUXC_BASE + 0x278)
#define IOMUXC_SW_PAD_CTL_CS2_CS3_CS4		(IOMUXC_BASE + 0x27C)
#define IOMUXC_SW_PAD_CTL_OE_CS0_CS1		(IOMUXC_BASE + 0x280)
#define IOMUXC_SW_PAD_CTL_DQM3_EB0_EB1		(IOMUXC_BASE + 0x284)
#define IOMUXC_SW_PAD_CTL_DQM0_DQM1_DQM2	(IOMUXC_BASE + 0x288)
#define IOMUXC_SW_PAD_CTL_SD29_SD30_SD31	(IOMUXC_BASE + 0x28C)
#define IOMUXC_SW_PAD_CTL_SD26_SD27_SD28	(IOMUXC_BASE + 0x290)
#define IOMUXC_SW_PAD_CTL_SD23_SD24_SD25	(IOMUXC_BASE + 0x294)
#define IOMUXC_SW_PAD_CTL_SD20_SD21_SD22	(IOMUXC_BASE + 0x298)
#define IOMUXC_SW_PAD_CTL_SD17_SD18_SD19	(IOMUXC_BASE + 0x29C)
#define IOMUXC_SW_PAD_CTL_SD14_SD15_SD16	(IOMUXC_BASE + 0x2A0)
#define IOMUXC_SW_PAD_CTL_SD11_SD12_SD13	(IOMUXC_BASE + 0x2A4)
#define IOMUXC_SW_PAD_CTL_SD8_SD9_SD10		(IOMUXC_BASE + 0x2A8)
#define IOMUXC_SW_PAD_CTL_SD5_SD6_SD7		(IOMUXC_BASE + 0x2AC)
#define IOMUXC_SW_PAD_CTL_SD2_SD3_SD4		(IOMUXC_BASE + 0x2B0)
#define IOMUXC_SW_PAD_CTL_SDBA0_SD0_SD1		(IOMUXC_BASE + 0x2B4)
#define IOMUXC_SW_PAD_CTL_A24_A25_SDBA1		(IOMUXC_BASE + 0x2B8)
#define IOMUXC_SW_PAD_CTL_A21_A22_A23		(IOMUXC_BASE + 0x2BC)
#define IOMUXC_SW_PAD_CTL_A18_A19_A20		(IOMUXC_BASE + 0x2C0)
#define IOMUXC_SW_PAD_CTL_A15_A16_A17		(IOMUXC_BASE + 0x2C4)
#define IOMUXC_SW_PAD_CTL_A12_A13_A14		(IOMUXC_BASE + 0x2C8)
#define IOMUXC_SW_PAD_CTL_A10_MA10_A11		(IOMUXC_BASE + 0x2CC)
#define IOMUXC_SW_PAD_CTL_A7_A8_A9		(IOMUXC_BASE + 0x2D0)
#define IOMUXC_SW_PAD_CTL_A4_A5_A6		(IOMUXC_BASE + 0x2D4)
#define IOMUXC_SW_PAD_CTL_A1_A2_A3		(IOMUXC_BASE + 0x2D8)
#define IOMUXC_SW_PAD_CTL_VPG0_VPG1_A0		(IOMUXC_BASE + 0x2DC)

/*
 * Memory regions and CS
 */
#define IPU_MEM_BASE	0x70000000
#define CSD0_BASE	0x80000000
#define CSD1_BASE	0x90000000
#define CS0_BASE	0xA0000000
#define CS1_BASE	0xA8000000
#define CS2_BASE	0xB0000000
#define CS3_BASE	0xB2000000
#define CS4_BASE	0xB4000000
#define CS4_PSRAM_BASE	0xB5000000
#define CS5_BASE	0xB6000000
#define PCMCIA_MEM_BASE	0xC0000000

/*
 * NAND controller
 */
#define NFC_BASE_ADDR	0xB8000000

/* SD card controller */
#define SDHC1_BASE_ADDR	0x50004000
#define SDHC2_BASE_ADDR	0x50008000

/*
 * Internal RAM (16KB)
 */
#define	IRAM_BASE_ADDR	0x1FFFC000
#define IRAM_SIZE	(16 * 1024)

#define MX31_AIPS1_BASE_ADDR	0x43f00000
#define IMX_USB_BASE		(MX31_AIPS1_BASE_ADDR + 0x88000)

/* USB portsc */
/* values for portsc field */
#define MXC_EHCI_PHY_LOW_POWER_SUSPEND	(1 << 23)
#define MXC_EHCI_FORCE_FS		(1 << 24)
#define MXC_EHCI_UTMI_8BIT		(0 << 28)
#define MXC_EHCI_UTMI_16BIT		(1 << 28)
#define MXC_EHCI_SERIAL			(1 << 29)
#define MXC_EHCI_MODE_UTMI		(0 << 30)
#define MXC_EHCI_MODE_PHILIPS		(1 << 30)
#define MXC_EHCI_MODE_ULPI		(2 << 30)
#define MXC_EHCI_MODE_SERIAL		(3 << 30)

/* values for flags field */
#define MXC_EHCI_INTERFACE_DIFF_UNI	(0 << 0)
#define MXC_EHCI_INTERFACE_DIFF_BI	(1 << 0)
#define MXC_EHCI_INTERFACE_SINGLE_UNI	(2 << 0)
#define MXC_EHCI_INTERFACE_SINGLE_BI	(3 << 0)
#define MXC_EHCI_INTERFACE_MASK		(0xf)

#define MXC_EHCI_POWER_PINS_ENABLED	(1 << 5)
#define MXC_EHCI_TTL_ENABLED		(1 << 6)

#define MXC_EHCI_INTERNAL_PHY		(1 << 7)
#define MXC_EHCI_IPPUE_DOWN		(1 << 8)
#define MXC_EHCI_IPPUE_UP		(1 << 9)

/*
 * CSPI register definitions
 */
#define MXC_CSPI
#define MXC_CSPICTRL_EN		(1 << 0)
#define MXC_CSPICTRL_MODE	(1 << 1)
#define MXC_CSPICTRL_XCH	(1 << 2)
#define MXC_CSPICTRL_SMC	(1 << 3)
#define MXC_CSPICTRL_POL	(1 << 4)
#define MXC_CSPICTRL_PHA	(1 << 5)
#define MXC_CSPICTRL_SSCTL	(1 << 6)
#define MXC_CSPICTRL_SSPOL	(1 << 7)
#define MXC_CSPICTRL_CHIPSELECT(x)	(((x) & 0x3) << 24)
#define MXC_CSPICTRL_BITCOUNT(x)	(((x) & 0x1f) << 8)
#define MXC_CSPICTRL_DATARATE(x)	(((x) & 0x7) << 16)
#define MXC_CSPICTRL_TC		(1 << 8)
#define MXC_CSPICTRL_RXOVF	(1 << 6)
#define MXC_CSPICTRL_MAXBITS	0x1f

#define MXC_CSPIPERIOD_32KHZ	(1 << 15)
#define MAX_SPI_BYTES	4

#define MXC_SPI_BASE_ADDRESSES \
	0x43fa4000, \
	0x50010000, \
	0x53f84000,

#endif /* __ASM_ARCH_MX31_IMX_REGS_H */
