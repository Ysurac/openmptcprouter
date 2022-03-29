/*
 * Copyright (c) 2011-2012 The Chromium OS Authors.
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/getopt.h>
#include <asm/sections.h>
#include <asm/state.h>

#include <os.h>

int sandbox_early_getopt_check(void)
{
	struct sandbox_state *state = state_get_current();
	struct sb_cmdline_option **sb_opt = __u_boot_sandbox_option_start;
	size_t num_options = __u_boot_sandbox_option_count();
	size_t i;
	int max_arg_len, max_noarg_len;

	/* parse_err will be a string of the faulting option */
	if (!state->parse_err)
		return 0;

	if (strcmp(state->parse_err, "help")) {
		printf("u-boot: error: failed while parsing option: %s\n"
			"\ttry running with --help for more information.\n",
			state->parse_err);
		os_exit(1);
	}

	printf(
		"u-boot, a command line test interface to U-Boot\n\n"
		"Usage: u-boot [options]\n"
		"Options:\n");

	max_arg_len = 0;
	for (i = 0; i < num_options; ++i)
		max_arg_len = max(strlen(sb_opt[i]->flag), max_arg_len);
	max_noarg_len = max_arg_len + 7;

	for (i = 0; i < num_options; ++i) {
		struct sb_cmdline_option *opt = sb_opt[i];

		/* first output the short flag if it has one */
		if (opt->flag_short >= 0x100)
			printf("      ");
		else
			printf("  -%c, ", opt->flag_short);

		/* then the long flag */
		if (opt->has_arg)
			printf("--%-*s", max_noarg_len, opt->flag);
		else
			printf("--%-*s <arg> ", max_arg_len, opt->flag);

		/* finally the help text */
		printf("  %s\n", opt->help);
	}

	os_exit(0);
}

static int sb_cmdline_cb_help(struct sandbox_state *state, const char *arg)
{
	/* just flag to sandbox_early_getopt_check to show usage */
	return 1;
}
SB_CMDLINE_OPT_SHORT(help, 'h', 0, "Display help");

int sandbox_main_loop_init(void)
{
	struct sandbox_state *state = state_get_current();

	/* Execute command if required */
	if (state->cmd) {
		run_command(state->cmd, 0);
		os_exit(state->exit_type);
	}

	return 0;
}

static int sb_cmdline_cb_command(struct sandbox_state *state, const char *arg)
{
	state->cmd = arg;
	return 0;
}
SB_CMDLINE_OPT_SHORT(command, 'c', 1, "Execute U-Boot command");

int main(int argc, char *argv[])
{
	struct sandbox_state *state;
	int err;

	err = state_init();
	if (err)
		return err;

	state = state_get_current();
	if (os_parse_args(state, argc, argv))
		return 1;

	/*
	 * Do pre- and post-relocation init, then start up U-Boot. This will
	 * never return.
	 */
	board_init_f(0);
}
