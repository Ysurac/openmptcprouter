/*
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
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

typedef enum {HLOS_TYPE, UBOOT_TYPE, SBL_TYPE, TZ_TYPE, RPM_TYPE}type;

//Structure to store the details of the sections of an image.
struct image_section {
	type section_type;
	int max_version;
	char file[256];
	char *type;
	char tmp_file[256];
	int img_version;
	int local_version;
	char *version_file;
	int is_present;
	int (*pre_op)(struct image_section *);
	int (*get_sw_id)(struct image_section *);
	int (*split_components)(struct image_section *, char **, char**, char**);
};

typedef struct mbn_header {
	uint32_t image_id;
	uint32_t ver_num;
	uint32_t image_src;
	uint8_t *image_dest_ptr;
	uint32_t image_size;
	uint32_t code_size;
	uint8_t *sig_ptr;
	uint32_t sig_sz;
	uint8_t *cert_ptr;
	uint32_t cert_sz;
}Mbn_Hdr;

typedef struct {
	uint32_t  codeword;
	uint32_t  magic;
	uint32_t  RESERVED_0;
	uint32_t  RESERVED_1;
	uint32_t  RESERVED_2;
	uint32_t  image_src;
	uint8_t  *image_dest_ptr;
	uint32_t  image_size;
	uint32_t  code_size;
	uint8_t  *sig_ptr;
	uint32_t  sig_size;
	uint8_t  *cert_ptr;
	uint32_t  cert_size;
	uint32_t  root_cert_sel;
	uint32_t  num_root_certs;
	uint32_t  RESERVED_5;
	uint32_t  RESERVED_6;
	uint32_t  RESERVED_7;
	uint32_t  RESERVED_8;
	uint32_t  RESERVED_9;
} Sbl_Hdr;

//functions in sysupgrade.c
int get_sections(void);
int is_authentication_check_enabled(void);
int get_local_image_version(struct image_section *);
int set_local_image_version(struct image_section *);
int is_version_check_enabled(void);
int get_sw_id_from_component_bin(struct image_section *);
int get_sw_id_from_component_bin_elf(struct image_section *);
int extract_kernel_binary(struct image_section *);
int is_image_version_higher(void);
int update_version(void);
int check_image_version(void);
int split_code_signature_cert_from_component_bin(struct image_section *, char **, char **, char **);
int split_code_signature_cert_from_component_bin_elf(struct image_section *, char **, char **, char **);
void generate_swid_ipad(char *, unsigned long long *);
void generate_hwid_opad(char *, char *, char *, unsigned long long *);
int generate_hash(char *, char *, char *);
int is_component_authenticated(char *, char *, char *);
int is_image_authenticated(void);
int do_board_upgrade_check(char *);
