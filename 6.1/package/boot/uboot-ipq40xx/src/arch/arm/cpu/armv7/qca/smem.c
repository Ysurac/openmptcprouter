/*
 * Copyright (c) 2013, 2015, 2016 The Linux Foundation. All rights reserved.
 *
 * Based on smem.c from lk.
 *
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
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
#include <common.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <asm/sizes.h>
#include <asm/arch-ipq40xx/smem.h>
#include <nand.h>
#include "ipq40xx_cdp.h"

extern int nand_env_device;
extern board_ipq40xx_params_t *gboard_param;

#define BUILD_ID_LEN 32

typedef struct smem_pmic_type
{
	unsigned pmic_model;
	unsigned pmic_die_revision;
}pmic_type;

struct qca_platfrom {
	unsigned  format;
	unsigned id;
	unsigned version;
	char     build_id[BUILD_ID_LEN];
	unsigned raw_id;
	unsigned raw_version;
	unsigned hw_platform;
	unsigned platform_version;
	unsigned accessory_chip;
	unsigned hw_platform_subtype;
	pmic_type pmic_info[3];
	unsigned foundry_id;
};

typedef enum {
	SMEM_SPINLOCK_ARRAY = 7,
	SMEM_AARM_PARTITION_TABLE = 9,
	SMEM_HW_SW_BUILD_ID = 137,
	SMEM_USABLE_RAM_PARTITION_TABLE = 402,
	SMEM_POWER_ON_STATUS_INFO = 403,
	SMEM_IMAGE_VERSION_TABLE = 469,
	SMEM_BOOT_FLASH_TYPE = 478,
	SMEM_BOOT_FLASH_INDEX = 479,
	SMEM_BOOT_FLASH_CHIP_SELECT = 480,
	SMEM_BOOT_FLASH_BLOCK_SIZE = 481,
	SMEM_BOOT_FLASH_DENSITY = 482,
	SMEM_PARTITION_TABLE_OFFSET = 483,
	SMEM_BOOT_DUALPARTINFO = 484,
	SMEM_FIRST_VALID_TYPE = SMEM_SPINLOCK_ARRAY,
	SMEM_LAST_VALID_TYPE = SMEM_BOOT_DUALPARTINFO,
	SMEM_MAX_SIZE = SMEM_BOOT_DUALPARTINFO + 1,
} smem_mem_type_t;

struct smem_proc_comm {
	unsigned command;
	unsigned status;
	unsigned data1;
	unsigned data2;
};

struct smem_heap_info {
	unsigned initialized;
	unsigned free_offset;
	unsigned heap_remaining;
	unsigned reserved;
};

struct smem_alloc_info {
	unsigned allocated;
	unsigned offset;
	unsigned size;
	unsigned reserved;
};

struct smem_machid_info {
	unsigned format;
	unsigned machid;
};

struct smem {
	struct smem_proc_comm proc_comm[4];
	unsigned version_info[32];
	struct smem_heap_info heap_info;
	struct smem_alloc_info alloc_info[SMEM_MAX_SIZE];
};

#define ALIGN_LEN	0x00000007

#define SMEM_PTN_NAME_MAX     16
#define SMEM_PTABLE_PARTS_MAX 32
#define SMEM_PTABLE_PARTS_DEFAULT 16

struct smem_ptn {
	char name[SMEM_PTN_NAME_MAX];
	unsigned start;
	unsigned size;
	unsigned attr;
} __attribute__ ((__packed__));

#define __str_fmt(x)		"%-" #x "s"
#define _str_fmt(x)		__str_fmt(x)
#define smem_ptn_name_fmt	_str_fmt(SMEM_PTN_NAME_MAX)

struct smem_ptable {
#define _SMEM_PTABLE_MAGIC_1 0x55ee73aa
#define _SMEM_PTABLE_MAGIC_2 0xe35ebddb
	unsigned magic[2];
	unsigned version;
	unsigned len;
	struct smem_ptn parts[SMEM_PTABLE_PARTS_MAX];
} __attribute__ ((__packed__));

/* partition table from SMEM */
static struct smem_ptable smem_ptable;

static struct smem *smem = (void *)(CONFIG_QCA_SMEM_BASE);

qca_smem_flash_info_t qca_smem_flash_info;
qca_smem_bootconfig_info_t qca_smem_bootconfig_info;

/**
 * smem_read_alloc_entry - reads an entry from SMEM
 * @type: the entry to read
 * @buf: the buffer location where the data will be stored
 * @len: the size of the buffer
 *
 * Reads and stores the entry in @buf. Returns 0 on success and 1 on
 * failure.
 *
 * NOTE: buf MUST be 4byte aligned, and len MUST be a multiple of 8.
 */
static unsigned smem_read_alloc_entry(smem_mem_type_t type, void *buf, int len)
{
	struct smem_alloc_info *ainfo;
	unsigned *dest = buf;
	unsigned src;
	unsigned size;


	if (((len & 0x3) != 0) || (((unsigned)buf & 0x3) != 0))
		return 1;

	if (type < SMEM_FIRST_VALID_TYPE || type > SMEM_LAST_VALID_TYPE)
	{
		return 1;
	}

	ainfo = &smem->alloc_info[type];
	if (readl(&ainfo->allocated) == 0)
	{
		return 1;
	}

	size = readl(&ainfo->size);

	if (size != (unsigned)((len + 7) & ~ALIGN_LEN))
	{
		return 1;
	}

	src = CONFIG_QCA_SMEM_BASE + readl(&ainfo->offset);
	for (; len > 0; src += 4, len -= 4)
		*(dest++) = readl(src);

	return 0;
}

/**
 * smem_ptable_init - initializes SMEM partition table
 *
 * Initialize partition table from SMEM.
 */
int smem_ptable_init(void)
{
	unsigned ret;

	ret = smem_read_alloc_entry(SMEM_AARM_PARTITION_TABLE,
				    &smem_ptable, sizeof(smem_ptable));

	if (ret != 0) {
		/*
		 * Trying for max partition 16 in case of smem_read_alloc_entry fails
		 */
		ret = smem_read_alloc_entry(SMEM_AARM_PARTITION_TABLE, &smem_ptable,
			sizeof(smem_ptable) - (sizeof(struct smem_ptn) * SMEM_PTABLE_PARTS_DEFAULT));
	}
	if (ret != 0)
		return -ENOMSG;

	if (smem_ptable.magic[0] != _SMEM_PTABLE_MAGIC_1 ||
	    smem_ptable.magic[1] != _SMEM_PTABLE_MAGIC_2)
		return -ENOMSG;

	debug("smem ptable found: ver: %d len: %d\n",
	      smem_ptable.version, smem_ptable.len);

	return 0;
}

/**
 * smem_bootconfig_info - retrieve bootconfig flags
 */
int smem_bootconfig_info(void)
{
	unsigned ret;

	ret = smem_read_alloc_entry(SMEM_BOOT_DUALPARTINFO,
		&qca_smem_bootconfig_info, sizeof(qca_smem_bootconfig_info_t));
	if (ret != 0)
		return -ENOMSG;
	if (qca_smem_bootconfig_info.magic != _SMEM_DUAL_BOOTINFO_MAGIC)
		return -ENOMSG;

	return 0;
}

unsigned int get_rootfs_active_partition(void)
{
	int i;

	for (i = 0; i < qca_smem_bootconfig_info.numaltpart; i++) {
		if (strncmp("rootfs", qca_smem_bootconfig_info.per_part_entry[i].name,
			     ALT_PART_NAME_LENGTH) == 0)
			return qca_smem_bootconfig_info.per_part_entry[i].primaryboot;
	}

	return 0; /* alt partition not available */
}

unsigned int get_partition_table_offset(void)
{
	int ret;
	uint32_t primary_mibib;

	ret = smem_read_alloc_entry(SMEM_PARTITION_TABLE_OFFSET,
					&primary_mibib, sizeof(uint32_t));
	if (ret != 0) {
		printf("smem: SMEM_PARTITION_TABLE_OFFSET not available\n");
		primary_mibib = 0;
	}

	return primary_mibib;
}

/**
 * smem_getpart - retreive partition start and size
 * @part_name: partition name
 * @start: location where the start offset is to be stored
 * @size: location where the size is to be stored
 *
 * Retreive the start offset in blocks and size in blocks, of the
 * specified partition.
 */
int smem_getpart(char *part_name, uint32_t *start, uint32_t *size)
{
	unsigned i;
	qca_smem_flash_info_t *sfi = &qca_smem_flash_info;
	struct smem_ptn *p;

	for (i = 0; i < smem_ptable.len; i++) {
		if (!strncmp(smem_ptable.parts[i].name, part_name,
			     SMEM_PTN_NAME_MAX))
			break;
	}
	if (i == smem_ptable.len)
		return -ENOENT;

	p = &smem_ptable.parts[i];

	*start = p->start;

	if (p->size == (~0u)) {
		/*
		 * Partition size is 'till end of device', calculate
		 * appropriately
		 */
		*size = (nand_info[nand_env_device].size /
				sfi->flash_block_size) - p->start;
	} else {
		*size = p->size;
	}

	return 0;
}

/**
 * smem_get_boot_flash - retreive the boot flash info
 * @flash_type: location where the flash type is to be stored
 * @flash_index: location where the flash index is to be stored
 * @flash_chip_select: location where the flash chip select is to be stored
 * @flash_block_size: location where the block size is to be stored
 * @flash_density: location where the flash size is to be stored
 *
 * Retreive the flash type and flash index, of the boot flash.
 */
int smem_get_boot_flash(uint32_t *flash_type,
			uint32_t *flash_index,
			uint32_t *flash_chip_select,
			uint32_t *flash_block_size,
			uint32_t *flash_density)
{
	int ret;

	ret = smem_read_alloc_entry(SMEM_BOOT_FLASH_TYPE,
				    flash_type, sizeof(uint32_t));
	if (ret != 0) {
		printf("smem: read flash type failed\n");
		return -ENOMSG;
	}

	ret = smem_read_alloc_entry(SMEM_BOOT_FLASH_INDEX,
				    flash_index, sizeof(uint32_t));
	if (ret != 0) {
		printf("smem: read flash index failed\n");
		return -ENOMSG;
	}

	ret = smem_read_alloc_entry(SMEM_BOOT_FLASH_CHIP_SELECT,
				    flash_chip_select, sizeof(uint32_t));
	if (ret != 0) {
		printf("smem: read flash chip select failed\n");
		return -ENOMSG;
	}

	ret = smem_read_alloc_entry(SMEM_BOOT_FLASH_BLOCK_SIZE,
				    flash_block_size, sizeof(uint32_t));
	if (ret != 0) {
		printf("smem: read flash block size failed\n");
		return -ENOMSG;
	}
	ret = smem_read_alloc_entry(SMEM_BOOT_FLASH_DENSITY,
				flash_density, sizeof(uint32_t));
	if (ret != 0) {
		printf("smem: read flash density failed\n");
		return -ENOMSG;
	}

	return 0;
}

int smem_get_build_version(char *version_name, int buf_size, int index)
{
	int ret;
	struct version_entry version_entry[32];

	ret = smem_read_alloc_entry(SMEM_IMAGE_VERSION_TABLE,
                        &version_entry, sizeof(version_entry));
	if (ret != 0)
		return -ENOMSG;

	snprintf(version_name, buf_size, "%s-%s\n",
			version_entry[index].oem_version_string,
			version_entry[index].version_string);

	return ret;
}

unsigned int smem_get_board_platform_type()
{
	struct qca_platfrom platform_type;
	unsigned int machid = 0;
	smem_read_alloc_entry(SMEM_HW_SW_BUILD_ID, &platform_type,
		sizeof(struct qca_platfrom));

	machid = ((platform_type.hw_platform << 24) |
		((SOCINFO_VERSION_MAJOR(platform_type.platform_version)) << 16) |
		((SOCINFO_VERSION_MINOR(platform_type.platform_version)) << 8) |
		(platform_type.hw_platform_subtype));

	return machid;
}

/**
 * smem_ptable_init - initializes RAM partition table from SMEM
 *
 */
int smem_ram_ptable_init(struct smem_ram_ptable *smem_ram_ptable)
{
	unsigned i;

	i = smem_read_alloc_entry(SMEM_USABLE_RAM_PARTITION_TABLE,
				smem_ram_ptable,
				sizeof(struct smem_ram_ptable));
	if (i != 0)
		return 0;

	if (smem_ram_ptable->magic[0] != _SMEM_RAM_PTABLE_MAGIC_1 ||
		smem_ram_ptable->magic[1] != _SMEM_RAM_PTABLE_MAGIC_2)
		return 0;

	printf("smem ram ptable found: ver: %d len: %d\n",
		smem_ram_ptable->version, smem_ram_ptable->len);


	return 1;
}

/*
 * get nand block size by device id.
 * dev_id is 0 for parallel nand.
 * dev_id is 1 for spi nand.
 */
uint32_t get_nand_block_size(uint8_t dev_id)
{
	struct mtd_info *mtd;

	mtd = &nand_info[dev_id];

	return mtd->erasesize;
}

/*
 * get flash block size based on partition name.
 */
static inline uint32_t get_flash_block_size(char *name,
				qca_smem_flash_info_t *smem)
{
	return (get_which_flash_param(name) == 1) ?
		get_nand_block_size(gboard_param->spi_nand_available)
		: smem->flash_block_size;
}

#define part_which_flash(p)	(((p)->attr & 0xff000000) >> 24)

static inline uint32_t get_part_block_size(struct smem_ptn *p,
					qca_smem_flash_info_t *sfi)
{
	return (part_which_flash(p) == 1) ?
			get_nand_block_size(gboard_param->spi_nand_available)
			: sfi->flash_block_size;
}

void qca_set_part_entry(char *name, qca_smem_flash_info_t *smem,
		qca_part_entry_t *part, uint32_t start, uint32_t size)
{
	uint32_t bsize = get_flash_block_size(name, smem);

	part->offset = ((loff_t)start) * bsize;
	part->size = ((loff_t)size) * bsize;
}

uint32_t qca_smem_get_flash_block_size(void)
{
	return qca_smem_flash_info.flash_block_size;
}

static char parts[4096];

char *qca_smem_part_to_mtdparts(char *mtdid)
{
	qca_smem_flash_info_t *sfi = &qca_smem_flash_info;
	int i, ret;
	int len = sizeof(parts);
	int device_id = 0;
	char *part = parts, *unit;
	int init = 0;
	uint32_t bsize;

	ret = snprintf(part, len, "%s:", mtdid);
	part += ret;
	len -= ret;

	for (i = 0; i < smem_ptable.len && len > 0; i++) {
		struct smem_ptn *p = &smem_ptable.parts[i];
		loff_t psize;
		bsize = get_part_block_size(p, sfi);

		if (part_which_flash(p) && init == 0) {
			device_id = (gboard_param->spi_nand_available == 1) ? 1 : 0;
			ret = snprintf(part, len, ";nand%d:", device_id);
			part += ret;
			len -= ret;
			init = 1;
		}
		if (p->size == (~0u)) {
			/*
			 * Partition size is 'till end of device', calculate
			 * appropriately
			 */
			psize = (nand_info[nand_env_device].size
					- (((loff_t)p->start) * bsize));
		} else {
			psize =  ((loff_t)p->size) * bsize;
		}

		if ((psize > SZ_1M) && (((psize & (SZ_1M - 1)) == 0))) {
			psize /= SZ_1M;
			unit = "M@";
		} else if ((psize > SZ_1K) && (((psize & (SZ_1K - 1)) == 0))) {
			psize /= SZ_1K;
			unit = "K@";
		} else {
			unit = "@";
		}

		ret = snprintf(part, len, "%lld%s0x%llx(%s),", psize, unit,
				((loff_t)p->start) * bsize, p->name);
		part += ret;
		len -= ret;
	}

	if (i == 0)
		return NULL;

	*part = 0;	/* Remove the trailing comma */

	return parts;
}

/*
 * retrieve the which_flash flag based on partition name.
 * flash_var is 1 if partition is in NAND.
 * flash_var is 0 if partition is in NOR.
 */
unsigned int get_which_flash_param(char *part_name)
{
	int i;
	int flash_var = 0;

	for (i = 0; i < smem_ptable.len; i++) {
		struct smem_ptn *p = &smem_ptable.parts[i];
		if (strcmp(p->name, part_name) == 0)
			flash_var = part_which_flash(p);
	}

	return flash_var;
}

int do_smeminfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	qca_smem_flash_info_t *sfi = &qca_smem_flash_info;
	int i;
	uint32_t bsize;

	printf(	"flash_type:		0x%x\n"
		"flash_index:		0x%x\n"
		"flash_chip_select:	0x%x\n"
		"flash_block_size:	0x%x\n"
		"flash_density:		0x%x\n"
		"partition table offset	0x%x\n",
			sfi->flash_type, sfi->flash_index,
			sfi->flash_chip_select, sfi->flash_block_size,
			sfi->flash_density, get_partition_table_offset());

	if (smem_ptable.len > 0) {
		printf("%-3s: " smem_ptn_name_fmt " %10s %16s %16s\n",
			"No.", "Name", "Attributes", "Start", "Size");
	} else {
		printf("Partition information not available\n");
	}

	for (i = 0; i < smem_ptable.len; i++) {
		struct smem_ptn *p = &smem_ptable.parts[i];
		loff_t psize;
		bsize = get_part_block_size(p, sfi);

		if (p->size == (~0u)) {
			/*
			 * Partition size is 'till end of device', calculate
			 * appropriately
			 */
			psize = nand_info[nand_env_device].size
				- (((loff_t)p->start) * bsize);
		} else {
			psize =  ((loff_t)p->size) * bsize;
		}

		printf("%3d: " smem_ptn_name_fmt " 0x%08x %#16llx %#16llx\n",
			i, p->name, p->attr, ((loff_t)p->start) *
			bsize, psize);
	}
	return 0;
}

U_BOOT_CMD(
	smeminfo,    1,    1,    do_smeminfo,
	"print SMEM FLASH information",
	"\n    - print flash details gathered from SMEM\n"
);
