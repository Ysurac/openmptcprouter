/*
 * Copyright (c) 2013-2014 The Linux Foundation. All rights reserved.
 */

#include <common.h>
#include <command.h>
#include <image.h>
#include <asm/arch-ipq806x/smem.h>

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
	{ "WLAN_FW.BIN",  0x41400000, 0x000FFF80, 0 },
	{ "WLAN_FW_900B.BIN", 0x44000000, 0x00600000, 0 },
	{ "EBICS0.BIN",   0x40000000, 0x20000000, 0 },
	{ "EBI1CS1.BIN",  0x60000000, 0x20000000, 0 }
};

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
		printf("Using directory %s in TFTP server\n", dumpdir);
	} else {
		printf("Env 'dumpdir' not set. Using / dir in TFTP server\n");
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
