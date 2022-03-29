/* Integrated Flash Controller NAND Machine Driver
 *
 * Copyright (c) 2012 Freescale Semiconductor, Inc
 *
 * Authors: Dipen Dudhat <Dipen.Dudhat@freescale.com>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <common.h>
#include <malloc.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>

#include <asm/io.h>
#include <asm/errno.h>
#include <asm/fsl_ifc.h>

#define MAX_BANKS	4
#define ERR_BYTE	0xFF /* Value returned for read bytes
				when read failed */
#define IFC_TIMEOUT_MSECS 10 /* Maximum number of mSecs to wait for IFC
				NAND Machine */

struct fsl_ifc_ctrl;

/* mtd information per set */
struct fsl_ifc_mtd {
	struct mtd_info mtd;
	struct nand_chip chip;
	struct fsl_ifc_ctrl *ctrl;

	struct device *dev;
	int bank;               /* Chip select bank number                */
	unsigned int bufnum_mask; /* bufnum = page & bufnum_mask */
	u8 __iomem *vbase;      /* Chip select base virtual address       */
};

/* overview of the fsl ifc controller */
struct fsl_ifc_ctrl {
	struct nand_hw_control controller;
	struct fsl_ifc_mtd *chips[MAX_BANKS];

	/* device info */
	struct fsl_ifc *regs;
	uint8_t __iomem *addr;   /* Address of assigned IFC buffer        */
	unsigned int cs_nand;    /* On which chipsel NAND is connected	  */
	unsigned int page;       /* Last page written to / read from      */
	unsigned int read_bytes; /* Number of bytes read during command   */
	unsigned int column;     /* Saved column from SEQIN               */
	unsigned int index;      /* Pointer to next byte to 'read'        */
	unsigned int status;     /* status read from NEESR after last op  */
	unsigned int oob;        /* Non zero if operating on OOB data     */
	unsigned int eccread;    /* Non zero for a full-page ECC read     */
};

static struct fsl_ifc_ctrl *ifc_ctrl;

/* 512-byte page with 4-bit ECC, 8-bit */
static struct nand_ecclayout oob_512_8bit_ecc4 = {
	.eccbytes = 8,
	.eccpos = {8, 9, 10, 11, 12, 13, 14, 15},
	.oobfree = { {0, 5}, {6, 2} },
};

/* 512-byte page with 4-bit ECC, 16-bit */
static struct nand_ecclayout oob_512_16bit_ecc4 = {
	.eccbytes = 8,
	.eccpos = {8, 9, 10, 11, 12, 13, 14, 15},
	.oobfree = { {2, 6}, },
};

/* 2048-byte page size with 4-bit ECC */
static struct nand_ecclayout oob_2048_ecc4 = {
	.eccbytes = 32,
	.eccpos = {
		8, 9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23,
		24, 25, 26, 27, 28, 29, 30, 31,
		32, 33, 34, 35, 36, 37, 38, 39,
	},
	.oobfree = { {2, 6}, {40, 24} },
};

/* 4096-byte page size with 4-bit ECC */
static struct nand_ecclayout oob_4096_ecc4 = {
	.eccbytes = 64,
	.eccpos = {
		8, 9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23,
		24, 25, 26, 27, 28, 29, 30, 31,
		32, 33, 34, 35, 36, 37, 38, 39,
		40, 41, 42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 52, 53, 54, 55,
		56, 57, 58, 59, 60, 61, 62, 63,
		64, 65, 66, 67, 68, 69, 70, 71,
	},
	.oobfree = { {2, 6}, {72, 56} },
};

/* 4096-byte page size with 8-bit ECC -- requires 218-byte OOB */
static struct nand_ecclayout oob_4096_ecc8 = {
	.eccbytes = 128,
	.eccpos = {
		8, 9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23,
		24, 25, 26, 27, 28, 29, 30, 31,
		32, 33, 34, 35, 36, 37, 38, 39,
		40, 41, 42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 52, 53, 54, 55,
		56, 57, 58, 59, 60, 61, 62, 63,
		64, 65, 66, 67, 68, 69, 70, 71,
		72, 73, 74, 75, 76, 77, 78, 79,
		80, 81, 82, 83, 84, 85, 86, 87,
		88, 89, 90, 91, 92, 93, 94, 95,
		96, 97, 98, 99, 100, 101, 102, 103,
		104, 105, 106, 107, 108, 109, 110, 111,
		112, 113, 114, 115, 116, 117, 118, 119,
		120, 121, 122, 123, 124, 125, 126, 127,
		128, 129, 130, 131, 132, 133, 134, 135,
	},
	.oobfree = { {2, 6}, {136, 82} },
};


/*
 * Generic flash bbt descriptors
 */
static u8 bbt_pattern[] = {'B', 'b', 't', '0' };
static u8 mirror_pattern[] = {'1', 't', 'b', 'B' };

static struct nand_bbt_descr bbt_main_descr = {
	.options = NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE |
		   NAND_BBT_2BIT | NAND_BBT_VERSION,
	.offs =	2, /* 0 on 8-bit small page */
	.len = 4,
	.veroffs = 6,
	.maxblocks = 4,
	.pattern = bbt_pattern,
};

static struct nand_bbt_descr bbt_mirror_descr = {
	.options = NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_WRITE |
		   NAND_BBT_2BIT | NAND_BBT_VERSION,
	.offs =	2, /* 0 on 8-bit small page */
	.len = 4,
	.veroffs = 6,
	.maxblocks = 4,
	.pattern = mirror_pattern,
};

/*
 * Set up the IFC hardware block and page address fields, and the ifc nand
 * structure addr field to point to the correct IFC buffer in memory
 */
static void set_addr(struct mtd_info *mtd, int column, int page_addr, int oob)
{
	struct nand_chip *chip = mtd->priv;
	struct fsl_ifc_mtd *priv = chip->priv;
	struct fsl_ifc_ctrl *ctrl = priv->ctrl;
	struct fsl_ifc *ifc = ctrl->regs;
	int buf_num;

	ctrl->page = page_addr;

	/* Program ROW0/COL0 */
	out_be32(&ifc->ifc_nand.row0, page_addr);
	out_be32(&ifc->ifc_nand.col0, (oob ? IFC_NAND_COL_MS : 0) | column);

	buf_num = page_addr & priv->bufnum_mask;

	ctrl->addr = priv->vbase + buf_num * (mtd->writesize * 2);
	ctrl->index = column;

	/* for OOB data point to the second half of the buffer */
	if (oob)
		ctrl->index += mtd->writesize;
}

static int is_blank(struct mtd_info *mtd, struct fsl_ifc_ctrl *ctrl,
		    unsigned int bufnum)
{
	struct nand_chip *chip = mtd->priv;
	struct fsl_ifc_mtd *priv = chip->priv;
	u8 __iomem *addr = priv->vbase + bufnum * (mtd->writesize * 2);
	u32 __iomem *main = (u32 *)addr;
	u8 __iomem *oob = addr + mtd->writesize;
	int i;

	for (i = 0; i < mtd->writesize / 4; i++) {
		if (__raw_readl(&main[i]) != 0xffffffff)
			return 0;
	}

	for (i = 0; i < chip->ecc.layout->eccbytes; i++) {
		int pos = chip->ecc.layout->eccpos[i];

		if (__raw_readb(&oob[pos]) != 0xff)
			return 0;
	}

	return 1;
}

/* returns nonzero if entire page is blank */
static int check_read_ecc(struct mtd_info *mtd, struct fsl_ifc_ctrl *ctrl,
			  u32 *eccstat, unsigned int bufnum)
{
	u32 reg = eccstat[bufnum / 4];
	int errors;

	errors = (reg >> ((3 - bufnum % 4) * 8)) & 15;

	return errors;
}

/*
 * execute IFC NAND command and wait for it to complete
 */
static int fsl_ifc_run_command(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct fsl_ifc_mtd *priv = chip->priv;
	struct fsl_ifc_ctrl *ctrl = priv->ctrl;
	struct fsl_ifc *ifc = ctrl->regs;
	long long end_tick;
	u32 eccstat[4];
	int i;

	/* set the chip select for NAND Transaction */
	out_be32(&ifc->ifc_nand.nand_csel, ifc_ctrl->cs_nand);

	/* start read/write seq */
	out_be32(&ifc->ifc_nand.nandseq_strt,
		 IFC_NAND_SEQ_STRT_FIR_STRT);

	/* wait for NAND Machine complete flag or timeout */
	end_tick = usec2ticks(IFC_TIMEOUT_MSECS * 1000) + get_ticks();

	while (end_tick > get_ticks()) {
		ctrl->status = in_be32(&ifc->ifc_nand.nand_evter_stat);

		if (ctrl->status & IFC_NAND_EVTER_STAT_OPC)
			break;
	}

	out_be32(&ifc->ifc_nand.nand_evter_stat, ctrl->status);

	if (ctrl->status & IFC_NAND_EVTER_STAT_FTOER)
		printf("%s: Flash Time Out Error\n", __func__);
	if (ctrl->status & IFC_NAND_EVTER_STAT_WPER)
		printf("%s: Write Protect Error\n", __func__);

	if (ctrl->eccread) {
		int errors;
		int bufnum = ctrl->page & priv->bufnum_mask;
		int sector = bufnum * chip->ecc.steps;
		int sector_end = sector + chip->ecc.steps - 1;

		for (i = sector / 4; i <= sector_end / 4; i++)
			eccstat[i] = in_be32(&ifc->ifc_nand.nand_eccstat[i]);

		for (i = sector; i <= sector_end; i++) {
			errors = check_read_ecc(mtd, ctrl, eccstat, i);

			if (errors == 15) {
				/*
				 * Uncorrectable error.
				 * OK only if the whole page is blank.
				 *
				 * We disable ECCER reporting due to erratum
				 * IFC-A002770 -- so report it now if we
				 * see an uncorrectable error in ECCSTAT.
				 */
				if (!is_blank(mtd, ctrl, bufnum))
					ctrl->status |=
						IFC_NAND_EVTER_STAT_ECCER;
				break;
			}

			mtd->ecc_stats.corrected += errors;
		}

		ctrl->eccread = 0;
	}

	/* returns 0 on success otherwise non-zero) */
	return ctrl->status == IFC_NAND_EVTER_STAT_OPC ? 0 : -EIO;
}

static void fsl_ifc_do_read(struct nand_chip *chip,
			    int oob,
			    struct mtd_info *mtd)
{
	struct fsl_ifc_mtd *priv = chip->priv;
	struct fsl_ifc_ctrl *ctrl = priv->ctrl;
	struct fsl_ifc *ifc = ctrl->regs;

	/* Program FIR/IFC_NAND_FCR0 for Small/Large page */
	if (mtd->writesize > 512) {
		out_be32(&ifc->ifc_nand.nand_fir0,
			 (IFC_FIR_OP_CW0 << IFC_NAND_FIR0_OP0_SHIFT) |
			 (IFC_FIR_OP_CA0 << IFC_NAND_FIR0_OP1_SHIFT) |
			 (IFC_FIR_OP_RA0 << IFC_NAND_FIR0_OP2_SHIFT) |
			 (IFC_FIR_OP_CMD1 << IFC_NAND_FIR0_OP3_SHIFT) |
			 (IFC_FIR_OP_RBCD << IFC_NAND_FIR0_OP4_SHIFT));
		out_be32(&ifc->ifc_nand.nand_fir1, 0x0);

		out_be32(&ifc->ifc_nand.nand_fcr0,
			(NAND_CMD_READ0 << IFC_NAND_FCR0_CMD0_SHIFT) |
			(NAND_CMD_READSTART << IFC_NAND_FCR0_CMD1_SHIFT));
	} else {
		out_be32(&ifc->ifc_nand.nand_fir0,
			 (IFC_FIR_OP_CW0 << IFC_NAND_FIR0_OP0_SHIFT) |
			 (IFC_FIR_OP_CA0 << IFC_NAND_FIR0_OP1_SHIFT) |
			 (IFC_FIR_OP_RA0  << IFC_NAND_FIR0_OP2_SHIFT) |
			 (IFC_FIR_OP_RBCD << IFC_NAND_FIR0_OP3_SHIFT));

		if (oob)
			out_be32(&ifc->ifc_nand.nand_fcr0,
				 NAND_CMD_READOOB << IFC_NAND_FCR0_CMD0_SHIFT);
		else
			out_be32(&ifc->ifc_nand.nand_fcr0,
				NAND_CMD_READ0 << IFC_NAND_FCR0_CMD0_SHIFT);
	}
}

/* cmdfunc send commands to the IFC NAND Machine */
static void fsl_ifc_cmdfunc(struct mtd_info *mtd, unsigned int command,
			     int column, int page_addr)
{
	struct nand_chip *chip = mtd->priv;
	struct fsl_ifc_mtd *priv = chip->priv;
	struct fsl_ifc_ctrl *ctrl = priv->ctrl;
	struct fsl_ifc *ifc = ctrl->regs;

	/* clear the read buffer */
	ctrl->read_bytes = 0;
	if (command != NAND_CMD_PAGEPROG)
		ctrl->index = 0;

	switch (command) {
	/* READ0 read the entire buffer to use hardware ECC. */
	case NAND_CMD_READ0: {
		out_be32(&ifc->ifc_nand.nand_fbcr, 0);
		set_addr(mtd, 0, page_addr, 0);

		ctrl->read_bytes = mtd->writesize + mtd->oobsize;
		ctrl->index += column;

		if (chip->ecc.mode == NAND_ECC_HW)
			ctrl->eccread = 1;

		fsl_ifc_do_read(chip, 0, mtd);
		fsl_ifc_run_command(mtd);
		return;
	}

	/* READOOB reads only the OOB because no ECC is performed. */
	case NAND_CMD_READOOB:
		out_be32(&ifc->ifc_nand.nand_fbcr, mtd->oobsize - column);
		set_addr(mtd, column, page_addr, 1);

		ctrl->read_bytes = mtd->writesize + mtd->oobsize;

		fsl_ifc_do_read(chip, 1, mtd);
		fsl_ifc_run_command(mtd);

		return;

	/* READID must read all possible bytes while CEB is active */
	case NAND_CMD_READID:
	case NAND_CMD_PARAM: {
		int timing = IFC_FIR_OP_RB;
		if (command == NAND_CMD_PARAM)
			timing = IFC_FIR_OP_RBCD;

		out_be32(&ifc->ifc_nand.nand_fir0,
				(IFC_FIR_OP_CMD0 << IFC_NAND_FIR0_OP0_SHIFT) |
				(IFC_FIR_OP_UA  << IFC_NAND_FIR0_OP1_SHIFT) |
				(timing << IFC_NAND_FIR0_OP2_SHIFT));
		out_be32(&ifc->ifc_nand.nand_fcr0,
				command << IFC_NAND_FCR0_CMD0_SHIFT);
		out_be32(&ifc->ifc_nand.row3, column);

		/*
		 * although currently it's 8 bytes for READID, we always read
		 * the maximum 256 bytes(for PARAM)
		 */
		out_be32(&ifc->ifc_nand.nand_fbcr, 256);
		ctrl->read_bytes = 256;

		set_addr(mtd, 0, 0, 0);
		fsl_ifc_run_command(mtd);
		return;
	}

	/* ERASE1 stores the block and page address */
	case NAND_CMD_ERASE1:
		set_addr(mtd, 0, page_addr, 0);
		return;

	/* ERASE2 uses the block and page address from ERASE1 */
	case NAND_CMD_ERASE2:
		out_be32(&ifc->ifc_nand.nand_fir0,
			 (IFC_FIR_OP_CW0 << IFC_NAND_FIR0_OP0_SHIFT) |
			 (IFC_FIR_OP_RA0 << IFC_NAND_FIR0_OP1_SHIFT) |
			 (IFC_FIR_OP_CMD1 << IFC_NAND_FIR0_OP2_SHIFT));

		out_be32(&ifc->ifc_nand.nand_fcr0,
			 (NAND_CMD_ERASE1 << IFC_NAND_FCR0_CMD0_SHIFT) |
			 (NAND_CMD_ERASE2 << IFC_NAND_FCR0_CMD1_SHIFT));

		out_be32(&ifc->ifc_nand.nand_fbcr, 0);
		ctrl->read_bytes = 0;
		fsl_ifc_run_command(mtd);
		return;

	/* SEQIN sets up the addr buffer and all registers except the length */
	case NAND_CMD_SEQIN: {
		u32 nand_fcr0;
		ctrl->column = column;
		ctrl->oob = 0;

		if (mtd->writesize > 512) {
			nand_fcr0 =
				(NAND_CMD_SEQIN << IFC_NAND_FCR0_CMD0_SHIFT) |
				(NAND_CMD_PAGEPROG << IFC_NAND_FCR0_CMD1_SHIFT);

			out_be32(&ifc->ifc_nand.nand_fir0,
				 (IFC_FIR_OP_CW0 << IFC_NAND_FIR0_OP0_SHIFT) |
				 (IFC_FIR_OP_CA0 << IFC_NAND_FIR0_OP1_SHIFT) |
				 (IFC_FIR_OP_RA0 << IFC_NAND_FIR0_OP2_SHIFT) |
				 (IFC_FIR_OP_WBCD  << IFC_NAND_FIR0_OP3_SHIFT) |
				 (IFC_FIR_OP_CW1 << IFC_NAND_FIR0_OP4_SHIFT));
			out_be32(&ifc->ifc_nand.nand_fir1, 0);
		} else {
			nand_fcr0 = ((NAND_CMD_PAGEPROG <<
					IFC_NAND_FCR0_CMD1_SHIFT) |
				    (NAND_CMD_SEQIN <<
					IFC_NAND_FCR0_CMD2_SHIFT));

			out_be32(&ifc->ifc_nand.nand_fir0,
				 (IFC_FIR_OP_CW0 << IFC_NAND_FIR0_OP0_SHIFT) |
				 (IFC_FIR_OP_CMD2 << IFC_NAND_FIR0_OP1_SHIFT) |
				 (IFC_FIR_OP_CA0 << IFC_NAND_FIR0_OP2_SHIFT) |
				 (IFC_FIR_OP_RA0 << IFC_NAND_FIR0_OP3_SHIFT) |
				 (IFC_FIR_OP_WBCD << IFC_NAND_FIR0_OP4_SHIFT));
			out_be32(&ifc->ifc_nand.nand_fir1,
				 (IFC_FIR_OP_CW1 << IFC_NAND_FIR1_OP5_SHIFT));

			if (column >= mtd->writesize)
				nand_fcr0 |=
				NAND_CMD_READOOB << IFC_NAND_FCR0_CMD0_SHIFT;
			else
				nand_fcr0 |=
				NAND_CMD_READ0 << IFC_NAND_FCR0_CMD0_SHIFT;
		}

		if (column >= mtd->writesize) {
			/* OOB area --> READOOB */
			column -= mtd->writesize;
			ctrl->oob = 1;
		}
		out_be32(&ifc->ifc_nand.nand_fcr0, nand_fcr0);
		set_addr(mtd, column, page_addr, ctrl->oob);
		return;
	}

	/* PAGEPROG reuses all of the setup from SEQIN and adds the length */
	case NAND_CMD_PAGEPROG:
		if (ctrl->oob)
			out_be32(&ifc->ifc_nand.nand_fbcr,
					ctrl->index - ctrl->column);
		else
			out_be32(&ifc->ifc_nand.nand_fbcr, 0);

		fsl_ifc_run_command(mtd);
		return;

	case NAND_CMD_STATUS:
		out_be32(&ifc->ifc_nand.nand_fir0,
				(IFC_FIR_OP_CW0 << IFC_NAND_FIR0_OP0_SHIFT) |
				(IFC_FIR_OP_RB << IFC_NAND_FIR0_OP1_SHIFT));
		out_be32(&ifc->ifc_nand.nand_fcr0,
				NAND_CMD_STATUS << IFC_NAND_FCR0_CMD0_SHIFT);
		out_be32(&ifc->ifc_nand.nand_fbcr, 1);
		set_addr(mtd, 0, 0, 0);
		ctrl->read_bytes = 1;

		fsl_ifc_run_command(mtd);

		/* Chip sometimes reporting write protect even when it's not */
		out_8(ctrl->addr, in_8(ctrl->addr) | NAND_STATUS_WP);
		return;

	case NAND_CMD_RESET:
		out_be32(&ifc->ifc_nand.nand_fir0,
				IFC_FIR_OP_CW0 << IFC_NAND_FIR0_OP0_SHIFT);
		out_be32(&ifc->ifc_nand.nand_fcr0,
				NAND_CMD_RESET << IFC_NAND_FCR0_CMD0_SHIFT);
		fsl_ifc_run_command(mtd);
		return;

	default:
		printf("%s: error, unsupported command 0x%x.\n",
			__func__, command);
	}
}

/*
 * Write buf to the IFC NAND Controller Data Buffer
 */
static void fsl_ifc_write_buf(struct mtd_info *mtd, const u8 *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct fsl_ifc_mtd *priv = chip->priv;
	struct fsl_ifc_ctrl *ctrl = priv->ctrl;
	unsigned int bufsize = mtd->writesize + mtd->oobsize;

	if (len <= 0) {
		printf("%s of %d bytes", __func__, len);
		ctrl->status = 0;
		return;
	}

	if ((unsigned int)len > bufsize - ctrl->index) {
		printf("%s beyond end of buffer "
		       "(%d requested, %u available)\n",
			__func__, len, bufsize - ctrl->index);
		len = bufsize - ctrl->index;
	}

	memcpy_toio(&ctrl->addr[ctrl->index], buf, len);
	ctrl->index += len;
}

/*
 * read a byte from either the IFC hardware buffer if it has any data left
 * otherwise issue a command to read a single byte.
 */
static u8 fsl_ifc_read_byte(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct fsl_ifc_mtd *priv = chip->priv;
	struct fsl_ifc_ctrl *ctrl = priv->ctrl;

	/* If there are still bytes in the IFC buffer, then use the
	 * next byte. */
	if (ctrl->index < ctrl->read_bytes)
		return in_8(&ctrl->addr[ctrl->index++]);

	printf("%s beyond end of buffer\n", __func__);
	return ERR_BYTE;
}

/*
 * Read two bytes from the IFC hardware buffer
 * read function for 16-bit buswith
 */
static uint8_t fsl_ifc_read_byte16(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct fsl_ifc_mtd *priv = chip->priv;
	struct fsl_ifc_ctrl *ctrl = priv->ctrl;
	uint16_t data;

	/*
	 * If there are still bytes in the IFC buffer, then use the
	 * next byte.
	 */
	if (ctrl->index < ctrl->read_bytes) {
		data = in_be16((uint16_t *)&ctrl->
					addr[ctrl->index]);
		ctrl->index += 2;
		return (uint8_t)data;
	}

	printf("%s beyond end of buffer\n", __func__);
	return ERR_BYTE;
}

/*
 * Read from the IFC Controller Data Buffer
 */
static void fsl_ifc_read_buf(struct mtd_info *mtd, u8 *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct fsl_ifc_mtd *priv = chip->priv;
	struct fsl_ifc_ctrl *ctrl = priv->ctrl;
	int avail;

	if (len < 0)
		return;

	avail = min((unsigned int)len, ctrl->read_bytes - ctrl->index);
	memcpy_fromio(buf, &ctrl->addr[ctrl->index], avail);
	ctrl->index += avail;

	if (len > avail)
		printf("%s beyond end of buffer "
		       "(%d requested, %d available)\n",
		       __func__, len, avail);
}

/*
 * Verify buffer against the IFC Controller Data Buffer
 */
static int fsl_ifc_verify_buf(struct mtd_info *mtd,
			       const u_char *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct fsl_ifc_mtd *priv = chip->priv;
	struct fsl_ifc_ctrl *ctrl = priv->ctrl;
	int i;

	if (len < 0) {
		printf("%s of %d bytes", __func__, len);
		return -EINVAL;
	}

	if ((unsigned int)len > ctrl->read_bytes - ctrl->index) {
		printf("%s beyond end of buffer "
		       "(%d requested, %u available)\n",
		       __func__, len, ctrl->read_bytes - ctrl->index);

		ctrl->index = ctrl->read_bytes;
		return -EINVAL;
	}

	for (i = 0; i < len; i++)
		if (in_8(&ctrl->addr[ctrl->index + i]) != buf[i])
			break;

	ctrl->index += len;
	return i == len && ctrl->status == IFC_NAND_EVTER_STAT_OPC ? 0 : -EIO;
}

/* This function is called after Program and Erase Operations to
 * check for success or failure.
 */
static int fsl_ifc_wait(struct mtd_info *mtd, struct nand_chip *chip)
{
	struct fsl_ifc_mtd *priv = chip->priv;
	struct fsl_ifc_ctrl *ctrl = priv->ctrl;
	struct fsl_ifc *ifc = ctrl->regs;
	u32 nand_fsr;

	if (ctrl->status != IFC_NAND_EVTER_STAT_OPC)
		return NAND_STATUS_FAIL;

	/* Use READ_STATUS command, but wait for the device to be ready */
	out_be32(&ifc->ifc_nand.nand_fir0,
		 (IFC_FIR_OP_CW0 << IFC_NAND_FIR0_OP0_SHIFT) |
		 (IFC_FIR_OP_RDSTAT << IFC_NAND_FIR0_OP1_SHIFT));
	out_be32(&ifc->ifc_nand.nand_fcr0, NAND_CMD_STATUS <<
			IFC_NAND_FCR0_CMD0_SHIFT);
	out_be32(&ifc->ifc_nand.nand_fbcr, 1);
	set_addr(mtd, 0, 0, 0);
	ctrl->read_bytes = 1;

	fsl_ifc_run_command(mtd);

	if (ctrl->status != IFC_NAND_EVTER_STAT_OPC)
		return NAND_STATUS_FAIL;

	nand_fsr = in_be32(&ifc->ifc_nand.nand_fsr);

	/* Chip sometimes reporting write protect even when it's not */
	nand_fsr = nand_fsr | NAND_STATUS_WP;
	return nand_fsr;
}

static int fsl_ifc_read_page(struct mtd_info *mtd,
			      struct nand_chip *chip,
			      uint8_t *buf, int page)
{
	struct fsl_ifc_mtd *priv = chip->priv;
	struct fsl_ifc_ctrl *ctrl = priv->ctrl;

	fsl_ifc_read_buf(mtd, buf, mtd->writesize);
	fsl_ifc_read_buf(mtd, chip->oob_poi, mtd->oobsize);

	if (ctrl->status != IFC_NAND_EVTER_STAT_OPC)
		mtd->ecc_stats.failed++;

	return 0;
}

/* ECC will be calculated automatically, and errors will be detected in
 * waitfunc.
 */
static void fsl_ifc_write_page(struct mtd_info *mtd,
				struct nand_chip *chip,
				const uint8_t *buf)
{
	fsl_ifc_write_buf(mtd, buf, mtd->writesize);
	fsl_ifc_write_buf(mtd, chip->oob_poi, mtd->oobsize);
}

static void fsl_ifc_ctrl_init(void)
{
	ifc_ctrl = kzalloc(sizeof(*ifc_ctrl), GFP_KERNEL);
	if (!ifc_ctrl)
		return;

	ifc_ctrl->regs = IFC_BASE_ADDR;

	/* clear event registers */
	out_be32(&ifc_ctrl->regs->ifc_nand.nand_evter_stat, ~0U);
	out_be32(&ifc_ctrl->regs->ifc_nand.pgrdcmpl_evt_stat, ~0U);

	/* Enable error and event for any detected errors */
	out_be32(&ifc_ctrl->regs->ifc_nand.nand_evter_en,
			IFC_NAND_EVTER_EN_OPC_EN |
			IFC_NAND_EVTER_EN_PGRDCMPL_EN |
			IFC_NAND_EVTER_EN_FTOER_EN |
			IFC_NAND_EVTER_EN_WPER_EN);

	out_be32(&ifc_ctrl->regs->ifc_nand.ncfgr, 0x0);
}

static void fsl_ifc_select_chip(struct mtd_info *mtd, int chip)
{
}

int board_nand_init(struct nand_chip *nand)
{
	struct fsl_ifc_mtd *priv;
	struct nand_ecclayout *layout;
	uint32_t cspr = 0, csor = 0;

	if (!ifc_ctrl) {
		fsl_ifc_ctrl_init();
		if (!ifc_ctrl)
			return -1;
	}

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->ctrl = ifc_ctrl;
	priv->vbase = nand->IO_ADDR_R;

	/* Find which chip select it is connected to.
	 */
	for (priv->bank = 0; priv->bank < MAX_BANKS; priv->bank++) {
		phys_addr_t base_addr = virt_to_phys(nand->IO_ADDR_R);

		cspr = in_be32(&ifc_ctrl->regs->cspr_cs[priv->bank].cspr);
		csor = in_be32(&ifc_ctrl->regs->csor_cs[priv->bank].csor);

		if ((cspr & CSPR_V) && (cspr & CSPR_MSEL) == CSPR_MSEL_NAND &&
		    (cspr & CSPR_BA) == CSPR_PHYS_ADDR(base_addr)) {
			ifc_ctrl->cs_nand = priv->bank << IFC_NAND_CSEL_SHIFT;
			break;
		}
	}

	if (priv->bank >= MAX_BANKS) {
		printf("%s: address did not match any "
		       "chip selects\n", __func__);
		kfree(priv);
		return -ENODEV;
	}

	ifc_ctrl->chips[priv->bank] = priv;

	/* fill in nand_chip structure */
	/* set up function call table */

	nand->write_buf = fsl_ifc_write_buf;
	nand->read_buf = fsl_ifc_read_buf;
	nand->verify_buf = fsl_ifc_verify_buf;
	nand->select_chip = fsl_ifc_select_chip;
	nand->cmdfunc = fsl_ifc_cmdfunc;
	nand->waitfunc = fsl_ifc_wait;

	/* set up nand options */
	nand->bbt_td = &bbt_main_descr;
	nand->bbt_md = &bbt_mirror_descr;

	/* set up nand options */
	nand->options = NAND_NO_READRDY | NAND_NO_AUTOINCR |
			NAND_USE_FLASH_BBT;

	if (cspr & CSPR_PORT_SIZE_16) {
		nand->read_byte = fsl_ifc_read_byte16;
		nand->options |= NAND_BUSWIDTH_16;
	} else {
		nand->read_byte = fsl_ifc_read_byte;
	}

	nand->controller = &ifc_ctrl->controller;
	nand->priv = priv;

	nand->ecc.read_page = fsl_ifc_read_page;
	nand->ecc.write_page = fsl_ifc_write_page;

	/* Hardware generates ECC per 512 Bytes */
	nand->ecc.size = 512;
	nand->ecc.bytes = 8;

	switch (csor & CSOR_NAND_PGS_MASK) {
	case CSOR_NAND_PGS_512:
		if (nand->options & NAND_BUSWIDTH_16) {
			layout = &oob_512_16bit_ecc4;
		} else {
			layout = &oob_512_8bit_ecc4;

			/* Avoid conflict with bad block marker */
			bbt_main_descr.offs = 0;
			bbt_mirror_descr.offs = 0;
		}

		priv->bufnum_mask = 15;
		break;

	case CSOR_NAND_PGS_2K:
		layout = &oob_2048_ecc4;
		priv->bufnum_mask = 3;
		break;

	case CSOR_NAND_PGS_4K:
		if ((csor & CSOR_NAND_ECC_MODE_MASK) ==
		    CSOR_NAND_ECC_MODE_4) {
			layout = &oob_4096_ecc4;
		} else {
			layout = &oob_4096_ecc8;
			nand->ecc.bytes = 16;
		}

		priv->bufnum_mask = 1;
		break;

	default:
		printf("ifc nand: bad csor %#x: bad page size\n", csor);
		return -ENODEV;
	}

	/* Must also set CSOR_NAND_ECC_ENC_EN if DEC_EN set */
	if (csor & CSOR_NAND_ECC_DEC_EN) {
		nand->ecc.mode = NAND_ECC_HW;
		nand->ecc.layout = layout;
	} else {
		nand->ecc.mode = NAND_ECC_SOFT;
	}

	return 0;
}
