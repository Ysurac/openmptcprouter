/*
 * Copyright (c) 2015, 2016 The Linux Foundation. All rights reserved.

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
*/

#include <configs/ipq40xx_cdp.h>
#include <common.h>
#include <command.h>
#include <image.h>
#include <nand.h>
#include <errno.h>
#include <asm/arch-ipq40xx/scm.h>
#include <part.h>
#include <linux/mtd/ubi.h>
#include <asm/arch-ipq40xx/smem.h>
#include <mmc.h>
#include "ipq40xx_cdp.h"

#define DLOAD_MAGIC_COOKIE	0x10
#define XMK_STR(x)		#x
#define MK_STR(x)		XMK_STR(x)

static int debug = 0;

DECLARE_GLOBAL_DATA_PTR;
static qca_smem_flash_info_t *sfi = &qca_smem_flash_info;
int ipq_fs_on_nand ;
extern int nand_env_device;
extern board_ipq40xx_params_t *gboard_param;
#ifdef CONFIG_QCA_MMC
static qca_mmc *host = &mmc_host;
#endif

typedef struct {
	unsigned int image_type;
	unsigned int header_vsn_num;
	unsigned int image_src;
	unsigned char *image_dest_ptr;
	unsigned int image_size;
	unsigned int code_size;
	unsigned char *signature_ptr;
	unsigned int signature_size;
	unsigned char *cert_chain_ptr;
	unsigned int cert_chain_size;
} mbn_header_t;

typedef struct {
	unsigned int kernel_load_addr;
	unsigned int kernel_load_size;
} kernel_img_info_t;

kernel_img_info_t kernel_img_info;

/**
 * Inovke the dump routine and in case of failure, do not stop unless the user
 * requested to stop
 */
#ifdef CONFIG_QCA_APPSBL_DLOAD
static int inline do_dumpipq_data(void)
{
	uint64_t etime;

	if (run_command("dumpipq_data", 0) != CMD_RET_SUCCESS) {
		printf("\nAuto crashdump saving failed!"
		"\nPress any key within 10s to take control of U-Boot");

		etime = get_timer_masked() + (10 * CONFIG_SYS_HZ);
		while (get_timer_masked() < etime) {
			if (tstc())
				break;
		}

		if (get_timer_masked() < etime)
			return CMD_RET_FAILURE;
	}
	return CMD_RET_SUCCESS;
}
#endif

/*
 * Set the root device and bootargs for mounting root filesystem.
 */
static int set_fs_bootargs(int *fs_on_nand)
{
	char *bootargs;
	unsigned int active_part = 0;

#define nand_rootfs	"ubi.mtd=" QCA_ROOT_FS_PART_NAME " root=mtd:ubi_rootfs rootfstype=squashfs"
#define nand_squash_on_ubi_rootfs	"ubi.mtd=" QCA_ROOT_FS_PART_NAME " ubi.block=0,1 root=/dev/ubiblock0_1 rootfstype=squashfs console=ttyMSM0,115200n8"

	if (sfi->flash_type == SMEM_BOOT_SPI_FLASH) {
		if (((sfi->rootfs.offset == 0xBAD0FF5E) &&
		(gboard_param->nor_emmc_available == 0)) ||
			get_which_flash_param("rootfs")) {
			bootargs = nand_squash_on_ubi_rootfs;
			*fs_on_nand = 1;
			gboard_param->nor_nand_available = 1;
			if (getenv("fsbootargs") == NULL)
				setenv("fsbootargs", bootargs);
		} else {
			if (smem_bootconfig_info() == 0) {
				active_part = get_rootfs_active_partition();
				if (active_part) {
					bootargs = "rootfsname=rootfs_1";
				} else {
					bootargs = "rootfsname=rootfs";
				}
			} else {
				bootargs = "rootfsname=rootfs";
			}
			*fs_on_nand = 0;
			if (getenv("fsbootargs") == NULL)
				setenv("fsbootargs", bootargs);
		}
	} else if (sfi->flash_type == SMEM_BOOT_NAND_FLASH) {
		bootargs = nand_squash_on_ubi_rootfs;
		if (getenv("fsbootargs") == NULL)
			setenv("fsbootargs", bootargs);
		*fs_on_nand = 1;
#ifdef CONFIG_QCA_MMC
	} else if (sfi->flash_type == SMEM_BOOT_MMC_FLASH) {
		if (smem_bootconfig_info() == 0) {
			active_part = get_rootfs_active_partition();
			if (active_part) {
				bootargs = "rootfsname=rootfs_1";
			} else {
				bootargs = "rootfsname=rootfs";
			}
		} else {
			bootargs = "rootfsname=rootfs";
		}

		*fs_on_nand = 0;
		if (getenv("fsbootargs") == NULL)
			setenv("fsbootargs", bootargs);
#endif
	} else {
		printf("bootipq: unsupported boot flash type\n");
		return -EINVAL;
	}

	return run_command("setenv bootargs ${bootargs} ${fsbootargs} rootwait", 0);
}

int config_select(unsigned int addr, const char **config, char *rcmd, int rcmd_size)
{
	/* Selecting a config name from the list of available
	 * config names by passing them to the fit_conf_get_node()
	 * function which is used to get the node_offset with the
	 * config name passed. Based on the return value index based
	 * or board name based config is used.
	 */

	int i;
	for (i = 0; i < MAX_CONF_NAME && config[i]; i++) {
		if (fit_conf_get_node((void *)addr, config[i]) >= 0) {
			snprintf(rcmd, rcmd_size, "bootm 0x%x#%s\n",
				addr, config[i]);
			return 0;
		}
	}
	printf("Config not availabale\n");
	return -1;
}

#include "exports.h"
#include "asm/arch-qcom-common/gpio.h"
#include "asm/arch-ipq40xx/iomap.h"
#include "asm/io.h"
#include "asm/arch-ipq40xx/timer.h"

#define GPT_FREQ       48
#define STM32_I2C_ADDR 0x74
#define SCL_PIN	       58
#define SDA_PIN	       59
#define HALF_PERIOD    3

#define SDA_PULL(SDA_PULL__state) gpio_tlmm_config(SDA_PIN, 0, 0, 0, 0, (SDA_PULL__state), 0, 0, 0)
#define SCL_PULL(SCL_PULL__state) gpio_tlmm_config(SCL_PIN, 0, 0, 0, 0, (SCL_PULL__state), 0, 0, 0)

static inline uint64_t ticks(void)
{
	uint64_t vect_hi1, vect_hi2;
	uint64_t vect_low;

	do {
		vect_hi1 = readl(GCNT_CNTCV_HI);
		vect_low = readl(GCNT_CNTCV_LO);
		vect_hi2 = readl(GCNT_CNTCV_HI);

	} while (vect_hi1 != vect_hi2);

	return ((uint64_t)vect_hi1 << 32 | vect_low);
}

static uint64_t g_wait_until;
static inline void wait_for(unsigned f)
{
	g_wait_until = (ticks() + f * GPT_FREQ) - 2 * GPT_FREQ;
}

static inline void wait(void)
{
	while (ticks() < g_wait_until)
		;
}

static inline void bitbang_write(uint8_t byte)
{
	for (int b = 7; b > -1; b--) {
		SCL_PULL(1);
		wait_for(HALF_PERIOD);
		SDA_PULL(byte >> b ^ 1);
		wait();

		SCL_PULL(0); // register the bit
		wait_for(HALF_PERIOD);
		wait();
	}

	SCL_PULL(1);
	SDA_PULL(0);
}

static int stm32_comm(int read, uint8_t *buff, unsigned buff_data_len)
{
	// deactivate pull-ups just in case
	gpio_set_value(SDA_PIN, 0);
	gpio_set_value(SCL_PIN, 0);
	//

	// start
	SDA_PULL(1);
	wait_for(HALF_PERIOD);
	wait();

	// send the addr
	bitbang_write(STM32_I2C_ADDR << 1 | read); // zeroth bit = 0 indicating write

	wait_for(HALF_PERIOD);
	wait();

	SCL_PULL(0);
	//while(!readl(GPIO_IN_OUT_ADDR(SCL_PIN)));
	wait_for(HALF_PERIOD);
	int ret = 0, nack = readl(GPIO_IN_OUT_ADDR(SDA_PIN));
	wait();

	if (nack) {
		SCL_PULL(1);
		goto stop;
	}

	for (unsigned i = 0; i < buff_data_len; i++) {
		if (read) {
			SDA_PULL(0);
			buff[i] = 0;
			for (int b = 7; b > -1; b--) {
				SCL_PULL(1);
				wait_for(HALF_PERIOD);
				wait();

				SCL_PULL(0); // register the bit
				wait_for(HALF_PERIOD);
				buff[i] |= readl(GPIO_IN_OUT_ADDR(SDA_PIN)) << b;
				wait();
			}

			SCL_PULL(1);
			SDA_PULL(i < buff_data_len - 1);

		} else {
			bitbang_write(buff[i]);
		}

		wait_for(HALF_PERIOD);
		wait();

		SCL_PULL(0);
		wait_for(HALF_PERIOD);
		if (read) {
			nack = 0;
		} else {
			nack = readl(GPIO_IN_OUT_ADDR(SDA_PIN));
		}
		wait();
		SCL_PULL(1);

		if (nack) {
			goto stop;
		}
	}

	ret = 1;
stop:
	// do the stop condition
	SDA_PULL(1);
	wait_for(HALF_PERIOD);
	wait();
	SCL_PULL(0);

	wait_for(HALF_PERIOD);
	wait();
	SDA_PULL(0);

	wait_for(HALF_PERIOD * 5); // todo:
	wait();

	return ret;
}

static int stm32_via_i2c_found(void)
{
	printf("STM32: ");

	uint8_t buff[11] = { 0x1, 0x3, 0x0, 0xfc, 0x3, 0xd2 }; // PROTO_GET_SUPPORTED

	if (!stm32_comm(0, buff, 6)) {
		goto undetected;
	}

	mdelay(28); // waiting for roughly the same amount r2ec does, read may fail otherwise

	if (!stm32_comm(1, buff, sizeof(buff)) || !buff[0] || buff[0] == 0xff) {
		goto undetected;
	}

	printf("detected:");
	for (unsigned i = 0; i < sizeof(buff); i++) {
		printf(" 0x%02X", buff[i]);
	}
	putc('\n');

	return 1;

undetected:
	printf("not detected\n");
	return 0;
}

int expecting_stm32(const char *mnf_name, const char *mnf_hwver)
{
	if (!mnf_name || !mnf_hwver || strncmp("RUTX", mnf_name, 4)) {
		return 1; // uncertain
	}
	mnf_name += 4;

	// first 2 chars are MAIN hwver, last 2 are RF...
	if (mnf_hwver[1] == '0' && mnf_hwver[0] == '0') {
		mnf_hwver += 2; // ...except on single board devices like R1
	}

	int hwver = atoi((char[]){ mnf_hwver[0], mnf_hwver[1], '\0' });

	// map of names and hwvers that no longer have stm32
	struct {
		char n[2];
		int16_t v;
	} static const map[] = {
		// todo: complete the list
		{ "08", 9 }, { "09", 9 }, { "10", 9 }, { "11", 9 }, { "12", 4 }, { "14", 3 }, { "R1", 6 },
		{ "50", 0 },
	};

	for (unsigned i = 0; i < sizeof(map) / sizeof(map[0]); i++) {
		if (!strncmp(map[i].n, mnf_name, sizeof(map[i].n))) {
			return hwver < map[i].v; // certain
		}
	}

	return 1; // uncertain
}

int conf_mdtb_select(void *addr, char *rcmd, int rcmd_size, const char **conf_name_out)
{
	const char *mnf_name = getenv("mnf_name");
	int stm32_found	     = stm32_via_i2c_found();
	if (!stm32_found && expecting_stm32(mnf_name, getenv("mnf_hwver"))) {
		for (unsigned i = 0; i < 10; i++) { // keep retrying for max 5 secs
			mdelay(500);
			if ((stm32_found = stm32_via_i2c_found())) {
				break;
			}
		}
	}

	char desc[16];

	if (mnf_name && strlen(mnf_name) >= 6) {
		strncpy(desc, mnf_name + 4, 2);
	} else {
		strcpy(desc, "08"); // fallback
		printf("\x1b[31mMDTB failed: MNF data seems incorrect\x1b[0m\n");
		goto bootm;
	}

	if (stm32_found) {
		strcat(desc, "_STM32");
	}

	static char name[16], *conf_name = name;
	for (unsigned i = 1; i < 100; i++) {
		sprintf(name, "conf_mdtb@%d", i);

		int offset = fit_conf_get_node(addr, name);
		char *dtb_desc;
		if (offset < 0 || fit_get_desc(addr, offset, &dtb_desc)) {
			break;
		}
		
		if (!strcmp(desc, dtb_desc)) {
			goto bootm;
		}
	}

	printf("\x1b[31mMDTB failed: suitable DTB not found\x1b[0m\n");
	conf_name = "config@5"; // fallback #2: matyt neperrase NAND'o vel, ypac jei praeitos loop i = 1

bootm:
	if (conf_name_out) {
		*conf_name_out = conf_name;
	}

	if (rcmd) {
		return rcmd_size <= snprintf(rcmd, rcmd_size, "bootm %p#%s\n", addr, conf_name);
	}

	return 0;
}

static int do_boot_signedimg(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
#ifdef CONFIG_QCA_APPSBL_DLOAD
	uint64_t etime;
	volatile u32 val;
#endif
	char runcmd[256];
	int ret;
	unsigned int request;
#ifdef CONFIG_QCA_MMC
	block_dev_desc_t *blk_dev;
	disk_partition_t disk_info;
	unsigned int active_part = 0;
#endif

	if (argc == 2 && strncmp(argv[1], "debug", 5) == 0)
		debug = 1;

#ifdef CONFIG_QCA_APPSBL_DLOAD

	ret = scm_call(SCM_SVC_BOOT, SCM_SVC_RD, NULL,
			0, (void *)&val, sizeof(val));
	/* check if we are in download mode */
	if (val == DLOAD_MAGIC_COOKIE) {
#ifdef CONFIG_IPQ_ETH_INIT_DEFER
		puts("\nNet:   ");
		eth_initialize(gd->bd);
#endif
		/* clear the magic and run the dump command */
		val = 0x0;
		ret = scm_call(SCM_SVC_BOOT, SCM_SVC_WR,
			(void *)&val, sizeof(val), NULL, 0);
		if (ret)
			printf ("Error in reseting the Magic cookie\n");

		etime = get_timer_masked() + (10 * CONFIG_SYS_HZ);

		printf("\nCrashdump magic found."
			"\nHit any key within 10s to stop dump activity...");
		while (!tstc()) {       /* while no incoming data */
			if (get_timer_masked() >= etime) {
				if (do_dumpipq_data() == CMD_RET_FAILURE)
					return CMD_RET_FAILURE;
				break;
			}
		}

		/* reset the system, some images might not be loaded
		 * when crashmagic is found
		 */
		run_command("reset", 0);
	}
#endif
	if ((ret = set_fs_bootargs(&ipq_fs_on_nand)))
		return ret;

	/* check the smem info to see which flash used for booting */
	if (sfi->flash_type == SMEM_BOOT_SPI_FLASH) {
		if (debug) {
			printf("Using nand device 2\n");
		}
		run_command("nand device 2", 0);
	} else if (sfi->flash_type == SMEM_BOOT_NAND_FLASH) {
		if (debug) {
			printf("Using nand device 0\n");
		}
	} else if (sfi->flash_type == SMEM_BOOT_MMC_FLASH) {
		if (debug) {
			printf("Using MMC device\n");
		}
	} else {
		printf("Unsupported BOOT flash type\n");
		return -1;
	}
	if (debug) {
		run_command("printenv bootargs", 0);
		printf("Booting from flash\n");
	}

	request = CONFIG_SYS_LOAD_ADDR;
	kernel_img_info.kernel_load_addr = request;

	if (ipq_fs_on_nand) {
		if (sfi->rootfs.offset == 0xBAD0FF5E) {
			sfi->rootfs.offset = 0;
			sfi->rootfs.size = IPQ_NAND_ROOTFS_SIZE;
		}
		/*
		 * The kernel will be available inside a UBI volume
		 */
		snprintf(runcmd, sizeof(runcmd),
			"nand device %d && "
			"set mtdids nand%d=nand%d && "
			"set mtdparts mtdparts=nand%d:0x%llx@0x%llx(fs),${msmparts} && "
			"ubi part fs && "
			"ubi read 0x%x kernel && ", gboard_param->spi_nand_available,
			gboard_param->spi_nand_available,
			gboard_param->spi_nand_available,
			gboard_param->spi_nand_available,
			sfi->rootfs.size, sfi->rootfs.offset,
			request);

		if (debug)
			printf("runcmd: %s\n", runcmd);

		if (run_command(runcmd, 0) != CMD_RET_SUCCESS)
			return CMD_RET_FAILURE;

		kernel_img_info.kernel_load_size =
			(unsigned int)ubi_get_volume_size("kernel");
#ifdef CONFIG_QCA_MMC
	} else if (sfi->flash_type == SMEM_BOOT_MMC_FLASH || (gboard_param->nor_emmc_available == 1)) {
		blk_dev = mmc_get_dev(host->dev_num);
		if (smem_bootconfig_info() == 0) {
			active_part = get_rootfs_active_partition();
			if (active_part) {
				ret = find_part_efi(blk_dev, "0:HLOS_1", &disk_info);
			} else {
				ret = find_part_efi(blk_dev, "0:HLOS", &disk_info);
			}
		} else {
			ret = find_part_efi(blk_dev, "0:HLOS", &disk_info);
		}

		if (ret > 0) {
			snprintf(runcmd, sizeof(runcmd), "mmc read 0x%x 0x%X 0x%X",
					CONFIG_SYS_LOAD_ADDR,
					(uint)disk_info.start, (uint)disk_info.size);

			if (run_command(runcmd, 0) != CMD_RET_SUCCESS)
				return CMD_RET_FAILURE;

			kernel_img_info.kernel_load_size = disk_info.size * disk_info.blksz;
		}
#endif
	} else {
		/*
		 * Kernel is in a separate partition
		 */
		snprintf(runcmd, sizeof(runcmd),
			/* NOR is treated as psuedo NAND */
			"nand read 0x%x 0x%llx 0x%llx && ",
			request, sfi->hlos.offset, sfi->hlos.size);

		if (debug)
			printf("runcmd: %s\n", runcmd);

		if (run_command(runcmd, 0) != CMD_RET_SUCCESS)
			return CMD_RET_FAILURE;

		kernel_img_info.kernel_load_size =  sfi->hlos.size;
	}

	request += sizeof(mbn_header_t);

	ret = scm_call(SCM_SVC_BOOT, KERNEL_AUTH_CMD, &kernel_img_info,
		sizeof(kernel_img_info_t), NULL, 0);

	if (ret) {
		printf("Kernel image authentication failed \n");
		BUG();
	}

	dcache_enable();

	ret = config_select(request, gboard_param->dtb_config_name,
				runcmd, sizeof(runcmd));

	if (debug)
		printf("runcmd: %s\n", runcmd);

#ifdef CONFIG_QCA_MMC
	board_mmc_deinit();
#endif

	if (ret < 0 || run_command(runcmd, 0) != CMD_RET_SUCCESS) {
#ifdef CONFIG_QCA_MMC
	mmc_initialize(gd->bd);
#endif
		dcache_disable();
		return CMD_RET_FAILURE;
	}

	return CMD_RET_SUCCESS;
}

static int do_boot_unsignedimg(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
#ifdef CONFIG_QCA_APPSBL_DLOAD
	uint64_t etime;
	volatile u32 val;
#endif
	int ret;
	char runcmd[256];
#ifdef CONFIG_QCA_MMC
	block_dev_desc_t *blk_dev;
	disk_partition_t disk_info;
	unsigned int active_part = 0;
#endif

	if (argc == 2 && strncmp(argv[1], "debug", 5) == 0)
		debug = 1;
#ifdef CONFIG_QCA_APPSBL_DLOAD
	ret = scm_call(SCM_SVC_BOOT, SCM_SVC_RD, NULL,
			0, (void *)&val, sizeof(val));
	/* check if we are in download mode */
	if (val == DLOAD_MAGIC_COOKIE) {
#ifdef CONFIG_IPQ_ETH_INIT_DEFER
		puts("\nNet:   ");
		eth_initialize(gd->bd);
#endif
		/* clear the magic and run the dump command */
		val = 0x0;
		ret = scm_call(SCM_SVC_BOOT, SCM_SVC_WR,
			(void *)&val, sizeof(val), NULL, 0);
		if (ret)
			printf ("Error in reseting the Magic cookie\n");

		etime = get_timer_masked() + (10 * CONFIG_SYS_HZ);

		printf("\nCrashdump magic found."
			"\nHit any key within 10s to stop dump activity...");
		while (!tstc()) {       /* while no incoming data */
			if (get_timer_masked() >= etime) {
				if (do_dumpipq_data() == CMD_RET_FAILURE)
					return CMD_RET_FAILURE;
				break;
			}
		}
		/* reset the system, some images might not be loaded
		 * when crashmagic is found
		 */
		run_command("reset", 0);
	}
#endif

	if ((ret = set_fs_bootargs(&ipq_fs_on_nand)))
		return ret;

	if (debug) {
		run_command("printenv bootargs", 0);
		printf("Booting from flash\n");
	}

	if (sfi->flash_type == SMEM_BOOT_NAND_FLASH) {
		if (debug) {
			printf("Using nand device 0\n");
		}

		/*
		 * The kernel is in seperate partition
		 */
		if (sfi->rootfs.offset == 0xBAD0FF5E) {
			printf(" bad offset of hlos");
			return -1;
		}

		snprintf(runcmd, sizeof(runcmd),
			"set mtdids nand0=nand0 && "
			"set mtdparts mtdparts=nand0:0x%llx@0x%llx(fs),${msmparts} && "
			"ubi part fs && "
			"ubi read 0x%x kernel && ",
			sfi->rootfs.size, sfi->rootfs.offset,
			CONFIG_SYS_LOAD_ADDR);

	} else if ((sfi->flash_type == SMEM_BOOT_SPI_FLASH) && (gboard_param->nor_emmc_available == 0)) {
		if ((sfi->rootfs.offset == 0xBAD0FF5E) ||
			get_which_flash_param("rootfs")) {
			if (sfi->rootfs.offset == 0xBAD0FF5E) {
				sfi->rootfs.offset = 0;
				sfi->rootfs.size = IPQ_NAND_ROOTFS_SIZE;
			}
			snprintf(runcmd, sizeof(runcmd),
				"nand device %d && "
				"set mtdids nand%d=nand%d && "
				"set mtdparts mtdparts=nand%d:0x%llx@0x%llx(fs),${msmparts} && "
				"ubi part fs && "
				"ubi read 0x%x kernel && ",
				gboard_param->spi_nand_available,
				gboard_param->spi_nand_available,
				gboard_param->spi_nand_available,
				gboard_param->spi_nand_available,
				sfi->rootfs.size, sfi->rootfs.offset,
				CONFIG_SYS_LOAD_ADDR);
		} else {
			/*
			 * Kernel is in a separate partition
			 */
			snprintf(runcmd, sizeof(runcmd),
				"sf probe &&"
				"sf read 0x%x 0x%x 0x%x && ",
				CONFIG_SYS_LOAD_ADDR, (uint)sfi->hlos.offset, (uint)sfi->hlos.size);
		}
#ifdef CONFIG_QCA_MMC
	} else if ((sfi->flash_type == SMEM_BOOT_MMC_FLASH) || (gboard_param->nor_emmc_available == 1)) {
		if (debug) {
			printf("Using MMC device\n");
		}
		blk_dev = mmc_get_dev(host->dev_num);
		if (smem_bootconfig_info() == 0) {
			active_part = get_rootfs_active_partition();
			if (active_part) {
				ret = find_part_efi(blk_dev, "0:HLOS_1", &disk_info);
			} else {
				ret = find_part_efi(blk_dev, "0:HLOS", &disk_info);
			}
		} else {
			ret = find_part_efi(blk_dev, "0:HLOS", &disk_info);
		}

		if (ret > 0) {
			snprintf(runcmd, sizeof(runcmd), "mmc read 0x%x 0x%x 0x%x",
					CONFIG_SYS_LOAD_ADDR,
					(uint)disk_info.start, (uint)disk_info.size);
		}

#endif   	/* CONFIG_QCA_MMC   */
	} else {
		printf("Unsupported BOOT flash type\n");
		return -1;
	}

	if (run_command(runcmd, 0) != CMD_RET_SUCCESS) {
#ifdef CONFIG_QCA_MMC
		mmc_initialize(gd->bd);
#endif
		return CMD_RET_FAILURE;
	}

	dcache_enable();

	ret = genimg_get_format((void *)CONFIG_SYS_LOAD_ADDR);
	if (ret == IMAGE_FORMAT_FIT) {
		if (!conf_mdtb_select((void *)CONFIG_SYS_LOAD_ADDR, runcmd, sizeof(runcmd), NULL))
			ret = 0;
		else
		ret = config_select(CONFIG_SYS_LOAD_ADDR, gboard_param->dtb_config_name,
				runcmd, sizeof(runcmd));
	} else if (ret == IMAGE_FORMAT_LEGACY) {
		snprintf(runcmd, sizeof(runcmd),
			"bootm 0x%x\n", CONFIG_SYS_LOAD_ADDR);
	} else {
		ret = genimg_get_format((void *)CONFIG_SYS_LOAD_ADDR +
					sizeof(mbn_header_t));
		if (ret == IMAGE_FORMAT_FIT) {
			ret = config_select((CONFIG_SYS_LOAD_ADDR + sizeof(mbn_header_t)),
					gboard_param->dtb_config_name,
					runcmd, sizeof(runcmd));
		} else if (ret == IMAGE_FORMAT_LEGACY) {
			snprintf(runcmd, sizeof(runcmd),
				"bootm 0x%x\n", (CONFIG_SYS_LOAD_ADDR +
				sizeof(mbn_header_t)));
		} else {
			dcache_disable();
			return CMD_RET_FAILURE;
		}
	}

	if (ret < 0 || run_command(runcmd, 0) != CMD_RET_SUCCESS) {
		dcache_disable();
		return CMD_RET_FAILURE;
	}
	return CMD_RET_SUCCESS;
}

static int do_bootipq(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int ret;
	char buf;
	/*
	 * set fdt_high parameter so that u-boot will not load
	 * dtb above CONFIG_IPQ40XX_FDT_HIGH region.
	 */
	if (run_command("set fdt_high " MK_STR(CONFIG_IPQ_FDT_HIGH) "\n", 0)
			!= CMD_RET_SUCCESS) {
		return CMD_RET_FAILURE;
	}

	ret = scm_call(SCM_SVC_FUSE, QFPROM_IS_AUTHENTICATE_CMD,
				NULL, 0, &buf, sizeof(char));

	if (ret == 0 && buf == 1) {
		return do_boot_signedimg(cmdtp, flag, argc, argv);
	} else if (ret == 0 || ret == -EOPNOTSUPP) {
		return do_boot_unsignedimg(cmdtp, flag, argc, argv);
	}

	return CMD_RET_FAILURE;
}

U_BOOT_CMD(bootipq, 2, 0, do_bootipq,
	   "bootipq from flash device",
	   "bootipq [debug] - Load image(s) and boots the kernel\n");
