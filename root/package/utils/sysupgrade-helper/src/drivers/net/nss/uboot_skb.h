/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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

#ifndef _UBOOT_COMPAT_H__
#define _UBOOT_COMPAT_H__

#include <common.h>
#include <malloc.h>
#include <net.h>

#define netif_start_queue(x)
#define netif_stop_queue(x)
#define netif_wake_queue(x)

#define dev_addr                enetaddr
struct sk_buff
{
	u8 * data;
	u32 len;
	u8 * data_unaligned;
	u32 padding;
	char cb[16];
};

struct sk_buff * alloc_skb(u32 size, int dummy);
void dev_kfree_skb_any(struct sk_buff *skb);
void skb_reserve(struct sk_buff *skb, unsigned int len);
void skb_put(struct sk_buff *skb, unsigned int len);

#define dev_kfree_skb				dev_kfree_skb_any
#define dev_kfree_skb_irq			dev_kfree_skb_any

#endif	/* _UBOOT_COMPAT_H__ */
