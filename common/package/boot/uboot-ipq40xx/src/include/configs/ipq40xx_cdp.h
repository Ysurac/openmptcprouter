/*
 *
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _IPQ40XX_CDP_H
#define _IPQ40XX_CDP_H

#if !defined(DO_DEPS_ONLY) || defined(DO_SOC_DEPS_ONLY)
/*
 * Beat the system! tools/scripts/make-asm-offsets uses
 * the following hard-coded define for both u-boot's
 * ASM offsets and platform specific ASM offsets :(
 */
#include <generated/generic-asm-offsets.h>
#ifdef __ASM_OFFSETS_H__
#undef __ASM_OFFSETS_H__
#endif
#if !defined(DO_SOC_DEPS_ONLY)
#include <generated/asm-offsets.h>
#endif
#endif /* !DO_DEPS_ONLY */

#define CONFIG_IPQ40XX
#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_SYS_NO_FLASH
#define CONFIG_SYS_CACHELINE_SIZE	64
#define CONFIG_SKIP_LOWLEVEL_INIT
#define CONFIG_SYS_HZ			1000
#define CONFIG_SYS_VSNPRINTF

#define CONFIG_IPQ40XX_USB
#ifdef CONFIG_IPQ40XX_USB
#define CONFIG_USB_XHCI
#define CONFIG_CMD_USB
#define CONFIG_DOS_PARTITION
#define CONFIG_USB_STORAGE
#define CONFIG_SYS_USB_XHCI_MAX_ROOT_PORTS	2
#define CONFIG_USB_MAX_CONTROLLER_COUNT		2
#endif

#define CONFIG_QCOM_UART
#define CONFIG_CONS_INDEX		1
#define CONFIG_QCOM_BAM			1

#define CONFIG_ENV_OVERWRITE
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{4800, 9600, 19200, 38400, 57600,\
								115200}
#define V_PROMPT			"(IPQ40xx) # "
#define CONFIG_SYS_PROMPT		V_PROMPT
#define CONFIG_SYS_CBSIZE		(256 * 2) /* Console I/O Buffer Size */
#define CONFIG_SYS_MAXARGS		16
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
						sizeof(CONFIG_SYS_PROMPT) + 16)

#define CONFIG_CPU_CONTEXT_DUMP_BASE	0x87B00000
#define CONFIG_CPU_CONTEXT_DUMP_SIZE	0x70000
#define CONFIG_TLV_DUMP_SIZE		0x10000

#define CONFIG_SYS_SDRAM_BASE		0x80000000
#define CONFIG_SYS_TEXT_BASE		0x87300000
#define CONFIG_SYS_SDRAM_SIZE		0x10000000
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SYS_TEXT_BASE + 0x100000 - GENERATED_GBL_DATA_SIZE)
#define CONFIG_MAX_RAM_BANK_SIZE	CONFIG_SYS_SDRAM_SIZE
#define CONFIG_SYS_LOAD_ADDR		(CONFIG_SYS_SDRAM_BASE + (64 << 20))
#define CONFIG_DTB_LOAD_ADDR		(CONFIG_SYS_SDRAM_BASE + (96 << 20))
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_OF_LIBFDT		1
#define CONFIG_OF_BOARD_SETUP		1

/*
 * IPQ_TFTP_MAX_ADDR: Starting address of UBoot load/Execution region.
 * TFTP file can be written upto IPQ_TFTP_MAX_ADDR.
 */
#define IPQ_TFTP_MAX_ADDR		0x87000000

/*
 * CONFIG_TZ_END_ADDR: Ending address of Trust Zone.
 * TFTP file can also be written from CONFIG_TZ_END_ADDR
 * to CONFIG_SYS_SDRAM_END.
 */
#define CONFIG_TZ_END_ADDR		0x88000000
#define CONFIG_SYS_SDRAM_END		(CONFIG_SYS_SDRAM_BASE + gd->ram_size)
#define CONFIG_IPQ40XX_I2C
#ifdef CONFIG_IPQ40XX_I2C
#define CONFIG_CMD_I2C
#define CONFIG_SYS_I2C_SPEED	0
#endif

#define CONFIG_IPQ40XX_PCI
#ifdef CONFIG_IPQ40XX_PCI
#define CONFIG_PCI
#define CONFIG_CMD_PCI
#define CONFIG_PCI_SCAN_SHOW
#endif

#ifndef __ASSEMBLY__
#include <compiler.h>
extern loff_t board_env_offset;
extern loff_t board_env_range;
extern loff_t board_env_size;

/*
 * XXX XXX Please do not instantiate this structure. XXX XXX
 * This is just a convenience to avoid
 *      - adding #defines for every new reservation
 *      - updating the multiple associated defines like smem base,
 *        kernel start etc...
 *      - re-calculation of the defines if the order changes or
 *        some reservations are deleted
 * For new reservations just adding a member to the structure should
 * suffice.
 * Ensure that the size of this structure matches with the definition
 * of the following IPQ40xx compile time definitions
 *      PHYS_OFFSET     (linux-sources/arch/arm/mach-msm/Kconfig)
 *      zreladdr        (linux-sources/arch/arm/mach-msm/Makefile.boot)
 *      CONFIG_SYS_INIT_SP_ADDR defined above should point to the bottom.
 *      MSM_SHARED_RAM_PHYS (linux-sources/arch/arm/mach-msm/board-ipq40xx.c)
 *
 */
#if !defined(DO_DEPS_ONLY) || defined(DO_SOC_DEPS_ONLY)
typedef struct {
	uint8_t uboot[1024 * 1024 - GENERATED_GBL_DATA_SIZE];	/* ~1MB */
	uint8_t init_stack[GENERATED_GBL_DATA_SIZE];
	uint8_t sbl[1024 * 1024];				/* 1 MB */
	uint8_t cnss_debug[6 * 1024 * 1024];			/* 6 MB */
	uint8_t tz_apps[3 * 1024 * 1024];			/* 3 MB */
	uint8_t smem[512 * 1024];				/* 512 KB */
	uint8_t tz[1536 * 1024];				/* 1.5 MB */
} __attribute__ ((__packed__)) qca_mem_reserve_t;

#define QCA_MEM_RESERVE_SIZE(x)		sizeof(((qca_mem_reserve_t *)0)->x)
#define QCA_MEM_RESERVE_BASE(x)		\
	(CONFIG_SYS_TEXT_BASE + \
	 ((uint32_t)&(((qca_mem_reserve_t *)0)->x)))
#endif

#define CONFIG_QCA_SMEM_BASE		0x87e00000
#define QCA_KERNEL_START_ADDR	\
	(CONFIG_SYS_SDRAM_BASE + GENERATED_QCA_RESERVE_SIZE)

#define QCA_DRAM_KERNEL_SIZE	\
	(CONFIG_SYS_SDRAM_SIZE - GENERATED_QCA_RESERVE_SIZE)

#define QCA_BOOT_PARAMS_ADDR	(QCA_KERNEL_START_ADDR + 0x100)
#endif

#define DEVICE_MODEL_MANIFEST 	"rutx"
#define QCA_ROOT_FS_PART_NAME           "rootfs"
#define QCA_ROOT_FS_ALT_PART_NAME       QCA_ROOT_FS_PART_NAME "_1"

/* Environment */
#define CONFIG_IPQ40XX_ENV
#define CONFIG_ARCH_CPU_INIT
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_CMD_SAVEENV
#define CONFIG_BOARD_LATE_INIT
#define CONFIG_ENV_OFFSET		board_env_offset
#define CONFIG_ENV_SIZE_MAX		(64 << 10) /* 64 KB  0:APPSBLENV -> 0x10000 */
#define CONFIG_ENV_RANGE		board_env_size
#define CONFIG_ENV_SIZE			board_env_range
#define CONFIG_SYS_MALLOC_LEN	(2048 << 10)

#define CONFIG_CMD_MEMORY
#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE + 0x1300000
#define CONFIG_SYS_MEMTEST_END		CONFIG_SYS_MEMTEST_START + 0x100
#define CONFIG_CMD_SOURCE		1
#define CONFIG_INITRD_TAG		1
#define CONFIG_FIT
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_NULLDEV
#define CONFIG_CMD_XIMG
#define CONFIG_CMD_NET
#define CONFIG_IPQ_ETH_INIT_DEFER

/* L1 cache line size is 64 bytes, L2 cache line size is 128 bytes
 * Cache flush and invalidation based on L1 cache, so the cache line
 * size is configured to 64 */
#define CONFIG_SYS_CACHELINE_SIZE	64

/* CMDS */
#define	CONFIG_CMD_RUN
#define CONFIG_CMD_HTTPD

/*
 * NAND Flash Configs
 */

/* CONFIG_QPIC_NAND: QPIC NAND in BAM mode
 * CONFIG_IPQ_NAND: QPIC NAND in FIFO/block mode.
 * BAM is enabled by default.
 */
#define CONFIG_QPIC_NAND
#define CONFIG_CMD_NAND
#define CONFIG_CMD_NAND_YAFFS
#define CONFIG_CMD_MEMORY
#define CONFIG_SYS_NAND_SELF_INIT
#define CONFIG_SYS_NAND_ONFI_DETECTION

/*
 * Expose SPI driver as a pseudo NAND driver to make use
 * of U-Boot's MTD framework.
 */
#define CONFIG_SYS_MAX_NAND_DEVICE	(CONFIG_IPQ_MAX_NAND_DEVICE + \
					CONFIG_IPQ_MAX_SPI_DEVICE)

#define CONFIG_IPQ_MAX_SPI_DEVICE	2
#define CONFIG_IPQ_MAX_NAND_DEVICE	1

#define CONFIG_IPQ_NAND_NAND_INFO_IDX	0
#define CONFIG_QPIC_NAND_NAND_INFO_IDX	0
#define CONFIG_IPQ_SPI_NAND_INFO_IDX	1
#define CONFIG_IPQ_SPI_NOR_INFO_IDX	2

#define CONFIG_FDT_FIXUP_PARTITIONS

/*
 * SPI Flash Configs
 */

#define CONFIG_QCA_SPI
#define CONFIG_SPI_FLASH
#define CONFIG_CMD_SF
#define CONFIG_SPI_FLASH_STMICRO
#define CONFIG_SPI_FLASH_SPANSION
#define CONFIG_SPI_FLASH_WINBOND
#define CONFIG_SPI_FLASH_MACRONIX
#define CONFIG_SPI_FLASH_GIGA
#define CONFIG_SPI_NOR_GENERIC
#define CONFIG_IPQ40XX_SPI

#define CONFIG_SF_DEFAULT_SPEED	1000000
#define CONFIG_SF_DEFAULT_BUS 0
#define CONFIG_SF_DEFAULT_CS 0
#define CONFIG_SF_SPI_NAND_CS 1
#define CONFIG_SF_DEFAULT_MODE SPI_MODE_0
#define CONFIG_IPQ40XX_XIP	1

#define CONFIG_QUP_SPI_USE_DMA 1

#define CONFIG_SPI_NAND_GIGA 1
#define CONFIG_SPI_NAND_ATO
#define CONFIG_SPI_NAND_MACRONIX
#define CONFIG_SPI_NAND_WINBOND
/*
 * ESS Configs
 */
#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_IPQ40XX_ESS	1
#define CONFIG_IPQ40XX_EDMA	1
#define CONFIG_NET_RETRY_COUNT		5
#define CONFIG_SYS_RX_ETH_BUFFER	16
#define CONFIG_IPQ40XX_MDIO	1
#define CONFIG_QCA8075_PHY	1
#define CONFIG_QCA8033_PHY	1
#define CONFIG_MII
#define CONFIG_CMD_MII
#define CONFIG_IPADDR		192.168.1.1
#define CONFIG_IPQ_NO_MACS	2


#define CONFIG_SHIFT_REG

/*
 * CRASH DUMP ENABLE
 */

#define CONFIG_QCA_APPSBL_DLOAD	1

#ifdef CONFIG_QCA_APPSBL_DLOAD
#define CONFIG_CMD_TFTPPUT
/* We will be uploading very big files */
#undef CONFIG_NET_RETRY_COUNT
#define CONFIG_NET_RETRY_COUNT 500
#endif
#define CONFIG_CMD_ECHO

/*
 * Expose SPI driver as a pseudo NAND driver to make use
 * of U-Boot's MTD framework.
 */
#define CONFIG_SYS_MAX_NAND_DEVICE	(CONFIG_IPQ_MAX_NAND_DEVICE + \
					 CONFIG_IPQ_MAX_SPI_DEVICE)

#define CONFIG_QCA_MMC

#ifdef CONFIG_QCA_MMC
#define CONFIG_CMD_MMC
#define CONFIG_MMC
#define CONFIG_EFI_PARTITION
#define CONFIG_GENERIC_MMC
#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_SYS_MMC_ENV_DEV  0
#endif


#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS

#define CONFIG_RBTREE		/* for ubi */
#define CONFIG_CMD_UBI
#define CONFIG_BOOTCOMMAND	"bootipq"
#define CONFIG_BOOTDELAY	2
#define CONFIG_IPQ_FDT_HIGH	0x87000000
#define CONFIG_LZMA

#define CONFIG_UBOOT_START 				0xf0000
#define CONFIG_UBOOT_SIZE 				0x80000


#define CONFIG_ART_SIZE 				0x10000

#define CONFIG_LOADADDR 				0x81000000

#define CONFIG_MNF_INFO_START				0x2F0000
#define CONFIG_LINUX_PASSWD_OFS				0xA0
#define CONFIG_LINUX_PASSWD_LEN				0x6A

#define WEBFAILSAFE_UPLOAD_ART_SIZE_IN_BYTES		CONFIG_ART_SIZE

// U-Boot partition size and offset
#define WEBFAILSAFE_UPLOAD_UBOOT_ADDRESS		CONFIG_LOAD_ADDR
//#define UPDATE_SCRIPT_UBOOT_SIZE_IN_BYTES		"0x80000"

// Firmware partition offset
#define WEBFAILSAFE_UPLOAD_KERNEL_ADDRESS		CFG_KERN_ADDR

// ART partition size and offset
#define WEBFAILSAFE_UPLOAD_ART_ADDRESS			CFG_FACTORY_ADDR

// Loading to range 0x81000000 - 0x87000000
#define WEBFAILSAFE_UPLOAD_FW_SIZE_IN_BYTES	(96 * 1024 *1024) // 0x6000000

// progress state info
#define WEBFAILSAFE_PROGRESS_START			0
#define WEBFAILSAFE_PROGRESS_TIMEOUT			1
#define WEBFAILSAFE_PROGRESS_UPLOAD_READY		2
#define WEBFAILSAFE_PROGRESS_UPGRADE_READY		3
#define WEBFAILSAFE_PROGRESS_UPGRADE_FAILED		4

// update type
#define WEBFAILSAFE_UPGRADE_TYPE_FIRMWARE		0
#define WEBFAILSAFE_UPGRADE_TYPE_UBOOT			1
#define WEBFAILSAFE_UPGRADE_TYPE_ART			2
#define WEBFAILSAFE_UPGRADE_TYPE_QSDK_FIRMWARE		3

#define CONFIG_MAX_UBOOT_SIZE				CONFIG_UBOOT_SIZE

#define CONFIG_SYS_LONGHELP
#define CONFIG_AUTO_COMPLETE

#define CONFIG_MNFINFO_SUPPORT
#define CONFIG_CMD_MNFINFO

#endif /* _IPQCDP_H */
