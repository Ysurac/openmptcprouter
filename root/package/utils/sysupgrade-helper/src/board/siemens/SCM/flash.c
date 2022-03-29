/*
 * (C) Copyright 2001, 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * Flash Routines for AMD devices on the TQM8260 board
 *
 *--------------------------------------------------------------------
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

#include <common.h>
#include <mpc8xx.h>

#define V_ULONG(a)	(*(volatile unsigned long *)( a ))
#define V_BYTE(a)	(*(volatile unsigned char *)( a ))


flash_info_t flash_info[CONFIG_SYS_MAX_FLASH_BANKS];


/*-----------------------------------------------------------------------
 */
void flash_reset (void)
{
	if (flash_info[0].flash_id != FLASH_UNKNOWN) {
		V_ULONG (flash_info[0].start[0]) = 0x00F000F0;
		V_ULONG (flash_info[0].start[0] + 4) = 0x00F000F0;
	}
}

/*-----------------------------------------------------------------------
 */
ulong flash_get_size (ulong baseaddr, flash_info_t * info)
{
	short i;
	unsigned long flashtest_h, flashtest_l;

	/* Write auto select command sequence and test FLASH answer */
	V_ULONG (baseaddr + ((ulong) 0x0555 << 3)) = 0x00AA00AA;
	V_ULONG (baseaddr + ((ulong) 0x02AA << 3)) = 0x00550055;
	V_ULONG (baseaddr + ((ulong) 0x0555 << 3)) = 0x00900090;
	V_ULONG (baseaddr + 4 + ((ulong) 0x0555 << 3)) = 0x00AA00AA;
	V_ULONG (baseaddr + 4 + ((ulong) 0x02AA << 3)) = 0x00550055;
	V_ULONG (baseaddr + 4 + ((ulong) 0x0555 << 3)) = 0x00900090;

	flashtest_h = V_ULONG (baseaddr);	/* manufacturer ID     */
	flashtest_l = V_ULONG (baseaddr + 4);

	switch ((int) flashtest_h) {
	case AMD_MANUFACT:
		info->flash_id = FLASH_MAN_AMD;
		break;
	case FUJ_MANUFACT:
		info->flash_id = FLASH_MAN_FUJ;
		break;
	default:
		info->flash_id = FLASH_UNKNOWN;
		info->sector_count = 0;
		info->size = 0;
		return (0);			/* no or unknown flash     */
	}

	flashtest_h = V_ULONG (baseaddr + 8);	/* device ID           */
	flashtest_l = V_ULONG (baseaddr + 12);
	if (flashtest_h != flashtest_l) {
		info->flash_id = FLASH_UNKNOWN;
	} else {
		switch (flashtest_h) {
		case AMD_ID_LV800T:
			info->flash_id += FLASH_AM800T;
			info->sector_count = 19;
			info->size = 0x00400000;
			break;			/* 4 * 1 MB = 4 MB  */
		case AMD_ID_LV800B:
			info->flash_id += FLASH_AM800B;
			info->sector_count = 19;
			info->size = 0x00400000;
			break;			/* 4 * 1 MB = 4 MB  */
		case AMD_ID_LV160T:
			info->flash_id += FLASH_AM160T;
			info->sector_count = 35;
			info->size = 0x00800000;
			break;			/* 4 * 2 MB = 8 MB  */
		case AMD_ID_LV160B:
			info->flash_id += FLASH_AM160B;
			info->sector_count = 35;
			info->size = 0x00800000;
			break;			/* 4 * 2 MB = 8 MB  */
		case AMD_ID_DL322T:
			info->flash_id += FLASH_AMDL322T;
			info->sector_count = 71;
			info->size = 0x01000000;
			break;			/* 4 * 4 MB = 16 MB */
		case AMD_ID_DL322B:
			info->flash_id += FLASH_AMDL322B;
			info->sector_count = 71;
			info->size = 0x01000000;
			break;			/* 4 * 4 MB = 16 MB */
		case AMD_ID_DL323T:
			info->flash_id += FLASH_AMDL323T;
			info->sector_count = 71;
			info->size = 0x01000000;
			break;			/* 4 * 4 MB = 16 MB */
		case AMD_ID_DL323B:
			info->flash_id += FLASH_AMDL323B;
			info->sector_count = 71;
			info->size = 0x01000000;
			break;			/* 4 * 4 MB = 16 MB */
		case AMD_ID_LV640U:
			info->flash_id += FLASH_AM640U;
			info->sector_count = 128;
			info->size = 0x02000000;
			break;			/* 4 * 8 MB = 32 MB */
		default:
			info->flash_id = FLASH_UNKNOWN;
			return (0);		/* no or unknown flash     */
		}
	}

	if (flashtest_h == AMD_ID_LV640U) {

		/* set up sector start adress table (uniform sector type) */
		for (i = 0; i < info->sector_count; i++)
			info->start[i] = baseaddr + (i * 0x00040000);

	} else if (info->flash_id & FLASH_BTYPE) {

		/* set up sector start adress table (bottom sector type) */
		info->start[0] = baseaddr + 0x00000000;
		info->start[1] = baseaddr + 0x00010000;
		info->start[2] = baseaddr + 0x00018000;
		info->start[3] = baseaddr + 0x00020000;
		for (i = 4; i < info->sector_count; i++) {
			info->start[i] = baseaddr + (i * 0x00040000) - 0x000C0000;
		}

	} else {

		/* set up sector start adress table (top sector type) */
		i = info->sector_count - 1;
		info->start[i--] = baseaddr + info->size - 0x00010000;
		info->start[i--] = baseaddr + info->size - 0x00018000;
		info->start[i--] = baseaddr + info->size - 0x00020000;
		for (; i >= 0; i--) {
			info->start[i] = baseaddr + i * 0x00040000;
		}
	}

	/* check for protected sectors */
	for (i = 0; i < info->sector_count; i++) {
		/* read sector protection at sector address, (A7 .. A0) = 0x02 */
		if ((V_ULONG (info->start[i] + 16) & 0x00010001) ||
			(V_ULONG (info->start[i] + 20) & 0x00010001)) {
			info->protect[i] = 1;	/* D0 = 1 if protected */
		} else {
			info->protect[i] = 0;
		}
	}

	flash_reset ();
	return (info->size);
}

/*-----------------------------------------------------------------------
 */
unsigned long flash_init (void)
{
	unsigned long size_b0 = 0;
	int i;

	/* Init: no FLASHes known */
	for (i = 0; i < CONFIG_SYS_MAX_FLASH_BANKS; ++i) {
		flash_info[i].flash_id = FLASH_UNKNOWN;
	}

	/* Static FLASH Bank configuration here (only one bank) */

	size_b0 = flash_get_size (CONFIG_SYS_FLASH0_BASE, &flash_info[0]);
	if (flash_info[0].flash_id == FLASH_UNKNOWN || size_b0 == 0) {
		printf ("## Unknown FLASH on Bank 0 - Size = 0x%08lx = %ld MB\n",
				size_b0, size_b0 >> 20);
	}

	/*
	 * protect monitor and environment sectors
	 */

#if CONFIG_SYS_MONITOR_BASE >= CONFIG_SYS_FLASH0_BASE
	flash_protect (FLAG_PROTECT_SET,
		       CONFIG_SYS_MONITOR_BASE,
		       CONFIG_SYS_MONITOR_BASE + monitor_flash_len - 1, &flash_info[0]);
#endif

#if defined(CONFIG_ENV_IS_IN_FLASH) && defined(CONFIG_ENV_ADDR)
# ifndef  CONFIG_ENV_SIZE
#  define CONFIG_ENV_SIZE	CONFIG_ENV_SECT_SIZE
# endif
	flash_protect (FLAG_PROTECT_SET,
		       CONFIG_ENV_ADDR,
		       CONFIG_ENV_ADDR + CONFIG_ENV_SIZE - 1, &flash_info[0]);
#endif

	return (size_b0);
}

/*-----------------------------------------------------------------------
 */
void flash_print_info (flash_info_t * info)
{
	int i;

	if (info->flash_id == FLASH_UNKNOWN) {
		printf ("missing or unknown FLASH type\n");
		return;
	}

	switch (info->flash_id & FLASH_VENDMASK) {
	case FLASH_MAN_AMD:
		printf ("AMD ");
		break;
	case FLASH_MAN_FUJ:
		printf ("FUJITSU ");
		break;
	default:
		printf ("Unknown Vendor ");
		break;
	}

	switch (info->flash_id & FLASH_TYPEMASK) {
	case FLASH_AM800T:
		printf ("29LV800T (8 M, top sector)\n");
		break;
	case FLASH_AM800B:
		printf ("29LV800T (8 M, bottom sector)\n");
		break;
	case FLASH_AM160T:
		printf ("29LV160T (16 M, top sector)\n");
		break;
	case FLASH_AM160B:
		printf ("29LV160B (16 M, bottom sector)\n");
		break;
	case FLASH_AMDL322T:
		printf ("29DL322T (32 M, top sector)\n");
		break;
	case FLASH_AMDL322B:
		printf ("29DL322B (32 M, bottom sector)\n");
		break;
	case FLASH_AMDL323T:
		printf ("29DL323T (32 M, top sector)\n");
		break;
	case FLASH_AMDL323B:
		printf ("29DL323B (32 M, bottom sector)\n");
		break;
	case FLASH_AM640U:
		printf ("29LV640D (64 M, uniform sector)\n");
		break;
	default:
		printf ("Unknown Chip Type\n");
		break;
	}

	printf ("  Size: %ld MB in %d Sectors\n",
			info->size >> 20, info->sector_count);

	printf ("  Sector Start Addresses:");
	for (i = 0; i < info->sector_count; ++i) {
		if ((i % 5) == 0)
			printf ("\n   ");
		printf (" %08lX%s",
			info->start[i],
			info->protect[i] ? " (RO)" : "     "
		);
	}
	printf ("\n");
	return;
}

/*-----------------------------------------------------------------------
 */
int flash_erase (flash_info_t * info, int s_first, int s_last)
{
	int flag, prot, sect, l_sect;
	ulong start, now, last;

	if ((s_first < 0) || (s_first > s_last)) {
		if (info->flash_id == FLASH_UNKNOWN) {
			printf ("- missing\n");
		} else {
			printf ("- no sectors to erase\n");
		}
		return 1;
	}

	prot = 0;
	for (sect = s_first; sect <= s_last; sect++) {
		if (info->protect[sect])
			prot++;
	}

	if (prot) {
		printf ("- Warning: %d protected sectors will not be erased!\n",
			prot);
	} else {
		printf ("\n");
	}

	l_sect = -1;

	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts ();

	V_ULONG (info->start[0] + (0x0555 << 3)) = 0x00AA00AA;
	V_ULONG (info->start[0] + (0x02AA << 3)) = 0x00550055;
	V_ULONG (info->start[0] + (0x0555 << 3)) = 0x00800080;
	V_ULONG (info->start[0] + (0x0555 << 3)) = 0x00AA00AA;
	V_ULONG (info->start[0] + (0x02AA << 3)) = 0x00550055;
	V_ULONG (info->start[0] + 4 + (0x0555 << 3)) = 0x00AA00AA;
	V_ULONG (info->start[0] + 4 + (0x02AA << 3)) = 0x00550055;
	V_ULONG (info->start[0] + 4 + (0x0555 << 3)) = 0x00800080;
	V_ULONG (info->start[0] + 4 + (0x0555 << 3)) = 0x00AA00AA;
	V_ULONG (info->start[0] + 4 + (0x02AA << 3)) = 0x00550055;
	udelay (1000);

	/* Start erase on unprotected sectors */
	for (sect = s_first; sect <= s_last; sect++) {
		if (info->protect[sect] == 0) {	/* not protected */
			V_ULONG (info->start[sect]) = 0x00300030;
			V_ULONG (info->start[sect] + 4) = 0x00300030;
			l_sect = sect;
		}
	}

	/* re-enable interrupts if necessary */
	if (flag)
		enable_interrupts ();

	/* wait at least 80us - let's wait 1 ms */
	udelay (1000);

	/*
	 * We wait for the last triggered sector
	 */
	if (l_sect < 0)
		goto DONE;

	start = get_timer (0);
	last = start;
	while ((V_ULONG (info->start[l_sect]) & 0x00800080) != 0x00800080 ||
	       (V_ULONG (info->start[l_sect] + 4) & 0x00800080) != 0x00800080)
	{
		if ((now = get_timer (start)) > CONFIG_SYS_FLASH_ERASE_TOUT) {
			printf ("Timeout\n");
			return 1;
		}
		/* show that we're waiting */
		if ((now - last) > 1000) {	/* every second */
			serial_putc ('.');
			last = now;
		}
	}

  DONE:
	/* reset to read mode */
	flash_reset ();

	printf (" done\n");
	return 0;
}

static int write_dword (flash_info_t *, ulong, unsigned char *);

/*-----------------------------------------------------------------------
 * Copy memory to flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */

int write_buff (flash_info_t * info, uchar * src, ulong addr, ulong cnt)
{
	ulong dp;
	static unsigned char bb[8];
	int i, l, rc, cc = cnt;

	dp = (addr & ~7);		/* get lower dword aligned address */

	/*
	 * handle unaligned start bytes
	 */
	if ((l = addr - dp) != 0) {
		for (i = 0; i < 8; i++)
			bb[i] = (i < l || (i - l) >= cc) ? V_BYTE (dp + i) : *src++;
		if ((rc = write_dword (info, dp, bb)) != 0) {
			return (rc);
		}
		dp += 8;
		cc -= 8 - l;
	}

	/*
	 * handle word aligned part
	 */
	while (cc >= 8) {
		if ((rc = write_dword (info, dp, src)) != 0) {
			return (rc);
		}
		dp += 8;
		src += 8;
		cc -= 8;
	}

	if (cc <= 0) {
		return (0);
	}

	/*
	 * handle unaligned tail bytes
	 */
	for (i = 0; i < 8; i++) {
		bb[i] = (i < cc) ? *src++ : V_BYTE (dp + i);
	}
	return (write_dword (info, dp, bb));
}

/*-----------------------------------------------------------------------
 * Write a dword to Flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
static int write_dword (flash_info_t * info, ulong dest, unsigned char *pdata)
{
	ulong start, cl, ch;
	int flag, i;

	for (ch = 0, i = 0; i < 4; i++)
		ch = (ch << 8) + *pdata++;	/* high word    */
	for (cl = 0, i = 0; i < 4; i++)
		cl = (cl << 8) + *pdata++;	/* low word */

	/* Check if Flash is (sufficiently) erased */
	if ((*((vu_long *) dest) & ch) != ch
		|| (*((vu_long *) (dest + 4)) & cl) != cl) {
		return (2);
	}

	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts ();

	V_ULONG (info->start[0] + (0x0555 << 3)) = 0x00AA00AA;
	V_ULONG (info->start[0] + (0x02AA << 3)) = 0x00550055;
	V_ULONG (info->start[0] + (0x0555 << 3)) = 0x00A000A0;
	V_ULONG (dest) = ch;
	V_ULONG (info->start[0] + 4 + (0x0555 << 3)) = 0x00AA00AA;
	V_ULONG (info->start[0] + 4 + (0x02AA << 3)) = 0x00550055;
	V_ULONG (info->start[0] + 4 + (0x0555 << 3)) = 0x00A000A0;
	V_ULONG (dest + 4) = cl;

	/* re-enable interrupts if necessary */
	if (flag)
		enable_interrupts ();

	/* data polling for D7 */
	start = get_timer (0);
	while (((V_ULONG (dest) & 0x00800080) != (ch & 0x00800080)) ||
		   ((V_ULONG (dest + 4) & 0x00800080) != (cl & 0x00800080))) {
		if (get_timer (start) > CONFIG_SYS_FLASH_WRITE_TOUT) {
			return (1);
		}
	}
	return (0);
}
