/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 * Copyright (c) 2009-2015, The Linux Foundation. All rights reserved.
 *
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

#ifndef __QPIC_NAND_H
#define __QPIC_NAND_H

#define IPQ40xx_EBI2ND_BASE		(0x079b0000)
#define IPQ40xx_QPIC_BAM_CTRL		(0x07984000)

#define NAND_REG(off)			(IPQ40xx_EBI2ND_BASE + (off))

#define NAND_FLASH_CMD			NAND_REG(0x0000)
#define NAND_ADDR0			NAND_REG(0x0004)
#define NAND_ADDR1			NAND_REG(0x0008)
#define NAND_FLASH_CHIP_SELECT		NAND_REG(0x000C)
#define NAND_EXEC_CMD			NAND_REG(0x0010)
#define NAND_FLASH_STATUS		NAND_REG(0x0014)
#define NAND_BUFFER_STATUS		NAND_REG(0x0018)
#define NAND_DEV0_CFG0			NAND_REG(0x0020)
#define NAND_DEV0_CFG1			NAND_REG(0x0024)
#define NAND_DEV0_ECC_CFG		NAND_REG(0x0028)
#define NAND_DEV1_CFG0			NAND_REG(0x0030)
#define NAND_DEV1_CFG1			NAND_REG(0x0034)
#define NAND_SFLASHC_CMD		NAND_REG(0x0038)
#define NAND_SFLASHC_EXEC_CMD		NAND_REG(0x003C)
#define NAND_READ_ID			NAND_REG(0x0040)
#define NAND_READ_STATUS		NAND_REG(0x0044)
#define NAND_CONFIG_DATA		NAND_REG(0x0050)
#define NAND_CONFIG			NAND_REG(0x0054)
#define NAND_CONFIG_MODE		NAND_REG(0x0058)
#define NAND_CONFIG_STATUS		NAND_REG(0x0060)
#define NAND_MACRO1_REG			NAND_REG(0x0064)
#define NAND_XFR_STEP1			NAND_REG(0x0070)
#define NAND_XFR_STEP2			NAND_REG(0x0074)
#define NAND_XFR_STEP3			NAND_REG(0x0078)
#define NAND_XFR_STEP4			NAND_REG(0x007C)
#define NAND_XFR_STEP5			NAND_REG(0x0080)
#define NAND_XFR_STEP6			NAND_REG(0x0084)
#define NAND_XFR_STEP7			NAND_REG(0x0088)
#define NAND_GENP_REG0			NAND_REG(0x0090)
#define NAND_GENP_REG1			NAND_REG(0x0094)
#define NAND_GENP_REG2			NAND_REG(0x0098)
#define NAND_GENP_REG3			NAND_REG(0x009C)
#define NAND_SFLASHC_STATUS		NAND_REG(0x001C)
#define NAND_DEV_CMD0			NAND_REG(0x00A0)
#define NAND_DEV_CMD1			NAND_REG(0x00A4)
#define NAND_DEV_CMD2			NAND_REG(0x00A8)
#define NAND_DEV_CMD_VLD		NAND_REG(0x00AC)
#define NAND_EBI2_MISR_SIG_REG		NAND_REG(0x00B0)
#define NAND_ADDR2			NAND_REG(0x00C0)
#define NAND_ADDR3			NAND_REG(0x00C4)
#define NAND_ADDR4			NAND_REG(0x00C8)
#define NAND_ADDR5			NAND_REG(0x00CC)
#define NAND_DEV_CMD3			NAND_REG(0x00D0)
#define NAND_DEV_CMD4			NAND_REG(0x00D4)
#define NAND_DEV_CMD5			NAND_REG(0x00D8)
#define NAND_DEV_CMD6			NAND_REG(0x00DC)
#define NAND_SFLASHC_BURST_CFG		NAND_REG(0x00E0)
#define NAND_ADDR6			NAND_REG(0x00E4)
#define NAND_ERASED_CW_DETECT_CFG	NAND_REG(0x00E8)
#define NAND_ERASED_CW_DETECT_STATUS	NAND_REG(0x00EC)
#define NAND_EBI2_ECC_BUF_CFG		NAND_REG(0x00F0)
#define NAND_HW_INFO			NAND_REG(0x00FC)
#define NAND_FLASH_BUFFER		NAND_REG(0x0100)
#define QPIC_NAND_CTRL			NAND_REG(0x0F00)
#define QPIC_NAND_DEBUG			NAND_REG(0x0F0C)

/* NANDc registers used during BAM transfer */
#define NAND_READ_LOCATION_n(n)		(NAND_REG(0xF20) + 4 * (n))
#define NAND_RD_LOC_LAST_BIT(x)		((x) << 31)
#define NAND_RD_LOC_SIZE(x)		((x) <<  16)
#define NAND_RD_LOC_OFFSET(x)		((x) <<  0)

/* Shift Values */
#define NAND_DEV0_CFG1_WIDE_BUS_SHIFT		1
#define NAND_DEV0_CFG0_DIS_STS_AFTER_WR_SHIFT	4
#define NAND_DEV0_CFG0_CW_PER_PAGE_SHIFT	6
#define NAND_DEV0_CFG0_UD_SIZE_BYTES_SHIFT	9
#define NAND_DEV0_CFG0_ADDR_CYCLE_SHIFT		27
#define NAND_DEV0_CFG0_SPARE_SZ_BYTES_SHIFT	23

#define NAND_DEV0_CFG1_RECOVERY_CYCLES_SHIFT	2
#define NAND_DEV0_CFG1_CS_ACTIVE_BSY_SHIFT	5
#define NAND_DEV0_CFG1_BAD_BLK_BYTE_NUM_SHIFT	6
#define NAND_DEV0_CFG1_BAD_BLK_IN_SPARE_SHIFT	16
#define NAND_DEV0_CFG1_WR_RD_BSY_GAP_SHIFT	17
#define NAND_DEV0_ECC_DISABLE_SHIFT		0
#define NAND_DEV0_ECC_SW_RESET_SHIFT		1
#define NAND_DEV0_ECC_MODE_SHIFT		4
#define NAND_DEV0_ECC_DISABLE_SHIFT		0
#define NAND_DEV0_ECC_PARITY_SZ_BYTES_SHIFT	8
#define NAND_DEV0_ECC_NUM_DATA_BYTES		16
#define NAND_DEV0_ECC_FORCE_CLK_OPEN_SHIFT	30

#define NAND_ERASED_CW_DETECT_STATUS_PAGE_ALL_ERASED		7
#define NAND_ERASED_CW_DETECT_STATUS_CODEWORD_ALL_ERASED	6
#define NAND_ERASED_CW_DETECT_STATUS_CODEWORD_ERASED		4

#define NAND_ERASED_CW_DETECT_CFG_RESET_CTRL		1
#define NAND_ERASED_CW_DETECT_CFG_ACTIVATE_CTRL		0
#define NAND_ERASED_CW_DETECT_ERASED_CW_ECC_MASK	(1 << 1)
#define NAND_ERASED_CW_DETECT_ERASED_CW_ECC_NO_MASK	(0 << 1)

/* device commands */
#define NAND_CMD_SOFT_RESET				0x01
#define NAND_CMD_PAGE_READ				0x32
#define NAND_CMD_PAGE_READ_ECC				0x33
#define NAND_CMD_PAGE_READ_ALL				0x34
#define NAND_CMD_SEQ_PAGE_READ				0x15
#define NAND_CMD_PRG_PAGE				0x36
#define NAND_CMD_PRG_PAGE_ECC				0x37
#define NAND_CMD_PRG_PAGE_ALL				0x39
#define NAND_CMD_BLOCK_ERASE				0x3A
#define NAND_CMD_FETCH_ID				0x0B
/* NAND Status errors */
#define NAND_FLASH_MPU_ERR				(1 << 8)
#define NAND_FLASH_TIMEOUT_ERR				(1 << 6)
#define NAND_FLASH_OP_ERR				(1 << 4)

#define NAND_FLASH_ERR				(NAND_FLASH_MPU_ERR | \
						 NAND_FLASH_TIMEOUT_ERR | \
						 NAND_FLASH_OP_ERR)

#define PROG_ERASE_OP_RESULT			(1 << 7)

#define NUM_ERRORS_MASK				0x0000001f
#define NUM_ERRORS(i)				((i) << 0)

#define DATA_CONSUMER_PIPE_INDEX		0
#define DATA_PRODUCER_PIPE_INDEX		1
#define CMD_PIPE_INDEX				2

/* Define BAM pipe lock groups for NANDc*/
#define P_LOCK_GROUP_0				0

/* Define BAM pipe lock super groups for NANDc
 * Note: This is configured by TZ.
 */
#define P_LOCK_SUPERGROUP_0			0
#define P_LOCK_SUPERGROUP_1			1

#define ONFI_SIGNATURE				0x49464E4F

#define ONFI_CRC_POLYNOMIAL			0x8005
#define ONFI_CRC_INIT_VALUE			0x4F4E

#define ONFI_READ_PARAM_PAGE_ADDR_CYCLES	1
#define ONFI_READ_ID_ADDR_CYCLES		1

#define ONFI_READ_ID_CMD			0x90
#define ONFI_READ_PARAM_PAGE_CMD		0xEC
#define ONFI_READ_ID_ADDR			0x20
#define ONFI_READ_PARAM_PAGE_ADDR		0x00

#define NAND_CFG0_RAW_ONFI_ID			0x88000800
#define NAND_CFG0_RAW_ONFI_PARAM_PAGE		0x88040000
#define NAND_CFG1_RAW_ONFI_ID			0x0005045D
#define NAND_CFG1_RAW_ONFI_PARAM_PAGE		0x0005045D
#define NAND_CFG0				0x290409c0
#define NAND_CFG1				0x08045d5c
#define NAND_ECC_BCH_CFG			0x42040d10
#define NAND_Bad_Block				0x00000175
#define NAND_ECC_BUF_CFG			0x00000203

#define ONFI_READ_ID_BUFFER_SIZE		0x4
#define ONFI_READ_PARAM_PAGE_BUFFER_SIZE	0x200
#define ONFI_PARAM_PAGE_SIZE			0x100

#define NAND_8BIT_DEVICE			0x01
#define NAND_16BIT_DEVICE			0x02

#define NAND_CW_SIZE_4_BIT_ECC			528
#define NAND_CW_SIZE_8_BIT_ECC			532

/* Indicates the data bytes in the user data portion of the code word. */
#define USER_DATA_BYTES_PER_CW			512

/* Indicates the number of bytes covered by BCH ECC logic when
 * a codeword is written to a NAND flash device.
 * This is also the number of bytes that are part of the image in  CW.
 * 516 bytes  = (512 bytes of user data and 4 bytes of spare data)
 */
#define DATA_BYTES_IN_IMG_PER_CW		516

#define NAND_CW_DIV_RIGHT_SHIFT			9

/* Number of max cw's the driver allows to flash. */
#define QPIC_NAND_MAX_CWS_IN_PAGE		10

/* Reset Values for Status registers */
#define NAND_FLASH_STATUS_RESET			0x00000020
#define NAND_READ_STATUS_RESET			0x000000C0

/* NANDc BAM pipe numbers */
#define DATA_CONSUMER_PIPE			0
#define DATA_PRODUCER_PIPE			1
#define CMD_PIPE				2

/* NANDc BAM pipe groups */
#define DATA_PRODUCER_PIPE_GRP			0
#define DATA_CONSUMER_PIPE_GRP			0
#define CMD_PIPE_GRP				1

/* NANDc EE */
#define QPIC_NAND_EE				0

/* NANDc max desc length. */
#define QPIC_NAND_MAX_DESC_LEN			0x7FFF

/* Register: NAND_CTRL */
#define BAM_MODE_EN				0x1

/* Register: NAND_DEBUG */
#define BAM_MODE_BIT_RESET			(1 << 31)

/* CMD Valid */
#define NAND_CMD_VALID_BASE			0x1D

#define NAND_ID_MAN(id)				((id) & 0xFF)
#define NAND_ID_DEV(id)				(((id) >> 8) & 0xFF)
#define NAND_ID_CFG(id)				(((id) >> 24) & 0xFF)

#define NAND_CFG_PAGE_SIZE(id)			(((id) >> 0) & 0x3)
#define NAND_CFG_SPARE_SIZE(id)			(((id) >> 2) & 0x3)
#define NAND_CFG_BLOCK_SIZE(id)			(((id) >> 4) & 0x3)

#define CHUNK_SIZE				512

#define MB_TO_BYTES(mb)				(((uint64_t)(mb)) << 20)
#define KB_TO_BYTES(kb)				((kb) << 10)

#define MTD_NAND_CHIP(mtd)			((struct nand_chip *)((mtd)->priv))
#define MTD_QPIC_NAND_DEV(mtd)			(MTD_NAND_CHIP(mtd)->priv)

#define QPIC_BAM_DATA_FIFO_SIZE			64
#define QPIC_BAM_CMD_FIFO_SIZE			64

/* result type */
typedef enum {
	NANDC_RESULT_SUCCESS = 0,
	NANDC_RESULT_FAILURE = 1,
	NANDC_RESULT_TIMEOUT = 2,
	NANDC_RESULT_PARAM_INVALID = 3,
	NANDC_RESULT_DEV_NOT_SUPPORTED = 4,
	NANDC_RESULT_BAD_PAGE = 5,
	NANDC_RESULT_BAD_BLOCK = 6,
} nand_result_t;

enum nand_bad_block_value
{
	NAND_BAD_BLK_VALUE_NOT_READ,
	NAND_BAD_BLK_VALUE_IS_BAD,
	NAND_BAD_BLK_VALUE_IS_GOOD,
};

enum nand_cfg_value
{
	NAND_CFG_RAW,
	NAND_CFG,
};

struct onfi_param_page
{
	uint32_t signature;
	uint16_t rev;
	uint16_t feature_supported;
	uint16_t opt_cmd_supported;
	uint8_t reserved_1[22];
	uint8_t mib[12];
	uint8_t device_model[20];
	uint8_t manufacturer_id;
	uint16_t date_code;
	uint8_t  reserved_2[13];
	uint32_t data_per_pg;
	uint16_t spare_per_pg;
	uint32_t data_per_partial_pg;
	uint16_t spare_per_partial_pg;
	uint32_t pgs_per_blk;
	uint32_t blks_per_LUN;
	uint8_t num_LUN;
	uint8_t num_addr_cycles;
	uint8_t num_bits_per_cell;
	uint16_t bad_blks_max_per_LUN;
	uint16_t blk_endurance;
	uint8_t guaranteed_vld_blks_at_start;
	uint16_t blk_endurance_for_garunteed_vld_blks;
	uint8_t num_prg_per_pg;
	uint8_t partial_prog_attr;
	uint8_t num_bits_ecc_correctability;
	uint8_t num_interleaved_addr_bits;
	uint8_t interleaved_op_attr;
	uint8_t reserved_3[13];
	uint8_t io_pin_capcacitance;
	uint16_t timing_mode_support;
	uint16_t prog_cache_timing_mode_support;
	uint16_t max_pg_prog_time_us;
	uint16_t max_blk_erase_time_us;
	uint16_t max_pr_rd_time_us;
	uint16_t min_chg_col_setup_time_us;
	uint8_t reserved_4[23];
	uint16_t vendor_rev;
	uint8_t vendor_specific[88];
	uint16_t interity_crc;
}__packed;

struct cfg_params
{
	uint32_t addr0;
	uint32_t addr1;
	uint32_t cfg0;
	uint32_t cfg1;
	uint32_t cmd;
	uint32_t ecc_cfg;
	uint32_t addr_loc_0;
	uint32_t exec;
};

struct onfi_probe_params
{
	uint32_t vld;
	uint32_t dev_cmd1;
	struct cfg_params cfg;
};

/* This stucture is used to create a static list of devices we support.
 * This include a subset of values defined in the flash_info struct as
 * other values can be derived.
 */
struct flash_id
{
	unsigned flash_id;
	unsigned mask;
	unsigned density;
	unsigned widebus;
	unsigned pagesize;
	unsigned blksize;
	unsigned oobsize;
	unsigned ecc_8_bits;
};

/* Structure to hold the pipe numbers */
struct qpic_nand_bam_pipes
{
	unsigned read_pipe;
	unsigned write_pipe;
	unsigned cmd_pipe;
	uint8_t  read_pipe_grp;
	uint8_t  write_pipe_grp;
	uint8_t  cmd_pipe_grp;
};

/* Structure to define the initial nand config */
struct qpic_nand_init_config
{
	uint32_t nand_base;
	uint32_t bam_base;
	uint32_t ee;
	uint32_t max_desc_len;
	struct qpic_nand_bam_pipes pipes;
};

enum nand_ecc_width
{
	NAND_WITH_4_BIT_ECC,
	NAND_WITH_8_BIT_ECC,
};

struct qpic_nand_dev {
	unsigned id;
	unsigned type;
	unsigned vendor;
	unsigned device;
	unsigned page_size;
	unsigned block_size;
	unsigned spare_size;
	unsigned num_blocks;
	enum nand_ecc_width ecc_width;
	unsigned num_pages_per_blk;
	unsigned num_pages_per_blk_mask;
	unsigned widebus;
	unsigned density;
	unsigned cw_size;
	unsigned cws_per_page;
	unsigned bad_blk_loc;
	unsigned dev_cfg;
	uint32_t cfg0;
	uint32_t cfg1;
	uint32_t cfg0_raw;
	uint32_t cfg1_raw;
	uint32_t ecc_bch_cfg;
	unsigned oob_per_page;
	unsigned char *buffers;
	unsigned char *pad_dat;
	unsigned char *pad_oob;
	unsigned char *zero_page;
	unsigned char *zero_oob;
};

#endif
