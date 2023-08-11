#ifndef FWTOOL_H
#define FWTOOL_H

enum fwtool_ret_id {
	FWTOOL_FIRMWARE_VALID,
	FWTOOL_ERROR_INVALID_FILE
};

int fwtool_validate_manifest(void *ptr, unsigned int length);

#endif // FWTOOL_H