#ifndef __MNF_INFO__
#define __MNF_INFO__

#include <common.h>

/* 
 * bitset, defines special features for mnf fields defined by mnf_field_s struct 
 */
typedef enum {
	MNF_FIELD_BINARY = 1, // mnf field is stored as binary, should be converted to/from hex as human readable format
	MNF_FIELD_REVERSED = 2 // mnf field is stored in reverse order
} mnf_field_flags;

struct mnf_field_s;
typedef struct mnf_field_s mnf_field_t;

/*
 * Callback function for use in mnfinfo restore/repair commands
 * generates a new sane value into 'buf' for a given field 
 * returns 1 if value stored in `old` did not appear valid. 
 */
typedef int (*mnf_field_restore_func_t)(mnf_field_t *field, const char *old, char *buf);

/*
 * Metadata for mnf values stored in flash
 */
struct mnf_field_s {
	char short_name;
	const char *long_name;
	const char *description;
	unsigned offset;
	unsigned length;
	mnf_field_restore_func_t restore_func;
	mnf_field_flags flags;
};

extern mnf_field_t mnf_fields[];

/* Helper functions for accessing mnf fields by name*/

mnf_field_t *mnf_get_field_info_short(char name);
mnf_field_t *mnf_get_field_info_long(const char *name);

/* Driver functions for accessing mnf values stored in flash */

extern int mnf_flash_read(const mnf_field_t *field, char *result);

extern int mnf_flash_write_init(void);
extern int mnf_flash_write(const mnf_field_t *field, const char *buf);
extern int mnf_flash_write_finalize(void);

/* Useful helper functions for other uses */

int mnf_get_field(const char *name, void *result);
int mnf_set_field(const char *name, const void *value);

/* Functions for formating mnf values to/from user readable strings */

void mnf_field_to_str(mnf_field_t *field, char *input, char *result);
int mnf_str_to_field(mnf_field_t *field, const char *input, char *result);

/* mnf command entry func */

int mnf_cmd_entry(int argc, char *argv[]);

#endif
