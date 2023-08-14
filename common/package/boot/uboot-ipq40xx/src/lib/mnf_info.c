#include <common.h>

#ifdef CONFIG_MNFINFO_SUPPORT

#include <mnf_info.h>
#include <linux/ctype.h>

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

mnf_field_t *mnf_get_field_info_short(char name)
{
	for (mnf_field_t *field = mnf_fields; field->short_name != '\0'; field++) {
		if (name == field->short_name)
			return field;
	}
	return NULL;
}

mnf_field_t *mnf_get_field_info_long(const char *name)
{
	for (mnf_field_t *field = mnf_fields; field->short_name != '\0'; field++) {
		if (!strcmp(name, field->long_name))
			return field;
	}
	return NULL;
}

int mnf_get_field(const char *name, void *result)
{
	mnf_field_t *field = mnf_get_field_info_long(name);

	if (!field)
		return -1;

	return mnf_flash_read(field, result);
}

int mnf_set_field(const char *name, const void *value)
{
	mnf_field_t *field = mnf_get_field_info_long(name);
	int err;

	if (!field)
		return -1;

	err = mnf_flash_write_init();
	if (err)
		return err;

	err = mnf_flash_write(field, value);
	if (err)
		return err; // If write fails, we should cancel the transaction and not call finalize()

	return mnf_flash_write_finalize();
}

static void apply_mnf_order(mnf_field_t *field, char *buf)
{
	if (field->flags & MNF_FIELD_REVERSED) {
		for (int i = 0; i < (field->length) / 2; i++) {
			char tmp		   = buf[i];
			buf[i]			   = buf[field->length - 1 - i];
			buf[field->length - 1 - i] = tmp;
		}
	}
}

static char int_to_hexchar(unsigned val)
{
	const char *hex_chars = "0123456789ABCDEF";

	if (val >= 16)
		return '\0';

	return hex_chars[val];
}

static void bin_to_hexstr(const char *input, char *result, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		int high_nibble = (input[i] >> 4) & 0xf;
		int low_nibble	= input[i] & 0xf;

		result[i * 2]	    = int_to_hexchar(high_nibble);
		result[(i * 2) + 1] = int_to_hexchar(low_nibble);
	}

	result[i * 2] = '\0';
}

void mnf_field_to_str(mnf_field_t *field, char *input, char *result)
{
	apply_mnf_order(field, input);

	if (field->flags & MNF_FIELD_BINARY) { // format the result as hex
		bin_to_hexstr(input, result, field->length);
	} else { // truncate the string to the first non-ascii char or field len, whichever comes first
		int i;
		for (i = 0; i < field->length; i++) {
			if (input[i] & 0x80)
				break;
		}
		memcpy(result, input, i);
		result[i] = 0x00;
	}
}

static int hexchar_to_int(char c)
{
	const char *hex_chars = "0123456789ABCDEF";
	char *found	      = strchr(hex_chars, toupper(c));

	if (!found)
		return -1;

	return found - hex_chars;
}

static int hexstr_to_bin(const char *input, char *result, int len)
{
	for (int i = 0; i < len; i++) {
		char high_char = input[i * 2];
		char low_char  = input[(i * 2) + 1];

		if (high_char == 0x00)
			break;

		int high_nibble = hexchar_to_int(high_char);
		int low_nibble	= hexchar_to_int(low_char);

		if (low_nibble < 0 || high_nibble < 0)
			return 1;

		result[i] = ((unsigned char)high_nibble << 4) | (unsigned char)low_nibble;
	}

	return 0;
}

int mnf_str_to_field(mnf_field_t *field, const char *input, char *result)
{
	memset(result, 0xff, field->length);

	if (field->flags & MNF_FIELD_BINARY) { // parse the input as hex encoded string
		if (hexstr_to_bin(input, result, field->length))
			return 1;
	} else { // copy the string directly, ommiting trailing null byte.
		memcpy(result, input, min(strlen(input), field->length));
	}

	apply_mnf_order(field, result);
	return 0;
}

#endif
