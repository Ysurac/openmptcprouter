/*
 * Copyright (c) 2012 - 2013 The Linux Foundation. All rights reserved.
 */

#include <common.h>
#include <nand.h>
#include <malloc.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/ipq_nand.h>

#include <asm/io.h>
#include <asm/errno.h>
#include <asm/arch-ipq806x/nand.h>

/*
 * MTD, NAND and the IPQ806x NAND controller uses various terms to
 * refer to the various different types of locations in a
 * codeword/page. The nomenclature used for variables, is explained
 * below.
 *
 * cw/codeword - used to refer to a chunk of bytes that will be read
 * or written at a time by the controller. The exact size depends on
 * the ECC mode. For 4-bit, it is 528 bytes. For 8-bit, it is 532
 * bytes.
 *
 * main - used to refer to locations that are covered by the ECC
 * engine, for ECC calculation. Appears before the ECC bytes in a
 * codeword.
 *
 * spare - used to refer to locations that are not covered by the ECC
 * engine, for ECC calculation. Appears after the ECC bytes in a
 * codeword.
 *
 * oob - used to refer to locations where filesystem metainfo will be
 * stored, this is inline with the MTD convention.
 *
 * data - used to refer to locations where file's contents will be
 * stored, this is inline with the MTD convention.
 */

enum ecc_mode {
	ECC_REQ_4BIT = 4,
	ECC_REQ_8BIT = 8
};

struct ipq_cw_layout {
	u_int data_offs;
	u_int data_size;
	u_int oob_offs;
	u_int oob_size;
};

/**
 * struct ipq_config - IPQ806x specific device config. info
 * @page_size:		page size, used for matching
 * @ecc_mode:		ECC mode, used for matching
 * @main_per_cw:        no. of bytes in the codeword that will be ECCed
 * @spare_per_cw:	no. of bytes in the codeword that will NOT be ECCed
 * @ecc_per_cw:	        no. of ECC bytes that will be generated
 * @bb_byte:            offset of the bad block marker within the codeword
 * @bb_in_spare:	is the bad block marker in spare area?
 * @cw_per_page:        the no. of codewords in a page
 * @ecc_page_layout:    the mapping of data and oob buf in AUTO mode
 * @raw_page_layout:    the mapping of data and oob buf in RAW mode
 */
struct ipq_config {
	u_int page_size;
	enum ecc_mode ecc_mode;

	u_int main_per_cw;
	u_int spare_per_cw;
	u_int ecc_per_cw;
	u_int bb_byte;
	u_int bb_in_spare;

	u_int cw_per_page;
	struct ipq_cw_layout *ecc_page_layout;
	struct ipq_cw_layout *raw_page_layout;
};

/**
 * struct ipq_nand_dev - driver state information
 * @main_per_cw:        no. of bytes in the codeword that will be ECCed
 * @spare_per_cw:	no. of bytes in the codeword that will NOT be ECCed
 * @cw_per_page:        the no. of codewords in a page
 * @ecc_page_layout:    the mapping of data and oob buf in AUTO mode
 * @raw_page_layout:    the mapping of data and oob buf in RAW mode
 * @curr_page_layout:   currently selected page layout ECC or raw
 * @dev_cfg0:           the value for DEVn_CFG0 register
 * @dev_cfg1:           the value for DEVn_CFG1 register
 * @dev_ecc_cfg:        the value for DEVn_ECC_CFG register
 * @dev_cfg0_raw:       the value for DEVn_CFG0 register, in raw mode
 * @dev_cfg1_raw:       the value for DEVn_CFG1 register, in raw mode
 * @buffers:            pointer to dynamically allocated buffers
 * @pad_dat:            the pad buffer for in-band data
 * @pad_oob:            the pad buffer for out-of-band data
 * @zero_page:          the zero page written for marking bad blocks
 * @zero_oob:           the zero OOB written for marking bad blocks
 * @read_cmd:           the controller cmd to do a read
 * @write_cmd:          the controller cmd to do a write
 * @oob_per_page:       the no. of OOB bytes per page, depends on OOB mode
 */
struct ipq_nand_dev {
	struct ebi2nd_regs *regs;

	u_int main_per_cw;
	u_int spare_per_cw;

	u_int cw_per_page;
	struct ipq_cw_layout *ecc_page_layout;
	struct ipq_cw_layout *raw_page_layout;
	struct ipq_cw_layout *curr_page_layout;

	uint32_t dev_cfg0;
	uint32_t dev_cfg1;
	uint32_t dev_ecc_cfg;

	uint32_t dev_cfg0_raw;
	uint32_t dev_cfg1_raw;

	u_char *buffers;
	u_char *pad_dat;
	u_char *pad_oob;
	u_char *zero_page;
	u_char *zero_oob;

	uint32_t read_cmd;
	uint32_t write_cmd;
	u_int oob_per_page;
};

#define MTD_NAND_CHIP(mtd) ((struct nand_chip *)((mtd)->priv))
#define MTD_IPQ_NAND_DEV(mtd) (MTD_NAND_CHIP(mtd)->priv)
#define MTD_ONFI_PARAMS(mtd) (&(MTD_NAND_CHIP(mtd)->onfi_params))
static struct ipq_nand_dev ipq_nand_dev;

static struct ipq_cw_layout ipq_sbl_page_layout_4ecc_2k[] = {
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
};

static struct ipq_cw_layout ipq_linux_page_layout_4ecc_2k[] = {
	{ 0, 516,   0,  0 },
	{ 0, 516,   0,  0 },
	{ 0, 516,   0,  0 },
	{ 0, 500, 500, 16 },
};

static struct ipq_cw_layout ipq_raw_page_layout_4ecc_2k[] = {
	{ 0, 528,   0,  0 },
	{ 0, 528,   0,  0 },
	{ 0, 528,   0,  0 },
	{ 0, 464, 464, 64 },
};

static struct ipq_config ipq_sbl_config_4ecc_2k = {
	.page_size = 2048,
	.ecc_mode = ECC_REQ_4BIT,

	.main_per_cw = 512,
	.ecc_per_cw = 10,
	.spare_per_cw = 5,
	.bb_in_spare = 0,
	.bb_byte = 465,

	.cw_per_page = 4,
	.ecc_page_layout = ipq_sbl_page_layout_4ecc_2k,
	.raw_page_layout = ipq_raw_page_layout_4ecc_2k
};

static struct ipq_config ipq_linux_config_4ecc_2k = {
	.page_size = 2048,
	.ecc_mode = ECC_REQ_4BIT,

	.main_per_cw = 516,
	.ecc_per_cw = 10,
	.spare_per_cw = 1,
	.bb_in_spare = 0,
	.bb_byte = 465,

	.cw_per_page = 4,
	.ecc_page_layout = ipq_linux_page_layout_4ecc_2k,
	.raw_page_layout = ipq_raw_page_layout_4ecc_2k
};

static struct ipq_cw_layout ipq_sbl_page_layout_8ecc_2k[] = {
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
};

static struct ipq_cw_layout ipq_linux_page_layout_8ecc_2k[] = {
	{ 0, 516,   0,  0 },
	{ 0, 516,   0,  0 },
	{ 0, 516,   0,  0 },
	{ 0, 500, 500, 16 },
};

static struct ipq_cw_layout ipq_raw_page_layout_8ecc_2k[] = {
	{ 0, 532,   0,  0 },
	{ 0, 532,   0,  0 },
	{ 0, 532,   0,  0 },
	{ 0, 452, 452, 80 },
};

static struct ipq_config ipq_sbl_config_8ecc_2k = {
	.page_size = 2048,
	.ecc_mode = ECC_REQ_8BIT,

	.main_per_cw = 512,
	.ecc_per_cw = 13,
	.spare_per_cw = 6,
	.bb_in_spare = 0,
	.bb_byte = 453,

	.cw_per_page = 4,
	.ecc_page_layout = ipq_sbl_page_layout_8ecc_2k,
	.raw_page_layout = ipq_raw_page_layout_8ecc_2k
};

static struct ipq_config ipq_linux_config_8ecc_2k = {
	.page_size = 2048,
	.ecc_mode = ECC_REQ_8BIT,

	.main_per_cw = 516,
	.ecc_per_cw = 13,
	.spare_per_cw = 2,
	.bb_in_spare = 0,
	.bb_byte = 453,

	.cw_per_page = 4,
	.ecc_page_layout = ipq_linux_page_layout_8ecc_2k,
	.raw_page_layout = ipq_raw_page_layout_8ecc_2k
};

static struct ipq_cw_layout ipq_sbl_page_layout_4ecc_4k[] = {
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
};

static struct ipq_cw_layout ipq_linux_page_layout_4ecc_4k[] = {
	{ 0, 516,   0,  0 },
	{ 0, 516,   0,  0 },
	{ 0, 516,   0,  0 },
	{ 0, 516,   0,  0 },
	{ 0, 516,   0,  0 },
	{ 0, 516,   0,  0 },
	{ 0, 516,   0,  0 },
	{ 0, 484, 484, 32 },
};

static struct ipq_cw_layout ipq_raw_page_layout_4ecc_4k[] = {
	{ 0, 528,   0,   0 },
	{ 0, 528,   0,   0 },
	{ 0, 528,   0,   0 },
	{ 0, 528,   0,   0 },
	{ 0, 528,   0,   0 },
	{ 0, 528,   0,   0 },
	{ 0, 528,   0,   0 },
	{ 0, 400, 400, 128 },
};

static struct ipq_config ipq_sbl_config_4ecc_4k = {
	.page_size = 4096,
	.ecc_mode = ECC_REQ_4BIT,

	.main_per_cw = 512,
	.ecc_per_cw = 10,
	.spare_per_cw = 5,
	.bb_in_spare = 0,
	.bb_byte = 401,

	.cw_per_page = 8,
	.ecc_page_layout = ipq_sbl_page_layout_4ecc_4k,
	.raw_page_layout = ipq_raw_page_layout_4ecc_4k
};

static struct ipq_config ipq_linux_config_4ecc_4k = {
	.page_size = 4096,
	.ecc_mode = ECC_REQ_4BIT,

	.main_per_cw = 516,
	.ecc_per_cw = 10,
	.spare_per_cw = 1,
	.bb_in_spare = 0,
	.bb_byte = 401,

	.cw_per_page = 8,
	.ecc_page_layout = ipq_linux_page_layout_4ecc_4k,
	.raw_page_layout = ipq_raw_page_layout_4ecc_4k
};

static struct ipq_cw_layout ipq_sbl_page_layout_8ecc_4k[] = {
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
	{ 0, 512, 0, 0 },
};

static struct ipq_cw_layout ipq_linux_page_layout_8ecc_4k[] = {
	{ 0, 516,   0,  0 },
	{ 0, 516,   0,  0 },
	{ 0, 516,   0,  0 },
	{ 0, 516,   0,  0 },
	{ 0, 516,   0,  0 },
	{ 0, 516,   0,  0 },
	{ 0, 516,   0,  0 },
	{ 0, 484, 484, 32 },
};

static struct ipq_cw_layout ipq_raw_page_layout_8ecc_4k[] = {
	{ 0, 532,   0,   0 },
	{ 0, 532,   0,   0 },
	{ 0, 532,   0,   0 },
	{ 0, 532,   0,   0 },
	{ 0, 532,   0,   0 },
	{ 0, 532,   0,   0 },
	{ 0, 532,   0,   0 },
	{ 0, 372, 372, 160 },
};

static struct ipq_config ipq_sbl_config_8ecc_4k = {
	.page_size = 4096,
	.ecc_mode = ECC_REQ_8BIT,

	.main_per_cw = 512,
	.ecc_per_cw = 13,
	.spare_per_cw = 6,
	.bb_in_spare = 0,
	.bb_byte = 373,

	.cw_per_page = 8,
	.ecc_page_layout = ipq_sbl_page_layout_8ecc_4k,
	.raw_page_layout = ipq_raw_page_layout_8ecc_4k
};

static struct ipq_config ipq_linux_config_8ecc_4k = {
	.page_size = 4096,
	.ecc_mode = ECC_REQ_8BIT,

	.main_per_cw = 516,
	.ecc_per_cw = 13,
	.spare_per_cw = 2,
	.bb_in_spare = 0,
	.bb_byte = 373,

	.cw_per_page = 8,
	.ecc_page_layout = ipq_linux_page_layout_8ecc_4k,
	.raw_page_layout = ipq_raw_page_layout_8ecc_4k
};

#define IPQ_CONFIGS_MAX 4

/*
 * List of supported configs. The code expects this list to be sorted
 * on ECC requirement size. So 4-bit first, 8-bit next and so on.
 */
static struct ipq_config *ipq_configs[IPQ_NAND_LAYOUT_MAX][IPQ_CONFIGS_MAX] = {
	{
		&ipq_sbl_config_4ecc_2k,
		&ipq_sbl_config_8ecc_2k,
		&ipq_sbl_config_4ecc_4k,
		&ipq_sbl_config_8ecc_4k,
	},
	{
		&ipq_linux_config_4ecc_2k,
		&ipq_linux_config_8ecc_2k,
		&ipq_linux_config_4ecc_4k,
		&ipq_linux_config_8ecc_4k,
	}
};

struct nand_ecclayout fake_ecc_layout;

/*
 * Convenient macros for the flash_cmd register, commands.
 */
#define PAGE_CMD                (LAST_PAGE(1) | PAGE_ACC(1))

#define IPQ_CMD_ABORT           (OP_CMD_ABORT_TRANSACTION)
#define IPQ_CMD_PAGE_READ       (OP_CMD_PAGE_READ | PAGE_CMD)
#define IPQ_CMD_PAGE_READ_ECC   (OP_CMD_PAGE_READ_WITH_ECC | PAGE_CMD)
#define IPQ_CMD_PAGE_READ_ALL   (OP_CMD_PAGE_READ_WITH_ECC_SPARE | PAGE_CMD)
#define IPQ_CMD_PAGE_PROG       (OP_CMD_PROGRAM_PAGE | PAGE_CMD)
#define IPQ_CMD_PAGE_PROG_ECC   (OP_CMD_PROGRAM_PAGE_WITH_ECC | PAGE_CMD)
#define IPQ_CMD_PAGE_PROG_ALL   (OP_CMD_PROGRAM_PAGE_WITH_SPARE | PAGE_CMD)
#define IPQ_CMD_BLOCK_ERASE     (OP_CMD_BLOCK_ERASE | PAGE_CMD)
#define IPQ_CMD_FETCH_ID        (OP_CMD_FETCH_ID)
#define IPQ_CMD_CHECK_STATUS    (OP_CMD_CHECK_STATUS)
#define IPQ_CMD_RESET_DEVICE    (OP_CMD_RESET_NAND_FLASH_DEVICE)

/*
 * Extract row bytes from a page no.
 */
#define PAGENO_ROW0(pgno)       ((pgno) & 0xFF)
#define PAGENO_ROW1(pgno)       (((pgno) >> 8) & 0xFF)
#define PAGENO_ROW2(pgno)       (((pgno) >> 16) & 0xFF)

/*
 * ADDR0 and ADDR1 register field macros, for generating address
 * cycles during page read and write accesses.
 */
#define ADDR_CYC_ROW0(row0)     ((row0) << 16)
#define ADDR_CYC_ROW1(row1)     ((row1) << 24)
#define ADDR_CYC_ROW2(row2)     ((row2) << 0)

#define NAND_READY_TIMEOUT      100000 /* 1 SEC */

/*
 * The flash buffer does not like byte accesses. A plain memcpy might
 * perform byte access, which can clobber the data to the
 * controller. Hence we implement our custom versions to write to and
 * read from the flash buffer.
 */

/*
 * Copy from memory to flash buffer.
 */
static void mem2hwcpy(void *dest, const void *src, size_t n)
{
	size_t i;
	uint32_t *src32 = (uint32_t *)src;
	uint32_t *dest32 = (uint32_t *)dest;
	size_t words = (n + sizeof(uint32_t) - 1) / sizeof(uint32_t);

	for (i = 0; i < words; i++)
		writel(src32[i], &dest32[i]);
}

/*
 * Copy from flash buffer to memory.
 */
static void hw2memcpy(void *dest, const void *src, size_t n)
{
	size_t i;
	uint32_t *src32 = (uint32_t *)src;
	uint32_t *dest32 = (uint32_t *)dest;
	size_t words = (n + sizeof(uint32_t) - 1) / sizeof(uint32_t);

	for (i = 0; i < words; i++)
		dest32[i] = readl(&src32[i]);
}

/*
 * Set the no. of codewords to read/write in the codeword counter.
 */
static void ipq_init_cw_count(struct mtd_info *mtd, u_int count)
{
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
	struct ebi2nd_regs *regs = dev->regs;

	clrsetbits_le32(&regs->dev0_cfg0, CW_PER_PAGE_MASK, CW_PER_PAGE(count));
}

/*
 * Set the row values for the address cycles, generated during the
 * read and write transactions.
 */
static void ipq_init_rw_pageno(struct mtd_info *mtd, u_long pageno)
{
	u_char row0;
	u_char row1;
	u_char row2;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
	struct ebi2nd_regs *regs = dev->regs;

	row0 = PAGENO_ROW0(pageno);
	row1 = PAGENO_ROW1(pageno);
	row2 = PAGENO_ROW2(pageno);

	writel(ADDR_CYC_ROW0(row0) | ADDR_CYC_ROW1(row1), &regs->addr0);
	writel(ADDR_CYC_ROW2(row2), &regs->addr1);
}

/*
 * Initialize the erased page detector function, in the
 * controller. This is done to prevent ECC error detection and
 * correction for erased pages, where the ECC bytes does not match
 * with the page data.
 */
static void ipq_init_erased_page_detector(struct mtd_info *mtd)
{
	uint32_t reset;
	uint32_t enable;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
	struct ebi2nd_regs *regs = dev->regs;

	reset = ERASED_CW_ECC_MASK(1) | AUTO_DETECT_RES(1);
	enable = ERASED_CW_ECC_MASK(1) | AUTO_DETECT_RES(0);

	writel(reset, &regs->erased_cw_detect_cfg);
	writel(enable, &regs->erased_cw_detect_cfg);
}

/*
 * Configure the controller, and internal driver state for non-ECC
 * mode operation.
 */
static void ipq_enter_raw_mode(struct mtd_info *mtd)
{
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
	struct ebi2nd_regs *regs = dev->regs;

	writel(dev->dev_cfg0_raw, &regs->dev0_cfg0);
	writel(dev->dev_cfg1_raw, &regs->dev0_cfg1);

	dev->read_cmd = IPQ_CMD_PAGE_READ;
	dev->write_cmd = IPQ_CMD_PAGE_PROG;
	dev->curr_page_layout = dev->raw_page_layout;
	dev->oob_per_page = mtd->oobsize;
}

/*
 * Configure the controller, and internal driver state for ECC mode
 * operation.
 */
static void ipq_exit_raw_mode(struct mtd_info *mtd)
{
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
	struct ebi2nd_regs *regs = dev->regs;

	writel(dev->dev_cfg0, &regs->dev0_cfg0);
	writel(dev->dev_cfg1, &regs->dev0_cfg1);
	writel(dev->dev_ecc_cfg, &regs->dev0_ecc_cfg);

	dev->read_cmd = IPQ_CMD_PAGE_READ_ALL;
	dev->write_cmd = IPQ_CMD_PAGE_PROG_ALL;
	dev->curr_page_layout = dev->ecc_page_layout;
	dev->oob_per_page = mtd->oobavail;
}

/*
 * Wait for the controller/flash to complete operation.
 */
static int ipq_wait_ready(struct mtd_info *mtd, uint32_t *status)
{
	u_long count = 0;
	uint32_t op_status;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
	struct ebi2nd_regs *regs = dev->regs;

	while (count < NAND_READY_TIMEOUT) {
		*status = readl(&regs->flash_status);
		op_status = *status & OPER_STATUS_MASK;
		if (op_status == OPER_STATUS_IDLE_STATE)
			break;

		udelay(10);
		count++;
	}

	writel(READY_BSY_N_EXTERNAL_FLASH_IS_READY, &regs->flash_status);

	if (count >= NAND_READY_TIMEOUT)
		return -ETIMEDOUT;

	return 0;
}

/*
 * Execute and wait for a command to complete.
 */
static int ipq_exec_cmd(struct mtd_info *mtd, uint32_t cmd, uint32_t *status)
{
	int ret;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
	struct ebi2nd_regs *regs = dev->regs;

	writel(cmd, &regs->flash_cmd);
	writel(EXEC_CMD(1), &regs->exec_cmd);

	ret = ipq_wait_ready(mtd, status);
	if (ret < 0)
		return ret;

	return 0;
}

/*
 * Check if error flags related to read operation have been set in the
 * status register.
 */
static int ipq_check_read_status(struct mtd_info *mtd, uint32_t status)
{
	uint32_t cw_erased = 0;
	uint32_t num_errors;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
	struct ebi2nd_regs *regs = dev->regs;

	debug("Read Status: %08x\n", status);

	/* Hardware handles erased page detection for BCH */
	if (dev->dev_cfg1 & ENABLE_BCH_ECC(1)) {
		cw_erased = readl(&regs->erased_cw_detect_status);
		cw_erased &= CODEWORD_ERASED_MASK;
	}

	num_errors = readl(&regs->buffer_status);
	num_errors &= NUM_ERRORS_MASK;

	if (status & MPU_ERROR_MASK)
		return -EPERM;

	if ((status & OP_ERR_MASK) && !cw_erased) {
		mtd->ecc_stats.failed++;
		return -EBADMSG;
	}

	if (num_errors)
		mtd->ecc_stats.corrected++;

	return 0;
}

/* On reading an erased page,value 0x54 will appear on the offsets 3
 * and 175 in a page in case of linux layout and value 0x76 will appear
 * in the offset 243 in case of SBL. So parse the entire page and
 * put 0xFFs on those offsets so that it appears all 0xffs */
static int ipq_nand_handle_erased_pg(struct mtd_info *mtd,
	struct mtd_oob_ops *ops, struct ipq_cw_layout *cwl, int ret)
{
	int i;
	uint8_t tmp[3];
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);

	if (ret != -EBADMSG)
		return ret;

	/* Hardware handles erased page detection for BCH */
	if (dev->dev_cfg1 & ENABLE_BCH_ECC(1))
		return ret;

	tmp[0] = ops->datbuf[3];
	tmp[1] = ops->datbuf[175];
	tmp[2] = ops->datbuf[243];

	switch (cwl->data_size) {
	case 516:
	case 500:
		if (!((tmp[0] == 0x54 && tmp[1] == 0xff) ||
		      (tmp[0] == 0xff && tmp[1] == 0x54)))
			return ret;
		break;
	case 512:
		if (tmp[2] != 0x76)
			return ret;
		break;
	default:
		return ret;
	}

	ops->datbuf[3] = ops->datbuf[175] = ops->datbuf[243] = 0xff;

	/* Check if codeword is full of 0xff */
	for (i = 0; (i < cwl->data_size) && (ops->datbuf[i] == 0xff); i++);

	if (i < cwl->data_size) {
		ops->datbuf[3] = tmp[0];
		ops->datbuf[175] = tmp[1];
		ops->datbuf[243] = tmp[2];
		return ret;
	}

	mtd->ecc_stats.failed--;

	return 0;
}

/*
 * Read a codeword into the data and oob buffers, at offsets specified
 * by the codeword layout.
 */
static int ipq_read_cw(struct mtd_info *mtd, u_int cwno,
		       struct mtd_oob_ops *ops)
{
	int ret;
	uint32_t status;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
	struct ebi2nd_regs *regs = dev->regs;
	struct ipq_cw_layout *cwl = &dev->curr_page_layout[cwno];

	ret = ipq_exec_cmd(mtd, dev->read_cmd, &status);
	if (ret < 0)
		return ret;

	ret = ipq_check_read_status(mtd, status);

	if (ops->datbuf != NULL) {
		hw2memcpy(ops->datbuf, &regs->buffn_acc[cwl->data_offs >> 2],
			  cwl->data_size);

		ret = ipq_nand_handle_erased_pg(mtd, ops, cwl, ret);
		if (ret < 0)
			return ret;

		ops->retlen += cwl->data_size;
		ops->datbuf += cwl->data_size;
	}

	if (ops->oobbuf != NULL) {
		if (ret < 0)
			return ret;
		hw2memcpy(ops->oobbuf, &regs->buffn_acc[cwl->oob_offs >> 2],
			  cwl->oob_size);

		ops->oobretlen += cwl->oob_size;
		ops->oobbuf += cwl->oob_size;
	}

	return 0;
}

/*
 * Read and discard codewords, to bring the codeword counter back to
 * zero.
 */
static void ipq_reset_cw_counter(struct mtd_info *mtd, u_int start_cw)
{
	u_int i;
	uint32_t status;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);

	for (i = start_cw; i < dev->cw_per_page; i++)
		ipq_exec_cmd(mtd, dev->read_cmd, &status);
}

/*
 * Read a page worth of data and oob.
 */
static int ipq_read_page(struct mtd_info *mtd, u_long pageno,
			 struct mtd_oob_ops *ops)
{
	u_int i;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
	int ret = 0;

	ipq_init_cw_count(mtd, dev->cw_per_page - 1);
	ipq_init_rw_pageno(mtd, pageno);
	ipq_init_erased_page_detector(mtd);

	for (i = 0; i < dev->cw_per_page; i++) {
		ret = ipq_read_cw(mtd, i, ops);
		if (ret < 0) {
			ipq_reset_cw_counter(mtd, i + 1);
			return ret;
		}
	}

	return ret;
}

/*
 * Estimate the no. of pages to read, based on the data length and oob
 * length.
 */
static u_long ipq_get_read_page_count(struct mtd_info *mtd,
				      struct mtd_oob_ops *ops)
{
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
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
static uint8_t *ipq_nand_read_oobbuf(struct mtd_info *mtd, struct mtd_oob_ops *ops)
{
	size_t read_ooblen;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);

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
static uint8_t *ipq_nand_read_datbuf(struct mtd_info *mtd, struct mtd_oob_ops *ops)
{
	size_t read_datlen;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);

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
static void ipq_nand_read_oobcopy(struct mtd_info *mtd, struct mtd_oob_ops *ops)
{
	size_t ooblen;
	size_t read_ooblen;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);

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
static void ipq_nand_read_datcopy(struct mtd_info *mtd, struct mtd_oob_ops *ops)
{
	size_t datlen;
	size_t read_datlen;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);

	if (ops->datbuf == NULL)
		return;

	read_datlen = ops->len - ops->retlen;
	datlen = MIN(read_datlen, mtd->writesize);

	if (read_datlen < mtd->writesize)
		memcpy(ops->datbuf + ops->retlen, dev->pad_dat, datlen);

	ops->retlen += datlen;
}

static int ipq_nand_read_oob(struct mtd_info *mtd, loff_t from,
			     struct mtd_oob_ops *ops)
{
	u_long start;
	u_long pages;
	u_long i;
	uint32_t corrected;
	struct nand_chip *chip = MTD_NAND_CHIP(mtd);
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
	int ret = 0;

	/* We don't support MTD_OOB_PLACE as of yet. */
	if (ops->mode == MTD_OOB_PLACE)
		return -ENOSYS;

	/* Check for reads past end of device */
	if (ops->datbuf && (from + ops->len) > mtd->size)
		return -EINVAL;

	if (from & (mtd->writesize - 1))
		return -EINVAL;

	if (ops->ooboffs != 0)
		return -EINVAL;

	if (ops->mode == MTD_OOB_RAW)
		ipq_enter_raw_mode(mtd);

	start = from >> chip->page_shift;
	pages = ipq_get_read_page_count(mtd, ops);

	debug("Start of page: %lu\n", start);
	debug("No of pages to read: %lu\n", pages);

	corrected = mtd->ecc_stats.corrected;

	for (i = start; i < (start + pages); i++) {
		struct mtd_oob_ops page_ops;

		page_ops.mode = ops->mode;
		page_ops.len = mtd->writesize;
		page_ops.ooblen = dev->oob_per_page;
		page_ops.datbuf = ipq_nand_read_datbuf(mtd, ops);
		page_ops.oobbuf = ipq_nand_read_oobbuf(mtd, ops);
		page_ops.retlen = 0;
		page_ops.oobretlen = 0;

		ret = ipq_read_page(mtd, i, &page_ops);
		if (ret < 0)
			goto done;

		ipq_nand_read_datcopy(mtd, ops);
		ipq_nand_read_oobcopy(mtd, ops);
	}

	if (mtd->ecc_stats.corrected != corrected)
		ret = -EUCLEAN;

done:
	ipq_exit_raw_mode(mtd);
	return ret;
}

static int ipq_nand_read(struct mtd_info *mtd, loff_t from, size_t len,
			 size_t *retlen, u_char *buf)
{
	int ret;
	struct mtd_oob_ops ops;

	ops.mode = MTD_OOB_AUTO;
	ops.len = len;
	ops.retlen = 0;
	ops.ooblen = 0;
	ops.oobretlen = 0;
	ops.ooboffs = 0;
	ops.datbuf = (uint8_t *)buf;
	ops.oobbuf = NULL;

	ret = ipq_nand_read_oob(mtd, from, &ops);
	*retlen = ops.retlen;

	return ret;
}

/*
 * Check if error flags related to write/erase operation have been set
 * in the status register.
 */
static int ipq_check_write_erase_status(uint32_t status)
{
	debug("Write Status: %08x\n", status);

	if (status & MPU_ERROR_MASK)
		return -EPERM;

	else if (status & OP_ERR_MASK)
		return -EIO;

	else if (!(status & PROG_ERASE_OP_RESULT_MASK))
		return -EIO;

	else
		return 0;
}

/*
 * Write a codeword with the specified data and oob.
 */
static int ipq_write_cw(struct mtd_info *mtd, u_int cwno,
			struct mtd_oob_ops *ops)
{
	int ret;
	uint32_t status;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
	struct ebi2nd_regs *regs = dev->regs;
	struct ipq_cw_layout *cwl = &(dev->curr_page_layout[cwno]);

	mem2hwcpy(&regs->buffn_acc[cwl->data_offs >> 2],
		  ops->datbuf, cwl->data_size);

	mem2hwcpy(&regs->buffn_acc[cwl->oob_offs >> 2],
		  ops->oobbuf, cwl->oob_size);

	ret = ipq_exec_cmd(mtd, dev->write_cmd, &status);
	if (ret < 0)
		return ret;

	ret = ipq_check_write_erase_status(status);
	if (ret < 0)
		return ret;

	ops->retlen += cwl->data_size;
	ops->datbuf += cwl->data_size;

	if (ops->oobbuf != NULL) {
		ops->oobretlen += cwl->oob_size;
		ops->oobbuf += cwl->oob_size;
	}

	return 0;
}

/*
 * Write a page worth of data and oob.
 */
static int ipq_write_page(struct mtd_info *mtd, u_long pageno,
			  struct mtd_oob_ops *ops)
{
	u_int i;
	int ret;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);

	ipq_init_cw_count(mtd, dev->cw_per_page - 1);
	ipq_init_rw_pageno(mtd, pageno);

	for (i = 0; i < dev->cw_per_page; i++) {
		ret = ipq_write_cw(mtd, i, ops);
		if (ret < 0) {
			ipq_reset_cw_counter(mtd, i + 1);
			return ret;
		}
	}

	return 0;
}

/*
 * Return the buffer containing the in-band data to be written.
 */
static uint8_t *ipq_nand_write_datbuf(struct mtd_info *mtd, struct mtd_oob_ops *ops)
{
	return ops->datbuf + ops->retlen;
}

/*
 * Return the buffer containing the OOB data to be written. If user
 * buffer does not provide on page worth of OOB data, return a padded
 * internal buffer with the OOB data copied in.
 */
static uint8_t *ipq_nand_write_oobbuf(struct mtd_info *mtd, struct mtd_oob_ops *ops)
{
	size_t write_ooblen;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);

	if (ops->oobbuf == NULL)
		return dev->pad_oob;

	write_ooblen = ops->ooblen - ops->oobretlen;
	memset(dev->pad_oob, dev->oob_per_page, 0xFF);

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
static void ipq_nand_write_datinc(struct mtd_info *mtd, struct mtd_oob_ops *ops)
{
	ops->retlen += mtd->writesize;
}

/*
 * Increment the counters to indicate the no. of OOB bytes written.
 */
static void ipq_nand_write_oobinc(struct mtd_info *mtd, struct mtd_oob_ops *ops)
{
	size_t write_ooblen;
	size_t ooblen;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);

	if (ops->oobbuf == NULL)
		return;

	write_ooblen = ops->ooblen - ops->oobretlen;
	ooblen = MIN(write_ooblen, dev->oob_per_page);

	ops->oobretlen += ooblen;
}

static int ipq_nand_write_oob(struct mtd_info *mtd, loff_t to,
			      struct mtd_oob_ops *ops)
{
	u_long i;
	u_long start;
	u_long pages;
	struct nand_chip *chip = MTD_NAND_CHIP(mtd);
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
	int ret = 0;

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

	if (ops->mode == MTD_OOB_RAW)
		ipq_enter_raw_mode(mtd);

	start = to >> chip->page_shift;
	pages = ops->len >> chip->page_shift;
	ops->retlen = 0;
	ops->oobretlen = 0;

	for (i = start; i < (start + pages); i++) {
		struct mtd_oob_ops page_ops;

		page_ops.mode = ops->mode;
		page_ops.len = mtd->writesize;
		page_ops.ooblen = dev->oob_per_page;
		page_ops.datbuf = ipq_nand_write_datbuf(mtd, ops);
		page_ops.oobbuf = ipq_nand_write_oobbuf(mtd, ops);
		page_ops.retlen = 0;
		page_ops.oobretlen = 0;

		ret = ipq_write_page(mtd, i, &page_ops);
		if (ret < 0)
			goto done;

		ipq_nand_write_datinc(mtd, ops);
		ipq_nand_write_oobinc(mtd, ops);
	}

done:
	ipq_exit_raw_mode(mtd);
	return ret;
}

static int ipq_nand_write(struct mtd_info *mtd, loff_t to, size_t len,
			  size_t *retlen, const u_char *buf)
{
	int ret;
	struct mtd_oob_ops ops;

	ops.mode = MTD_OOB_AUTO;
	ops.len = len;
	ops.retlen = 0;
	ops.ooblen = 0;
	ops.oobretlen = 0;
	ops.ooboffs = 0;
	ops.datbuf = (uint8_t *)buf;
	ops.oobbuf = NULL;

	ret = ipq_nand_write_oob(mtd, to, &ops);
	*retlen = ops.retlen;

	return ret;
}

static int ipq_nand_block_isbad(struct mtd_info *mtd, loff_t offs)
{
	int ret;
	uint8_t oobbuf;
	struct mtd_oob_ops ops;

	/* Check for invalid offset */
	if (offs > mtd->size)
		return -EINVAL;

	if (offs & (mtd->erasesize - 1))
		return -EINVAL;

	ops.mode = MTD_OOB_RAW;
	ops.len = 0;
	ops.retlen = 0;
	ops.ooblen = 1;
	ops.oobretlen = 0;
	ops.ooboffs = 0;
	ops.datbuf = NULL;
	ops.oobbuf = &oobbuf;

	ret = ipq_nand_read_oob(mtd, offs, &ops);
	if (ret < 0)
		return ret;

	return oobbuf != 0xFF;
}

static int ipq_nand_block_markbad(struct mtd_info *mtd, loff_t offs)
{
	int ret;
	struct mtd_oob_ops ops;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);

	/* Check for invalid offset */
	if (offs > mtd->size)
		return -EINVAL;

	if (offs & (mtd->erasesize - 1))
		return -EINVAL;

	ops.mode = MTD_OOB_RAW;
	ops.len = mtd->writesize;
	ops.retlen = 0;
	ops.ooblen = mtd->oobsize;
	ops.oobretlen = 0;
	ops.ooboffs = 0;
	ops.datbuf = dev->zero_page;
	ops.oobbuf = dev->zero_oob;

	ret = ipq_nand_write_oob(mtd, offs, &ops);

	if (!ret)
		mtd->ecc_stats.badblocks++;

	return ret;
}

/*
 * Erase the specified block.
 */
static int ipq_nand_erase_block(struct mtd_info *mtd, u_long blockno)
{
	loff_t offs;
	u_long pageno;
	uint32_t status;
	struct nand_chip *chip = MTD_NAND_CHIP(mtd);
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
	struct ebi2nd_regs *regs = dev->regs;
	int ret = 0;

	ipq_init_cw_count(mtd, 0);

	offs = blockno << chip->phys_erase_shift;
	pageno = offs >> chip->page_shift;
	writel(pageno, &regs->addr0);

	ret = ipq_exec_cmd(mtd, IPQ_CMD_BLOCK_ERASE, &status);
	if (ret < 0)
		return ret;

	return ipq_check_write_erase_status(status);
}

static int ipq_nand_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	u_long i;
	u_long blocks;
	u_long start;
	loff_t offs;
	struct nand_chip *chip = MTD_NAND_CHIP(mtd);
	int ret = 0;

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

		if (!instr->scrub && ipq_nand_block_isbad(mtd, offs)) {
			debug("ipq_nand: attempt to erase a bad block");
			return -EIO;
		}

		ret = ipq_nand_erase_block(mtd, i);
		if (ret < 0) {
			instr->fail_addr = offs;
			break;
		}
	}

	return ret;
}

static void ipq_nand_sync(struct mtd_info *mtd)
{
	/* Nop */
}

static int ipq_nand_scan_bbt_nop(struct mtd_info *mtd)
{
	return 0;
}

#define NAND_ID_MAN(id) ((id) & 0xFF)
#define NAND_ID_DEV(id) (((id) >> 8) & 0xFF)
#define NAND_ID_CFG(id) (((id) >> 24) & 0xFF)

#define NAND_CFG_PAGE_SIZE(id)   (((id) >> 0) & 0x3)
#define NAND_CFG_SPARE_SIZE(id)  (((id) >> 2) & 0x3)
#define NAND_CFG_BLOCK_SIZE(id)  (((id) >> 4) & 0x3)

#define CHUNK_SIZE        512

#define MB_TO_BYTES(mb)   (((uint64_t)(mb)) << 20)
#define KB_TO_BYTES(kb)   ((kb) << 10)

/* ONFI Signature */
#define ONFI_SIG          0x49464E4F
#define ONFI_READ_ID_ADDR 0x20

static int ipq_nand_onfi_probe(struct mtd_info *mtd, uint32_t *onfi_id)
{
	int ret;
	uint32_t status;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
	struct ebi2nd_regs *regs = dev->regs;

	writel(ONFI_READ_ID_ADDR, &regs->addr0);
	ret = ipq_exec_cmd(mtd, IPQ_CMD_FETCH_ID, &status);
	if (ret < 0)
		return ret;

	*onfi_id = readl(&regs->flash_read_id);

	return 0;
}

int ipq_nand_get_info_onfi(struct mtd_info *mtd)
{
	uint32_t status;
	int ret;
	uint32_t dev_cmd_vld_orig;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
	struct ebi2nd_regs *regs = dev->regs;
	struct nand_onfi_params *p = MTD_ONFI_PARAMS(mtd);

	ipq_enter_raw_mode(mtd);

	writel(0, &regs->addr0);
	writel(0, &regs->addr1);

	dev_cmd_vld_orig = readl(&regs->dev_cmd_vld);
	clrsetbits_le32(&regs->dev_cmd_vld, READ_START_VLD_MASK,
			READ_START_VLD(0));

	clrsetbits_le32(&regs->dev_cmd1, READ_ADDR_MASK,
			READ_ADDR(NAND_CMD_PARAM));

	clrsetbits_le32(&regs->dev0_cfg0, NUM_ADDR_CYCLES_MASK,
			NUM_ADDR_CYCLES(1));

	ipq_init_cw_count(mtd, 0);

	ret = ipq_exec_cmd(mtd, IPQ_CMD_PAGE_READ_ALL, &status);
	if (ret < 0)
		goto err_exit;

	ret = ipq_check_read_status(mtd, status);
	if (ret < 0)
		goto err_exit;

	hw2memcpy(p, &regs->buffn_acc[0], sizeof(struct nand_onfi_params));

	mtd->writesize = le32_to_cpu(p->byte_per_page);
	mtd->erasesize = le32_to_cpu(p->pages_per_block) * mtd->writesize;
	mtd->oobsize = le16_to_cpu(p->spare_bytes_per_page);
	mtd->size = (uint64_t)le32_to_cpu(p->blocks_per_lun) * (mtd->erasesize);

err_exit:
	/* Restoring the page read command in read command register */
	clrsetbits_le32(&regs->dev_cmd1, READ_ADDR_MASK,
			READ_ADDR(NAND_CMD_READ0));

	writel(dev_cmd_vld_orig, &regs->dev_cmd_vld);

	return 0;
}

/*
 * Read the ID from the flash device.
 */
static int ipq_nand_probe(struct mtd_info *mtd, uint32_t *id)
{
	int ret;
	uint32_t status;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
	struct ebi2nd_regs *regs = dev->regs;

	writel(0, &regs->addr0);

	ret = ipq_exec_cmd(mtd, IPQ_CMD_FETCH_ID, &status);
	if (ret < 0)
		return ret;

	*id = readl(&regs->flash_read_id);

	return 0;
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
 * Populate flash parameters from the look up table.
 */
static void nand_get_info_flash_dev(struct mtd_info *mtd,
				    const struct nand_flash_dev *flash_dev)
{
	mtd->writesize = flash_dev->pagesize;
	mtd->erasesize = flash_dev->erasesize;
	mtd->oobsize = flash_dev->pagesize >> 5;
}

/*
 * Populate flash parameters from the configuration byte.
 */
static void nand_get_info_cfg(struct mtd_info *mtd, uint8_t cfg_id)
{
	u_int cfg_page_size;
	u_int cfg_block_size;
	u_int cfg_spare_size;
	u_int chunks;
	u_int spare_per_chunk;

	/* writesize = 1KB * (2 ^ cfg_page_size) */
	cfg_page_size = NAND_CFG_PAGE_SIZE(cfg_id);
	mtd->writesize = 1024 << cfg_page_size;

	/* erasesize = 64KB * (2 ^ cfg_block_size) */
	cfg_block_size = NAND_CFG_BLOCK_SIZE(cfg_id);
	mtd->erasesize = (64 * 1024) << cfg_block_size;

	/* Spare per 512B = 8 * (2 ^ cfg_spare_size) */
	cfg_spare_size = NAND_CFG_SPARE_SIZE(cfg_id);
	chunks = mtd->writesize / CHUNK_SIZE;
	spare_per_chunk = 8 << cfg_spare_size;
	mtd->oobsize = spare_per_chunk * chunks;

	if ((mtd->oobsize > 64) && (mtd->writesize == 2048)) {
		printf("ipq_nand: Found a 2K page device with"
		       " %d oobsize - changing oobsize to 64"
		       " bytes.\n", mtd->oobsize);
		mtd->oobsize = 64;
	}
}

/*
 * Populate flash parameters for non-ONFI devices.
 */
static int nand_get_info(struct mtd_info *mtd, uint32_t flash_id)
{
	uint8_t man_id;
	uint8_t dev_id;
	uint8_t cfg_id;
	const struct nand_manufacturers *flash_man;
	const struct nand_flash_dev *flash_dev;

	man_id = NAND_ID_MAN(flash_id);
	dev_id = NAND_ID_DEV(flash_id);
	cfg_id = NAND_ID_CFG(flash_id);

	debug("Manufacturer ID: %x\n", man_id);
	debug("Device ID: %x\n", dev_id);
	debug("Config. Byte: %x\n", cfg_id);

	flash_man = flash_get_man(man_id);
	flash_dev = flash_get_dev(dev_id);

	if (flash_man == NULL || flash_dev == NULL) {
		printf("ipq_nand: unknown NAND device manufacturer: %x"
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
		nand_get_info_cfg(mtd, cfg_id);
	else
		nand_get_info_flash_dev(mtd, flash_dev);

	return 0;
}

/*
 * Read the device ID, and populate the MTD callbacks and the device
 * parameters.
 */
int ipq_nand_scan(struct mtd_info *mtd)
{
	int ret;
	uint32_t nand_id1 = 0;
	uint32_t nand_id2 = 0;
	uint32_t onfi_sig = 0;
	struct nand_chip *chip = MTD_NAND_CHIP(mtd);
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
        struct ebi2nd_regs *regs = dev->regs;

	ret = ipq_nand_onfi_probe(mtd, &onfi_sig);
	if (ret < 0)
		return ret;

	if (onfi_sig == ONFI_SIG) {
		ret = ipq_nand_get_info_onfi(mtd);
		if (ret < 0)
			return ret;

	} else {
		ret = ipq_nand_probe(mtd, &nand_id1);
		if (ret < 0)
			return ret;

		ret = ipq_nand_probe(mtd, &nand_id2);
		if (ret < 0)
			return ret;

		if (nand_id1 != nand_id2) {
			/*
			 * Bus-hold or other interface concerns can cause
			 * random data to appear. If the two results do not
			 * match, we are reading garbage.
			 */
			return -ENODEV;
		}

		ret = nand_get_info(mtd, nand_id1);
		if (ret < 0)
			return ret;
	}
	writel(0x04E00480, &regs->xfr_step1);
	writel(0x41F0419A, &regs->xfr_step2);
	writel(0x81E08180, &regs->xfr_step3);
	writel(0xD000C000, &regs->xfr_step4);
	writel(0xC000C000, &regs->xfr_step5);
	writel(0xC000C000, &regs->xfr_step6);
	writel(0xC000C000, &regs->xfr_step7);
	mtd->type = MTD_NANDFLASH;
	mtd->flags = MTD_CAP_NANDFLASH;

	mtd->erase = ipq_nand_erase;
	mtd->point = NULL;
	mtd->unpoint = NULL;
	mtd->read = ipq_nand_read;
	mtd->write = ipq_nand_write;
	mtd->read_oob = ipq_nand_read_oob;
	mtd->write_oob = ipq_nand_write_oob;
	mtd->lock = NULL;
	mtd->unlock = NULL;
	mtd->block_isbad = ipq_nand_block_isbad;
	mtd->block_markbad = ipq_nand_block_markbad;
	mtd->sync = ipq_nand_sync;

	mtd->ecclayout = NULL;

	chip->page_shift = ffs(mtd->writesize) - 1;
	chip->phys_erase_shift = ffs(mtd->erasesize) - 1;

	/* One of the NAND layer functions that the command layer
	 * tries to access directly.
	 */
	chip->scan_bbt = ipq_nand_scan_bbt_nop;

	return 0;
}

/*
 * Configure the hardware for the selected NAND device configuration.
 */
static void ipq_nand_hw_config(struct mtd_info *mtd, struct ipq_config *cfg)
{
	u_int i;
	u_int enable_bch;
	u_int raw_cw_size;
	uint32_t dev_cmd_vld;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
	struct ebi2nd_regs *regs = dev->regs;

	dev->main_per_cw = cfg->main_per_cw;
	dev->spare_per_cw = cfg->spare_per_cw;
	dev->cw_per_page = cfg->cw_per_page;
	dev->ecc_page_layout = cfg->ecc_page_layout;
	dev->raw_page_layout = cfg->raw_page_layout;
	raw_cw_size = cfg->main_per_cw + cfg->spare_per_cw + cfg->ecc_per_cw + 1;

	mtd->oobavail = 0;
	for (i = 0; i < dev->cw_per_page; i++) {
		struct ipq_cw_layout *cw_layout = &dev->ecc_page_layout[i];
		mtd->oobavail += cw_layout->oob_size;
	}

	/*
	 * We use Reed-Solom ECC engine for 4-bit ECC. And BCH ECC
	 * engine for 8-bit ECC. Thats the way SBL and Linux kernel
	 * does it.
	 */
	enable_bch = 0;
	if (cfg->ecc_mode == ECC_REQ_8BIT)
		enable_bch = 1;

	dev->dev_cfg0 = (BUSY_TIMEOUT_ERROR_SELECT_2_SEC
			 | DISABLE_STATUS_AFTER_WRITE(0)
			 | MSB_CW_PER_PAGE(0)
			 | CW_PER_PAGE(cfg->cw_per_page - 1)
			 | UD_SIZE_BYTES(cfg->main_per_cw)
			 | RS_ECC_PARITY_SIZE_BYTES(cfg->ecc_per_cw)
			 | SPARE_SIZE_BYTES(cfg->spare_per_cw)
			 | NUM_ADDR_CYCLES(5));

	dev->dev_cfg0_raw = (BUSY_TIMEOUT_ERROR_SELECT_2_SEC
			     | DISABLE_STATUS_AFTER_WRITE(0)
			     | MSB_CW_PER_PAGE(0)
			     | CW_PER_PAGE(cfg->cw_per_page - 1)
			     | UD_SIZE_BYTES(raw_cw_size)
			     | RS_ECC_PARITY_SIZE_BYTES(0)
			     | SPARE_SIZE_BYTES(0)
			     | NUM_ADDR_CYCLES(5));

	dev->dev_cfg1 = (ECC_DISABLE(0)
			 | WIDE_FLASH_8_BIT_DATA_BUS
			 | NAND_RECOVERY_CYCLES(3)
			 | CS_ACTIVE_BSY_ASSERT_CS_DURING_BUSY
			 | BAD_BLOCK_BYTE_NUM(cfg->bb_byte)
			 | BAD_BLOCK_IN_SPARE_AREA(cfg->bb_in_spare)
			 | WR_RD_BSY_GAP_6_CLOCK_CYCLES_GAP
			 | ECC_ENCODER_CGC_EN(0)
			 | ECC_DECODER_CGC_EN(0)
			 | DISABLE_ECC_RESET_AFTER_OPDONE(0)
			 | ENABLE_BCH_ECC(enable_bch)
			 | RS_ECC_MODE(0));

	dev->dev_cfg1_raw = (ECC_DISABLE(1)
			     | WIDE_FLASH_8_BIT_DATA_BUS
			     | NAND_RECOVERY_CYCLES(3)
			     | CS_ACTIVE_BSY_ASSERT_CS_DURING_BUSY
			     | BAD_BLOCK_BYTE_NUM(0x11)
			     | BAD_BLOCK_IN_SPARE_AREA(1)
			     | WR_RD_BSY_GAP_6_CLOCK_CYCLES_GAP
			     | ECC_ENCODER_CGC_EN(0)
			     | ECC_DECODER_CGC_EN(0)
			     | DISABLE_ECC_RESET_AFTER_OPDONE(0)
			     | ENABLE_BCH_ECC(0)
			     | RS_ECC_MODE(0));

	dev->dev_ecc_cfg = (BCH_ECC_DISABLE(0)
			    | ECC_SW_RESET(0)
			    | BCH_ECC_MODE_8_BIT_ECC_ERROR_DETECTION_CORRECTION
			    | BCH_ECC_PARITY_SIZE_BYTES(cfg->ecc_per_cw)
			    | ECC_NUM_DATA_BYTES(cfg->main_per_cw)
			    | ECC_FORCE_CLK_OPEN(1));

	dev->read_cmd = IPQ_CMD_PAGE_READ_ALL;
	dev->write_cmd = IPQ_CMD_PAGE_PROG_ALL;

	dev->curr_page_layout = dev->ecc_page_layout;
	dev->oob_per_page = mtd->oobavail;

	writel(dev->dev_cfg0, &regs->dev0_cfg0);
	writel(dev->dev_cfg1, &regs->dev0_cfg1);
	writel(dev->dev_ecc_cfg, &regs->dev0_ecc_cfg);
	writel(dev->main_per_cw - 1, &regs->ebi2_ecc_buf_cfg);

	dev_cmd_vld = (SEQ_READ_START_VLD(1) | ERASE_START_VLD(1)
		       | WRITE_START_VLD(1) | READ_START_VLD(1));
	writel(dev_cmd_vld, &regs->dev_cmd_vld);
}

/*
 * Setup the hardware and the driver state. Called after the scan and
 * is passed in the results of the scan.
 */
int ipq_nand_post_scan_init(struct mtd_info *mtd, enum ipq_nand_layout layout)
{
	u_int i;
	size_t alloc_size;
	struct ipq_config **configs;
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);
	struct nand_chip *chip = MTD_NAND_CHIP(mtd);
	struct nand_onfi_params *nand_onfi = MTD_ONFI_PARAMS(mtd);
	int ret = 0;
	u_char *buf;

	alloc_size = (mtd->writesize   /* For dev->pad_dat */
		      + mtd->oobsize   /* For dev->pad_oob */
		      + mtd->writesize /* For dev->zero_page */
		      + mtd->oobsize); /* For dev->zero_oob */

	dev->buffers = malloc(alloc_size);
	if (dev->buffers == NULL) {
		printf("ipq_nand: failed to allocate memory\n");
		return -ENOMEM;
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

	configs = ipq_configs[layout];

	for (i = 0; i < IPQ_CONFIGS_MAX; i++) {
		if ((configs[i]->page_size == mtd->writesize)
		    && (nand_onfi->ecc_bits <= configs[i]->ecc_mode))
			break;
	}

	debug("ECC bits = %d\n", nand_onfi->ecc_bits);

	if (i == IPQ_CONFIGS_MAX) {
		printf("ipq_nand: unsupported dev. configuration\n");
		ret = -ENOENT;
		goto err_exit;
	}

	ipq_nand_hw_config(mtd, configs[i]);

	/*
	 * Safe to use a single instance global variable,
	 * fake_ecc_layout, since we will be called only once for the
	 * lifetime of the driver. We can be called more than once,
	 * but the previous instance of the driver would have been
	 * deinit before the next one is inited.
	 */
	memset(&fake_ecc_layout, 0, sizeof(fake_ecc_layout));
	chip->ecc.layout = &fake_ecc_layout;

	debug("OOB Avail: %d\n", mtd->oobavail);
	debug("CFG0: 0x%04X\n", dev->dev_cfg0);
	debug("CFG1: 0x%04X\n", dev->dev_cfg1);
	debug("Raw CFG0: 0x%04X\n", dev->dev_cfg0_raw);
	debug("Raw CFG1: 0x%04X\n", dev->dev_cfg1_raw);
	debug("ECC : 0x%04X\n", dev->dev_ecc_cfg);

	goto exit;

err_exit:
	free(dev->buffers);

exit:
	return ret;
}

static struct nand_chip nand_chip[CONFIG_SYS_MAX_NAND_DEVICE];

/*
 * Initialize controller and register as an MTD device.
 */
int ipq_nand_init(enum ipq_nand_layout layout)
{
	uint32_t status;
	struct nand_chip *chip;
	int ret;
	struct mtd_info *mtd;

	mtd = &nand_info[CONFIG_IPQ_NAND_NAND_INFO_IDX];
	mtd->priv = &nand_chip[0];

	ipq_nand_dev.regs = (struct ebi2nd_regs *) EBI2ND_BASE;

	chip = mtd->priv;
	chip->priv = &ipq_nand_dev;

	/* Soft Reset */
	ret = ipq_exec_cmd(mtd, IPQ_CMD_ABORT, &status);
	if (ret < 0) {
		printf("ipq_nand: controller reset timedout\n");
		return ret;
	}

	/* Set some sane HW configuration, for ID read. */
	ipq_nand_hw_config(mtd, ipq_configs[layout][0]);

	/* Reset Flash Memory */
	ret = ipq_exec_cmd(mtd, IPQ_CMD_RESET_DEVICE, &status);
	if (ret < 0) {
		printf("ipq_nand: flash reset timedout\n");
		return ret;
	}

	/* Identify the NAND device. */
	ret = ipq_nand_scan(mtd);
	if (ret < 0) {
		printf("ipq_nand: failed to identify device\n");
		return ret;
	}

	ret = ipq_nand_post_scan_init(mtd, layout);
	if (ret < 0)
		return ret;

	/* Register with MTD subsystem. */
	ret = nand_register(CONFIG_IPQ_NAND_NAND_INFO_IDX);
	if (ret < 0) {
		printf("ipq_nand: failed to register with MTD subsystem\n");
		return ret;
	}

	return 0;
}

static int ipq_nand_deinit(void)
{
	int ret = 0;
	struct mtd_info *mtd = &nand_info[CONFIG_IPQ_NAND_NAND_INFO_IDX];
	struct ipq_nand_dev *dev = MTD_IPQ_NAND_DEV(mtd);

#ifdef CONFIG_MTD_DEVICE
	ret = del_mtd_device(mtd);
	if (ret < 0)
		return ret;
#endif

	free(dev->buffers);

	return ret;
}

static int do_ipq_nand_cmd(cmd_tbl_t *cmdtp, int flag,
			   int argc, char * const argv[])
{
	int ret;
	enum ipq_nand_layout layout;

	if (argc != 2)
		return CMD_RET_USAGE;

	if (strcmp(argv[1], "sbl") == 0)
		layout = IPQ_NAND_LAYOUT_SBL;

	else if (strcmp(argv[1], "linux") == 0)
		layout = IPQ_NAND_LAYOUT_LINUX;

	else
		return CMD_RET_USAGE;

	ret = ipq_nand_deinit();
	if (ret < 0)
		return CMD_RET_FAILURE;

	nand_curr_device = -1;

	ret = ipq_nand_init(layout);
	if (ret < 0)
		return CMD_RET_FAILURE;

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(ipq_nand, 2, 1, do_ipq_nand_cmd,
	   "Switch between SBL and Linux kernel page layout.",
	   "ipq_nand (sbl | linux)");
