#include <common.h>

#ifdef CONFIG_CMD_MNFINFO

#include <command.h>
#include <mnf_info.h>

typedef enum {
	MNFCMD_CONFIRM = 1, // Don't apply any changes
	MNFCMD_FORCE   = 2, // Always overwrite mnf fields when restoring.
} mnf_cmd_flags;

typedef struct {
	mnf_cmd_flags bit;
	const char *name;
} mnf_cmd_flag_map_t;

static mnf_cmd_flag_map_t mnf_cmd_flag_map[] = { 
        { MNFCMD_CONFIRM, "CONFIRM" },
	{ MNFCMD_FORCE, "FORCE" },
	{ 0 } 
};

#define MNFINFO_PRINT(...)                                                                                   \
	do {                                                                                                 \
		static char __mnf_buf[256];                                                                  \
		snprintf(__mnf_buf, sizeof(__mnf_buf), __VA_ARGS__);                                         \
		printf("%s\n", __mnf_buf);                                                                   \
	} while (0)

#define MNFINFO_RESULT_OK(...)                                                                               \
	do {                                                                                                 \
		static char __mnf_buf[256];                                                                  \
		snprintf(__mnf_buf, sizeof(__mnf_buf), __VA_ARGS__);                                         \
		printf("%s\n", __mnf_buf);                                                                   \
	} while (0)

#define MNFINFO_RESULT_ERR(...)                                                                              \
	do {                                                                                                 \
		static char __mnf_buf[256];                                                                  \
		snprintf(__mnf_buf, sizeof(__mnf_buf), __VA_ARGS__);                                         \
		printf("ERROR: %s\n", __mnf_buf);                                                            \
	} while (0)

#define RET_MNFINFO_RESULT_USAGE(...)                                                                        \
	do {                                                                                                 \
		print_usage_help();                                                                          \
		MNFINFO_RESULT_ERR(__VA_ARGS__);                                                             \
		return 1;                                                                                    \
	} while (0)

static int print_usage_help(void)
{
	MNFINFO_PRINT("");
	MNFINFO_PRINT("Usage: mnf <command> [args...]");
	MNFINFO_PRINT("");
	MNFINFO_PRINT("  Available commands:");
	MNFINFO_PRINT("    list                          - list available mnf fields");
	MNFINFO_PRINT("    show                          - show values of all mnf fields");
	MNFINFO_PRINT("    get <name>                    - display value of an mnf field");
	MNFINFO_PRINT("    set <name> <value> [flags...] - set the value of an mnf field");
	MNFINFO_PRINT("    restore <name> [flags...]     - restore mnf field to a default value");
	MNFINFO_PRINT("    repair [flags...]             - restore all mnf fields");
	MNFINFO_PRINT("");
	MNFINFO_PRINT("  Available flags:");
	MNFINFO_PRINT("    CONFIRM                       - Do not ask for confirmation");
	MNFINFO_PRINT("    FORCE                         - Ignore validity checks when restoring");
	MNFINFO_PRINT("");
	return 1;
}

static int mnf_cmd_parse_flags(int argc, char *const argv[], mnf_cmd_flags *result)
{
	*result = 0;

	for (int i = 0; i < argc; i++) {
		mnf_cmd_flags bit = 0;

		for (mnf_cmd_flag_map_t *flag = mnf_cmd_flag_map; flag->bit != 0; flag++) {
			if (!strcmp(argv[i], flag->name))
				bit = flag->bit;
		}

		if (!bit)
                        RET_MNFINFO_RESULT_USAGE("Invalid flag '%s'", argv[i]);

		*result |= bit;
	}

	return 0;
}

static int mnf_cmd_get_field(const char *name, mnf_field_t **result)
{
	if (strlen(name) == 1) {
		*result = mnf_get_field_info_short(name[0]);
	} else {
		*result = mnf_get_field_info_long(name);
	}

	if (!*result)
		RET_MNFINFO_RESULT_USAGE("Invalid mnf field '%s'", name);

	return 0;
}

static int mnf_cmd_read_field(mnf_field_t *field, char *buf)
{
	if (mnf_flash_read(field, buf)) {
		MNFINFO_RESULT_ERR("Could not read mnf info");
		return 1;
	}
	return 0;
}

static int mnf_cmd_write_init(void)
{
	if (mnf_flash_write_init()) {
		MNFINFO_RESULT_ERR("Could not initialize flash for writing");
		return 1;
	}
	return 0;
}

static int mnf_cmd_write_field(mnf_field_t *field, const char *data)
{
	if (mnf_flash_write(field, data)) {
		MNFINFO_RESULT_ERR("Could not update flash data");
		return 1;
	}
	return 0;
}

static int mnf_cmd_write_finalize(mnf_cmd_flags flags)
{
	if (!(flags & MNFCMD_CONFIRM)) {
		char input[256];

		MNFINFO_PRINT("");
		MNFINFO_PRINT("Warning!");
		MNFINFO_PRINT("Changing mnf info may break your device!");
                MNFINFO_PRINT("Proceed only if you know what you are doing.");
		MNFINFO_PRINT("");

		readline_into_buffer("Type `yes` to continue: ", input, 0);

		if (strcmp(input, "yes")) {
			printf("Write aborted\n");
			return 1;
		}
	}

	if (mnf_flash_write_finalize()) {
		MNFINFO_RESULT_ERR("Could not write to flash");
		return 1;
	}

	return 0;
}

static int mnf_cmd_repair_field(mnf_field_t *field, mnf_cmd_flags flags)
{
	char old[256], new[256], oldf[256], newf[256];

	if (mnf_cmd_read_field(field, old))
		return 1;

	mnf_field_to_str(field, old, oldf);

	if (field->restore_func(field, oldf, newf) || flags & MNFCMD_FORCE) { // field is invalid
		MNFINFO_PRINT(" %16s - restore to '%s'", field->description, newf);
		mnf_str_to_field(field, newf, new);

		if (mnf_cmd_write_field(field, new))
			return 1;

	} else { // field is valid, skip
		MNFINFO_PRINT(" %16s - valid, keeping", field->description);
	}

	return 0;
}

static int do_list(int argc, char *const argv[])
{
	if (argc > 0)
		RET_MNFINFO_RESULT_USAGE("'list' takes 0 arguments");

	MNFINFO_PRINT("    Argument        Description");
	MNFINFO_PRINT(" ====================================");

	for (mnf_field_t *field = mnf_fields; field->short_name != '\0'; field++) {
		MNFINFO_PRINT("    %c, %-10s - %s", field->short_name, field->long_name, field->description);
	}

	MNFINFO_RESULT_OK("");

	return 0;
}

static int do_show(int argc, char *const argv[])
{
	if (argc > 0)
		RET_MNFINFO_RESULT_USAGE("'show' takes 0 arguments");

	MNFINFO_PRINT(" %20s  %s", "Description", "Value");
	MNFINFO_PRINT(" ==========================================");

	for (mnf_field_t *field = mnf_fields; field->short_name != '\0'; field++) {
		char buf[256];
		char formated[256];

		if (mnf_cmd_read_field(field, buf))
			return 1;

		mnf_field_to_str(field, buf, formated);

		MNFINFO_PRINT(" %20s: %s", field->description, formated);
	}

	MNFINFO_RESULT_OK("");

	return 0;
}

static int do_get(int argc, char *const argv[])
{
	mnf_field_t *field;
	char buf[256];
	char formated[256];

	if (argc != 1)
		RET_MNFINFO_RESULT_USAGE("'get' takes 1 argument");

	if (mnf_cmd_get_field(argv[0], &field))
		return 1;

	if (mnf_cmd_read_field(field, buf))
		return 1;

	mnf_field_to_str(field, buf, formated);

	MNFINFO_RESULT_OK("%s", formated);
	return 0;
}

static int do_set(int argc, char *const argv[])
{
	mnf_cmd_flags flags;
	mnf_field_t *field;
	char buf[256];

	if (argc < 2)
		RET_MNFINFO_RESULT_USAGE("'set' takes at least 2 arguments");;

	if (mnf_cmd_parse_flags(argc - 2, argv + 2, &flags))
		return 1;

	if (mnf_cmd_get_field(argv[0], &field))
		return 1;

	if (mnf_str_to_field(field, argv[1], buf)) {
		MNFINFO_RESULT_ERR("Invalid mnf value");
		return 1;
	}

	if (mnf_cmd_write_init())
		return 1;

	if (mnf_cmd_write_field(field, buf))
		return 1;

	if (mnf_cmd_write_finalize(flags))
		return 1;

	MNFINFO_RESULT_OK("%s updated successfully", field->description);

	return 0;
}

static int do_repair(int argc, char *const argv[])
{
	mnf_cmd_flags flags;

	if (mnf_cmd_parse_flags(argc, argv, &flags))
		return 1;

	if (mnf_cmd_write_init())
		return 1;

	MNFINFO_PRINT("Repairing mnf values:");

	for (mnf_field_t *field = mnf_fields; field->short_name != '\0'; field++) {
		if (mnf_cmd_repair_field(field, flags))
			return 1;
	}

	if (mnf_cmd_write_finalize(flags))
		return 1;

	MNFINFO_RESULT_OK("Repair complete");

	return 0;
}

static int do_restore(int argc, char *const argv[])
{
	mnf_cmd_flags flags;
	mnf_field_t *field;

	if (argc < 1)
		RET_MNFINFO_RESULT_USAGE("'restore' takes at least 1 argument");;

	if (mnf_cmd_parse_flags(argc - 1, argv + 1, &flags))
		return 1;

	if (mnf_cmd_get_field(argv[0], &field))
		return 1;

	if (mnf_cmd_write_init())
		return 1;

	MNFINFO_PRINT("Restoring mnf field:");

	if (mnf_cmd_repair_field(field, flags | MNFCMD_FORCE))
		return 1;

	if (mnf_cmd_write_finalize(flags))
		return 1;

	MNFINFO_RESULT_OK("%s restored successfully", field->description);

	return 0;
}

int mnf_cmd_entry(int argc, char *argv[])
{
	const char *subcmd;

	if (argc < 1)
		RET_MNFINFO_RESULT_USAGE("No command specified");

	subcmd = argv[0];

	if (!strcmp(subcmd, "list"))
		return do_list(argc - 1, argv + 1);

	if (!strcmp(subcmd, "show"))
		return do_show(argc - 1, argv + 1);

	if (!strcmp(subcmd, "get"))
		return do_get(argc - 1, argv + 1);

	if (!strcmp(subcmd, "set"))
		return do_set(argc - 1, argv + 1);

	if (!strcmp(subcmd, "repair"))
		return do_repair(argc - 1, argv + 1);

	if (!strcmp(subcmd, "restore"))
		return do_restore(argc - 1, argv + 1);

	RET_MNFINFO_RESULT_USAGE("Unrecognized command '%s'", subcmd);
}

static int do_mnfinfo(cmd_tbl_t *cmd, int flag, int argc, char *const argv[]) {
        return mnf_cmd_entry(argc - 1, argv + 1);
}

U_BOOT_CMD(mnf, 10, 0, do_mnfinfo, "Edit mnf info",
	"<command> [args...]\n"
	"\n"
	"  Available commands:\n"
	"    list                          - list available mnf fields\n"
	"    show                          - show values of all mnf fields\n"
	"    get <name>                    - display value of an mnf field\n"
	"    set <name> <value> [flags...] - set the value of an mnf field\n"
	"    restore <name> [flags...]     - restore mnf field to a default value\n"
	"    repair [flags...]             - restore all mnf fields\n"
	"\n"
	"  Available flags:\n"
	"    CONFIRM                       - Do not ask for confirmation\n"
	"    FORCE                         - Ignore validity checks when restoring\n");

#endif