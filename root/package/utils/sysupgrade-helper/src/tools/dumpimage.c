/*
 * Based on mkimage.c.
 *
 * Written by Guilherme Maciel Ferreira <guilherme.maciel.ferreira@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include "dumpimage.h"
#include <image.h>
#include <version.h>
#include <sysupgrade.h>
static void usage(void);

/* image_type_params linked list to maintain registered image types supports */
static struct image_type_params *dumpimage_tparams;

/* parameters initialized by core will be used by the image type code */
static struct image_tool_params params = {
	.type = IH_TYPE_KERNEL,
};

/**
 * dumpimage_register() - register respective image generation/list support
 *
 * the input struct image_type_params is checked and appended to the link
 * list, if the input structure is already registered, issue an error
 *
 * @tparams: Image type parameters
 */
static void dumpimage_register(struct image_type_params *tparams)
{
	struct image_type_params **tp;

	if (!tparams) {
		fprintf(stderr, "%s: %s: Null input\n", params.cmdname,
			__func__);
		exit(EXIT_FAILURE);
	}

	/* scan the linked list, check for registry and point the last one */
	for (tp = &dumpimage_tparams; *tp != NULL; tp = &(*tp)->next) {
		if (!strcmp((*tp)->name, tparams->name)) {
			fprintf(stderr, "%s: %s already registered\n",
				params.cmdname, tparams->name);
			return;
		}
	}

	/* add input struct entry at the end of link list */
	*tp = tparams;
	/* mark input entry as last entry in the link list */
	tparams->next = NULL;

	debug("Registered %s\n", tparams->name);
}

/**
 * dumpimage_get_type() - find the image type params for a given image type
 *
 * Scan all registered image types and check the input type_id for each
 * supported image type
 *
 * @return respective image_type_params pointer. If the input type is not
 * supported by any of registered image types, returns NULL
 */
static struct image_type_params *dumpimage_get_type(int type)
{
	struct image_type_params *curr;

	for (curr = dumpimage_tparams; curr != NULL; curr = curr->next) {
		if (curr->check_image_type) {
			if (!curr->check_image_type(type))
				return curr;
		}
	}
	return NULL;
}

/*
 * dumpimage_verify_print_header() - verifies the image header
 *
 * Scan registered image types and verify the image_header for each
 * supported image type. If verification is successful, this prints
 * the respective header.
 *
 * @return 0 on success, negative if input image format does not match with
 * any of supported image types
 */
static int dumpimage_verify_print_header(void *ptr, struct stat *sbuf)
{
	int retval = -1;
	struct image_type_params *curr;

	for (curr = dumpimage_tparams; curr != NULL; curr = curr->next) {
		if (curr->verify_header) {
			retval = curr->verify_header((unsigned char *)ptr,
						     sbuf->st_size, &params);
			if (retval != 0)
				continue;
			/*
			 * Print the image information  if verify is
			 * successful
			 */
			if (curr->print_header) {
				curr->print_header(ptr);
			} else {
				fprintf(stderr,
					"%s: print_header undefined for %s\n",
					params.cmdname, curr->name);
			}
			break;
		}
	}

	return retval;
}

/*
 * dumpimage_extract_datafile -
 *
 * It scans all registered image types,
 * verifies image_header for each supported image type
 * if verification is successful, it extracts the desired file,
 * indexed by pflag, from the image
 *
 * returns negative if input image format does not match with any of
 * supported image types
 */
static int dumpimage_extract_datafile(void *ptr, struct stat *sbuf)
{
	int retval = -1;
	struct image_type_params *curr;

	for (curr = dumpimage_tparams; curr != NULL; curr = curr->next) {
		if (curr->verify_header) {
			retval = curr->verify_header((unsigned char *)ptr,
						     sbuf->st_size, &params);
			if (retval != 0)
				continue;
			/*
			 * Extract the file from the image
			 * if verify is successful
			 */
			if (curr->extract_datafile) {
				retval = curr->extract_datafile(ptr, &params);
				break;
			} else {
				fprintf(stderr,
					"%s: extract_datafile undefined for %s\n",
					params.cmdname, curr->name);
			break;
			}
		}
	}

	return retval;
}

int main(int argc, char **argv)
{
	int opt;
	int ifd = -1;
	struct stat sbuf;
	char *ptr;
	int retval = 0;
	struct image_type_params *tparams = NULL;

	/* Init all image generation/list support */
	register_image_tool(dumpimage_register);

	params.cmdname = *argv;

	while ((opt = getopt(argc, argv, "c:li:o:T:p:V")) != -1) {
		switch (opt) {
		case 'l':
			params.lflag = 1;
			break;
		case 'i':
			params.imagefile = optarg;
			params.iflag = 1;
			break;
		case 'o':
			params.outfile = optarg;
			break;
		case 'T':
			/* for compatibility with new u-boot */
			break;
		case 'p':
			params.pflag = strtoul(optarg, &ptr, 10);
			if (*ptr) {
				fprintf(stderr,
					"%s: invalid file position %s\n",
					params.cmdname, *argv);
				exit(EXIT_FAILURE);
			}
			break;
		case 'V':
			printf("dumpimage version %s\n", PLAIN_VERSION);
			exit(EXIT_SUCCESS);
		case 'c':
			return do_board_upgrade_check(optarg);
			break;
		default:
			usage();
		}
	}

	if (optind >= argc)
		usage();

	/* set tparams as per input type_id */
	tparams = dumpimage_get_type(params.type);
	if (tparams == NULL) {
		fprintf(stderr, "%s: unsupported type %s\n",
			params.cmdname, genimg_get_type_name(params.type));
		exit(EXIT_FAILURE);
	}

	/*
	 * check the passed arguments parameters meets the requirements
	 * as per image type to be generated/listed
	 */
	if (tparams->check_params) {
		if (tparams->check_params(&params))
			usage();
	}

	if (params.iflag)
		params.datafile = argv[optind];
	else
		params.imagefile = argv[optind];
	if (!params.outfile)
		params.outfile = params.datafile;

	ifd = open(params.imagefile, O_RDONLY|O_BINARY);
	if (ifd < 0) {
		fprintf(stderr, "%s: Can't open \"%s\": %s\n",
			params.cmdname, params.imagefile,
			strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (params.lflag || params.iflag) {
		if (fstat(ifd, &sbuf) < 0) {
			fprintf(stderr, "%s: Can't stat \"%s\": %s\n",
				params.cmdname, params.imagefile,
				strerror(errno));
			exit(EXIT_FAILURE);
		}

		if ((unsigned)sbuf.st_size < tparams->header_size) {
			fprintf(stderr,
				"%s: Bad size: \"%s\" is not valid image\n",
				params.cmdname, params.imagefile);
			exit(EXIT_FAILURE);
		}

		ptr = mmap(0, sbuf.st_size, PROT_READ, MAP_SHARED, ifd, 0);
		if (ptr == MAP_FAILED) {
			fprintf(stderr, "%s: Can't read \"%s\": %s\n",
				params.cmdname, params.imagefile,
				strerror(errno));
			exit(EXIT_FAILURE);
		}

		/*
		 * Both calls bellow scan through dumpimage registry for all
		 * supported image types and verify the input image file
		 * header for match
		 */
		if (params.iflag) {
			/*
			 * Extract the data files from within the matched
			 * image type. Returns the error code if not matched
			 */
			retval = dumpimage_extract_datafile(ptr, &sbuf);
		} else {
			/*
			 * Print the image information for matched image type
			 * Returns the error code if not matched
			 */
			retval = dumpimage_verify_print_header(ptr, &sbuf);
		}

		(void)munmap((void *)ptr, sbuf.st_size);
		(void)close(ifd);

		return retval;
	}

	(void)close(ifd);

	return EXIT_SUCCESS;
}

static void usage(void)
{
	fprintf(stderr, "Usage: %s -l image\n"
		"          -l ==> list image header information\n",
		params.cmdname);
	fprintf(stderr,
		"       %s -i image [-p position] [-o outfile] data_file\n"
		"          -i ==> extract from the 'image' a specific 'data_file'"
		", indexed by 'position' (starting at 0)\n",
		params.cmdname);
	fprintf(stderr,
		"       %s -V ==> print version information and exit\n",
		params.cmdname);
	fprintf(stderr,
		"       %s -c image\n"
		"          -c ==> do board upgrade check\n",
		params.cmdname);

	exit(EXIT_FAILURE);
}
