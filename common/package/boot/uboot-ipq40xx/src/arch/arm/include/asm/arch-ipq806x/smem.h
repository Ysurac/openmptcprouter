#ifndef SMEM_H
#define SMEM_H

#define RAM_PARTITION_SDRAM 14
#define RAM_PARTITION_SYS_MEMORY 1
#define IPQ_NAND_ROOTFS_SIZE (64 << 20)

enum {
	SMEM_BOOT_NO_FLASH        = 0,
	SMEM_BOOT_NOR_FLASH       = 1,
	SMEM_BOOT_NAND_FLASH      = 2,
	SMEM_BOOT_ONENAND_FLASH   = 3,
	SMEM_BOOT_SDC_FLASH       = 4,
	SMEM_BOOT_MMC_FLASH       = 5,
	SMEM_BOOT_SPI_FLASH       = 6,
};

struct smem_ram_ptn {
	char name[16];
	unsigned start;
	unsigned size;

	/* RAM Partition attribute: READ_ONLY, READWRITE etc.  */
	unsigned attr;

	/* RAM Partition category: EBI0, EBI1, IRAM, IMEM */
	unsigned category;

	/* RAM Partition domain: APPS, MODEM, APPS & MODEM (SHARED) etc. */
	unsigned domain;

	/* RAM Partition type: system, bootloader, appsboot, apps etc. */
	unsigned type;

	/* reserved for future expansion without changing version number */
	unsigned reserved2, reserved3, reserved4, reserved5;
} __attribute__ ((__packed__));

struct smem_ram_ptable {
#define _SMEM_RAM_PTABLE_MAGIC_1 0x9DA5E0A8
#define _SMEM_RAM_PTABLE_MAGIC_2 0xAF9EC4E2
	unsigned magic[2];
	unsigned version;
	unsigned reserved1;
	unsigned len;
	struct smem_ram_ptn parts[32];
	unsigned buf;
} __attribute__ ((__packed__));

int smem_ptable_init(void);
int smem_get_boot_flash(uint32_t *flash_type,
			uint32_t *flash_index,
			uint32_t *flash_chip_select,
			uint32_t *flash_block_size);
int smem_getpart(char *name, uint32_t *start, uint32_t *size);
unsigned int smem_get_board_machtype(void);
int smem_ram_ptable_init(struct smem_ram_ptable *smem_ram_ptable);

typedef struct {
	loff_t offset;
	loff_t size;
} ipq_part_entry_t;

typedef struct {
	uint32_t		flash_type;
	uint32_t		flash_index;
	uint32_t		flash_chip_select;
	uint32_t		flash_block_size;
	ipq_part_entry_t	hlos;
#ifdef CONFIG_IPQ_LOAD_NSS_FW
	ipq_part_entry_t	nss[2];
#endif
	ipq_part_entry_t	rootfs;
} ipq_smem_flash_info_t;

extern ipq_smem_flash_info_t ipq_smem_flash_info;

void ipq_set_part_entry(ipq_smem_flash_info_t *sfi, ipq_part_entry_t *part,
			uint32_t start, uint32_t size);

#define ALT_PART_NAME_LENGTH 16
struct per_part_info
{
	char name[ALT_PART_NAME_LENGTH];
	uint32_t primaryboot;
	uint32_t upgraded;
};

#define NUM_ALT_PARTITION 3
typedef struct
{
#define _SMEM_DUAL_BOOTINFO_MAGIC       0xA5A3A1A0
	/* Magic number for identification when reading from flash */
	uint32_t magic;
	/* upgradeinprogress indicates to attempting the upgrade */
	uint32_t    upgradeinprogress;
	/* numaltpart indicate number of alt partitions */
	uint32_t    numaltpart;

	struct per_part_info per_part_entry[NUM_ALT_PARTITION];
} ipq_smem_bootconfig_info_t;

extern ipq_smem_bootconfig_info_t ipq_smem_bootconfig_info;

int smem_bootconfig_info(void);
unsigned int get_rootfs_active_partition(void);
unsigned int get_mibib_active_partition(void);

#endif
