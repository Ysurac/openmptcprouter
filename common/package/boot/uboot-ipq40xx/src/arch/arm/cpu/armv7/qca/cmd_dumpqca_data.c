/*
 * Copyright (c) 2015-2016 The Linux Foundation. All rights reserved.

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

#include <common.h>
#include <command.h>
#include <image.h>
#include <asm/arch-ipq40xx/smem.h>
#include <asm/arch-ipq40xx/scm.h>
#include <configs/ipq40xx_cdp.h>
#include <nand.h>
#include <spi_flash.h>
#include <mmc.h>
#include <asm/errno.h>
#include <linux/time.h>
DECLARE_GLOBAL_DATA_PTR;

static int do_dumpqca_data(cmd_tbl_t *cmdtp, int flag, int argc,
				char *const argv[])
{
	char runcmd[128];
	char *serverip = NULL;
	/* dump to root of TFTP server if none specified */
	char *dumpdir;
	uint32_t memaddr;
	int ret;
	char buf;

	if (argc == 2) {
		serverip = argv[1];
		printf("Using given serverip %s\n", serverip);
		setenv("serverip", serverip);
	} else {
		serverip = getenv("serverip");
		if (serverip != NULL) {
			printf("Using serverip from env %s\n", serverip);
		} else {
			printf("\nServer ip not found, run dhcp or configure\n");
			return CMD_RET_FAILURE;
		}
	}

	if ((dumpdir = getenv("dumpdir")) != NULL) {
		printf("Using directory %s in TFTP server\n", dumpdir);
	} else {
		dumpdir = "";
		printf("Env 'dumpdir' not set. Using / dir in TFTP server\n");
	}

	printf("\nProcessing dumps.....\n");
	ret = scm_call(SCM_SVC_FUSE, QFPROM_IS_AUTHENTICATE_CMD,
				NULL, 0, &buf, sizeof(char));
	if (ret == 0 && buf == 1) {
		/*
		* Dumps CPU_CONTEXT_DUMP_BASE to gd->ram_size.
		*/
		memaddr = CONFIG_CPU_CONTEXT_DUMP_BASE;
		gd->ram_size = CONFIG_CPU_CONTEXT_DUMP_SIZE;
		snprintf(runcmd, sizeof(runcmd), "tftpput 0x%x 0x%x %s/%s",
			memaddr, (unsigned int)gd->ram_size,
			dumpdir, "EBICS0.bin");
	} else {
		/*
		* Dumps CONFIG_SYS_SDRAM_BASE to gd->ram_size.
		*/
		memaddr = CONFIG_SYS_SDRAM_BASE;
		snprintf(runcmd, sizeof(runcmd), "tftpput 0x%x 0x%x %s/%s",
			memaddr, (unsigned int)gd->ram_size,
			dumpdir, "EBICS0.bin");
	}

	if (run_command(runcmd, 0) != CMD_RET_SUCCESS)
		return CMD_RET_FAILURE;
	udelay(10000); /* give some delay for server */
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(dumpipq_data, 2, 0, do_dumpqca_data,
	   "dumpipq_data crashdump collection from memory",
	   "dumpipq_data [serverip] - Crashdump collection from memory vi tftp\n");


#ifdef CONFIG_QCA_APPSBL_DLOAD
#define MAX_UNAME_SIZE 1024
#define QCOM_WDT_SCM_TLV_TYPE_SIZE 1
#define QCOM_WDT_SCM_TLV_LEN_SIZE 2
#define QCOM_WDT_SCM_TLV_TYPE_LEN_SIZE (QCOM_WDT_SCM_TLV_TYPE_SIZE +\
						QCOM_WDT_SCM_TLV_LEN_SIZE)
#define MAX_NAND_PAGE_SIZE 2048
#define MAX_EMMC_BLK_LEN 1024

#ifndef CONFIG_CRASHDUMP_SPI_SPEED
#define CONFIG_CRASHDUMP_SPI_SPEED	1000000
#endif

#ifndef CONFIG_CRASHDUMP_SPI_MODE
#define CONFIG_CRASHDUMP_SPI_MODE	SPI_MODE_3
#endif

#ifndef CONFIG_SYS_MMC_CRASHDUMP_DEV
#define CONFIG_SYS_MMC_CRASHDUMP_DEV 0
#endif

/* Type in TLV for crashdump data type */
enum {
	QCOM_WDT_LOG_DUMP_TYPE_INVALID,
	QCOM_WDT_LOG_DUMP_TYPE_UNAME,
};

/* This will be used for parsing the TLV data */
struct qcom_wdt_scm_tlv_msg {
	unsigned char *msg_buffer;
	unsigned char *cur_msg_buffer_pos;
	unsigned int len;
};

/* Actual crashdump related data */
struct qcom_wdt_crashdump_data {
	unsigned char uname[MAX_UNAME_SIZE];
	unsigned int uname_length;
	unsigned char *cpu_context;
};

/* Context for NAND Flash memory */
struct crashdump_flash_nand_cxt {
	loff_t start_crashdump_offset;
	loff_t cur_crashdump_offset;
	int cur_page_data_len;
	int write_size;
	unsigned char temp_data[MAX_NAND_PAGE_SIZE];
};

/* Context for SPI NOR Flash memory */
struct crashdump_flash_spi_cxt {
	loff_t start_crashdump_offset;
	loff_t cur_crashdump_offset;
};

#ifdef CONFIG_QCA_MMC
/* Context for EMMC Flash memory */
struct crashdump_flash_emmc_cxt {
	loff_t start_crashdump_offset;
	loff_t cur_crashdump_offset;
	int cur_blk_data_len;
	int write_size;
	unsigned char temp_data[MAX_EMMC_BLK_LEN];
};
#endif

static qca_smem_flash_info_t *sfi = &qca_smem_flash_info;
static struct spi_flash *crashdump_spi_flash;
static struct crashdump_flash_nand_cxt crashdump_nand_cnxt;
static struct crashdump_flash_spi_cxt crashdump_flash_spi_cnxt;
#ifdef CONFIG_QCA_MMC
static struct mmc *mmc;
static struct crashdump_flash_emmc_cxt crashdump_emmc_cnxt;
#endif
static struct qcom_wdt_crashdump_data g_crashdump_data;

/* Converts the string in hex format to integer data format */
static inline int str2off(const char *p, loff_t *num)
{
	char *endptr;

	*num = simple_strtoull(p, &endptr, 16);

	return *p != '\0' && *endptr == '\0';
}

/* Extracts the type and length in TLV for current offset */
static int qcom_wdt_scm_extract_tlv_info(
		struct qcom_wdt_scm_tlv_msg *scm_tlv_msg,
		unsigned char *type,
		unsigned int *size)
{
	unsigned char *x = scm_tlv_msg->cur_msg_buffer_pos;
	unsigned char *y = scm_tlv_msg->msg_buffer +
				scm_tlv_msg->len;

	if ((x + QCOM_WDT_SCM_TLV_TYPE_LEN_SIZE) >= y)
		return -EINVAL;

	*type = x[0];
	*size = x[1] | (x[2] << 8);

	return 0;
}

/* Extracts the value from TLV for current offset */
static int qcom_wdt_scm_extract_tlv_data(
		struct qcom_wdt_scm_tlv_msg *scm_tlv_msg,
		unsigned char *data,
		unsigned int size)
{
	unsigned char *x = scm_tlv_msg->cur_msg_buffer_pos;
	unsigned char *y = scm_tlv_msg->msg_buffer + scm_tlv_msg->len;

	if ((x + QCOM_WDT_SCM_TLV_TYPE_LEN_SIZE + size) >= y)
		return -EINVAL;

	memcpy(data, x + 3, size);

	scm_tlv_msg->cur_msg_buffer_pos +=
		(size + QCOM_WDT_SCM_TLV_TYPE_LEN_SIZE);

	return 0;
}

/*
* This function parses the TLV message and stores the actual values
* in crashdump_data. For each TLV, It first determines the type and
* length, then it extracts the actual value and stores in the appropriate
* field in crashdump_data.
*/
static int qcom_wdt_extract_crashdump_data(
		struct qcom_wdt_scm_tlv_msg *scm_tlv_msg,
		struct qcom_wdt_crashdump_data *crashdump_data)
{
	unsigned char cur_type = QCOM_WDT_LOG_DUMP_TYPE_INVALID;
	unsigned int cur_size;
	int ret_val;

	do {
		ret_val = qcom_wdt_scm_extract_tlv_info(scm_tlv_msg,
				&cur_type, &cur_size);

		if (ret_val)
			break;

		switch (cur_type) {
		case QCOM_WDT_LOG_DUMP_TYPE_UNAME:
			crashdump_data->uname_length = cur_size;
			ret_val = qcom_wdt_scm_extract_tlv_data(scm_tlv_msg,
					crashdump_data->uname,
					cur_size);

			break;

		case QCOM_WDT_LOG_DUMP_TYPE_INVALID:
			break;

		default:
			ret_val = -EINVAL;
			break;
		}

		if (ret_val != 0)
			break;

	} while (cur_type != QCOM_WDT_LOG_DUMP_TYPE_INVALID);

	return ret_val;
}

/*
* Init function for NAND flash writing. It intializes its own context
* and erases the required sectors
*/
int init_crashdump_nand_flash_write(void *cnxt, loff_t offset,
					unsigned int total_size)
{
	nand_erase_options_t nand_erase_options;
	struct crashdump_flash_nand_cxt *nand_cnxt = cnxt;
	int ret;

	nand_cnxt->start_crashdump_offset = offset;
	nand_cnxt->cur_crashdump_offset = offset;
	nand_cnxt->cur_page_data_len = 0;
	nand_cnxt->write_size = nand_info[0].writesize;

	if (nand_info[0].writesize > MAX_NAND_PAGE_SIZE) {
		printf("nand page write size is more than configured size\n");
		return -ENOMEM;
	}

	memset(&nand_erase_options, 0, sizeof(nand_erase_options));

	nand_erase_options.length = total_size;
	nand_erase_options.offset = offset;

	ret = nand_erase_opts(&nand_info[0],
			    &nand_erase_options);
	if (ret)
		return ret;

	return 0;
}

/*
* Deinit function for NAND flash writing. It writes the remaining data
* stored in temp buffer to NAND.
*/
int deinit_crashdump_nand_flash_write(void *cnxt)
{
	struct crashdump_flash_nand_cxt *nand_cnxt = cnxt;
	unsigned int cur_nand_write_len = nand_cnxt->cur_page_data_len;
	int ret_val = 0;
	int remaining_bytes = nand_cnxt->write_size -
			nand_cnxt->cur_page_data_len;

	if (cur_nand_write_len) {
		/*
		* Make the write data in multiple of page write size
		* and write remaining data in NAND flash
		*/
		memset(nand_cnxt->temp_data + nand_cnxt->cur_page_data_len,
			0xFF, remaining_bytes);

		cur_nand_write_len = nand_cnxt->write_size;
		ret_val = nand_write(&nand_info[0],
				nand_cnxt->cur_crashdump_offset,
				&cur_nand_write_len, nand_cnxt->temp_data);
	}

	return ret_val;
}

/*
* Write function for NAND flash. NAND writing works on page basis so
* this function writes the data in mulitple of page size and stores the
* remaining data in temp buffer. This temp buffer data will be appended
* with next write data.
*/
int crashdump_nand_flash_write_data(void *cnxt,
		unsigned char *data, unsigned int size)
{
	struct crashdump_flash_nand_cxt *nand_cnxt = cnxt;
	unsigned char *cur_data_pos = data;
	unsigned int remaining_bytes;
	unsigned int total_bytes;
	unsigned int cur_nand_write_len;
	unsigned int remaining_len_cur_page;
	int ret_val;

	remaining_bytes = total_bytes = nand_cnxt->cur_page_data_len + size;

	/*
	* Check for minimum write size and store the data in temp buffer if
	* the total size is less than it
	*/
	if (total_bytes < nand_cnxt->write_size) {
		memcpy(nand_cnxt->temp_data + nand_cnxt->cur_page_data_len,
				data, size);
		nand_cnxt->cur_page_data_len += size;

		return 0;
	}

	/*
	* Append the remaining length of data for complete nand page write in
	* currently stored data and do the nand write
	*/
	remaining_len_cur_page = nand_cnxt->write_size -
			nand_cnxt->cur_page_data_len;
	cur_nand_write_len = nand_cnxt->write_size;

	memcpy(nand_cnxt->temp_data + nand_cnxt->cur_page_data_len, data,
			remaining_len_cur_page);

	ret_val = nand_write(&nand_info[0], nand_cnxt->cur_crashdump_offset,
				&cur_nand_write_len,
				nand_cnxt->temp_data);

	if (ret_val)
		return ret_val;

	cur_data_pos += remaining_len_cur_page;
	nand_cnxt->cur_crashdump_offset += cur_nand_write_len;

	/*
	* Calculate the write length in multiple of page length and do the nand
	* write for same length
	*/
	cur_nand_write_len = ((data + size - cur_data_pos) /
				nand_cnxt->write_size) * nand_cnxt->write_size;

	if (cur_nand_write_len > 0) {
		ret_val = nand_write(&nand_info[0],
				nand_cnxt->cur_crashdump_offset,
				&cur_nand_write_len,
				cur_data_pos);

		if (ret_val)
			return ret_val;
	}

	cur_data_pos += cur_nand_write_len;
	nand_cnxt->cur_crashdump_offset += cur_nand_write_len;

	/* Store the remaining data in temp data */
	remaining_bytes = data + size - cur_data_pos;

	memcpy(nand_cnxt->temp_data, cur_data_pos, remaining_bytes);

	nand_cnxt->cur_page_data_len = remaining_bytes;

	return 0;
}

/* Init function for SPI NOR flash writing. It erases the required sectors */
int init_crashdump_spi_flash_write(void *cnxt,
			loff_t offset,
			unsigned int total_size)
{
	int ret;
	unsigned int required_erase_size;
	struct crashdump_flash_spi_cxt *spi_flash_cnxt = cnxt;

	spi_flash_cnxt->start_crashdump_offset = offset;
	spi_flash_cnxt->cur_crashdump_offset = offset;

	if (total_size & (sfi->flash_block_size - 1))
		required_erase_size = (total_size &
					~(sfi->flash_block_size - 1)) +
					sfi->flash_block_size;
	else
		required_erase_size = total_size;

	ret = spi_flash_erase(crashdump_spi_flash,
				offset,
				required_erase_size);

	return ret;
}

/* Write function for SPI NOR flash */
int crashdump_spi_flash_write_data(void *cnxt,
		unsigned char *data, unsigned int size)
{
	struct crashdump_flash_spi_cxt *spi_flash_cnxt = cnxt;
	unsigned int cur_size = size;
	int ret;

	ret = spi_flash_write(crashdump_spi_flash,
			spi_flash_cnxt->cur_crashdump_offset,
			cur_size, data);

	if (!ret)
		spi_flash_cnxt->cur_crashdump_offset += cur_size;

	return ret;
}

/* Deinit function for SPI NOR flash writing. */
int deinit_crashdump_spi_flash_write(void *cnxt)
{
	return 0;
}

#ifdef CONFIG_QCA_MMC
/* Init function for EMMC. It initialzes the EMMC */
static int crashdump_init_mmc(struct mmc *mmc)
{
	int ret;

	if (!mmc) {
		puts("No MMC card found\n");
		return -EINVAL;
	}

	ret = mmc_init(mmc);

	if (ret)
		puts("MMC init failed\n");

	return ret;
}

/*
* Init function for EMMC flash writing. It initialzes its
* own context and EMMC
*/
int init_crashdump_emmc_flash_write(void *cnxt, loff_t offset,
					unsigned int total_size)
{
	struct crashdump_flash_emmc_cxt *emmc_cnxt = cnxt;

	emmc_cnxt->start_crashdump_offset = offset;
	emmc_cnxt->cur_crashdump_offset = offset;
	emmc_cnxt->cur_blk_data_len = 0;
	emmc_cnxt->write_size =  mmc->write_bl_len;

	if (mmc->write_bl_len > MAX_EMMC_BLK_LEN) {
		printf("mmc block length is more than configured size\n");
		return -ENOMEM;
	}

	return 0;
}

/*
* Deinit function for EMMC flash writing. It writes the remaining data
* stored in temp buffer to EMMC
*/
int deinit_crashdump_emmc_flash_write(void *cnxt)
{
	struct crashdump_flash_emmc_cxt *emmc_cnxt = cnxt;
	unsigned int cur_blk_write_len = emmc_cnxt->cur_blk_data_len;
	int ret_val = 0;
	int n;
	int remaining_bytes = emmc_cnxt->write_size -
			emmc_cnxt->cur_blk_data_len;

	if (cur_blk_write_len) {
		/*
		* Make the write data in multiple of block length size
		* and write remaining data in emmc
		*/
		memset(emmc_cnxt->temp_data + emmc_cnxt->cur_blk_data_len,
			0xFF, remaining_bytes);

		cur_blk_write_len = emmc_cnxt->write_size;
		n = mmc->block_dev.block_write(CONFIG_SYS_MMC_CRASHDUMP_DEV,
						emmc_cnxt->cur_crashdump_offset,
						1,
						(u_char *)emmc_cnxt->temp_data);

		ret_val = (n == 1) ? 0 : -ENOMEM;
	}

	return ret_val;
}

/*
* Write function for EMMC flash. EMMC writing works on block basis so
* this function writes the data in mulitple of block length and stores
* remaining data in temp buffer. This temp buffer data will be appended
* with next write data.
*/
int crashdump_emmc_flash_write_data(void *cnxt,
		unsigned char *data, unsigned int size)
{
	struct crashdump_flash_emmc_cxt *emmc_cnxt = cnxt;
	unsigned char *cur_data_pos = data;
	unsigned int remaining_bytes;
	unsigned int total_bytes;
	unsigned int cur_emmc_write_len;
	unsigned int cur_emmc_blk_len;
	unsigned int remaining_len_cur_page;
	int ret_val;
	int n;

	remaining_bytes = total_bytes = emmc_cnxt->cur_blk_data_len + size;

	/*
	* Check for block size and store the data in temp buffer if
	* the total size is less than it
	*/
	if (total_bytes < emmc_cnxt->write_size) {
		memcpy(emmc_cnxt->temp_data + emmc_cnxt->cur_blk_data_len,
				data, size);
		emmc_cnxt->cur_blk_data_len += size;

		return 0;
	}

	/*
	* Append the remaining length of data for complete emmc block write in
	* currently stored data and do the block write
	*/
	remaining_len_cur_page = emmc_cnxt->write_size -
			emmc_cnxt->cur_blk_data_len;
	cur_emmc_write_len = emmc_cnxt->write_size;

	memcpy(emmc_cnxt->temp_data + emmc_cnxt->cur_blk_data_len, data,
			remaining_len_cur_page);

	n = mmc->block_dev.block_write(CONFIG_SYS_MMC_CRASHDUMP_DEV,
					emmc_cnxt->cur_crashdump_offset,
					1,
					(u_char *)emmc_cnxt->temp_data);

	ret_val = (n == 1) ? 0 : -ENOMEM;

	if (ret_val)
		return ret_val;

	cur_data_pos += remaining_len_cur_page;
	emmc_cnxt->cur_crashdump_offset += 1;

	/*
	* Calculate the write length in multiple of block length and do the
	* emmc block write for same length
	*/
	cur_emmc_blk_len = ((data + size - cur_data_pos) /
				emmc_cnxt->write_size);
	cur_emmc_write_len = cur_emmc_blk_len * emmc_cnxt->write_size;

	if (cur_emmc_write_len > 0) {
		n = mmc->block_dev.block_write(CONFIG_SYS_MMC_CRASHDUMP_DEV,
						emmc_cnxt->cur_crashdump_offset,
						cur_emmc_blk_len,
						(u_char *)cur_data_pos);

		ret_val = (n == cur_emmc_blk_len) ? 0 : -1;

		if (ret_val)
			return ret_val;
	}

	cur_data_pos += cur_emmc_write_len;
	emmc_cnxt->cur_crashdump_offset += cur_emmc_blk_len;

	/* Store the remaining data in temp data */
	remaining_bytes = data + size - cur_data_pos;

	memcpy(emmc_cnxt->temp_data, cur_data_pos, remaining_bytes);

	emmc_cnxt->cur_blk_data_len = remaining_bytes;

	return 0;
}
#endif

/*
* This function writes the crashdump data in flash memory.
* It has function pointers for init, deinit and writing. These
* function pointers are being initialized with respective flash
* memory writing routines.
*/
static int qcom_wdt_write_crashdump_data(
		struct qcom_wdt_crashdump_data *crashdump_data,
		int flash_type, loff_t crashdump_offset)
{
	int ret = 0;
	void *crashdump_cnxt;
	int (*crashdump_flash_write)(void *cnxt, unsigned char *data,
					unsigned int size);
	int (*crashdump_flash_write_init)(void *cnxt, loff_t offset,
					unsigned int total_size);
	int (*crashdump_flash_write_deinit)(void *cnxt);
	unsigned int required_size;

	/*
	* Determine the flash type and initialize function pointer for flash
	* operations and its context which needs to be passed to these functions
	*/
	if (flash_type == SMEM_BOOT_NAND_FLASH) {
		crashdump_cnxt = (void *)&crashdump_nand_cnxt;
		crashdump_flash_write_init = init_crashdump_nand_flash_write;
		crashdump_flash_write = crashdump_nand_flash_write_data;
		crashdump_flash_write_deinit =
			deinit_crashdump_nand_flash_write;
	} else if (flash_type == SMEM_BOOT_SPI_FLASH) {
		if (!crashdump_spi_flash) {
			crashdump_spi_flash = spi_flash_probe(sfi->flash_index,
				sfi->flash_chip_select,
				CONFIG_CRASHDUMP_SPI_SPEED,
				CONFIG_CRASHDUMP_SPI_MODE);

			if (!crashdump_spi_flash) {
				printf("spi_flash_probe() failed");
				return -EIO;
			}
		}

		crashdump_cnxt = (void *)&crashdump_flash_spi_cnxt;
		crashdump_flash_write = crashdump_spi_flash_write_data;
		crashdump_flash_write_init = init_crashdump_spi_flash_write;
		crashdump_flash_write_deinit =
			deinit_crashdump_spi_flash_write;
#ifdef CONFIG_QCA_MMC
	} else if (flash_type == SMEM_BOOT_MMC_FLASH) {
		mmc = find_mmc_device(CONFIG_SYS_MMC_CRASHDUMP_DEV);

		ret = crashdump_init_mmc(mmc);

		if (ret)
			return ret;

		crashdump_cnxt = (void *)&crashdump_emmc_cnxt;
		crashdump_flash_write_init = init_crashdump_emmc_flash_write;
		crashdump_flash_write = crashdump_emmc_flash_write_data;
		crashdump_flash_write_deinit =
			deinit_crashdump_emmc_flash_write;
#endif
	} else {
		return -EINVAL;
	}

	/* Start writing cpu context and uname in flash */
	required_size = CONFIG_CPU_CONTEXT_DUMP_SIZE +
				crashdump_data->uname_length;

	ret = crashdump_flash_write_init(crashdump_cnxt,
			crashdump_offset,
			required_size);

	if (ret)
		return ret;

	ret = crashdump_flash_write(crashdump_cnxt,
			crashdump_data->cpu_context,
			CONFIG_CPU_CONTEXT_DUMP_SIZE);

	if (!ret)
		ret = crashdump_flash_write(crashdump_cnxt,
			crashdump_data->uname,
			crashdump_data->uname_length);

	if (!ret)
		ret = crashdump_flash_write_deinit(crashdump_cnxt);

	return ret;
}

/*
* Function for collecting the crashdump data in flash. It extracts the
* crashdump TLV(Type Length Value) data and CPU context information from
* page allocated by kernel for crashdump data collection. It determines
* the type of boot flash memory and writes all these crashdump information
* in provided offset in flash memory.
*/
static int do_dumpqca_flash_data(cmd_tbl_t *cmdtp, int flag, int argc,
				char *const argv[])
{
	unsigned char *kernel_crashdump_address =
		(unsigned char *) CONFIG_CPU_CONTEXT_DUMP_BASE;
	struct qcom_wdt_scm_tlv_msg tlv_msg;
	int flash_type;
	int ret_val;
	loff_t crashdump_offset;

	if (argc != 2)
		return CMD_RET_USAGE;

	if (sfi->flash_type == SMEM_BOOT_NAND_FLASH) {
		flash_type = SMEM_BOOT_NAND_FLASH;
	} else if (sfi->flash_type == SMEM_BOOT_SPI_FLASH) {
		flash_type = SMEM_BOOT_SPI_FLASH;
#ifdef CONFIG_QCA_MMC
	} else if (sfi->flash_type == SMEM_BOOT_MMC_FLASH) {
		flash_type = SMEM_BOOT_MMC_FLASH;
#endif
	} else {
		printf("command not supported for this flash memory\n");
		return CMD_RET_FAILURE;
	}

	ret_val = str2off(argv[1], &crashdump_offset);

	if (!ret_val)
		return CMD_RET_USAGE;

	g_crashdump_data.cpu_context = kernel_crashdump_address;
	tlv_msg.msg_buffer = kernel_crashdump_address + CONFIG_CPU_CONTEXT_DUMP_SIZE;
	tlv_msg.cur_msg_buffer_pos = tlv_msg.msg_buffer;
	tlv_msg.len = CONFIG_TLV_DUMP_SIZE;

	ret_val = qcom_wdt_extract_crashdump_data(&tlv_msg, &g_crashdump_data);

	if (!ret_val)
		ret_val = qcom_wdt_write_crashdump_data(&g_crashdump_data,
				flash_type, crashdump_offset);

	if (ret_val) {
		printf("crashdump data writing in flash failure\n");
		return CMD_RET_FAILURE;
	}

	printf("crashdump data writing in flash successful\n");

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(dumpipq_flash_data, 2, 0, do_dumpqca_flash_data,
	   "dumpipq_flash_data crashdump collection and storing in flash",
	   "dumpipq_flash_data [offset in flash]\n");
#endif
