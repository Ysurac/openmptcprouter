/*
 * Copyright (c) 2013-2016 The Linux Foundation. All rights reserved.
 */

#include <common.h>
#include <command.h>
#include <image.h>
#include <asm/arch-ipq806x/smem.h>
#include <asm/sizes.h>
#include <asm/arch-ipq806x/scm.h>
#include <asm/arch-ipq806x/iomap.h>
#include <asm/errno.h>
#include <nand.h>
#include <spi_flash.h>
#include <mmc.h>

struct dumpinfo_t {
	char name[16]; /* use only file name in 8.3 format */
	uint32_t start;
	uint32_t size;
	int is_aligned_access; /* non zero represent 4 byte access */
};

struct dumpinfo_t dumpinfo[] = {
	/* Note1: when aligned access is set, the contents
	 * are copied to a temporary location and so
	 * the size of region should not exceed the size
	 * of region pointed by IPQ_TEMP_DUMP_ADDR
	 *
	 * Note2: IPQ_NSSTCM_DUMP_ADDR should be the
	 * first entry */
	{ "NSSTCM.BIN",   IPQ_NSSTCM_DUMP_ADDR, 0x20000, 0 },
	{ "IMEM_A.BIN",   0x2a000000, 0x0003f000, 0 },
	{ "IMEM_C.BIN",   0x2a03f000, 0x00001000, 0 },
	{ "IMEM_D.BIN",   0x2A040000, 0x00020000, 0 },
	{ "CODERAM.BIN",  0x00020000, 0x00028000, 0 },
	{ "SPS_RAM.BIN",  0x12000000, 0x0002C000, 0 },
	{ "RPM_MSG.BIN",  0x00108000, 0x00005fff, 1 },
	{ "SPS_BUFF.BIN", 0x12040000, 0x00004000, 0 },
	{ "SPS_PIPE.BIN", 0x12800000, 0x00008000, 0 },
	{ "LPASS.BIN",    0x28400000, 0x00020000, 0 },
	{ "RPM_WDT.BIN",  0x0006206C, 0x00000004, 0 },
	{ "CPU0_WDT.BIN", 0x0208A044, 0x00000004, 0 },
	{ "CPU1_WDT.BIN", 0x0209A044, 0x00000004, 0 },
	{ "CPU0_REG.BIN", 0x39013ea8, 0x000000AC, 0 },
	{ "CPU1_REG.BIN", 0x39013f54, 0x000000AC, 0 },
	{ "WLAN_FW.BIN",  0x41400000, 0x000FFF80, 0 },
	{ "WLAN_FW_900B.BIN", 0x44000000, 0x00600000, 0 },
	{ "EBICS0.BIN",   0x40000000, 0x20000000, 0 },
	{ "EBI1CS1.BIN",  0x60000000, 0x20000000, 0 }
};

void forever(void) { while (1); }
/*
 * Set the cold/warm boot address for one of the CPU cores.
 */
int scm_set_boot_addr(void)
{
	int ret;
	struct {
		unsigned int flags;
		unsigned long addr;
	} cmd;

	cmd.addr = (unsigned long)forever;
	cmd.flags = SCM_FLAG_COLDBOOT_CPU1;

	ret = scm_call(SCM_SVC_BOOT, SCM_BOOT_ADDR,
			&cmd, sizeof(cmd), NULL, 0);
	if (ret) {
		printf("--- %s: scm_call failed ret = %d\n", __func__, ret);
	}

	return ret;
}

static int krait_release_secondary(void)
{
	writel(0xa4, CPU1_APCS_SAW2_VCTL);
	barrier();
	udelay(512);

	writel(0x109, CPU1_APCS_CPU_PWR_CTL);
	writel(0x101, CPU1_APCS_CPU_PWR_CTL);
	barrier();
	udelay(1);

	writel(0x121, CPU1_APCS_CPU_PWR_CTL);
	barrier();
	udelay(2);

	writel(0x120, CPU1_APCS_CPU_PWR_CTL);
	barrier();
	udelay(2);

	writel(0x100, CPU1_APCS_CPU_PWR_CTL);
	barrier();
	udelay(100);

	writel(0x180, CPU1_APCS_CPU_PWR_CTL);
	barrier();

	return 0;
}

static int do_dumpipq_data(cmd_tbl_t *cmdtp, int flag, int argc,
				char *const argv[])
{
	char runcmd[128];
	char *serverip = NULL;
	char *dumpdir = ""; /* dump to root of TFTP server if none specified */
	int indx = 0;
	uint32_t memaddr = 0;

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

	if (getenv("dumpdir") != NULL) {
		dumpdir = getenv("dumpdir");
		if (dumpdir != NULL)
			printf("Using directory %s in TFTP server\n", dumpdir);
	} else {
		printf("Env 'dumpdir' not set. Using / dir in TFTP server\n");
	}

	if (scm_set_boot_addr() == 0) {
		/* Pull Core-1 out of reset, iff scm call succeeds */
		krait_release_secondary();
	}


	for (indx = 0; indx < ARRAY_SIZE(dumpinfo); indx++) {
		printf("\nProcessing %s:", dumpinfo[indx].name);
		memaddr = dumpinfo[indx].start;

		if (dumpinfo[indx].is_aligned_access) {
			snprintf(runcmd, sizeof(runcmd), "cp.l 0x%x 0x%x 0x%x",
				memaddr, IPQ_TEMP_DUMP_ADDR,
				dumpinfo[indx].size);

			if (run_command(runcmd, 0) != CMD_RET_SUCCESS)
				return CMD_RET_FAILURE;

			memaddr = IPQ_TEMP_DUMP_ADDR;
		}

		snprintf(runcmd, sizeof(runcmd), "tftpput 0x%x 0x%x %s/%s",
			memaddr, dumpinfo[indx].size,
			dumpdir, dumpinfo[indx].name);
		if (run_command(runcmd, 0) != CMD_RET_SUCCESS)
			return CMD_RET_FAILURE;
		udelay(10000); /* give some delay for server */
	}

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(dumpipq_data, 2, 0, do_dumpipq_data,
	   "dumpipq_data crashdump collection from memory",
	   "dumpipq_data [serverip] - Crashdump collection from memory vi tftp\n");

#ifdef CONFIG_IPQ_APPSBL_DLOAD
#define MAX_UNAME_SIZE 1024
#define TLV_MSG_OFFSET 2048
#define PAGE_SIZE 4096
#define QCOM_WDT_SCM_TLV_TYPE_SIZE 1
#define QCOM_WDT_SCM_TLV_LEN_SIZE 2
#define QCOM_WDT_SCM_TLV_TYPE_LEN_SIZE (QCOM_WDT_SCM_TLV_TYPE_SIZE +\
						QCOM_WDT_SCM_TLV_LEN_SIZE)
#define CRASHDUMP_CPU_CONTEXT_SIZE 2048
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

#ifdef CONFIG_IPQ_MMC
/* Context for EMMC Flash memory */
struct crashdump_flash_emmc_cxt {
	loff_t start_crashdump_offset;
	loff_t cur_crashdump_offset;
	int cur_blk_data_len;
	int write_size;
	unsigned char temp_data[MAX_EMMC_BLK_LEN];
};
#endif

static ipq_smem_flash_info_t *sfi = &ipq_smem_flash_info;
static struct spi_flash *crashdump_spi_flash;
static struct crashdump_flash_nand_cxt crashdump_nand_cnxt;
static struct crashdump_flash_spi_cxt crashdump_flash_spi_cnxt;
#ifdef CONFIG_IPQ_MMC
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

#ifdef CONFIG_IPQ_MMC
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
#ifdef CONFIG_IPQ_MMC
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
	required_size = CRASHDUMP_CPU_CONTEXT_SIZE +
				crashdump_data->uname_length;

	ret = crashdump_flash_write_init(crashdump_cnxt,
			crashdump_offset,
			required_size);

	if (ret)
		return ret;

	ret = crashdump_flash_write(crashdump_cnxt,
			crashdump_data->cpu_context,
			CRASHDUMP_CPU_CONTEXT_SIZE);

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
static int do_dumpipq_flash_data(cmd_tbl_t *cmdtp, int flag, int argc,
				char *const argv[])
{
	unsigned char *kernel_crashdump_address =
		*(unsigned char **) CONFIG_IPQ_KERNEL_CRASHDUMP_ADDRESS;
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
#ifdef CONFIG_IPQ_MMC
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
	tlv_msg.msg_buffer = kernel_crashdump_address + TLV_MSG_OFFSET;
	tlv_msg.cur_msg_buffer_pos = tlv_msg.msg_buffer;
	tlv_msg.len = PAGE_SIZE - TLV_MSG_OFFSET;

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

U_BOOT_CMD(dumpipq_flash_data, 2, 0, do_dumpipq_flash_data,
	   "dumpipq_flash_data crashdump collection and storing in flash",
	   "dumpipq_flash_data [offset in flash]\n");
#endif
