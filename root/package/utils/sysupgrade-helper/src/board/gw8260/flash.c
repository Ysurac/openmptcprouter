/*
 * (C) Copyright 2000
 * Marius Groeger <mgroeger@sysgo.de>
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 *
 * (C) Copyright 2000, 2001
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2001
 * Advent Networks, Inc. <http://www.adventnetworks.com>
 * Oliver Brown <oliverb@alumni.utexas.net>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*********************************************************************/
/* DESCRIPTION:
 *   This file contains the flash routines for the GW8260 board.
 *
 *
 *
 * MODULE DEPENDENCY:
 *   None
 *
 *
 * RESTRICTIONS/LIMITATIONS:
 *
 *   Only supports the following flash devices:
 *     AMD 29F080B
 *     AMD 29F016D
 *
 * Copyright (c) 2001, Advent Networks, Inc.
 *
 */
/*********************************************************************/

#include <common.h>
#include <mpc8260.h>

flash_info_t flash_info[CONFIG_SYS_MAX_FLASH_BANKS];

static ulong flash_get_size (vu_long *addr, flash_info_t *info);
static int write_word (flash_info_t *info, ulong dest, ulong data);

/*********************************************************************/
/*		      functions					     */
/*********************************************************************/

/*
 * NAME: flash_init() -	 initializes flash banks
 *
 * DESCRIPTION:
 *   This function initializes the flash bank(s).
 *
 * RETURNS:
 *   The size in bytes of the flash
 *
 * RESTRICTIONS/LIMITATIONS:
 *
 *
 */
unsigned long flash_init(void)
{
	int i;

	/* Init: no FLASHes known */
	for (i = 0; i < CONFIG_SYS_MAX_FLASH_BANKS; ++i)
		flash_info[i].flash_id = FLASH_UNKNOWN;

	/* for now, only support the 4 MB Flash SIMM */
	(void)flash_get_size((vu_long *) CONFIG_SYS_FLASH0_BASE,
			      &flash_info[0]);
	/*
	 * protect monitor and environment sectors
	 */
#if CONFIG_SYS_MONITOR_BASE >= CONFIG_SYS_FLASH0_BASE
	flash_protect(FLAG_PROTECT_SET,
		      CONFIG_SYS_MONITOR_BASE,
		      CONFIG_SYS_MONITOR_BASE + monitor_flash_len - 1,
		      &flash_info[0]);
#endif

#if defined(CONFIG_ENV_IS_IN_FLASH) && defined(CONFIG_ENV_ADDR)
#ifndef CONFIG_ENV_SIZE
#define CONFIG_ENV_SIZE	CONFIG_ENV_SECT_SIZE
#endif
	flash_protect(FLAG_PROTECT_SET,
		      CONFIG_ENV_ADDR,
		      CONFIG_ENV_ADDR + CONFIG_ENV_SIZE - 1, &flash_info[0]);
#endif

	return CONFIG_SYS_FLASH0_SIZE * 1024 * 1024;	/*size */
}

/*********************************************************************/
/* NAME: flash_print_info() - prints flash imformation		     */
/*								     */
/* DESCRIPTION:							     */
/*   This function prints the flash information.		     */
/*								     */
/* INPUTS:							     */
/*   flash_info_t *info - flash information structure		     */
/*								     */
/* OUTPUTS:							     */
/*   Displays flash information to console			     */
/*								     */
/* RETURNS:							     */
/*   None							     */
/*								     */
/* RESTRICTIONS/LIMITATIONS:					     */
/*								     */
/*								     */
/*********************************************************************/
void flash_print_info  (flash_info_t *info)
{
    int i;

    if (info->flash_id == FLASH_UNKNOWN) {
	printf ("missing or unknown FLASH type\n");
	return;
    }

    switch ((info->flash_id >> 16) & 0xff) {
    case 0x1:
	printf ("AMD ");
	break;
    default:
	printf ("Unknown Vendor ");
	break;
    }

    switch (info->flash_id & FLASH_TYPEMASK) {
    case AMD_ID_F040B:
	printf ("AM29F040B (4 Mbit)\n");
	break;
    case AMD_ID_F080B:
	printf ("AM29F080B (8 Mbit)\n");
	break;
    case AMD_ID_F016D:
	printf ("AM29F016D (16 Mbit)\n");
	break;
    default:
	printf ("Unknown Chip Type\n");
	break;
    }

    printf ("  Size: %ld MB in %d Sectors\n",
	    info->size >> 20, info->sector_count);

    printf ("  Sector Start Addresses:");
    for (i=0; i<info->sector_count; ++i) {
	if ((i % 5) == 0)
	    printf ("\n	  ");
	printf (" %08lX%s",
		info->start[i],
		info->protect[i] ? " (RO)" : "	   "
	    );
    }
    printf ("\n");
    return;
}

/*********************************************************************/
/*   The following code cannot be run from FLASH!		     */
/*********************************************************************/

/*********************************************************************/
/* NAME: flash_get_size() - detects the flash size		     */
/*								     */
/* DESCRIPTION:							     */
/*   1) Reads vendor ID and devices ID from the flash devices.	     */
/*   2) Initializes flash info struct.				     */
/*   3) Return the flash size					     */
/*								     */
/* INPUTS:							     */
/*   vu_long *addr	- pointer to start of flash		     */
/*   flash_info_t *info - flash information structure		     */
/*								     */
/* OUTPUTS:							     */
/*   None							     */
/*								     */
/* RETURNS:							     */
/*   Size of the flash in bytes, or 0 if device id is unknown.	     */
/*								     */
/* RESTRICTIONS/LIMITATIONS:					     */
/*   Only supports the following devices:			     */
/*     AM29F080D						     */
/*     AM29F016D						     */
/*								     */
/*********************************************************************/
static ulong flash_get_size (vu_long *addr, flash_info_t *info)
{
    short i;
    vu_long vendor, devid;
    ulong base = (ulong)addr;

    /*printf("addr   = %08lx\n", (unsigned long)addr); */

    /* Reset and Write auto select command: read Manufacturer ID */
    addr[0x0000] = 0xf0f0f0f0;
    addr[0x0555] = 0xAAAAAAAA;
    addr[0x02AA] = 0x55555555;
    addr[0x0555] = 0x90909090;
    udelay (1000);

    vendor = addr[0];
    /*printf("vendor = %08lx\n", vendor); */
    if (vendor != 0x01010101) {
	info->size = 0;
	goto out;
    }

    devid = addr[1];
    /*printf("devid  = %08lx\n", devid); */

    if ((devid & 0xff) == AMD_ID_F080B) {
	info->flash_id	   = (vendor & 0xff) << 16 | AMD_ID_F080B;
	/* we have 16 sectors with 64KB each x 4 */
	info->sector_count = 16;
	info->size	   = 4 * info->sector_count * 64*1024;
    } else if ((devid & 0xff) == AMD_ID_F016D){
	info->flash_id	   = (vendor & 0xff) << 16 | AMD_ID_F016D;
	/* we have 32 sectors with 64KB each x 4 */
	info->sector_count = 32;
	info->size	   = 4 * info->sector_count * 64*1024;
    } else {
	info->size = 0;
	goto out;
    }
    /*printf("sector count = %08x\n", info->sector_count); */
    /* check for protected sectors */
    for (i = 0; i < info->sector_count; i++) {
	/* sector base address */
	info->start[i] = base + i * (info->size / info->sector_count);
	/* read sector protection at sector address, (A7 .. A0) = 0x02 */
	/* D0 = 1 if protected */
	addr = (volatile unsigned long *)(info->start[i]);
	info->protect[i] = addr[2] & 1;
    }

    /* reset command */
    addr = (vu_long *)info->start[0];

  out:
    addr[0] = 0xf0f0f0f0;

    /*printf("size = %08x\n", info->size); */
    return info->size;
}

/*********************************************************************/
/* NAME: flash_erase() - erases flash by sector			     */
/*								     */
/* DESCRIPTION:							     */
/*   This function erases flash sectors starting for s_first to	     */
/*   s_last.							     */
/*								     */
/* INPUTS:							     */
/*   flash_info_t *info - flash information structure		     */
/*   int s_first - first sector to erase			     */
/*   int s_last	 - last sector to erase				     */
/*								     */
/* OUTPUTS:							     */
/*   None							     */
/*								     */
/* RETURNS:							     */
/*   Returns 0 for success, 1 for failure.			     */
/*								     */
/* RESTRICTIONS/LIMITATIONS:					     */
/*								     */
/*********************************************************************/
int flash_erase (flash_info_t *info, int s_first, int s_last)
{
    vu_long *addr = (vu_long*)(info->start[0]);
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
	if (info->protect[sect]) {
	    prot++;
	}
    }

    if (prot) {
	printf ("- Warning: %d protected sectors will not be erased!\n",
		prot);
    } else {
	printf ("\n");
    }

    l_sect = -1;

    /* Disable interrupts which might cause a timeout here */
    flag = disable_interrupts();

    addr[0x0555] = 0xAAAAAAAA;
    addr[0x02AA] = 0x55555555;
    addr[0x0555] = 0x80808080;
    addr[0x0555] = 0xAAAAAAAA;
    addr[0x02AA] = 0x55555555;
    udelay (100);

    /* Start erase on unprotected sectors */
    for (sect = s_first; sect <= s_last; sect++) {
	if (info->protect[sect] == 0) { /* not protected */
	    addr = (vu_long*)(info->start[sect]);
	    addr[0] = 0x30303030;
	    l_sect = sect;
	}
    }

    /* re-enable interrupts if necessary */
    if (flag)
	enable_interrupts();

    /* wait at least 80us - let's wait 1 ms */
    udelay (1000);

    /*
     * We wait for the last triggered sector
     */
    if (l_sect < 0)
	goto DONE;

    start = get_timer (0);
    last  = start;
    addr = (vu_long*)(info->start[l_sect]);
    while ((addr[0] & 0x80808080) != 0x80808080) {
	if ((now = get_timer(start)) > CONFIG_SYS_FLASH_ERASE_TOUT) {
	    printf ("Timeout\n");
	    return 1;
	}
	/* show that we're waiting */
	if ((now - last) > 1000) {  /* every second */
	    serial_putc ('.');
	    last = now;
	}
    }

  DONE:
    /* reset to read mode */
    addr = (volatile unsigned long *)info->start[0];
    addr[0] = 0xF0F0F0F0;   /* reset bank */

    printf (" done\n");
    return 0;
}

/*********************************************************************/
/* NAME: write_buff() - writes a buffer to flash		     */
/*								     */
/* DESCRIPTION:							     */
/*   This function copies a buffer, *src, to flash.		     */
/*								     */
/* INPUTS:							     */
/*  flash_info_t *info - flash information structure		     */
/*  uchar *src - pointer to buffer to write to flash		     */
/*  ulong addr - address to start write at			     */
/*  ulong cnt - number of bytes to write to flash		     */
/*								     */
/* OUTPUTS:							     */
/*   None							     */
/*								     */
/* RETURNS:							     */
/*   0 - OK							     */
/*   1 - write timeout						     */
/*   2 - Flash not erased					     */
/*								     */
/* RESTRICTIONS/LIMITATIONS:					     */
/*								     */
/*********************************************************************/
int write_buff (flash_info_t *info, uchar *src, ulong addr, ulong cnt)
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
	    data = (data << 8) | (*(uchar *)cp);
	}
	for (; (i < 4) && (cnt > 0); ++i) {
	    data = (data << 8) | *src++;
	    --cnt;
	    ++cp;
	}
	for (; (cnt == 0) && (i < 4); ++i, ++cp) {
	    data = (data << 8) | (*(uchar *)cp);
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
	wp  += 4;
	cnt -= 4;
    }

    if (cnt == 0) {
	return (0);
    }

    /*
     * handle unaligned tail bytes
     */
    data = 0;
    for (i = 0, cp = wp; (i < 4) && (cnt > 0); ++i, ++cp) {
	data = (data << 8) | *src++;
	--cnt;
    }
    for (; (i < 4); ++i, ++cp) {
	data = (data << 8) | (*(uchar *)cp);
    }

    return (write_word(info, wp, data));
}

/*********************************************************************/
/* NAME: write_word() - writes a word to flash			     */
/*								     */
/* DESCRIPTION:							     */
/*   This writes a single word to flash.			     */
/*								     */
/* INPUTS:							     */
/*  flash_info_t *info - flash information structure		     */
/*  ulong dest - address to write				     */
/*  ulong data - data to write					     */
/*								     */
/* OUTPUTS:							     */
/*   None							     */
/*								     */
/* RETURNS:							     */
/*   0 - OK							     */
/*   1 - write timeout						     */
/*   2 - Flash not erased					     */
/*								     */
/* RESTRICTIONS/LIMITATIONS:					     */
/*								     */
/*********************************************************************/
static int write_word (flash_info_t *info, ulong dest, ulong data)
{
    vu_long *addr = (vu_long*)(info->start[0]);
    ulong start;
    int flag;

    /* Check if Flash is (sufficiently) erased */
    if ((*((vu_long *)dest) & data) != data) {
	return (2);
    }
    /* Disable interrupts which might cause a timeout here */
    flag = disable_interrupts();

    addr[0x0555] = 0xAAAAAAAA;
    addr[0x02AA] = 0x55555555;
    addr[0x0555] = 0xA0A0A0A0;

    *((vu_long *)dest) = data;

    /* re-enable interrupts if necessary */
    if (flag)
	enable_interrupts();

    /* data polling for D7 */
    start = get_timer (0);
    while ((*((vu_long *)dest) & 0x80808080) != (data & 0x80808080)) {
	if (get_timer(start) > CONFIG_SYS_FLASH_WRITE_TOUT) {
	    return (1);
	}
    }
    return (0);
}
/*********************************************************************/
/*			   End of flash.c			     */
/*********************************************************************/
