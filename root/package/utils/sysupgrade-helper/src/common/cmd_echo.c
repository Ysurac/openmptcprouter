/*
 * Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
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
#include <command.h>

int do_echo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i;
	int putnl = 1;

	for (i = 1; i < argc; i++) {
		char *p = argv[i], c;

		if (i > 1)
			putc(' ');
		while ((c = *p++) != '\0') {
			if (c == '\\' && *p == 'c') {
				putnl = 0;
				p++;
			} else {
				putc(c);
			}
		}
	}

	if (putnl)
		putc('\n');

	return 0;
}

U_BOOT_CMD(
	echo,	CONFIG_SYS_MAXARGS,	1,	do_echo,
	"echo args to console",
	"[args..]\n"
	"    - echo args to console; \\c suppresses newline"
);
