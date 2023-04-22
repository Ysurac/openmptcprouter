/*
 * Copyright (c) 2012 - 2015 The Linux Foundation. All rights reserved.
 */

#ifndef _IPQ_GMAC_H
#define _IPQ_GMAC_H
#include <common.h>
#include <net.h>
#include <configs/ipq806x_cdp.h>

#define CONFIG_MACRESET_TIMEOUT			(3 * CONFIG_SYS_HZ)
#define CONFIG_MDIO_TIMEOUT			(3 * CONFIG_SYS_HZ)
#define CONFIG_PHYRESET_TIMEOUT			(3 * CONFIG_SYS_HZ)
#define CONFIG_AUTONEG_TIMEOUT			(5 * CONFIG_SYS_HZ)
/* MAC configuration register definitions */
#define FRAMEBURSTENABLE			(1 << 21)
#define MII_PORTSELECT				(1 << 15)
#define FES_100					(1 << 14)
#define DISABLERXOWN				(1 << 13)
#define FULLDPLXMODE				(1 << 11)
#define RXENABLE				(1 << 2)
#define TXENABLE				(1 << 3)
#define DW_DMA_BASE_OFFSET			(0x1000)

/* Poll demand definitions */
#define POLL_DATA				(0x0)


/* Descriptior related definitions */
#define MAC_MAX_FRAME_SZ			(1600)

/*
 * txrx_status definitions
 */

/* tx status bits definitions */
#define DESC_TXSTS_OWNBYDMA			(1 << 31)
#define DESC_TXSTS_TXINT			(1 << 30)
#define DESC_TXSTS_TXLAST			(1 << 29)
#define DESC_TXSTS_TXFIRST			(1 << 28)
#define DESC_TXSTS_TXCRCDIS			(1 << 27)

#define DESC_TXSTS_TXPADDIS			(1 << 26)
#define DESC_TXSTS_TXCHECKINSCTRL		(3 << 22)
#define DESC_TXSTS_TXRINGEND			(1 << 21)
#define DESC_TXSTS_TXCHAIN			(1 << 20)
#define DESC_TXSTS_MSK				(0x1FFFF << 0)

/* rx status bits definitions */
#define DESC_RXSTS_OWNBYDMA			(1 << 31)
#define DESC_RXSTS_DAFILTERFAIL			(1 << 30)
#define DESC_RXSTS_FRMLENMSK			(0x3FFF << 16)
#define DESC_RXSTS_FRMLENSHFT			(16)

#define DESC_RXSTS_ERROR			(1 << 15)
#define DESC_RXSTS_RXTRUNCATED			(1 << 14)
#define DESC_RXSTS_SAFILTERFAIL			(1 << 13)
#define DESC_RXSTS_RXIPC_GIANTFRAME		(1 << 12)
#define DESC_RXSTS_RXDAMAGED			(1 << 11)
#define DESC_RXSTS_RXVLANTAG			(1 << 10)
#define DESC_RXSTS_RXFIRST			(1 << 9)
#define DESC_RXSTS_RXLAST			(1 << 8)
#define DESC_RXSTS_RXIPC_GIANT			(1 << 7)
#define DESC_RXSTS_RXCOLLISION			(1 << 6)
#define DESC_RXSTS_RXFRAMEETHER			(1 << 5)
#define DESC_RXSTS_RXWATCHDOG			(1 << 4)
#define DESC_RXSTS_RXMIIERROR			(1 << 3)
#define DESC_RXSTS_RXDRIBBLING			(1 << 2)
#define DESC_RXSTS_RXCRC			(1 << 1)

/*
 * dmamac_cntl definitions
 */

/* tx control bits definitions */
#if defined(CONFIG_DW_ALTDESCRIPTOR)

#define DESC_TXCTRL_SIZE1MASK			(0x1FFF << 0)
#define DESC_TXCTRL_SIZE1SHFT			(0)
#define DESC_TXCTRL_SIZE2MASK			(0x1FFF << 16)
#define DESC_TXCTRL_SIZE2SHFT			(16)

#else

#define DESC_TXCTRL_TXINT			(1 << 31)
#define DESC_TXCTRL_TXLAST			(1 << 30)
#define DESC_TXCTRL_TXFIRST			(1 << 29)
#define DESC_TXCTRL_TXCHECKINSCTRL		(3 << 27)
#define DESC_TXCTRL_TXCRCDIS			(1 << 26)
#define DESC_TXCTRL_TXRINGEND			(1 << 25)
#define DESC_TXCTRL_TXCHAIN			(1 << 24)

#define DESC_TXCTRL_SIZE1MASK			(0x7FF << 0)
#define DESC_TXCTRL_SIZE1SHFT			(0)
#define DESC_TXCTRL_SIZE2MASK			(0x7FF << 11)
#define DESC_TXCTRL_SIZE2SHFT			(11)

#endif

/* rx control bits definitions */
#if defined(CONFIG_DW_ALTDESCRIPTOR)

#define DESC_RXCTRL_RXINTDIS			(1 << 31)
#define DESC_RXCTRL_RXRINGEND			(1 << 15)
#define DESC_RXCTRL_RXCHAIN			(1 << 14)

#define DESC_RXCTRL_SIZE1MASK			(0x1FFF << 0)
#define DESC_RXCTRL_SIZE1SHFT			(0)
#define DESC_RXCTRL_SIZE2MASK			(0x1FFF << 16)
#define DESC_RXCTRL_SIZE2SHFT			(16)

#else

#define DESC_RXCTRL_RXINTDIS			(1 << 31)
#define DESC_RXCTRL_RXRINGEND			(1 << 25)
#define DESC_RXCTRL_RXCHAIN			(1 << 24)

#define DESC_RXCTRL_SIZE1MASK			(0x7FF << 0)
#define DESC_RXCTRL_SIZE1SHFT			(0)
#define DESC_RXCTRL_SIZE2MASK			(0x7FF << 11)
#define DESC_RXCTRL_SIZE2SHFT			(11)

#endif

/* Speed specific definitions */
#define NETDEV_TX_BUSY				(1)

/* Duplex mode specific definitions */
#define HALF_DUPLEX				(1)
#define FULL_DUPLEX				(2)

/* Bus mode register definitions */
#define FIXEDBURST				(1 << 16)
#define PRIORXTX_41				(3 << 14)
#define PRIORXTX_31				(2 << 14)
#define PRIORXTX_21				(1 << 14)
#define PRIORXTX_11				(0 << 14)
#define BURST_1					(1 << 8)
#define BURST_2					(2 << 8)
#define BURST_4					(4 << 8)
#define BURST_8					(8 << 8)
#define BURST_16				(16 << 8)
#define BURST_32				(32 << 8)
#define RXHIGHPRIO				(1 << 1)
#define DMAMAC_SRST				(1 << 0)
#define DMA_ARB					(1 << 1)
#define DESC_SKIP_LEN_0				(0 << 2)
#define DMA_INT_DISABLE				(0 << 0)
/* Operation mode definitions */
#define STOREFORWARD				(1 << 21)
#define FLUSHTXFIFO				(1 << 20)
#define TXSTART					(1 << 13)
#define TXSECONDFRAME				(1 << 2)
#define RXSTART					(1 << 1)
#define RX_THRESHOLD_128			(3 << 3)
#define OP_SECOND_FRAME				(1 << 2)

/* GMAC config definitions */
#define MII_PORT_SELECT				(1 << 15)
#define GMII_PORT_SELECT			(0 << 15)
#define FRAME_BURST_ENABLE			(1 << 21)
#define JUMBO_FRAME_ENABLE			(1 << 20)
#define HALF_DUPLEX_ENABLE			(0 << 11)
#define FULL_DUPLEX_ENABLE			(1 << 11)
#define TX_ENABLE				(1 << 3)
#define RX_ENABLE				(1 << 2)
#define RX_IPC_OFFLOAD				(1 << 10)

/* GMAC Fram filter definitions */
#define GMAC_FRAME_RX_ALL			(1 << 31)
#define PROMISCUOUS_MODE_ON			(1 << 0)
#define DISABLE_BCAST_FRAMES			(1 << 5)

/* DMA Flow control definitions */
#define FULL_3KB				(1 << 10)
#define HW_FLW_CNTL_ENABLE			(1 << 8)

/* GMAC Flow control definitions */
#define RX_FLW_CNTL_ENABLE			(1 << 2)
#define TX_FLW_CNTL_ENABLE			(1 << 1)


/* Descriptor definitions */
#define TX_END_OF_RING				(1 << 21)
#define RX_END_OF_RING				(1 << 15)

#define NO_OF_TX_DESC				8
#define NO_OF_RX_DESC				PKTBUFSRX
#define MAX_WAIT				1000

#define CACHE_LINE_SIZE				(CONFIG_SYS_CACHELINE_SIZE)

#define VLAN_ETH_FRAME_LEN			1518
#define ETH_ZLEN				60
#define ETH_HLEN				12
#define ETH_FCS_LEN				4
#define VLAN_HLEN				4
#define NET_IP_ALIGN				2

#define ETHERNET_EXTRA				(NET_IP_ALIGN + 2 * CACHE_LINE_SIZE)
#define ETH_MAX_FRAME_LEN			(VLAN_ETH_FRAME_LEN + \
						 ETH_FCS_LEN + \
						 ((4 - NET_IP_ALIGN) & 0x3))
/* QSGMII PHY bit definitions */
#define QSGMII_PHY_SRDS_PLL_ICP_600UA		(6 << 28)
#define QSGMII_PHY_SRDS_PLL_AMP			(2 << 24)
#define QSGMII_PHY_SRDS_RSV			(16 << 16)
#define QSGMII_PHY_SRDS_EN_LCKDT		(1 << 1)
#define QSGMII_PHY_SRDS_EN_PLL			(1 << 2)
#define QSGMII_PHY_SRDS_PLL_LPFR		(4 << 4)
#define QSGMII_PHY_QSGMII_TXDRV			(12 << 28)
#define QSGMII_PHY_QSGMII_SLC			(2 << 26)
#define QSGMII_PHY_QSGMII_RXPI_SLC		(2 << 22)
#define QSGMII_PHY_QSGMII_RXEQ			(1 << 20)
#define QSGMII_PHY_QSGMII_RXCOM			(2 << 18)
#define QSGMII_PHY_QDGMII_PGAIN(x)		(x << 12)
#define QSGMII_PHY_QSGMII_DEEMP			(2 << 10)
#define QSGMII_PHY_QSGMII_TXPI_SLC		(2 << 8)
#define QSGMII_PHY_QSGMII_EN_SGMII		(1 << 7)
#define QSGMII_PHY_QSGMII_EN_TX			(1 << 3)
#define QSGMII_PHY_QSGMII_EN_SD			(1 << 2)
#define QSGMII_PHY_QSGMII_EN_RX			(1 << 1)
#define QSGMII_PHY_QSGMII_EN_CDR		(1 << 0)
#define QSGMII_PHY_CDR_EN			(1 << 0)
#define QSGMII_PHY_RX_FRONT_EN			(1 << 1)
#define QSGMII_PHY_RX_SIGNAL_DETECT_EN		(1 << 2)
#define QSGMII_PHY_TX_DRIVER_EN			(1 << 3)
#define QSGMII_PHY_QSGMII_EN			(1 << 7)
#define QSGMII_PHY_DEEMPHASIS_LVL		(2 << 10)
#define QSGMII_PHY_PHASE_LOOP_GAIN		(2 << 12)
#define QSGMII_PHY_RX_DC_BIAS			(2 << 18)
#define QSGMII_PHY_RX_INPUT_EQU			(1 << 20)
#define QSGMII_PHY_CDR_PI_SLEW			(2 << 22)
#define QSGMII_PHY_TX_SLEW			(2 << 26)
#define QSGMII_PHY_TX_DRV_AMP			(12 << 28)

/* NSS_RESET_SPARE bit definitions */
#define NSS_RESET_ALL				0x1FFFFFFF
#define SRDS_N_RESET_CLR			(1 << 28)
#define CAL_PRBS_RST_N_RESET_CLR		(1 << 26)
#define LCKDT_RST_N_RESET_CLR			(1 << 27)
#define PCS_CH_SPEED_FORCE_OUTSIDE		8
#define PCS_CHn_SPEED_FORCE_OUTSIDE(n)		(PCS_CH_SPEED_FORCE_OUTSIDE << \
						(n * 4))
/* PCS channel bit definitions */
#define PCS_DEBUG_SELECT			0xF0000000
#define PCS_CH0_FORCE_SPEED			2
#define PCS_CHn_FORCE_SPEED(n)			(PCS_CH0_FORCE_SPEED << (n * 4))
#define PCS_CH0_SPEED_MASK			0xC
#define PCS_CHn_SPEED_MASK(n)			(PCS_CH0_SPEED_MASK << (n * 4))
#define PCS_CH_SPEED_10				0x0
#define PCS_CH_SPEED_100			0x4
#define PCS_CH_SPEED_1000			0x8
#define PCS_CHn_SPEED(ch, speed)		(speed << (ch * 4))


#define PCS_CHn_ANEG_EN(ch)			(1 << ((ch * 8) + 7))
#define PCS_CHn_ANEG_DISABLE(ch)		(~PCS_CHn_ANEG_EN(ch))

#define PCS_SGMII_MAC				2

#define PCS_CH0_SD_SERDES			(1 << 16)
#define PCS_CH1_SD_SERDES			(1 << 17)
#define PCS_CH2_SD_SERDES			(1 << 18)
#define PCS_CH3_SD_SERDES			(1 << 19)

#define PHY_SPEED_DUPLEX_RESOLVED_MASK		1
#define PHY_SPEED_DUPLEX_RESOLVED_SHIFT		11
#define PHY_SPEED_MASK				3
#define PHY_SPEED_SHIFT				14
#define PHY_DUPLEX_SHIFT			13
#define PHY_DUPLEX_MASK				1
#define SPEED_1000M				2
#define SPEED_100M				1
#define SPEED_10M				0

/* PHY addr for 8033 Port */
#define GMAC_8033_PORT0				0x6
#define GMAC_8033_PORT1				0x7

#define NSS_ETH_CLK_DIV(clk_div, mac_unit)	(clk_div << (mac_unit * 8))
#define NSS_ETH_CLK_DIV_SHIFT			16
#define NSS_ETH_CLK_DIV_MASK			0x7F
#define CLK_DIV_SGMII_100M			(CLK_DIV_QSGMII_100M / 2)
#define CLK_DIV_SGMII_1000M			(CLK_DIV_QSGMII_1000M / 2)
#define CLK_DIV_SGMII_10M			(CLK_DIV_QSGMII_10M / 2)
#define CLK_DIV_QSGMII_100M			10
#define CLK_DIV_QSGMII_1000M			2
#define CLK_DIV_QSGMII_10M			100
#define CLK_DIV_SHIFT				8
#define MAC_CFG_DUPLEX_SHIFT			11
#define GMAC_UNIT0				0
#define GMAC_UNIT1				1
#define GMAC_UNIT2				2
#define GMAC_UNIT3				3

/* Bit definitions for PCS_QSGMII_CTL register */
#define PCS_QSGMII_SW_VER_1_7			0x2
#define PCS_QSGMII_CUTTHROUGH_RX		0x8
#define PCS_QSGMII_CUTTHROUGH_TX		0x4
#define PCS_QSGMII_SW_VER_1_7			0x2
#define PCS_QSGMII_ATHR_CSCO_AUTONEG		0x1
#define PCS_QSGMII_SHORT_LATENCY		0x20
#define PCS_QSGMII_SHORT_THRESH			0x10
#define PCS_QSGMII_DEPTH_THRESH(n)		(n << 8)
#define PCS_CH0_SERDES_SN_DETECT		0x800
#define PCS_CHn_SERDES_SN_DETECT(n)		(PCS_CH0_SERDES_SN_DETECT << n)
#define PCS_CH0_SERDES_SN_DETECT_2		0x10000
#define PCS_CHn_SERDES_SN_DETECT_2(n)		(PCS_CH0_SERDES_SN_DETECT_2 << n)

#define PCS_QSGMII_MAC_LINK(stat, macn)		(((stat) >> \
						(macn * 8)) & 1)

#define PCS_QSGMII_MAC_SPEED(stat,macn)		(((stat) >> \
						((macn * 8) + 2)) & 3)

#define PCS_QSGMII_MAC_DUPLEX(stat,macn)	(((stat) >> \
						((macn * 8) + 1)) & 1)

#define NSS_ETH_GMAC_PHY_INTF_SEL		(0 << 16)
#define NSS_ETH_GMAC_PHY_IFG_LIMIT		(12 << 8)
#define NSS_ETH_GMAC_PHY_IFG			12
#define MACSEC_BYPASS_EXT_EN			0x7

#define phy_reg_read(base, addr, reg)	\
	ipq_mdio_read(addr, reg, NULL)

#define phy_reg_write(base, addr, reg, data)	\
	ipq_mdio_write(addr, reg, data)


typedef struct
{
	volatile u32 status;		/* Status */
	volatile u32 length;		/* Buffer 1 and Buffer 2 length */
	volatile u32 buffer1;		/* Network Buffer 1 pointer (Dma-able) */
	volatile u32 data1;		/* This holds virtual address of buffer1, not used by DMA */
	/* Following is used only by driver */
	volatile u32 extstatus;		/* Extended status of a Rx Descriptor */
	volatile u32 reserved1;		/* Reserved word */
	volatile u32 timestamplow;	/* Lower 32 bits of the 64 bit timestamp value */
	volatile u32 timestamphigh;	/* Higher 32 bits of the 64 bit timestamp value */
} ipq_gmac_desc_t ;

struct ipq_forced_mode {
	u32 speed;
	u32 phy_addr;
	u8 miiwrite_done;
	u8 gmac_port;
	u8 is_forced;
};

struct bitbang_nodes {
	int mdio;
	int mdc;
} __attribute__ ((aligned(8)));

struct ipq_eth_dev {
	u8			*phy_address;
	uint			no_of_phys;
	uint			interface;
	uint			speed;
	uint			duplex;
	uint			phy_configured;
	uint			mac_unit;
	uint			mac_ps;
	int			link_printed;
	u32			padding;
	ipq_gmac_desc_t		*desc_tx[NO_OF_TX_DESC];
	ipq_gmac_desc_t		*desc_rx[NO_OF_RX_DESC];
	uint			next_tx;
	uint			next_rx;
	int			txdesc_count;
	int			rxdesc_count;
	struct eth_mac_regs	*mac_regs_p;
	struct eth_dma_regs	*dma_regs_p;
	struct eth_device *dev;
	const char phy_name[MDIO_NAME_LEN];
	struct ipq_forced_mode *forced_params;
	ipq_gmac_board_cfg_t	*gmac_board_cfg;
} __attribute__ ((aligned(8)));

struct eth_mac_regs {
	u32 conf;		/* 0x00 */
	u32 framefilt;		/* 0x04 */
	u32 hashtablehigh;	/* 0x08 */
	u32 hashtablelow;	/* 0x0c */
	u32 miiaddr;		/* 0x10 */
	u32 miidata;		/* 0x14 */
	u32 flowcontrol;	/* 0x18 */
	u32 vlantag;		/* 0x1c */
	u32 version;		/* 0x20 */
	u8 reserved_1[20];	/* 0x24 -- 0x37 */
	u32 intreg;		/* 0x38 */
	u32 intmask;		/* 0x3c */
	u32 macaddr0hi;		/* 0x40 */
	u32 macaddr0lo;		/* 0x44 */
};

struct eth_dma_regs {
	u32 busmode;		/* 0x00 */
	u32 txpolldemand;	/* 0x04 */
	u32 rxpolldemand;	/* 0x08 */
	u32 rxdesclistaddr;	/* 0x0c */
	u32 txdesclistaddr;	/* 0x10 */
	u32 status;		/* 0x14 */
	u32 opmode;		/* 0x18 */
	u32 intenable;		/* 0x1c */
	u8 reserved[40];	/* 0x20 -- 0x47 */
	u32 currhosttxdesc;	/* 0x48 */
	u32 currhostrxdesc;	/* 0x4c */
	u32 currhosttxbuffaddr;	/* 0x50 */
	u32 currhostrxbuffaddr;	/* 0x54 */
};

void athrs17_reg_init(ipq_gmac_board_cfg_t *);
void gmac_reset(void);
void gmacsec_reset(void);

enum DmaDescriptorStatus {	/* status word of DMA descriptor */
	DescOwnByDma		= 0x80000000,	/* (OWN)Descriptor is owned by DMA engine		31	RW	*/

	DescDAFilterFail	= 0x40000000,	/* (AFM)Rx - DA Filter Fail for the rx frame		30		*/

	DescFrameLengthMask	= 0x3FFF0000,	/* (FL)Receive descriptor frame length			29:16		*/
	DescFrameLengthShift	= 16,

	DescError		= 0x00008000,	/* (ES)Error summary bit - OR of the follo. bits:	15		*/
						/* DE || OE || IPC || LC || RWT || RE || CE */
	DescRxTruncated		= 0x00004000,	/* (DE)Rx - no more descriptors for receive frame	14		*/
	DescSAFilterFail	= 0x00002000,	/* (SAF)Rx - SA Filter Fail for the received frame	13		*/
	DescRxLengthError	= 0x00001000,	/* (LE)Rx - frm size not matching with len field	12		*/
	DescRxDamaged		= 0x00000800,	/* (OE)Rx - frm was damaged due to buffer overflow	11		*/
	DescRxVLANTag		= 0x00000400,	/* (VLAN)Rx - received frame is a VLAN frame		10		*/
	DescRxFirst		= 0x00000200,	/* (FS)Rx - first descriptor of the frame		 9		*/
	DescRxLast		= 0x00000100,	/* (LS)Rx - last descriptor of the frame		 8		*/
	DescRxLongFrame		= 0x00000080,	/* (Giant Frame)Rx - frame is longer than 1518/1522	 7		*/
	DescRxCollision		= 0x00000040,	/* (LC)Rx - late collision occurred during reception	 6		*/
	DescRxFrameEther	= 0x00000020,	/* (FT)Rx - Frame type - Ethernet, otherwise 802.3	 5		*/
	DescRxWatchdog		= 0x00000010,	/* (RWT)Rx - watchdog timer expired during reception	 4		*/
	DescRxMiiError		= 0x00000008,	/* (RE)Rx - error reported by MII interface		 3		*/
	DescRxDribbling		= 0x00000004,	/* (DE)Rx - frame contains non int multiple of 8 bits	 2		*/
	DescRxCrc		= 0x00000002,	/* (CE)Rx - CRC error					 1		*/

	DescRxEXTsts		= 0x00000001,	/* Extended Status Available (RDES4)			 0		*/

	DescTxIntEnable		= 0x40000000,	/* (IC)Tx - interrupt on completion			30		*/
	DescTxLast		= 0x20000000,	/* (LS)Tx - Last segment of the frame			29		*/
	DescTxFirst		= 0x10000000,	/* (FS)Tx - First segment of the frame			28		*/
	DescTxDisableCrc	= 0x08000000,	/* (DC)Tx - Add CRC disabled (first segment only)	27		*/
	DescTxDisablePadd	= 0x04000000,	/* (DP)disable padding, added by - reyaz		26		*/

	DescTxCisMask		= 0x00c00000,	/* Tx checksum offloading control mask			23:22		*/
	DescTxCisBypass		= 0x00000000,	/* Checksum bypass							*/
	DescTxCisIpv4HdrCs	= 0x00400000,	/* IPv4 header checksum							*/
	DescTxCisTcpOnlyCs	= 0x00800000,	/* TCP/UDP/ICMP checksum. Pseudo header checksum is assumed to be present*/
	DescTxCisTcpPseudoCs	= 0x00c00000,	/* TCP/UDP/ICMP checksum fully in hardware including pseudo header	*/

	TxDescEndOfRing		= 0x00200000,	/* (TER)End of descriptors ring				21		*/
	TxDescChain		= 0x00100000,	/* (TCH)Second buffer address is chain address		20		*/

	DescRxChkBit0		= 0x00000001,	/* () Rx - Rx Payload Checksum Error			 0		*/
	DescRxChkBit7		= 0x00000080,	/* (IPC CS ERROR)Rx - Ipv4 header checksum error	 7		*/
	DescRxChkBit5		= 0x00000020,	/* (FT)Rx - Frame type - Ethernet, otherwise 802.3	 5		*/

	DescRxTSavail		= 0x00000080,	/* Time stamp available					 7		*/
	DescRxFrameType		= 0x00000020,	/* (FT)Rx - Frame type - Ethernet, otherwise 802.3	 5		*/

	DescTxIpv4ChkError	= 0x00010000,	/* (IHE) Tx Ip header error				16		*/
	DescTxTimeout		= 0x00004000,	/* (JT)Tx - Transmit jabber timeout			14		*/
	DescTxFrameFlushed	= 0x00002000,	/* (FF)Tx - DMA/MTL flushed the frame due to SW flush	13		*/
	DescTxPayChkError	= 0x00001000,	/* (PCE) Tx Payload checksum Error			12		*/
	DescTxLostCarrier	= 0x00000800,	/* (LC)Tx - carrier lost during tramsmission		11		*/
	DescTxNoCarrier		= 0x00000400,	/* (NC)Tx - no carrier signal from the tranceiver	10		*/
	DescTxLateCollision	= 0x00000200,	/* (LC)Tx - transmission aborted due to collision	 9		*/
	DescTxExcCollisions	= 0x00000100,	/* (EC)Tx - transmission aborted after 16 collisions	 8		*/
	DescTxVLANFrame		= 0x00000080,	/* (VF)Tx - VLAN-type frame				 7		*/

	DescTxCollMask		= 0x00000078,	/* (CC)Tx - Collision count				6:3		*/
	DescTxCollShift		= 3,

	DescTxExcDeferral	= 0x00000004,	/* (ED)Tx - excessive deferral				 2		*/
	DescTxUnderflow		= 0x00000002,	/* (UF)Tx - late data arrival from the memory		 1		*/
	DescTxDeferred		= 0x00000001,	/* (DB)Tx - frame transmision deferred			 0		*/

	/*
	 * This explains the RDES1/TDES1 bits layout
	 * ------------------------------------------------------------------------
	 * RDES1/TDES1 | Control Bits | Byte Count Buffer 2 | Byte Count Buffer 1 |
	 * ------------------------------------------------------------------------
	 */
	// DmaDescriptorLength	length word of DMA descriptor


	RxDisIntCompl		= 0x80000000,	/* (Disable Rx int on completion) 			31		*/
	RxDescEndOfRing		= 0x00008000,	/* (TER)End of descriptors ring				15		*/
	RxDescChain		= 0x00004000,	/* (TCH)Second buffer address is chain address		14		*/

	// DescSize2Mask	= 0x1FFF0000,	/* (TBS2) Buffer 2 size					28:16		*/
	// DescSize2Shift	= 16,
	DescSize1Mask		= 0x00001FFF,	/* (TBS1) Buffer 1 size					12:0		*/
	DescSize1Shift		= 0,


	/*
	 * This explains the RDES4 Extended Status bits layout
	 * ----------------------------------------------------------------------
	 * RDES4	|		Extended Status				|
	 * ----------------------------------------------------------------------
	 */
	DescRxPtpAvail		= 0x00004000,	/* PTP snapshot available				14		*/
	DescRxPtpVer		= 0x00002000,	/* When set indicates IEEE1584 Version 2 (else Ver1)	13		*/
	DescRxPtpFrameType	= 0x00001000,	/* PTP frame type Indicates PTP sent over ethernet	12		*/
	DescRxPtpMessageType	= 0x00000F00,	/* Message Type						11:8		*/
	DescRxPtpNo		= 0x00000000,	/* 0000 => No PTP message received					*/
	DescRxPtpSync		= 0x00000100,	/* 0001 => Sync (all clock types) received				*/
	DescRxPtpFollowUp	= 0x00000200,	/* 0010 => Follow_Up (all clock types) received				*/
	DescRxPtpDelayReq	= 0x00000300,	/* 0011 => Delay_Req (all clock types) received				*/
	DescRxPtpDelayResp	= 0x00000400,	/* 0100 => Delay_Resp (all clock types) received			*/
	DescRxPtpPdelayReq	= 0x00000500,	/* 0101 => Pdelay_Req (in P to P tras clk) or Announce in Ord and Bound clk */
	DescRxPtpPdelayResp	= 0x00000600,	/* 0110 => Pdealy_Resp(in P to P trans clk) or Management in Ord and Bound clk */
	DescRxPtpPdelayRespFP	= 0x00000700,	/* 0111 => Pdealy_Resp_Follow_Up (in P to P trans clk) or Signaling in Ord and Bound clk */
	DescRxPtpIPV6		= 0x00000080,	/* Received Packet is in IPV6 Packet			 7		*/
	DescRxPtpIPV4		= 0x00000040,	/* Received Packet is in IPV4 Packet			 6		*/

	DescRxChkSumBypass	= 0x00000020,	/* When set indicates checksum offload engine		 5
						 * is bypassed								*/
	DescRxIpPayloadError	= 0x00000010,	/* When set indicates 16bit IP payload CS is in error	 4		*/
	DescRxIpHeaderError	= 0x00000008,	/* When set indicates 16bit IPV4 header CS is in	 3
						 * error or IP datagram version is not consistent
						 * with Ethernet type value						*/
	DescRxIpPayloadType	= 0x00000007,	/* Indicate the type of payload encapsulated		2:0
						 * in IPdatagram processed by COE (Rx)					*/
	DescRxIpPayloadUnknown	= 0x00000000,	/* Unknown or didnot process IP payload					*/
	DescRxIpPayloadUDP	= 0x00000001,	/* UDP									*/
	DescRxIpPayloadTCP	= 0x00000002,	/* TCP									*/
	DescRxIpPayloadICMP	= 0x00000003,	/* ICMP									*/

};

/**********************************************************
 * GMAC DMA registers
 * For Pci based system address is BARx + GmaDmaBase
 * For any other system translation is done accordingly
 **********************************************************/

enum DmaRegisters
{
	DmaBusMode	= 0x0000,	/* CSR0	- Bus Mode Register			*/
	DmaTxPollDemand	= 0x0004,	/* CSR1	- Transmit Poll Demand Register		*/
	DmaRxPollDemand	= 0x0008,	/* CSR2	- Receive Poll Demand Register		*/
	DmaRxBaseAddr	= 0x000C,	/* CSR3	- Receive Descriptor list base address	*/
	DmaTxBaseAddr	= 0x0010,	/* CSR4	- Transmit Descriptor list base address	*/
	DmaStatus	= 0x0014,	/* CSR5	- Dma status Register			*/
	DmaControl	= 0x0018,	/* CSR6	- Dma Operation Mode Register		*/
	DmaInterrupt	= 0x001C,	/* CSR7	- Interrupt enable			*/
	DmaMissedFr	= 0x0020,	/* CSR8	- Missed Frame & Buffer overflow Counter*/
	DmaTxCurrDesc	= 0x0048,	/*	- Current host Tx Desc Register		*/
	DmaRxCurrDesc	= 0x004C,	/*	- Current host Rx Desc Register		*/
	DmaTxCurrAddr	= 0x0050,	/* CSR20- Current host transmit buffer address	*/
	DmaRxCurrAddr	= 0x0054,	/* CSR21- Current host receive buffer address	*/
};

/**********************************************************
 * DMA Engine registers Layout
 **********************************************************/

/* DmaBusMode			= 0x0000,	CSR0 - Bus Mode */
enum DmaBusModeReg
{						/* Bit description					Bits	R/W	Reset value */
	DmaFixedBurstEnable	= 0x00010000,	/* (FB)Fixed Burst SINGLE, INCR4, INCR8 or INCR16	16	RW		*/
	DmaFixedBurstDisable	= 0x00000000,	/*		SINGLE, INCR				 0			*/
	DmaTxPriority		= 0x08000000,

	DmaTxPriorityRatio11	= 0x00000000,	/* (PR)TX:RX DMA priority ratio 1:1			15:14	RW	00	*/
	DmaTxPriorityRatio21	= 0x00004000,	/* (PR)TX:RX DMA priority ratio 2:1						*/
	DmaTxPriorityRatio31	= 0x00008000,	/* (PR)TX:RX DMA priority ratio 3:1						*/
	DmaTxPriorityRatio41	= 0x0000C000,	/* (PR)TX:RX DMA priority ratio 4:1						*/

	DmaBurstLengthx8	= 0x01000000,	/* When set mutiplies the PBL by 8			24	RW	 0	*/

	DmaBurstLength256	= 0x01002000,	/*(DmaBurstLengthx8 | DmaBurstLength32) = 256	[24]	13:8			*/
	DmaBurstLength128	= 0x01001000,	/*(DmaBurstLengthx8 | DmaBurstLength16) = 128	[24]	13:8			*/
	DmaBurstLength64	= 0x01000800,	/*(DmaBurstLengthx8 | DmaBurstLength8) = 64	[24]	13:8			*/
	DmaBurstLength32	= 0x00002000,	/* (PBL) programmable Dma burst length = 32		13:8	RW		*/
	DmaBurstLength16	= 0x00001000,	/* Dma burst length = 16							*/
	DmaBurstLength8		= 0x00000800,	/* Dma burst length = 8								*/
	DmaBurstLength4		= 0x00000400,	/* Dma burst length = 4								*/
	DmaBurstLength2		= 0x00000200,	/* Dma burst length = 2								*/
	DmaBurstLength1		= 0x00000100,	/* Dma burst length = 1								*/
	DmaBurstLength0		= 0x00000000,	/* Dma burst length = 0							00	*/

	DmaDescriptor8Words	= 0x00000080,	/* Enh Descriptor works 1 => 8 word descriptor		 7		0	*/
	DmaDescriptor4Words	= 0x00000000,	/* Enh Descriptor works 0 => 4 word descriptor		 7		0	*/

	DmaDescriptorSkip16	= 0x00000040,	/* (DSL)Descriptor skip length (no.of dwords)		6:2	RW		*/
	DmaDescriptorSkip8	= 0x00000020,	/* between two unchained descriptors						*/
	DmaDescriptorSkip4	= 0x00000010,
	DmaDescriptorSkip2	= 0x00000008,
	DmaDescriptorSkip1	= 0x00000004,
	DmaDescriptorSkip0	= 0x00000000,

	DmaArbitRr		= 0x00000000,	/* (DA) DMA RR arbitration				 1	RW	0	*/
	DmaArbitPr		= 0x00000002,	/* Rx has priority over Tx							*/

	DmaResetOn		= 0x00000001,	/* (SWR)Software Reset DMA engine			 0	RW		*/
	DmaResetOff		= 0x00000000,	/*							 0			*/
};

/* DmaStatus			= 0x0014,	CSR5 - Dma status Register */
enum DmaStatusReg
{
	/* Bit 28 27 and 26 indicate whether the interrupt due to PMT GMACMMC or GMAC LINE Remaining bits are DMA interrupts	*/
	GmacPmtIntr		= 0x10000000,	/* (GPI)Gmac subsystem interrupt			28	RO	0	*/
	GmacMmcIntr		= 0x08000000,	/* (GMI)Gmac MMC subsystem interrupt			27	RO	0	*/
	GmacLineIntfIntr	= 0x04000000,	/* Line interface interrupt				26	RO	0	*/

	DmaErrorBit2		= 0x02000000,	/* (EB)Error bits 0-data buffer, 1-desc. access		25	RO	0	*/
	DmaErrorBit1		= 0x01000000,	/* (EB)Error bits 0-write trnsf, 1-read transfr		24	RO	0	*/
	DmaErrorBit0		= 0x00800000,	/* (EB)Error bits 0-Rx DMA, 1-Tx DMA			23	RO	0	*/

	DmaTxState		= 0x00700000,	/* (TS)Transmit process state				22:20	RO		*/
	DmaTxStopped		= 0x00000000,	/* Stopped - Reset or Stop Tx Command issued				000	*/
	DmaTxFetching		= 0x00100000,	/* Running - fetching the Tx descriptor						*/
	DmaTxWaiting		= 0x00200000,	/* Running - waiting for status							*/
	DmaTxReading		= 0x00300000,	/* Running - reading the data from host memory					*/
	DmaTxSuspended		= 0x00600000,	/* Suspended - Tx Descriptor unavailabe						*/
	DmaTxClosing		= 0x00700000,	/* Running - closing Rx descriptor						*/

	DmaRxState		= 0x000E0000,	/* (RS)Receive process state				19:17	RO		*/
	DmaRxStopped		= 0x00000000,	/* Stopped - Reset or Stop Rx Command issued				000	*/
	DmaRxFetching		= 0x00020000,	/* Running - fetching the Rx descriptor						*/
	DmaRxWaiting		= 0x00060000,	/* Running - waiting for packet							*/
	DmaRxSuspended		= 0x00080000,	/* Suspended - Rx Descriptor unavailable					*/
	DmaRxClosing		= 0x000A0000,	/* Running - closing descriptor							*/
	DmaRxQueuing		= 0x000E0000,	/* Running - queuing the recieve frame into host memory				*/

	DmaIntNormal		= 0x00010000,	/* (NIS)Normal interrupt summary			16	RW	0	*/
	DmaIntAbnormal		= 0x00008000,	/* (AIS)Abnormal interrupt summary			15	RW	0	*/

	DmaIntEarlyRx		= 0x00004000,	/* Early receive interrupt (Normal)				RW	0	*/
	DmaIntBusError		= 0x00002000,	/* Fatal bus error (Abnormal)					RW	0	*/
	DmaIntEarlyTx		= 0x00000400,	/* Early transmit interrupt (Abnormal)				RW	0	*/
	DmaIntRxWdogTO		= 0x00000200,	/* Receive Watchdog Timeout (Abnormal)				RW	0	*/
	DmaIntRxStopped		= 0x00000100,	/* Receive process stopped (Abnormal)				RW	0	*/
	DmaIntRxNoBuffer	= 0x00000080,	/* Receive buffer unavailable (Abnormal)			RW	0	*/
	DmaIntRxCompleted	= 0x00000040,	/* Completion of frame reception (Normal)			RW	0	*/
	DmaIntTxUnderflow	= 0x00000020,	/* Transmit underflow (Abnormal)				RW	0	*/
	DmaIntRcvOverflow	= 0x00000010,	/* Receive Buffer overflow interrupt				RW	0	*/
	DmaIntTxJabberTO	= 0x00000008,	/* Transmit Jabber Timeout (Abnormal)				RW	0	*/
	DmaIntTxNoBuffer	= 0x00000004,	/* Transmit buffer unavailable (Normal)				RW	0	*/
	DmaIntTxStopped		= 0x00000002,	/* Transmit process stopped (Abnormal)				RW	0	*/
	DmaIntTxCompleted	= 0x00000001,	/* Transmit completed (Normal)					RW	0	*/
};

/* DmaControl			= 0x0018,	CSR6 - Dma Operation Mode Register */
enum DmaControlReg
{
	DmaDisableDropTcpCs	= 0x04000000,	/* (DT) Dis. drop. of tcp/ip CS error frames		26	RW	0	*/

	DmaStoreAndForward	= 0x00200000,	/* (SF)Store and forward				21	RW	0	*/
	DmaFlushTxFifo		= 0x00100000,	/* (FTF)Tx FIFO controller is reset to default		20	RW	0	*/

	DmaTxThreshCtrl		= 0x0001C000,	/* (TTC)Controls thre Threh of MTL tx Fifo		16:14	RW		*/
	DmaTxThreshCtrl16	= 0x0001C000,	/* (TTC)Controls thre Threh of MTL tx Fifo 16		16:14	RW		*/
	DmaTxThreshCtrl24	= 0x00018000,	/* (TTC)Controls thre Threh of MTL tx Fifo 24		16:14	RW		*/
	DmaTxThreshCtrl32	= 0x00014000,	/* (TTC)Controls thre Threh of MTL tx Fifo 32		16:14	RW		*/
	DmaTxThreshCtrl40	= 0x00010000,	/* (TTC)Controls thre Threh of MTL tx Fifo 40		16:14	RW		*/
	DmaTxThreshCtrl256	= 0x0000c000,	/* (TTC)Controls thre Threh of MTL tx Fifo 256		16:14	RW		*/
	DmaTxThreshCtrl192	= 0x00008000,	/* (TTC)Controls thre Threh of MTL tx Fifo 192		16:14	RW		*/
	DmaTxThreshCtrl128	= 0x00004000,	/* (TTC)Controls thre Threh of MTL tx Fifo 128		16:14	RW		*/
	DmaTxThreshCtrl64	= 0x00000000,	/* (TTC)Controls thre Threh of MTL tx Fifo 64		16:14	RW	000	*/

	DmaTxStart		= 0x00002000,	/* (ST)Start/Stop transmission				13	RW	0	*/

	DmaRxFlowCtrlDeact	= 0x00401800,	/* (RFD)Rx flow control deact. threhold		  [22]	12:11	RW		*/
	DmaRxFlowCtrlDeact1K	= 0x00000000,	/* (RFD)Rx flow control deact. threhold (1kbytes) [22]	12:11	RW	00	*/
	DmaRxFlowCtrlDeact2K	= 0x00000800,	/* (RFD)Rx flow control deact. threhold (2kbytes) [22]	12:11	RW		*/
	DmaRxFlowCtrlDeact3K	= 0x00001000,	/* (RFD)Rx flow control deact. threhold (3kbytes) [22]	12:11	RW		*/
	DmaRxFlowCtrlDeact4K	= 0x00001800,	/* (RFD)Rx flow control deact. threhold (4kbytes) [22]	12:11	RW		*/
	DmaRxFlowCtrlDeact5K	= 0x00400000,	/* (RFD)Rx flow control deact. threhold (4kbytes) [22]	12:11	RW		*/
	DmaRxFlowCtrlDeact6K	= 0x00400800,	/* (RFD)Rx flow control deact. threhold (4kbytes) [22]	12:11	RW		*/
	DmaRxFlowCtrlDeact7K	= 0x00401000,	/* (RFD)Rx flow control deact. threhold (4kbytes) [22]	12:11	RW		*/

	DmaRxFlowCtrlAct	= 0x00800600,	/* (RFA)Rx flow control Act. threshold		  [23]	10:09	RW		*/
	DmaRxFlowCtrlAct1K	= 0x00000000,	/* (RFA)Rx flow control Act. threshold (1kbytes)  [23]	10:09	RW	00	*/
	DmaRxFlowCtrlAct2K	= 0x00000200,	/* (RFA)Rx flow control Act. threshold (2kbytes)  [23]	10:09	RW		*/
	DmaRxFlowCtrlAct3K	= 0x00000400,	/* (RFA)Rx flow control Act. threshold (3kbytes)  [23]	10:09	RW		*/
	DmaRxFlowCtrlAct4K	= 0x00000600,	/* (RFA)Rx flow control Act. threshold (4kbytes)  [23]	10:09	RW		*/
	DmaRxFlowCtrlAct5K	= 0x00800000,	/* (RFA)Rx flow control Act. threshold (5kbytes)  [23]	10:09	RW		*/
	DmaRxFlowCtrlAct6K	= 0x00800200,	/* (RFA)Rx flow control Act. threshold (6kbytes)  [23]	10:09	RW		*/
	DmaRxFlowCtrlAct7K	= 0x00800400,	/* (RFA)Rx flow control Act. threshold (7kbytes)  [23]	10:09	RW		*/

	DmaRxThreshCtrl		= 0x00000018,	/* (RTC)Controls thre Threh of MTL rx Fifo		4:3	RW		*/
	DmaRxThreshCtrl64	= 0x00000000,	/* (RTC)Controls thre Threh of MTL tx Fifo 64		4:3	RW		*/
	DmaRxThreshCtrl32	= 0x00000008,	/* (RTC)Controls thre Threh of MTL tx Fifo 32		4:3	RW		*/
	DmaRxThreshCtrl96	= 0x00000010,	/* (RTC)Controls thre Threh of MTL tx Fifo 96		4:3	RW		*/
	DmaRxThreshCtrl128	= 0x00000018,	/* (RTC)Controls thre Threh of MTL tx Fifo 128		4:3	RW		*/

	DmaEnHwFlowCtrl		= 0x00000100,	/* (EFC)Enable HW flow control				8	RW		*/
	DmaDisHwFlowCtrl	= 0x00000000,	/* Disable HW flow control						0	*/

	DmaFwdErrorFrames	= 0x00000080,	/* (FEF)Forward error frames				7	RW	0	*/
	DmaFwdUnderSzFrames	= 0x00000040,	/* (FUF)Forward undersize frames			6	RW	0	*/
	DmaTxSecondFrame	= 0x00000004,	/* (OSF)Operate on second frame				4	RW	0	*/
	DmaRxStart		= 0x00000002,	/* (SR)Start/Stop reception				1	RW	0	*/
};

/* GmacFlowControl		= 0x0018,	Flow control Register Layout */
enum GmacFlowControlReg
{
	GmacPauseTimeMask	= 0xFFFF0000,	/* (PT) PAUSE TIME field in the control frame	31:16	RW	0000	*/
	GmacPauseTimeShift	= 16,
	DmaDisableFlushRxFrames = 0x01000000,

	GmacPauseLowThresh	= 0x00000030,
	GmacPauseLowThresh3	= 0x00000030,	/* (PLT)thresh for pause tmr 256 slot time	5:4	RW		*/
	GmacPauseLowThresh2	= 0x00000020,	/*			     144 slot time				*/
	GmacPauseLowThresh1	= 0x00000010,	/*			      28 slot time				*/
	GmacPauseLowThresh0	= 0x00000000,	/*			       4 slot time			000	*/

	GmacUnicastPauseFrame	= 0x00000008,
	GmacUnicastPauseFrameOn	= 0x00000008,	/* (UP)Detect pause frame with unicast addr.	3	RW		*/
	GmacUnicastPauseFrameOff = 0x00000000,	/* Detect only pause frame with multicast addr.			0	*/

	GmacRxFlowControl	= 0x00000004,
	GmacRxFlowControlEnable	= 0x00000004,	/* (RFE)Enable Rx flow control			2	RW		*/
	GmacRxFlowControlDisable = 0x00000000,	/* Disable Rx flow control					0	*/

	GmacTxFlowControl	= 0x00000002,
	GmacTxFlowControlEnable	= 0x00000002,	/* (TFE)Enable Tx flow control			1	RW		*/
	GmacTxFlowControlDisable = 0x00000000,	/* Disable flow control						0	*/

	GmacFlowControlBackPressure = 0x00000001,
	GmacSendPauseFrame	= 0x00000001,	/* (FCB/PBA)send pause frm/Apply back pressure	0	RW	0	*/
};
#endif	/* _IPQ_GMAC_H */
