/*
 * (C) Copyright 2014 The Linux Foundation. All rights reserved.
 * (C) Copyright 2008 Semihalf
 *
 * (C) Copyright 2000-2004
 * DENX Software Engineering
 * Wolfgang Denk, wd@denx.de
 *
 * Updated-by: Prafulla Wadaskar <prafulla@marvell.com>
 *		FIT image specific code abstracted from mkimage.c
 *		some functions added to address abstraction
 *
 * All rights reserved.
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

#include "imagetool.h"
#include "mkimage.h"
#include <image.h>
#include <u-boot/crc.h>

static image_header_t header;

static int fit_verify_header (unsigned char *ptr, int image_size,
			struct image_tool_params *params)
{
	return fdt_check_header ((void *)ptr);
}

static int fit_check_image_types (uint8_t type)
{
	if (type == IH_TYPE_FLATDT)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

/**
 * fit_handle_file - main FIT file processing function
 *
 * fit_handle_file() runs dtc to convert .its to .itb, includes
 * binary data, updates timestamp property and calculates hashes.
 *
 * datafile  - .its file
 * imagefile - .itb file
 *
 * returns:
 *     only on success, otherwise calls exit (EXIT_FAILURE);
 */
static int fit_handle_file (struct image_tool_params *params)
{
	char tmpfile[MKIMAGE_MAX_TMPFILE_LEN];
	char cmd[MKIMAGE_MAX_DTC_CMDLINE_LEN];
	int tfd;
	struct stat sbuf;
	unsigned char *ptr;

	/* Flattened Image Tree (FIT) format  handling */
	debug ("FIT format handling\n");

	/* call dtc to include binary properties into the tmp file */
	if (strlen (params->imagefile) +
		strlen (MKIMAGE_TMPFILE_SUFFIX) + 1 > sizeof (tmpfile)) {
		fprintf (stderr, "%s: Image file name (%s) too long, "
				"can't create tmpfile",
				params->imagefile, params->cmdname);
		return (EXIT_FAILURE);
	}
	sprintf (tmpfile, "%s%s", params->imagefile, MKIMAGE_TMPFILE_SUFFIX);

	/* dtc -I dts -O -p 200 datafile > tmpfile */
	sprintf (cmd, "%s %s %s > %s",
		MKIMAGE_DTC, params->dtc, params->datafile, tmpfile);
	debug ("Trying to execute \"%s\"\n", cmd);
	if (system (cmd) == -1) {
		fprintf (stderr, "%s: system(%s) failed: %s\n",
				params->cmdname, cmd, strerror(errno));
		unlink (tmpfile);
		return (EXIT_FAILURE);
	}

	/* load FIT blob into memory */
	tfd = open (tmpfile, O_RDWR|O_BINARY);

	if (tfd < 0) {
		fprintf (stderr, "%s: Can't open %s: %s\n",
				params->cmdname, tmpfile, strerror(errno));
		unlink (tmpfile);
		return (EXIT_FAILURE);
	}

	if (fstat (tfd, &sbuf) < 0) {
		fprintf (stderr, "%s: Can't stat %s: %s\n",
				params->cmdname, tmpfile, strerror(errno));
		unlink (tmpfile);
		return (EXIT_FAILURE);
	}

	ptr = mmap (0, sbuf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED,
				tfd, 0);
	if (ptr == MAP_FAILED) {
		fprintf (stderr, "%s: Can't read %s: %s\n",
				params->cmdname, tmpfile, strerror(errno));
		unlink (tmpfile);
		return (EXIT_FAILURE);
	}

	/* check if ptr has a valid blob */
	if (fdt_check_header (ptr)) {
		fprintf (stderr, "%s: Invalid FIT blob\n", params->cmdname);
		unlink (tmpfile);
		return (EXIT_FAILURE);
	}

	/* set hashes for images in the blob */
	if (fit_set_hashes (ptr)) {
		fprintf (stderr, "%s Can't add hashes to FIT blob",
				params->cmdname);
		unlink (tmpfile);
		return (EXIT_FAILURE);
	}

	/* add a timestamp at offset 0 i.e., root  */
	if (fit_set_timestamp (ptr, 0, sbuf.st_mtime)) {
		fprintf (stderr, "%s: Can't add image timestamp\n",
				params->cmdname);
		unlink (tmpfile);
		return (EXIT_FAILURE);
	}
	debug ("Added timestamp successfully\n");

	munmap ((void *)ptr, sbuf.st_size);
	close (tfd);

	if (rename (tmpfile, params->imagefile) == -1) {
		fprintf (stderr, "%s: Can't rename %s to %s: %s\n",
				params->cmdname, tmpfile, params->imagefile,
				strerror (errno));
		unlink (tmpfile);
		unlink (params->imagefile);
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

static int fit_save_datafile (struct image_tool_params *params,
				const void *file_data, size_t file_size)
{
	int dfd;
	const char *datafile = params->outfile;

	dfd = open(datafile, O_RDWR | O_CREAT | O_TRUNC | O_BINARY,
		   S_IRUSR | S_IWUSR);
	if (dfd < 0) {
		fprintf(stderr, "%s: Can't open \"%s\": %s\n",
			params->cmdname, datafile, strerror(errno));
		return (EXIT_FAILURE);
	}

	if (write(dfd, (void *)file_data, file_size) != (ssize_t)file_size) {
		fprintf(stderr, "%s: Write error on \"%s\": %s\n",
			params->cmdname, datafile, strerror(errno));
		close(dfd);
		return (EXIT_FAILURE);
	}

	close(dfd);

	return (EXIT_SUCCESS);
}

static int fit_extract_datafile (void *ptr, struct image_tool_params *params)
{
	const void *fit = (const void *) ptr;
	int images_noffset, noffset, ndepth;
	const void *file_data = NULL;
	size_t file_size;

	images_noffset = fdt_path_offset(fit, FIT_IMAGES_PATH);

	/* check if ptr has a valid blob */
	if (fdt_check_header (fit)) {
		fprintf (stderr, "%s: Invalid FIT blob\n", params->cmdname);
		unlink (params->imagefile);
		return (EXIT_FAILURE);
	}

	ndepth = 0;
	noffset = fdt_next_node(fit, images_noffset, &ndepth);
	while (noffset >= 0 && ndepth > 0) {
		const char *name = fdt_get_name(fit, noffset, NULL);

		if (ndepth != 1)
			goto next_node;

		if (strcmp(params->datafile, name)) {
			/* This is not the image you're looking for... */
			goto next_node;
		}

		if (!fit_image_check_hashes(fit, noffset)) {
			fprintf(stderr,
				"%s: Invalid hash in image %s, aborting\n",
				params->cmdname, params->datafile);
			unlink (params->imagefile);
			return (EXIT_FAILURE);
		}

		if (fit_image_get_data(fit, noffset, &file_data, &file_size)) {
			fprintf(stderr,
				"%s: Can't read data in image %s, aborting\n",
				params->cmdname, params->datafile);
			unlink(params->imagefile);
			return (EXIT_FAILURE);
		}

		break;
next_node:
		noffset = fdt_next_node(fit, noffset, &ndepth);
	}

	if (!file_data) {
		fprintf(stderr, "%s: Can't find image name %s\n",
			params->cmdname, params->datafile);
		return (EXIT_FAILURE);
	}

	return fit_save_datafile(params, file_data, file_size);
}

static int fit_check_params (struct image_tool_params *params)
{
	return	((params->dflag && (params->fflag || params->lflag)) ||
		(params->fflag && (params->dflag || params->lflag)) ||
		(params->lflag && (params->dflag || params->fflag)));
}

static struct image_type_params fitimage_params = {
	.name = "FIT Image support",
	.header_size = sizeof(image_header_t),
	.hdr = (void*)&header,
	.verify_header = fit_verify_header,
	.print_header = fit_print_contents,
	.check_image_type = fit_check_image_types,
	.fflag_handle = fit_handle_file,
	.set_header = NULL,	/* FIT images use DTB header */
	.extract_datafile = fit_extract_datafile,
	.check_params = fit_check_params,
};

void init_fit_image_type (void)
{
	register_image_type (&fitimage_params);
}
