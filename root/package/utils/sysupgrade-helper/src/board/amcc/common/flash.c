/*
 * (C) Copyright 2004-2005
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002 Jun Gu <jung@artesyncp.com>
 * Add support for Am29F016D and dynamic switch setting.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Modified 4/5/2001
 * Wait for completion of each sector erase command issued
 * 4/5/2001
 * Chris Hallinan - DS4.COM, Inc. - clh@net1plus.com
 */

#include <common.h>
#include <asm/ppc4xx.h>
#include <asm/processor.h>

flash_info_t flash_info[CONFIG_SYS_MAX_FLASH_BANKS];	/* info for FLASH chips */

/*-----------------------------------------------------------------------
 * Functions
 */
static int write_word(flash_info_t * info, ulong dest, ulong data);
#ifdef CONFIG_SYS_FLASH_2ND_16BIT_DEV
static int write_word_1(flash_info_t * info, ulong dest, ulong data);
static int write_word_2(flash_info_t * info, ulong dest, ulong data);
static int flash_erase_1(flash_info_t * info, int s_first, int s_last);
static int flash_erase_2(flash_info_t * info, int s_first, int s_last);
static ulong flash_get_size_1(vu_long * addr, flash_info_t * info);
static ulong flash_get_size_2(vu_long * addr, flash_info_t * info);
#endif

void flash_print_info(flash_info_t * info)
{
	int i;
	int k;
	int size;
	int erased;
	volatile unsigned long *flash;

	if (info->flash_id == FLASH_UNKNOWN) {
		printf("missing or unknown FLASH type\n");
		return;
	}

	switch (info->flash_id & FLASH_VENDMASK) {
	case FLASH_MAN_AMD:
		printf("AMD ");
		break;
	case FLASH_MAN_STM:
		printf("STM ");
		break;
	case FLASH_MAN_FUJ:
		printf("FUJITSU ");
		break;
	case FLASH_MAN_SST:
		printf("SST ");
		break;
	case FLASH_MAN_MX:
		printf ("MACRONIX ");
		break;
	default:
		printf("Unknown Vendor ");
		break;
	}

	switch (info->flash_id & FLASH_TYPEMASK) {
	case FLASH_AM040:
		printf("AM29F040 (512 Kbit, uniform sector size)\n");
		break;
	case FLASH_AM400B:
		printf("AM29LV400B (4 Mbit, bottom boot sect)\n");
		break;
	case FLASH_AM400T:
		printf("AM29LV400T (4 Mbit, top boot sector)\n");
		break;
	case FLASH_AM800B:
		printf("AM29LV800B (8 Mbit, bottom boot sect)\n");
		break;
	case FLASH_AM800T:
		printf("AM29LV800T (8 Mbit, top boot sector)\n");
		break;
	case FLASH_AMD016:
		printf("AM29F016D (16 Mbit, uniform sector size)\n");
		break;
	case FLASH_AM160B:
		printf("AM29LV160B (16 Mbit, bottom boot sect)\n");
		break;
	case FLASH_AM160T:
		printf("AM29LV160T (16 Mbit, top boot sector)\n");
		break;
	case FLASH_AM320B:
		printf("AM29LV320B (32 Mbit, bottom boot sect)\n");
		break;
	case FLASH_AM320T:
		printf("AM29LV320T (32 Mbit, top boot sector)\n");
		break;
	case FLASH_AM033C:
		printf("AM29LV033C (32 Mbit, top boot sector)\n");
		break;
	case FLASH_SST800A:
		printf("SST39LF/VF800 (8 Mbit, uniform sector size)\n");
		break;
	case FLASH_SST160A:
		printf("SST39LF/VF160 (16 Mbit, uniform sector size)\n");
		break;
	case FLASH_STMW320DT:
		printf ("M29W320DT (32 M, top sector)\n");
		break;
	case FLASH_MXLV320T:
		printf ("MXLV320T (32 Mbit, top sector)\n");
		break;
	default:
		printf("Unknown Chip Type\n");
		break;
	}

	printf("  Size: %ld KB in %d Sectors\n",
	       info->size >> 10, info->sector_count);

	printf("  Sector Start Addresses:");
	for (i = 0; i < info->sector_count; ++i) {
		/*
		 * Check if whole sector is erased
		 */
		if (i != (info->sector_count - 1))
			size = info->start[i + 1] - info->start[i];
		else
			size = info->start[0] + info->size - info->start[i];
		erased = 1;
		flash = (volatile unsigned long *)info->start[i];
		size = size >> 2;	/* divide by 4 for longword access */
		for (k = 0; k < size; k++) {
			if (*flash++ != 0xffffffff) {
				erased = 0;
				break;
			}
		}

		if ((i % 5) == 0)
			printf("\n   ");
		printf(" %08lX%s%s",
		       info->start[i],
		       erased ? " E" : "  ", info->protect[i] ? "RO " : "   ");
	}
	printf("\n");
	return;
}


/*
 * The following code cannot be run from FLASH!
 */
#ifdef CONFIG_SYS_FLASH_2ND_16BIT_DEV
static ulong flash_get_size(vu_long * addr, flash_info_t * info)
{
	/* bit 0 used for big flash marking */
	if ((ulong)addr & 0x1) {
		return flash_get_size_2((vu_long *)((ulong)addr & 0xfffffffe), info);
	} else {
		return flash_get_size_1(addr, info);
	}
}

static ulong flash_get_size_1(vu_long * addr, flash_info_t * info)
#else
static ulong flash_get_size(vu_long * addr, flash_info_t * info)
#endif
{
	short i;
	CONFIG_SYS_FLASH_WORD_SIZE value;
	ulong base = (ulong) addr;
	volatile CONFIG_SYS_FLASH_WORD_SIZE *addr2 = (CONFIG_SYS_FLASH_WORD_SIZE *) addr;

	DEBUGF("FLASH ADDR: %08x\n", (unsigned)addr);

	/* Write auto select command: read Manufacturer ID */
	addr2[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00AA00AA;
	addr2[CONFIG_SYS_FLASH_ADDR1] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00550055;
	addr2[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00900090;
	udelay(1000);

	value = addr2[0];
	DEBUGF("FLASH MANUFACT: %x\n", value);

	switch (value) {
	case (CONFIG_SYS_FLASH_WORD_SIZE) AMD_MANUFACT:
		info->flash_id = FLASH_MAN_AMD;
		break;
	case (CONFIG_SYS_FLASH_WORD_SIZE) FUJ_MANUFACT:
		info->flash_id = FLASH_MAN_FUJ;
		break;
	case (CONFIG_SYS_FLASH_WORD_SIZE) SST_MANUFACT:
		info->flash_id = FLASH_MAN_SST;
		break;
	case (CONFIG_SYS_FLASH_WORD_SIZE) STM_MANUFACT:
		info->flash_id = FLASH_MAN_STM;
		break;
	default:
		info->flash_id = FLASH_UNKNOWN;
		info->sector_count = 0;
		info->size = 0;
		return (0);	/* no or unknown flash  */
	}

	value = addr2[1];	/* device ID */
	DEBUGF("\nFLASH DEVICEID: %x\n", value);

	switch (value) {
	case (CONFIG_SYS_FLASH_WORD_SIZE) AMD_ID_LV040B:
		info->flash_id += FLASH_AM040;
		info->sector_count = 8;
		info->size = 0x0080000;		/* => 512 KiB */
		break;

	case (CONFIG_SYS_FLASH_WORD_SIZE) AMD_ID_F040B:
		info->flash_id += FLASH_AM040;
		info->sector_count = 8;
		info->size = 0x0080000;		/* => 512 KiB */
		break;

	case (CONFIG_SYS_FLASH_WORD_SIZE) STM_ID_M29W040B:
		info->flash_id += FLASH_AM040;
		info->sector_count = 8;
		info->size = 0x0080000;		/* => 512 KiB */
		break;

	case (CONFIG_SYS_FLASH_WORD_SIZE) AMD_ID_F016D:
		info->flash_id += FLASH_AMD016;
		info->sector_count = 32;
		info->size = 0x00200000;	/* => 2 MiB */
		break;

	case (CONFIG_SYS_FLASH_WORD_SIZE) AMD_ID_LV033C:
		info->flash_id += FLASH_AMDLV033C;
		info->sector_count = 64;
		info->size = 0x00400000;	/* => 4 MiB */
		break;

	case (CONFIG_SYS_FLASH_WORD_SIZE) AMD_ID_LV400T:
		info->flash_id += FLASH_AM400T;
		info->sector_count = 11;
		info->size = 0x00080000;	/* => 512 KiB */
		break;

	case (CONFIG_SYS_FLASH_WORD_SIZE) AMD_ID_LV400B:
		info->flash_id += FLASH_AM400B;
		info->sector_count = 11;
		info->size = 0x00080000;	/* => 512 KiB */
		break;

	case (CONFIG_SYS_FLASH_WORD_SIZE) AMD_ID_LV800T:
		info->flash_id += FLASH_AM800T;
		info->sector_count = 19;
		info->size = 0x00100000;	/* => 1 MiB */
		break;

	case (CONFIG_SYS_FLASH_WORD_SIZE) AMD_ID_LV800B:
		info->flash_id += FLASH_AM800B;
		info->sector_count = 19;
		info->size = 0x00100000;	/* => 1 MiB */
		break;

	case (CONFIG_SYS_FLASH_WORD_SIZE) AMD_ID_LV160T:
		info->flash_id += FLASH_AM160T;
		info->sector_count = 35;
		info->size = 0x00200000;	/* => 2 MiB */
		break;

	case (CONFIG_SYS_FLASH_WORD_SIZE) AMD_ID_LV160B:
		info->flash_id += FLASH_AM160B;
		info->sector_count = 35;
		info->size = 0x00200000;	/* => 2 MiB */
		break;

	default:
		info->flash_id = FLASH_UNKNOWN;
		return (0);	/* => no or unknown flash */
	}

	/* set up sector start address table */
	if (((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_SST) ||
	    ((info->flash_id & FLASH_TYPEMASK) == FLASH_AM040) ||
	    ((info->flash_id & FLASH_TYPEMASK) == FLASH_AMD016)) {
		for (i = 0; i < info->sector_count; i++)
			info->start[i] = base + (i * 0x00010000);
	} else {
		if (info->flash_id & FLASH_BTYPE) {
			/* set sector offsets for bottom boot block type */
			info->start[0] = base + 0x00000000;
			info->start[1] = base + 0x00004000;
			info->start[2] = base + 0x00006000;
			info->start[3] = base + 0x00008000;
			for (i = 4; i < info->sector_count; i++) {
				info->start[i] =
				    base + (i * 0x00010000) - 0x00030000;
			}
		} else {
			/* set sector offsets for top boot block type */
			i = info->sector_count - 1;
			info->start[i--] = base + info->size - 0x00004000;
			info->start[i--] = base + info->size - 0x00006000;
			info->start[i--] = base + info->size - 0x00008000;
			for (; i >= 0; i--) {
				info->start[i] = base + i * 0x00010000;
			}
		}
	}

	/* check for protected sectors */
	for (i = 0; i < info->sector_count; i++) {
		/* read sector protection at sector address, (A7 .. A0) = 0x02 */
		/* D0 = 1 if protected */
		addr2 = (volatile CONFIG_SYS_FLASH_WORD_SIZE *)(info->start[i]);

		/* For AMD29033C flash we need to resend the command of *
		 * reading flash protection for upper 8 Mb of flash     */
		if (i == 32) {
			addr2[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0xAAAAAAAA;
			addr2[CONFIG_SYS_FLASH_ADDR1] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x55555555;
			addr2[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x90909090;
		}

		if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_SST)
			info->protect[i] = 0;
		else
			info->protect[i] = addr2[2] & 1;
	}

	/* issue bank reset to return to read mode */
	addr2[0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00F000F0;

	return (info->size);
}

static int wait_for_DQ7_1(flash_info_t * info, int sect)
{
	ulong start, now, last;
	volatile CONFIG_SYS_FLASH_WORD_SIZE *addr =
	    (CONFIG_SYS_FLASH_WORD_SIZE *) (info->start[sect]);

	start = get_timer(0);
	last = start;
	while ((addr[0] & (CONFIG_SYS_FLASH_WORD_SIZE) 0x00800080) !=
	       (CONFIG_SYS_FLASH_WORD_SIZE) 0x00800080) {
		if ((now = get_timer(start)) > CONFIG_SYS_FLASH_ERASE_TOUT) {
			printf("Timeout\n");
			return -1;
		}
		/* show that we're waiting */
		if ((now - last) > 1000) {	/* every second */
			putc('.');
			last = now;
		}
	}
	return 0;
}

#ifdef CONFIG_SYS_FLASH_2ND_16BIT_DEV
int flash_erase(flash_info_t * info, int s_first, int s_last)
{
	if (((info->flash_id & FLASH_TYPEMASK) == FLASH_AM320B) ||
	    ((info->flash_id & FLASH_TYPEMASK) == FLASH_AM320T) ||
	    ((info->flash_id & FLASH_TYPEMASK) == FLASH_MXLV320T) ||
	    ((info->flash_id & FLASH_TYPEMASK) == FLASH_STMW320DT)) {
		return flash_erase_2(info, s_first, s_last);
	} else {
		return flash_erase_1(info, s_first, s_last);
	}
}

static int flash_erase_1(flash_info_t * info, int s_first, int s_last)
#else
int flash_erase(flash_info_t * info, int s_first, int s_last)
#endif
{
	volatile CONFIG_SYS_FLASH_WORD_SIZE *addr = (CONFIG_SYS_FLASH_WORD_SIZE *) (info->start[0]);
	volatile CONFIG_SYS_FLASH_WORD_SIZE *addr2;
	int flag, prot, sect;
	int i;

	if ((s_first < 0) || (s_first > s_last)) {
		if (info->flash_id == FLASH_UNKNOWN) {
			printf("- missing\n");
		} else {
			printf("- no sectors to erase\n");
		}
		return 1;
	}

	if (info->flash_id == FLASH_UNKNOWN) {
		printf("Can't erase unknown flash type - aborted\n");
		return 1;
	}

	prot = 0;
	for (sect = s_first; sect <= s_last; ++sect) {
		if (info->protect[sect]) {
			prot++;
		}
	}

	if (prot) {
		printf("- Warning: %d protected sectors will not be erased!\n",
		       prot);
	} else {
		printf("\n");
	}

	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts();

	/* Start erase on unprotected sectors */
	for (sect = s_first; sect <= s_last; sect++) {
		if (info->protect[sect] == 0) {	/* not protected */
			addr2 = (CONFIG_SYS_FLASH_WORD_SIZE *) (info->start[sect]);

			if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_SST) {
				addr[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00AA00AA;
				addr[CONFIG_SYS_FLASH_ADDR1] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00550055;
				addr[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00800080;
				addr[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00AA00AA;
				addr[CONFIG_SYS_FLASH_ADDR1] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00550055;
				addr2[0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00500050;	/* block erase */
				for (i = 0; i < 50; i++)
					udelay(1000);	/* wait 1 ms */
			} else {
				addr[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00AA00AA;
				addr[CONFIG_SYS_FLASH_ADDR1] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00550055;
				addr[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00800080;
				addr[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00AA00AA;
				addr[CONFIG_SYS_FLASH_ADDR1] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00550055;
				addr2[0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00300030;	/* sector erase */
			}
			/*
			 * Wait for each sector to complete, it's more
			 * reliable.  According to AMD Spec, you must
			 * issue all erase commands within a specified
			 * timeout.  This has been seen to fail, especially
			 * if printf()s are included (for debug)!!
			 */
			wait_for_DQ7_1(info, sect);
		}
	}

	/* re-enable interrupts if necessary */
	if (flag)
		enable_interrupts();

	/* wait at least 80us - let's wait 1 ms */
	udelay(1000);

	/* reset to read mode */
	addr = (CONFIG_SYS_FLASH_WORD_SIZE *) info->start[0];
	addr[0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00F000F0;	/* reset bank */

	printf(" done\n");
	return 0;
}

/*-----------------------------------------------------------------------
 * Copy memory to flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
int write_buff(flash_info_t * info, uchar * src, ulong addr, ulong cnt)
{
	ulong cp, wp, data;
	int i, l, rc;

	wp = (addr & ~3);	/* get lower word aligned address */

	/*
	 * handle unaligned start bytes
	 */
	if ((l = addr - wp) != 0) {
		data = 0;
		for (i = 0, cp = wp; i < l; ++i, ++cp) {
			data = (data << 8) | (*(uchar *) cp);
		}
		for (; i < 4 && cnt > 0; ++i) {
			data = (data << 8) | *src++;
			--cnt;
			++cp;
		}
		for (; cnt == 0 && i < 4; ++i, ++cp) {
			data = (data << 8) | (*(uchar *) cp);
		}

		if ((rc = write_word(info, wp, data)) != 0) {
			return (rc);
		}
		wp += 4;
	}

	/*
	 * handle word aligned part
	 */
	while (cnt >= 4) {
		data = 0;
		for (i = 0; i < 4; ++i) {
			data = (data << 8) | *src++;
		}
		if ((rc = write_word(info, wp, data)) != 0) {
			return (rc);
		}
		wp += 4;
		cnt -= 4;
	}

	if (cnt == 0) {
		return (0);
	}

	/*
	 * handle unaligned tail bytes
	 */
	data = 0;
	for (i = 0, cp = wp; i < 4 && cnt > 0; ++i, ++cp) {
		data = (data << 8) | *src++;
		--cnt;
	}
	for (; i < 4; ++i, ++cp) {
		data = (data << 8) | (*(uchar *) cp);
	}

	return (write_word(info, wp, data));
}

/*-----------------------------------------------------------------------
 * Copy memory to flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
#ifdef CONFIG_SYS_FLASH_2ND_16BIT_DEV
static int write_word(flash_info_t * info, ulong dest, ulong data)
{
	if (((info->flash_id & FLASH_TYPEMASK) == FLASH_AM320B) ||
	    ((info->flash_id & FLASH_TYPEMASK) == FLASH_AM320T) ||
	    ((info->flash_id & FLASH_TYPEMASK) == FLASH_MXLV320T) ||
	    ((info->flash_id & FLASH_TYPEMASK) == FLASH_STMW320DT)) {
		return write_word_2(info, dest, data);
	} else {
		return write_word_1(info, dest, data);
	}
}

static int write_word_1(flash_info_t * info, ulong dest, ulong data)
#else
static int write_word(flash_info_t * info, ulong dest, ulong data)
#endif
{
	volatile CONFIG_SYS_FLASH_WORD_SIZE *addr2 = (CONFIG_SYS_FLASH_WORD_SIZE *) (info->start[0]);
	volatile CONFIG_SYS_FLASH_WORD_SIZE *dest2 = (CONFIG_SYS_FLASH_WORD_SIZE *) dest;
	volatile CONFIG_SYS_FLASH_WORD_SIZE *data2 = (CONFIG_SYS_FLASH_WORD_SIZE *) & data;
	ulong start;
	int i;

	/* Check if Flash is (sufficiently) erased */
	if ((*((vu_long *)dest) & data) != data) {
		return (2);
	}

	for (i = 0; i < 4 / sizeof(CONFIG_SYS_FLASH_WORD_SIZE); i++) {
		int flag;

		/* Disable interrupts which might cause a timeout here */
		flag = disable_interrupts();

		addr2[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00AA00AA;
		addr2[CONFIG_SYS_FLASH_ADDR1] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00550055;
		addr2[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00A000A0;

		dest2[i] = data2[i];

		/* re-enable interrupts if necessary */
		if (flag)
			enable_interrupts();

		/* data polling for D7 */
		start = get_timer(0);
		while ((dest2[i] & (CONFIG_SYS_FLASH_WORD_SIZE) 0x00800080) !=
		       (data2[i] & (CONFIG_SYS_FLASH_WORD_SIZE) 0x00800080)) {

			if (get_timer(start) > CONFIG_SYS_FLASH_WRITE_TOUT) {
				return (1);
			}
		}
	}

	return (0);
}

#ifdef CONFIG_SYS_FLASH_2ND_16BIT_DEV

#undef  CONFIG_SYS_FLASH_WORD_SIZE
#define CONFIG_SYS_FLASH_WORD_SIZE unsigned short

/*
 * The following code cannot be run from FLASH!
 */
static ulong flash_get_size_2(vu_long * addr, flash_info_t * info)
{
	short i;
	int n;
	CONFIG_SYS_FLASH_WORD_SIZE value;
	ulong base = (ulong) addr;
	volatile CONFIG_SYS_FLASH_WORD_SIZE *addr2 = (CONFIG_SYS_FLASH_WORD_SIZE *) addr;

	DEBUGF("FLASH ADDR: %08x\n", (unsigned)addr);

	/* Write auto select command: read Manufacturer ID */
	addr2[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00AA00AA;
	addr2[CONFIG_SYS_FLASH_ADDR1] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00550055;
	addr2[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00900090;
	udelay(1000);

	value = addr2[0];
	DEBUGF("FLASH MANUFACT: %x\n", value);

	switch (value) {
	case (CONFIG_SYS_FLASH_WORD_SIZE) AMD_MANUFACT:
		info->flash_id = FLASH_MAN_AMD;
		break;
	case (CONFIG_SYS_FLASH_WORD_SIZE) FUJ_MANUFACT:
		info->flash_id = FLASH_MAN_FUJ;
		break;
	case (CONFIG_SYS_FLASH_WORD_SIZE) SST_MANUFACT:
		info->flash_id = FLASH_MAN_SST;
		break;
	case (CONFIG_SYS_FLASH_WORD_SIZE) STM_MANUFACT:
		info->flash_id = FLASH_MAN_STM;
		break;
	case (CONFIG_SYS_FLASH_WORD_SIZE) MX_MANUFACT:
		info->flash_id = FLASH_MAN_MX;
		break;
	default:
		info->flash_id = FLASH_UNKNOWN;
		info->sector_count = 0;
		info->size = 0;
		return (0);	/* no or unknown flash  */
	}

	value = addr2[1];	/* device ID */

	DEBUGF("\nFLASH DEVICEID: %x\n", value);

	switch (value) {

	case (CONFIG_SYS_FLASH_WORD_SIZE)AMD_ID_LV320T:
		info->flash_id += FLASH_AM320T;
		info->sector_count = 71;
		info->size = 0x00400000;  break;	/* => 4 MiB	*/

	case (CONFIG_SYS_FLASH_WORD_SIZE)AMD_ID_LV320B:
		info->flash_id += FLASH_AM320B;
		info->sector_count = 71;
		info->size = 0x00400000;  break;	/* => 4 MiB	*/

	case (CONFIG_SYS_FLASH_WORD_SIZE)STM_ID_29W320DT:
		info->flash_id += FLASH_STMW320DT;
		info->sector_count = 67;
		info->size = 0x00400000;  break;	/* => 4 MiB	*/

	case (CONFIG_SYS_FLASH_WORD_SIZE)MX_ID_LV320T:
		info->flash_id += FLASH_MXLV320T;
		info->sector_count = 71;
		info->size = 0x00400000;
		break;	/* => 4 MB	*/

	default:
		info->flash_id = FLASH_UNKNOWN;
		return (0);	/* => no or unknown flash */
	}

	/* set up sector start address table */
	if (((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_SST) ||
	    ((info->flash_id & FLASH_TYPEMASK) == FLASH_AM040) ||
	    ((info->flash_id & FLASH_TYPEMASK) == FLASH_AMD016)) {
		for (i = 0; i < info->sector_count; i++)
			info->start[i] = base + (i * 0x00010000);
	} else if ((info->flash_id & FLASH_TYPEMASK) == FLASH_STMW320DT) {
		/* set sector offsets for top boot block type		*/
		base += info->size;
		i = info->sector_count;
		/*  1 x 16k boot sector */
		base -= 16 << 10;
		--i;
		info->start[i] = base;
		/*  2 x 8k  boot sectors */
		for (n=0; n<2; ++n) {
			base -= 8 << 10;
			--i;
			info->start[i] = base;
		}
		/*  1 x 32k boot sector */
		base -= 32 << 10;
		--i;
		info->start[i] = base;

		while (i > 0) {			/* 64k regular sectors	*/
			base -= 64 << 10;
			--i;
			info->start[i] = base;
		}
	} else if ((info->flash_id & FLASH_TYPEMASK) == FLASH_MXLV320T) {
		i = info->sector_count - 1;
		info->start[i--] = base + info->size - 0x00002000;
		info->start[i--] = base + info->size - 0x00004000;
		info->start[i--] = base + info->size - 0x00006000;
		info->start[i--] = base + info->size - 0x00008000;
		info->start[i--] = base + info->size - 0x0000a000;
		info->start[i--] = base + info->size - 0x0000c000;
		info->start[i--] = base + info->size - 0x0000e000;
		info->start[i--] = base + info->size - 0x00010000;

		for (; i >= 0; i--)
			info->start[i] = base + i * 0x00010000;
	} else {
		if (info->flash_id & FLASH_BTYPE) {
			/* set sector offsets for bottom boot block type */
			info->start[0] = base + 0x00000000;
			info->start[1] = base + 0x00002000;
			info->start[2] = base + 0x00004000;
			info->start[3] = base + 0x00006000;
			info->start[4] = base + 0x00008000;
			info->start[5] = base + 0x0000a000;
			info->start[6] = base + 0x0000c000;
			info->start[7] = base + 0x0000e000;
			for (i = 8; i < info->sector_count; i++) {
				info->start[i] =
				    base + ((i-7) * 0x00010000);
			}
		} else {
			/* set sector offsets for top boot block type */
			i = info->sector_count - 1;
			info->start[i--] = base + info->size - 0x00002000;
			info->start[i--] = base + info->size - 0x00004000;
			info->start[i--] = base + info->size - 0x00006000;
			info->start[i--] = base + info->size - 0x00008000;
			info->start[i--] = base + info->size - 0x0000a000;
			info->start[i--] = base + info->size - 0x0000c000;
			info->start[i--] = base + info->size - 0x0000e000;
			for (; i >= 0; i--) {
				info->start[i] = base + i * 0x00010000;
			}
		}
	}

	/* check for protected sectors */
	for (i = 0; i < info->sector_count; i++) {
		/* read sector protection at sector address, (A7 .. A0) = 0x02 */
		/* D0 = 1 if protected */
		addr2 = (volatile CONFIG_SYS_FLASH_WORD_SIZE *)(info->start[i]);

		/* For AMD29033C flash we need to resend the command of *
		 * reading flash protection for upper 8 Mb of flash     */
		if (i == 32) {
			addr2[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0xAAAAAAAA;
			addr2[CONFIG_SYS_FLASH_ADDR1] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x55555555;
			addr2[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x90909090;
		}

		if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_SST)
			info->protect[i] = 0;
		else
			info->protect[i] = addr2[2] & 1;
	}

	/* issue bank reset to return to read mode */
	addr2[0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00F000F0;

	return (info->size);
}

static int wait_for_DQ7_2(flash_info_t * info, int sect)
{
	ulong start, now, last;
	volatile CONFIG_SYS_FLASH_WORD_SIZE *addr =
	    (CONFIG_SYS_FLASH_WORD_SIZE *) (info->start[sect]);

	start = get_timer(0);
	last = start;
	while ((addr[0] & (CONFIG_SYS_FLASH_WORD_SIZE) 0x00800080) !=
	       (CONFIG_SYS_FLASH_WORD_SIZE) 0x00800080) {
		if ((now = get_timer(start)) > CONFIG_SYS_FLASH_ERASE_TOUT) {
			printf("Timeout\n");
			return -1;
		}
		/* show that we're waiting */
		if ((now - last) > 1000) {	/* every second */
			putc('.');
			last = now;
		}
	}
	return 0;
}

static int flash_erase_2(flash_info_t * info, int s_first, int s_last)
{
	volatile CONFIG_SYS_FLASH_WORD_SIZE *addr = (CONFIG_SYS_FLASH_WORD_SIZE *) (info->start[0]);
	volatile CONFIG_SYS_FLASH_WORD_SIZE *addr2;
	int flag, prot, sect;
	int i;

	if ((s_first < 0) || (s_first > s_last)) {
		if (info->flash_id == FLASH_UNKNOWN) {
			printf("- missing\n");
		} else {
			printf("- no sectors to erase\n");
		}
		return 1;
	}

	if (info->flash_id == FLASH_UNKNOWN) {
		printf("Can't erase unknown flash type - aborted\n");
		return 1;
	}

	prot = 0;
	for (sect = s_first; sect <= s_last; ++sect) {
		if (info->protect[sect]) {
			prot++;
		}
	}

	if (prot) {
		printf("- Warning: %d protected sectors will not be erased!\n",
		       prot);
	} else {
		printf("\n");
	}

	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts();

	/* Start erase on unprotected sectors */
	for (sect = s_first; sect <= s_last; sect++) {
		if (info->protect[sect] == 0) {	/* not protected */
			addr2 = (CONFIG_SYS_FLASH_WORD_SIZE *) (info->start[sect]);

			if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_SST) {
				addr[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00AA00AA;
				addr[CONFIG_SYS_FLASH_ADDR1] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00550055;
				addr[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00800080;
				addr[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00AA00AA;
				addr[CONFIG_SYS_FLASH_ADDR1] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00550055;
				addr2[0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00500050;	/* block erase */
				for (i = 0; i < 50; i++)
					udelay(1000);	/* wait 1 ms */
			} else {
				addr[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00AA00AA;
				addr[CONFIG_SYS_FLASH_ADDR1] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00550055;
				addr[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00800080;
				addr[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00AA00AA;
				addr[CONFIG_SYS_FLASH_ADDR1] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00550055;
				addr2[0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00300030;	/* sector erase */
			}
			/*
			 * Wait for each sector to complete, it's more
			 * reliable.  According to AMD Spec, you must
			 * issue all erase commands within a specified
			 * timeout.  This has been seen to fail, especially
			 * if printf()s are included (for debug)!!
			 */
			wait_for_DQ7_2(info, sect);
		}
	}

	/* re-enable interrupts if necessary */
	if (flag)
		enable_interrupts();

	/* wait at least 80us - let's wait 1 ms */
	udelay(1000);

	/* reset to read mode */
	addr = (CONFIG_SYS_FLASH_WORD_SIZE *) info->start[0];
	addr[0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00F000F0;	/* reset bank */

	printf(" done\n");
	return 0;
}

static int write_word_2(flash_info_t * info, ulong dest, ulong data)
{
	ulong *data_ptr = &data;
	volatile CONFIG_SYS_FLASH_WORD_SIZE *addr2 = (CONFIG_SYS_FLASH_WORD_SIZE *)(info->start[0]);
	volatile CONFIG_SYS_FLASH_WORD_SIZE *dest2 = (CONFIG_SYS_FLASH_WORD_SIZE *)dest;
	volatile CONFIG_SYS_FLASH_WORD_SIZE *data2 = (CONFIG_SYS_FLASH_WORD_SIZE *)data_ptr;
	ulong start;
	int i;

	/* Check if Flash is (sufficiently) erased */
	if ((*((vu_long *)dest) & data) != data) {
		return (2);
	}

	for (i = 0; i < 4 / sizeof(CONFIG_SYS_FLASH_WORD_SIZE); i++) {
		int flag;

		/* Disable interrupts which might cause a timeout here */
		flag = disable_interrupts();

		addr2[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00AA00AA;
		addr2[CONFIG_SYS_FLASH_ADDR1] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00550055;
		addr2[CONFIG_SYS_FLASH_ADDR0] = (CONFIG_SYS_FLASH_WORD_SIZE) 0x00A000A0;

		dest2[i] = data2[i];

		/* re-enable interrupts if necessary */
		if (flag)
			enable_interrupts();

		/* data polling for D7 */
		start = get_timer(0);
		while ((dest2[i] & (CONFIG_SYS_FLASH_WORD_SIZE) 0x00800080) !=
		       (data2[i] & (CONFIG_SYS_FLASH_WORD_SIZE) 0x00800080)) {

			if (get_timer(start) > CONFIG_SYS_FLASH_WRITE_TOUT) {
				return (1);
			}
		}
	}

	return (0);
}
#endif /* CONFIG_SYS_FLASH_2ND_16BIT_DEV */
