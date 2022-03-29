/*
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
 *
 * (C) Copyright 2003
 * Ingo Assmus <ingo.assmus@keymile.com>
 *
 * based on - Driver for MV64360X ethernet ports
 * Copyright (C) 2002 rabeeh@galileo.co.il
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <common.h>
#include <net.h>
#include <malloc.h>
#include <miiphy.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <asm/types.h>
#include <asm/system.h>
#include <asm/byteorder.h>
#include <asm/arch/cpu.h>

#if defined(CONFIG_KIRKWOOD)
#include <asm/arch/kirkwood.h>
#elif defined(CONFIG_ORION5X)
#include <asm/arch/orion5x.h>
#endif

#include "mvgbe.h"

DECLARE_GLOBAL_DATA_PTR;

#define MV_PHY_ADR_REQUEST 0xee
#define MVGBE_SMI_REG (((struct mvgbe_registers *)MVGBE0_BASE)->smi)

#if defined(CONFIG_MII) || defined(CONFIG_CMD_MII)
/*
 * smi_reg_read - miiphy_read callback function.
 *
 * Returns 16bit phy register value, or 0xffff on error
 */
static int smi_reg_read(const char *devname, u8 phy_adr, u8 reg_ofs, u16 * data)
{
	struct eth_device *dev = eth_get_dev_by_name(devname);
	struct mvgbe_device *dmvgbe = to_mvgbe(dev);
	struct mvgbe_registers *regs = dmvgbe->regs;
	u32 smi_reg;
	u32 timeout;

	/* Phyadr read request */
	if (phy_adr == MV_PHY_ADR_REQUEST &&
			reg_ofs == MV_PHY_ADR_REQUEST) {
		/* */
		*data = (u16) (MVGBE_REG_RD(regs->phyadr) & PHYADR_MASK);
		return 0;
	}
	/* check parameters */
	if (phy_adr > PHYADR_MASK) {
		printf("Err..(%s) Invalid PHY address %d\n",
			__FUNCTION__, phy_adr);
		return -EFAULT;
	}
	if (reg_ofs > PHYREG_MASK) {
		printf("Err..(%s) Invalid register offset %d\n",
			__FUNCTION__, reg_ofs);
		return -EFAULT;
	}

	timeout = MVGBE_PHY_SMI_TIMEOUT;
	/* wait till the SMI is not busy */
	do {
		/* read smi register */
		smi_reg = MVGBE_REG_RD(MVGBE_SMI_REG);
		if (timeout-- == 0) {
			printf("Err..(%s) SMI busy timeout\n", __FUNCTION__);
			return -EFAULT;
		}
	} while (smi_reg & MVGBE_PHY_SMI_BUSY_MASK);

	/* fill the phy address and regiser offset and read opcode */
	smi_reg = (phy_adr << MVGBE_PHY_SMI_DEV_ADDR_OFFS)
		| (reg_ofs << MVGBE_SMI_REG_ADDR_OFFS)
		| MVGBE_PHY_SMI_OPCODE_READ;

	/* write the smi register */
	MVGBE_REG_WR(MVGBE_SMI_REG, smi_reg);

	/*wait till read value is ready */
	timeout = MVGBE_PHY_SMI_TIMEOUT;

	do {
		/* read smi register */
		smi_reg = MVGBE_REG_RD(MVGBE_SMI_REG);
		if (timeout-- == 0) {
			printf("Err..(%s) SMI read ready timeout\n",
				__FUNCTION__);
			return -EFAULT;
		}
	} while (!(smi_reg & MVGBE_PHY_SMI_READ_VALID_MASK));

	/* Wait for the data to update in the SMI register */
	for (timeout = 0; timeout < MVGBE_PHY_SMI_TIMEOUT; timeout++)
		;

	*data = (u16) (MVGBE_REG_RD(MVGBE_SMI_REG) & MVGBE_PHY_SMI_DATA_MASK);

	debug("%s:(adr %d, off %d) value= %04x\n", __FUNCTION__, phy_adr,
		reg_ofs, *data);

	return 0;
}

/*
 * smi_reg_write - imiiphy_write callback function.
 *
 * Returns 0 if write succeed, -EINVAL on bad parameters
 * -ETIME on timeout
 */
static int smi_reg_write(const char *devname, u8 phy_adr, u8 reg_ofs, u16 data)
{
	struct eth_device *dev = eth_get_dev_by_name(devname);
	struct mvgbe_device *dmvgbe = to_mvgbe(dev);
	struct mvgbe_registers *regs = dmvgbe->regs;
	u32 smi_reg;
	u32 timeout;

	/* Phyadr write request*/
	if (phy_adr == MV_PHY_ADR_REQUEST &&
			reg_ofs == MV_PHY_ADR_REQUEST) {
		MVGBE_REG_WR(regs->phyadr, data);
		return 0;
	}

	/* check parameters */
	if (phy_adr > PHYADR_MASK) {
		printf("Err..(%s) Invalid phy address\n", __FUNCTION__);
		return -EINVAL;
	}
	if (reg_ofs > PHYREG_MASK) {
		printf("Err..(%s) Invalid register offset\n", __FUNCTION__);
		return -EINVAL;
	}

	/* wait till the SMI is not busy */
	timeout = MVGBE_PHY_SMI_TIMEOUT;
	do {
		/* read smi register */
		smi_reg = MVGBE_REG_RD(MVGBE_SMI_REG);
		if (timeout-- == 0) {
			printf("Err..(%s) SMI busy timeout\n", __FUNCTION__);
			return -ETIME;
		}
	} while (smi_reg & MVGBE_PHY_SMI_BUSY_MASK);

	/* fill the phy addr and reg offset and write opcode and data */
	smi_reg = (data << MVGBE_PHY_SMI_DATA_OFFS);
	smi_reg |= (phy_adr << MVGBE_PHY_SMI_DEV_ADDR_OFFS)
		| (reg_ofs << MVGBE_SMI_REG_ADDR_OFFS);
	smi_reg &= ~MVGBE_PHY_SMI_OPCODE_READ;

	/* write the smi register */
	MVGBE_REG_WR(MVGBE_SMI_REG, smi_reg);

	return 0;
}
#endif

/* Stop and checks all queues */
static void stop_queue(u32 * qreg)
{
	u32 reg_data;

	reg_data = readl(qreg);

	if (reg_data & 0xFF) {
		/* Issue stop command for active channels only */
		writel((reg_data << 8), qreg);

		/* Wait for all queue activity to terminate. */
		do {
			/*
			 * Check port cause register that all queues
			 * are stopped
			 */
			reg_data = readl(qreg);
		}
		while (reg_data & 0xFF);
	}
}

/*
 * set_access_control - Config address decode parameters for Ethernet unit
 *
 * This function configures the address decode parameters for the Gigabit
 * Ethernet Controller according the given parameters struct.
 *
 * @regs	Register struct pointer.
 * @param	Address decode parameter struct.
 */
static void set_access_control(struct mvgbe_registers *regs,
				struct mvgbe_winparam *param)
{
	u32 access_prot_reg;

	/* Set access control register */
	access_prot_reg = MVGBE_REG_RD(regs->epap);
	/* clear window permission */
	access_prot_reg &= (~(3 << (param->win * 2)));
	access_prot_reg |= (param->access_ctrl << (param->win * 2));
	MVGBE_REG_WR(regs->epap, access_prot_reg);

	/* Set window Size reg (SR) */
	MVGBE_REG_WR(regs->barsz[param->win].size,
			(((param->size / 0x10000) - 1) << 16));

	/* Set window Base address reg (BA) */
	MVGBE_REG_WR(regs->barsz[param->win].bar,
			(param->target | param->attrib | param->base_addr));
	/* High address remap reg (HARR) */
	if (param->win < 4)
		MVGBE_REG_WR(regs->ha_remap[param->win], param->high_addr);

	/* Base address enable reg (BARER) */
	if (param->enable == 1)
		MVGBE_REG_BITS_RESET(regs->bare, (1 << param->win));
	else
		MVGBE_REG_BITS_SET(regs->bare, (1 << param->win));
}

static void set_dram_access(struct mvgbe_registers *regs)
{
	struct mvgbe_winparam win_param;
	int i;

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		/* Set access parameters for DRAM bank i */
		win_param.win = i;	/* Use Ethernet window i */
		/* Window target - DDR */
		win_param.target = MVGBE_TARGET_DRAM;
		/* Enable full access */
		win_param.access_ctrl = EWIN_ACCESS_FULL;
		win_param.high_addr = 0;
		/* Get bank base and size */
		win_param.base_addr = gd->bd->bi_dram[i].start;
		win_param.size = gd->bd->bi_dram[i].size;
		if (win_param.size == 0)
			win_param.enable = 0;
		else
			win_param.enable = 1;	/* Enable the access */

		/* Enable DRAM bank */
		switch (i) {
		case 0:
			win_param.attrib = EBAR_DRAM_CS0;
			break;
		case 1:
			win_param.attrib = EBAR_DRAM_CS1;
			break;
		case 2:
			win_param.attrib = EBAR_DRAM_CS2;
			break;
		case 3:
			win_param.attrib = EBAR_DRAM_CS3;
			break;
		default:
			/* invalid bank, disable access */
			win_param.enable = 0;
			win_param.attrib = 0;
			break;
		}
		/* Set the access control for address window(EPAPR) RD/WR */
		set_access_control(regs, &win_param);
	}
}

/*
 * port_init_mac_tables - Clear all entrance in the UC, SMC and OMC tables
 *
 * Go through all the DA filter tables (Unicast, Special Multicast & Other
 * Multicast) and set each entry to 0.
 */
static void port_init_mac_tables(struct mvgbe_registers *regs)
{
	int table_index;

	/* Clear DA filter unicast table (Ex_dFUT) */
	for (table_index = 0; table_index < 4; ++table_index)
		MVGBE_REG_WR(regs->dfut[table_index], 0);

	for (table_index = 0; table_index < 64; ++table_index) {
		/* Clear DA filter special multicast table (Ex_dFSMT) */
		MVGBE_REG_WR(regs->dfsmt[table_index], 0);
		/* Clear DA filter other multicast table (Ex_dFOMT) */
		MVGBE_REG_WR(regs->dfomt[table_index], 0);
	}
}

/*
 * port_uc_addr - This function Set the port unicast address table
 *
 * This function locates the proper entry in the Unicast table for the
 * specified MAC nibble and sets its properties according to function
 * parameters.
 * This function add/removes MAC addresses from the port unicast address
 * table.
 *
 * @uc_nibble	Unicast MAC Address last nibble.
 * @option      0 = Add, 1 = remove address.
 *
 * RETURN: 1 if output succeeded. 0 if option parameter is invalid.
 */
static int port_uc_addr(struct mvgbe_registers *regs, u8 uc_nibble,
			int option)
{
	u32 unicast_reg;
	u32 tbl_offset;
	u32 reg_offset;

	/* Locate the Unicast table entry */
	uc_nibble = (0xf & uc_nibble);
	/* Register offset from unicast table base */
	tbl_offset = (uc_nibble / 4);
	/* Entry offset within the above register */
	reg_offset = uc_nibble % 4;

	switch (option) {
	case REJECT_MAC_ADDR:
		/*
		 * Clear accepts frame bit at specified unicast
		 * DA table entry
		 */
		unicast_reg = MVGBE_REG_RD(regs->dfut[tbl_offset]);
		unicast_reg &= (0xFF << (8 * reg_offset));
		MVGBE_REG_WR(regs->dfut[tbl_offset], unicast_reg);
		break;
	case ACCEPT_MAC_ADDR:
		/* Set accepts frame bit at unicast DA filter table entry */
		unicast_reg = MVGBE_REG_RD(regs->dfut[tbl_offset]);
		unicast_reg &= (0xFF << (8 * reg_offset));
		unicast_reg |= ((0x01 | (RXUQ << 1)) << (8 * reg_offset));
		MVGBE_REG_WR(regs->dfut[tbl_offset], unicast_reg);
		break;
	default:
		return 0;
	}
	return 1;
}

/*
 * port_uc_addr_set - This function Set the port Unicast address.
 */
static void port_uc_addr_set(struct mvgbe_registers *regs, u8 * p_addr)
{
	u32 mac_h;
	u32 mac_l;

	mac_l = (p_addr[4] << 8) | (p_addr[5]);
	mac_h = (p_addr[0] << 24) | (p_addr[1] << 16) | (p_addr[2] << 8) |
		(p_addr[3] << 0);

	MVGBE_REG_WR(regs->macal, mac_l);
	MVGBE_REG_WR(regs->macah, mac_h);

	/* Accept frames of this address */
	port_uc_addr(regs, p_addr[5], ACCEPT_MAC_ADDR);
}

/*
 * mvgbe_init_rx_desc_ring - Curve a Rx chain desc list and buffer in memory.
 */
static void mvgbe_init_rx_desc_ring(struct mvgbe_device *dmvgbe)
{
	struct mvgbe_rxdesc *p_rx_desc;
	int i;

	/* initialize the Rx descriptors ring */
	p_rx_desc = dmvgbe->p_rxdesc;
	for (i = 0; i < RINGSZ; i++) {
		p_rx_desc->cmd_sts =
			MVGBE_BUFFER_OWNED_BY_DMA | MVGBE_RX_EN_INTERRUPT;
		p_rx_desc->buf_size = PKTSIZE_ALIGN;
		p_rx_desc->byte_cnt = 0;
		p_rx_desc->buf_ptr = dmvgbe->p_rxbuf + i * PKTSIZE_ALIGN;
		if (i == (RINGSZ - 1))
			p_rx_desc->nxtdesc_p = dmvgbe->p_rxdesc;
		else {
			p_rx_desc->nxtdesc_p = (struct mvgbe_rxdesc *)
				((u32) p_rx_desc + MV_RXQ_DESC_ALIGNED_SIZE);
			p_rx_desc = p_rx_desc->nxtdesc_p;
		}
	}
	dmvgbe->p_rxdesc_curr = dmvgbe->p_rxdesc;
}

static int mvgbe_init(struct eth_device *dev)
{
	struct mvgbe_device *dmvgbe = to_mvgbe(dev);
	struct mvgbe_registers *regs = dmvgbe->regs;
#if (defined (CONFIG_MII) || defined (CONFIG_CMD_MII)) \
	 && defined (CONFIG_SYS_FAULT_ECHO_LINK_DOWN)
	int i;
#endif
	/* setup RX rings */
	mvgbe_init_rx_desc_ring(dmvgbe);

	/* Clear the ethernet port interrupts */
	MVGBE_REG_WR(regs->ic, 0);
	MVGBE_REG_WR(regs->ice, 0);
	/* Unmask RX buffer and TX end interrupt */
	MVGBE_REG_WR(regs->pim, INT_CAUSE_UNMASK_ALL);
	/* Unmask phy and link status changes interrupts */
	MVGBE_REG_WR(regs->peim, INT_CAUSE_UNMASK_ALL_EXT);

	set_dram_access(regs);
	port_init_mac_tables(regs);
	port_uc_addr_set(regs, dmvgbe->dev.enetaddr);

	/* Assign port configuration and command. */
	MVGBE_REG_WR(regs->pxc, PRT_CFG_VAL);
	MVGBE_REG_WR(regs->pxcx, PORT_CFG_EXTEND_VALUE);
	MVGBE_REG_WR(regs->psc0, PORT_SERIAL_CONTROL_VALUE);

	/* Assign port SDMA configuration */
	MVGBE_REG_WR(regs->sdc, PORT_SDMA_CFG_VALUE);
	MVGBE_REG_WR(regs->tqx[0].qxttbc, QTKNBKT_DEF_VAL);
	MVGBE_REG_WR(regs->tqx[0].tqxtbc,
		(QMTBS_DEF_VAL << 16) | QTKNRT_DEF_VAL);
	/* Turn off the port/RXUQ bandwidth limitation */
	MVGBE_REG_WR(regs->pmtu, 0);

	/* Set maximum receive buffer to 9700 bytes */
	MVGBE_REG_WR(regs->psc0, MVGBE_MAX_RX_PACKET_9700BYTE
			| (MVGBE_REG_RD(regs->psc0) & MRU_MASK));

	/* Enable port initially */
	MVGBE_REG_BITS_SET(regs->psc0, MVGBE_SERIAL_PORT_EN);

	/*
	 * Set ethernet MTU for leaky bucket mechanism to 0 - this will
	 * disable the leaky bucket mechanism .
	 */
	MVGBE_REG_WR(regs->pmtu, 0);

	/* Assignment of Rx CRDB of given RXUQ */
	MVGBE_REG_WR(regs->rxcdp[RXUQ], (u32) dmvgbe->p_rxdesc_curr);
	/* ensure previous write is done before enabling Rx DMA */
	isb();
	/* Enable port Rx. */
	MVGBE_REG_WR(regs->rqc, (1 << RXUQ));

#if (defined (CONFIG_MII) || defined (CONFIG_CMD_MII)) \
	 && defined (CONFIG_SYS_FAULT_ECHO_LINK_DOWN)
	/* Wait up to 5s for the link status */
	for (i = 0; i < 5; i++) {
		u16 phyadr;

		miiphy_read(dev->name, MV_PHY_ADR_REQUEST,
				MV_PHY_ADR_REQUEST, &phyadr);
		/* Return if we get link up */
		if (miiphy_link(dev->name, phyadr))
			return 0;
		udelay(1000000);
	}

	printf("No link on %s\n", dev->name);
	return -1;
#endif
	return 0;
}

static int mvgbe_halt(struct eth_device *dev)
{
	struct mvgbe_device *dmvgbe = to_mvgbe(dev);
	struct mvgbe_registers *regs = dmvgbe->regs;

	/* Disable all gigE address decoder */
	MVGBE_REG_WR(regs->bare, 0x3f);

	stop_queue(&regs->tqc);
	stop_queue(&regs->rqc);

	/* Disable port */
	MVGBE_REG_BITS_RESET(regs->psc0, MVGBE_SERIAL_PORT_EN);
	/* Set port is not reset */
	MVGBE_REG_BITS_RESET(regs->psc1, 1 << 4);
#ifdef CONFIG_SYS_MII_MODE
	/* Set MMI interface up */
	MVGBE_REG_BITS_RESET(regs->psc1, 1 << 3);
#endif
	/* Disable & mask ethernet port interrupts */
	MVGBE_REG_WR(regs->ic, 0);
	MVGBE_REG_WR(regs->ice, 0);
	MVGBE_REG_WR(regs->pim, 0);
	MVGBE_REG_WR(regs->peim, 0);

	return 0;
}

static int mvgbe_write_hwaddr(struct eth_device *dev)
{
	struct mvgbe_device *dmvgbe = to_mvgbe(dev);
	struct mvgbe_registers *regs = dmvgbe->regs;

	/* Programs net device MAC address after initialization */
	port_uc_addr_set(regs, dmvgbe->dev.enetaddr);
	return 0;
}

static int mvgbe_send(struct eth_device *dev, void *dataptr, int datasize)
{
	struct mvgbe_device *dmvgbe = to_mvgbe(dev);
	struct mvgbe_registers *regs = dmvgbe->regs;
	struct mvgbe_txdesc *p_txdesc = dmvgbe->p_txdesc;
	void *p = (void *)dataptr;
	u32 cmd_sts;
	u32 txuq0_reg_addr;

	/* Copy buffer if it's misaligned */
	if ((u32) dataptr & 0x07) {
		if (datasize > PKTSIZE_ALIGN) {
			printf("Non-aligned data too large (%d)\n",
					datasize);
			return -1;
		}

		memcpy(dmvgbe->p_aligned_txbuf, p, datasize);
		p = dmvgbe->p_aligned_txbuf;
	}

	p_txdesc->cmd_sts = MVGBE_ZERO_PADDING | MVGBE_GEN_CRC;
	p_txdesc->cmd_sts |= MVGBE_TX_FIRST_DESC | MVGBE_TX_LAST_DESC;
	p_txdesc->cmd_sts |= MVGBE_BUFFER_OWNED_BY_DMA;
	p_txdesc->cmd_sts |= MVGBE_TX_EN_INTERRUPT;
	p_txdesc->buf_ptr = (u8 *) p;
	p_txdesc->byte_cnt = datasize;

	/* Set this tc desc as zeroth TXUQ */
	txuq0_reg_addr = (u32)&regs->tcqdp[TXUQ];
	writel((u32) p_txdesc, txuq0_reg_addr);

	/* ensure tx desc writes above are performed before we start Tx DMA */
	isb();

	/* Apply send command using zeroth TXUQ */
	MVGBE_REG_WR(regs->tqc, (1 << TXUQ));

	/*
	 * wait for packet xmit completion
	 */
	cmd_sts = readl(&p_txdesc->cmd_sts);
	while (cmd_sts & MVGBE_BUFFER_OWNED_BY_DMA) {
		/* return fail if error is detected */
		if ((cmd_sts & (MVGBE_ERROR_SUMMARY | MVGBE_TX_LAST_FRAME)) ==
				(MVGBE_ERROR_SUMMARY | MVGBE_TX_LAST_FRAME) &&
				cmd_sts & (MVGBE_UR_ERROR | MVGBE_RL_ERROR)) {
			printf("Err..(%s) in xmit packet\n", __FUNCTION__);
			return -1;
		}
		cmd_sts = readl(&p_txdesc->cmd_sts);
	};
	return 0;
}

static int mvgbe_recv(struct eth_device *dev)
{
	struct mvgbe_device *dmvgbe = to_mvgbe(dev);
	struct mvgbe_rxdesc *p_rxdesc_curr = dmvgbe->p_rxdesc_curr;
	u32 cmd_sts;
	u32 timeout = 0;
	u32 rxdesc_curr_addr;

	/* wait untill rx packet available or timeout */
	do {
		if (timeout < MVGBE_PHY_SMI_TIMEOUT)
			timeout++;
		else {
			debug("%s time out...\n", __FUNCTION__);
			return -1;
		}
	} while (readl(&p_rxdesc_curr->cmd_sts) & MVGBE_BUFFER_OWNED_BY_DMA);

	if (p_rxdesc_curr->byte_cnt != 0) {
		debug("%s: Received %d byte Packet @ 0x%x (cmd_sts= %08x)\n",
			__FUNCTION__, (u32) p_rxdesc_curr->byte_cnt,
			(u32) p_rxdesc_curr->buf_ptr,
			(u32) p_rxdesc_curr->cmd_sts);
	}

	/*
	 * In case received a packet without first/last bits on
	 * OR the error summary bit is on,
	 * the packets needs to be dropeed.
	 */
	cmd_sts = readl(&p_rxdesc_curr->cmd_sts);

	if ((cmd_sts &
		(MVGBE_RX_FIRST_DESC | MVGBE_RX_LAST_DESC))
		!= (MVGBE_RX_FIRST_DESC | MVGBE_RX_LAST_DESC)) {

		printf("Err..(%s) Dropping packet spread on"
			" multiple descriptors\n", __FUNCTION__);

	} else if (cmd_sts & MVGBE_ERROR_SUMMARY) {

		printf("Err..(%s) Dropping packet with errors\n",
			__FUNCTION__);

	} else {
		/* !!! call higher layer processing */
		debug("%s: Sending Received packet to"
			" upper layer (NetReceive)\n", __FUNCTION__);

		/* let the upper layer handle the packet */
		NetReceive((p_rxdesc_curr->buf_ptr + RX_BUF_OFFSET),
			(int)(p_rxdesc_curr->byte_cnt - RX_BUF_OFFSET));
	}
	/*
	 * free these descriptors and point next in the ring
	 */
	p_rxdesc_curr->cmd_sts =
		MVGBE_BUFFER_OWNED_BY_DMA | MVGBE_RX_EN_INTERRUPT;
	p_rxdesc_curr->buf_size = PKTSIZE_ALIGN;
	p_rxdesc_curr->byte_cnt = 0;

	rxdesc_curr_addr = (u32)&dmvgbe->p_rxdesc_curr;
	writel((unsigned)p_rxdesc_curr->nxtdesc_p, rxdesc_curr_addr);

	return 0;
}

int mvgbe_initialize(bd_t *bis)
{
	struct mvgbe_device *dmvgbe;
	struct eth_device *dev;
	int devnum;
	u8 used_ports[MAX_MVGBE_DEVS] = CONFIG_MVGBE_PORTS;

	for (devnum = 0; devnum < MAX_MVGBE_DEVS; devnum++) {
		/*skip if port is configured not to use */
		if (used_ports[devnum] == 0)
			continue;

		dmvgbe = malloc(sizeof(struct mvgbe_device));

		if (!dmvgbe)
			goto error1;

		memset(dmvgbe, 0, sizeof(struct mvgbe_device));

		dmvgbe->p_rxdesc =
			(struct mvgbe_rxdesc *)memalign(PKTALIGN,
			MV_RXQ_DESC_ALIGNED_SIZE*RINGSZ + 1);

		if (!dmvgbe->p_rxdesc)
			goto error2;

		dmvgbe->p_rxbuf = (u8 *) memalign(PKTALIGN,
			RINGSZ*PKTSIZE_ALIGN + 1);

		if (!dmvgbe->p_rxbuf)
			goto error3;

		dmvgbe->p_aligned_txbuf = memalign(8, PKTSIZE_ALIGN);

		if (!dmvgbe->p_aligned_txbuf)
			goto error4;

		dmvgbe->p_txdesc = (struct mvgbe_txdesc *) memalign(
			PKTALIGN, sizeof(struct mvgbe_txdesc) + 1);

		if (!dmvgbe->p_txdesc) {
			free(dmvgbe->p_aligned_txbuf);
error4:
			free(dmvgbe->p_rxbuf);
error3:
			free(dmvgbe->p_rxdesc);
error2:
			free(dmvgbe);
error1:
			printf("Err.. %s Failed to allocate memory\n",
				__FUNCTION__);
			return -1;
		}

		dev = &dmvgbe->dev;

		/* must be less than sizeof(dev->name) */
		sprintf(dev->name, "egiga%d", devnum);

		switch (devnum) {
		case 0:
			dmvgbe->regs = (void *)MVGBE0_BASE;
			break;
#if defined(MVGBE1_BASE)
		case 1:
			dmvgbe->regs = (void *)MVGBE1_BASE;
			break;
#endif
		default:	/* this should never happen */
			printf("Err..(%s) Invalid device number %d\n",
				__FUNCTION__, devnum);
			return -1;
		}

		dev->init = (void *)mvgbe_init;
		dev->halt = (void *)mvgbe_halt;
		dev->send = (void *)mvgbe_send;
		dev->recv = (void *)mvgbe_recv;
		dev->write_hwaddr = (void *)mvgbe_write_hwaddr;

		eth_register(dev);

#if defined(CONFIG_MII) || defined(CONFIG_CMD_MII)
		miiphy_register(dev->name, smi_reg_read, smi_reg_write);
		/* Set phy address of the port */
		miiphy_write(dev->name, MV_PHY_ADR_REQUEST,
				MV_PHY_ADR_REQUEST, PHY_BASE_ADR + devnum);
#endif
	}
	return 0;
}
