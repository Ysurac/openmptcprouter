/*
 * Copyright (C) 2004-2006 Atmel Corporation
 *
 * Based on linux/include/asm-arm/setup.h
 *   Copyright (C) 1997-1999 Russel King
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
#ifndef __ASM_AVR32_SETUP_H__
#define __ASM_AVR32_SETUP_H__

#define COMMAND_LINE_SIZE 256

/* Magic number indicating that a tag table is present */
#define ATAG_MAGIC	0xa2a25441

#ifndef __ASSEMBLY__

/*
 * Generic memory range, used by several tags.
 *
 *   addr is always physical.
 *   size is measured in bytes.
 *   next is for use by the OS, e.g. for grouping regions into
 *	  linked lists.
 */
struct tag_mem_range {
	u32			addr;
	u32			size;
	struct tag_mem_range *	next;
};

/* The list ends with an ATAG_NONE node. */
#define ATAG_NONE	0x00000000

struct tag_header {
	u32 size;
	u32 tag;
};

/* The list must start with an ATAG_CORE node */
#define ATAG_CORE	0x54410001

struct tag_core {
	u32 flags;
	u32 pagesize;
	u32 rootdev;
};

/* it is allowed to have multiple ATAG_MEM nodes */
#define ATAG_MEM	0x54410002
/* ATAG_MEM uses tag_mem_range */

/* command line: \0 terminated string */
#define ATAG_CMDLINE	0x54410003

struct tag_cmdline {
	char	cmdline[1];	/* this is the minimum size */
};

/* Ramdisk image (may be compressed) */
#define ATAG_RDIMG	0x54410004
/* ATAG_RDIMG uses tag_mem_range */

/* Information about various clocks present in the system */
#define ATAG_CLOCK	0x54410005

struct tag_clock {
	u32	clock_id;	/* Which clock are we talking about? */
	u32	clock_flags;	/* Special features */
	u64	clock_hz;	/* Clock speed in Hz */
};

/* The clock types we know about */
#define ACLOCK_BOOTCPU	0	/* The CPU we're booting from */
#define ACLOCK_HSB	1	/* Deprecated */

/* Memory reserved for the system (e.g. the bootloader) */
#define ATAG_RSVD_MEM	0x54410006
/* ATAG_RSVD_MEM uses tag_mem_range */

/* Ethernet information */

#define ATAG_ETHERNET	0x54410007

struct tag_ethernet {
	u8	mac_index;
	u8	mii_phy_addr;
	u8	hw_address[6];
};

#define AETH_INVALID_PHY	0xff

/* board information information */
#define ATAG_BOARDINFO	0x54410008

struct tag_boardinfo {
	u32	board_number;
};

struct tag {
	struct tag_header hdr;
	union {
		struct tag_core core;
		struct tag_mem_range mem_range;
		struct tag_cmdline cmdline;
		struct tag_clock clock;
		struct tag_ethernet ethernet;
		struct tag_boardinfo boardinfo;
	} u;
};

struct tagtable {
	u32	tag;
	int	(*parse)(struct tag *);
};

#define __tag __attribute_used__ __attribute__((__section__(".taglist")))
#define __tagtable(tag, fn)						\
	static struct tagtable __tagtable_##fn __tag = { tag, fn }

#define tag_member_present(tag,member)					\
	((unsigned long)(&((struct tag *)0L)->member + 1)		\
	 <= (tag)->hdr.size * 4)

#define tag_next(t)	((struct tag *)((u32 *)(t) + (t)->hdr.size))
#define tag_size(type)	((sizeof(struct tag_header) + sizeof(struct type)) >> 2)

#define for_each_tag(t,base)						\
	for (t = base; t->hdr.size; t = tag_next(t))

#endif /* !__ASSEMBLY__ */

#endif /* __ASM_AVR32_SETUP_H__ */
