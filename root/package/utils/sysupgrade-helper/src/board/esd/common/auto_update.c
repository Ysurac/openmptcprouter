/*
 * (C) Copyright 2003-2004
 * Gary Jennejohn, DENX Software Engineering, garyj@denx.de.
 * Stefan Roese, esd gmbh germany, stefan.roese@esd-electronics.com
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
#include <image.h>
#include <asm/byteorder.h>
#include <fat.h>
#include <part.h>

#include "auto_update.h"

#ifdef CONFIG_AUTO_UPDATE

#if !defined(CONFIG_CMD_FAT)
#error "must define CONFIG_CMD_FAT"
#endif

extern au_image_t au_image[];
extern int N_AU_IMAGES;

/* where to load files into memory */
#define LOAD_ADDR ((unsigned char *)0x100000)
#define MAX_LOADSZ 0x1c00000

/* externals */
extern int fat_register_device(block_dev_desc_t *, int);
extern int file_fat_detectfs(void);
extern long file_fat_read(const char *, void *, unsigned long);
long do_fat_read (const char *filename, void *buffer,
		  unsigned long maxsize, int dols);
extern int flash_sect_erase(ulong, ulong);
extern int flash_sect_protect (int, ulong, ulong);
extern int flash_write (char *, ulong, ulong);

extern block_dev_desc_t ide_dev_desc[CONFIG_SYS_IDE_MAXDEVICE];

int au_check_cksum_valid(int i, long nbytes)
{
	image_header_t *hdr;

	hdr = (image_header_t *)LOAD_ADDR;
#if defined(CONFIG_FIT)
	if (genimg_get_format ((void *)hdr) != IMAGE_FORMAT_LEGACY) {
		puts ("Non legacy image format not supported\n");
		return -1;
	}
#endif

	if ((au_image[i].type == AU_FIRMWARE) &&
	    (au_image[i].size != image_get_data_size (hdr))) {
		printf ("Image %s has wrong size\n", au_image[i].name);
		return -1;
	}

	if (nbytes != (image_get_image_size (hdr))) {
		printf ("Image %s bad total SIZE\n", au_image[i].name);
		return -1;
	}

	/* check the data CRC */
	if (!image_check_dcrc (hdr)) {
		printf ("Image %s bad data checksum\n", au_image[i].name);
		return -1;
	}
	return 0;
}

int au_check_header_valid(int i, long nbytes)
{
	image_header_t *hdr;

	hdr = (image_header_t *)LOAD_ADDR;
#if defined(CONFIG_FIT)
	if (genimg_get_format ((void *)hdr) != IMAGE_FORMAT_LEGACY) {
		puts ("Non legacy image format not supported\n");
		return -1;
	}
#endif

	/* check the easy ones first */
	if (nbytes < image_get_header_size ()) {
		printf ("Image %s bad header SIZE\n", au_image[i].name);
		return -1;
	}
	if (!image_check_magic (hdr) || !image_check_arch (hdr, IH_ARCH_PPC)) {
		printf ("Image %s bad MAGIC or ARCH\n", au_image[i].name);
		return -1;
	}
	if (!image_check_hcrc (hdr)) {
		printf ("Image %s bad header checksum\n", au_image[i].name);
		return -1;
	}

	/* check the type - could do this all in one gigantic if() */
	if (((au_image[i].type & AU_TYPEMASK) == AU_FIRMWARE) &&
	    !image_check_type (hdr, IH_TYPE_FIRMWARE)) {
		printf ("Image %s wrong type\n", au_image[i].name);
		return -1;
	}
	if (((au_image[i].type & AU_TYPEMASK) == AU_SCRIPT) &&
	    !image_check_type (hdr, IH_TYPE_SCRIPT)) {
		printf ("Image %s wrong type\n", au_image[i].name);
		return -1;
	}

	return 0;
}

int au_do_update(int i, long sz)
{
	image_header_t *hdr;
	char *addr;
	long start, end;
	int off, rc;
	uint nbytes;
	int k;

	hdr = (image_header_t *)LOAD_ADDR;
#if defined(CONFIG_FIT)
	if (genimg_get_format ((void *)hdr) != IMAGE_FORMAT_LEGACY) {
		puts ("Non legacy image format not supported\n");
		return -1;
	}
#endif

	switch (au_image[i].type & AU_TYPEMASK) {
	case AU_SCRIPT:
		printf("Executing script %s\n", au_image[i].name);

		/* execute a script */
		if (image_check_type (hdr, IH_TYPE_SCRIPT)) {
			addr = (char *)((char *)hdr + image_get_header_size ());
			/* stick a NULL at the end of the script, otherwise */
			/* parse_string_outer() runs off the end. */
			addr[image_get_data_size (hdr)] = 0;
			addr += 8;

			/*
			 * Replace cr/lf with ;
			 */
			k = 0;
			while (addr[k] != 0) {
				if ((addr[k] == 10) || (addr[k] == 13)) {
					addr[k] = ';';
				}
				k++;
			}

			run_command(addr, 0);
			return 0;
		}

		break;

	case AU_FIRMWARE:
	case AU_NOR:
	case AU_NAND:
		start = au_image[i].start;
		end = au_image[i].start + au_image[i].size - 1;

		/*
		 * do not update firmware when image is already in flash.
		 */
		if (au_image[i].type == AU_FIRMWARE) {
			char *orig = (char*)start;
			char *new  = (char *)((char *)hdr +
					      image_get_header_size ());
			nbytes = image_get_data_size (hdr);

			while (--nbytes) {
				if (*orig++ != *new++) {
					break;
				}
			}
			if (!nbytes) {
				printf ("Skipping firmware update - "
					"images are identical\n");
				break;
			}
		}

		/* unprotect the address range */
		if (((au_image[i].type & AU_FLAGMASK) == AU_PROTECT) ||
		    (au_image[i].type == AU_FIRMWARE)) {
			flash_sect_protect (0, start, end);
		}

		/*
		 * erase the address range.
		 */
		if (au_image[i].type != AU_NAND) {
			printf ("Updating NOR FLASH with image %s\n",
				au_image[i].name);
			debug ("flash_sect_erase(%lx, %lx);\n", start, end);
			flash_sect_erase (start, end);
		}

		udelay(10000);

		/* strip the header - except for the kernel and ramdisk */
		if (au_image[i].type != AU_FIRMWARE) {
			addr = (char *)hdr;
			off = image_get_header_size ();
			nbytes = image_get_image_size (hdr);
		} else {
			addr = (char *)((char *)hdr + image_get_header_size ());
			off = 0;
			nbytes = image_get_data_size (hdr);
		}

		/*
		 * copy the data from RAM to FLASH
		 */
		if (au_image[i].type != AU_NAND) {
			debug ("flash_write(%p, %lx, %x)\n",
			       addr, start, nbytes);
			rc = flash_write ((char *)addr, start,
					  (nbytes + 1) & ~1);
		} else {
			rc = -1;
		}
		if (rc != 0) {
			printf ("Flashing failed due to error %d\n", rc);
			return -1;
		}

		/*
		 * check the dcrc of the copy
		 */
		if (au_image[i].type != AU_NAND) {
			rc = crc32 (0, (uchar *)(start + off),
				    image_get_data_size (hdr));
		}
		if (rc != image_get_dcrc (hdr)) {
			printf ("Image %s Bad Data Checksum After COPY\n",
				au_image[i].name);
			return -1;
		}

		/* protect the address range */
		/* this assumes that ONLY the firmware is protected! */
		if (((au_image[i].type & AU_FLAGMASK) == AU_PROTECT) ||
		    (au_image[i].type == AU_FIRMWARE)) {
			flash_sect_protect (1, start, end);
		}

		break;

	default:
		printf("Wrong image type selected!\n");
	}

	return 0;
}

static void process_macros (const char *input, char *output)
{
	char c, prev;
	const char *varname_start = NULL;
	int inputcnt  = strlen (input);
	int outputcnt = CONFIG_SYS_CBSIZE;
	int state = 0;	/* 0 = waiting for '$'	*/
			/* 1 = waiting for '(' or '{' */
			/* 2 = waiting for ')' or '}' */
			/* 3 = waiting for '''  */
#ifdef DEBUG_PARSER
	char *output_start = output;

	printf ("[PROCESS_MACROS] INPUT len %d: \"%s\"\n",
		strlen(input), input);
#endif

	prev = '\0';			/* previous character */

	while (inputcnt && outputcnt) {
	    c = *input++;
	    inputcnt--;

	    if (state != 3) {
	    /* remove one level of escape characters */
	    if ((c == '\\') && (prev != '\\')) {
		if (inputcnt-- == 0)
			break;
		prev = c;
		c = *input++;
	    }
	    }

	    switch (state) {
	    case 0:			/* Waiting for (unescaped) $ */
		if ((c == '\'') && (prev != '\\')) {
			state = 3;
			break;
		}
		if ((c == '$') && (prev != '\\')) {
			state++;
		} else {
			*(output++) = c;
			outputcnt--;
		}
		break;
	    case 1:			/* Waiting for ( */
		if (c == '(' || c == '{') {
			state++;
			varname_start = input;
		} else {
			state = 0;
			*(output++) = '$';
			outputcnt--;

			if (outputcnt) {
				*(output++) = c;
				outputcnt--;
			}
		}
		break;
	    case 2:			/* Waiting for )	*/
		if (c == ')' || c == '}') {
			int i;
			char envname[CONFIG_SYS_CBSIZE], *envval;
			/* Varname # of chars */
			int envcnt = input - varname_start - 1;

			/* Get the varname */
			for (i = 0; i < envcnt; i++) {
				envname[i] = varname_start[i];
			}
			envname[i] = 0;

			/* Get its value */
			envval = getenv (envname);

			/* Copy into the line if it exists */
			if (envval != NULL)
				while ((*envval) && outputcnt) {
					*(output++) = *(envval++);
					outputcnt--;
				}
			/* Look for another '$' */
			state = 0;
		}
		break;
	    case 3:			/* Waiting for '	*/
		if ((c == '\'') && (prev != '\\')) {
			state = 0;
		} else {
			*(output++) = c;
			outputcnt--;
		}
		break;
	    }
	    prev = c;
	}

	if (outputcnt)
		*output = 0;

#ifdef DEBUG_PARSER
	printf ("[PROCESS_MACROS] OUTPUT len %d: \"%s\"\n",
		strlen (output_start), output_start);
#endif
}

/*
 * this is called from board_init() after the hardware has been set up
 * and is usable. That seems like a good time to do this.
 * Right now the return value is ignored.
 */
int do_auto_update(void)
{
	block_dev_desc_t *stor_dev = NULL;
	long sz;
	int i, res, cnt, old_ctrlc;
	char buffer[32];
	char str[80];
	int n;

	if  (ide_dev_desc[0].type != DEV_TYPE_UNKNOWN) {
		stor_dev = get_dev ("ide", 0);
		if (stor_dev == NULL) {
			debug ("ide: unknown device\n");
			return -1;
		}
	}

	if (fat_register_device (stor_dev, 1) != 0) {
		debug ("Unable to register ide disk 0:1\n");
		return -1;
	}

	/*
	 * Check if magic file is present
	 */
	if ((n = do_fat_read (AU_MAGIC_FILE, buffer,
			      sizeof(buffer), LS_NO)) <= 0) {
		debug ("No auto_update magic file (n=%d)\n", n);
		return -1;
	}

#ifdef CONFIG_AUTO_UPDATE_SHOW
	board_auto_update_show (1);
#endif
	puts("\nAutoUpdate Disk detected! Trying to update system...\n");

	/* make sure that we see CTRL-C and save the old state */
	old_ctrlc = disable_ctrlc (0);

	/* just loop thru all the possible files */
	for (i = 0; i < N_AU_IMAGES; i++) {
		/*
		 * Try to expand the environment var in the fname
		 */
		process_macros (au_image[i].name, str);
		strcpy (au_image[i].name, str);

		printf("Reading %s ...", au_image[i].name);
		/* just read the header */
		sz = do_fat_read (au_image[i].name, LOAD_ADDR,
				  image_get_header_size (), LS_NO);
		debug ("read %s sz %ld hdr %d\n",
			au_image[i].name, sz, image_get_header_size ());
		if (sz <= 0 || sz < image_get_header_size ()) {
			puts(" not found\n");
			continue;
		}
		if (au_check_header_valid (i, sz) < 0) {
			puts(" header not valid\n");
			continue;
		}
		sz = do_fat_read (au_image[i].name, LOAD_ADDR,
				  MAX_LOADSZ, LS_NO);
		debug ("read %s sz %ld hdr %d\n",
			au_image[i].name, sz, image_get_header_size ());
		if (sz <= 0 || sz <= image_get_header_size ()) {
			puts(" not found\n");
			continue;
		}
		if (au_check_cksum_valid (i, sz) < 0) {
			puts(" checksum not valid\n");
			continue;
		}
		puts(" done\n");

		do {
			res = au_do_update (i, sz);
			/* let the user break out of the loop */
			if (ctrlc() || had_ctrlc ()) {
				clear_ctrlc ();
				break;
			}
			cnt++;
		} while (res < 0);
	}

	/* restore the old state */
	disable_ctrlc (old_ctrlc);

	puts("AutoUpdate finished\n\n");
#ifdef CONFIG_AUTO_UPDATE_SHOW
	board_auto_update_show (0);
#endif

	return 0;
}

int auto_update(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	do_auto_update();

	return 0;
}
U_BOOT_CMD(
	autoupd,	1,	1,	auto_update,
	"Automatically update images",
	""
);
#endif /* CONFIG_AUTO_UPDATE */
