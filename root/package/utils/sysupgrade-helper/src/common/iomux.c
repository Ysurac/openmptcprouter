/*
 * (C) Copyright 2008
 * Gary Jennejohn, DENX Software Engineering GmbH, garyj@denx.de.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <serial.h>
#include <malloc.h>

#ifdef CONFIG_CONSOLE_MUX
void iomux_printdevs(const int console)
{
	int i;
	struct stdio_dev *dev;

	for (i = 0; i < cd_count[console]; i++) {
		dev = console_devices[console][i];
		printf("%s ", dev->name);
	}
	printf("\n");
}

/* This tries to preserve the old list if an error occurs. */
int iomux_doenv(const int console, const char *arg)
{
	char *console_args, *temp, **start;
	int i, j, k, io_flag, cs_idx, repeat;
	struct stdio_dev *dev;
	struct stdio_dev **cons_set;

	console_args = strdup(arg);
	if (console_args == NULL)
		return 1;
	/*
	 * Check whether a comma separated list of devices was
	 * entered and count how many devices were entered.
	 * The array start[] has pointers to the beginning of
	 * each device name (up to MAX_CONSARGS devices).
	 *
	 * Have to do this twice - once to count the number of
	 * commas and then again to populate start.
	 */
	i = 0;
	temp = console_args;
	for (;;) {
		temp = strchr(temp, ',');
		if (temp != NULL) {
			i++;
			temp++;
			continue;
		}
		/* There's always one entry more than the number of commas. */
		i++;
		break;
	}
	start = (char **)malloc(i * sizeof(char *));
	if (start == NULL) {
		free(console_args);
		return 1;
	}
	i = 0;
	start[0] = console_args;
	for (;;) {
		temp = strchr(start[i++], ',');
		if (temp == NULL)
			break;
		*temp = '\0';
		start[i] = temp + 1;
	}
	cons_set = (struct stdio_dev **)calloc(i, sizeof(struct stdio_dev *));
	if (cons_set == NULL) {
		free(start);
		free(console_args);
		return 1;
	}

	switch (console) {
	case stdin:
		io_flag = DEV_FLAGS_INPUT;
		break;
	case stdout:
	case stderr:
		io_flag = DEV_FLAGS_OUTPUT;
		break;
	default:
		free(start);
		free(console_args);
		free(cons_set);
		return 1;
	}

	cs_idx = 0;
	for (j = 0; j < i; j++) {
		/*
		 * Check whether the device exists and is valid.
		 * console_assign() also calls search_device(),
		 * but I need the pointer to the device.
		 */
		dev = search_device(io_flag, start[j]);
		if (dev == NULL)
			continue;
		/*
		 * Prevent multiple entries for a device.
		 */
		 repeat = 0;
		 for (k = 0; k < cs_idx; k++) {
			if (dev == cons_set[k]) {
				repeat++;
				break;
			}
		 }
		 if (repeat)
			continue;
		/*
		 * Try assigning the specified device.
		 * This could screw up the console settings for apps.
		 */
		if (console_assign(console, start[j]) < 0)
			continue;
#ifdef CONFIG_SERIAL_MULTI
		/*
		 * This was taken from common/cmd_nvedit.c.
		 * This will never work because serial_assign() returns
		 * 1 upon error, not -1.
		 * This would almost always return an error anyway because
		 * serial_assign() expects the name of a serial device, like
		 * serial_smc, but the user generally only wants to set serial.
		 */
		if (serial_assign(start[j]) < 0)
			continue;
#endif
		cons_set[cs_idx++] = dev;
	}
	free(console_args);
	free(start);
	/* failed to set any console */
	if (cs_idx == 0) {
		free(cons_set);
		return 1;
	} else {
		/* Works even if console_devices[console] is NULL. */
		console_devices[console] =
			(struct stdio_dev **)realloc(console_devices[console],
			cs_idx * sizeof(struct stdio_dev *));
		if (console_devices[console] == NULL) {
			free(cons_set);
			return 1;
		}
		memcpy(console_devices[console], cons_set, cs_idx *
			sizeof(struct stdio_dev *));

		cd_count[console] = cs_idx;
	}
	free(cons_set);
	return 0;
}
#endif /* CONFIG_CONSOLE_MUX */
