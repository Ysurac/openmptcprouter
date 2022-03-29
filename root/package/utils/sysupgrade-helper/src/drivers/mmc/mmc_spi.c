/*
 * generic mmc spi driver
 *
 * Copyright (C) 2010 Thomas Chou <thomas@wytron.com.tw>
 * Licensed under the GPL-2 or later.
 */
#include <common.h>
#include <malloc.h>
#include <part.h>
#include <mmc.h>
#include <spi.h>
#include <crc.h>
#include <linux/crc7.h>
#include <linux/byteorder/swab.h>

/* MMC/SD in SPI mode reports R1 status always */
#define R1_SPI_IDLE		(1 << 0)
#define R1_SPI_ERASE_RESET	(1 << 1)
#define R1_SPI_ILLEGAL_COMMAND	(1 << 2)
#define R1_SPI_COM_CRC		(1 << 3)
#define R1_SPI_ERASE_SEQ	(1 << 4)
#define R1_SPI_ADDRESS		(1 << 5)
#define R1_SPI_PARAMETER	(1 << 6)
/* R1 bit 7 is always zero, reuse this bit for error */
#define R1_SPI_ERROR		(1 << 7)

/* Response tokens used to ack each block written: */
#define SPI_MMC_RESPONSE_CODE(x)	((x) & 0x1f)
#define SPI_RESPONSE_ACCEPTED		((2 << 1)|1)
#define SPI_RESPONSE_CRC_ERR		((5 << 1)|1)
#define SPI_RESPONSE_WRITE_ERR		((6 << 1)|1)

/* Read and write blocks start with these tokens and end with crc;
 * on error, read tokens act like a subset of R2_SPI_* values.
 */
#define SPI_TOKEN_SINGLE	0xfe	/* single block r/w, multiblock read */
#define SPI_TOKEN_MULTI_WRITE	0xfc	/* multiblock write */
#define SPI_TOKEN_STOP_TRAN	0xfd	/* terminate multiblock write */

/* MMC SPI commands start with a start bit "0" and a transmit bit "1" */
#define MMC_SPI_CMD(x) (0x40 | (x & 0x3f))

/* bus capability */
#define MMC_SPI_VOLTAGE (MMC_VDD_32_33 | MMC_VDD_33_34)
#define MMC_SPI_MIN_CLOCK 400000 /* 400KHz to meet MMC spec */

/* timeout value */
#define CTOUT 8
#define RTOUT 3000000 /* 1 sec */
#define WTOUT 3000000 /* 1 sec */

static uint mmc_spi_sendcmd(struct mmc *mmc, ushort cmdidx, u32 cmdarg)
{
	struct spi_slave *spi = mmc->priv;
	u8 cmdo[7];
	u8 r1;
	int i;
	cmdo[0] = 0xff;
	cmdo[1] = MMC_SPI_CMD(cmdidx);
	cmdo[2] = cmdarg >> 24;
	cmdo[3] = cmdarg >> 16;
	cmdo[4] = cmdarg >> 8;
	cmdo[5] = cmdarg;
	cmdo[6] = (crc7(0, &cmdo[1], 5) << 1) | 0x01;
	spi_xfer(spi, sizeof(cmdo) * 8, cmdo, NULL, 0);
	for (i = 0; i < CTOUT; i++) {
		spi_xfer(spi, 1 * 8, NULL, &r1, 0);
		if (i && (r1 & 0x80) == 0) /* r1 response */
			break;
	}
	debug("%s:cmd%d resp%d %x\n", __func__, cmdidx, i, r1);
	return r1;
}

static uint mmc_spi_readdata(struct mmc *mmc, void *xbuf,
				u32 bcnt, u32 bsize)
{
	struct spi_slave *spi = mmc->priv;
	u8 *buf = xbuf;
	u8 r1;
	u16 crc;
	int i;
	while (bcnt--) {
		for (i = 0; i < RTOUT; i++) {
			spi_xfer(spi, 1 * 8, NULL, &r1, 0);
			if (r1 != 0xff) /* data token */
				break;
		}
		debug("%s:tok%d %x\n", __func__, i, r1);
		if (r1 == SPI_TOKEN_SINGLE) {
			spi_xfer(spi, bsize * 8, NULL, buf, 0);
			spi_xfer(spi, 2 * 8, NULL, &crc, 0);
#ifdef CONFIG_MMC_SPI_CRC_ON
			if (swab16(cyg_crc16(buf, bsize)) != crc) {
				debug("%s: CRC error\n", mmc->name);
				r1 = R1_SPI_COM_CRC;
				break;
			}
#endif
			r1 = 0;
		} else {
			r1 = R1_SPI_ERROR;
			break;
		}
		buf += bsize;
	}
	return r1;
}

static uint mmc_spi_writedata(struct mmc *mmc, const void *xbuf,
			      u32 bcnt, u32 bsize, int multi)
{
	struct spi_slave *spi = mmc->priv;
	const u8 *buf = xbuf;
	u8 r1;
	u16 crc;
	u8 tok[2];
	int i;
	tok[0] = 0xff;
	tok[1] = multi ? SPI_TOKEN_MULTI_WRITE : SPI_TOKEN_SINGLE;
	while (bcnt--) {
#ifdef CONFIG_MMC_SPI_CRC_ON
		crc = swab16(cyg_crc16((u8 *)buf, bsize));
#endif
		spi_xfer(spi, 2 * 8, tok, NULL, 0);
		spi_xfer(spi, bsize * 8, buf, NULL, 0);
		spi_xfer(spi, 2 * 8, &crc, NULL, 0);
		for (i = 0; i < CTOUT; i++) {
			spi_xfer(spi, 1 * 8, NULL, &r1, 0);
			if ((r1 & 0x10) == 0) /* response token */
				break;
		}
		debug("%s:tok%d %x\n", __func__, i, r1);
		if (SPI_MMC_RESPONSE_CODE(r1) == SPI_RESPONSE_ACCEPTED) {
			for (i = 0; i < WTOUT; i++) { /* wait busy */
				spi_xfer(spi, 1 * 8, NULL, &r1, 0);
				if (i && r1 == 0xff) {
					r1 = 0;
					break;
				}
			}
			if (i == WTOUT) {
				debug("%s:wtout %x\n", __func__, r1);
				r1 = R1_SPI_ERROR;
				break;
			}
		} else {
			debug("%s: err %x\n", __func__, r1);
			r1 = R1_SPI_COM_CRC;
			break;
		}
		buf += bsize;
	}
	if (multi && bcnt == -1) { /* stop multi write */
		tok[1] = SPI_TOKEN_STOP_TRAN;
		spi_xfer(spi, 2 * 8, tok, NULL, 0);
		for (i = 0; i < WTOUT; i++) { /* wait busy */
			spi_xfer(spi, 1 * 8, NULL, &r1, 0);
			if (i && r1 == 0xff) {
				r1 = 0;
				break;
			}
		}
		if (i == WTOUT) {
			debug("%s:wstop %x\n", __func__, r1);
			r1 = R1_SPI_ERROR;
		}
	}
	return r1;
}

static int mmc_spi_request(struct mmc *mmc, struct mmc_cmd *cmd,
		struct mmc_data *data)
{
	struct spi_slave *spi = mmc->priv;
	u8 r1;
	int i;
	int ret = 0;
	debug("%s:cmd%d %x %x %x\n", __func__,
	      cmd->cmdidx, cmd->resp_type, cmd->cmdarg, cmd->flags);
	spi_claim_bus(spi);
	spi_cs_activate(spi);
	r1 = mmc_spi_sendcmd(mmc, cmd->cmdidx, cmd->cmdarg);
	if (r1 == 0xff) { /* no response */
		ret = NO_CARD_ERR;
		goto done;
	} else if (r1 & R1_SPI_COM_CRC) {
		ret = COMM_ERR;
		goto done;
	} else if (r1 & ~R1_SPI_IDLE) { /* other errors */
		ret = TIMEOUT;
		goto done;
	} else if (cmd->resp_type == MMC_RSP_R2) {
		r1 = mmc_spi_readdata(mmc, cmd->response, 1, 16);
		for (i = 0; i < 4; i++)
			cmd->response[i] = swab32(cmd->response[i]);
		debug("r128 %x %x %x %x\n", cmd->response[0], cmd->response[1],
		      cmd->response[2], cmd->response[3]);
	} else if (!data) {
		switch (cmd->cmdidx) {
		case SD_CMD_APP_SEND_OP_COND:
		case MMC_CMD_SEND_OP_COND:
			cmd->response[0] = (r1 & R1_SPI_IDLE) ? 0 : OCR_BUSY;
			break;
		case SD_CMD_SEND_IF_COND:
		case MMC_CMD_SPI_READ_OCR:
			spi_xfer(spi, 4 * 8, NULL, cmd->response, 0);
			cmd->response[0] = swab32(cmd->response[0]);
			debug("r32 %x\n", cmd->response[0]);
			break;
		case MMC_CMD_SEND_STATUS:
			spi_xfer(spi, 1 * 8, NULL, cmd->response, 0);
			cmd->response[0] = (cmd->response[0] & 0xff) ?
				MMC_STATUS_ERROR : MMC_STATUS_RDY_FOR_DATA;
			break;
		}
	} else {
		debug("%s:data %x %x %x\n", __func__,
		      data->flags, data->blocks, data->blocksize);
		if (data->flags == MMC_DATA_READ)
			r1 = mmc_spi_readdata(mmc, data->dest,
				data->blocks, data->blocksize);
		else if  (data->flags == MMC_DATA_WRITE)
			r1 = mmc_spi_writedata(mmc, data->src,
				data->blocks, data->blocksize,
				(cmd->cmdidx == MMC_CMD_WRITE_MULTIPLE_BLOCK));
		if (r1 & R1_SPI_COM_CRC)
			ret = COMM_ERR;
		else if (r1) /* other errors */
			ret = TIMEOUT;
	}
done:
	spi_cs_deactivate(spi);
	spi_release_bus(spi);
	return ret;
}

static void mmc_spi_set_ios(struct mmc *mmc)
{
	struct spi_slave *spi = mmc->priv;
	debug("%s: clock %u\n", __func__, mmc->clock);
	if (mmc->clock)
		spi_set_speed(spi, mmc->clock);
}

static int mmc_spi_init_p(struct mmc *mmc)
{
	struct spi_slave *spi = mmc->priv;
	mmc->clock = 0;
	spi_set_speed(spi, MMC_SPI_MIN_CLOCK);
	spi_claim_bus(spi);
	/* cs deactivated for 100+ clock */
	spi_xfer(spi, 18 * 8, NULL, NULL, 0);
	spi_release_bus(spi);
	return 0;
}

struct mmc *mmc_spi_init(uint bus, uint cs, uint speed, uint mode)
{
	struct mmc *mmc;

	mmc = malloc(sizeof(*mmc));
	if (!mmc)
		return NULL;
	memset(mmc, 0, sizeof(*mmc));
	mmc->priv = spi_setup_slave(bus, cs, speed, mode);
	if (!mmc->priv) {
		free(mmc);
		return NULL;
	}
	sprintf(mmc->name, "MMC_SPI");
	mmc->send_cmd = mmc_spi_request;
	mmc->set_ios = mmc_spi_set_ios;
	mmc->init = mmc_spi_init_p;
	mmc->getcd = NULL;
	mmc->host_caps = MMC_MODE_SPI;

	mmc->voltages = MMC_SPI_VOLTAGE;
	mmc->f_max = speed;
	mmc->f_min = MMC_SPI_MIN_CLOCK;
	mmc->block_dev.part_type = PART_TYPE_DOS;

	mmc_register(mmc);

	return mmc;
}
