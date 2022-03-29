/*
 * sh_eth.c - Driver for Renesas SH7763's ethernet controler.
 *
 * Copyright (C) 2008, 2011 Renesas Solutions Corp.
 * Copyright (c) 2008, 2011 Nobuhiro Iwamatsu
 * Copyright (c) 2007 Carlos Munoz <carlos@kenati.com>
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

#include <config.h>
#include <common.h>
#include <malloc.h>
#include <net.h>
#include <netdev.h>
#include <miiphy.h>
#include <asm/errno.h>
#include <asm/io.h>

#include "sh_eth.h"

#ifndef CONFIG_SH_ETHER_USE_PORT
# error "Please define CONFIG_SH_ETHER_USE_PORT"
#endif
#ifndef CONFIG_SH_ETHER_PHY_ADDR
# error "Please define CONFIG_SH_ETHER_PHY_ADDR"
#endif
#ifdef CONFIG_SH_ETHER_CACHE_WRITEBACK
#define flush_cache_wback(addr, len)	\
			dcache_wback_range((u32)addr, (u32)(addr + len - 1))
#else
#define flush_cache_wback(...)
#endif

#define TIMEOUT_CNT 1000

int sh_eth_send(struct eth_device *dev, void *packet, int len)
{
	struct sh_eth_dev *eth = dev->priv;
	int port = eth->port, ret = 0, timeout;
	struct sh_eth_info *port_info = &eth->port_info[port];

	if (!packet || len > 0xffff) {
		printf(SHETHER_NAME ": %s: Invalid argument\n", __func__);
		ret = -EINVAL;
		goto err;
	}

	/* packet must be a 4 byte boundary */
	if ((int)packet & 3) {
		printf(SHETHER_NAME ": %s: packet not 4 byte alligned\n", __func__);
		ret = -EFAULT;
		goto err;
	}

	/* Update tx descriptor */
	flush_cache_wback(packet, len);
	port_info->tx_desc_cur->td2 = ADDR_TO_PHY(packet);
	port_info->tx_desc_cur->td1 = len << 16;
	/* Must preserve the end of descriptor list indication */
	if (port_info->tx_desc_cur->td0 & TD_TDLE)
		port_info->tx_desc_cur->td0 = TD_TACT | TD_TFP | TD_TDLE;
	else
		port_info->tx_desc_cur->td0 = TD_TACT | TD_TFP;

	/* Restart the transmitter if disabled */
	if (!(inl(EDTRR(port)) & EDTRR_TRNS))
		outl(EDTRR_TRNS, EDTRR(port));

	/* Wait until packet is transmitted */
	timeout = TIMEOUT_CNT;
	while (port_info->tx_desc_cur->td0 & TD_TACT && timeout--)
		udelay(100);

	if (timeout < 0) {
		printf(SHETHER_NAME ": transmit timeout\n");
		ret = -ETIMEDOUT;
		goto err;
	}

	port_info->tx_desc_cur++;
	if (port_info->tx_desc_cur >= port_info->tx_desc_base + NUM_TX_DESC)
		port_info->tx_desc_cur = port_info->tx_desc_base;

err:
	return ret;
}

int sh_eth_recv(struct eth_device *dev)
{
	struct sh_eth_dev *eth = dev->priv;
	int port = eth->port, len = 0;
	struct sh_eth_info *port_info = &eth->port_info[port];
	uchar *packet;

	/* Check if the rx descriptor is ready */
	if (!(port_info->rx_desc_cur->rd0 & RD_RACT)) {
		/* Check for errors */
		if (!(port_info->rx_desc_cur->rd0 & RD_RFE)) {
			len = port_info->rx_desc_cur->rd1 & 0xffff;
			packet = (uchar *)
				ADDR_TO_P2(port_info->rx_desc_cur->rd2);
			NetReceive(packet, len);
		}

		/* Make current descriptor available again */
		if (port_info->rx_desc_cur->rd0 & RD_RDLE)
			port_info->rx_desc_cur->rd0 = RD_RACT | RD_RDLE;
		else
			port_info->rx_desc_cur->rd0 = RD_RACT;

		/* Point to the next descriptor */
		port_info->rx_desc_cur++;
		if (port_info->rx_desc_cur >=
		    port_info->rx_desc_base + NUM_RX_DESC)
			port_info->rx_desc_cur = port_info->rx_desc_base;
	}

	/* Restart the receiver if disabled */
	if (!(inl(EDRRR(port)) & EDRRR_R))
		outl(EDRRR_R, EDRRR(port));

	return len;
}

static int sh_eth_reset(struct sh_eth_dev *eth)
{
	int port = eth->port;
#if defined(CONFIG_CPU_SH7763) || defined(CONFIG_CPU_SH7734)
	int ret = 0, i;

	/* Start e-dmac transmitter and receiver */
	outl(EDSR_ENALL, EDSR(port));

	/* Perform a software reset and wait for it to complete */
	outl(EDMR_SRST, EDMR(port));
	for (i = 0; i < TIMEOUT_CNT ; i++) {
		if (!(inl(EDMR(port)) & EDMR_SRST))
			break;
		udelay(1000);
	}

	if (i == TIMEOUT_CNT) {
		printf(SHETHER_NAME  ": Software reset timeout\n");
		ret = -EIO;
	}

	return ret;
#else
	outl(inl(EDMR(port)) | EDMR_SRST, EDMR(port));
	udelay(3000);
	outl(inl(EDMR(port)) & ~EDMR_SRST, EDMR(port));

	return 0;
#endif
}

static int sh_eth_tx_desc_init(struct sh_eth_dev *eth)
{
	int port = eth->port, i, ret = 0;
	u32 tmp_addr;
	struct sh_eth_info *port_info = &eth->port_info[port];
	struct tx_desc_s *cur_tx_desc;

	/*
	 * Allocate tx descriptors. They must be TX_DESC_SIZE bytes aligned
	 */
	port_info->tx_desc_malloc = malloc(NUM_TX_DESC *
						 sizeof(struct tx_desc_s) +
						 TX_DESC_SIZE - 1);
	if (!port_info->tx_desc_malloc) {
		printf(SHETHER_NAME ": malloc failed\n");
		ret = -ENOMEM;
		goto err;
	}

	tmp_addr = (u32) (((int)port_info->tx_desc_malloc + TX_DESC_SIZE - 1) &
			  ~(TX_DESC_SIZE - 1));
	flush_cache_wback(tmp_addr, NUM_TX_DESC * sizeof(struct tx_desc_s));
	/* Make sure we use a P2 address (non-cacheable) */
	port_info->tx_desc_base = (struct tx_desc_s *)ADDR_TO_P2(tmp_addr);
	port_info->tx_desc_cur = port_info->tx_desc_base;

	/* Initialize all descriptors */
	for (cur_tx_desc = port_info->tx_desc_base, i = 0; i < NUM_TX_DESC;
	     cur_tx_desc++, i++) {
		cur_tx_desc->td0 = 0x00;
		cur_tx_desc->td1 = 0x00;
		cur_tx_desc->td2 = 0x00;
	}

	/* Mark the end of the descriptors */
	cur_tx_desc--;
	cur_tx_desc->td0 |= TD_TDLE;

	/* Point the controller to the tx descriptor list. Must use physical
	   addresses */
	outl(ADDR_TO_PHY(port_info->tx_desc_base), TDLAR(port));
#if defined(CONFIG_CPU_SH7763) || defined(CONFIG_CPU_SH7734)
	outl(ADDR_TO_PHY(port_info->tx_desc_base), TDFAR(port));
	outl(ADDR_TO_PHY(cur_tx_desc), TDFXR(port));
	outl(0x01, TDFFR(port));/* Last discriptor bit */
#endif

err:
	return ret;
}

static int sh_eth_rx_desc_init(struct sh_eth_dev *eth)
{
	int port = eth->port, i , ret = 0;
	struct sh_eth_info *port_info = &eth->port_info[port];
	struct rx_desc_s *cur_rx_desc;
	u32 tmp_addr;
	u8 *rx_buf;

	/*
	 * Allocate rx descriptors. They must be RX_DESC_SIZE bytes aligned
	 */
	port_info->rx_desc_malloc = malloc(NUM_RX_DESC *
						 sizeof(struct rx_desc_s) +
						 RX_DESC_SIZE - 1);
	if (!port_info->rx_desc_malloc) {
		printf(SHETHER_NAME ": malloc failed\n");
		ret = -ENOMEM;
		goto err;
	}

	tmp_addr = (u32) (((int)port_info->rx_desc_malloc + RX_DESC_SIZE - 1) &
			  ~(RX_DESC_SIZE - 1));
	flush_cache_wback(tmp_addr, NUM_RX_DESC * sizeof(struct rx_desc_s));
	/* Make sure we use a P2 address (non-cacheable) */
	port_info->rx_desc_base = (struct rx_desc_s *)ADDR_TO_P2(tmp_addr);

	port_info->rx_desc_cur = port_info->rx_desc_base;

	/*
	 * Allocate rx data buffers. They must be 32 bytes aligned  and in
	 * P2 area
	 */
	port_info->rx_buf_malloc = malloc(NUM_RX_DESC * MAX_BUF_SIZE + 31);
	if (!port_info->rx_buf_malloc) {
		printf(SHETHER_NAME ": malloc failed\n");
		ret = -ENOMEM;
		goto err_buf_malloc;
	}

	tmp_addr = (u32)(((int)port_info->rx_buf_malloc + (32 - 1)) &
			  ~(32 - 1));
	port_info->rx_buf_base = (u8 *)ADDR_TO_P2(tmp_addr);

	/* Initialize all descriptors */
	for (cur_rx_desc = port_info->rx_desc_base,
	     rx_buf = port_info->rx_buf_base, i = 0;
	     i < NUM_RX_DESC; cur_rx_desc++, rx_buf += MAX_BUF_SIZE, i++) {
		cur_rx_desc->rd0 = RD_RACT;
		cur_rx_desc->rd1 = MAX_BUF_SIZE << 16;
		cur_rx_desc->rd2 = (u32) ADDR_TO_PHY(rx_buf);
	}

	/* Mark the end of the descriptors */
	cur_rx_desc--;
	cur_rx_desc->rd0 |= RD_RDLE;

	/* Point the controller to the rx descriptor list */
	outl(ADDR_TO_PHY(port_info->rx_desc_base), RDLAR(port));
#if defined(CONFIG_CPU_SH7763) || defined(CONFIG_CPU_SH7734)
	outl(ADDR_TO_PHY(port_info->rx_desc_base), RDFAR(port));
	outl(ADDR_TO_PHY(cur_rx_desc), RDFXR(port));
	outl(RDFFR_RDLF, RDFFR(port));
#endif

	return ret;

err_buf_malloc:
	free(port_info->rx_desc_malloc);
	port_info->rx_desc_malloc = NULL;

err:
	return ret;
}

static void sh_eth_tx_desc_free(struct sh_eth_dev *eth)
{
	int port = eth->port;
	struct sh_eth_info *port_info = &eth->port_info[port];

	if (port_info->tx_desc_malloc) {
		free(port_info->tx_desc_malloc);
		port_info->tx_desc_malloc = NULL;
	}
}

static void sh_eth_rx_desc_free(struct sh_eth_dev *eth)
{
	int port = eth->port;
	struct sh_eth_info *port_info = &eth->port_info[port];

	if (port_info->rx_desc_malloc) {
		free(port_info->rx_desc_malloc);
		port_info->rx_desc_malloc = NULL;
	}

	if (port_info->rx_buf_malloc) {
		free(port_info->rx_buf_malloc);
		port_info->rx_buf_malloc = NULL;
	}
}

static int sh_eth_desc_init(struct sh_eth_dev *eth)
{
	int ret = 0;

	ret = sh_eth_tx_desc_init(eth);
	if (ret)
		goto err_tx_init;

	ret = sh_eth_rx_desc_init(eth);
	if (ret)
		goto err_rx_init;

	return ret;
err_rx_init:
	sh_eth_tx_desc_free(eth);

err_tx_init:
	return ret;
}

static int sh_eth_phy_config(struct sh_eth_dev *eth)
{
	int port = eth->port, ret = 0;
	struct sh_eth_info *port_info = &eth->port_info[port];
	struct eth_device *dev = port_info->dev;
	struct phy_device *phydev;

	phydev = phy_connect(
			miiphy_get_dev_by_name(dev->name),
			port_info->phy_addr, dev, CONFIG_SH_ETHER_PHY_MODE);
	port_info->phydev = phydev;
	phy_config(phydev);

	return ret;
}

static int sh_eth_config(struct sh_eth_dev *eth, bd_t *bd)
{
	int port = eth->port, ret = 0;
	u32 val;
	struct sh_eth_info *port_info = &eth->port_info[port];
	struct eth_device *dev = port_info->dev;
	struct phy_device *phy;

	/* Configure e-dmac registers */
	outl((inl(EDMR(port)) & ~EMDR_DESC_R) | EDMR_EL, EDMR(port));
	outl(0, EESIPR(port));
	outl(0, TRSCER(port));
	outl(0, TFTR(port));
	outl((FIFO_SIZE_T | FIFO_SIZE_R), FDR(port));
	outl(RMCR_RST, RMCR(port));
#if !defined(CONFIG_CPU_SH7757) && !defined(CONFIG_CPU_SH7724)
	outl(0, RPADIR(port));
#endif
	outl((FIFO_F_D_RFF | FIFO_F_D_RFD), FCFTR(port));

	/* Configure e-mac registers */
#if defined(CONFIG_CPU_SH7757)
	outl(ECSIPR_BRCRXIP | ECSIPR_PSRTOIP | ECSIPR_LCHNGIP |
		ECSIPR_MPDIP | ECSIPR_ICDIP, ECSIPR(port));
#else
	outl(0, ECSIPR(port));
#endif

	/* Set Mac address */
	val = dev->enetaddr[0] << 24 | dev->enetaddr[1] << 16 |
	    dev->enetaddr[2] << 8 | dev->enetaddr[3];
	outl(val, MAHR(port));

	val = dev->enetaddr[4] << 8 | dev->enetaddr[5];
	outl(val, MALR(port));

	outl(RFLR_RFL_MIN, RFLR(port));
#if !defined(CONFIG_CPU_SH7757) && !defined(CONFIG_CPU_SH7724)
	outl(0, PIPR(port));
#endif
#if !defined(CONFIG_CPU_SH7724)
	outl(APR_AP, APR(port));
	outl(MPR_MP, MPR(port));
#endif
#if defined(CONFIG_CPU_SH7763) || defined(CONFIG_CPU_SH7734)
	outl(TPAUSER_TPAUSE, TPAUSER(port));
#elif defined(CONFIG_CPU_SH7757)
	outl(TPAUSER_UNLIMITED, TPAUSER(port));
#endif

#if defined(CONFIG_CPU_SH7734)
	outl(CONFIG_SH_ETHER_SH7734_MII, RMII_MII(port));
#endif
	/* Configure phy */
	ret = sh_eth_phy_config(eth);
	if (ret) {
		printf(SHETHER_NAME ": phy config timeout\n");
		goto err_phy_cfg;
	}
	phy = port_info->phydev;
	phy_startup(phy);

	val = 0;

	/* Set the transfer speed */
	if (phy->speed == 100) {
		printf(SHETHER_NAME ": 100Base/");
#if defined(CONFIG_CPU_SH7763) || defined(CONFIG_CPU_SH7734)
		outl(GECMR_100B, GECMR(port));
#elif defined(CONFIG_CPU_SH7757)
		outl(1, RTRATE(port));
#elif defined(CONFIG_CPU_SH7724)
		val = ECMR_RTM;
#endif
	} else if (phy->speed == 10) {
		printf(SHETHER_NAME ": 10Base/");
#if defined(CONFIG_CPU_SH7763) || defined(CONFIG_CPU_SH7734)
		outl(GECMR_10B, GECMR(port));
#elif defined(CONFIG_CPU_SH7757)
		outl(0, RTRATE(port));
#endif
	}
#if defined(CONFIG_CPU_SH7763) || defined(CONFIG_CPU_SH7734)
	else if (phy->speed == 1000) {
		printf(SHETHER_NAME ": 1000Base/");
		outl(GECMR_1000B, GECMR(port));
	}
#endif

	/* Check if full duplex mode is supported by the phy */
	if (phy->duplex) {
		printf("Full\n");
		outl(val | (ECMR_CHG_DM|ECMR_RE|ECMR_TE|ECMR_DM), ECMR(port));
	} else {
		printf("Half\n");
		outl(val | (ECMR_CHG_DM|ECMR_RE|ECMR_TE),  ECMR(port));
	}

	return ret;

err_phy_cfg:
	return ret;
}

static void sh_eth_start(struct sh_eth_dev *eth)
{
	/*
	 * Enable the e-dmac receiver only. The transmitter will be enabled when
	 * we have something to transmit
	 */
	outl(EDRRR_R, EDRRR(eth->port));
}

static void sh_eth_stop(struct sh_eth_dev *eth)
{
	outl(~EDRRR_R, EDRRR(eth->port));
}

int sh_eth_init(struct eth_device *dev, bd_t *bd)
{
	int ret = 0;
	struct sh_eth_dev *eth = dev->priv;

	ret = sh_eth_reset(eth);
	if (ret)
		goto err;

	ret = sh_eth_desc_init(eth);
	if (ret)
		goto err;

	ret = sh_eth_config(eth, bd);
	if (ret)
		goto err_config;

	sh_eth_start(eth);

	return ret;

err_config:
	sh_eth_tx_desc_free(eth);
	sh_eth_rx_desc_free(eth);

err:
	return ret;
}

void sh_eth_halt(struct eth_device *dev)
{
	struct sh_eth_dev *eth = dev->priv;
	sh_eth_stop(eth);
}

int sh_eth_initialize(bd_t *bd)
{
    int ret = 0;
	struct sh_eth_dev *eth = NULL;
    struct eth_device *dev = NULL;

    eth = (struct sh_eth_dev *)malloc(sizeof(struct sh_eth_dev));
	if (!eth) {
		printf(SHETHER_NAME ": %s: malloc failed\n", __func__);
		ret = -ENOMEM;
		goto err;
	}

    dev = (struct eth_device *)malloc(sizeof(struct eth_device));
	if (!dev) {
		printf(SHETHER_NAME ": %s: malloc failed\n", __func__);
		ret = -ENOMEM;
		goto err;
	}
    memset(dev, 0, sizeof(struct eth_device));
    memset(eth, 0, sizeof(struct sh_eth_dev));

	eth->port = CONFIG_SH_ETHER_USE_PORT;
	eth->port_info[eth->port].phy_addr = CONFIG_SH_ETHER_PHY_ADDR;

    dev->priv = (void *)eth;
    dev->iobase = 0;
    dev->init = sh_eth_init;
    dev->halt = sh_eth_halt;
    dev->send = sh_eth_send;
    dev->recv = sh_eth_recv;
    eth->port_info[eth->port].dev = dev;

	sprintf(dev->name, SHETHER_NAME);

    /* Register Device to EtherNet subsystem  */
    eth_register(dev);

	bb_miiphy_buses[0].priv = eth;
	miiphy_register(dev->name, bb_miiphy_read, bb_miiphy_write);

	if (!eth_getenv_enetaddr("ethaddr", dev->enetaddr))
		puts("Please set MAC address\n");

	return ret;

err:
	if (dev)
		free(dev);

	if (eth)
		free(eth);

	printf(SHETHER_NAME ": Failed\n");
	return ret;
}

/******* for bb_miiphy *******/
static int sh_eth_bb_init(struct bb_miiphy_bus *bus)
{
	return 0;
}

static int sh_eth_bb_mdio_active(struct bb_miiphy_bus *bus)
{
	struct sh_eth_dev *eth = bus->priv;
	int port = eth->port;

	outl(inl(PIR(port)) | PIR_MMD, PIR(port));

	return 0;
}

static int sh_eth_bb_mdio_tristate(struct bb_miiphy_bus *bus)
{
	struct sh_eth_dev *eth = bus->priv;
	int port = eth->port;

	outl(inl(PIR(port)) & ~PIR_MMD, PIR(port));

	return 0;
}

static int sh_eth_bb_set_mdio(struct bb_miiphy_bus *bus, int v)
{
	struct sh_eth_dev *eth = bus->priv;
	int port = eth->port;

	if (v)
		outl(inl(PIR(port)) | PIR_MDO, PIR(port));
	else
		outl(inl(PIR(port)) & ~PIR_MDO, PIR(port));

	return 0;
}

static int sh_eth_bb_get_mdio(struct bb_miiphy_bus *bus, int *v)
{
	struct sh_eth_dev *eth = bus->priv;
	int port = eth->port;

	*v = (inl(PIR(port)) & PIR_MDI) >> 3;

	return 0;
}

static int sh_eth_bb_set_mdc(struct bb_miiphy_bus *bus, int v)
{
	struct sh_eth_dev *eth = bus->priv;
	int port = eth->port;

	if (v)
		outl(inl(PIR(port)) | PIR_MDC, PIR(port));
	else
		outl(inl(PIR(port)) & ~PIR_MDC, PIR(port));

	return 0;
}

static int sh_eth_bb_delay(struct bb_miiphy_bus *bus)
{
	udelay(10);

	return 0;
}

struct bb_miiphy_bus bb_miiphy_buses[] = {
	{
		.name		= "sh_eth",
		.init		= sh_eth_bb_init,
		.mdio_active	= sh_eth_bb_mdio_active,
		.mdio_tristate	= sh_eth_bb_mdio_tristate,
		.set_mdio	= sh_eth_bb_set_mdio,
		.get_mdio	= sh_eth_bb_get_mdio,
		.set_mdc	= sh_eth_bb_set_mdc,
		.delay		= sh_eth_bb_delay,
	}
};
int bb_miiphy_buses_num = ARRAY_SIZE(bb_miiphy_buses);
