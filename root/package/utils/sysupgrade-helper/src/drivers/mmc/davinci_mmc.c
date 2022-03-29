/*
 * Davinci MMC Controller Driver
 *
 * Copyright (C) 2010 Texas Instruments Incorporated
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
#include <command.h>
#include <mmc.h>
#include <part.h>
#include <malloc.h>
#include <asm/io.h>
#include <asm/arch/sdmmc_defs.h>

#define DAVINCI_MAX_BLOCKS	(32)
#define WATCHDOG_COUNT		(100000)

#define get_val(addr)		REG(addr)
#define set_val(addr, val)	REG(addr) = (val)
#define set_bit(addr, val)	set_val((addr), (get_val(addr) | (val)))
#define clear_bit(addr, val)	set_val((addr), (get_val(addr) & ~(val)))

/* Set davinci clock prescalar value based on the required clock in HZ */
static void dmmc_set_clock(struct mmc *mmc, uint clock)
{
	struct davinci_mmc *host = mmc->priv;
	struct davinci_mmc_regs *regs = host->reg_base;
	uint clkrt, sysclk2, act_clock;

	if (clock < mmc->f_min)
		clock = mmc->f_min;
	if (clock > mmc->f_max)
		clock = mmc->f_max;

	set_val(&regs->mmcclk, 0);
	sysclk2 = host->input_clk;
	clkrt = (sysclk2 / (2 * clock)) - 1;

	/* Calculate the actual clock for the divider used */
	act_clock = (sysclk2 / (2 * (clkrt + 1)));

	/* Adjust divider if actual clock exceeds the required clock */
	if (act_clock > clock)
		clkrt++;

	/* check clock divider boundary and correct it */
	if (clkrt > 0xFF)
		clkrt = 0xFF;

	set_val(&regs->mmcclk, (clkrt | MMCCLK_CLKEN));
}

/* Status bit wait loop for MMCST1 */
static int
dmmc_wait_fifo_status(volatile struct davinci_mmc_regs *regs, uint status)
{
	uint wdog = WATCHDOG_COUNT;

	while (--wdog && ((get_val(&regs->mmcst1) & status) != status))
		udelay(10);

	if (!(get_val(&regs->mmcctl) & MMCCTL_WIDTH_4_BIT))
		udelay(100);

	if (wdog == 0)
		return COMM_ERR;

	return 0;
}

/* Busy bit wait loop for MMCST1 */
static int dmmc_busy_wait(volatile struct davinci_mmc_regs *regs)
{
	uint wdog = WATCHDOG_COUNT;

	while (--wdog && (get_val(&regs->mmcst1) & MMCST1_BUSY))
		udelay(10);

	if (wdog == 0)
		return COMM_ERR;

	return 0;
}

/* Status bit wait loop for MMCST0 - Checks for error bits as well */
static int dmmc_check_status(volatile struct davinci_mmc_regs *regs,
		uint *cur_st, uint st_ready, uint st_error)
{
	uint wdog = WATCHDOG_COUNT;
	uint mmcstatus = *cur_st;

	while (wdog--) {
		if (mmcstatus & st_ready) {
			*cur_st = mmcstatus;
			mmcstatus = get_val(&regs->mmcst1);
			return 0;
		} else if (mmcstatus & st_error) {
			if (mmcstatus & MMCST0_TOUTRS)
				return TIMEOUT;
			printf("[ ST0 ERROR %x]\n", mmcstatus);
			/*
			 * Ignore CRC errors as some MMC cards fail to
			 * initialize on DM365-EVM on the SD1 slot
			 */
			if (mmcstatus & MMCST0_CRCRS)
				return 0;
			return COMM_ERR;
		}
		udelay(10);

		mmcstatus = get_val(&regs->mmcst0);
	}

	printf("Status %x Timeout ST0:%x ST1:%x\n", st_ready, mmcstatus,
			get_val(&regs->mmcst1));
	return COMM_ERR;
}

/*
 * Sends a command out on the bus.  Takes the mmc pointer,
 * a command pointer, and an optional data pointer.
 */
static int
dmmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
	struct davinci_mmc *host = mmc->priv;
	volatile struct davinci_mmc_regs *regs = host->reg_base;
	uint mmcstatus, status_rdy, status_err;
	uint i, cmddata, bytes_left = 0;
	int fifo_words, fifo_bytes, err;
	char *data_buf = NULL;

	/* Clear status registers */
	mmcstatus = get_val(&regs->mmcst0);
	fifo_words = (host->version == MMC_CTLR_VERSION_2) ? 16 : 8;
	fifo_bytes = fifo_words << 2;

	/* Wait for any previous busy signal to be cleared */
	dmmc_busy_wait(regs);

	cmddata = cmd->cmdidx;
	cmddata |= MMCCMD_PPLEN;

	/* Send init clock for CMD0 */
	if (cmd->cmdidx == MMC_CMD_GO_IDLE_STATE)
		cmddata |= MMCCMD_INITCK;

	switch (cmd->resp_type) {
	case MMC_RSP_R1b:
		cmddata |= MMCCMD_BSYEXP;
		/* Fall-through */
	case MMC_RSP_R1:    /* R1, R1b, R5, R6, R7 */
		cmddata |= MMCCMD_RSPFMT_R1567;
		break;
	case MMC_RSP_R2:
		cmddata |= MMCCMD_RSPFMT_R2;
		break;
	case MMC_RSP_R3: /* R3, R4 */
		cmddata |= MMCCMD_RSPFMT_R3;
		break;
	}

	set_val(&regs->mmcim, 0);

	if (data) {
		/* clear previous data transfer if any and set new one */
		bytes_left = (data->blocksize * data->blocks);

		/* Reset FIFO - Always use 32 byte fifo threshold */
		set_val(&regs->mmcfifoctl,
				(MMCFIFOCTL_FIFOLEV | MMCFIFOCTL_FIFORST));

		if (host->version == MMC_CTLR_VERSION_2)
			cmddata |= MMCCMD_DMATRIG;

		cmddata |= MMCCMD_WDATX;
		if (data->flags == MMC_DATA_READ) {
			set_val(&regs->mmcfifoctl, MMCFIFOCTL_FIFOLEV);
		} else if (data->flags == MMC_DATA_WRITE) {
			set_val(&regs->mmcfifoctl,
					(MMCFIFOCTL_FIFOLEV |
					 MMCFIFOCTL_FIFODIR));
			cmddata |= MMCCMD_DTRW;
		}

		set_val(&regs->mmctod, 0xFFFF);
		set_val(&regs->mmcnblk, (data->blocks & MMCNBLK_NBLK_MASK));
		set_val(&regs->mmcblen, (data->blocksize & MMCBLEN_BLEN_MASK));

		if (data->flags == MMC_DATA_WRITE) {
			uint val;
			data_buf = (char *)data->src;
			/* For write, fill FIFO with data before issue of CMD */
			for (i = 0; (i < fifo_words) && bytes_left; i++) {
				memcpy((char *)&val, data_buf, 4);
				set_val(&regs->mmcdxr, val);
				data_buf += 4;
				bytes_left -= 4;
			}
		}
	} else {
		set_val(&regs->mmcblen, 0);
		set_val(&regs->mmcnblk, 0);
	}

	set_val(&regs->mmctor, 0x1FFF);

	/* Send the command */
	set_val(&regs->mmcarghl, cmd->cmdarg);
	set_val(&regs->mmccmd, cmddata);

	status_rdy = MMCST0_RSPDNE;
	status_err = (MMCST0_TOUTRS | MMCST0_TOUTRD |
			MMCST0_CRCWR | MMCST0_CRCRD);
	if (cmd->resp_type & MMC_RSP_CRC)
		status_err |= MMCST0_CRCRS;

	mmcstatus = get_val(&regs->mmcst0);
	err = dmmc_check_status(regs, &mmcstatus, status_rdy, status_err);
	if (err)
		return err;

	/* For R1b wait for busy done */
	if (cmd->resp_type == MMC_RSP_R1b)
		dmmc_busy_wait(regs);

	/* Collect response from controller for specific commands */
	if (mmcstatus & MMCST0_RSPDNE) {
		/* Copy the response to the response buffer */
		if (cmd->resp_type & MMC_RSP_136) {
			cmd->response[0] = get_val(&regs->mmcrsp67);
			cmd->response[1] = get_val(&regs->mmcrsp45);
			cmd->response[2] = get_val(&regs->mmcrsp23);
			cmd->response[3] = get_val(&regs->mmcrsp01);
		} else if (cmd->resp_type & MMC_RSP_PRESENT) {
			cmd->response[0] = get_val(&regs->mmcrsp67);
		}
	}

	if (data == NULL)
		return 0;

	if (data->flags == MMC_DATA_READ) {
		/* check for DATDNE along with DRRDY as the controller might
		 * set the DATDNE without DRRDY for smaller transfers with
		 * less than FIFO threshold bytes
		 */
		status_rdy = MMCST0_DRRDY | MMCST0_DATDNE;
		status_err = MMCST0_TOUTRD | MMCST0_CRCRD;
		data_buf = data->dest;
	} else {
		status_rdy = MMCST0_DXRDY | MMCST0_DATDNE;
		status_err = MMCST0_CRCWR;
	}

	/* Wait until all of the blocks are transferred */
	while (bytes_left) {
		err = dmmc_check_status(regs, &mmcstatus, status_rdy,
				status_err);
		if (err)
			return err;

		if (data->flags == MMC_DATA_READ) {
			/*
			 * MMC controller sets the Data receive ready bit
			 * (DRRDY) in MMCST0 even before the entire FIFO is
			 * full. This results in erratic behavior if we start
			 * reading the FIFO soon after DRRDY.  Wait for the
			 * FIFO full bit in MMCST1 for proper FIFO clearing.
			 */
			if (bytes_left > fifo_bytes)
				dmmc_wait_fifo_status(regs, 0x4a);
			else if (bytes_left == fifo_bytes)
				dmmc_wait_fifo_status(regs, 0x40);

			for (i = 0; bytes_left && (i < fifo_words); i++) {
				cmddata = get_val(&regs->mmcdrr);
				memcpy(data_buf, (char *)&cmddata, 4);
				data_buf += 4;
				bytes_left -= 4;
			}
		} else {
			/*
			 * MMC controller sets the Data transmit ready bit
			 * (DXRDY) in MMCST0 even before the entire FIFO is
			 * empty. This results in erratic behavior if we start
			 * writing the FIFO soon after DXRDY.  Wait for the
			 * FIFO empty bit in MMCST1 for proper FIFO clearing.
			 */
			dmmc_wait_fifo_status(regs, MMCST1_FIFOEMP);
			for (i = 0; bytes_left && (i < fifo_words); i++) {
				memcpy((char *)&cmddata, data_buf, 4);
				set_val(&regs->mmcdxr, cmddata);
				data_buf += 4;
				bytes_left -= 4;
			}
			dmmc_busy_wait(regs);
		}
	}

	err = dmmc_check_status(regs, &mmcstatus, MMCST0_DATDNE, status_err);
	if (err)
		return err;

	return 0;
}

/* Initialize Davinci MMC controller */
static int dmmc_init(struct mmc *mmc)
{
	struct davinci_mmc *host = mmc->priv;
	struct davinci_mmc_regs *regs = host->reg_base;

	/* Clear status registers explicitly - soft reset doesn't clear it
	 * If Uboot is invoked from UBL with SDMMC Support, the status
	 * registers can have uncleared bits
	 */
	get_val(&regs->mmcst0);
	get_val(&regs->mmcst1);

	/* Hold software reset */
	set_bit(&regs->mmcctl, MMCCTL_DATRST);
	set_bit(&regs->mmcctl, MMCCTL_CMDRST);
	udelay(10);

	set_val(&regs->mmcclk, 0x0);
	set_val(&regs->mmctor, 0x1FFF);
	set_val(&regs->mmctod, 0xFFFF);

	/* Clear software reset */
	clear_bit(&regs->mmcctl, MMCCTL_DATRST);
	clear_bit(&regs->mmcctl, MMCCTL_CMDRST);

	udelay(10);

	/* Reset FIFO - Always use the maximum fifo threshold */
	set_val(&regs->mmcfifoctl, (MMCFIFOCTL_FIFOLEV | MMCFIFOCTL_FIFORST));
	set_val(&regs->mmcfifoctl, MMCFIFOCTL_FIFOLEV);

	return 0;
}

/* Set buswidth or clock as indicated by the GENERIC_MMC framework */
static void dmmc_set_ios(struct mmc *mmc)
{
	struct davinci_mmc *host = mmc->priv;
	struct davinci_mmc_regs *regs = host->reg_base;

	/* Set the bus width */
	if (mmc->bus_width == 4)
		set_bit(&regs->mmcctl, MMCCTL_WIDTH_4_BIT);
	else
		clear_bit(&regs->mmcctl, MMCCTL_WIDTH_4_BIT);

	/* Set clock speed */
	if (mmc->clock)
		dmmc_set_clock(mmc, mmc->clock);
}

/* Called from board_mmc_init during startup. Can be called multiple times
 * depending on the number of slots available on board and controller
 */
int davinci_mmc_init(bd_t *bis, struct davinci_mmc *host)
{
	struct mmc *mmc;

	mmc = malloc(sizeof(struct mmc));
	memset(mmc, 0, sizeof(struct mmc));

	sprintf(mmc->name, "davinci");
	mmc->priv = host;
	mmc->send_cmd = dmmc_send_cmd;
	mmc->set_ios = dmmc_set_ios;
	mmc->init = dmmc_init;
	mmc->getcd = NULL;

	mmc->f_min = 200000;
	mmc->f_max = 25000000;
	mmc->voltages = host->voltages;
	mmc->host_caps = host->host_caps;

	mmc->b_max = DAVINCI_MAX_BLOCKS;

	mmc_register(mmc);

	return 0;
}
