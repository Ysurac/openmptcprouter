#include <common.h>
#include <command.h>

static void firstboot_wipe_ubifs(const char *mtdpart)
{
	char cmd[256];

	printf("\n  Wiping %s_2 partition...\n\n", mtdpart);

	snprintf(cmd, sizeof(cmd),
		 "ubi part %s && "
		 /* 'ubi write' with size=0 erases the partition */
		 "ubi write 0 rootfs_data 0",
		 mtdpart);

	if (run_command(cmd, 0) != CMD_RET_SUCCESS) {
		printf("Failed to wipe partition\n");
	}
}

static int do_firstboot(struct cmd_tbl_s *cmdtbl, int flag, int argc, char * const argv[])
{
	int ret;

	printf("\n  Reseting device...\n");

	ret = run_command("setenv mtdids nand1=nand1 && "
			  "setenv mtdparts mtdparts=nand1:128m(ubi0),128m(ubi1)",
			  0);
	if (ret != CMD_RET_SUCCESS) {
		printf("Failed to mount mtd partitions\n");
		return CMD_RET_FAILURE;
	}

	firstboot_wipe_ubifs("ubi0");
	firstboot_wipe_ubifs("ubi1");

	printf("\n  Reset complete!\n\n");

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(firstboot, 2, 0, do_firstboot, "Reset device to factory settings\n",
	   "\n"
	   "    - Reset device to factory settings\n"
	   "firstboot p\n"
	   "    - Reset device and linux password \n");