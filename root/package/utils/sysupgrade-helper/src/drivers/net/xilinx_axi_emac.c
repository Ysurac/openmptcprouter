/*
 * Copyright (C) 2011 Michal Simek <monstr@monstr.eu>
 * Copyright (C) 2011 PetaLogix
 * Copyright (C) 2010 Xilinx, Inc. All rights reserved.
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

#include <config.h>
#include <common.h>
#include <net.h>
#include <malloc.h>
#include <asm/io.h>
#include <phy.h>
#include <miiphy.h>

#if !defined(CONFIG_PHYLIB)
# error AXI_ETHERNET requires PHYLIB
#endif

/* Link setup */
#define XAE_EMMC_LINKSPEED_MASK	0xC0000000 /* Link speed */
#define XAE_EMMC_LINKSPD_10	0x00000000 /* Link Speed mask for 10 Mbit */
#define XAE_EMMC_LINKSPD_100	0x40000000 /* Link Speed mask for 100 Mbit */
#define XAE_EMMC_LINKSPD_1000	0x80000000 /* Link Speed mask for 1000 Mbit */

/* Interrupt Status/Enable/Mask Registers bit definitions */
#define XAE_INT_RXRJECT_MASK	0x00000008 /* Rx frame rejected */
#define XAE_INT_MGTRDY_MASK	0x00000080 /* MGT clock Lock */

/* Receive Configuration Word 1 (RCW1) Register bit definitions */
#define XAE_RCW1_RX_MASK	0x10000000 /* Receiver enable */

/* Transmitter Configuration (TC) Register bit definitions */
#define XAE_TC_TX_MASK		0x10000000 /* Transmitter enable */

#define XAE_UAW1_UNICASTADDR_MASK	0x0000FFFF

/* MDIO Management Configuration (MC) Register bit definitions */
#define XAE_MDIO_MC_MDIOEN_MASK		0x00000040 /* MII management enable*/

/* MDIO Management Control Register (MCR) Register bit definitions */
#define XAE_MDIO_MCR_PHYAD_MASK		0x1F000000 /* Phy Address Mask */
#define XAE_MDIO_MCR_PHYAD_SHIFT	24	   /* Phy Address Shift */
#define XAE_MDIO_MCR_REGAD_MASK		0x001F0000 /* Reg Address Mask */
#define XAE_MDIO_MCR_REGAD_SHIFT	16	   /* Reg Address Shift */
#define XAE_MDIO_MCR_OP_READ_MASK	0x00008000 /* Op Code Read Mask */
#define XAE_MDIO_MCR_OP_WRITE_MASK	0x00004000 /* Op Code Write Mask */
#define XAE_MDIO_MCR_INITIATE_MASK	0x00000800 /* Ready Mask */
#define XAE_MDIO_MCR_READY_MASK		0x00000080 /* Ready Mask */

#define XAE_MDIO_DIV_DFT	29	/* Default MDIO clock divisor */

/* DMA macros */
/* Bitmasks of XAXIDMA_CR_OFFSET register */
#define XAXIDMA_CR_RUNSTOP_MASK	0x00000001 /* Start/stop DMA channel */
#define XAXIDMA_CR_RESET_MASK	0x00000004 /* Reset DMA engine */

/* Bitmasks of XAXIDMA_SR_OFFSET register */
#define XAXIDMA_HALTED_MASK	0x00000001  /* DMA channel halted */

/* Bitmask for interrupts */
#define XAXIDMA_IRQ_IOC_MASK	0x00001000 /* Completion intr */
#define XAXIDMA_IRQ_DELAY_MASK	0x00002000 /* Delay interrupt */
#define XAXIDMA_IRQ_ALL_MASK	0x00007000 /* All interrupts */

/* Bitmasks of XAXIDMA_BD_CTRL_OFFSET register */
#define XAXIDMA_BD_CTRL_TXSOF_MASK	0x08000000 /* First tx packet */
#define XAXIDMA_BD_CTRL_TXEOF_MASK	0x04000000 /* Last tx packet */

#define DMAALIGN	128

static u8 rxframe[PKTSIZE_ALIGN] __attribute((aligned(DMAALIGN)));

/* Reflect dma offsets */
struct axidma_reg {
	u32 control; /* DMACR */
	u32 status; /* DMASR */
	u32 current; /* CURDESC */
	u32 reserved;
	u32 tail; /* TAILDESC */
};

/* Private driver structures */
struct axidma_priv {
	struct axidma_reg *dmatx;
	struct axidma_reg *dmarx;
	int phyaddr;

	struct phy_device *phydev;
	struct mii_dev *bus;
};

/* BD descriptors */
struct axidma_bd {
	u32 next;	/* Next descriptor pointer */
	u32 reserved1;
	u32 phys;	/* Buffer address */
	u32 reserved2;
	u32 reserved3;
	u32 reserved4;
	u32 cntrl;	/* Control */
	u32 status;	/* Status */
	u32 app0;
	u32 app1;	/* TX start << 16 | insert */
	u32 app2;	/* TX csum seed */
	u32 app3;
	u32 app4;
	u32 sw_id_offset;
	u32 reserved5;
	u32 reserved6;
};

/* Static BDs - driver uses only one BD */
static struct axidma_bd tx_bd __attribute((aligned(DMAALIGN)));
static struct axidma_bd rx_bd __attribute((aligned(DMAALIGN)));

struct axi_regs {
	u32 reserved[3];
	u32 is; /* 0xC: Interrupt status */
	u32 reserved2;
	u32 ie; /* 0x14: Interrupt enable */
	u32 reserved3[251];
	u32 rcw1; /* 0x404: Rx Configuration Word 1 */
	u32 tc; /* 0x408: Tx Configuration */
	u32 reserved4;
	u32 emmc; /* 0x410: EMAC mode configuration */
	u32 reserved5[59];
	u32 mdio_mc; /* 0x500: MII Management Config */
	u32 mdio_mcr; /* 0x504: MII Management Control */
	u32 mdio_mwd; /* 0x508: MII Management Write Data */
	u32 mdio_mrd; /* 0x50C: MII Management Read Data */
	u32 reserved6[124];
	u32 uaw0; /* 0x700: Unicast address word 0 */
	u32 uaw1; /* 0x704: Unicast address word 1 */
};

/* Use MII register 1 (MII status register) to detect PHY */
#define PHY_DETECT_REG  1

/*
 * Mask used to verify certain PHY features (or register contents)
 * in the register above:
 *  0x1000: 10Mbps full duplex support
 *  0x0800: 10Mbps half duplex support
 *  0x0008: Auto-negotiation support
 */
#define PHY_DETECT_MASK 0x1808

static inline int mdio_wait(struct eth_device *dev)
{
	struct axi_regs *regs = (struct axi_regs *)dev->iobase;
	u32 timeout = 200;

	/* Wait till MDIO interface is ready to accept a new transaction. */
	while (timeout && (!(in_be32(&regs->mdio_mcr)
						& XAE_MDIO_MCR_READY_MASK))) {
		timeout--;
		udelay(1);
	}
	if (!timeout) {
		printf("%s: Timeout\n", __func__);
		return 1;
	}
	return 0;
}

static u32 phyread(struct eth_device *dev, u32 phyaddress, u32 registernum,
								u16 *val)
{
	struct axi_regs *regs = (struct axi_regs *)dev->iobase;
	u32 mdioctrlreg = 0;

	if (mdio_wait(dev))
		return 1;

	mdioctrlreg = ((phyaddress << XAE_MDIO_MCR_PHYAD_SHIFT) &
			XAE_MDIO_MCR_PHYAD_MASK) |
			((registernum << XAE_MDIO_MCR_REGAD_SHIFT)
			& XAE_MDIO_MCR_REGAD_MASK) |
			XAE_MDIO_MCR_INITIATE_MASK |
			XAE_MDIO_MCR_OP_READ_MASK;

	out_be32(&regs->mdio_mcr, mdioctrlreg);

	if (mdio_wait(dev))
		return 1;

	/* Read data */
	*val = in_be32(&regs->mdio_mrd);
	return 0;
}

static u32 phywrite(struct eth_device *dev, u32 phyaddress, u32 registernum,
								u32 data)
{
	struct axi_regs *regs = (struct axi_regs *)dev->iobase;
	u32 mdioctrlreg = 0;

	if (mdio_wait(dev))
		return 1;

	mdioctrlreg = ((phyaddress << XAE_MDIO_MCR_PHYAD_SHIFT) &
			XAE_MDIO_MCR_PHYAD_MASK) |
			((registernum << XAE_MDIO_MCR_REGAD_SHIFT)
			& XAE_MDIO_MCR_REGAD_MASK) |
			XAE_MDIO_MCR_INITIATE_MASK |
			XAE_MDIO_MCR_OP_WRITE_MASK;

	/* Write data */
	out_be32(&regs->mdio_mwd, data);

	out_be32(&regs->mdio_mcr, mdioctrlreg);

	if (mdio_wait(dev))
		return 1;

	return 0;
}

/* Setting axi emac and phy to proper setting */
static int setup_phy(struct eth_device *dev)
{
	u16 phyreg;
	u32 i, speed, emmc_reg, ret;
	struct axidma_priv *priv = dev->priv;
	struct axi_regs *regs = (struct axi_regs *)dev->iobase;
	struct phy_device *phydev;

	u32 supported = SUPPORTED_10baseT_Half |
			SUPPORTED_10baseT_Full |
			SUPPORTED_100baseT_Half |
			SUPPORTED_100baseT_Full |
			SUPPORTED_1000baseT_Half |
			SUPPORTED_1000baseT_Full;

	if (priv->phyaddr == -1) {
		/* Detect the PHY address */
		for (i = 31; i >= 0; i--) {
			ret = phyread(dev, i, PHY_DETECT_REG, &phyreg);
			if (!ret && (phyreg != 0xFFFF) &&
			((phyreg & PHY_DETECT_MASK) == PHY_DETECT_MASK)) {
				/* Found a valid PHY address */
				priv->phyaddr = i;
				debug("axiemac: Found valid phy address, %x\n",
									phyreg);
				break;
			}
		}
	}

	/* Interface - look at tsec */
	phydev = phy_connect(priv->bus, priv->phyaddr, dev, 0);

	phydev->supported &= supported;
	phydev->advertising = phydev->supported;
	priv->phydev = phydev;
	phy_config(phydev);
	phy_startup(phydev);

	switch (phydev->speed) {
	case 1000:
		speed = XAE_EMMC_LINKSPD_1000;
		break;
	case 100:
		speed = XAE_EMMC_LINKSPD_100;
		break;
	case 10:
		speed = XAE_EMMC_LINKSPD_10;
		break;
	default:
		return 0;
	}

	/* Setup the emac for the phy speed */
	emmc_reg = in_be32(&regs->emmc);
	emmc_reg &= ~XAE_EMMC_LINKSPEED_MASK;
	emmc_reg |= speed;

	/* Write new speed setting out to Axi Ethernet */
	out_be32(&regs->emmc, emmc_reg);

	/*
	* Setting the operating speed of the MAC needs a delay. There
	* doesn't seem to be register to poll, so please consider this
	* during your application design.
	*/
	udelay(1);

	return 1;
}

/* STOP DMA transfers */
static void axiemac_halt(struct eth_device *dev)
{
	struct axidma_priv *priv = dev->priv;
	u32 temp;

	/* Stop the hardware */
	temp = in_be32(&priv->dmatx->control);
	temp &= ~XAXIDMA_CR_RUNSTOP_MASK;
	out_be32(&priv->dmatx->control, temp);

	temp = in_be32(&priv->dmarx->control);
	temp &= ~XAXIDMA_CR_RUNSTOP_MASK;
	out_be32(&priv->dmarx->control, temp);

	debug("axiemac: Halted\n");
}

static int axi_ethernet_init(struct eth_device *dev)
{
	struct axi_regs *regs = (struct axi_regs *)dev->iobase;
	u32 timeout = 200;

	/*
	 * Check the status of the MgtRdy bit in the interrupt status
	 * registers. This must be done to allow the MGT clock to become stable
	 * for the Sgmii and 1000BaseX PHY interfaces. No other register reads
	 * will be valid until this bit is valid.
	 * The bit is always a 1 for all other PHY interfaces.
	 */
	while (timeout && (!(in_be32(&regs->is) & XAE_INT_MGTRDY_MASK))) {
		timeout--;
		udelay(1);
	}
	if (!timeout) {
		printf("%s: Timeout\n", __func__);
		return 1;
	}

	/* Stop the device and reset HW */
	/* Disable interrupts */
	out_be32(&regs->ie, 0);

	/* Disable the receiver */
	out_be32(&regs->rcw1, in_be32(&regs->rcw1) & ~XAE_RCW1_RX_MASK);

	/*
	 * Stopping the receiver in mid-packet causes a dropped packet
	 * indication from HW. Clear it.
	 */
	/* Set the interrupt status register to clear the interrupt */
	out_be32(&regs->is, XAE_INT_RXRJECT_MASK);

	/* Setup HW */
	/* Set default MDIO divisor */
	out_be32(&regs->mdio_mc, XAE_MDIO_DIV_DFT | XAE_MDIO_MC_MDIOEN_MASK);

	debug("axiemac: InitHw done\n");
	return 0;
}

static int axiemac_setup_mac(struct eth_device *dev)
{
	struct axi_regs *regs = (struct axi_regs *)dev->iobase;

	/* Set the MAC address */
	int val = ((dev->enetaddr[3] << 24) | (dev->enetaddr[2] << 16) |
		(dev->enetaddr[1] << 8) | (dev->enetaddr[0]));
	out_be32(&regs->uaw0, val);

	val = (dev->enetaddr[5] << 8) | dev->enetaddr[4] ;
	val |= in_be32(&regs->uaw1) & ~XAE_UAW1_UNICASTADDR_MASK;
	out_be32(&regs->uaw1, val);
	return 0;
}

/* Reset DMA engine */
static void axi_dma_init(struct eth_device *dev)
{
	struct axidma_priv *priv = dev->priv;
	u32 timeout = 500;

	/* Reset the engine so the hardware starts from a known state */
	out_be32(&priv->dmatx->control, XAXIDMA_CR_RESET_MASK);
	out_be32(&priv->dmarx->control, XAXIDMA_CR_RESET_MASK);

	/* At the initialization time, hardware should finish reset quickly */
	while (timeout--) {
		/* Check transmit/receive channel */
		/* Reset is done when the reset bit is low */
		if (!(in_be32(&priv->dmatx->control) |
				in_be32(&priv->dmarx->control))
						& XAXIDMA_CR_RESET_MASK) {
			break;
		}
	}
	if (!timeout)
		printf("%s: Timeout\n", __func__);
}

static int axiemac_init(struct eth_device *dev, bd_t * bis)
{
	struct axidma_priv *priv = dev->priv;
	struct axi_regs *regs = (struct axi_regs *)dev->iobase;
	u32 temp;

	debug("axiemac: Init started\n");
	/*
	 * Initialize AXIDMA engine. AXIDMA engine must be initialized before
	 * AxiEthernet. During AXIDMA engine initialization, AXIDMA hardware is
	 * reset, and since AXIDMA reset line is connected to AxiEthernet, this
	 * would ensure a reset of AxiEthernet.
	 */
	axi_dma_init(dev);

	/* Initialize AxiEthernet hardware. */
	if (axi_ethernet_init(dev))
		return -1;

	/* Disable all RX interrupts before RxBD space setup */
	temp = in_be32(&priv->dmarx->control);
	temp &= ~XAXIDMA_IRQ_ALL_MASK;
	out_be32(&priv->dmarx->control, temp);

	/* Start DMA RX channel. Now it's ready to receive data.*/
	out_be32(&priv->dmarx->current, (u32)&rx_bd);

	/* Setup the BD. */
	memset(&rx_bd, 0, sizeof(rx_bd));
	rx_bd.next = (u32)&rx_bd;
	rx_bd.phys = (u32)&rxframe;
	rx_bd.cntrl = sizeof(rxframe);
	/* Flush the last BD so DMA core could see the updates */
	flush_cache((u32)&rx_bd, sizeof(rx_bd));

	/* It is necessary to flush rxframe because if you don't do it
	 * then cache can contain uninitialized data */
	flush_cache((u32)&rxframe, sizeof(rxframe));

	/* Start the hardware */
	temp = in_be32(&priv->dmarx->control);
	temp |= XAXIDMA_CR_RUNSTOP_MASK;
	out_be32(&priv->dmarx->control, temp);

	/* Rx BD is ready - start */
	out_be32(&priv->dmarx->tail, (u32)&rx_bd);

	/* Enable TX */
	out_be32(&regs->tc, XAE_TC_TX_MASK);
	/* Enable RX */
	out_be32(&regs->rcw1, XAE_RCW1_RX_MASK);

	/* PHY setup */
	if (!setup_phy(dev)) {
		axiemac_halt(dev);
		return -1;
	}

	debug("axiemac: Init complete\n");
	return 0;
}

static int axiemac_send(struct eth_device *dev, void *ptr, int len)
{
	struct axidma_priv *priv = dev->priv;
	u32 timeout;

	if (len > PKTSIZE_ALIGN)
		len = PKTSIZE_ALIGN;

	/* Flush packet to main memory to be trasfered by DMA */
	flush_cache((u32)ptr, len);

	/* Setup Tx BD */
	memset(&tx_bd, 0, sizeof(tx_bd));
	/* At the end of the ring, link the last BD back to the top */
	tx_bd.next = (u32)&tx_bd;
	tx_bd.phys = (u32)ptr;
	/* Save len */
	tx_bd.cntrl = len | XAXIDMA_BD_CTRL_TXSOF_MASK |
						XAXIDMA_BD_CTRL_TXEOF_MASK;

	/* Flush the last BD so DMA core could see the updates */
	flush_cache((u32)&tx_bd, sizeof(tx_bd));

	if (in_be32(&priv->dmatx->status) & XAXIDMA_HALTED_MASK) {
		u32 temp;
		out_be32(&priv->dmatx->current, (u32)&tx_bd);
		/* Start the hardware */
		temp = in_be32(&priv->dmatx->control);
		temp |= XAXIDMA_CR_RUNSTOP_MASK;
		out_be32(&priv->dmatx->control, temp);
	}

	/* Start transfer */
	out_be32(&priv->dmatx->tail, (u32)&tx_bd);

	/* Wait for transmission to complete */
	debug("axiemac: Waiting for tx to be done\n");
	timeout = 200;
	while (timeout && (!in_be32(&priv->dmatx->status) &
			(XAXIDMA_IRQ_DELAY_MASK | XAXIDMA_IRQ_IOC_MASK))) {
		timeout--;
		udelay(1);
	}
	if (!timeout) {
		printf("%s: Timeout\n", __func__);
		return 1;
	}

	debug("axiemac: Sending complete\n");
	return 0;
}

static int isrxready(struct eth_device *dev)
{
	u32 status;
	struct axidma_priv *priv = dev->priv;

	/* Read pending interrupts */
	status = in_be32(&priv->dmarx->status);

	/* Acknowledge pending interrupts */
	out_be32(&priv->dmarx->status, status & XAXIDMA_IRQ_ALL_MASK);

	/*
	 * If Reception done interrupt is asserted, call RX call back function
	 * to handle the processed BDs and then raise the according flag.
	 */
	if ((status & (XAXIDMA_IRQ_DELAY_MASK | XAXIDMA_IRQ_IOC_MASK)))
		return 1;

	return 0;
}

static int axiemac_recv(struct eth_device *dev)
{
	u32 length;
	struct axidma_priv *priv = dev->priv;
	u32 temp;

	/* Wait for an incoming packet */
	if (!isrxready(dev))
		return 0;

	debug("axiemac: RX data ready\n");

	/* Disable IRQ for a moment till packet is handled */
	temp = in_be32(&priv->dmarx->control);
	temp &= ~XAXIDMA_IRQ_ALL_MASK;
	out_be32(&priv->dmarx->control, temp);

	length = rx_bd.app4 & 0xFFFF; /* max length mask */
#ifdef DEBUG
	print_buffer(&rxframe, &rxframe[0], 1, length, 16);
#endif
	/* Pass the received frame up for processing */
	if (length)
		NetReceive(rxframe, length);

#ifdef DEBUG
	/* It is useful to clear buffer to be sure that it is consistent */
	memset(rxframe, 0, sizeof(rxframe));
#endif
	/* Setup RxBD */
	/* Clear the whole buffer and setup it again - all flags are cleared */
	memset(&rx_bd, 0, sizeof(rx_bd));
	rx_bd.next = (u32)&rx_bd;
	rx_bd.phys = (u32)&rxframe;
	rx_bd.cntrl = sizeof(rxframe);

	/* Write bd to HW */
	flush_cache((u32)&rx_bd, sizeof(rx_bd));

	/* It is necessary to flush rxframe because if you don't do it
	 * then cache will contain previous packet */
	flush_cache((u32)&rxframe, sizeof(rxframe));

	/* Rx BD is ready - start again */
	out_be32(&priv->dmarx->tail, (u32)&rx_bd);

	debug("axiemac: RX completed, framelength = %d\n", length);

	return length;
}

static int axiemac_miiphy_read(const char *devname, uchar addr,
							uchar reg, ushort *val)
{
	struct eth_device *dev = eth_get_dev();
	u32 ret;

	ret = phyread(dev, addr, reg, val);
	debug("axiemac: Read MII 0x%x, 0x%x, 0x%x\n", addr, reg, *val);
	return ret;
}

static int axiemac_miiphy_write(const char *devname, uchar addr,
							uchar reg, ushort val)
{
	struct eth_device *dev = eth_get_dev();

	debug("axiemac: Write MII 0x%x, 0x%x, 0x%x\n", addr, reg, val);
	return phywrite(dev, addr, reg, val);
}

static int axiemac_bus_reset(struct mii_dev *bus)
{
	debug("axiemac: Bus reset\n");
	return 0;
}

int xilinx_axiemac_initialize(bd_t *bis, unsigned long base_addr,
							unsigned long dma_addr)
{
	struct eth_device *dev;
	struct axidma_priv *priv;

	dev = calloc(1, sizeof(struct eth_device));
	if (dev == NULL)
		return -1;

	dev->priv = calloc(1, sizeof(struct axidma_priv));
	if (dev->priv == NULL) {
		free(dev);
		return -1;
	}
	priv = dev->priv;

	sprintf(dev->name, "aximac.%lx", base_addr);

	dev->iobase = base_addr;
	priv->dmatx = (struct axidma_reg *)dma_addr;
	/* RX channel offset is 0x30 */
	priv->dmarx = (struct axidma_reg *)(dma_addr + 0x30);
	dev->init = axiemac_init;
	dev->halt = axiemac_halt;
	dev->send = axiemac_send;
	dev->recv = axiemac_recv;
	dev->write_hwaddr = axiemac_setup_mac;

#ifdef CONFIG_PHY_ADDR
	priv->phyaddr = CONFIG_PHY_ADDR;
#else
	priv->phyaddr = -1;
#endif

	eth_register(dev);

#if defined(CONFIG_MII) || defined(CONFIG_CMD_MII) || defined(CONFIG_PHYLIB)
	miiphy_register(dev->name, axiemac_miiphy_read, axiemac_miiphy_write);
	priv->bus = miiphy_get_dev_by_name(dev->name);
	priv->bus->reset = axiemac_bus_reset;
#endif
	return 1;
}
