/*
 * Copyright (c) 2015, 2016 The Linux Foundation. All rights reserved.

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

#include <common.h>
#include <net.h>
#include <asm-generic/errno.h>
#include <asm/io.h>

#include "ipq40xx_ess_sw.h"
#include "ipq40xx_cdp.h"

extern board_ipq40xx_params_t *gboard_param;
static inline void ipq40xx_ess_sw_rd(u32 addr, u32 * data)
{
	*data = readl((void __iomem *)(IPQ40XX_NSS_BASE + addr));
}

static inline void ipq40xx_ess_sw_wr(u32 addr, u32 data)
{
	writel(data, ((void __iomem *)(IPQ40XX_NSS_BASE + addr)));
}

void ipq40xx_ess_disable_lookup(void)
{
	ipq40xx_ess_sw_wr(S17_P0LOOKUP_CTRL_REG, 0x140000);
	ipq40xx_ess_sw_wr(S17_P1LOOKUP_CTRL_REG, 0x14001c);
	ipq40xx_ess_sw_wr(S17_P2LOOKUP_CTRL_REG, 0x14001a);
	ipq40xx_ess_sw_wr(S17_P3LOOKUP_CTRL_REG, 0x140016);
	ipq40xx_ess_sw_wr(S17_P4LOOKUP_CTRL_REG, 0x14001e);
	ipq40xx_ess_sw_wr(S17_P5LOOKUP_CTRL_REG, 0x140000);
	ipq40xx_ess_sw_wr(S17_GLOFW_CTRL1_REG, 0x3e3e3e);
}

void ipq40xx_ess_enable_lookup(void)
{
	ipq40xx_ess_sw_wr(S17_P0LOOKUP_CTRL_REG, 0x14003e);
	ipq40xx_ess_sw_wr(S17_P1LOOKUP_CTRL_REG, 0x14001d);
	ipq40xx_ess_sw_wr(S17_P2LOOKUP_CTRL_REG, 0x14001b);
	ipq40xx_ess_sw_wr(S17_P3LOOKUP_CTRL_REG, 0x140017);
	ipq40xx_ess_sw_wr(S17_P4LOOKUP_CTRL_REG, 0x14000f);
	ipq40xx_ess_sw_wr(S17_P5LOOKUP_CTRL_REG, 0x140001);
	ipq40xx_ess_sw_wr(S17_GLOFW_CTRL1_REG, 0x3f3f3f);
}

int ipq40xx_ess_sw_init(ipq40xx_edma_board_cfg_t *cfg)
{
	u32 data;

	ipq40xx_ess_sw_wr(S17_GLOFW_CTRL1_REG, 0x3e3e3e);
	/*
	 * configure Speed, Duplex.
	 */
	ipq40xx_ess_sw_wr(S17_P0STATUS_REG, S17_PORT_SPEED(2) |
					S17_PORT_FULL_DUP |
					S17_TX_FLOW_EN |
					S17_RX_FLOW_EN);

	switch(gboard_param->machid) {
	case MACH_TYPE_IPQ40XX_AP_DK01_1_S1:
	case MACH_TYPE_IPQ40XX_AP_DK01_1_C1:
	case MACH_TYPE_IPQ40XX_AP_DK01_1_C2:
	case MACH_TYPE_IPQ40XX_AP_DK04_1_C1:
	case MACH_TYPE_IPQ40XX_AP_DK04_1_C4:
	case MACH_TYPE_IPQ40XX_AP_DK04_1_C2:
	case MACH_TYPE_IPQ40XX_AP_DK04_1_C3:
	case MACH_TYPE_IPQ40XX_AP_DK04_1_C5:
	case MACH_TYPE_IPQ40XX_AP_DK05_1_C1:
	case MACH_TYPE_IPQ40XX_AP_DK06_1_C1:
	case MACH_TYPE_IPQ40XX_AP_DK07_1_C1:
	case MACH_TYPE_IPQ40XX_AP_DK07_1_C2:
	case MACH_TYPE_IPQ40XX_AP_DK07_1_C3:

		ipq40xx_ess_sw_wr(S17_P0LOOKUP_CTRL_REG, 0x140000);
		ipq40xx_ess_sw_wr(S17_P1LOOKUP_CTRL_REG, 0x140000);
		ipq40xx_ess_sw_wr(S17_P2LOOKUP_CTRL_REG, 0x140000);
		ipq40xx_ess_sw_wr(S17_P3LOOKUP_CTRL_REG, 0x140000);
		ipq40xx_ess_sw_wr(S17_P4LOOKUP_CTRL_REG, 0x140000);
		ipq40xx_ess_sw_wr(S17_P5LOOKUP_CTRL_REG, 0x140000);
		/*
		 * HOL setting for Port0
		 */
		ipq40xx_ess_sw_wr(S17_PORT0_HOL_CTRL0, 0x1e444444);
		ipq40xx_ess_sw_wr(S17_PORT0_HOL_CTRL1, 0x1c6);
		/*
		 * HOL setting for Port1
		 */
		ipq40xx_ess_sw_wr(S17_PORT1_HOL_CTRL0, 0x1e004444);
		ipq40xx_ess_sw_wr(S17_PORT1_HOL_CTRL1, 0x1c6);
		/*
		 * HOL setting for Port2
		 */
		ipq40xx_ess_sw_wr(S17_PORT2_HOL_CTRL0, 0x1e004444);
		ipq40xx_ess_sw_wr(S17_PORT2_HOL_CTRL1, 0x1c6);
		/*
		 * HOL setting for Port3
		 */
		ipq40xx_ess_sw_wr(S17_PORT3_HOL_CTRL0, 0x1e004444);
		ipq40xx_ess_sw_wr(S17_PORT3_HOL_CTRL1, 0x1c6);
		/*
		 * HOL setting for Port4
		 */
		ipq40xx_ess_sw_wr(S17_PORT4_HOL_CTRL0, 0x1e004444);
		ipq40xx_ess_sw_wr(S17_PORT4_HOL_CTRL1, 0x1c6);
		/*
		 * HOL setting for Port5
		 */
		ipq40xx_ess_sw_wr(S17_PORT5_HOL_CTRL0, 0x1e444444);
		ipq40xx_ess_sw_wr(S17_PORT5_HOL_CTRL1, 0x1c6);
		break;
	case MACH_TYPE_IPQ40XX_DB_DK02_1_C1:
	case MACH_TYPE_IPQ40XX_DB_DK01_1_C1:
		ipq40xx_ess_sw_wr(S17_P4STATUS_REG, S17_PORT_SPEED(2) |
						S17_PORT_FULL_DUP |
						S17_TX_FLOW_EN |
						S17_RX_FLOW_EN |
						S17_PORT_TX_MAC_EN |
						S17_PORT_RX_MAC_EN);
		ipq40xx_ess_sw_wr(S17_P5STATUS_REG, S17_PORT_SPEED(2) |
						S17_PORT_FULL_DUP |
						S17_TX_FLOW_EN |
						S17_RX_FLOW_EN |
						S17_PORT_TX_MAC_EN |
						S17_PORT_RX_MAC_EN);
		ipq40xx_ess_sw_wr(ESS_MIB_REG, 0x100000);
		ipq40xx_ess_sw_wr(S17_P4LOOKUP_CTRL_REG, 0x34006f);;
		break;
	default:
		printf("ess cfg not supported for %x machid\n",
						gboard_param->machid);
		return -1;
	}
	mdelay(1);
	/*
	 * Enable Rx and Tx mac.
	 */
	ipq40xx_ess_sw_rd(S17_P0STATUS_REG, &data);
	ipq40xx_ess_sw_wr(S17_P0STATUS_REG, data |
				S17_PORT_TX_MAC_EN |
				S17_PORT_RX_MAC_EN);
	ipq40xx_ess_sw_rd(ESS_MIB_OFFSET, &data);
	ipq40xx_ess_sw_wr(ESS_MIB_OFFSET, data |
					ESS_MIB_EN);
	ipq40xx_ess_sw_wr(S17_GLOFW_CTRL1_REG, 0x7f7f7f);

	return 0;
}

#ifdef CONFIG_ESS_MIB_EN
static void ess_dump_stats(struct ess_rx_stats *rx,
			struct ess_tx_stats *tx, u32 port)
{
	/*
	 * Tx stats
	 */
	printf ("########tx port: %d stats ########\n", port);
	printf("port: %d tx_broadcast: %d\n", port, tx->tx_broad);
	printf("port: %d tx_pause: %d\n", port, tx->tx_pause);
	printf("port: %d tx_multi: %d\n", port, tx->tx_multi);
	printf("port: %d tx_underrun: %d\n", port, tx->tx_underrun);
	printf("port: %d tx_64 byte: %d\n", port, tx->tx_64b);
	printf("port: %d tx_128 byte: %d\n", port, tx->tx_128b);
	printf("port: %d tx_256 byte: %d\n", port, tx->tx_256b);
	printf("port: %d tx_512 byte: %d\n", port, tx->tx_512b);
	printf("port: %d tx_1024 byte: %d\n", port, tx->tx_1024b);
	printf("port: %d tx_1518 byte: %d\n", port, tx->tx_1518b);
	printf("port: %d tx_max byte: %d\n", port, tx->tx_maxb);
	printf("port: %d tx_oversize: %d\n", port, tx->tx_oversiz);
	printf("port: %d tx_byte_lo: %d\n", port, tx->tx_bytel);
	printf("port: %d tx_byte_hi: %d\n", port, tx->tx_byteh);
	printf("port: %d tx_collision: %d\n", port, tx->tx_collision);
	printf("port: %d tx_abort_col: %d\n", port, tx->tx_abortcol);
	printf("port: %d tx_multi_col: %d\n", port, tx->tx_multicol);
	printf("port: %d tx_singla_col: %d\n", port, tx->tx_singalcol);
	printf("port: %d tx_exec_defer: %d\n", port, tx->tx_execdefer);
	printf("port: %d tx_defer: %d\n", port, tx->tx_defer);
	printf("port: %d tx_late_col: %d\n", port, tx->tx_latecol);
	printf("port: %d tx_unicast: %d\n", port, tx->tx_unicast);
	/*
	 * rx stats
	 */
	printf ("########rx port: %d stats ########\n\n", port);
	printf("port: %d rx_broadcast: %d\n", port, rx->rx_broad);
	printf("port: %d rx_pause: %d\n", port, rx->rx_pause);
	printf("port: %d rx_multi: %d\n", port, rx->rx_multi);
	printf("port: %d rx_fcserr: %d\n", port, rx->rx_fcserr);
	printf("port: %d rx_allignerr: %d\n", port, rx->rx_allignerr);
	printf("port: %d rx_runt: %d\n", port, rx->rx_runt);
	printf("port: %d rx_frag: %d\n", port, rx->rx_frag);
	printf("port: %d rx_64 byte: %d\n", port, rx->rx_64b);
	printf("port: %d rx_128 byte: %d\n", port, rx->rx_128b);
	printf("port: %d rx_256 byte: %d\n", port, rx->rx_256b);
	printf("port: %d rx_512 byte: %d\n", port, rx->rx_512b);
	printf("port: %d rx_1024 byte: %d\n", port, rx->rx_1024b);
	printf("port: %d rx_1518 byte: %d\n", port, rx->rx_1518b);
	printf("port: %d rx_max byte: %d\n", port, rx->rx_maxb);
	printf("port: %d rx_too long: %d\n", port, rx->rx_tool);
	printf("port: %d rx_good byte lo: %d\n", port, rx->rx_goodbl);
	printf("port: %d rx_good byte hi: %d\n", port, rx->rx_goodbh);
	printf("port: %d rx overflow: %d\n", port, rx->rx_overflow);
	printf("port: %d rx_bad lo: %d\n", port, rx->rx_badbl);
	printf("port: %d rx_bad hi: %d\n", port, rx->rx_badbu);
	printf("port: %d rx unicast: %d\n", port, rx->rx_unicast);
}

static int ipq40xx_ess_stats(cmd_tbl_t *cmdtp, int flag,
				int argc, char *const argv[])
{
	struct ess_rx_stats rx_mib;
	struct ess_tx_stats tx_mib;
	u32 portno;
	unsigned long timebase;

	if (argc != 2)
	        return CMD_RET_USAGE;

	portno = simple_strtoul(argv[1], NULL, 16);

	if (portno > 6)
	        return CMD_RET_USAGE;
	/*
	 * Tx stats
	 */
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_BROAD_REG), &tx_mib.tx_broad);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_PAUSE_REG), &tx_mib.tx_pause);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_MULTI_REG), &tx_mib.tx_multi);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_MULTI_REG), &tx_mib.tx_underrun);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_64B_REG), &tx_mib.tx_64b);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_128B_REG), &tx_mib.tx_128b);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_256B_REG), &tx_mib.tx_256b);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_512B_REG), &tx_mib.tx_512b);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_1024B_REG), &tx_mib.tx_1024b);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_1518B_REG), &tx_mib.tx_1518b);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_MAXB_REG), &tx_mib.tx_maxb);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_OVSIZE_REG), &tx_mib.tx_oversiz);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_TXBYTEL_REG), &tx_mib.tx_bytel);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_TXBYTEU_REG), &tx_mib.tx_byteh);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_COLL_REG), &tx_mib.tx_collision);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_ABTCOLL_REG), &tx_mib.tx_abortcol);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_MLTCOL_REG), &tx_mib.tx_multicol);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_SINGCOL_REG), &tx_mib.tx_singalcol);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_EXDF_REG), &tx_mib.tx_execdefer);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_DEF_REG), &tx_mib.tx_defer);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_LATECOL_REG), &tx_mib.tx_latecol);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_TX_UNICAST), &tx_mib.tx_unicast);
	/*
	 * Rx stats
	 */

	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_BROAD_REG), &rx_mib.rx_broad);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_PAUSE_REG), &rx_mib.rx_pause);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_MULTI_REG), &rx_mib.rx_multi);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_FCSERR_REG), &rx_mib.rx_fcserr);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_ALIGNERR_REG), &rx_mib.rx_allignerr);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_RUNT_REG), &rx_mib.rx_runt);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_FRAGMENT_REG), &rx_mib.rx_frag);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_RUNT_REG), &rx_mib.rx_64b);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_128B_REG), &rx_mib.rx_128b);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_phy_RX_256B_REG), &rx_mib.rx_256b);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_512B_REG), &rx_mib.rx_512b);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_1024B_REG), &rx_mib.rx_1024b);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_1518B_REG), &rx_mib.rx_1518b);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_MAXB_REG), &rx_mib.rx_maxb);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_TOLO_REG), &rx_mib.rx_tool);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_GOODBL_REG), &rx_mib.rx_goodbl);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_GOODBU_REG), &rx_mib.rx_goodbh);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_OVERFLW_REG), rx_mib.rx_overflow);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_BADBL_REG), &rx_mib.rx_badbl);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_BADBL_REG), &rx_mib.rx_badbu);
	ipq40xx_ess_sw_rd((ess_mib(portno) |
			ESS_PHY_RX_UNICAST), &rx_mib.rx_unicast);
	/*
	 * Dump stats
	 */
	ess_dump_stats(&rx_mib, &tx_mib, portno);
	return 0;
}
U_BOOT_CMD(essstats, 2, 0, ipq40xx_ess_stats,
	"get ess mib stats for the given ess port",
	"essstats <0 - 5>");
#endif

