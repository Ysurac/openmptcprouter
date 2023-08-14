#include <common.h>

#ifdef CONFIG_MNFINFO_SUPPORT


#include <mnf_info.h>
#include <linux/ctype.h>
#include <linux/time.h>

#define CONFIG_MAC_VID_STR "001E42"

static int validate_num(mnf_field_t *field, const char *old)
{
	for (int i = 0; i < field->length; i++) {
		if (!isdigit(old[i]))
			return 1;
	}
	return 0;
}

/* Set to mac random mac address with our VID, skip if mac already has our VID */
static int restore_mac(mnf_field_t *field, const char *old, char *buf)
{
	unsigned nid;
	srand(get_timer(0));
	nid = rand() & 0xffffff;

	sprintf(buf, "%s%06x", CONFIG_MAC_VID_STR, nid);

	for (int i = 0; i < strlen(buf); i++) {
		buf[i] = toupper(buf[i]);
	}

	return strncmp(old, CONFIG_MAC_VID_STR, strlen(CONFIG_MAC_VID_STR)) != 0;
}

/* Set to fixed fallback device, skip if field contains valid model number */
static int restore_name(mnf_field_t *field, const char* old, char *buf) {
	const char *good_models[] = { "RUTX08", "RUTX09", "RUTX10", "RUTX11", "RUTX12", "RUTX14", "RUTXR1", "RUTX50" };
	
	strcpy(buf, "RUTX08000000");

	for (int i = 0; i < ARRAY_SIZE(good_models); i++) {
		if (!strncmp(old, good_models[i], 6))
			return 0;
	}
	return 1;
}

/* Clear the field with 0's, skip if field contains only digits */
static int restore_num(mnf_field_t *field, const char *old, char *buf)
{
	memset(buf, '0', field->length);
	buf[field->length] = '\0';

	return validate_num(field, old);
}

/* Clear the field with 0xff's, don't overwrite by default */
static int clear(mnf_field_t *field, const char *old, char *buf)
{
	strcpy(buf, "");

	return 0;
}

mnf_field_t mnf_fields[] = {
//	short/long name,     description,        offset, len, restore func   , flags
	{ 'm', "mac",       "MAC address",         0x00,   6, restore_mac    , MNF_FIELD_BINARY },
	{ 'n', "name",      "Model name",          0x10,  12, restore_name   },
	{ 'w', "wps",       "WPS PIN",             0x20,   8, clear          },
	{ 's', "sn",        "Serial number",       0x30,  10, restore_num    },
	{ 'b', "batch",     "Batch number",        0x40,   4, restore_num    },
	{ 'H', "hwver",     "Hardware version",    0x50,   4, restore_num    },
	{ 'W', "wifi_pass", "WiFi password",       0x90,  16, clear          },
	{ 'x', "passwd",    "Linux password",      0xA0, 106, clear          },
	{ '1', "sim1",      "SIM 1 PIN",        0x10000,   4, clear          },
	{ '2', "sim2",      "SIM 2 PIN",        0x10010,   4, clear          },
	{ '3', "sim3",      "SIM 3 PIN",        0x10020,   4, clear          },
	{ '4', "sim4",      "SIM 4 PIN",        0x10030,   4, clear          },
	{ '\0' }
};

#endif
