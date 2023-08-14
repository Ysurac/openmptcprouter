/*
 * Copyright (c) 2013-2014 The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <command.h>
#include <image.h>
#include <nand.h>
#include <errno.h>
#include <asm/arch-ipq806x/smem.h>
#include <asm/arch-ipq806x/scm.h>
#include <linux/mtd/ubi.h>
#include <part.h>
#include "../../../../../board/qcom/common/qca_common.h"

#define img_addr		((void *)CONFIG_SYS_LOAD_ADDR)
#define CE1_REG_USAGE		(0)
#define CE1_ADM_USAGE		(1)
#define CE1_RESOURCE		(1)

DECLARE_GLOBAL_DATA_PTR;
static int debug = 0;
static ipq_smem_flash_info_t *sfi = &ipq_smem_flash_info;
int ipq_fs_on_nand, rootfs_part_avail = 1;
extern board_ipq806x_params_t *gboard_param;
#ifdef CONFIG_IPQ_MMC
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

typedef struct {
	unsigned int resource;
	unsigned int channel_id;
} switch_ce_chn_buf_t;

kernel_img_info_t kernel_img_info;

#ifdef CONFIG_IPQ_LOAD_NSS_FW
/**
 * check if the image and its header is valid and move it to
 * load address as specified in the header
 */
static int load_nss_img(const char *runcmd, char *args, int argslen,
						int nsscore)
{
	char cmd[128];
	int ret;

	if (debug)
		printf(runcmd);

	if ((ret = run_command(runcmd, 0)) != CMD_RET_SUCCESS) {
		return ret;
	}

	snprintf(cmd, sizeof(cmd), "bootm start 0x%x; bootm loados", CONFIG_SYS_LOAD_ADDR);

	if (debug)
		printf(cmd);

	if ((ret = run_command(cmd, 0)) != CMD_RET_SUCCESS) {
		return ret;
	}

	if (args) {
		snprintf(args, argslen, "qca-nss-drv.load%d=0x%x,"
				"qca-nss-drv.entry%d=0x%x,"
				"qca-nss-drv.string%d=\"%.*s\"",
				nsscore, image_get_load(img_addr),
				nsscore, image_get_ep(img_addr),
				nsscore, IH_NMLEN, image_get_name(img_addr));
	}

	return ret;
}

#endif /* CONFIG_IPQ_LOAD_NSS_FW */

/*
 * Set the root device and bootargs for mounting root filesystem.
 */
static int set_fs_bootargs(int *fs_on_nand)
{
	char *bootargs;
#ifdef CONFIG_IPQ_MMC
	char emmc_rootfs[30];
	block_dev_desc_t *blk_dev = mmc_get_dev(host->dev_num);
	disk_partition_t disk_info;
	int pos;
#endif
	unsigned int active_part = 0;

#define nand_rootfs	"ubi.mtd=" IPQ_ROOT_FS_PART_NAME " root=mtd:ubi_rootfs rootfstype=squashfs"
#define nor_rootfs	"root=mtd:" IPQ_ROOT_FS_PART_NAME " rootfstype=squashfs"

	if (sfi->flash_type == SMEM_BOOT_SPI_FLASH) {

		if (sfi->rootfs.offset == 0xBAD0FF5E) {
			rootfs_part_avail = 0;
			/*
			 * While booting out of SPI-NOR, not having a
			 * 'rootfs' entry in the partition table implies
			 * that the Root FS is available in the NAND flash
			 */
			bootargs = nand_rootfs;
			*fs_on_nand = 1;
			if (smem_bootconfig_info() == 0) {
				active_part = get_rootfs_active_partition();
				sfi->rootfs.offset = active_part * IPQ_NAND_ROOTFS_SIZE;
			} else {
				sfi->rootfs.offset = 0;
			}
			sfi->rootfs.size = IPQ_NAND_ROOTFS_SIZE;
		} else {
			bootargs = nor_rootfs;
			*fs_on_nand = 0;
		}
	} else if (sfi->flash_type == SMEM_BOOT_NAND_FLASH) {
		bootargs = nand_rootfs;
		*fs_on_nand = 1;
#ifdef CONFIG_IPQ_MMC
	} else if (sfi->flash_type == SMEM_BOOT_MMC_FLASH) {
		if (smem_bootconfig_info() == 0) {
			active_part = get_rootfs_active_partition();
			if (active_part) {
				pos = find_part_efi(blk_dev, IPQ_ROOT_FS_ALT_PART_NAME, &disk_info);
			} else {
				pos = find_part_efi(blk_dev, IPQ_ROOT_FS_PART_NAME, &disk_info);
			}
		} else {
			pos = find_part_efi(blk_dev, IPQ_ROOT_FS_PART_NAME, &disk_info);
		}
		if (pos > 0) {
			snprintf(emmc_rootfs, sizeof(emmc_rootfs),
				"root=/dev/mmcblk0p%d", pos);
			bootargs = emmc_rootfs;
			*fs_on_nand = 0;
		}
#endif
	} else {
		printf("bootipq: unsupported boot flash type\n");
		return -EINVAL;
	}

	if (getenv("fsbootargs") == NULL)
		setenv("fsbootargs", bootargs);

	return run_command("setenv bootargs ${bootargs} ${fsbootargs}", 0);
}

/**
 * Inovke the dump routine and in case of failure, do not stop unless the user
 * requested to stop
 */
static int inline do_dumpipq_data()
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

static int switch_ce_channel_buf(unsigned int channel_id)
{
	int ret;
	switch_ce_chn_buf_t ce1_chn_buf;

	ce1_chn_buf.resource   = CE1_RESOURCE;
	ce1_chn_buf.channel_id = channel_id;

	ret = scm_call(SCM_SVC_TZ, CE_CHN_SWITCH_CMD, &ce1_chn_buf,
		sizeof(switch_ce_chn_buf_t), NULL, 0);

	return ret;
}

/**
 * Load the Kernel image in mbn format and transfer control to kernel.
 */
static int do_bootmbn(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
#ifdef CONFIG_IPQ_LOAD_NSS_FW
	char bootargs[IH_NMLEN+32];
#endif
	char runcmd[256];
	int ret;
	unsigned int request;
#ifdef CONFIG_IPQ_MMC
	block_dev_desc_t *blk_dev = mmc_get_dev(host->dev_num);
	disk_partition_t disk_info;
	unsigned int active_part = 0;
#endif

#ifdef CONFIG_IPQ_APPSBL_DLOAD
	unsigned long * dmagic1 = (unsigned long *) 0x2A03F000;
	unsigned long * dmagic2 = (unsigned long *) 0x2A03F004;
#endif

	if (argc == 2 && strncmp(argv[1], "debug", 5) == 0)
		debug = 1;

#ifdef CONFIG_IPQ_APPSBL_DLOAD
	/* check if we are in download mode */
	if (*dmagic1 == 0xE47B337D && *dmagic2 == 0x0501CAB0) {
		/* clear the magic and run the dump command */
		*dmagic1 = 0;
		*dmagic2 = 0;
		uint64_t etime = get_timer_masked() + (10 * CONFIG_SYS_HZ);

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

#ifdef CONFIG_IPQ806X_PCI
	board_pci_deinit();
#endif /* CONFIG_IPQ806X_PCI */

	if ((ret = set_fs_bootargs(&ipq_fs_on_nand)))
		return ret;

	/* check the smem info to see which flash used for booting */
	if (sfi->flash_type == SMEM_BOOT_SPI_FLASH) {
		if (debug) {
			printf("Using nand device 1\n");
		}
		run_command("nand device 1", 0);
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

#ifdef CONFIG_IPQ_LOAD_NSS_FW
	/* check the smem info to see whether the partition size is valid.
	 * refer board/qcom/ipq806x_cdp/ipq806x_cdp.c:ipq_get_part_details
	 * for more details
	 */
	if (sfi->nss[0].size != 0xBAD0FF5E) {
		snprintf(runcmd, sizeof(runcmd), "nand read 0x%x 0x%llx 0x%llx",
				CONFIG_SYS_LOAD_ADDR,
				sfi->nss[0].offset, sfi->nss[0].size);

		if (load_nss_img(runcmd, bootargs, sizeof(bootargs), 0)
				!= CMD_RET_SUCCESS)
			return CMD_RET_FAILURE;

		if (getenv("nssbootargs0") == NULL)
			setenv("nssbootargs0", bootargs);

		run_command("setenv bootargs ${bootargs} ${nssbootargs0}", 0);
	}

	if (sfi->nss[1].size != 0xBAD0FF5E) {
		snprintf(runcmd, sizeof(runcmd), "nand read 0x%x 0x%llx 0x%llx",
				CONFIG_SYS_LOAD_ADDR,
				sfi->nss[1].offset, sfi->nss[1].size);

		if (load_nss_img(runcmd, bootargs, sizeof(bootargs), 1)
				!= CMD_RET_SUCCESS)
			return CMD_RET_FAILURE;

		if (getenv("nssbootargs1") == NULL)
			setenv("nssbootargs1", bootargs);

		run_command("setenv bootargs ${bootargs} ${nssbootargs1}", 0);
	}
#endif /* CONFIG_IPQ_LOAD_NSS_FW */

	if (debug) {
		run_command("printenv bootargs", 0);
		printf("Booting from flash\n");
	}

	request = CONFIG_SYS_LOAD_ADDR;
	kernel_img_info.kernel_load_addr = request;

	if (ipq_fs_on_nand) {

		/*
		 * The kernel will be available inside a UBI volume
		 */
		snprintf(runcmd, sizeof(runcmd),
			"set mtdids nand0=nand0 && "
			"set mtdparts mtdparts=nand0:0x%llx@0x%llx(fs),${msmparts} && "
			"ubi part fs && "
			"ubi read 0x%x kernel && ", sfi->rootfs.size, sfi->rootfs.offset,
			request);


		if (debug)
			printf(runcmd);

		if (run_command(runcmd, 0) != CMD_RET_SUCCESS)
			return CMD_RET_FAILURE;

		kernel_img_info.kernel_load_size =
			(unsigned int)ubi_get_volume_size("kernel");

#ifdef CONFIG_IPQ_MMC
	} else if (sfi->flash_type == SMEM_BOOT_MMC_FLASH) {
		if (smem_bootconfig_info() == 0) {
			active_part = get_rootfs_active_partition();
			if (active_part) {
				ret = find_part_efi(blk_dev, "kernel_1", &disk_info);
			} else {
				ret = find_part_efi(blk_dev, "kernel", &disk_info);
			}
		} else {
			ret = find_part_efi(blk_dev, "kernel", &disk_info);
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
			"set mtdids nand1=nand1 && "
			"set mtdparts mtdparts=nand1:${msmparts} && "
			"nand read 0x%x 0x%llx 0x%llx",
			request, sfi->hlos.offset, sfi->hlos.size);

		if (debug)
			printf(runcmd);

		if (run_command(runcmd, 0) != CMD_RET_SUCCESS)
			return CMD_RET_FAILURE;

		kernel_img_info.kernel_load_size =  sfi->hlos.size;
	}

	request += sizeof(mbn_header_t);

	/* This sys call will switch the CE1 channel to register usage */
	ret = switch_ce_channel_buf(CE1_REG_USAGE);

	if (ret)
		return CMD_RET_FAILURE;

	ret = scm_call(SCM_SVC_BOOT, KERNEL_AUTH_CMD, &kernel_img_info,
		sizeof(kernel_img_info_t), NULL, 0);

	if (ret) {
		printf("Kernel image authentication failed \n");
		BUG();
	}

	/*
	 * This sys call will switch the CE1 channel to ADM usage
	 * so that HLOS can use it.
	 */
	ret = switch_ce_channel_buf(CE1_ADM_USAGE);

	if (ret)
		return CMD_RET_FAILURE;

	snprintf(runcmd, sizeof(runcmd), "bootm 0x%x\n", request);

	if (debug)
		printf(runcmd);

#ifdef CONFIG_IPQ_MMC
	board_mmc_deinit();
#endif

	if (run_command(runcmd, 0) != CMD_RET_SUCCESS) {
#ifdef CONFIG_IPQ_MMC
		mmc_initialize(gd->bd);
#endif
		return CMD_RET_FAILURE;
	}

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(bootmbn, 2, 0, do_bootmbn,
	   "bootmbn from flash device",
	   "bootmbn [debug] - Load image(s) and boots the kernel\n");

/**
 * Load the NSS images and Kernel image and transfer control to kernel
 */
static int do_bootipq(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
#ifdef CONFIG_IPQ_LOAD_NSS_FW
	char bootargs[IH_NMLEN+32];
#endif
	char runcmd[256];
	int nandid = 0, ret;
	unsigned int active_part = 0;

#ifdef CONFIG_IPQ_MMC
	block_dev_desc_t *blk_dev = mmc_get_dev(host->dev_num);
	disk_partition_t disk_info;
#endif

#ifdef CONFIG_IPQ_APPSBL_DLOAD
	unsigned long * dmagic1 = (unsigned long *) 0x2A03F000;
	unsigned long * dmagic2 = (unsigned long *) 0x2A03F004;
#endif

	if (argc == 2 && strncmp(argv[1], "debug", 5) == 0)
		debug = 1;

#ifdef CONFIG_IPQ_APPSBL_DLOAD
	/* check if we are in download mode */
	if (*dmagic1 == 0xE47B337D && *dmagic2 == 0x0501CAB0) {
		/* clear the magic and run the dump command */
		*dmagic1 = 0;
		*dmagic2 = 0;
		uint64_t etime = get_timer_masked() + (10 * CONFIG_SYS_HZ);

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

#ifdef CONFIG_IPQ806X_PCI
	board_pci_deinit();
#endif /* CONFIG_IPQ806X_PCI */

	if ((ret = set_fs_bootargs(&ipq_fs_on_nand)))
		return ret;

	/* check the smem info to see which flash used for booting */
	if (sfi->flash_type == SMEM_BOOT_SPI_FLASH) {
		nandid = 1;
		if (debug) {
			printf("Using nand device 1\n");
		}
		run_command("nand device 1", 0);
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

#ifdef CONFIG_IPQ_LOAD_NSS_FW
	/* check the smem info to see whether the partition size is valid.
	 * refer board/qcom/ipq806x_cdp/ipq806x_cdp.c:ipq_get_part_details
	 * for more details
	 */
	if (sfi->nss[0].size != 0xBAD0FF5E) {
		snprintf(runcmd, sizeof(runcmd), "nand read 0x%x 0x%llx 0x%llx",
				CONFIG_SYS_LOAD_ADDR,
				sfi->nss[0].offset, sfi->nss[0].size);

		if (load_nss_img(runcmd, bootargs, sizeof(bootargs), 0)
				!= CMD_RET_SUCCESS)
			return CMD_RET_FAILURE;

		if (getenv("nssbootargs0") == NULL)
			setenv("nssbootargs0", bootargs);

		run_command("setenv bootargs ${bootargs} ${nssbootargs0}", 0);
	}

	if (sfi->nss[1].size != 0xBAD0FF5E) {
		snprintf(runcmd, sizeof(runcmd), "nand read 0x%x 0x%llx 0x%llx",
				CONFIG_SYS_LOAD_ADDR,
				sfi->nss[1].offset, sfi->nss[1].size);

		if (load_nss_img(runcmd, bootargs, sizeof(bootargs), 1)
				!= CMD_RET_SUCCESS)
			return CMD_RET_FAILURE;

		if (getenv("nssbootargs1") == NULL)
			setenv("nssbootargs1", bootargs);

		run_command("setenv bootargs ${bootargs} ${nssbootargs1}", 0);
	}
#endif /* CONFIG_IPQ_LOAD_NSS_FW */

	if (debug) {
		run_command("printenv bootargs", 0);
		printf("Booting from flash\n");
	}

	if (ipq_fs_on_nand) {

		/*
		 * The kernel will be available inside a UBI volume
		 */
		snprintf(runcmd, sizeof(runcmd),
			"set mtdids nand0=nand0 && "
			"set mtdparts mtdparts=nand0:0x%llx@0x%llx(fs),${msmparts} && "
			"ubi part fs && "
			"ubi read 0x%x kernel && "
			"bootm 0x%x\n", sfi->rootfs.size, sfi->rootfs.offset,
				CONFIG_SYS_LOAD_ADDR, CONFIG_SYS_LOAD_ADDR);
#ifdef CONFIG_IPQ_MMC
	} else if (sfi->flash_type == SMEM_BOOT_MMC_FLASH) {
		if (smem_bootconfig_info() == 0) {
			active_part = get_rootfs_active_partition();
			if (active_part) {
				ret = find_part_efi(blk_dev, "kernel_1", &disk_info);
			} else {
				ret = find_part_efi(blk_dev, "kernel", &disk_info);
			}
		} else {
			ret = find_part_efi(blk_dev, "kernel", &disk_info);
		}

		if (ret > 0) {
			snprintf(runcmd, sizeof(runcmd), "mmc read 0x%x 0x%X 0x%X",
					CONFIG_SYS_LOAD_ADDR,
					(uint)disk_info.start, (uint)disk_info.size);

			if (run_command(runcmd, 0) != CMD_RET_SUCCESS)
				return CMD_RET_FAILURE;

			snprintf(runcmd, sizeof(runcmd),"bootm 0x%x\n",
						CONFIG_SYS_LOAD_ADDR);
		}
#endif
	} else {

		/*
		 * Kernel is in a separate partition
		 */
		snprintf(runcmd, sizeof(runcmd),
			/* NOR is treated as psuedo NAND */
			"set mtdids nand1=nand1 && "
			"set mtdparts mtdparts=nand1:${msmparts} && "
			"set autostart yes;"
			"nboot 0x%x %d 0x%llx", CONFIG_SYS_LOAD_ADDR,
				nandid, sfi->hlos.offset);
	}

	if (debug)
		printf(runcmd);

#ifdef CONFIG_IPQ_MMC
	board_mmc_deinit();
#endif

	if (run_command(runcmd, 0) != CMD_RET_SUCCESS) {
#ifdef CONFIG_IPQ_MMC
		mmc_initialize(gd->bd);
#endif
		return CMD_RET_FAILURE;
	}

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(bootipq, 2, 0, do_bootipq,
	   "bootipq from flash device",
	   "bootipq [debug] - Load image(s) and boots the kernel\n");
