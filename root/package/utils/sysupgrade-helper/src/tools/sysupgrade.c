/*
 * Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "dumpimage.h"
#include "sysupgrade.h"
#include <elf.h>
#include <dirent.h>
#include <../drivers/mtd/ubi/ubi-media.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>
#include <limits.h>

#define SBL_VERSION_FILE       "sbl_version"
#define TZ_VERSION_FILE        "tz_version"
#define HLOS_VERSION_FILE      "hlos_version"
#define APPSBL_VERSION_FILE    "appsbl_version"
#define RPM_VERSION_FILE       "rpm_version"
#define VERSION_FILE_BASENAME  "/sys/devices/system/qfprom/qfprom0/"
#define AUTHENTICATE_FILE	"/sys/devices/system/qfprom/qfprom0/authenticate"
#define TEMP_KERNEL_PATH	"/tmp/tmp_kernel.bin"
#define MAX_SBL_VERSION	11
#define MAX_HLOS_VERSION	32
#define MAX_TZ_VERSION		14
#define MAX_APPSBL_VERSION	14
#define MAX_RPM_VERSION	8
#define HASH_P_FLAG		0x02200000
#define TMP_FILE_DIR		"/tmp/"
#define CERT_SIZE		2048
#define PRESENT		1
#define MBN_HDR_SIZE		40
#define SBL_HDR_SIZE		80
#define SIG_SIZE		256
#define NOT_PRESENT		0
#define SIG_CERT_2_SIZE	4352
#define SIG_CERT_3_SIZE	6400
#define SBL_NAND_PREAMBLE	10240
#define SBL_HDR_RESERVED	12

#define ARRAY_SIZE(array)	sizeof(array)/sizeof(array[0])

struct image_section sections[] = {
	{
		.section_type		= UBOOT_TYPE,
		.type			= "u-boot",
		.max_version		= MAX_APPSBL_VERSION,
		.file			= TMP_FILE_DIR,
		.version_file		= APPSBL_VERSION_FILE,
		.is_present		= NOT_PRESENT,
	},
	{
		.section_type		= HLOS_TYPE,
		.type			= "hlos",
		.max_version		= MAX_HLOS_VERSION,
		.file			= TMP_FILE_DIR,
		.version_file		= HLOS_VERSION_FILE,
		.is_present		= NOT_PRESENT,
	},
	{
		.section_type		= HLOS_TYPE,
		.type			= "ubi",
		.tmp_file		= TMP_FILE_DIR,
		.pre_op			= extract_kernel_binary,
		.max_version		= MAX_HLOS_VERSION,
		.file			= TEMP_KERNEL_PATH,
		.version_file		= HLOS_VERSION_FILE,
		.is_present		= NOT_PRESENT,
	},
	{
		.section_type		= TZ_TYPE,
		.type			= "tz",
		.max_version		= MAX_TZ_VERSION,
		.file			= TMP_FILE_DIR,
		.version_file		= TZ_VERSION_FILE,
		.is_present		= NOT_PRESENT,
	},
	{
		.section_type		= SBL_TYPE,
		.type			= "sbl1",
		.max_version		= MAX_SBL_VERSION,
		.file			= TMP_FILE_DIR,
		.version_file		= SBL_VERSION_FILE,
		.is_present		= NOT_PRESENT,
	},
	{
		.section_type		= SBL_TYPE,
		.type			= "sbl2",
		.max_version		= MAX_SBL_VERSION,
		.file			= TMP_FILE_DIR,
		.version_file		= SBL_VERSION_FILE,
		.is_present		= NOT_PRESENT,
	},
	{
		.section_type		= SBL_TYPE,
		.type			= "sbl3",
		.max_version		= MAX_SBL_VERSION,
		.file			= TMP_FILE_DIR,
		.version_file		= SBL_VERSION_FILE,
		.is_present		= NOT_PRESENT,
	},
	{
		.section_type		= RPM_TYPE,
		.type			= "rpm",
		.max_version		= MAX_RPM_VERSION,
		.file			= TMP_FILE_DIR,
		.version_file		= RPM_VERSION_FILE,
		.is_present		= NOT_PRESENT,
	},
};

#define NO_OF_SECTIONS		ARRAY_SIZE(sections)
int src_size;

int check_mbn_elf(struct image_section **sec)
{
	int fd = open((*sec)->file, O_RDONLY);
	struct stat sb;
	uint8_t *fp;
	Elf32_Ehdr *elf;

	if (fd < 0) {
		perror((*sec)->file);
		return 0;
	}

	if (fstat(fd, &sb) == -1) {
		perror("fstat");
		close(fd);
		return 0;
	}

	fp = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (fp == MAP_FAILED) {
		perror("mmap");
		close(fd);
		return 0;
	}

	elf = (Elf32_Ehdr *)fp;
	if (!strncmp((char *)&(elf->e_ident[1]), "ELF", 3)) {
		(*sec)->get_sw_id = get_sw_id_from_component_bin_elf;
		(*sec)->split_components = split_code_signature_cert_from_component_bin_elf;
	} else if (!strncmp((char *)&(((Elf32_Ehdr *)(fp + SBL_NAND_PREAMBLE))->e_ident[1]), "ELF", 3)) {
		(*sec)->get_sw_id = get_sw_id_from_component_bin_elf;
		(*sec)->split_components = split_code_signature_cert_from_component_bin_elf;
	} else {
		(*sec)->get_sw_id = get_sw_id_from_component_bin;
		(*sec)->split_components = split_code_signature_cert_from_component_bin;
	}

	return 1;
}

int get_sections(void)
{
	DIR *dir = opendir(TMP_FILE_DIR);
	struct dirent *file;
	int i;
	struct image_section *sec;

	if (dir == NULL) {
		printf("Error accessing the image directory\n");
		return 0;
	}

	while ((file = readdir(dir)) != NULL) {
		for (i = 0, sec = &sections[0]; i < NO_OF_SECTIONS; i++, sec++) {
			if (strstr(file->d_name, sec->type)) {
				if (sec->pre_op) {
					strlcat(sec->tmp_file, file->d_name,
							sizeof(sec->tmp_file));
					if (!sec->pre_op(sec)) {
						printf("Error extracting kernel from ubi\n");
						return 0;
					}
				} else {
					strlcat(sec->file, file->d_name,
							sizeof(sec->file));
				}
				if (!check_mbn_elf(&sec)) {
					closedir(dir);
					return 0;
				}
				if (!sec->get_sw_id(sec)) {
					closedir(dir);
					return 0;
				}
				get_local_image_version(sec);
				sec->is_present = PRESENT;
				break;
			}
		}
	}
	closedir(dir);
	return 1;
}

/**
 * is_authentication_check_enabled() - checks whether installed image is
 * secure(1) or not(0)
 *
 */
int is_authentication_check_enabled(void)
{
	int fd = open(AUTHENTICATE_FILE, O_RDONLY);
	char authenticate_string[4];
	int len;

	if (fd == -1) {
		perror(AUTHENTICATE_FILE);
		return 0;
	}

	len = read(fd, authenticate_string, 1);
	close(fd);

	if (len > 0 && authenticate_string[0] == '0') {
		return 0;
	}

	return 1;
}

/**
 * get_local_image_version() check the version file & if it exists, read the
 * value & save it into global variable local_version
 *
 */
int get_local_image_version(struct image_section *section)
{
	int len, fd;
	char local_version_string[16], version_file[64];
	struct stat st;

	snprintf(version_file, sizeof(version_file), "%s%s", VERSION_FILE_BASENAME, section->version_file);
	fd = open(version_file, O_RDONLY);
	if (fd == -1) {
		perror(version_file);
		return 0;
	}

	fstat(fd, &st);

	len = st.st_size < sizeof(local_version_string) - 1 ? st.st_size :
							sizeof(local_version_string) - 1;
	read(fd, local_version_string, len);
	local_version_string[len] = '\0';
	close(fd);

	section->local_version = atoi(local_version_string);
	printf("Local image version:%s\n", local_version_string);

	return 1;
}

/**
 * set_local_image_version() update the version of the image by writing the version
 * to the version file
 *
 */
int set_local_image_version(struct image_section *section)
{
	int fd;
	char version_string[16], version_file[64];
	int len;

	snprintf(version_file, sizeof(version_file), "%s%s", TMP_FILE_DIR, section->version_file);
	fd = open(version_file, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		perror(version_file);
		return 0;
	}

	len = snprintf(version_string, 8, "%d", section->img_version);
	printf("Version to be updated:%s\n", version_string);
	if (write(fd, version_string, len) == -1) {
		printf("Error writing version to %s\n", version_file);
		close(fd);
		return 0;
	}

	close(fd);
	return 1;
}

/**
 * is_version_check_enabled() checks whether version check is
 * enabled(non-zero value) or not
 *
 */
int is_version_check_enabled()
{
	if (get_local_image_version(&sections[0]) != -1) {
		printf("Returning 1 from is_version_check_enabled because local_version_string is non-ZERO\n");
		return 1;
	}

	return 0;
}

char *find_value(char *buffer, char *search, int size)
{
	char *value = malloc(size * sizeof(char));
	int i, j;

	if (value == NULL) {
		return NULL;
	}

	for (i = 0; i < CERT_SIZE; i++) {
		for (j = 0; search[j] && (buffer[i + j] == search[j]); j++);
		if (search[j] == '\0') {
			strlcpy(value, &buffer[i - size], size);
			value[size - 1] = '\0';
			return value;
		}
	}
	free(value);
	return NULL;
}

/**
 * get_sw_id_from_component_bin() parses the MBN header & checks image size v/s
 * code size. If both differ, it means signature & certificates are
 * appended at end.
 * Extract the attestation certificate & read the Subject & retreive the SW_ID.
 *
 * @bin_file: struct image_section *
 */
int get_sw_id_from_component_bin(struct image_section *section)
{
	Mbn_Hdr *mbn_hdr;
	int fd = open(section->file, O_RDONLY);
	struct stat sb;
	uint8_t *fp;
	int cert_offset;
	char *sw_version;
	int sig_cert_size;

	if (fd == -1) {
		perror(section->file);
		return 0;
	}

	if (fstat(fd, &sb) == -1) {
		perror("fstat");
		close(fd);
		return 0;
	}

	fp = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (fp == MAP_FAILED) {
		perror("mmap");
		close(fd);
		return 0;
	}

	mbn_hdr = (Mbn_Hdr *)fp;
	if (strstr(section->file, sections[4].type)) {
		uint32_t preamble = sections[2].is_present ? SBL_NAND_PREAMBLE : 0;
		Sbl_Hdr *sbl_hdr = (Sbl_Hdr *)(fp + preamble);

		sig_cert_size = sbl_hdr->image_size - sbl_hdr->code_size;
		cert_offset = preamble + sbl_hdr->cert_ptr - sbl_hdr->image_dest_ptr +
				SBL_HDR_SIZE;
	} else {
		sig_cert_size = mbn_hdr->image_size - mbn_hdr->code_size;
		cert_offset = mbn_hdr->cert_ptr - mbn_hdr->image_dest_ptr + 40;
	}
	if (sig_cert_size != SIG_CERT_2_SIZE && sig_cert_size != SIG_CERT_3_SIZE) {
		printf("Error: Image without version information\n");
		close(fd);
		return 0;
	}

	printf("Image with version information\n");
	sw_version = find_value((char *)(fp + cert_offset), "SW_ID", 17);
	if (sw_version != NULL) {
		sw_version[8] = '\0';
		sscanf(sw_version, "%x", &section->img_version);
		printf("SW ID:%d\n", section->img_version);
		free(sw_version);
	}

	close(fd);
	return 1;
}

int process_elf(char *bin_file, uint8_t **fp, Elf32_Ehdr **elf, Elf32_Phdr **phdr, Mbn_Hdr **mbn_hdr)
{
	int fd = open(bin_file, O_RDONLY);
	struct stat sb;
	int version = 0;
	int i = 0;

	if (fd < 0) {
		perror(bin_file);
		return 0;
	}

	if (fstat(fd, &sb) == -1) {
		perror("fstat");
		close(fd);
		return 0;
	}

	*fp = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (*fp == MAP_FAILED) {
		perror("mmap");
		close(fd);
		return 0;
	}

	*elf = (Elf32_Ehdr *)*fp;
	while (strncmp((char *)&((*elf)->e_ident[1]), "ELF", 3)) {
		*fp = (uint8_t *)((char *)(*fp) + SBL_NAND_PREAMBLE);
		*elf = (Elf32_Ehdr *)*fp;
	}

	*phdr = (Elf32_Phdr *)(*fp + (*elf)->e_phoff);
	for (i = 0; i < (*elf)->e_phnum; i++, (*phdr)++) {
		if ((*phdr)->p_flags == HASH_P_FLAG) {
			*mbn_hdr = (Mbn_Hdr *)(*fp + (*phdr)->p_offset);
			if ((*mbn_hdr)->image_size != (*mbn_hdr)->code_size) {
				version = 1;
				break;
			} else {
				printf("Error: Image without version information\n");
				close(fd);
				return 0;
			}
		}
	}

	if (version != 1) {
		printf("Error: Image without version information\n");
		return 0;
	}

	close(fd);
	return 1;
}

/**
 * get_sw_id_from_component_bin_elf() parses the ELF header to get the MBN header
 * of the hash table segment. Parses the MBN header of hash table segment & checks
 * total size v/s actual component size. If both differ, it means signature &
 * certificates are appended at end.
 * Extract the attestation certificate & read the Subject & retreive the SW_ID.
 *
 * @bin_file: struct image_section *
 */
int get_sw_id_from_component_bin_elf(struct image_section *section)
{
	Elf32_Ehdr *elf;
	Elf32_Phdr *phdr;
	Mbn_Hdr *mbn_hdr;
	uint8_t *fp;
	int cert_offset;
	char *sw_version;

	if (!process_elf(section->file, &fp, &elf, &phdr, &mbn_hdr)) {
		return 0;
	}

	cert_offset = mbn_hdr->cert_ptr - mbn_hdr->image_dest_ptr + 40;
	printf("Image with version information\n");
	sw_version = find_value((char *)(fp + phdr->p_offset + cert_offset), "SW_ID", 17);
	if (sw_version) {
		sw_version[8] = '\0';
		sscanf(sw_version, "%x", &section->img_version);
		printf("SW ID:%d\n", section->img_version);
		free(sw_version);
	}

	return 1;
}

int find_mtd_part_size(void)
{
	char *mtdname = "kernel";
	char prefix[] = "/dev/mtd";
	char dev[PATH_MAX];
	int i = -1, fd;
	int vol_size;
	int flag = 0;
	char mtd_part[256];
	FILE *fp = fopen("/proc/mtd", "r");
	mtd_info_t mtd_dev_info;

	if (fp == NULL) {
		printf("Error finding mtd part\n");
		return -1;
	}

	while (fgets(dev, sizeof(dev), fp)) {
		if (strstr(dev, mtdname)) {
			flag = 1;
			break;
		}
		i++;
	}
	fclose(fp);

	if (flag != 1) {
		printf("%s block not found\n", mtdname);
		return -1;
	}

	snprintf(mtd_part, sizeof(mtd_part), "%s%d", prefix, i);

	fd = open(mtd_part, O_RDWR);
	if (fd == -1) {
		return -1;
	}

	if (ioctl(fd, MEMGETINFO, &mtd_dev_info) == -1) {
		printf("Error getting block size\n");
		close(fd);
		return -1;
	}

	vol_size = mtd_dev_info.erasesize;
	close(fd);

	return vol_size;
}

/**
 * In case of NAND image, Kernel image is ubinized & version information is
 * part of Kernel image. Hence need to un-ubinize the image.
 * To get the kernel image, Find the volume with volume id 0. Kernel image
 * is fragmented and hence to assemble it to get complete image.
 * In UBI image, first look for UBI#, which is magic number used to identify
 * each eraseble block. Parse the UBI header, which starts with UBI# & get
 * the VID(volume ID) header offset as well as Data offset.
 * Traverse to VID header offset & check the volume ID. If it is ZERO, Kernel
 * image is stored in this volume. Use Data offset to extract the Kernel image.
 *
 * @bin_file: struct image_section *
 */
int extract_kernel_binary(struct image_section *section)
{
	struct ubi_ec_hdr *ubi_ec;
	struct ubi_vid_hdr *ubi_vol;
	uint8_t *fp;
	int fd, ofd, magic, data_size, vid_hdr_offset, data_offset;
	struct stat sb;

	fd = open(section->tmp_file, O_RDONLY);
	if (fd < 0) {
		perror(section->tmp_file);
		return 0;
	}

	if (fstat(fd, &sb) == -1) {
		perror("fstat");
		close(fd);
		return 0;
	}

	fp = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (fp == MAP_FAILED) {
		perror("mmap");
		close(fd);
		return 0;
	}

	ofd = open(section->file, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	if (ofd == -1) {
		perror(section->file);
		close(fd);
		return 0;
	}

	data_size = find_mtd_part_size();
	if (data_size == -1) {
		printf("Error finding data size\n");
		return 0;
	}

	ubi_ec = (struct ubi_ec_hdr *)fp;
	magic = be32_to_cpu(ubi_ec->magic);
	while (magic == UBI_EC_HDR_MAGIC) {
		vid_hdr_offset = be32_to_cpu(ubi_ec->vid_hdr_offset);
		data_offset = be32_to_cpu(ubi_ec->data_offset);
		ubi_vol = (struct ubi_vid_hdr *)((uint8_t *)ubi_ec + vid_hdr_offset);
		magic = be32_to_cpu(ubi_vol->magic);
		if (magic != UBI_VID_HDR_MAGIC) {
			printf("Wrong ubi format\n");
			close(ofd);
			close(fd);
			return 0;
		}

		if (ubi_vol->vol_id == 0) {
			write(ofd, (void *)((uint8_t *)ubi_ec + data_offset), data_size);
		}
		if ((int)ubi_vol->vol_id > 0) {
			break;
		}

		ubi_ec = (struct ubi_ec_hdr *)((uint8_t *)ubi_ec + data_offset + data_size);
		magic = be32_to_cpu(ubi_ec->magic);
	}

	close(ofd);
	close(fd);
	printf("Kernel extracted from ubi image\n");
	return 1;
}

/**
 * is_image_version_higher() iterates through each component and check
 * versions against locally installed version.
 * If newer component version is lower than locally insatlled image,
 * abort the FW upgrade process.
 *
 * @img: char *
 */
int is_image_version_higher(void)
{
	int i;
	for (i = 0; i < NO_OF_SECTIONS; i++) {
		if (!sections[i].is_present) {
			continue;
		}

		if (sections[i].local_version > sections[i].img_version) {
			printf("Version of image %s (%d) is lower than minimal supported version(%d)\n",
					sections[i].file,
					sections[i].img_version,
					sections[i].local_version);
			return 0;
		}

		if (sections[i].img_version > sections[i].max_version) {
			printf("Version of image %s (%d) is higher than maximum supported version(%d)\n",
					sections[i].file,
					sections[i].img_version,
					sections[i].max_version);
		}
	}

	return 1;
}

/**
 * Update the version information file based on currently SW_ID being installed.
 *
 */
int update_version(void)
{
	int i;
	for (i = 0; i < NO_OF_SECTIONS; i++) {
		if (!sections[i].is_present) {
			continue;
		}

		if (set_local_image_version(&sections[i]) != 1) {
			printf("Error updating version of %s\n", sections[i].file);
			return 0;
		}
	}

	return 1;
}

int check_image_version(void)
{
	if (is_version_check_enabled() == 0) {
		printf("Version check is not enabled, upgrade to continue !!!\n");
		return 1;
	}

	if (is_image_version_higher() == 0) {
		printf("New image versions are lower than existing image, upgrade to STOP !!!\n");
		return 0;
	}

	if (update_version() != 1) {
		printf("Error while updating verison information\n");
		return 0;
	}
	printf("Update completed!\n");

	return 1;
}

/**
 * split_code_signature_cert_from_component_bin splits the component
 * binary by splitting into code(including MBN header), signature file &
 * attenstation certificate.
 *
 * @bin_file: char *
 * @src: char *
 * @sig: char *
 * @cert: char *
 */
int split_code_signature_cert_from_component_bin(struct image_section *section,
		char **src, char **sig, char **cert)
{
	Mbn_Hdr *mbn_hdr;
	Sbl_Hdr *sbl_hdr;
	int fd = open(section->file, O_RDONLY);
	uint8_t *fp;
	int sig_offset = 0;
	int src_offset = 0;
	int cert_offset = 0;
	struct stat sb;
	int sig_cert_size;

	if (fd == -1) {
		perror(section->file);
		return 0;
	}

	if (fstat(fd, &sb) == -1) {
		perror("fstat");
		close(fd);
		return 0;
	}

	fp = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (fp == MAP_FAILED) {
		perror("mmap");
		close(fd);
		return 0;
	}

	mbn_hdr = (Mbn_Hdr *)fp;
	if (strstr(section->file, sections[4].type)) {
		uint32_t preamble = sections[2].is_present ? SBL_NAND_PREAMBLE : 0;

		sbl_hdr = (Sbl_Hdr *)(fp + preamble);
		src_offset = preamble;
		sig_offset = preamble + sbl_hdr->sig_ptr - sbl_hdr->image_dest_ptr +
				SBL_HDR_SIZE;
		cert_offset = preamble + sbl_hdr->cert_ptr - sbl_hdr->image_dest_ptr +
				SBL_HDR_SIZE;
		sig_cert_size = sbl_hdr->image_size - sbl_hdr->code_size;
		src_size = sbl_hdr->sig_ptr - sbl_hdr->image_dest_ptr + SBL_HDR_SIZE;
	} else {
		sig_cert_size = mbn_hdr->image_size - mbn_hdr->code_size;
		src_size = mbn_hdr->sig_ptr - mbn_hdr->image_dest_ptr + MBN_HDR_SIZE;
		sig_offset += mbn_hdr->sig_ptr - mbn_hdr->image_dest_ptr + MBN_HDR_SIZE;
		cert_offset += mbn_hdr->cert_ptr - mbn_hdr->image_dest_ptr + MBN_HDR_SIZE;
	}

	if (sig_cert_size != SIG_CERT_2_SIZE && sig_cert_size != SIG_CERT_3_SIZE) {
		printf("Error: Image without version information\n");
		close(fd);
		return 0;
	}
        *src = malloc(src_size + 1);
	if (*src == NULL) {
		close(fd);
		return 0;
	}
	memcpy(*src, fp + src_offset, src_size);
	(*src)[src_size] = '\0';

	*sig = malloc((SIG_SIZE + 1) * sizeof(char));
	if (*sig == NULL) {
		free(*src);
		return 0;
	}
	memcpy(*sig, fp + sig_offset, SIG_SIZE);
	(*sig)[SIG_SIZE] = '\0';

	*cert = malloc((CERT_SIZE + 1) * sizeof(char));
	if (*cert == NULL) {
		free(*src);
		free(*sig);
		return 0;
	}
	memcpy(*cert, fp + cert_offset, CERT_SIZE);
	(*cert)[CERT_SIZE] = '\0';

	close(fd);
	return 1;
}

/**
 * split_code_signature_cert_from_component_bin_elf splits the component
 * binary by splitting into code(including ELF header), signature file &
 * attenstation certificate.
 *
 * @bin_file: char *
 * @src: char *
 * @sig: char *
 * @cert: char *
 */
int split_code_signature_cert_from_component_bin_elf(struct image_section *section,
		char **src, char **sig, char **cert)
{
	Elf32_Ehdr *elf;
	Elf32_Phdr *phdr;
	Mbn_Hdr *mbn_hdr;
	uint8_t *fp;
	int sig_offset;
	int cert_offset;
	int len;

	if (!process_elf(section->file, &fp, &elf, &phdr, &mbn_hdr)) {
		return 0;
	}

	sig_offset = mbn_hdr->sig_ptr - mbn_hdr->image_dest_ptr + MBN_HDR_SIZE;
	len = sig_offset;
	*src = malloc((len + 1) * sizeof(char));
	if (*src == NULL) {
		return 0;
	}
	memcpy(*src, fp + phdr->p_offset, len);
	src_size = len;
	(*src)[len] = '\0';

	*sig = malloc((SIG_SIZE + 1) * sizeof(char));
	if (*sig == NULL) {
		free(*src);
		return 0;
	}
	memcpy(*sig, fp + phdr->p_offset + sig_offset, SIG_SIZE);
	(*sig)[SIG_SIZE] = '\0';

	cert_offset = mbn_hdr->cert_ptr - mbn_hdr->image_dest_ptr + MBN_HDR_SIZE;
	*cert = malloc((CERT_SIZE + 1) * sizeof(char));
	if (*cert == NULL) {
		free(*src);
		free(*sig);
		return 0;
	}
	memcpy(*cert, fp + phdr->p_offset + cert_offset, CERT_SIZE);
	(*cert)[CERT_SIZE] = '\0';

	return 1;
}

/**
 * being used to calculate the image hash
 *
 */
#define SW_MASK		0x3636363636363636ull

void generate_swid_ipad(char *sw_id, unsigned long long *swid_xor_ipad)
{
	unsigned long long int val;

	val = strtoull(sw_id, NULL, 16);
	*swid_xor_ipad = val ^ SW_MASK;

	printf("%llx\n", *swid_xor_ipad);
}

/**
 * being used to calculate the image hash
 *
 */
#define HW_ID_MASK		0x5c5c5c5cull
#define OEM_ID_MASK		0x00005c5cull
#define OEM_MODEL_ID_MASK	0x00005c5cull

void generate_hwid_opad(char *hw_id, char *oem_id, char *oem_model_id,
					unsigned long long *hwid_xor_opad)
{
	unsigned long long val;

	val = strtoull(hw_id, NULL, 16);
	*hwid_xor_opad = (((val >> 32) ^ HW_ID_MASK) << 32);

	val = strtoul(oem_id, NULL, 16);
	*hwid_xor_opad |= ((val ^ OEM_ID_MASK) << 16);

	val = strtoul(oem_model_id, NULL, 16);
	*hwid_xor_opad |= (val ^ OEM_MODEL_ID_MASK) & 0xffff;

	printf("%llx\n", *hwid_xor_opad);
}

int create_file(char *name, char *buffer, int size)
{
	int fd;

	fd = open(name, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		perror(name);
		return 0;
	}

	if (write(fd, buffer, size) == -1) {
		close(fd);
		return 0;
	}

	close(fd);
	return 1;
}

char *create_xor_ipad_opad(char *f_xor, unsigned long long *xor_buffer)
{
	int fd;
	char *file;

	file = mktemp(f_xor);
	fd = open(file, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		perror(file);
		return NULL;
	}
	write(fd, xor_buffer, sizeof(*xor_buffer));
	close(fd);
	return file;
}

char *read_file(char *file_name, size_t *file_size)
{
	int fd;
	struct stat st;
	char *buffer;

	fd = open(file_name, O_RDONLY);
	if (fd == -1) {
		perror(file_name);
		return NULL;
	}

	fstat(fd, &st);
	buffer = malloc(st.st_size * sizeof(buffer));
	if (buffer == NULL) {
		close(fd);
		return NULL;
	}
	read(fd, buffer, st.st_size);
	*file_size = (size_t) st.st_size;
	close(fd);
	return buffer;
}

int generate_hash(char *cert, char *sw_file, char *hw_file)
{
	unsigned long long swid_xor_ipad, hwid_xor_opad;
	char *tmp;
	char *sw_id_str = find_value(cert, "SW_ID", 17);
	char *hw_id_str = find_value(cert, "HW_ID", 17);
	char *oem_id_str = find_value(cert, "OEM_ID", 5);
	char *oem_model_id_str = find_value(cert, "MODEL_ID", 5);
	char f_sw_xor[] = "/tmp/swid_xor_XXXXXX";
	char f_hw_xor[] = "/tmp/hwid_xor_XXXXXX";

	if (sw_id_str == NULL || hw_id_str == NULL || oem_id_str == NULL || oem_model_id_str == NULL) {
		if (sw_id_str != NULL) {
			free(sw_id_str);
		}
		if (hw_id_str != NULL) {
			free(hw_id_str);
		}
		if (oem_id_str != NULL) {
			free(oem_id_str);
		}
		if (oem_model_id_str != NULL) {
			free(oem_model_id_str);
		}
		return 0;
	}
	printf("sw_id=%s\thw_id=%s\t", sw_id_str, hw_id_str);
	printf("oem_id=%s\toem_model_id=%s\n", oem_id_str, oem_model_id_str);

	generate_swid_ipad(sw_id_str, &swid_xor_ipad);
	tmp = create_xor_ipad_opad(f_sw_xor, &swid_xor_ipad);
	if (tmp == NULL) {
		free(sw_id_str);
		free(hw_id_str);
		free(oem_id_str);
		free(oem_model_id_str);
		return 0;
	}
	strlcpy(sw_file, tmp, 32);
	free(tmp);

	generate_hwid_opad(hw_id_str, oem_id_str, oem_model_id_str, &hwid_xor_opad);
	tmp = create_xor_ipad_opad(f_hw_xor, &hwid_xor_opad);
	if (tmp == NULL) {
		free(sw_id_str);
		free(hw_id_str);
		free(oem_id_str);
		free(oem_model_id_str);
		return 0;
	}
	strlcpy(hw_file, tmp, 32);
	free(tmp);

	free(sw_id_str);
	free(hw_id_str);
	free(oem_id_str);
	free(oem_model_id_str);

	return 1;
}

void remove_file(char *sw_file, char *hw_file, char *code_file, char *pub_file)
{
	remove(sw_file);
	remove(hw_file);
	remove(code_file);
	remove(pub_file);
	remove("src");
	remove("sig");
	remove("cert");
}

/**
 * is_component_authenticated() usage the code, signature & public key retrieved
 * for each component.
 *
 * @src: char *
 * @sig: char *
 * @cert: char *
 */
int is_component_authenticated(char *src, char *sig, char *cert)
{
	char command[256];
	char *computed_hash;
	char *reference_hash;
	char pub_key[] = "/tmp/pub_keyXXXXXX", *pub_file;
	char code_hash[] = "/tmp/code_hash_XXXXXX", *code_file;
	char tmp_hash[] = "/tmp/tmp_hash_XXXXXX", *tmp_file;
	char f_computed_hash[] = "/tmp/computed_hash_XXXXXX", *computed_file;
	char f_reference_hash[] = "/tmp/reference_hash_XXXXXX", *reference_file;
	char sw_file[32],hw_file[32];
	int retval;
	size_t comp_hash_size, ref_hash_size;

	if (!create_file("src", src, src_size) || !create_file("sig", sig, SIG_SIZE) ||
			!create_file("cert", cert, CERT_SIZE)) {
		return 0;
	}

	pub_file = mktemp(pub_key);
	snprintf(command, sizeof(command),
		"openssl x509 -in cert -pubkey -inform DER -noout > %s", pub_file);
	retval = system(command);
	if (retval != 0) {
		remove("src");
		remove("sig");
		remove("cert");
		printf("Error generating public key\n");
		return 0;
	}

	retval = generate_hash(cert, sw_file, hw_file);
	if (retval == 0) {
		return 0;
	}

	code_file = mktemp(code_hash);
	snprintf(command, sizeof(command),
		"openssl dgst -sha256 -binary -out %s src", code_file);
	retval = system(command);
	if (retval != 0) {
		remove_file(sw_file, hw_file, code_file, pub_file);
		printf("Error in openssl digest\n");
		return 0;
	}

	tmp_file = mktemp(tmp_hash);
	snprintf(command, sizeof(command),
		"cat %s %s | openssl dgst -sha256 -binary -out %s",
						sw_file, code_file, tmp_file);
	retval = system(command);
	if (retval != 0) {
		remove_file(sw_file, hw_file, code_file, pub_file);
		remove(tmp_file);
		printf("Error generating temp has\n");
		return 0;
	}

	computed_file = mktemp(f_computed_hash);
	snprintf(command, sizeof(command),
		"cat %s %s | openssl dgst -sha256 -binary -out %s",
						hw_file, tmp_file, computed_file);
	retval = system(command);
	if (retval != 0) {
		remove_file(sw_file, hw_file, code_file, pub_file);
		remove(tmp_file);
		remove(computed_file);
		printf("Error generating hash\n");
		return 0;
	}

	reference_file = mktemp(f_reference_hash);
	snprintf(command, sizeof(command),
		"openssl rsautl -in sig -pubin -inkey %s -verify > %s",
						pub_file, reference_file);
	retval = system(command);
	if (retval != 0) {
		remove_file(sw_file, hw_file, code_file, pub_file);
		remove(tmp_file);
		remove(computed_file);
		remove(reference_file);
		printf("Error generating reference hash\n");
		return 0;
	}

	computed_hash = read_file(computed_file, &comp_hash_size);
	reference_hash = read_file(reference_file, &ref_hash_size);
	if (computed_hash == NULL || reference_hash == NULL) {
		remove_file(sw_file, hw_file, code_file, pub_file);
		remove(tmp_file);
		remove(computed_file);
		remove(reference_file);
		free(computed_hash?computed_hash:reference_hash);
		return 0;
	}

	remove_file(sw_file, hw_file, code_file, pub_file);
	remove(tmp_file);
	remove(computed_file);
	remove(reference_file);
	if (memcmp(computed_hash, reference_hash, ref_hash_size) ||
			(comp_hash_size != ref_hash_size)) {
		free(computed_hash);
		free(reference_hash);
		printf("Error: Hash or file_size not equal\n");
		return 0;
	}
	free(computed_hash);
	free(reference_hash);
	return 1;
}

/**
 * is_image_authenticated() iterates through each component and check
 * whether individual component is authenticated. If not, abort the FW
 * upgrade process.
 *
 * @img: char *
 */
int is_image_authenticated(void)
{
	int i;
	char *src, *sig, *cert;
	for (i = 0; i < NO_OF_SECTIONS; i++) {
		if (!sections[i].is_present) {
			continue;
		}
		if (!sections[i].split_components(&sections[i], &src, &sig, &cert)) {
			printf("Error while splitting code/signature/Certificate from %s\n",
					sections[i].file);
			return 0;
		}
		if (!is_component_authenticated(src, sig, cert)) {
			printf("Error while authenticating %s\n", sections[i].file);
			return 0;
		}
	}

	return 1;
}

int do_board_upgrade_check(char *img)
{
	if (is_authentication_check_enabled()) {
		if (!get_sections()) {
			printf("Error: %s is not a signed image\n", img);
			return 1;
		}

		if (!is_image_authenticated()) {
			printf("Error: \"%s\" couldn't be authenticated. Abort...\n", img);
			return 1;
		}

		if (!check_image_version()) {
			printf("Error: \"%s\" couldn't be upgraded. Abort...\n", img);
			return 1;
		}
	}

	return 0;
}
