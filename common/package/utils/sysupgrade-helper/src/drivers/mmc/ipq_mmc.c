/*
 * Copyright (c) 2014 The Linux Foundation. All rights reserved.

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

#include <config.h>
#include <common.h>
#include <command.h>
#include <mmc.h>
#include <part.h>
#include <malloc.h>
#include <asm/io.h>
#include <asm/arch-ipq806x/clock.h>
#include "../../board/qcom/ipq806x_cdp/ipq806x_cdp.h"
#include "ipq_mmc.h"

static inline void ipq_reg_wr_delay(ipq_mmc  *host)
{
	while (readl(host->base + MCI_STATUS2) & MCI_MCLK_REG_WR_ACTIVE);
}

static inline void
ipq_start_command_exec(ipq_mmc  *host, struct mmc_cmd *cmd)
{
	unsigned int c = 0;
	unsigned int arg = cmd->cmdarg;

	writel(MCI_CLEAR_STATIC_MASK, host->base + MMCICLEAR);

	ipq_reg_wr_delay(host);

	c |= (cmd->cmdidx | MCI_CPSM_ENABLE);

	if (cmd->resp_type & MMC_RSP_PRESENT) {
		if (cmd->resp_type & MMC_RSP_136)
			c |= MCI_CPSM_LONGRSP;
		c |= MCI_CPSM_RESPONSE;
	}

	if ((cmd->resp_type & MMC_RSP_R1b) == MMC_RSP_R1b) {
		c |= MCI_CPSM_PROGENA;
	}

	writel(arg, host->base + MMCIARGUMENT);
	writel(c, host->base + MMCICOMMAND);
	ipq_reg_wr_delay(host);
}

static int
ipq_start_command(ipq_mmc *host, struct mmc_cmd *cmd)
{
	unsigned int status = 0;
	int rc = 0;

	ipq_start_command_exec(host, cmd);

	while (readl(host->base + MMCISTATUS) & MCI_CMDACTIVE);

	status = readl(host->base + MMCISTATUS);

	if ((cmd->resp_type != MMC_RSP_NONE)) {

		if (status & MCI_CMDTIMEOUT) {
			rc = TIMEOUT;
		}
		/*  MMC_CMD_SEND_OP_COND response doesn't have CRC. */
		if ((status & MCI_CMDCRCFAIL) && (cmd->cmdidx != MMC_CMD_SEND_OP_COND)) {
			rc = UNUSABLE_ERR;
		}
		cmd->response[0] = readl(host->base + MMCIRESPONSE0);
		/*
		 * Read rest of the response registers only if
		 * long response is expected for this command
		 */
		if (cmd->resp_type & MMC_RSP_136) {
			cmd->response[1] = readl(host->base + MMCIRESPONSE1);
			cmd->response[2] = readl(host->base + MMCIRESPONSE2);
			cmd->response[3] = readl(host->base + MMCIRESPONSE3);
		}
	}

	writel(MCI_CLEAR_STATIC_MASK, host->base + MMCICLEAR);
	ipq_reg_wr_delay(host);

	return rc;
}


static int
ipq_pio_read(ipq_mmc *host, char *buffer, unsigned int remain)
{
	unsigned int *ptr = (unsigned int *) buffer;
	unsigned int status = 0;
	unsigned int count = 0;
	unsigned int error = MCI_DATACRCFAIL | MCI_DATATIMEOUT | MCI_RXOVERRUN;
	unsigned int i;


	do {
		status = readl(host->base + MMCISTATUS);
		udelay(1);

		if (status & error) {
			break;
		}

		if (status & MCI_RXDATAAVLBL ) {
			unsigned rd_cnt = 1;

			if (status & MCI_RXFIFOHALFFULL) {
				rd_cnt = MCI_HFIFO_COUNT;
			}

			for (i = 0; i < rd_cnt; i++) {
				*ptr = readl(host->base + MMCIFIFO +
					(count % MCI_FIFOSIZE));
				ptr++;
				count += sizeof(unsigned int);
			}

			if (count == remain)
				break;

		} else if (status & MCI_DATAEND) {
			break;
		}
	} while (1);

	return count;
}

static int
ipq_pio_write(ipq_mmc *host, const char *buffer,
		  unsigned int remain)
{
	unsigned int *ptr = (unsigned int *) buffer;
	unsigned int status = 0;
	unsigned int count = 0;
	unsigned int error = MCI_DATACRCFAIL | MCI_DATATIMEOUT | MCI_TXUNDERRUN;
	unsigned int bcnt = 0;
	unsigned int sz = 0;
	int i;

	do {
		status = readl(host->base + MMCISTATUS);

		bcnt = remain - count;

		if (!bcnt)
			break;

		if ((status & MCI_TXFIFOEMPTY) ||
			(status & MCI_TXFIFOHALFEMPTY)) {

			sz = ((bcnt >> 2) >  MCI_HFIFO_COUNT) \
					? MCI_HFIFO_COUNT : (bcnt >> 2);

			for (i = 0; i < sz; i++) {
				writel(*ptr, host->base + MMCIFIFO);
				ptr++;
				count += sizeof(unsigned int);
			}
		}
	} while (1);

	do {
		status = readl(host->base + MMCISTATUS);

		if (status & error) {
			count = status;
			break;
		}
	} while (!(status & MCI_DATAEND));

	return count;
}

static int
ipq_mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
	unsigned int datactrl = 0;
	ipq_mmc *host = mmc->priv;
	unsigned int xfer_size ;
	int status = 0;

	if (data) {
		writel((readl(host->base + MMCICLOCK) | MCI_CLK_FLOWENA),
						host->base + MMCICLOCK);
		xfer_size = data->blocksize * data->blocks;
		datactrl = MCI_DPSM_ENABLE | (data->blocksize << 4);
		writel(0xffffffff, host->base + MMCIDATATIMER);

		if (data->flags & MMC_DATA_READ)
			datactrl |= (MCI_DPSM_DIRECTION | MCI_RX_DATA_PEND);

		writel(xfer_size, host->base + MMCIDATALENGTH);
		writel(datactrl, host->base + MMCIDATACTRL);
		ipq_reg_wr_delay(host);

		status = ipq_start_command(host, cmd);

		if (data->flags & MMC_DATA_READ) {
			ipq_pio_read(host, data->dest, xfer_size);
		} else {
			ipq_pio_write(host, data->src, xfer_size);
		}
		writel(0, host->base + MMCIDATACTRL);
		ipq_reg_wr_delay(host);
	} else if (cmd) {
		status = ipq_start_command(host, cmd);
	}

	return status;
}


void ipq_set_ios(struct mmc *mmc)
{
	ipq_mmc *host = mmc->priv;
	u32 clk = 0, pwr = 0;
	int mode;

	if (mmc->clock <= mmc->f_min) {
		mode = MMC_IDENTIFY_MODE;
	} else {
		mode = MMC_DATA_TRANSFER_MODE;
	}

	if (mode != host->clk_mode) {
		host->clk_mode = mode;
		emmc_clock_config(host->clk_mode);
	}

	pwr = MCI_PWR_UP | MCI_PWR_ON;

	writel(pwr, host->base + MMCIPOWER);
	ipq_reg_wr_delay(host);
	clk = readl(host->base + MMCICLOCK);

	clk |= MCI_CLK_ENABLE;
	clk |= MCI_CLK_SELECTIN;
	clk |= MCI_CLK_FLOWENA;
	/* feedback clock */
	clk |= (2 << 14);

	if (mmc->bus_width == 1) {
		 clk |= MCI_CLK_WIDEBUS_1;
	} else if (mmc->bus_width == 4) {
		clk |= MCI_CLK_WIDEBUS_4;
	} else if (mmc->bus_width == 8) {
		 clk |= MCI_CLK_WIDEBUS_8;
	}

	/* Select free running MCLK as input clock of cm_dll_sdc4 */
	clk |= (2 << 23);

	/* Don't write into registers if clocks are disabled */
	writel(clk, host->base + MMCICLOCK);
	ipq_reg_wr_delay(host);

	writel((readl(host->base + MMCICLOCK) | MCI_CLK_PWRSAVE), host->base + MMCICLOCK);
	ipq_reg_wr_delay(host);
}

int ipq_mmc_start (struct mmc *mmc)
{

	return 0;
}

int ipq_mmc_init(bd_t *bis, ipq_mmc *host)
{
	struct mmc *mmc;

	mmc = malloc(sizeof(struct mmc));
	if (NULL == mmc)
		return -1;

	memset(mmc, 0, sizeof(struct mmc));

	snprintf(mmc->name, sizeof(mmc->name), "ipq_mmc");
	mmc->priv = host;
	mmc->send_cmd = ipq_mmc_send_cmd;
	mmc->set_ios = ipq_set_ios;
	mmc->init = ipq_mmc_start;
	mmc->getcd = NULL;

	mmc->f_min = 400000;
	mmc->f_max = 52000000;

	mmc->b_max = 512;
	/* voltage either 2.7-3.6V or 1.70 -1.95V  */
	mmc->voltages = 0x40FF8080;
	mmc->host_caps = MMC_MODE_8BIT;
	mmc->host_caps |= MMC_MODE_HC;

	mmc_register(mmc);
	host->mmc = mmc;
	host->dev_num = mmc->block_dev.dev;

	return 0;
}
