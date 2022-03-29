/* * Copyright (c) 2012 - 2013 The Linux Foundation. All rights reserved.* */

#ifndef __ASM_ARCH_MSM_NSS_GMAC_H
#define __ASM_ARCH_MSM_NSS_GMAC_H

/* NSS GMAC Base Addresses */
#define NSS_GMAC0_BASE                  0x37000000
#define NSS_GMAC1_BASE                  0x37200000
#define NSS_GMAC2_BASE                  0x37400000
#define NSS_GMAC3_BASE                  0x37600000
#define NSS_GMAC_REG_LEN                0x00200000

/* NSS GMAC Specific defines */
#define NSS_REG_BASE                    0x03000000
#define NSS_REG_LEN                     0x0000FFFF

#define MSM_TCSR_BASE                   0x1A400000

/* TCSR offsets */
#define TCSR_PXO_SEL                    0x00C0

/* Offsets of NSS config and status registers within NSS_REG_BASE */
/* We start the GMAC numbering from 0 */
#define NSS_CSR_REV                     0x0000
#define NSS_CSR_CFG                     0x0004
#define NSS_ETH_CLK_GATE_CTL            0x0008
#define NSS_ETH_CLK_DIV0                0x000C
#define NSS_ETH_CLK_DIV1                0x0010
#define NSS_ETH_CLK_SRC_CTL             0x0014
#define NSS_ETH_CLK_INV_CTL             0x0018
#define NSS_MACSEC_CTL                  0x0028
#define NSS_QSGMII_CLK_CTL              0x002C
#define NSS_GMAC0_CTL                   0x0030
#define NSS_GMAC1_CTL                   0x0034
#define NSS_GMAC2_CTL                   0x0038
#define NSS_GMAC3_CTL                   0x003C
#define NSS_ETH_CLK_ROOT_STAT           0x0040
#define NSS_QSGMII_STAT                 0x0044
#define NSS_ETH_SPARE_CTL               0x0088
#define NSS_ETH_SPARE_STAT              0x008C
#define NSS_PCS_QSGMII_BERT_THLD_CTL    0x00C0
#define NSS_PCS_CAL_LCKDT_CTL           0x0120
#define NSS_QSGMII_PHY_QSGMII_CTL       0x0134
#define NSS_QSGMII_PHY_SERDES_CTL       0x0144
#define NSS_PCS_MODE_CTL                0x0068

/* Offsets of MSM_CLK_CTL_BASE */
#define MSM_CLK_CTL_BASE                0x00900000
/* NSS_QSGMII_PHY_QSGMII_CTL bits */
#define QSGMII_TX_AMPLITUDE_600mV       (0xC << 28)
#define QSGMII_TX_SLC_10                (0x2 << 26)
#define QSGMII_THRESHOLD_DEFAULT        (0x0 << 24)
#define QSGMII_SLEW_RATE_DEFAULT        (0x2 << 22)
#define QSGMII_RX_EQUALIZER_DEFAULT     (0x1 << 20)
#define QSGMII_RX_DC_BIAS_DEFAULT       (0x2 << 18)
#define QSGMII_PHASE_LOOP_GAIN_DEFAULT  (0x2 << 12)
#define QSGMII_TX_DE_EMPHASIS_DEFAULT   (0x2 << 10)
#define QSGMII_ENABLE                   (0x1 << 7)
#define QSGMII_ENABLE_TX                (0x1 << 3)
#define QSGMII_ENABLE_SD                (0x1 << 2)
#define QSGMII_ENABLE_RX                (0x1 << 1)
#define QSGMII_ENABLE_CDR               (0x1 << 0)

/* NSS_QSGMII_PHY_SERDES_CTL bits */
#define PLL_PUMP_CURRENT_600uA          (0x6 << 28)
#define PLL_TANK_CURRENT_7mA            (0x2 << 24)
#define PCIE_MAX_POWER_MODE             (0x1 << 20)
#define PLL_LOOP_FILTER_RESISTOR_DEFAULT (0x4 << 4)
#define PLL_ENABLE                      (0x1 << 2)
#define SERDES_ENABLE_LCKDT             (0x1 << 1)

/* NSS_PCS_CAL_LCKDT_CTL bits */
#define LCKDT_RST_n                     (0x1 << 19)

/* Macros to calculate register offsets */
#define NSS_GMACn_CTL(n)                (NSS_GMAC0_CTL +  (n * 4))
#define NSS_ETH_CLK_CTLn(x)             (NSS_ETH_CLK_CTL0 +  (x * 4))


/* NSS_ETH_CLK_GATE_CTL bits */
#define MACSEC3_CORE_CLK                (1 << 30)
#define MACSEC2_CORE_CLK                (1 << 29)
#define MACSEC1_CORE_CLK                (1 << 28)
#define GMAC0_PTP_CLK                   (1 << 16)
#define GMAC0_RGMII_RX_CLK              (1 << 9)
#define GMAC0_RGMII_TX_CLK              (1 << 8)
#define GMAC0_GMII_RX_CLK               (1 << 4)
#define GMAC0_GMII_TX_CLK               (1 << 0)


#define GMAC0_RGMII_TX_CLK_SHIFT                8
#define GMAC0_RGMII_RX_CLK_SHIFT                9
#define GMAC0_GMII_RX_CLK_SHIFT                 4
#define GMAC0_GMII_TX_CLK_SHIFT                 0
#define GMAC0_PTP_CLK_SHIFT                     16

/* Macros to calculate bit offsets in NSS_ETH_CLK_GATE_CTL register
 * MACSEC_CORE_CLK: x = 1,2,3
 * GMII_xx_CLK: x = 0,1,2,3
 * RGMII_xx_CLK: x = 0,1
 * PTP_CLK: x = 0,1,2,3
*/
#define MACSECn_CORE_CLK(x)             (1 << (MACSEC1_CORE_CLK + x))
#define GMACn_GMII_TX_CLK(x)            (1 << (GMAC0_GMII_TX_CLK_SHIFT + x))
#define GMACn_GMII_RX_CLK(x)            (1 << (GMAC0_GMII_RX_CLK_SHIFT + x))
#define GMACn_RGMII_TX_CLK(x)           (1 << (GMAC0_RGMII_TX_CLK_SHIFT + (x * 2)))
#define GMACn_RGMII_RX_CLK(x)           (1 << (GMAC0_RGMII_RX_CLK_SHIFT + (x * 2)))
#define GMACn_PTP_CLK(x)                (1 << (GMAC0_PTP_CLK_SHIFT + x))

/* NSS_ETH_CLK_DIV0 bits ; n = 0,1,2,3 */
#define RGMII_CLK_DIV_1000                      1
#define RGMII_CLK_DIV_100                       9
#define RGMII_CLK_DIV_10                        100
#define SGMII_CLK_DIV_1000                      1
#define SGMII_CLK_DIV_100                       5
#define SGMII_CLK_DIV_10                        50
#define QSGMII_CLK_DIV_1000                     2
#define QSGMII_CLK_DIV_100                      10
#define QSGMII_CLK_DIV_10                       100
#define GMACn_CLK_DIV_SIZE                      0x7F
#define GMACn_CLK_DIV(n,val)                    (val << (n * 8))


/* NSS_ETH_CLK_SRC_CTL bits */
#define GMAC0_GMII_CLK_RGMII                    (1 << 0)
#define GMAC1_GMII_CLK_RGMII                    (1 << 1)

/* Macros to calculate bit offsets in NSS_ETH_CLK_CTL3 register */
#define GMACn_GMII_CLK_RGMII(x)                 (1 << x)

/* NSS_QSGMII_CLK_CTL bits */
#define GMAC0_TX_CLK_HALT                       (1 << 7)
#define GMAC0_RX_CLK_HALT                       (1 << 8)
#define GMAC1_TX_CLK_HALT                       (1 << 9)
#define GMAC1_RX_CLK_HALT                       (1 << 10)
#define GMAC2_TX_CLK_HALT                       (1 << 11)
#define GMAC2_RX_CLK_HALT                       (1 << 12)
#define GMAC3_TX_CLK_HALT                       (1 << 13)
#define GMAC3_RX_CLK_HALT                       (1 << 14)
#define NSS_QSGMII_CLK_CTL_CLR_MSK		0

#define GMAC0_QSGMII_TX_CLK_SHIFT               7
#define GMAC0_QSGMII_RX_CLK_SHIFT               8

/* Macros to calculate bit offsets in NSS_QSGMII_CLK_CTL register */
#define GMACn_QSGMII_TX_CLK(n)          (1 << (GMAC0_QSGMII_TX_CLK_SHIFT + (n * 2)))
#define GMACn_QSGMII_RX_CLK(n)          (1 << (GMAC0_QSGMII_RX_CLK_SHIFT + (n * 2)))

/* NSS_GMACn_CTL bits */
#define GMAC_IFG_CTL(x)                         (x)
#define GMAC_IFG_LIMIT(x)                       (x << 8)
#define GMAC_PHY_RGMII                          (1 << 16)
#define GMAC_PHY_QSGMII                         (0 << 16)
#define GMAC_FLOW_CTL                           (1 << 18)
#define GMAC_CSYS_REQ                           (1 << 19)
#define GMAC_PTP_TRIG                           (1 << 20)

/* GMAC min Inter Frame Gap values */
#define GMAC_IFG                                12
#define GMAC_IFG_MIN_1000                       10
#define GMAC_IFG_MIN_HALF_DUPLEX                8

/*
 * GMAC min Inter Frame Gap Limits.
 * In full duplex mode set to same value as IFG
*/
#define GMAC_IFG_LIMIT_HALF                     12

/* QSGMII Specific defines */
#define QSGMII_REG_BASE                         0x1bb00000
#define QSGMII_REG_LEN                          0x0000FFFF

/* QSGMII Register offsets */
#define PCS_QSGMII_CTL                          0x020
#define PCS_QSGMII_SGMII_MODE                   0x064
#define PCS_ALL_CH_CTL                          0x080
#define PCS_CAL_LCKDT_CTL                       0x120
#define QSGMII_PHY_MODE_CTL                     0x128
#define QSGMII_PHY_QSGMII_CTL			0x134
#define PCS_QSGMII_MAC_STAT			0x74

/* Bit definitions for PCS_QSGMII_SGMII_MODE */
#define PCS_QSGMII_MODE_SGMII			(0x0 << 0)
#define PCS_QSGMII_MODE_QSGMII			(0x1 << 0)

/* Bit definitions for PCS_MODE_CTL */
#define PCS_MODE_CTL_SGMII_MAC_CLR              (0 << 2)
#define PCS_MODE_CTL_SGMII_PHY_CLR              (0 << 1)
#define PCS_MODE_CTL_SGMII_MAC_SET              (1 << 2)
#define PCS_MODE_CTL_SGMII_PHY_SET              (1 << 1)

/* Bit definitions for QSGMII_PHY_MODE_CTL */
#define QSGMII_PHY_MODE_SGMII			(0x0 << 0)
#define QSGMII_PHY_MODE_QSGMII			(0x1 << 0)

/* Bit definitions for QSGMII_PHY_QSGMII_CTL */
#define QSGMII_TX_SLC_CTL(x)			(x << 26)

/* Interface between GMAC and PHY */
#define GMAC_INTF_RGMII				0
#define GMAC_INTF_SGMII				1
#define GMAC_INTF_QSGMII			2

/* GMAC phy interface selection */
//#ifdef CONFIG_ARCH_IPQ806X
#if defined(CONFIG_IPQ_GMAC_PHY_PROFILE_2R_2S)
#define NSS_GMAC0_MII_MODE              GMAC_INTF_RGMII
#define NSS_GMAC1_MII_MODE              GMAC_INTF_RGMII
#define NSS_GMAC2_MII_MODE              GMAC_INTF_SGMII
#define NSS_GMAC3_MII_MODE              GMAC_INTF_SGMII
#elif defined (CONFIG_IPQ_GMAC_PHY_PROFILE_1R_3S)
#define NSS_GMAC0_MII_MODE              GMAC_INTF_RGMII
#define NSS_GMAC1_MII_MODE              GMAC_INTF_SGMII
#define NSS_GMAC2_MII_MODE              GMAC_INTF_SGMII
#define NSS_GMAC3_MII_MODE              GMAC_INTF_SGMII
#elif defined (CONFIG_IPQ_GMAC_PHY_PROFILE_QS)
#define NSS_GMAC0_MII_MODE              GMAC_INTF_QSGMII
#define NSS_GMAC1_MII_MODE              GMAC_INTF_QSGMII
#define NSS_GMAC2_MII_MODE              GMAC_INTF_QSGMII
#define NSS_GMAC3_MII_MODE              GMAC_INTF_QSGMII
#else
#define NSS_GMAC0_MII_MODE              GMAC_INTF_RGMII
#define NSS_GMAC1_MII_MODE              GMAC_INTF_RGMII
#define NSS_GMAC2_MII_MODE              GMAC_INTF_SGMII
#define NSS_GMAC3_MII_MODE              GMAC_INTF_SGMII
#endif
//#endif /* CONFIG_ARCH_IPQ806X */

struct nss_gmac_platform_data {
	uint32_t phy_mdio_addr;                 /* MDIO address of the connected PHY */
	uint32_t poll_required;                 /* [0/1] Link status poll? */
	uint32_t rgmii_delay;
};

/* Macros to calculate register offsets */

#define NSS_MAX_GMACS                           4

/* public functions */
extern uchar ipq_def_enetaddr[];
void ipq_gmac_common_init(ipq_gmac_board_cfg_t *cfg);
int ipq_gmac_init(ipq_gmac_board_cfg_t *cfg);
int get_eth_mac_address(uchar *enetaddr, uint no_of_macs);
int ipq_qca8511_init(ipq_gmac_board_cfg_t *gmac_cfg);
int ipq_athrs17_init(ipq_gmac_board_cfg_t *gmac_cfg);
void ipq_register_switch(int(*sw_init)(ipq_gmac_board_cfg_t *cfg));
#endif /*__ASM_ARCH_MSM_NSS_GMAC_H */
