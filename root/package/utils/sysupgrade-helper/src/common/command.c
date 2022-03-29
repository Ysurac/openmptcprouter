/*
 * (C) Copyright 2000-2009
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

/*
 *  Command Processor Table
 */

#include <common.h>
#include <command.h>
#include <linux/ctype.h>

/*
 * Use puts() instead of printf() to avoid printf buffer overflow
 * for long help messages
 */

int _do_help (cmd_tbl_t *cmd_start, int cmd_items, cmd_tbl_t * cmdtp, int
	      flag, int argc, char * const argv[])
{
	int i;
	int rcode = 0;

	if (argc == 1) {	/*show list of commands */
		cmd_tbl_t *cmd_array[cmd_items];
		int i, j, swaps;

		/* Make array of commands from .uboot_cmd section */
		cmdtp = cmd_start;
		for (i = 0; i < cmd_items; i++) {
			cmd_array[i] = cmdtp++;
		}

		/* Sort command list (trivial bubble sort) */
		for (i = cmd_items - 1; i > 0; --i) {
			swaps = 0;
			for (j = 0; j < i; ++j) {
				if (strcmp (cmd_array[j]->name,
					    cmd_array[j + 1]->name) > 0) {
					cmd_tbl_t *tmp;
					tmp = cmd_array[j];
					cmd_array[j] = cmd_array[j + 1];
					cmd_array[j + 1] = tmp;
					++swaps;
				}
			}
			if (!swaps)
				break;
		}

		/* print short help (usage) */
		for (i = 0; i < cmd_items; i++) {
			const char *usage = cmd_array[i]->usage;

			/* allow user abort */
			if (ctrlc ())
				return 1;
			if (usage == NULL)
				continue;
			printf("%-*s- %s\n", CONFIG_SYS_HELP_CMD_WIDTH,
			       cmd_array[i]->name, usage);
		}
		return 0;
	}
	/*
	 * command help (long version)
	 */
	for (i = 1; i < argc; ++i) {
		if ((cmdtp = find_cmd_tbl (argv[i], cmd_start, cmd_items )) != NULL) {
			rcode |= cmd_usage(cmdtp);
		} else {
			printf ("Unknown command '%s' - try 'help'"
				" without arguments for list of all"
				" known commands\n\n", argv[i]
					);
			rcode = 1;
		}
	}
	return rcode;
}

/***************************************************************************
 * find command table entry for a command
 */
cmd_tbl_t *find_cmd_tbl (const char *cmd, cmd_tbl_t *table, int table_len)
{
	cmd_tbl_t *cmdtp;
	cmd_tbl_t *cmdtp_temp = table;	/*Init value */
	const char *p;
	int len;
	int n_found = 0;

	if (!cmd)
		return NULL;
	/*
	 * Some commands allow length modifiers (like "cp.b");
	 * compare command name only until first dot.
	 */
	len = ((p = strchr(cmd, '.')) == NULL) ? strlen (cmd) : (p - cmd);

	for (cmdtp = table;
	     cmdtp != table + table_len;
	     cmdtp++) {
		if (strncmp (cmd, cmdtp->name, len) == 0) {
			if (len == strlen (cmdtp->name))
				return cmdtp;	/* full match */

			cmdtp_temp = cmdtp;	/* abbreviated command ? */
			n_found++;
		}
	}
	if (n_found == 1) {			/* exactly one match */
		return cmdtp_temp;
	}

	return NULL;	/* not found or ambiguous command */
}

cmd_tbl_t *find_cmd (const char *cmd)
{
	int len = &__u_boot_cmd_end - &__u_boot_cmd_start;
	return find_cmd_tbl(cmd, &__u_boot_cmd_start, len);
}

int cmd_usage(const cmd_tbl_t *cmdtp)
{
	printf("%s - %s\n\n", cmdtp->name, cmdtp->usage);

#ifdef	CONFIG_SYS_LONGHELP
	printf("Usage:\n%s ", cmdtp->name);

	if (!cmdtp->help) {
		puts ("- No additional help available.\n");
		return 1;
	}

	puts (cmdtp->help);
	putc ('\n');
#endif	/* CONFIG_SYS_LONGHELP */
	return 1;
}

#ifdef CONFIG_AUTO_COMPLETE

int var_complete(int argc, char * const argv[], char last_char, int maxv, char *cmdv[])
{
	static char tmp_buf[512];
	int space;

	space = last_char == '\0' || isblank(last_char);

	if (space && argc == 1)
		return env_complete("", maxv, cmdv, sizeof(tmp_buf), tmp_buf);

	if (!space && argc == 2)
		return env_complete(argv[1], maxv, cmdv, sizeof(tmp_buf), tmp_buf);

	return 0;
}

/*************************************************************************************/

static int complete_cmdv(int argc, char * const argv[], char last_char, int maxv, char *cmdv[])
{
	cmd_tbl_t *cmdtp;
	const char *p;
	int len, clen;
	int n_found = 0;
	const char *cmd;

	/* sanity? */
	if (maxv < 2)
		return -2;

	cmdv[0] = NULL;

	if (argc == 0) {
		/* output full list of commands */
		for (cmdtp = &__u_boot_cmd_start; cmdtp != &__u_boot_cmd_end; cmdtp++) {
			if (n_found >= maxv - 2) {
				cmdv[n_found++] = "...";
				break;
			}
			cmdv[n_found++] = cmdtp->name;
		}
		cmdv[n_found] = NULL;
		return n_found;
	}

	/* more than one arg or one but the start of the next */
	if (argc > 1 || (last_char == '\0' || isblank(last_char))) {
		cmdtp = find_cmd(argv[0]);
		if (cmdtp == NULL || cmdtp->complete == NULL) {
			cmdv[0] = NULL;
			return 0;
		}
		return (*cmdtp->complete)(argc, argv, last_char, maxv, cmdv);
	}

	cmd = argv[0];
	/*
	 * Some commands allow length modifiers (like "cp.b");
	 * compare command name only until first dot.
	 */
	p = strchr(cmd, '.');
	if (p == NULL)
		len = strlen(cmd);
	else
		len = p - cmd;

	/* return the partial matches */
	for (cmdtp = &__u_boot_cmd_start; cmdtp != &__u_boot_cmd_end; cmdtp++) {

		clen = strlen(cmdtp->name);
		if (clen < len)
			continue;

		if (memcmp(cmd, cmdtp->name, len) != 0)
			continue;

		/* too many! */
		if (n_found >= maxv - 2) {
			cmdv[n_found++] = "...";
			break;
		}

		cmdv[n_found++] = cmdtp->name;
	}

	cmdv[n_found] = NULL;
	return n_found;
}

static int make_argv(char *s, int argvsz, char *argv[])
{
	int argc = 0;

	/* split into argv */
	while (argc < argvsz - 1) {

		/* skip any white space */
		while (isblank(*s))
			++s;

		if (*s == '\0')	/* end of s, no more args	*/
			break;

		argv[argc++] = s;	/* begin of argument string	*/

		/* find end of string */
		while (*s && !isblank(*s))
			++s;

		if (*s == '\0')		/* end of s, no more args	*/
			break;

		*s++ = '\0';		/* terminate current arg	 */
	}
	argv[argc] = NULL;

	return argc;
}

static void print_argv(const char *banner, const char *leader, const char *sep, int linemax, char * const argv[])
{
	int ll = leader != NULL ? strlen(leader) : 0;
	int sl = sep != NULL ? strlen(sep) : 0;
	int len, i;

	if (banner) {
		puts("\n");
		puts(banner);
	}

	i = linemax;	/* force leader and newline */
	while (*argv != NULL) {
		len = strlen(*argv) + sl;
		if (i + len >= linemax) {
			puts("\n");
			if (leader)
				puts(leader);
			i = ll - sl;
		} else if (sep)
			puts(sep);
		puts(*argv++);
		i += len;
	}
	printf("\n");
}

static int find_common_prefix(char * const argv[])
{
	int i, len;
	char *anchor, *s, *t;

	if (*argv == NULL)
		return 0;

	/* begin with max */
	anchor = *argv++;
	len = strlen(anchor);
	while ((t = *argv++) != NULL) {
		s = anchor;
		for (i = 0; i < len; i++, t++, s++) {
			if (*t != *s)
				break;
		}
		len = s - anchor;
	}
	return len;
}

static char tmp_buf[CONFIG_SYS_CBSIZE];	/* copy of console I/O buffer	*/

int cmd_auto_complete(const char *const prompt, char *buf, int *np, int *colp)
{
	int n = *np, col = *colp;
	char *argv[CONFIG_SYS_MAXARGS + 1];		/* NULL terminated	*/
	char *cmdv[20];
	char *s, *t;
	const char *sep;
	int i, j, k, len, seplen, argc;
	int cnt;
	char last_char;

	if (strcmp(prompt, CONFIG_SYS_PROMPT) != 0)
		return 0;	/* not in normal console */

	cnt = strlen(buf);
	if (cnt >= 1)
		last_char = buf[cnt - 1];
	else
		last_char = '\0';

	/* copy to secondary buffer which will be affected */
	strcpy(tmp_buf, buf);

	/* separate into argv */
	argc = make_argv(tmp_buf, sizeof(argv)/sizeof(argv[0]), argv);

	/* do the completion and return the possible completions */
	i = complete_cmdv(argc, argv, last_char, sizeof(cmdv)/sizeof(cmdv[0]), cmdv);

	/* no match; bell and out */
	if (i == 0) {
		if (argc > 1)	/* allow tab for non command */
			return 0;
		putc('\a');
		return 1;
	}

	s = NULL;
	len = 0;
	sep = NULL;
	seplen = 0;
	if (i == 1) { /* one match; perfect */
		k = strlen(argv[argc - 1]);
		s = cmdv[0] + k;
		len = strlen(s);
		sep = " ";
		seplen = 1;
	} else if (i > 1 && (j = find_common_prefix(cmdv)) != 0) {	/* more */
		k = strlen(argv[argc - 1]);
		j -= k;
		if (j > 0) {
			s = cmdv[0] + k;
			len = j;
		}
	}

	if (s != NULL) {
		k = len + seplen;
		/* make sure it fits */
		if (n + k >= CONFIG_SYS_CBSIZE - 2) {
			putc('\a');
			return 1;
		}

		t = buf + cnt;
		for (i = 0; i < len; i++)
			*t++ = *s++;
		if (sep != NULL)
			for (i = 0; i < seplen; i++)
				*t++ = sep[i];
		*t = '\0';
		n += k;
		col += k;
		puts(t - k);
		if (sep == NULL)
			putc('\a');
		*np = n;
		*colp = col;
	} else {
		print_argv(NULL, "  ", " ", 78, cmdv);

		puts(prompt);
		puts(buf);
	}
	return 1;
}

#endif

#ifdef CMD_DATA_SIZE
int cmd_get_data_size(char* arg, int default_size)
{
	/* Check for a size specification .b, .w or .l.
	 */
	int len = strlen(arg);
	if (len > 2 && arg[len-2] == '.') {
		switch(arg[len-1]) {
		case 'b':
			return 1;
		case 'w':
			return 2;
		case 'l':
			return 4;
		case 's':
			return -2;
		default:
			return -1;
		}
	}
	return default_size;
}
#endif

#if defined(CONFIG_NEEDS_MANUAL_RELOC)
DECLARE_GLOBAL_DATA_PTR;

void fixup_cmdtable(cmd_tbl_t *cmdtp, int size)
{
	int	i;

	if (gd->reloc_off == 0)
		return;

	for (i = 0; i < size; i++) {
		ulong addr;

		addr = (ulong) (cmdtp->cmd) + gd->reloc_off;
#if DEBUG_COMMANDS
		printf("Command \"%s\": 0x%08lx => 0x%08lx\n",
		       cmdtp->name, (ulong) (cmdtp->cmd), addr);
#endif
		cmdtp->cmd =
			(int (*)(struct cmd_tbl_s *, int, int, char * const []))addr;
		addr = (ulong)(cmdtp->name) + gd->reloc_off;
		cmdtp->name = (char *)addr;
		if (cmdtp->usage) {
			addr = (ulong)(cmdtp->usage) + gd->reloc_off;
			cmdtp->usage = (char *)addr;
		}
#ifdef	CONFIG_SYS_LONGHELP
		if (cmdtp->help) {
			addr = (ulong)(cmdtp->help) + gd->reloc_off;
			cmdtp->help = (char *)addr;
		}
#endif
#ifdef CONFIG_AUTO_COMPLETE
		if (cmdtp->complete) {
			addr = (ulong)(cmdtp->complete) + gd->reloc_off;
			cmdtp->complete =
				(int (*)(int, char * const [], char, int, char * []))addr;
		}
#endif
		cmdtp++;
	}
}
#endif

/**
 * Call a command function. This should be the only route in U-Boot to call
 * a command, so that we can track whether we are waiting for input or
 * executing a command.
 *
 * @param cmdtp		Pointer to the command to execute
 * @param flag		Some flags normally 0 (see CMD_FLAG_.. above)
 * @param argc		Number of arguments (arg 0 must be the command text)
 * @param argv		Arguments
 * @return 0 if command succeeded, else non-zero (CMD_RET_...)
 */
static int cmd_call(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int result;

	result = (cmdtp->cmd)(cmdtp, flag, argc, argv);
	if (result)
		debug("Command failed, result=%d", result);
	return result;
}

enum command_ret_t cmd_process(int flag, int argc, char * const argv[],
			       int *repeatable)
{
	enum command_ret_t rc = CMD_RET_SUCCESS;
	cmd_tbl_t *cmdtp;

	/* Look up command in command table */
	cmdtp = find_cmd(argv[0]);
	if (cmdtp == NULL) {
		printf("Unknown command '%s' - try 'help'\n", argv[0]);
		return 1;
	}

	/* found - check max args */
	if (argc > cmdtp->maxargs)
		rc = CMD_RET_USAGE;

#if defined(CONFIG_CMD_BOOTD)
	/* avoid "bootd" recursion */
	else if (cmdtp->cmd == do_bootd) {
		if (flag & CMD_FLAG_BOOTD) {
			puts("'bootd' recursion detected\n");
			rc = CMD_RET_FAILURE;
		} else {
			flag |= CMD_FLAG_BOOTD;
		}
	}
#endif

	/* If OK so far, then do the command */
	if (!rc) {
		rc = cmd_call(cmdtp, flag, argc, argv);
		*repeatable &= cmdtp->repeatable;
	}
	if (rc == CMD_RET_USAGE)
		rc = cmd_usage(cmdtp);
	return rc;
}
