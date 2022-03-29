/*
 * Code for setting up command line flags like `./u-boot --help`
 *
 * Copyright (c) 2011 The Chromium OS Authors.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef __SANDBOX_GETOPT_H
#define __SANDBOX_GETOPT_H

struct sandbox_state;

/*
 * Internal structure for storing details about the flag.
 * Most people should not have to dig around in this as
 * it only gets parsed by the core sandbox code.  End
 * consumer code should focus on the macros below and
 * the callback function.
 */
struct sb_cmdline_option {
	/* The long flag name: "help" for "--help" */
	const char *flag;
	/* The (optional) short flag name: "h" for "-h" */
	int flag_short;
	/* The help string shown to the user when processing --help */
	const char *help;
	/* Whether this flag takes an argument */
	int has_arg;
	/* Callback into the end consumer code with the option */
	int (*callback)(struct sandbox_state *state, const char *opt);
};

/*
 * Internal macro to expand the lower macros into the necessary
 * magic junk that makes this all work.
 */
#define _SB_CMDLINE_OPT(f, s, ha, h) \
	static struct sb_cmdline_option sb_cmdline_option_##f = { \
		.flag = #f, \
		.flag_short = s, \
		.help = h, \
		.has_arg = ha, \
		.callback = sb_cmdline_cb_##f, \
	}; \
	/* Ppointer to the struct in a special section for the linker script */ \
	static __attribute__((section(".u_boot_sandbox_getopt"), used)) \
		struct sb_cmdline_option *sb_cmdline_option_##f##_ptr = \
		&sb_cmdline_option_##f

/**
 * Macros for end code to declare new command line flags.
 *
 * @param f   The long flag name e.g. help
 * @param ha  Does the flag have an argument e.g. 0/1
 * @param h   The help string displayed when showing --help
 *
 * This invocation:
 *   SB_CMDLINE_OPT(foo, 0, "The foo arg");
 * Will create a new flag named "--foo" (no short option) that takes
 * no argument.  If the user specifies "--foo", then the callback func
 * sb_cmdline_cb_foo() will automatically be called.
 */
#define SB_CMDLINE_OPT(f, ha, h) _SB_CMDLINE_OPT(f, 0, ha, h)
/*
 * Same as above, but @s is used to specify a short flag e.g.
 *   SB_CMDLINE_OPT(foo, 'f', 0, "The foo arg");
 */
#define SB_CMDLINE_OPT_SHORT(f, s, ha, h) _SB_CMDLINE_OPT(f, s, ha, h)

#endif
