/*
 * (C) Copyright 2007
 * Stefano Babic, DENX Software Engineering, sbabic@denx.de.
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
#include <command.h>
#include <dm9000.h>

static int do_read_dm9000_eeprom ( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]) {
	unsigned int i;
	u8 data[2];

	for (i=0; i < 0x40; i++) {
		if (!(i % 0x10))
			printf("\n%08x:", i);
		dm9000_read_srom_word(i, data);
		printf(" %02x%02x", data[1], data[0]);
	}
	printf ("\n");
	return (0);
}

static int do_write_dm9000_eeprom ( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]) {
	int offset,value;

	if (argc < 4)
		return cmd_usage(cmdtp);

	offset=simple_strtoul(argv[2],NULL,16);
	value=simple_strtoul(argv[3],NULL,16);
	if (offset > 0x40) {
		printf("Wrong offset : 0x%x\n",offset);
		return cmd_usage(cmdtp);
	}
	dm9000_write_srom_word(offset, value);
	return (0);
}

int do_dm9000_eeprom ( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]) {
	if (argc < 2)
		return cmd_usage(cmdtp);

	if (strcmp (argv[1],"read") == 0)
		return (do_read_dm9000_eeprom(cmdtp,flag,argc,argv));
	else if (strcmp (argv[1],"write") == 0)
		return (do_write_dm9000_eeprom(cmdtp,flag,argc,argv));
	else
		return cmd_usage(cmdtp);
}

U_BOOT_CMD(
	dm9000ee,4,1,do_dm9000_eeprom,
	"Read/Write eeprom connected to Ethernet Controller",
	"\ndm9000ee write <word offset> <value> \n"
	"\tdm9000ee read \n"
	"\tword:\t\t00-02 : MAC Address\n"
	"\t\t\t03-07 : DM9000 Configuration\n"
	"\t\t\t08-63 : User data"
);
