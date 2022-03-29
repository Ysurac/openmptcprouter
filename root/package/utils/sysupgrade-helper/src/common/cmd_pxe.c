/*
 * Copyright 2010-2011 Calxeda, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <errno.h>
#include <linux/list.h>

#include "menu.h"

#define MAX_TFTP_PATH_LEN 127

/*
 * Like getenv, but prints an error if envvar isn't defined in the
 * environment.  It always returns what getenv does, so it can be used in
 * place of getenv without changing error handling otherwise.
 */
static char *from_env(char *envvar)
{
	char *ret;

	ret = getenv(envvar);

	if (!ret)
		printf("missing environment variable: %s\n", envvar);

	return ret;
}

/*
 * Convert an ethaddr from the environment to the format used by pxelinux
 * filenames based on mac addresses. Convert's ':' to '-', and adds "01-" to
 * the beginning of the ethernet address to indicate a hardware type of
 * Ethernet. Also converts uppercase hex characters into lowercase, to match
 * pxelinux's behavior.
 *
 * Returns 1 for success, -ENOENT if 'ethaddr' is undefined in the
 * environment, or some other value < 0 on error.
 */
static int format_mac_pxe(char *outbuf, size_t outbuf_len)
{
	size_t ethaddr_len;
	char *p, *ethaddr;

	ethaddr = from_env("ethaddr");

	if (!ethaddr)
		return -ENOENT;

	ethaddr_len = strlen(ethaddr);

	/*
	 * ethaddr_len + 4 gives room for "01-", ethaddr, and a NUL byte at
	 * the end.
	 */
	if (outbuf_len < ethaddr_len + 4) {
		printf("outbuf is too small (%d < %d)\n",
				outbuf_len, ethaddr_len + 4);

		return -EINVAL;
	}

	strcpy(outbuf, "01-");

	for (p = outbuf + 3; *ethaddr; ethaddr++, p++) {
		if (*ethaddr == ':')
			*p = '-';
		else
			*p = tolower(*ethaddr);
	}

	*p = '\0';

	return 1;
}

/*
 * Returns the directory the file specified in the bootfile env variable is
 * in. If bootfile isn't defined in the environment, return NULL, which should
 * be interpreted as "don't prepend anything to paths".
 */
static int get_bootfile_path(const char *file_path, char *bootfile_path,
			     size_t bootfile_path_size)
{
	char *bootfile, *last_slash;
	size_t path_len = 0;

	if (file_path[0] == '/')
		goto ret;

	bootfile = from_env("bootfile");

	if (!bootfile)
		goto ret;

	last_slash = strrchr(bootfile, '/');

	if (last_slash == NULL)
		goto ret;

	path_len = (last_slash - bootfile) + 1;

	if (bootfile_path_size < path_len) {
		printf("bootfile_path too small. (%d < %d)\n",
				bootfile_path_size, path_len);

		return -1;
	}

	strncpy(bootfile_path, bootfile, path_len);

 ret:
	bootfile_path[path_len] = '\0';

	return 1;
}

static int (*do_getfile)(char *file_path, char *file_addr);

static int do_get_tftp(char *file_path, char *file_addr)
{
	char *tftp_argv[] = {"tftp", NULL, NULL, NULL};

	tftp_argv[1] = file_addr;
	tftp_argv[2] = file_path;

	if (do_tftpb(NULL, 0, 3, tftp_argv))
		return -ENOENT;

	return 1;
}

static char *fs_argv[5];

static int do_get_ext2(char *file_path, char *file_addr)
{
#ifdef CONFIG_CMD_EXT2
	fs_argv[0] = "ext2load";
	fs_argv[3] = file_addr;
	fs_argv[4] = file_path;

	if (!do_ext2load(NULL, 0, 5, fs_argv))
		return 1;
#endif
	return -ENOENT;
}

static int do_get_fat(char *file_path, char *file_addr)
{
#ifdef CONFIG_CMD_FAT
	fs_argv[0] = "fatload";
	fs_argv[3] = file_addr;
	fs_argv[4] = file_path;

	if (!do_fat_fsload(NULL, 0, 5, fs_argv))
		return 1;
#endif
	return -ENOENT;
}

/*
 * As in pxelinux, paths to files referenced from files we retrieve are
 * relative to the location of bootfile. get_relfile takes such a path and
 * joins it with the bootfile path to get the full path to the target file. If
 * the bootfile path is NULL, we use file_path as is.
 *
 * Returns 1 for success, or < 0 on error.
 */
static int get_relfile(char *file_path, void *file_addr)
{
	size_t path_len;
	char relfile[MAX_TFTP_PATH_LEN+1];
	char addr_buf[10];
	int err;

	err = get_bootfile_path(file_path, relfile, sizeof(relfile));

	if (err < 0)
		return err;

	path_len = strlen(file_path);
	path_len += strlen(relfile);

	if (path_len > MAX_TFTP_PATH_LEN) {
		printf("Base path too long (%s%s)\n",
					relfile,
					file_path);

		return -ENAMETOOLONG;
	}

	strcat(relfile, file_path);

	printf("Retrieving file: %s\n", relfile);

	sprintf(addr_buf, "%p", file_addr);

	return do_getfile(relfile, addr_buf);
}

/*
 * Retrieve the file at 'file_path' to the locate given by 'file_addr'. If
 * 'bootfile' was specified in the environment, the path to bootfile will be
 * prepended to 'file_path' and the resulting path will be used.
 *
 * Returns 1 on success, or < 0 for error.
 */
static int get_pxe_file(char *file_path, void *file_addr)
{
	unsigned long config_file_size;
	char *tftp_filesize;
	int err;

	err = get_relfile(file_path, file_addr);

	if (err < 0)
		return err;

	/*
	 * the file comes without a NUL byte at the end, so find out its size
	 * and add the NUL byte.
	 */
	tftp_filesize = from_env("filesize");

	if (!tftp_filesize)
		return -ENOENT;

	if (strict_strtoul(tftp_filesize, 16, &config_file_size) < 0)
		return -EINVAL;

	*(char *)(file_addr + config_file_size) = '\0';

	return 1;
}

#define PXELINUX_DIR "pxelinux.cfg/"

/*
 * Retrieves a file in the 'pxelinux.cfg' folder. Since this uses get_pxe_file
 * to do the hard work, the location of the 'pxelinux.cfg' folder is generated
 * from the bootfile path, as described above.
 *
 * Returns 1 on success or < 0 on error.
 */
static int get_pxelinux_path(char *file, void *pxefile_addr_r)
{
	size_t base_len = strlen(PXELINUX_DIR);
	char path[MAX_TFTP_PATH_LEN+1];

	if (base_len + strlen(file) > MAX_TFTP_PATH_LEN) {
		printf("path (%s%s) too long, skipping\n",
				PXELINUX_DIR, file);
		return -ENAMETOOLONG;
	}

	sprintf(path, PXELINUX_DIR "%s", file);

	return get_pxe_file(path, pxefile_addr_r);
}

/*
 * Looks for a pxe file with a name based on the pxeuuid environment variable.
 *
 * Returns 1 on success or < 0 on error.
 */
static int pxe_uuid_path(void *pxefile_addr_r)
{
	char *uuid_str;

	uuid_str = from_env("pxeuuid");

	if (!uuid_str)
		return -ENOENT;

	return get_pxelinux_path(uuid_str, pxefile_addr_r);
}

/*
 * Looks for a pxe file with a name based on the 'ethaddr' environment
 * variable.
 *
 * Returns 1 on success or < 0 on error.
 */
static int pxe_mac_path(void *pxefile_addr_r)
{
	char mac_str[21];
	int err;

	err = format_mac_pxe(mac_str, sizeof(mac_str));

	if (err < 0)
		return err;

	return get_pxelinux_path(mac_str, pxefile_addr_r);
}

/*
 * Looks for pxe files with names based on our IP address. See pxelinux
 * documentation for details on what these file names look like.  We match
 * that exactly.
 *
 * Returns 1 on success or < 0 on error.
 */
static int pxe_ipaddr_paths(void *pxefile_addr_r)
{
	char ip_addr[9];
	int mask_pos, err;

	sprintf(ip_addr, "%08X", ntohl(NetOurIP));

	for (mask_pos = 7; mask_pos >= 0;  mask_pos--) {
		err = get_pxelinux_path(ip_addr, pxefile_addr_r);

		if (err > 0)
			return err;

		ip_addr[mask_pos] = '\0';
	}

	return -ENOENT;
}

/*
 * Entry point for the 'pxe get' command.
 * This Follows pxelinux's rules to download a config file from a tftp server.
 * The file is stored at the location given by the pxefile_addr_r environment
 * variable, which must be set.
 *
 * UUID comes from pxeuuid env variable, if defined
 * MAC addr comes from ethaddr env variable, if defined
 * IP
 *
 * see http://syslinux.zytor.com/wiki/index.php/PXELINUX
 *
 * Returns 0 on success or 1 on error.
 */
static int
do_pxe_get(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char *pxefile_addr_str;
	unsigned long pxefile_addr_r;
	int err;

	do_getfile = do_get_tftp;

	if (argc != 1)
		return CMD_RET_USAGE;

	pxefile_addr_str = from_env("pxefile_addr_r");

	if (!pxefile_addr_str)
		return 1;

	err = strict_strtoul(pxefile_addr_str, 16,
				(unsigned long *)&pxefile_addr_r);
	if (err < 0)
		return 1;

	/*
	 * Keep trying paths until we successfully get a file we're looking
	 * for.
	 */
	if (pxe_uuid_path((void *)pxefile_addr_r) > 0
		|| pxe_mac_path((void *)pxefile_addr_r) > 0
		|| pxe_ipaddr_paths((void *)pxefile_addr_r) > 0
		|| get_pxelinux_path("default", (void *)pxefile_addr_r) > 0) {

		printf("Config file found\n");

		return 0;
	}

	printf("Config file not found\n");

	return 1;
}

/*
 * Wrapper to make it easier to store the file at file_path in the location
 * specified by envaddr_name. file_path will be joined to the bootfile path,
 * if any is specified.
 *
 * Returns 1 on success or < 0 on error.
 */
static int get_relfile_envaddr(char *file_path, char *envaddr_name)
{
	unsigned long file_addr;
	char *envaddr;

	envaddr = from_env(envaddr_name);

	if (!envaddr)
		return -ENOENT;

	if (strict_strtoul(envaddr, 16, &file_addr) < 0)
		return -EINVAL;

	return get_relfile(file_path, (void *)file_addr);
}

/*
 * A note on the pxe file parser.
 *
 * We're parsing files that use syslinux grammar, which has a few quirks.
 * String literals must be recognized based on context - there is no
 * quoting or escaping support. There's also nothing to explicitly indicate
 * when a label section completes. We deal with that by ending a label
 * section whenever we see a line that doesn't include.
 *
 * As with the syslinux family, this same file format could be reused in the
 * future for non pxe purposes. The only action it takes during parsing that
 * would throw this off is handling of include files. It assumes we're using
 * pxe, and does a tftp download of a file listed as an include file in the
 * middle of the parsing operation. That could be handled by refactoring it to
 * take a 'include file getter' function.
 */

/*
 * Describes a single label given in a pxe file.
 *
 * Create these with the 'label_create' function given below.
 *
 * name - the name of the menu as given on the 'menu label' line.
 * kernel - the path to the kernel file to use for this label.
 * append - kernel command line to use when booting this label
 * initrd - path to the initrd to use for this label.
 * attempted - 0 if we haven't tried to boot this label, 1 if we have.
 * localboot - 1 if this label specified 'localboot', 0 otherwise.
 * list - lets these form a list, which a pxe_menu struct will hold.
 */
struct pxe_label {
	char *name;
	char *menu;
	char *kernel;
	char *append;
	char *initrd;
	int attempted;
	int localboot;
	struct list_head list;
};

/*
 * Describes a pxe menu as given via pxe files.
 *
 * title - the name of the menu as given by a 'menu title' line.
 * default_label - the name of the default label, if any.
 * timeout - time in tenths of a second to wait for a user key-press before
 *           booting the default label.
 * prompt - if 0, don't prompt for a choice unless the timeout period is
 *          interrupted.  If 1, always prompt for a choice regardless of
 *          timeout.
 * labels - a list of labels defined for the menu.
 */
struct pxe_menu {
	char *title;
	char *default_label;
	int timeout;
	int prompt;
	struct list_head labels;
};

/*
 * Allocates memory for and initializes a pxe_label. This uses malloc, so the
 * result must be free()'d to reclaim the memory.
 *
 * Returns NULL if malloc fails.
 */
static struct pxe_label *label_create(void)
{
	struct pxe_label *label;

	label = malloc(sizeof(struct pxe_label));

	if (!label)
		return NULL;

	memset(label, 0, sizeof(struct pxe_label));

	return label;
}

/*
 * Free the memory used by a pxe_label, including that used by its name,
 * kernel, append and initrd members, if they're non NULL.
 *
 * So - be sure to only use dynamically allocated memory for the members of
 * the pxe_label struct, unless you want to clean it up first. These are
 * currently only created by the pxe file parsing code.
 */
static void label_destroy(struct pxe_label *label)
{
	if (label->name)
		free(label->name);

	if (label->kernel)
		free(label->kernel);

	if (label->append)
		free(label->append);

	if (label->initrd)
		free(label->initrd);

	free(label);
}

/*
 * Print a label and its string members if they're defined.
 *
 * This is passed as a callback to the menu code for displaying each
 * menu entry.
 */
static void label_print(void *data)
{
	struct pxe_label *label = data;
	const char *c = label->menu ? label->menu : label->kernel;

	printf("%s:\t%s\n", label->name, c);

	if (label->kernel)
		printf("\t\tkernel: %s\n", label->kernel);

	if (label->append)
		printf("\t\tappend: %s\n", label->append);

	if (label->initrd)
		printf("\t\tinitrd: %s\n", label->initrd);
}

/*
 * Boot a label that specified 'localboot'. This requires that the 'localcmd'
 * environment variable is defined. Its contents will be executed as U-boot
 * command.  If the label specified an 'append' line, its contents will be
 * used to overwrite the contents of the 'bootargs' environment variable prior
 * to running 'localcmd'.
 *
 * Returns 1 on success or < 0 on error.
 */
static int label_localboot(struct pxe_label *label)
{
	char *localcmd, *dupcmd;
	int ret;

	localcmd = from_env("localcmd");

	if (!localcmd)
		return -ENOENT;

	/*
	 * dup the command to avoid any issues with the version of it existing
	 * in the environment changing during the execution of the command.
	 */
	dupcmd = strdup(localcmd);

	if (!dupcmd)
		return -ENOMEM;

	if (label->append)
		setenv("bootargs", label->append);

	printf("running: %s\n", dupcmd);

	ret = run_command(dupcmd, 0);

	free(dupcmd);

	return ret;
}

/*
 * Boot according to the contents of a pxe_label.
 *
 * If we can't boot for any reason, we return.  A successful boot never
 * returns.
 *
 * The kernel will be stored in the location given by the 'kernel_addr_r'
 * environment variable.
 *
 * If the label specifies an initrd file, it will be stored in the location
 * given by the 'ramdisk_addr_r' environment variable.
 *
 * If the label specifies an 'append' line, its contents will overwrite that
 * of the 'bootargs' environment variable.
 */
static void label_boot(struct pxe_label *label)
{
	char *bootm_argv[] = { "bootm", NULL, NULL, NULL, NULL };
	int bootm_argc = 3;

	label_print(label);

	label->attempted = 1;

	if (label->localboot) {
		label_localboot(label);
		return;
	}

	if (label->kernel == NULL) {
		printf("No kernel given, skipping %s\n",
				label->name);
		return;
	}

	if (label->initrd) {
		if (get_relfile_envaddr(label->initrd, "ramdisk_addr_r") < 0) {
			printf("Skipping %s for failure retrieving initrd\n",
					label->name);
			return;
		}

		bootm_argv[2] = getenv("ramdisk_addr_r");
	} else {
		bootm_argv[2] = "-";
	}

	if (get_relfile_envaddr(label->kernel, "kernel_addr_r") < 0) {
		printf("Skipping %s for failure retrieving kernel\n",
				label->name);
		return;
	}

	if (label->append)
		setenv("bootargs", label->append);

	bootm_argv[1] = getenv("kernel_addr_r");

	/*
	 * fdt usage is optional.  If there is an fdt_addr specified, we will
	 * pass it along to bootm, and adjust argc appropriately.
	 */
	bootm_argv[3] = getenv("fdt_addr");

	if (bootm_argv[3])
		bootm_argc = 4;

	do_bootm(NULL, 0, bootm_argc, bootm_argv);
}

/*
 * Tokens for the pxe file parser.
 */
enum token_type {
	T_EOL,
	T_STRING,
	T_EOF,
	T_MENU,
	T_TITLE,
	T_TIMEOUT,
	T_LABEL,
	T_KERNEL,
	T_LINUX,
	T_APPEND,
	T_INITRD,
	T_LOCALBOOT,
	T_DEFAULT,
	T_PROMPT,
	T_INCLUDE,
	T_INVALID
};

/*
 * A token - given by a value and a type.
 */
struct token {
	char *val;
	enum token_type type;
};

/*
 * Keywords recognized.
 */
static const struct token keywords[] = {
	{"menu", T_MENU},
	{"title", T_TITLE},
	{"timeout", T_TIMEOUT},
	{"default", T_DEFAULT},
	{"prompt", T_PROMPT},
	{"label", T_LABEL},
	{"kernel", T_KERNEL},
	{"linux", T_LINUX},
	{"localboot", T_LOCALBOOT},
	{"append", T_APPEND},
	{"initrd", T_INITRD},
	{"include", T_INCLUDE},
	{NULL, T_INVALID}
};

/*
 * Since pxe(linux) files don't have a token to identify the start of a
 * literal, we have to keep track of when we're in a state where a literal is
 * expected vs when we're in a state a keyword is expected.
 */
enum lex_state {
	L_NORMAL = 0,
	L_KEYWORD,
	L_SLITERAL
};

/*
 * get_string retrieves a string from *p and stores it as a token in
 * *t.
 *
 * get_string used for scanning both string literals and keywords.
 *
 * Characters from *p are copied into t-val until a character equal to
 * delim is found, or a NUL byte is reached. If delim has the special value of
 * ' ', any whitespace character will be used as a delimiter.
 *
 * If lower is unequal to 0, uppercase characters will be converted to
 * lowercase in the result. This is useful to make keywords case
 * insensitive.
 *
 * The location of *p is updated to point to the first character after the end
 * of the token - the ending delimiter.
 *
 * On success, the new value of t->val is returned. Memory for t->val is
 * allocated using malloc and must be free()'d to reclaim it.  If insufficient
 * memory is available, NULL is returned.
 */
static char *get_string(char **p, struct token *t, char delim, int lower)
{
	char *b, *e;
	size_t len, i;

	/*
	 * b and e both start at the beginning of the input stream.
	 *
	 * e is incremented until we find the ending delimiter, or a NUL byte
	 * is reached. Then, we take e - b to find the length of the token.
	 */
	b = e = *p;

	while (*e) {
		if ((delim == ' ' && isspace(*e)) || delim == *e)
			break;
		e++;
	}

	len = e - b;

	/*
	 * Allocate memory to hold the string, and copy it in, converting
	 * characters to lowercase if lower is != 0.
	 */
	t->val = malloc(len + 1);
	if (!t->val)
		return NULL;

	for (i = 0; i < len; i++, b++) {
		if (lower)
			t->val[i] = tolower(*b);
		else
			t->val[i] = *b;
	}

	t->val[len] = '\0';

	/*
	 * Update *p so the caller knows where to continue scanning.
	 */
	*p = e;

	t->type = T_STRING;

	return t->val;
}

/*
 * Populate a keyword token with a type and value.
 */
static void get_keyword(struct token *t)
{
	int i;

	for (i = 0; keywords[i].val; i++) {
		if (!strcmp(t->val, keywords[i].val)) {
			t->type = keywords[i].type;
			break;
		}
	}
}

/*
 * Get the next token.  We have to keep track of which state we're in to know
 * if we're looking to get a string literal or a keyword.
 *
 * *p is updated to point at the first character after the current token.
 */
static void get_token(char **p, struct token *t, enum lex_state state)
{
	char *c = *p;

	t->type = T_INVALID;

	/* eat non EOL whitespace */
	while (isblank(*c))
		c++;

	/*
	 * eat comments. note that string literals can't begin with #, but
	 * can contain a # after their first character.
	 */
	if (*c == '#') {
		while (*c && *c != '\n')
			c++;
	}

	if (*c == '\n') {
		t->type = T_EOL;
		c++;
	} else if (*c == '\0') {
		t->type = T_EOF;
		c++;
	} else if (state == L_SLITERAL) {
		get_string(&c, t, '\n', 0);
	} else if (state == L_KEYWORD) {
		/*
		 * when we expect a keyword, we first get the next string
		 * token delimited by whitespace, and then check if it
		 * matches a keyword in our keyword list. if it does, it's
		 * converted to a keyword token of the appropriate type, and
		 * if not, it remains a string token.
		 */
		get_string(&c, t, ' ', 1);
		get_keyword(t);
	}

	*p = c;
}

/*
 * Increment *c until we get to the end of the current line, or EOF.
 */
static void eol_or_eof(char **c)
{
	while (**c && **c != '\n')
		(*c)++;
}

/*
 * All of these parse_* functions share some common behavior.
 *
 * They finish with *c pointing after the token they parse, and return 1 on
 * success, or < 0 on error.
 */

/*
 * Parse a string literal and store a pointer it at *dst. String literals
 * terminate at the end of the line.
 */
static int parse_sliteral(char **c, char **dst)
{
	struct token t;
	char *s = *c;

	get_token(c, &t, L_SLITERAL);

	if (t.type != T_STRING) {
		printf("Expected string literal: %.*s\n", (int)(*c - s), s);
		return -EINVAL;
	}

	*dst = t.val;

	return 1;
}

/*
 * Parse a base 10 (unsigned) integer and store it at *dst.
 */
static int parse_integer(char **c, int *dst)
{
	struct token t;
	char *s = *c;
	unsigned long temp;

	get_token(c, &t, L_SLITERAL);

	if (t.type != T_STRING) {
		printf("Expected string: %.*s\n", (int)(*c - s), s);
		return -EINVAL;
	}

	if (strict_strtoul(t.val, 10, &temp) < 0) {
		printf("Expected unsigned integer: %s\n", t.val);
		return -EINVAL;
	}

	*dst = (int)temp;

	free(t.val);

	return 1;
}

static int parse_pxefile_top(char *p, struct pxe_menu *cfg, int nest_level);

/*
 * Parse an include statement, and retrieve and parse the file it mentions.
 *
 * base should point to a location where it's safe to store the file, and
 * nest_level should indicate how many nested includes have occurred. For this
 * include, nest_level has already been incremented and doesn't need to be
 * incremented here.
 */
static int handle_include(char **c, char *base,
				struct pxe_menu *cfg, int nest_level)
{
	char *include_path;
	char *s = *c;
	int err;

	err = parse_sliteral(c, &include_path);

	if (err < 0) {
		printf("Expected include path: %.*s\n",
				 (int)(*c - s), s);
		return err;
	}

	err = get_pxe_file(include_path, base);

	if (err < 0) {
		printf("Couldn't retrieve %s\n", include_path);
		return err;
	}

	return parse_pxefile_top(base, cfg, nest_level);
}

/*
 * Parse lines that begin with 'menu'.
 *
 * b and nest are provided to handle the 'menu include' case.
 *
 * b should be the address where the file currently being parsed is stored.
 *
 * nest_level should be 1 when parsing the top level pxe file, 2 when parsing
 * a file it includes, 3 when parsing a file included by that file, and so on.
 */
static int parse_menu(char **c, struct pxe_menu *cfg, char *b, int nest_level)
{
	struct token t;
	char *s = *c;
	int err = 0;

	get_token(c, &t, L_KEYWORD);

	switch (t.type) {
	case T_TITLE:
		err = parse_sliteral(c, &cfg->title);

		break;

	case T_INCLUDE:
		err = handle_include(c, b + strlen(b) + 1, cfg,
						nest_level + 1);
		break;

	default:
		printf("Ignoring malformed menu command: %.*s\n",
				(int)(*c - s), s);
	}

	if (err < 0)
		return err;

	eol_or_eof(c);

	return 1;
}

/*
 * Handles parsing a 'menu line' when we're parsing a label.
 */
static int parse_label_menu(char **c, struct pxe_menu *cfg,
				struct pxe_label *label)
{
	struct token t;
	char *s;

	s = *c;

	get_token(c, &t, L_KEYWORD);

	switch (t.type) {
	case T_DEFAULT:
		if (cfg->default_label)
			free(cfg->default_label);

		cfg->default_label = strdup(label->name);

		if (!cfg->default_label)
			return -ENOMEM;

		break;
	case T_LABEL:
		parse_sliteral(c, &label->menu);
		break;
	default:
		printf("Ignoring malformed menu command: %.*s\n",
				(int)(*c - s), s);
	}

	eol_or_eof(c);

	return 0;
}

/*
 * Parses a label and adds it to the list of labels for a menu.
 *
 * A label ends when we either get to the end of a file, or
 * get some input we otherwise don't have a handler defined
 * for.
 *
 */
static int parse_label(char **c, struct pxe_menu *cfg)
{
	struct token t;
	int len;
	char *s = *c;
	struct pxe_label *label;
	int err;

	label = label_create();
	if (!label)
		return -ENOMEM;

	err = parse_sliteral(c, &label->name);
	if (err < 0) {
		printf("Expected label name: %.*s\n", (int)(*c - s), s);
		label_destroy(label);
		return -EINVAL;
	}

	list_add_tail(&label->list, &cfg->labels);

	while (1) {
		s = *c;
		get_token(c, &t, L_KEYWORD);

		err = 0;
		switch (t.type) {
		case T_MENU:
			err = parse_label_menu(c, cfg, label);
			break;

		case T_KERNEL:
		case T_LINUX:
			err = parse_sliteral(c, &label->kernel);
			break;

		case T_APPEND:
			err = parse_sliteral(c, &label->append);
			if (label->initrd)
				break;
			s = strstr(label->append, "initrd=");
			if (!s)
				break;
			s += 7;
			len = (int)(strchr(s, ' ') - s);
			label->initrd = malloc(len + 1);
			strncpy(label->initrd, s, len);
			label->initrd[len] = '\0';

			break;

		case T_INITRD:
			if (!label->initrd)
				err = parse_sliteral(c, &label->initrd);
			break;

		case T_LOCALBOOT:
			err = parse_integer(c, &label->localboot);
			break;

		case T_EOL:
			break;

		default:
			/*
			 * put the token back! we don't want it - it's the end
			 * of a label and whatever token this is, it's
			 * something for the menu level context to handle.
			 */
			*c = s;
			return 1;
		}

		if (err < 0)
			return err;
	}
}

/*
 * This 16 comes from the limit pxelinux imposes on nested includes.
 *
 * There is no reason at all we couldn't do more, but some limit helps prevent
 * infinite (until crash occurs) recursion if a file tries to include itself.
 */
#define MAX_NEST_LEVEL 16

/*
 * Entry point for parsing a menu file. nest_level indicates how many times
 * we've nested in includes.  It will be 1 for the top level menu file.
 *
 * Returns 1 on success, < 0 on error.
 */
static int parse_pxefile_top(char *p, struct pxe_menu *cfg, int nest_level)
{
	struct token t;
	char *s, *b, *label_name;
	int err;

	b = p;

	if (nest_level > MAX_NEST_LEVEL) {
		printf("Maximum nesting (%d) exceeded\n", MAX_NEST_LEVEL);
		return -EMLINK;
	}

	while (1) {
		s = p;

		get_token(&p, &t, L_KEYWORD);

		err = 0;
		switch (t.type) {
		case T_MENU:
			err = parse_menu(&p, cfg, b, nest_level);
			break;

		case T_TIMEOUT:
			err = parse_integer(&p, &cfg->timeout);
			break;

		case T_LABEL:
			err = parse_label(&p, cfg);
			break;

		case T_DEFAULT:
			err = parse_sliteral(&p, &label_name);

			if (label_name) {
				if (cfg->default_label)
					free(cfg->default_label);

				cfg->default_label = label_name;
			}

			break;

		case T_INCLUDE:
			err = handle_include(&p, b + ALIGN(strlen(b), 4), cfg,
							nest_level + 1);
			break;

		case T_PROMPT:
			err = parse_integer(&p, &cfg->prompt);
			break;

		case T_EOL:
			break;

		case T_EOF:
			return 1;

		default:
			printf("Ignoring unknown command: %.*s\n",
							(int)(p - s), s);
			eol_or_eof(&p);
		}

		if (err < 0)
			return err;
	}
}

/*
 * Free the memory used by a pxe_menu and its labels.
 */
static void destroy_pxe_menu(struct pxe_menu *cfg)
{
	struct list_head *pos, *n;
	struct pxe_label *label;

	if (cfg->title)
		free(cfg->title);

	if (cfg->default_label)
		free(cfg->default_label);

	list_for_each_safe(pos, n, &cfg->labels) {
		label = list_entry(pos, struct pxe_label, list);

		label_destroy(label);
	}

	free(cfg);
}

/*
 * Entry point for parsing a pxe file. This is only used for the top level
 * file.
 *
 * Returns NULL if there is an error, otherwise, returns a pointer to a
 * pxe_menu struct populated with the results of parsing the pxe file (and any
 * files it includes). The resulting pxe_menu struct can be free()'d by using
 * the destroy_pxe_menu() function.
 */
static struct pxe_menu *parse_pxefile(char *menucfg)
{
	struct pxe_menu *cfg;

	cfg = malloc(sizeof(struct pxe_menu));

	if (!cfg)
		return NULL;

	memset(cfg, 0, sizeof(struct pxe_menu));

	INIT_LIST_HEAD(&cfg->labels);

	if (parse_pxefile_top(menucfg, cfg, 1) < 0) {
		destroy_pxe_menu(cfg);
		return NULL;
	}

	return cfg;
}

/*
 * Converts a pxe_menu struct into a menu struct for use with U-boot's generic
 * menu code.
 */
static struct menu *pxe_menu_to_menu(struct pxe_menu *cfg)
{
	struct pxe_label *label;
	struct list_head *pos;
	struct menu *m;
	int err;

	/*
	 * Create a menu and add items for all the labels.
	 */
	m = menu_create(cfg->title, cfg->timeout, cfg->prompt, label_print);

	if (!m)
		return NULL;

	list_for_each(pos, &cfg->labels) {
		label = list_entry(pos, struct pxe_label, list);

		if (menu_item_add(m, label->name, label) != 1) {
			menu_destroy(m);
			return NULL;
		}
	}

	/*
	 * After we've created items for each label in the menu, set the
	 * menu's default label if one was specified.
	 */
	if (cfg->default_label) {
		err = menu_default_set(m, cfg->default_label);
		if (err != 1) {
			if (err != -ENOENT) {
				menu_destroy(m);
				return NULL;
			}

			printf("Missing default: %s\n", cfg->default_label);
		}
	}

	return m;
}

/*
 * Try to boot any labels we have yet to attempt to boot.
 */
static void boot_unattempted_labels(struct pxe_menu *cfg)
{
	struct list_head *pos;
	struct pxe_label *label;

	list_for_each(pos, &cfg->labels) {
		label = list_entry(pos, struct pxe_label, list);

		if (!label->attempted)
			label_boot(label);
	}
}

/*
 * Boot the system as prescribed by a pxe_menu.
 *
 * Use the menu system to either get the user's choice or the default, based
 * on config or user input.  If there is no default or user's choice,
 * attempted to boot labels in the order they were given in pxe files.
 * If the default or user's choice fails to boot, attempt to boot other
 * labels in the order they were given in pxe files.
 *
 * If this function returns, there weren't any labels that successfully
 * booted, or the user interrupted the menu selection via ctrl+c.
 */
static void handle_pxe_menu(struct pxe_menu *cfg)
{
	void *choice;
	struct menu *m;
	int err;

	m = pxe_menu_to_menu(cfg);
	if (!m)
		return;

	err = menu_get_choice(m, &choice);

	menu_destroy(m);

	/*
	 * err == 1 means we got a choice back from menu_get_choice.
	 *
	 * err == -ENOENT if the menu was setup to select the default but no
	 * default was set. in that case, we should continue trying to boot
	 * labels that haven't been attempted yet.
	 *
	 * otherwise, the user interrupted or there was some other error and
	 * we give up.
	 */

	if (err == 1)
		label_boot(choice);
	else if (err != -ENOENT)
		return;

	boot_unattempted_labels(cfg);
}

/*
 * Boots a system using a pxe file
 *
 * Returns 0 on success, 1 on error.
 */
static int
do_pxe_boot(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned long pxefile_addr_r;
	struct pxe_menu *cfg;
	char *pxefile_addr_str;

	do_getfile = do_get_tftp;

	if (argc == 1) {
		pxefile_addr_str = from_env("pxefile_addr_r");
		if (!pxefile_addr_str)
			return 1;

	} else if (argc == 2) {
		pxefile_addr_str = argv[1];
	} else {
		return CMD_RET_USAGE;
	}

	if (strict_strtoul(pxefile_addr_str, 16, &pxefile_addr_r) < 0) {
		printf("Invalid pxefile address: %s\n", pxefile_addr_str);
		return 1;
	}

	cfg = parse_pxefile((char *)(pxefile_addr_r));

	if (cfg == NULL) {
		printf("Error parsing config file\n");
		return 1;
	}

	handle_pxe_menu(cfg);

	destroy_pxe_menu(cfg);

	return 0;
}

static cmd_tbl_t cmd_pxe_sub[] = {
	U_BOOT_CMD_MKENT(get, 1, 1, do_pxe_get, "", ""),
	U_BOOT_CMD_MKENT(boot, 2, 1, do_pxe_boot, "", "")
};

int do_pxe(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *cp;

	if (argc < 2)
		return CMD_RET_USAGE;

	/* drop initial "pxe" arg */
	argc--;
	argv++;

	cp = find_cmd_tbl(argv[0], cmd_pxe_sub, ARRAY_SIZE(cmd_pxe_sub));

	if (cp)
		return cp->cmd(cmdtp, flag, argc, argv);

	return CMD_RET_USAGE;
}

U_BOOT_CMD(
	pxe, 3, 1, do_pxe,
	"commands to get and boot from pxe files",
	"get - try to retrieve a pxe file using tftp\npxe "
	"boot [pxefile_addr_r] - boot from the pxe file at pxefile_addr_r\n"
);

/*
 * Boots a system using a local disk syslinux/extlinux file
 *
 * Returns 0 on success, 1 on error.
 */
int do_sysboot(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned long pxefile_addr_r;
	struct pxe_menu *cfg;
	char *pxefile_addr_str;
	char *filename;
	int prompt = 0;

	if (strstr(argv[1], "-p")) {
		prompt = 1;
		argc--;
		argv++;
	}

	if (argc < 4)
		return cmd_usage(cmdtp);

	if (argc < 5) {
		pxefile_addr_str = from_env("pxefile_addr_r");
		if (!pxefile_addr_str)
			return 1;
	} else {
		pxefile_addr_str = argv[4];
	}

	if (argc < 6)
		filename = getenv("bootfile");
	else {
		filename = argv[5];
		setenv("bootfile", filename);
	}

	if (strstr(argv[3], "ext2"))
		do_getfile = do_get_ext2;
	else if (strstr(argv[3], "fat"))
		do_getfile = do_get_fat;
	else {
		printf("Invalid filesystem: %s\n", argv[3]);
		return 1;
	}
	fs_argv[1] = argv[1];
	fs_argv[2] = argv[2];

	if (strict_strtoul(pxefile_addr_str, 16, &pxefile_addr_r) < 0) {
		printf("Invalid pxefile address: %s\n", pxefile_addr_str);
		return 1;
	}

	if (get_pxe_file(filename, (void *)pxefile_addr_r) < 0) {
		printf("Error reading config file\n");
		return 1;
	}

	cfg = parse_pxefile((char *)(pxefile_addr_r));

	if (cfg == NULL) {
		printf("Error parsing config file\n");
		return 1;
	}

	if (prompt)
		cfg->prompt = 1;

	handle_pxe_menu(cfg);

	destroy_pxe_menu(cfg);

	return 0;
}

U_BOOT_CMD(
	sysboot, 7, 1, do_sysboot,
	"command to get and boot from syslinux files",
	"[-p] <interface> <dev[:part]> <ext2|fat> [addr] [filename]\n"
	"    - load and parse syslinux menu file 'filename' from ext2 or fat\n"
	"      filesystem on 'dev' on 'interface' to address 'addr'"
);
