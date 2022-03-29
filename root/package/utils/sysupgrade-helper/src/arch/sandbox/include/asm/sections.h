/*
 * decls for symbols defined in the linker script
 *
 * Copyright (c) 2012 The Chromium OS Authors.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef __SANDBOX_SECTIONS_H
#define __SANDBOX_SECTIONS_H

struct sb_cmdline_option;

extern struct sb_cmdline_option *__u_boot_sandbox_option_start[],
	*__u_boot_sandbox_option_end[];

static inline size_t __u_boot_sandbox_option_count(void)
{
	return __u_boot_sandbox_option_end - __u_boot_sandbox_option_start;
}

#endif
