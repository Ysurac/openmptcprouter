/*
 * Manage Keyboard Matrices
 *
 * Copyright (c) 2012 The Chromium OS Authors.
 * (C) Copyright 2004 DENX Software Engineering, Wolfgang Denk, wd@denx.de
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <fdtdec.h>
#include <key_matrix.h>
#include <malloc.h>
#include <linux/input.h>

/**
 * Determine if the current keypress configuration can cause key ghosting
 *
 * We figure this out by seeing if we have two or more keys in the same
 * column, as well as two or more keys in the same row.
 *
 * @param config	Keyboard matrix config
 * @param keys		List of keys to check
 * @param valid		Number of valid keypresses to check
 * @return 0 if no ghosting is possible, 1 if it is
 */
static int has_ghosting(struct key_matrix *config, struct key_matrix_key *keys,
			int valid)
{
	int key_in_same_col = 0, key_in_same_row = 0;
	int i, j;

	for (i = 0; i < valid; i++) {
		/*
		 * Find 2 keys such that one key is in the same row
		 * and the other is in the same column as the i-th key.
		 */
		for (j = i + 1; j < valid; j++) {
			if (keys[j].col == keys[i].col)
				key_in_same_col = 1;
			if (keys[j].row == keys[i].row)
				key_in_same_row = 1;
		}
	}

	if (key_in_same_col && key_in_same_row)
		return 1;
	else
		return 0;
}

int key_matrix_decode(struct key_matrix *config, struct key_matrix_key keys[],
		      int num_keys, int keycode[], int max_keycodes)
{
	const u8 *keymap;
	int valid, upto;
	int pos;

	debug("%s: num_keys = %d\n", __func__, num_keys);
	keymap = config->plain_keycode;
	for (valid = upto = 0; upto < num_keys; upto++) {
		struct key_matrix_key *key = &keys[upto];

		debug("  valid=%d, row=%d, col=%d\n", key->valid, key->row,
		      key->col);
		if (!key->valid)
			continue;
		pos = key->row * config->num_cols + key->col;
		if (config->fn_keycode && pos == config->fn_pos)
			keymap = config->fn_keycode;

		/* Convert the (row, col) values into a keycode */
		if (valid < max_keycodes)
			keycode[valid++] = keymap[pos];
		debug("    keycode=%d\n", keymap[pos]);
	}

	/* For a ghost key config, ignore the keypresses for this iteration. */
	if (valid >= 3 && has_ghosting(config, keys, valid)) {
		valid = 0;
		debug("    ghosting detected!\n");
	}
	debug("  %d valid keycodes found\n", valid);

	return valid;
}

/**
 * Create a new keycode map from some provided data
 *
 * This decodes a keycode map in the format used by the fdt, which is one
 * word per entry, with the row, col and keycode encoded in that word.
 *
 * We create a (row x col) size byte array with each entry containing the
 * keycode for that (row, col). We also search for map_keycode and return
 * its position if found (this is used for finding the Fn key).
 *
 * @param config        Key matrix dimensions structure
 * @param data          Keycode data
 * @param len           Number of entries in keycode table
 * @param map_keycode   Key code to find in the map
 * @param pos           Returns position of map_keycode, if found, else -1
 * @return map  Pointer to allocated map
 */
static uchar *create_keymap(struct key_matrix *config, u32 *data, int len,
			    int map_keycode, int *pos)
{
	uchar *map;

	if (pos)
		*pos = -1;
	map = (uchar *)calloc(1, config->key_count);
	if (!map) {
		debug("%s: failed to malloc %d bytes\n", __func__,
			config->key_count);
		return NULL;
	}

	for (; len >= sizeof(u32); data++, len -= 4) {
		u32 tmp = fdt32_to_cpu(*data);
		int key_code, row, col;
		int entry;

		row = (tmp >> 24) & 0xff;
		col = (tmp >> 16) & 0xff;
		key_code = tmp & 0xffff;
		entry = row * config->num_cols + col;
		map[entry] = key_code;
		if (pos && map_keycode == key_code)
			*pos = entry;
	}

	return map;
}

int key_matrix_decode_fdt(struct key_matrix *config, const void *blob,
			  int node)
{
	const struct fdt_property *prop;
	int offset;

	/* Check each property name for ones that we understand */
	for (offset = fdt_first_property_offset(blob, node);
		      offset > 0;
		      offset = fdt_next_property_offset(blob, offset)) {
		const char *name;
		int len;

		prop = fdt_get_property_by_offset(blob, offset, NULL);
		name = fdt_string(blob, fdt32_to_cpu(prop->nameoff));
		len = strlen(name);

		/* Name needs to match "1,<type>keymap" */
		debug("%s: property '%s'\n", __func__, name);
		if (strncmp(name, "1,", 2) || len < 8 ||
		    strcmp(name + len - 6, "keymap"))
			continue;

		len -= 8;
		if (len == 0) {
			config->plain_keycode = create_keymap(config,
				(u32 *)prop->data, fdt32_to_cpu(prop->len),
				KEY_FN, &config->fn_pos);
		} else if (0 == strncmp(name + 2, "fn-", len)) {
			config->fn_keycode = create_keymap(config,
				(u32 *)prop->data, fdt32_to_cpu(prop->len),
				-1, NULL);
		} else {
			debug("%s: unrecognised property '%s'\n", __func__,
			      name);
		}
	}
	debug("%s: Decoded key maps %p, %p from fdt\n", __func__,
	      config->plain_keycode, config->fn_keycode);

	if (!config->plain_keycode) {
		debug("%s: cannot find keycode-plain map\n", __func__);
		return -1;
	}

	return 0;
}

int key_matrix_init(struct key_matrix *config, int rows, int cols)
{
	memset(config, '\0', sizeof(*config));
	config->num_rows = rows;
	config->num_cols = cols;
	config->key_count = rows * cols;
	assert(config->key_count > 0);

	return 0;
}
