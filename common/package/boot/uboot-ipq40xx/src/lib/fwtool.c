#include <common.h>
#include <exports.h>
#include <fwtool.h>

#define FWIMAGE_MAGIC		0x46577830 /* FWx0 */

struct fwimage_trailer {
	uint32_t magic;
	uint32_t crc32;
	uint8_t type;
	uint8_t __pad[3];
	uint32_t size;
};

int fwtool_validate_manifest(void *ptr, unsigned int length)
{
	struct fwimage_trailer tr = { 0 };
	char buffer[512] = { 0 };
	int offset, len;

	if (sizeof(tr) * 2 > length) {
		return FWTOOL_ERROR_INVALID_FILE;
	}

	// read signature trailer first
	offset = length - sizeof(tr);
	memcpy(&tr, ptr + offset, sizeof(tr));

	if (cpu_to_be32(tr.magic) != FWIMAGE_MAGIC ||
	    cpu_to_be32(tr.size) >= length) {
		return FWTOOL_ERROR_INVALID_FILE;
	}

	// read manifest trailer
	offset = length - sizeof(tr) - cpu_to_be32(tr.size);
	memcpy(&tr, ptr + offset, sizeof(tr));

	if (cpu_to_be32(tr.magic) != FWIMAGE_MAGIC ||
	    cpu_to_be32(tr.size) >= length) {
		return FWTOOL_ERROR_INVALID_FILE;
	}

	// read manifest trailer
	offset -= cpu_to_be32(tr.size) - sizeof(tr) - 8;
	len = cpu_to_be32(tr.size) - sizeof(tr);
	len = (len > sizeof(buffer)) ? sizeof(buffer) : len;
	memcpy(&buffer[0], ptr + offset, len);

	buffer[len - 1] = '\0';

	if (!strstr(buffer, "supported_devices")) {
		return FWTOOL_ERROR_INVALID_FILE;
	}

	if (!strstr(buffer, "teltonika,")) {
		return FWTOOL_ERROR_INVALID_FILE;
	}

	if (!strstr(buffer, DEVICE_MODEL_MANIFEST)) {
		return FWTOOL_ERROR_INVALID_FILE;
	}

	return FWTOOL_FIRMWARE_VALID;
}