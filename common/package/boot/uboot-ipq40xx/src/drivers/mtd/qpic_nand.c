/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 * Copyright (c) 2009-2015, The Linux Foundation. All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <common.h>
#include <malloc.h>

#include <asm/io.h>
#include <asm/errno.h>
#include <nand.h>
#include <linux/mtd/nand.h>
#include <asm/arch-qcom-common/bam.h>
#include <asm/arch-qcom-common/qpic_nand.h>

typedef unsigned long addr_t;

struct cmd_element ce_array[100] __attribute__ ((aligned(16)));
struct cmd_element ce_read_array[20] __attribute__ ((aligned(16)));
static struct qpic_nand_dev qpic_nand_dev;
struct bam_desc qpic_cmd_desc_fifo[QPIC_BAM_CMD_FIFO_SIZE] __attribute__ ((aligned(BAM_DESC_SIZE)));
struct bam_desc qpic_data_desc_fifo[QPIC_BAM_DATA_FIFO_SIZE] __attribute__ ((aligned(BAM_DESC_SIZE)));
static struct bam_instance bam;
struct nand_ecclayout fake_ecc_layout;

static void
qpic_nand_wait_for_cmd_exec(uint32_t num_desc)
{
	/* Create a read/write event to notify the periperal of the added desc. */
	bam_sys_gen_event(&bam, CMD_PIPE_INDEX, num_desc);

	/* Wait for the descriptors to be processed */
	bam_wait_for_interrupt(&bam, CMD_PIPE_INDEX, P_PRCSD_DESC_EN_MASK);

	/* Read offset update for the circular FIFO */
	bam_read_offset_update(&bam, CMD_PIPE_INDEX);
}

static void
qpic_nand_wait_for_data(uint32_t pipe_num)
{
	/* Wait for the descriptors to be processed */
	bam_wait_for_interrupt(&bam, pipe_num, P_PRCSD_DESC_EN_MASK);

	/* Read offset update for the circular FIFO */
	bam_read_offset_update(&bam, pipe_num);
}

static uint32_t
qpic_nand_read_reg(uint32_t reg_addr,
		   uint8_t flags)
{
	uint32_t val;
	struct cmd_element *cmd_list_read_ptr = ce_read_array;

	bam_add_cmd_element(cmd_list_read_ptr, reg_addr,
			   (uint32_t)((addr_t)&val), CE_READ_TYPE);

	/* Enqueue the desc for the above command */
	bam_add_one_desc(&bam,
			 CMD_PIPE_INDEX,
			 (unsigned char*)((addr_t)cmd_list_read_ptr),
			 BAM_CE_SIZE,
			 BAM_DESC_CMD_FLAG| BAM_DESC_INT_FLAG | flags);

	qpic_nand_wait_for_cmd_exec(1);

	return val;
}

/* Assume the BAM is in a locked state. */
void
qpic_nand_erased_status_reset(struct cmd_element *cmd_list_ptr, uint8_t flags)
{
	uint32_t val = 0;

	/* Reset the Erased Codeword/Page detection controller. */
	val = NAND_ERASED_CW_DETECT_CFG_RESET_CTRL;

	bam_add_cmd_element(cmd_list_ptr, NAND_ERASED_CW_DETECT_CFG, val,
			    CE_WRITE_TYPE);

	/* Enqueue the desc for the above command */
	bam_add_one_desc(&bam,
			 CMD_PIPE_INDEX,
			 (unsigned char*)cmd_list_ptr,
			 BAM_CE_SIZE,
			 BAM_DESC_CMD_FLAG | BAM_DESC_INT_FLAG | flags);

	qpic_nand_wait_for_cmd_exec(1);

	/* Enable the Erased Codeword/Page detection
	 * controller to check the data as it arrives.
	 * Also disable ECC reporting for an erased CW.
	 */
	val = NAND_ERASED_CW_DETECT_CFG_ACTIVATE_CTRL |
		NAND_ERASED_CW_DETECT_ERASED_CW_ECC_MASK;

	bam_add_cmd_element(cmd_list_ptr, NAND_ERASED_CW_DETECT_CFG, val,
			    CE_WRITE_TYPE);

	/* Enqueue the desc for the above command */
	bam_add_one_desc(&bam,
			 CMD_PIPE_INDEX,
			 (unsigned char*)cmd_list_ptr,
			 BAM_CE_SIZE,
			 BAM_DESC_CMD_FLAG | BAM_DESC_INT_FLAG |
			 BAM_DESC_UNLOCK_FLAG);

	qpic_nand_wait_for_cmd_exec(1);
}

static nand_result_t
qpic_nand_check_status(struct mtd_info *mtd, uint32_t status)
{
	uint32_t erase_sts;

	/* Check for errors */
	if (status & NAND_FLASH_ERR) {
		/* Check if this is an ECC error on an erased page. */
		if (status & NAND_FLASH_OP_ERR) {
			erase_sts = qpic_nand_read_reg(
					NAND_ERASED_CW_DETECT_STATUS, 0);
			if ((erase_sts & 
			    (1 << NAND_ERASED_CW_DETECT_STATUS_PAGE_ALL_ERASED))) {
				/* Mask the OP ERROR. */
				status &= ~NAND_FLASH_OP_ERR;
				qpic_nand_erased_status_reset(ce_array, 0);
			}
		}

		/* ECC error flagged on an erased page read.
		 * Ignore and return success.
		 */
		if (!(status & NAND_FLASH_ERR))
			return NANDC_RESULT_SUCCESS;

		printf("Nand Flash error. Status = %d\n", status);

		if (status & NAND_FLASH_OP_ERR) {
			mtd->ecc_stats.failed++;
			return -EBADMSG;
		}

		if (status & NAND_FLASH_MPU_ERR)
			return -EPERM;

		if (status & NAND_FLASH_TIMEOUT_ERR)
			return -ETIMEDOUT;
		else
			return NANDC_RESULT_FAILURE;
	}

	return NANDC_RESULT_SUCCESS;
}

static uint32_t
qpic_nand_fetch_id(struct mtd_info *mtd)
{
	struct cmd_element *cmd_list_ptr = ce_array;
	struct cmd_element *cmd_list_ptr_start = ce_array;
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);
	int num_desc = 0;
	uint32_t status;
	uint32_t id;
	uint32_t flash_cmd = NAND_CMD_FETCH_ID;
	uint32_t exec_cmd = 1;
	int nand_ret = NANDC_RESULT_SUCCESS;
	uint32_t vld;

	/* Issue the Fetch id command to the NANDc */
	bam_add_cmd_element(cmd_list_ptr, NAND_FLASH_CMD, (uint32_t)flash_cmd,
			    CE_WRITE_TYPE);
	cmd_list_ptr++;

	vld = NAND_CMD_VALID_BASE;

	bam_add_cmd_element(cmd_list_ptr, NAND_DEV_CMD_VLD, (uint32_t)vld,
			    CE_WRITE_TYPE);
	cmd_list_ptr++;

	/* Execute the cmd */
	bam_add_cmd_element(cmd_list_ptr, NAND_EXEC_CMD, (uint32_t)exec_cmd,
			    CE_WRITE_TYPE);
	cmd_list_ptr++;

	/* Prepare the cmd desc for the above commands */
	bam_add_one_desc(&bam,
			 CMD_PIPE_INDEX,
			 (unsigned char*)cmd_list_ptr_start,
			 ((uint32_t)cmd_list_ptr - (uint32_t)cmd_list_ptr_start),
			 BAM_DESC_LOCK_FLAG | BAM_DESC_INT_FLAG |
			 BAM_DESC_NWD_FLAG | BAM_DESC_CMD_FLAG);

	/* Keep track of the number of desc added. */
	num_desc++;
	qpic_nand_wait_for_cmd_exec(num_desc);

	cmd_list_ptr_start = ce_array;
	cmd_list_ptr = ce_array;

	/* Read the status register */
	status = qpic_nand_read_reg(NAND_FLASH_STATUS, 0);

	/* Check for errors */
	nand_ret = qpic_nand_check_status(mtd, status);
	if (nand_ret) {
		printf("Read ID cmd status failed\n");
		goto qpic_nand_fetch_id_err;
	}

	/* Read the id */
	id = qpic_nand_read_reg(NAND_READ_ID, BAM_DESC_UNLOCK_FLAG);

	dev->id = id;
	dev->vendor = id & 0xff;
	dev->device = (id >> 8) & 0xff;
	dev->dev_cfg = (id >> 24) & 0xFF;
	dev->widebus = 0;
	dev->widebus &= (id >> 24) & 0xFF;
	dev->widebus = dev->widebus? 1: 0;

	printf("ID = %x\n", dev->id);
	printf("Vendor = %x\n", dev->vendor);
	printf("Device = %x\n", dev->device);

qpic_nand_fetch_id_err:
	return nand_ret;
}

static int
qpic_bam_init(struct qpic_nand_init_config *config)
{
	uint32_t bam_ret = NANDC_RESULT_SUCCESS;

	bam.base = config->bam_base;
	/* Set Read pipe params. */
	bam.pipe[DATA_PRODUCER_PIPE_INDEX].pipe_num = config->pipes.read_pipe;
	/* System consumer */
	bam.pipe[DATA_PRODUCER_PIPE_INDEX].trans_type = BAM2SYS;
	bam.pipe[DATA_PRODUCER_PIPE_INDEX].fifo.size = QPIC_BAM_DATA_FIFO_SIZE;
	bam.pipe[DATA_PRODUCER_PIPE_INDEX].fifo.head = qpic_data_desc_fifo;
	bam.pipe[DATA_PRODUCER_PIPE_INDEX].lock_grp = config->pipes.read_pipe_grp;

	/* Set Write pipe params. */
	bam.pipe[DATA_CONSUMER_PIPE_INDEX].pipe_num = config->pipes.write_pipe;
	/* System producer */
	bam.pipe[DATA_CONSUMER_PIPE_INDEX].trans_type = SYS2BAM;
	bam.pipe[DATA_CONSUMER_PIPE_INDEX].fifo.size = QPIC_BAM_DATA_FIFO_SIZE;
	bam.pipe[DATA_CONSUMER_PIPE_INDEX].fifo.head = qpic_data_desc_fifo;
	bam.pipe[DATA_CONSUMER_PIPE_INDEX].lock_grp = config->pipes.write_pipe_grp;

	/* Set Cmd pipe params. */
	bam.pipe[CMD_PIPE_INDEX].pipe_num = config->pipes.cmd_pipe;
	/* System consumer */
	bam.pipe[CMD_PIPE_INDEX].trans_type = SYS2BAM;
	bam.pipe[CMD_PIPE_INDEX].fifo.size = QPIC_BAM_CMD_FIFO_SIZE;
	bam.pipe[CMD_PIPE_INDEX].fifo.head = qpic_cmd_desc_fifo;
	bam.pipe[CMD_PIPE_INDEX].lock_grp = config->pipes.cmd_pipe_grp;

	/* Programs the threshold for BAM transfer
	 * When this threshold is reached, BAM signals the peripheral via the
	 * pipe_bytes_available interface.
	 * The peripheral is signalled with this notification in the following cases:
	 * a.  It has accumulated all the descriptors.
	 * b.  It has accumulated more than threshold bytes.
	 * c.  It has reached EOT (End Of Transfer).
	 * Note: this value needs to be set by the h/w folks and is specific
	 * for each peripheral.
	*/
	bam.threshold = 32;

	/* Set the EE.  */
	bam.ee = config->ee;

	/* Set the max desc length for this BAM. */
	bam.max_desc_len = config->max_desc_len;

	/* BAM Init. */
	bam_init(&bam);

	/* Initialize BAM QPIC read pipe */
	bam_sys_pipe_init(&bam, DATA_PRODUCER_PIPE_INDEX);

	/* Init read fifo */
	bam_ret = bam_pipe_fifo_init(&bam,
				 bam.pipe[DATA_PRODUCER_PIPE_INDEX].pipe_num);

	if (bam_ret) {
		printf("QPIC:NANDc BAM Read FIFO init error\n");
		bam_ret = NANDC_RESULT_FAILURE;
		goto qpic_nand_bam_init_error;
	}

	/* Initialize BAM QPIC write pipe */
	bam_sys_pipe_init(&bam, DATA_CONSUMER_PIPE_INDEX);

	/* Init write fifo. Use the same fifo as read fifo. */
	bam_ret = bam_pipe_fifo_init(&bam,
				bam.pipe[DATA_CONSUMER_PIPE_INDEX].pipe_num);

	if (bam_ret) {
		printf("QPIC: NANDc: BAM Write FIFO init error\n");
		bam_ret = NANDC_RESULT_FAILURE;
		goto qpic_nand_bam_init_error;
	}

	/* Initialize BAM QPIC cmd pipe */
	bam_sys_pipe_init(&bam, CMD_PIPE_INDEX);

	/* Init cmd fifo */
	bam_ret = bam_pipe_fifo_init(&bam, bam.pipe[CMD_PIPE_INDEX].pipe_num);

	if (bam_ret) {
		printf("QPIC:NANDc BAM CMD FIFO init error\n");
		bam_ret = NANDC_RESULT_FAILURE;
		goto qpic_nand_bam_init_error;
	}
	/* Enable BAM Mode in QPIC */
	writel(BAM_MODE_EN, QPIC_NAND_CTRL);
qpic_nand_bam_init_error:
return bam_ret;
}

/* Adds command elements for addr and cfg register writes.
 * cfg: Defines the configuration for the flash cmd.
 * start: Address where the command elements are added.
 *
 * Returns the address where the next cmd element can be added.
 */
struct cmd_element*
qpic_nand_add_addr_n_cfg_ce(struct cfg_params *cfg,
			    struct cmd_element *start)
{
	struct cmd_element *cmd_list_ptr = start;

	bam_add_cmd_element(cmd_list_ptr, NAND_ADDR0, (uint32_t)cfg->addr0,
			    CE_WRITE_TYPE);
	cmd_list_ptr++;
	bam_add_cmd_element(cmd_list_ptr, NAND_ADDR1, (uint32_t)cfg->addr1,
			    CE_WRITE_TYPE);
	cmd_list_ptr++;
	bam_add_cmd_element(cmd_list_ptr, NAND_DEV0_CFG0, (uint32_t)cfg->cfg0,
			    CE_WRITE_TYPE);
	cmd_list_ptr++;
	bam_add_cmd_element(cmd_list_ptr, NAND_DEV0_CFG1, (uint32_t)cfg->cfg1,
			    CE_WRITE_TYPE);
	cmd_list_ptr++;

	return cmd_list_ptr;
}


static struct cmd_element*
qpic_nand_add_onfi_probe_ce(struct onfi_probe_params *params,
			    struct cmd_element *start)
{
	struct cmd_element *cmd_list_ptr = start;

	cmd_list_ptr = qpic_nand_add_addr_n_cfg_ce(&params->cfg, cmd_list_ptr);

	bam_add_cmd_element(cmd_list_ptr, NAND_DEV_CMD1,
			   (uint32_t)params->dev_cmd1, CE_WRITE_TYPE);
	cmd_list_ptr++;
	bam_add_cmd_element(cmd_list_ptr, NAND_DEV_CMD_VLD,
			   (uint32_t)params->vld, CE_WRITE_TYPE);
	cmd_list_ptr++;
	bam_add_cmd_element(cmd_list_ptr, NAND_READ_LOCATION_n(0),
			   (uint32_t)params->cfg.addr_loc_0, CE_WRITE_TYPE);
	cmd_list_ptr++;
	bam_add_cmd_element(cmd_list_ptr, NAND_FLASH_CMD,
			   (uint32_t)params->cfg.cmd, CE_WRITE_TYPE);
	cmd_list_ptr++;
	bam_add_cmd_element(cmd_list_ptr, NAND_EXEC_CMD,
			   (uint32_t)params->cfg.exec, CE_WRITE_TYPE);
	cmd_list_ptr++;

	return cmd_list_ptr;
}

static int
onfi_probe_cmd_exec(struct mtd_info *mtd,
				    struct onfi_probe_params *params,
					unsigned char* data_ptr,
					int data_len)
{
	struct cmd_element *cmd_list_ptr = ce_array;
	struct cmd_element *cmd_list_ptr_start = ce_array;
	int num_desc = 0;
	uint32_t status = 0;
	int nand_ret = NANDC_RESULT_SUCCESS;
	uint8_t desc_flags = BAM_DESC_NWD_FLAG | BAM_DESC_CMD_FLAG
				| BAM_DESC_LOCK_FLAG | BAM_DESC_INT_FLAG;

	params->cfg.addr_loc_0 = 0;
	params->cfg.addr_loc_0 |= NAND_RD_LOC_LAST_BIT(1);
	params->cfg.addr_loc_0 |= NAND_RD_LOC_OFFSET(0);
	params->cfg.addr_loc_0 |= NAND_RD_LOC_SIZE(data_len);

	cmd_list_ptr = qpic_nand_add_onfi_probe_ce(params, cmd_list_ptr);

	/* Enqueue the desc for the above commands */
	bam_add_one_desc(&bam,
			 CMD_PIPE_INDEX,
			 (unsigned char*)cmd_list_ptr_start,
			 ((addr_t)(uint32_t)cmd_list_ptr -
			 (uint32_t)cmd_list_ptr_start),
			 desc_flags);

	cmd_list_ptr_start = cmd_list_ptr;
	num_desc++;

	/* Add Data desc */
	bam_add_desc(&bam,
		     DATA_PRODUCER_PIPE_INDEX,
		     (unsigned char *)((addr_t)data_ptr),
		     data_len,
		     BAM_DESC_INT_FLAG);

	/* Wait for the commands to be executed */
	qpic_nand_wait_for_cmd_exec(num_desc);

	/* Read buffer status and check for errors. */
	status = qpic_nand_read_reg(NAND_FLASH_STATUS, 0);

	nand_ret = qpic_nand_check_status(mtd, status);
	if (nand_ret) {
		goto onfi_probe_exec_err;
	}

	/* Wait for data to be available */
	qpic_nand_wait_for_data(DATA_PRODUCER_PIPE_INDEX);

	/* Check for errors */
	nand_ret = qpic_nand_check_status(mtd, status);

onfi_probe_exec_err:
	return nand_ret;
}

/* TODO: check why both vld and cmd need to be written. */
void
qpic_nand_onfi_probe_cleanup(uint32_t vld, uint32_t dev_cmd1)
{
	struct cmd_element *cmd_list_ptr = ce_array;
	struct cmd_element *cmd_list_ptr_start = ce_array;

	bam_add_cmd_element(cmd_list_ptr, NAND_DEV_CMD1, dev_cmd1,
			    CE_WRITE_TYPE);
	cmd_list_ptr++;
	bam_add_cmd_element(cmd_list_ptr, NAND_DEV_CMD_VLD, vld,
			    CE_WRITE_TYPE);
	cmd_list_ptr++;

	/* Enqueue the desc for the above commands */
	bam_add_one_desc(&bam,
			 CMD_PIPE_INDEX,
			 (unsigned char*)cmd_list_ptr_start,
			 ((uint32_t)cmd_list_ptr -
			 (uint32_t)cmd_list_ptr_start),
			 BAM_DESC_UNLOCK_FLAG | BAM_DESC_CMD_FLAG|
			 BAM_DESC_INT_FLAG);

	qpic_nand_wait_for_cmd_exec(1);
}

static int
qpic_nand_onfi_save_params(struct mtd_info *mtd,
			   struct onfi_param_page *param_page)
{
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);
	int onfi_ret = NANDC_RESULT_SUCCESS;
	uint32_t ecc_bits;

	dev->page_size = param_page->data_per_pg;
	mtd->writesize = dev->page_size;
	dev->block_size = param_page->pgs_per_blk * (dev->page_size);
	mtd->erasesize = dev->block_size;
	dev->num_blocks = param_page->blks_per_LUN;
	dev->widebus = param_page->feature_supported & 0x1;
	dev->density = param_page->blks_per_LUN * (dev->block_size);
	mtd->size = dev->density;
	dev->spare_size = param_page->spare_per_pg;
	mtd->oobsize = dev->spare_size;
	ecc_bits = param_page->num_bits_ecc_correctability;
	dev->num_pages_per_blk = param_page->pgs_per_blk;
	dev->num_pages_per_blk_mask = param_page->pgs_per_blk - 1;

	if (ecc_bits >= 8)
		dev->ecc_width = NAND_WITH_8_BIT_ECC;
	else
		dev->ecc_width = NAND_WITH_4_BIT_ECC;

	return onfi_ret;
}

static void
qpic_nand_save_config(struct mtd_info *mtd)
{
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);
	struct nand_chip *chip = MTD_NAND_CHIP(mtd);

	/* Save Configurations */
	dev->cws_per_page = dev->page_size >> NAND_CW_DIV_RIGHT_SHIFT;

	/* Verify that we have enough buffer to handle all the cws in a page. */
	if (!(dev->cws_per_page <= QPIC_NAND_MAX_CWS_IN_PAGE)) {
		printf("Not enough buffer to handle CW \n");
		return;
	}

	/* Codeword Size = UD_SIZE_BYTES + ECC_PARITY_SIZE_BYTES
	 *                          + SPARE_SIZE_BYTES + Bad Block size
	 */
	if (dev->ecc_width & NAND_WITH_8_BIT_ECC) {
		dev->cw_size = NAND_CW_SIZE_8_BIT_ECC;
		/* Use 8-bit ecc */
		dev->ecc_bch_cfg |= (1 << NAND_DEV0_ECC_MODE_SHIFT);

		if (dev->widebus) {
			/* spare size bytes in each CW */
			dev->cfg0 |= (0 << NAND_DEV0_CFG0_SPARE_SZ_BYTES_SHIFT);
			/* parity bytes in each CW */
			dev->ecc_bch_cfg |= (14 <<
					     NAND_DEV0_ECC_PARITY_SZ_BYTES_SHIFT);
		} else {
			/* spare size bytes in each CW */
			dev->cfg0 |= (2 << NAND_DEV0_CFG0_SPARE_SZ_BYTES_SHIFT);
			/* parity bytes in each CW */
			dev->ecc_bch_cfg |= (13 <<
					     NAND_DEV0_ECC_PARITY_SZ_BYTES_SHIFT);
		}
	} else {
		dev->cw_size = NAND_CW_SIZE_4_BIT_ECC;
		if (dev->widebus) {
			/* spare size bytes in each CW */
			dev->cfg0 |= (2 << NAND_DEV0_CFG0_SPARE_SZ_BYTES_SHIFT);
			/* parity bytes in each CW */
			dev->ecc_bch_cfg |= (8 <<
					     NAND_DEV0_ECC_PARITY_SZ_BYTES_SHIFT);
		} else {
			/* spare size bytes in each CW */
			dev->cfg0 |= (4 << NAND_DEV0_CFG0_SPARE_SZ_BYTES_SHIFT);
			/* parity bytes in each CW */
			dev->ecc_bch_cfg |= (7 <<
					     NAND_DEV0_ECC_PARITY_SZ_BYTES_SHIFT);
		}
	}

	mtd->oobavail = (mtd->oobsize)/(dev->cws_per_page);
	dev->oob_per_page = mtd->oobavail;
	/* BAD_BLOCK_BYTE_NUM = Page Size - (CW_PER_PAGE * Codeword Size) + 1
	 * Note: Set CW_PER_PAGE to 1 less than the actual number.
	 */
	dev->bad_blk_loc = dev->page_size - dev->cw_size *
				 (dev->cws_per_page - 1) + 1;

	dev->cfg0 |= ((dev->cws_per_page - 1) << NAND_DEV0_CFG0_CW_PER_PAGE_SHIFT) /* 4/8 cw/pg for 2/4k */
				|(DATA_BYTES_IN_IMG_PER_CW << NAND_DEV0_CFG0_UD_SIZE_BYTES_SHIFT) /* 516 user data bytes */
				|(5 << NAND_DEV0_CFG0_ADDR_CYCLE_SHIFT) /* 5 address cycles */
				|(0 << NAND_DEV0_CFG0_DIS_STS_AFTER_WR_SHIFT);/* Send read status cmd after each write. */

	dev->cfg1 |= (7 << NAND_DEV0_CFG1_RECOVERY_CYCLES_SHIFT) /* 8 recovery cycles */
				|(0 << NAND_DEV0_CFG1_CS_ACTIVE_BSY_SHIFT)		/* Allow CS deassertion */
				|(dev->bad_blk_loc << NAND_DEV0_CFG1_BAD_BLK_BYTE_NUM_SHIFT)/* Bad block marker location */
				|(0 << NAND_DEV0_CFG1_BAD_BLK_IN_SPARE_SHIFT)		/* Bad block in user data area */
				|(2 << NAND_DEV0_CFG1_WR_RD_BSY_GAP_SHIFT)		/* 8 cycle tWB/tRB */
				|(dev->widebus << NAND_DEV0_CFG1_WIDE_BUS_SHIFT);	/* preserve wide flash flag */

	dev->cfg0_raw = ((dev->cws_per_page- 1) << NAND_DEV0_CFG0_CW_PER_PAGE_SHIFT)
					|(5 << NAND_DEV0_CFG0_ADDR_CYCLE_SHIFT)
					|(528 << NAND_DEV0_CFG0_UD_SIZE_BYTES_SHIFT) //figure out the size of cw
					| (1 << NAND_DEV0_CFG0_DIS_STS_AFTER_WR_SHIFT);

	dev->cfg1_raw = (7 <<  NAND_DEV0_CFG1_RECOVERY_CYCLES_SHIFT)
					| (0 <<  NAND_DEV0_CFG1_CS_ACTIVE_BSY_SHIFT)
					| (17 <<  NAND_DEV0_CFG1_BAD_BLK_BYTE_NUM_SHIFT)
					| (1 << NAND_DEV0_CFG1_BAD_BLK_IN_SPARE_SHIFT)
					| (2 << NAND_DEV0_CFG1_WR_RD_BSY_GAP_SHIFT)
					| (dev->widebus << NAND_DEV0_CFG1_WIDE_BUS_SHIFT)
					| 1 ; /* to disable reed solomon ecc..this feild is now read only. */

	dev->ecc_bch_cfg |= (0 << NAND_DEV0_ECC_DISABLE_SHIFT) /* Enable ECC */
						| (0 << NAND_DEV0_ECC_SW_RESET_SHIFT) /* Put ECC core in op mode */
						| (DATA_BYTES_IN_IMG_PER_CW <<
						   NAND_DEV0_ECC_NUM_DATA_BYTES)
						| (1 << NAND_DEV0_ECC_FORCE_CLK_OPEN_SHIFT); /* Enable all clocks */

	/*
	 * Safe to use a single instance global variable,
	 * fake_ecc_layout, since we will be called only once for the
	 * lifetime of the driver. We can be called more than once,
	 * but the previous instance of the driver would have been
	 * deinit before the next one is inited.
	 */
	memset(&fake_ecc_layout, 0, sizeof(fake_ecc_layout));
	chip->ecc.layout = &fake_ecc_layout;
}

/*
 * Check if it a spansion device which needs S/W Reset
 * before Read parameter Page command
 */
static int
is_spansion_device(uint8_t manu_id, uint8_t dev_id)
{
	int i;
	for (i = 0; spansion_devices[i].id; i++) {
		if (spansion_devices[i].id == manu_id &&
				spansion_devices[i].device == dev_id)
			return 1;
	}

	return 0;
}

/* Onfi probe should issue the following commands to the flash device:
 * 1. Read ID - with addr ONFI_READ_ID_ADDR.
 *              This returns the ONFI ASCII string indicating support for ONFI.
 * 2. Read Prameter Page - with addr ONFI_READ_PARAM_PAGE_ADDR.
 *                         This returns the params for the device.
 * Each command inturn issues commands- ADDR0, ADDR1, chip_select,
 * cfg0, cfg1, cmd_vld, dev_cmd1, read_loc0, flash, exec.
 */
static int
qpic_nand_onfi_probe(struct mtd_info *mtd)
{
	struct onfi_probe_params params;
	uint32_t vld;
	uint32_t dev_cmd1;
	unsigned char *buffer;
	unsigned char onfi_str[4];
	uint32_t *id;
	struct onfi_param_page *param_page;
	int onfi_ret = NANDC_RESULT_SUCCESS;
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);
	uint8_t num_desc = 0;
	struct cmd_element *cmd_list_ptr;
	struct cmd_element *cmd_list_ptr_start;
	uint32_t status, nand_ret;

	/* Allocate memory required to read the onfi param page */
	buffer = (unsigned char*) malloc(ONFI_READ_PARAM_PAGE_BUFFER_SIZE);
	if (buffer == NULL) {
		printf("Buffer Alloc Failed \n");
		return -ENOMEM;
	}

	/* Read the vld and dev_cmd1 registers before modifying */
	vld = qpic_nand_read_reg(NAND_DEV_CMD_VLD, 0);
	dev_cmd1 = qpic_nand_read_reg(NAND_DEV_CMD1, 0);

	/* Initialize flash cmd */
	params.cfg.cmd = NAND_CMD_PAGE_READ;
	params.cfg.exec = 1;

	/* Execute Read ID cmd */

	/* Initialize the config */
	params.cfg.cfg0 = NAND_CFG0_RAW_ONFI_ID;
	params.cfg.cfg1 = NAND_CFG1_RAW_ONFI_ID;

	/* Initialize the cmd and vld */
	params.dev_cmd1 = (dev_cmd1 & 0xFFFFFF00) | ONFI_READ_ID_CMD;
	params.vld = vld & 0xFFFFFFFE;

	/* Initialize the address
	 * addr1 is not used bcos of the cfg.
	 */
	params.cfg.addr0 = ONFI_READ_ID_ADDR;
	params.cfg.addr1 = 0;

	/* Lock the pipe and execute the cmd. */
	onfi_ret = onfi_probe_cmd_exec(mtd, &params, onfi_str,
				       ONFI_READ_ID_BUFFER_SIZE);
	if (onfi_ret) {
		printf("ONFI Read id cmd failed\n");
		goto qpic_nand_onfi_probe_err;
	}

	/* Write back vld and cmd and unlock the pipe. */
	qpic_nand_onfi_probe_cleanup(vld, dev_cmd1);

	/* Check for onfi string */
	id = (uint32_t*)onfi_str;
	if (*id != ONFI_SIGNATURE) {
		printf("Not an ONFI device\n");
		/* Not an onfi device. Return error. */
		onfi_ret = NANDC_RESULT_DEV_NOT_SUPPORTED;
		goto qpic_nand_onfi_probe_err;
	}

	printf("ONFI device found\n");

	nand_ret = is_spansion_device(dev->vendor, dev->device);
	if (nand_ret) {
		printf("Is a spansion device needing reset logic\n");
		/*
	 	 * Add Reset logic for Spansion
	 	 */
		cmd_list_ptr = ce_array;
		cmd_list_ptr_start = ce_array;
		bam_add_cmd_element(cmd_list_ptr, NAND_FLASH_CMD,
				0x0D, CE_WRITE_TYPE);
		cmd_list_ptr++;
		bam_add_cmd_element(cmd_list_ptr, NAND_DEV_CMD_VLD,
				(u32)NAND_CMD_VALID_BASE, CE_WRITE_TYPE);
		cmd_list_ptr++;

		bam_add_cmd_element(cmd_list_ptr, NAND_EXEC_CMD,
				0x1, CE_WRITE_TYPE);
		cmd_list_ptr++;
		bam_add_one_desc(&bam, CMD_PIPE_INDEX,
				(unsigned char *)cmd_list_ptr_start,
				((uint32_t)cmd_list_ptr - (uint32_t)cmd_list_ptr_start),
				BAM_DESC_NWD_FLAG | BAM_DESC_CMD_FLAG |
				BAM_DESC_INT_FLAG);
		num_desc++;
		qpic_nand_wait_for_cmd_exec(num_desc);
		status = qpic_nand_read_reg(NAND_FLASH_STATUS, 0);

		/* Check for errors */
		nand_ret = qpic_nand_check_status(mtd, status);
		if (nand_ret) {
			printf("Reset cmd status failed\n");
			return nand_ret;
		}
	}

	/* Now read the param page */
	/* Initialize the config */
	params.cfg.cfg0 = NAND_CFG0_RAW_ONFI_PARAM_PAGE;
	params.cfg.cfg1 = NAND_CFG1_RAW_ONFI_PARAM_PAGE;

	/* Initialize the cmd and vld */
	params.dev_cmd1 = (dev_cmd1 & 0xFFFFFF00) | ONFI_READ_PARAM_PAGE_CMD;
	params.vld = vld & 0xFFFFFFFE;

	/* Initialize the address
	 * addr1 is not used bcos of the cfg.
	 */
	params.cfg.addr0 = ONFI_READ_PARAM_PAGE_ADDR;
	params.cfg.addr1 = 0;

	/* Lock the pipe and execute the cmd. */
	onfi_ret = onfi_probe_cmd_exec(mtd, &params, buffer,
				       ONFI_READ_PARAM_PAGE_BUFFER_SIZE);
	if (onfi_ret) {
		printf("ONFI Read param page failed\n");
		goto qpic_nand_onfi_probe_err;
	}

	/* Write back vld and cmd and unlock the pipe. */
	qpic_nand_onfi_probe_cleanup(vld, dev_cmd1);

	/* Verify the integrity of the returned page */
	param_page = (struct onfi_param_page*)buffer;

	/* TODO: Add CRC check to validate the param page. */

	/* Save the parameter values */
	onfi_ret = qpic_nand_onfi_save_params(mtd, param_page);

qpic_nand_onfi_probe_err:
	if (onfi_ret)
		printf("ONFI probe failed\n");

	if (buffer)
		free(buffer);

	return onfi_ret;
}

/* Enquues a desc for a flash cmd with NWD flag set:
 * cfg: Defines the configuration for the flash cmd.
 * start: Address where the command elements are added.
 *
 * Returns the address where the next cmd element can be added.
 */
struct cmd_element*
qpic_nand_add_cmd_ce(struct cfg_params *cfg,
                                 struct cmd_element *start)
{
	struct cmd_element *cmd_list_ptr;

	cmd_list_ptr = qpic_nand_add_addr_n_cfg_ce(cfg, start);

	bam_add_cmd_element(cmd_list_ptr, NAND_FLASH_CMD, (uint32_t)cfg->cmd,
			    CE_WRITE_TYPE);
	cmd_list_ptr++;

	bam_add_cmd_element(cmd_list_ptr, NAND_EXEC_CMD, (uint32_t)cfg->exec,
			    CE_WRITE_TYPE);
	cmd_list_ptr++;

	return cmd_list_ptr;
}

/* Reads nand_flash_status */
struct cmd_element*
qpic_nand_add_read_ce(struct cmd_element *start, uint32_t *flash_status_read)
{
	struct cmd_element *cmd_list_ptr = start;

	bam_add_cmd_element(cmd_list_ptr, NAND_FLASH_STATUS,
			   (uint32_t)((addr_t)flash_status_read), CE_READ_TYPE);
	cmd_list_ptr++;

	return cmd_list_ptr;
}

/* Resets nand_flash_status and nand_read_status */
struct cmd_element*
qpic_nand_reset_status_ce(struct cmd_element *start, uint32_t read_status)
{
	struct cmd_element *cmd_list_ptr = start;
	uint32_t flash_status_reset;
	uint32_t read_status_reset;

	/* Read and reset the status registers. */
	flash_status_reset = NAND_FLASH_STATUS_RESET;
	read_status_reset = NAND_READ_STATUS_RESET;

	bam_add_cmd_element(cmd_list_ptr, NAND_FLASH_STATUS,
			   (uint32_t)flash_status_reset, CE_WRITE_TYPE);
	cmd_list_ptr++;

	if (read_status) {
		bam_add_cmd_element(cmd_list_ptr, NAND_READ_STATUS,
				   (uint32_t)read_status_reset, CE_WRITE_TYPE);
		cmd_list_ptr++;
	}

	return cmd_list_ptr;
}

struct cmd_element*
qpic_nand_add_isbad_cmd_ce(struct cfg_params *cfg,
                                 struct cmd_element *start)
{
	struct cmd_element *cmd_list_ptr = start;

	bam_add_cmd_element(cmd_list_ptr, NAND_DEV0_ECC_CFG,
			   (uint32_t)cfg->ecc_cfg, CE_WRITE_TYPE);
	cmd_list_ptr++;

	bam_add_cmd_element(cmd_list_ptr, NAND_READ_LOCATION_n(0),
			   (uint32_t)cfg->addr_loc_0, CE_WRITE_TYPE);
	cmd_list_ptr++;

	cmd_list_ptr = qpic_nand_add_cmd_ce(cfg, cmd_list_ptr);

	return cmd_list_ptr;
}

static int
qpic_nand_block_isbad_exec(struct mtd_info *mtd,
			   struct cfg_params *params,
			   uint8_t *bad_block)
{

	struct cmd_element *cmd_list_ptr = ce_array;
	struct cmd_element *cmd_list_ptr_start = ce_array;
	uint8_t desc_flags = BAM_DESC_NWD_FLAG | BAM_DESC_CMD_FLAG
				| BAM_DESC_LOCK_FLAG | BAM_DESC_INT_FLAG;
	int num_desc = 0;
	uint32_t status = 0;
	int nand_ret = NANDC_RESULT_SUCCESS;

	cmd_list_ptr = qpic_nand_add_isbad_cmd_ce(params, cmd_list_ptr);

	/* Enqueue the desc for the above commands */
	bam_add_one_desc(&bam,
			 CMD_PIPE_INDEX,
			 (unsigned char*)cmd_list_ptr_start,
			 ((uint32_t)cmd_list_ptr - (uint32_t)cmd_list_ptr_start),
			 desc_flags);

	num_desc++;

	/* Add Data desc */
	bam_add_desc(&bam,
		     DATA_PRODUCER_PIPE_INDEX,
		     (unsigned char *)((addr_t)bad_block),
		     4,
		     BAM_DESC_INT_FLAG);

	qpic_nand_wait_for_cmd_exec(num_desc);

	status = qpic_nand_read_reg(NAND_FLASH_STATUS, 0);

	nand_ret = qpic_nand_check_status(mtd, status);

	/* Dummy read to unlock pipe. */
	status = qpic_nand_read_reg(NAND_FLASH_STATUS, BAM_DESC_UNLOCK_FLAG);

	if (nand_ret)
		return nand_ret;

	qpic_nand_wait_for_data(DATA_PRODUCER_PIPE_INDEX);

	return nand_ret;
}

/**
 * qpic_nand_block_isbad() - Checks is given block is bad
 * @page - number of page the block starts at
 *
 * Returns nand_result_t
 */
static int qpic_nand_block_isbad(struct mtd_info *mtd, loff_t offs)
{
	unsigned cwperpage;
	struct cfg_params params;
	uint8_t bad_block[4];
	unsigned nand_ret = NANDC_RESULT_SUCCESS;
	uint32_t page;
	struct nand_chip *chip = MTD_NAND_CHIP(mtd);
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);

	/* Check for invalid offset */
	if (offs > mtd->size)
		return -EINVAL;

	if (offs & (mtd->erasesize - 1))
		return -EINVAL;

	page = offs >> chip->page_shift;
		
	/* Read the bad block value from the flash.
	 * Bad block value is stored in the first page of the block.
	 */
	/* Read the first page in the block. */
	cwperpage = (dev->cws_per_page);

	/* Read page cmd */
	params.cmd = NAND_CMD_PAGE_READ_ECC;
	/* Clear the CW per page bits */
	params.cfg0 = dev->cfg0_raw & ~(7U <<
			 NAND_DEV0_CFG0_CW_PER_PAGE_SHIFT);
	params.cfg1 = dev->cfg1_raw;

	/* addr0 - Write column addr + few bits in row addr upto 32 bits. */
	params.addr0 = ((page << 16) | (USER_DATA_BYTES_PER_CW  *
					 (cwperpage)));

	/* addr1 - Write rest of row addr.
	 * This will be all 0s.
	 */
	params.addr1 = (page >> 16) & 0xff;
	params.addr_loc_0 = NAND_RD_LOC_OFFSET(0);
	params.addr_loc_0 |= NAND_RD_LOC_LAST_BIT(1);
	params.addr_loc_0 |= NAND_RD_LOC_SIZE(4); /* Read 4 bytes */
 	params.ecc_cfg = 0x1; /* Disable ECC */
	params.exec = 1;

	if (qpic_nand_block_isbad_exec(mtd, &params, bad_block)) {
		printf("Could not read bad block value\n");
		return NANDC_RESULT_FAILURE;
	}

	if (dev->widebus) {
		if (bad_block[0] != 0xFF && bad_block[1] != 0xFF) {
			nand_ret = NANDC_RESULT_BAD_BLOCK;
		}
	} else if (bad_block[0] != 0xFF) {
		nand_ret = NANDC_RESULT_BAD_BLOCK;
	}
	return nand_ret;
}

/* Return num of desc added. */
static void
qpic_nand_add_wr_page_cws_cmd_desc(struct mtd_info *mtd, struct cfg_params *cfg,
				   uint32_t status[],
				   enum nand_cfg_value cfg_mode)
{
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);
	struct cmd_element *cmd_list_ptr = ce_array;
	struct cmd_element *cmd_list_read_ptr = ce_read_array;
	struct cmd_element *cmd_list_ptr_start = ce_array;
	struct cmd_element *cmd_list_read_ptr_start = ce_read_array;
	uint32_t ecc;
	int num_desc = 0;
	int int_flag = 0;
	unsigned int i;

	if (cfg_mode == NAND_CFG)
		ecc = dev->ecc_bch_cfg;
	else
		ecc = 0x1; /* Disable ECC */

	/* Add ECC configuration */
	bam_add_cmd_element(cmd_list_ptr, NAND_DEV0_ECC_CFG,
						(uint32_t)ecc, CE_WRITE_TYPE);
	cmd_list_ptr++;
	cmd_list_ptr = qpic_nand_add_addr_n_cfg_ce(cfg, cmd_list_ptr);

	bam_add_cmd_element(cmd_list_ptr, NAND_FLASH_CMD,
						(uint32_t)cfg->cmd, CE_WRITE_TYPE);
	cmd_list_ptr++;

	/* Enqueue the desc for the above commands */
	bam_add_one_desc(&bam,
			 CMD_PIPE_INDEX,
			 (unsigned char*)cmd_list_ptr_start,
			 ((uint32_t)cmd_list_ptr - (uint32_t)cmd_list_ptr_start),
			 BAM_DESC_CMD_FLAG | BAM_DESC_LOCK_FLAG);

	num_desc++;

	/* Add CE for all the CWs */
	for (i = 0; i < (dev->cws_per_page); i++) {
		cmd_list_ptr_start = cmd_list_ptr;
		int_flag = BAM_DESC_INT_FLAG;

		bam_add_cmd_element(cmd_list_ptr, NAND_EXEC_CMD, (uint32_t)cfg->exec,
				    CE_WRITE_TYPE);
		cmd_list_ptr++;

		/* Enqueue the desc for the above commands */
		bam_add_one_desc(&bam,
				 CMD_PIPE_INDEX,
				 (unsigned char*)cmd_list_ptr_start,
				 ((uint32_t)cmd_list_ptr - (uint32_t)cmd_list_ptr_start),
				 BAM_DESC_NWD_FLAG | BAM_DESC_CMD_FLAG);

		num_desc++;
		cmd_list_ptr_start = cmd_list_ptr;
		cmd_list_read_ptr_start = cmd_list_read_ptr;

		cmd_list_read_ptr = qpic_nand_add_read_ce(cmd_list_read_ptr_start,
							  &status[i]);
		/* Enqueue the desc for the NAND_FLASH_STATUS read command */
		bam_add_one_desc(&bam,
				 CMD_PIPE_INDEX,
				 (unsigned char*)cmd_list_read_ptr_start,
				 ((uint32_t)cmd_list_read_ptr -
				 (uint32_t)cmd_list_read_ptr_start),
				 BAM_DESC_CMD_FLAG);

		/* Set interrupt bit only for the last CW */
		if (i == (dev->cws_per_page) - 1)
			cmd_list_ptr = qpic_nand_reset_status_ce(cmd_list_ptr,
								 1);
		else
			cmd_list_ptr = qpic_nand_reset_status_ce(cmd_list_ptr,
								 0);

		/* Enqueue the desc for NAND_FLASH_STATUS and NAND_READ_STATUS
		 * write commands */
		bam_add_one_desc(&bam,
				 CMD_PIPE_INDEX,
				 (unsigned char*)cmd_list_ptr_start,
				 ((uint32_t)cmd_list_ptr - 
				 (uint32_t)cmd_list_ptr_start),
				 int_flag | BAM_DESC_CMD_FLAG | BAM_DESC_UNLOCK_FLAG);
		num_desc += 2;

		qpic_nand_wait_for_cmd_exec(num_desc);

		status[i] = qpic_nand_check_status(mtd, status[i]);

		num_desc = 0;
	}
	return;
}

void
qpic_add_wr_page_cws_data_desc(struct mtd_info *mtd, const void *buffer,
			       enum nand_cfg_value cfg_mode,
			       const void *spareaddr)
{
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);
	int len;
	int flags;
	uint32_t start;
	unsigned num_desc = 0;
	unsigned i;

	for( i = 0; i < (dev->cws_per_page); i++) {
		flags = 0;

		/* Set the interrupt flag on the last CW write for the page. */
		if( i == (dev->cws_per_page) - 1)
			flags |= BAM_DESC_INT_FLAG;

		if (cfg_mode != NAND_CFG_RAW) {
			start = (uint32_t)buffer + i * DATA_BYTES_IN_IMG_PER_CW;
			if (i < ((dev->cws_per_page) - 1)) {
				len = DATA_BYTES_IN_IMG_PER_CW;
				flags |= BAM_DESC_EOT_FLAG;
			} else {
				/* Allow space for spare bytes in the last page */
				len = USER_DATA_BYTES_PER_CW -
					 (((dev->cws_per_page) -  1) << 2);
				flags = 0;
			}
		} else {
			start = (uint32_t)buffer + i * (dev->cw_size);
			if (i < ((dev->cws_per_page) - 1)) {
				len = (dev->cw_size);
				flags |= BAM_DESC_EOT_FLAG;
			}
			else {
				len = (dev->cw_size - mtd->oobsize);
				flags = 0;
			}
		}
		bam_add_one_desc(&bam, DATA_CONSUMER_PIPE_INDEX, (unsigned char*)(start),
				 len, flags);
		num_desc++;

		if ((i == ((dev->cws_per_page) - 1))) {
			/* write extra data */
			start = (uint32_t)spareaddr;
			if (cfg_mode == NAND_CFG)
				len = ((dev->cws_per_page) << 2);
			else
				len = mtd->oobsize;
			flags = BAM_DESC_EOT_FLAG | BAM_DESC_INT_FLAG;
			bam_add_one_desc(&bam, DATA_CONSUMER_PIPE_INDEX,
					(unsigned char*)(start), len, flags);
			num_desc++;
		}
	}

	bam_sys_gen_event(&bam, DATA_CONSUMER_PIPE_INDEX, num_desc);
}

static nand_result_t
qpic_nand_write_page(struct mtd_info *mtd, uint32_t pg_addr,
					 enum nand_cfg_value cfg_mode,
					 struct mtd_oob_ops *ops)
{
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);
	struct cfg_params cfg;
	uint32_t status[QPIC_NAND_MAX_CWS_IN_PAGE];
	int nand_ret = NANDC_RESULT_SUCCESS;
	unsigned i;

	if (cfg_mode == NAND_CFG_RAW) {
		cfg.cfg0 = dev->cfg0_raw;
		cfg.cfg1 = dev->cfg1_raw;
		cfg.cmd = NAND_CMD_PRG_PAGE;
	} else {
		cfg.cfg0 = dev->cfg0;
		cfg.cfg1 = dev->cfg1;
		cfg.cmd = NAND_CMD_PRG_PAGE_ALL;
	}

	cfg.exec = 1;

	cfg.addr0 = pg_addr << 16;
	cfg.addr1 = (pg_addr >> 16) & 0xff;

	qpic_add_wr_page_cws_data_desc(mtd, ops->datbuf, cfg_mode, ops->oobbuf);

	qpic_nand_add_wr_page_cws_cmd_desc(mtd, &cfg, status, cfg_mode);

	/* Check for errors */
	for(i = 0; i < (dev->cws_per_page); i++) {
		nand_ret = qpic_nand_check_status(mtd, status[i]);
		if (nand_ret) {
			printf(
				"Failed to write CW %d for page: %d\n",
				i, pg_addr);
			break;
		}
	}

	/* Wait for data to be available */
	qpic_nand_wait_for_data(DATA_CONSUMER_PIPE_INDEX);

	ops->retlen += mtd->writesize;
	ops->datbuf += mtd->writesize;

	if (ops->oobbuf != NULL) {
			ops->oobretlen += dev->oob_per_page;
			ops->oobbuf += dev->oob_per_page;
	}

	return nand_ret;
}

static int
qpic_nand_mark_badblock(struct mtd_info *mtd, loff_t offs)
{
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);
	struct nand_chip *chip = MTD_NAND_CHIP(mtd);
	uint32_t page;
	int nand_ret = NANDC_RESULT_SUCCESS;
	struct mtd_oob_ops ops;

	/* Check for invalid offset */
	if (offs > mtd->size)
		return -EINVAL;

	if (offs & (mtd->erasesize - 1))
		return -EINVAL;

	page = offs >> chip->page_shift;

	ops.mode = MTD_OOB_RAW;
	ops.len = mtd->writesize;
	ops.retlen = 0;
	ops.ooblen = mtd->oobsize;
	ops.oobretlen = 0;
	ops.ooboffs = 0;
	ops.datbuf = dev->zero_page;
	ops.oobbuf = dev->zero_oob;

	/* Going to first page of the block */
	if (page & (dev->num_pages_per_blk_mask))
		page = page - (page & (dev->num_pages_per_blk_mask));

	nand_ret = qpic_nand_write_page(mtd, page, NAND_CFG_RAW, &ops);
	if (!nand_ret)
		mtd->ecc_stats.badblocks++;
	return nand_ret;

}

/*
 * Populate flash parameters from the look up table.
 */
static void qpic_nand_get_info_flash_dev(struct mtd_info *mtd,
                    const struct nand_flash_dev *flash_dev)
{
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);
	mtd->writesize = dev->page_size = flash_dev->pagesize;
	mtd->erasesize = dev->block_size = flash_dev->erasesize;
	mtd->oobsize = dev->spare_size = (flash_dev->pagesize >> 5);
}


/*
 * Populate flash parameters from the configuration byte.
 */
static void qpic_nand_get_info_cfg(struct mtd_info *mtd, uint8_t cfg_id)
{
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);
	u_int cfg_page_size;
	u_int cfg_block_size;
	u_int cfg_spare_size;
	u_int chunks;
	u_int spare_per_chunk;

	/* writesize = 1KB * (2 ^ cfg_page_size) */
	cfg_page_size = NAND_CFG_PAGE_SIZE(cfg_id);
	mtd->writesize = dev->page_size = (1024 << cfg_page_size);

	/* erasesize = 64KB * (2 ^ cfg_block_size) */
	cfg_block_size = NAND_CFG_BLOCK_SIZE(cfg_id);
	mtd->erasesize = dev->block_size = ((64 * 1024) << cfg_block_size);

	/* Spare per 512B = 8 * (2 ^ cfg_spare_size) */
	cfg_spare_size = NAND_CFG_SPARE_SIZE(cfg_id);
	chunks = (mtd->writesize / CHUNK_SIZE);
	spare_per_chunk = (8 << cfg_spare_size);
	mtd->oobsize = dev->spare_size = (spare_per_chunk * chunks);

	if ((mtd->oobsize > 64) && (mtd->writesize == 2048)) {
		printf("ipq_nand: Found a 2K page device with"
			" %d oobsize - changing oobsize to 64"
			" bytes.\n", mtd->oobsize);
		mtd->oobsize = 64;
	}
}

/*
 * Retreive the flash info entry using the device ID.
 */
static const struct nand_flash_dev *flash_get_dev(uint8_t dev_id)
{
	int i;

	for (i = 0; nand_flash_ids[i].id; i++) {
		if (nand_flash_ids[i].id == dev_id)
			return &nand_flash_ids[i];
	}

	return NULL;
}

/*
 * Retreive the manuf. info entry using manufacturer ID.
 */
static const struct nand_manufacturers *flash_get_man(uint8_t man_id)
{
	int i;

	for (i = 0; nand_manuf_ids[i].id; i++) {
		if (nand_manuf_ids[i].id == man_id)
			return &nand_manuf_ids[i];
	}

	return NULL;
}


/*
 * Populate flash parameters for non-ONFI devices.
 */
static int qpic_nand_get_info(struct mtd_info *mtd, uint32_t flash_id)
{
	uint8_t man_id;
	uint8_t dev_id;
	uint8_t cfg_id;
	const struct nand_manufacturers *flash_man;
	const struct nand_flash_dev *flash_dev;
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);

	man_id = NAND_ID_MAN(flash_id);
	dev_id = NAND_ID_DEV(flash_id);
	cfg_id = NAND_ID_CFG(flash_id);

	debug("Manufacturer ID: %x\n", man_id);
	debug("Device ID: %x\n", dev_id);
	debug("Config. Byte: %x\n", cfg_id);

	flash_man = flash_get_man(man_id);
	flash_dev = flash_get_dev(dev_id);

	if (flash_man == NULL || flash_dev == NULL) {
	printf("qpic_nand: unknown NAND device manufacturer: %x"
		" device: %x\n", man_id, dev_id);
		return -ENOENT;
	}

	mtd->size = MB_TO_BYTES(flash_dev->chipsize);
	/*
	* For older NAND flash, we obtained the flash information
	* from the flash_dev table. For newer flashes the information
	* is available in the cfg byte, in the NAND ID sequence.
	*/
	if (!flash_dev->pagesize)
		qpic_nand_get_info_cfg(mtd, cfg_id);
	else
		qpic_nand_get_info_flash_dev(mtd, flash_dev);

	dev->ecc_width = NAND_WITH_4_BIT_ECC;

	dev->num_blocks = mtd->size;
	dev->num_blocks /= (dev->block_size);
	dev->num_pages_per_blk = dev->block_size / dev->page_size;
	dev->num_pages_per_blk_mask = dev->num_pages_per_blk - 1;

	return 0;
}

static void
qpic_nand_non_onfi_probe(struct mtd_info *mtd)
{
	struct qpic_nand_dev *dev= MTD_QPIC_NAND_DEV(mtd);

	/* Read the nand id. */
	qpic_nand_fetch_id(mtd);
	
	qpic_nand_get_info(mtd, dev->id);

	return;
}

static void qpic_nand_sync(struct mtd_info *mtd)
{
	/* Nop */
}

static int qpic_nand_scan_bbt_nop(struct mtd_info *mtd)
{
	return 0;
}

/*
 * Estimate the no. of pages to read, based on the data length and oob
 * length.
 */
static u_long qpic_get_read_page_count(struct mtd_info *mtd,
                      struct mtd_oob_ops *ops)
{
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);
	struct nand_chip *chip = MTD_NAND_CHIP(mtd);

	if (ops->datbuf != NULL) {
		return (ops->len + mtd->writesize - 1) >> chip->page_shift;
	} else {
	if (dev->oob_per_page == 0)
		return 0;

	return (ops->ooblen + dev->oob_per_page - 1) / dev->oob_per_page;
	}
}

/*
 * Return the buffer where the next OOB data should be stored. If the
 * user buffer is insufficient to hold one page worth of OOB data,
 * return an internal buffer, to hold the data temporarily.
 */
static uint8_t *qpic_nand_read_oobbuf(struct mtd_info *mtd,
				     struct mtd_oob_ops *ops)
{
	size_t read_ooblen;
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);

	if (ops->oobbuf == NULL)
		return NULL;

	read_ooblen = ops->ooblen - ops->oobretlen;
	if (read_ooblen < dev->oob_per_page)
		return dev->pad_oob;

	return ops->oobbuf + ops->oobretlen;
}
/*
 * Return the buffer where the next in-band data should be stored. If
 * the user buffer is insufficient to hold one page worth of in-band
 * data, return an internal buffer, to hold the data temporarily.
 */
static uint8_t *qpic_nand_read_datbuf(struct mtd_info *mtd,
				     struct mtd_oob_ops *ops)
{
	size_t read_datlen;
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);

	if (ops->datbuf == NULL)
		return NULL;

	read_datlen = ops->len - ops->retlen;
	if (read_datlen < mtd->writesize)
		return dev->pad_dat;

	return ops->datbuf + ops->retlen;
}

/*
 * Copy the OOB data from the internal buffer, to the user buffer, if
 * the internal buffer was used for the read.
 */
static void qpic_nand_read_oobcopy(struct mtd_info *mtd,
				  struct mtd_oob_ops *ops)
{
	size_t ooblen;
	size_t read_ooblen;
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);

	if (ops->oobbuf == NULL)
		return;

	read_ooblen = ops->ooblen - ops->oobretlen;
	ooblen = MIN(read_ooblen, dev->oob_per_page);
	if (read_ooblen < dev->oob_per_page)
		memcpy(ops->oobbuf + ops->oobretlen, dev->pad_oob, ooblen);

	ops->oobretlen += ooblen;
}

/*
 * Copy the in-band data from the internal buffer, to the user buffer,
 * if the internal buffer was used for the read.
 */
static void qpic_nand_read_datcopy(struct mtd_info *mtd, struct mtd_oob_ops *ops)
{
	size_t datlen;
	size_t read_datlen;
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);

	if (ops->datbuf == NULL)
		return;

	read_datlen = ops->len - ops->retlen;
	datlen = MIN(read_datlen, mtd->writesize);

	if (read_datlen < mtd->writesize)
		memcpy(ops->datbuf + ops->retlen, dev->pad_dat, datlen);

	ops->retlen += datlen;
}

static int
qpic_nand_read_page(struct mtd_info *mtd, uint32_t page,
		    enum nand_cfg_value cfg_mode,
		    struct mtd_oob_ops *ops)
{
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);
	struct cfg_params params;
	uint32_t ecc;
	uint32_t flash_sts[QPIC_NAND_MAX_CWS_IN_PAGE];
	uint32_t buffer_sts[QPIC_NAND_MAX_CWS_IN_PAGE];
	uint32_t addr_loc_0;
	uint32_t addr_loc_1;
	struct cmd_element *cmd_list_ptr = ce_array;
	struct cmd_element *cmd_list_ptr_start = ce_array;
	uint32_t num_cmd_desc = 0;
	uint32_t num_data_desc = 0;
	uint32_t i;
	int nand_ret = NANDC_RESULT_SUCCESS;
	uint8_t flags = 0;
	uint32_t *cmd_list_temp = NULL;
	uint32_t num_errors;
	uint16_t data_bytes;
	uint16_t ud_bytes_in_last_cw;
	uint16_t oob_bytes;
	unsigned char *buffer;
	unsigned char *spareaddr;

	params.addr0 = page << 16;
	params.addr1 = (page >> 16) & 0xff;
	if (cfg_mode == NAND_CFG_RAW) {
		params.cfg0 = dev->cfg0_raw;
		params.cfg1 = dev->cfg1_raw;
		params.cmd = NAND_CMD_PAGE_READ;
		ecc = 0x1; /* Disable ECC */
		data_bytes =  dev->cw_size;
		oob_bytes = mtd->oobsize;
		ud_bytes_in_last_cw = (dev->cw_size - mtd->oobsize);
	} else {
		params.cfg0 = dev->cfg0;
		params.cfg1 = dev->cfg1;
		params.cmd = NAND_CMD_PAGE_READ_ALL;
		ecc = (dev->ecc_bch_cfg);
		data_bytes = DATA_BYTES_IN_IMG_PER_CW;
		ud_bytes_in_last_cw = USER_DATA_BYTES_PER_CW -
					 (((dev->cws_per_page) - 1) << 2);
		oob_bytes = DATA_BYTES_IN_IMG_PER_CW - ud_bytes_in_last_cw;
	}
	params.exec = 1;

	/* Read all the Data bytes in the first 3 CWs. */
	addr_loc_0 = NAND_RD_LOC_OFFSET(0);
	addr_loc_0 |= NAND_RD_LOC_SIZE(data_bytes);;
	addr_loc_0 |= NAND_RD_LOC_LAST_BIT(1);

	addr_loc_1 = NAND_RD_LOC_OFFSET(ud_bytes_in_last_cw);
	addr_loc_1 |= NAND_RD_LOC_SIZE(oob_bytes);
	addr_loc_1 |= NAND_RD_LOC_LAST_BIT(1);

	/* Reset and Configure erased CW/page detection controller */
	qpic_nand_erased_status_reset(ce_array, BAM_DESC_LOCK_FLAG);

	if (ops->datbuf == NULL) {
		buffer = dev->pad_dat;
	} else {
		buffer = ops->datbuf;
	}

	if (ops->oobbuf == NULL) {
		spareaddr = dev->pad_oob;
	} else {
		spareaddr = ops->oobbuf;
	}
	/* Queue up the command and data descriptors for all the codewords in a page
	 * and do a single bam transfer at the end.*/
	for (i = 0; i < (dev->cws_per_page); i++) {
		num_cmd_desc = 0;
		num_data_desc = 0;

		if (i == 0) {
			cmd_list_ptr = qpic_nand_add_addr_n_cfg_ce(&params, cmd_list_ptr);

			bam_add_cmd_element(cmd_list_ptr, NAND_DEV0_ECC_CFG,(uint32_t)ecc,
					    CE_WRITE_TYPE);
			cmd_list_ptr++;
		} else
			cmd_list_ptr_start = cmd_list_ptr;

		bam_add_cmd_element(cmd_list_ptr, NAND_FLASH_CMD, (uint32_t)params.cmd,
				    CE_WRITE_TYPE);
		cmd_list_ptr++;

		if (i == (dev->cws_per_page) - 1) {
			addr_loc_0 = NAND_RD_LOC_OFFSET(0);
			addr_loc_0 |= NAND_RD_LOC_SIZE(ud_bytes_in_last_cw);
			addr_loc_0 |= NAND_RD_LOC_LAST_BIT(0);
					  /* Write addr loc 1 only for the last CW. */
			bam_add_cmd_element(cmd_list_ptr, NAND_READ_LOCATION_n(1),
					   (uint32_t)addr_loc_1, CE_WRITE_TYPE);
			cmd_list_ptr++;
			flags = 0;
			/* Add Data desc */
			bam_add_one_desc(&bam, DATA_PRODUCER_PIPE_INDEX,
					 (unsigned char *)((addr_t)(buffer)),
					 ud_bytes_in_last_cw,
					 flags);
			num_data_desc++;
			bam_add_one_desc(&bam,
					 DATA_PRODUCER_PIPE_INDEX,
					 (unsigned char *)((addr_t)(spareaddr)),
					 oob_bytes,
					 BAM_DESC_INT_FLAG);
			num_data_desc++;
			bam_sys_gen_event(&bam, DATA_PRODUCER_PIPE_INDEX,
					  num_data_desc);
		} else {
				/* Add Data desc */
			bam_add_one_desc(&bam,
					 DATA_PRODUCER_PIPE_INDEX,
					 (unsigned char *)((addr_t)buffer),
					 data_bytes,
					 0);
			num_data_desc++;
			bam_sys_gen_event(&bam, DATA_PRODUCER_PIPE_INDEX,
					  num_data_desc);
		}
		/* Write addr loc 0. */
		bam_add_cmd_element(cmd_list_ptr,
				    NAND_READ_LOCATION_n(0),
				    (uint32_t)addr_loc_0,
				    CE_WRITE_TYPE);

		cmd_list_ptr++;

		bam_add_cmd_element(cmd_list_ptr,
				    NAND_EXEC_CMD,
				    (uint32_t)params.exec,
				    CE_WRITE_TYPE);
		cmd_list_ptr++;

		/* Enqueue the desc for the above commands */
		bam_add_one_desc(&bam,
				 CMD_PIPE_INDEX,
				 (unsigned char*)cmd_list_ptr_start,
				 ((uint32_t)cmd_list_ptr -
				 (uint32_t)cmd_list_ptr_start),
				 BAM_DESC_NWD_FLAG | BAM_DESC_CMD_FLAG);
		num_cmd_desc++;

		bam_add_cmd_element(cmd_list_ptr, NAND_FLASH_STATUS,
				   (uint32_t)((addr_t)&(flash_sts[i])),
				   CE_READ_TYPE);

		cmd_list_temp = (uint32_t *)cmd_list_ptr;

		cmd_list_ptr++;

		bam_add_cmd_element(cmd_list_ptr, NAND_BUFFER_STATUS,
				   (uint32_t)((addr_t)&(buffer_sts[i])),
				   CE_READ_TYPE);
		cmd_list_ptr++;

		if (i == (dev->cws_per_page) - 1) {
			flags = BAM_DESC_CMD_FLAG | BAM_DESC_UNLOCK_FLAG;
		} else
			flags = BAM_DESC_CMD_FLAG;

		/* Enqueue the desc for the above command */
		bam_add_one_desc(&bam,
				 CMD_PIPE_INDEX,
				 (unsigned char*)((addr_t)cmd_list_temp),
				 ((uint32_t)cmd_list_ptr - (uint32_t)cmd_list_temp),
				 flags);
		num_cmd_desc++;

		if (ops->datbuf != NULL) {
			if (i == (dev->cws_per_page - 1)) {
				buffer += ud_bytes_in_last_cw;
				ops->datbuf += ud_bytes_in_last_cw;
				ops->retlen += ud_bytes_in_last_cw;
			} else {
				buffer = ops->datbuf + data_bytes;
				ops->datbuf += data_bytes;
				ops->retlen += data_bytes;
			}
		}
		else {
			if (i == (dev->cws_per_page - 1)) {
				buffer += ud_bytes_in_last_cw;
			} else {
				buffer += data_bytes;
			}
		}
		if ((i == (dev->cws_per_page) - 1)) {
			if (ops->oobbuf != NULL) {
				spareaddr += oob_bytes;
				ops->oobretlen += oob_bytes;
				ops->oobbuf += oob_bytes;
			} else
				spareaddr += oob_bytes;
		}
		/* Notify BAM HW about the newly added descriptors */
		bam_sys_gen_event(&bam, CMD_PIPE_INDEX, num_cmd_desc);
	}

	qpic_nand_wait_for_data(DATA_PRODUCER_PIPE_INDEX);

	/* Check status */
	for (i = 0; i < (dev->cws_per_page) ; i ++) {
		flash_sts[i] = qpic_nand_check_status(mtd, flash_sts[i]);
		if (flash_sts[i]) {
			printf("NAND page read failed. page: %x status %x\n",
				 page, flash_sts[i]);
			goto qpic_nand_read_page_error;
		}
	}

	for (i = 0; i < (dev->cws_per_page); i++) {
		num_errors = buffer_sts[i];
		num_errors &= NUM_ERRORS_MASK;
		if(num_errors)
			mtd->ecc_stats.corrected++;
	}
qpic_nand_read_page_error:
return nand_ret;
}

static int qpic_nand_read_oob(struct mtd_info *mtd, loff_t to,
                                    struct mtd_oob_ops *ops)
{
	unsigned i = 0, ret = 0;
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);
	struct nand_chip *chip = MTD_NAND_CHIP(mtd);
	uint32_t start_page;
	uint32_t num_pages;
	loff_t offs;
	uint32_t corrected;
	enum nand_cfg_value cfg_mode;

	/* We don't support MTD_OOB_PLACE as of yet. */
	if (ops->mode == MTD_OOB_PLACE)
		return -ENOSYS;

	/* Check for reads past end of device */
	if (ops->datbuf && (to + ops->len) > mtd->size)
		return -EINVAL;

	if (to & (mtd->writesize - 1))
		return -EINVAL;

	if (ops->ooboffs != 0)
		return -EINVAL;

	if(ops->mode == MTD_OOB_RAW) {
		cfg_mode = NAND_CFG_RAW;
		dev->oob_per_page = mtd->oobsize;
	} else {
		cfg_mode = NAND_CFG;
		dev->oob_per_page = mtd->oobavail;
	}

	start_page = ((to >> chip->page_shift));
	num_pages = qpic_get_read_page_count(mtd, ops);

	corrected = mtd->ecc_stats.corrected;

	for (i = 0; i < num_pages; i++) {
		struct mtd_oob_ops page_ops;
		page_ops.mode = ops->mode;
		page_ops.len = mtd->writesize;
		page_ops.ooblen = dev->oob_per_page;
		page_ops.datbuf = qpic_nand_read_datbuf(mtd, ops);
		page_ops.oobbuf = qpic_nand_read_oobbuf(mtd, ops);
		page_ops.retlen = 0;
		page_ops.oobretlen = 0;

		ret = qpic_nand_read_page(mtd, start_page + i, cfg_mode,
					  &page_ops);
		if (ret == NANDC_RESULT_BAD_PAGE) {
			offs = (start_page + i) << chip->page_shift;
			qpic_nand_mark_badblock(mtd, offs);
		}
		if (ret) {
			printf("qpic_nand_read: reading page %d failed with %d err \n",
					start_page + i, ret);
			return ret;
		}
		qpic_nand_read_datcopy(mtd, ops);
		qpic_nand_read_oobcopy(mtd, ops);
	}

	if (mtd->ecc_stats.corrected != corrected) {
		ret = -EUCLEAN;
		return ret;
	}

	return NANDC_RESULT_SUCCESS;
}

/**
 * qpic_nand_read() - read data
 * @start_page: number of page to begin reading from
 * @num_pages: number of pages to read
 * @buffer: buffer where to store the read data
 * @spareaddr: buffer where to store spare data.
 * 		If null, spare data wont be read
 *
 * This function reads @num_pages starting from @start_page and stores the
 * read data in buffer. Note that it's in the caller responsibility to make
 * sure the read pages are all from same partition.
 *
 */
static int qpic_nand_read(struct mtd_info *mtd, loff_t from, size_t len,
             size_t *retlen, u_char *buf)
{
	unsigned ret = 0;
	struct mtd_oob_ops ops;

	ops.mode = MTD_OOB_AUTO;
	ops.len = len;
	ops.retlen = 0;
	ops.ooblen = 0;
	ops.oobretlen = 0;
	ops.ooboffs = 0;
	ops.datbuf = (uint8_t *)buf;
	ops.oobbuf = NULL;

	ret = qpic_nand_read_oob(mtd, from, &ops);
	*retlen = ops.retlen;

	return ret;
}

/*
 * Return the buffer containing the in-band data to be written.
 */
static uint8_t *qpic_nand_write_datbuf(struct mtd_info *mtd,
				       struct mtd_oob_ops *ops)
{
	return ops->datbuf + ops->retlen;
}

/*
 * Return the buffer containing the OOB data to be written. If user
 * buffer does not provide on page worth of OOB data, return a padded
 * internal buffer with the OOB data copied in.
 */
static uint8_t *qpic_nand_write_oobbuf(struct mtd_info *mtd,
				       struct mtd_oob_ops *ops)
{
	size_t write_ooblen;
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);

	if (ops->oobbuf == NULL)
		return dev->pad_oob;

	write_ooblen = ops->ooblen - ops->oobretlen;
	memset(dev->pad_oob, 0xFF, dev->oob_per_page);

	if (write_ooblen < dev->oob_per_page) {
		memcpy(dev->pad_oob, ops->oobbuf + ops->oobretlen, write_ooblen);
		return dev->pad_oob;
	}

	return ops->oobbuf + ops->oobretlen;
}

/*
 * Increment the counters to indicate the no. of in-band bytes
 * written.
 */
static void qpic_nand_write_datinc(struct mtd_info *mtd,
				   struct mtd_oob_ops *ops)
{
	ops->retlen += mtd->writesize;
}

/*
 * Increment the counters to indicate the no. of OOB bytes written.
 */
static void qpic_nand_write_oobinc(struct mtd_info *mtd,
				   struct mtd_oob_ops *ops)
{
	size_t write_ooblen;
	size_t ooblen;
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);

	if (ops->oobbuf == NULL)
		return;

	write_ooblen = ops->ooblen - ops->oobretlen;
	ooblen = MIN(write_ooblen, dev->oob_per_page);

	ops->oobretlen += ooblen;
}

static int qpic_nand_write_oob(struct mtd_info *mtd, loff_t to,
				struct mtd_oob_ops *ops)

{
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);
	int i, ret = NANDC_RESULT_SUCCESS;
	struct nand_chip *chip = MTD_NAND_CHIP(mtd);
	loff_t offs;
	u_long start_page;
	u_long num_pages;
	enum nand_cfg_value cfg_mode;

	/* We don't support MTD_OOB_PLACE as of yet. */
	if (ops->mode == MTD_OOB_PLACE)
		return -ENOSYS;

	/* Check for writes past end of device. */
	if ((to + ops->len) > mtd->size)
		return -EINVAL;

	if (to & (mtd->writesize - 1))
		return -EINVAL;

	if (ops->len & (mtd->writesize - 1))
		return -EINVAL;

	if (ops->ooboffs != 0)
		return -EINVAL;

	if (ops->datbuf == NULL)
		return -EINVAL;

	if(ops->mode == MTD_OOB_RAW) {
		cfg_mode = NAND_CFG_RAW;
		dev->oob_per_page = mtd->oobsize;
	}
	else {
		cfg_mode = NAND_CFG;
		dev->oob_per_page = mtd->oobavail;
	}

	start_page = (to >> chip->page_shift);
	num_pages = ((ops->len) >> chip->page_shift);
	ops->retlen = 0;
	ops->oobretlen = 0;

	for (i = 0; i < (int)num_pages; i++) {
		struct mtd_oob_ops page_ops;

		page_ops.mode = ops->mode;
		page_ops.len = mtd->writesize;
		page_ops.ooblen = dev->oob_per_page;
		page_ops.datbuf = qpic_nand_write_datbuf(mtd,ops);
		page_ops.oobbuf = qpic_nand_write_oobbuf(mtd, ops);
		page_ops.retlen = 0;
		page_ops.oobretlen = 0;

		ret = qpic_nand_write_page(mtd, start_page + i,
				   cfg_mode, &page_ops);
		if (ret) {
			printf("flash_write: write failure @ page %ld, block %ld\n",
					start_page + i,
				(start_page + i) / (dev->num_pages_per_blk));
			if (ret == NANDC_RESULT_BAD_PAGE) {
				offs = (start_page + i) << chip->page_shift;
				qpic_nand_mark_badblock(mtd, offs);
			}
			goto out;
		} else {
			qpic_nand_write_datinc(mtd, ops);
			qpic_nand_write_oobinc(mtd, ops);
		}
	}
out:
	return ret;
}

/**
 * qpic_nand_write() - read data
 * @start_page: number of page to begin writing to
 * @num_pages: number of pages to write
 * @buffer: buffer to be written
 * @write_extra_bytes: true if spare data (ox 0xff) to be written
 *
 * This function writes @num_pages starting from @start_page. Note that it's
 * in the caller responsibility to make sure the written pages are all from
 * same partition.
 *
 */
static int qpic_nand_write(struct mtd_info *mtd, loff_t to, size_t len,
     size_t  *retlen, const u_char *buf)
{
	int ret = NANDC_RESULT_SUCCESS;
	struct mtd_oob_ops ops;

	if (!buf) {
		printf("qpic_nand_write: buffer = null\n");
		return NANDC_RESULT_PARAM_INVALID;
	}

	ops.mode = MTD_OOB_AUTO;
	ops.len = len;
	ops.retlen = 0;
	ops.ooblen = 0;
	ops.oobretlen = 0;
	ops.ooboffs = 0;
	ops.datbuf = (uint8_t *)buf;
	ops.oobbuf = NULL;

	ret = qpic_nand_write_oob(mtd, to, &ops);
	*retlen = ops.retlen;

	return ret;
}

/* Function to erase a block on the nand.
 * page: Starting page address for the block.
 */
nand_result_t qpic_nand_blk_erase(struct mtd_info *mtd, uint32_t page)
{
	struct cfg_params cfg;
	struct cmd_element *cmd_list_ptr = ce_array;
	struct cmd_element *cmd_list_read_ptr = ce_read_array;
	struct cmd_element *cmd_list_ptr_start = ce_array;
	struct cmd_element *cmd_list_read_ptr_start = ce_read_array;
	uint32_t status;
	int num_desc = 0;
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);
	uint32_t blk_addr = page / (dev->num_pages_per_blk);
	struct nand_chip *chip = MTD_NAND_CHIP(mtd);

	/* Fill in params for the erase flash cmd */
	cfg.addr0 = page;
	cfg.addr1 = 0;
	/* Clear CW_PER_PAGE in cfg0 */
	cfg.cfg0 = dev->cfg0 & ~(7U << NAND_DEV0_CFG0_CW_PER_PAGE_SHIFT);
	cfg.cfg1 = dev->cfg1;
	cfg.cmd = NAND_CMD_BLOCK_ERASE;
	cfg.exec = 1;
	cmd_list_ptr = qpic_nand_add_cmd_ce(&cfg, cmd_list_ptr);

	/* Enqueue the desc for the above commands */
	bam_add_one_desc(&bam,
		CMD_PIPE_INDEX,
		(unsigned char*)cmd_list_ptr_start,
		((uint32_t)cmd_list_ptr - (uint32_t)cmd_list_ptr_start),
		BAM_DESC_NWD_FLAG | BAM_DESC_CMD_FLAG | BAM_DESC_INT_FLAG |
			BAM_DESC_LOCK_FLAG);

	cmd_list_ptr_start = cmd_list_ptr;
	num_desc++;

	qpic_nand_wait_for_cmd_exec(num_desc);

	status = qpic_nand_read_reg(NAND_FLASH_STATUS, 0);

	cmd_list_ptr_start = cmd_list_ptr;
	cmd_list_read_ptr_start = cmd_list_read_ptr;

	/* QPIC controller automatically sends
	* GET_STATUS cmd to the nand card because
	* of the configuration programmed.
	* Read the result of GET_STATUS cmd.
	*/
	cmd_list_read_ptr = qpic_nand_add_read_ce(cmd_list_read_ptr, &status);

	/* Enqueue the desc for the NAND_FLASH_STATUS read command */
	bam_add_one_desc(&bam,
		CMD_PIPE_INDEX,
		(unsigned char*)cmd_list_read_ptr_start,
		((uint32_t)cmd_list_read_ptr - (uint32_t)cmd_list_read_ptr_start),
		BAM_DESC_CMD_FLAG);

	cmd_list_ptr = qpic_nand_reset_status_ce(cmd_list_ptr, 1);

	/* Enqueue the desc for NAND_FLASH_STATUS and NAND_READ_STATUS write commands */
	bam_add_one_desc(&bam,
		CMD_PIPE_INDEX,
		(unsigned char*)cmd_list_ptr_start,
		((uint32_t)cmd_list_ptr - (uint32_t)cmd_list_ptr_start),
		BAM_DESC_INT_FLAG | BAM_DESC_CMD_FLAG);
	num_desc = 2;
	qpic_nand_wait_for_cmd_exec(num_desc);

	status = qpic_nand_check_status(mtd, status);

	/* Dummy read to unlock pipe. */
	qpic_nand_read_reg(NAND_FLASH_STATUS, BAM_DESC_UNLOCK_FLAG);

	/* Check for status errors*/
	if (status) {
		printf("NAND Erase error: Block address belongs to "
		       "bad block: %d\n", blk_addr);
		qpic_nand_mark_badblock(mtd, (page << chip->page_shift));
		return status;
	}

	/* Check for PROG_ERASE_OP_RESULT bit for the result of erase operation. */
	if (!(status & PROG_ERASE_OP_RESULT))
		return NANDC_RESULT_SUCCESS;

	qpic_nand_mark_badblock(mtd, (page << chip->page_shift));
	return status;
}


int
qpic_nand_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	int ret = 0, i;

	loff_t offs;
	u_long blocks;
	u_long start;
	u_long pageno;
	struct nand_chip *chip = MTD_NAND_CHIP(mtd);
	struct qpic_nand_dev *dev = MTD_QPIC_NAND_DEV(mtd);

	/* Check for erase past end of device. */
	if ((instr->addr + instr->len) > mtd->size)
		return -EINVAL;

	if (instr->addr & (mtd->erasesize - 1))
		return -EINVAL;

	if (instr->len & (mtd->erasesize - 1))
		return -EINVAL;

	start = instr->addr >> chip->phys_erase_shift;
	blocks = instr->len >> chip->phys_erase_shift;

	debug("number of blks to erase: %lu\n", blocks);

	for (i = start; i < (start + blocks); i++) {
		offs = i << chip->phys_erase_shift;
		pageno = offs >> chip->page_shift;
		/* Erase only if the block is not bad */
		if (!instr->scrub && qpic_nand_block_isbad(mtd, offs)) {
			printf("NAND Erase error: Block address belongs to "
				"bad block: %ld\n",  (pageno / (dev->num_pages_per_blk)));
			return -EIO;
	}

		ret = qpic_nand_blk_erase(mtd, pageno);
		if (ret) {
			instr->fail_addr = offs;
			printf("Erase operation failed \n");
		}
	}
	return ret;
}

void
qpic_nand_mtd_params(struct mtd_info *mtd)
{
	struct nand_chip *chip = MTD_NAND_CHIP(mtd);
	mtd->type = MTD_NANDFLASH;
	mtd->flags = MTD_CAP_NANDFLASH;

	mtd->erase = qpic_nand_erase;
	mtd->point = NULL;
	mtd->unpoint = NULL;
	mtd->read = qpic_nand_read;
	mtd->write = qpic_nand_write;
	mtd->read_oob = qpic_nand_read_oob;
	mtd->write_oob = qpic_nand_write_oob;
	mtd->lock = NULL;
	mtd->unlock = NULL;
	mtd->block_isbad = qpic_nand_block_isbad;
	mtd->block_markbad = qpic_nand_mark_badblock;
	mtd->sync = qpic_nand_sync;

	mtd->ecclayout = NULL;

	chip->page_shift = ffs(mtd->writesize) - 1;
	chip->phys_erase_shift = ffs(mtd->erasesize) - 1;

	/* One of the NAND layer functions that the command layer
	 * tries to access directly.
	 */
	chip->scan_bbt = qpic_nand_scan_bbt_nop;
}

static struct nand_chip nand_chip[CONFIG_SYS_MAX_NAND_DEVICE];

int
qpic_nand_init(struct qpic_nand_init_config *config)
{
	struct mtd_info *mtd;
	struct nand_chip *chip;
	int ret = 0;
	struct qpic_nand_dev *dev;
	size_t alloc_size;
	unsigned char *buf;

	mtd = &nand_info[CONFIG_QPIC_NAND_NAND_INFO_IDX];
	mtd->priv = &nand_chip[0];

	chip = mtd->priv;
	chip->priv = &qpic_nand_dev;
    
	qpic_bam_init(config);

	ret = qpic_nand_fetch_id(mtd);
	if (ret < 0)
		return ret;
	ret = qpic_nand_onfi_probe(mtd);
	if (ret < 0)
		return ret;
	else if (ret > 0)
		qpic_nand_non_onfi_probe(mtd);

	/* Save the RAW and read/write configs */
	qpic_nand_save_config(mtd);

	dev = MTD_QPIC_NAND_DEV(mtd);
	qpic_nand_mtd_params(mtd);

	alloc_size = (mtd->writesize  /* For dev->pad_dat */
		     + mtd->oobsize   /* For dev->pad_oob */
		     + mtd->writesize /* For dev->zero_page */
		     + mtd->oobsize); /* For dev->zero_oob */

	dev->buffers = malloc(alloc_size);
	if (dev->buffers == NULL) {
		printf("qpic_nand: failed to allocate memory\n");
		ret = -ENOMEM;
		goto err_buf;
	}
	buf = dev->buffers;

	dev->pad_dat = buf;
	buf += mtd->writesize;
	dev->pad_oob = buf;
	buf += mtd->oobsize;

	dev->zero_page = buf;
	buf += mtd->writesize;

	dev->zero_oob = buf;
	buf += mtd->oobsize;

	memset(dev->zero_page, 0x0, mtd->writesize);
	memset(dev->zero_oob, 0x0, mtd->oobsize);

	/* Register with MTD subsystem. */
	ret = nand_register(CONFIG_QPIC_NAND_NAND_INFO_IDX);
	if (ret < 0) {
		printf("qpic_nand: failed to register with MTD subsystem\n");
		ret = NANDC_RESULT_FAILURE;
		goto err_reg;
	}
	if (ret == 0)
		return ret;
err_reg:
	free(dev->buffers);
err_buf:
	return ret;
}
