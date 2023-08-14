#include <command.h>
#include <common.h>
#include <linux/string.h>

#define QCA8075_PHY_CNT 4

extern void qca8075_phy_set_power(int phy, u8 power);

int do_phy_power_set(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int phy;
	int power;

	if (argc != 3) {
		return CMD_RET_USAGE;
	}

	power = argv[2][0] - '0';

	if (!strcmp(argv[1], "all")) {
		for (phy = 0; phy < QCA8075_PHY_CNT; phy++) {
			qca8075_phy_set_power(phy, power);
		}
	} else {
		phy = argv[1][0] - '0';

		if (phy < 0 || phy > QCA8075_PHY_CNT) {
			return CMD_RET_USAGE;
		}

		qca8075_phy_set_power(phy, power);
	}

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(phy_power_set, 3, 1, do_phy_power_set,
	"set phy power state",
	"id\n"
	"	- phy to operate on. \"all\" or an integer\n"
	"power\n"
	"	- value to set the power to\n"
);