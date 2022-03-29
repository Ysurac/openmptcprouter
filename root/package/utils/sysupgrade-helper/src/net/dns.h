/*
 * (C) Masami Komiya <mkomiya@sonare.it> 2005
 *  Copyright 2009, Robin Getz <rgetz@blackfin.uclinux.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 */

#ifndef __DNS_H__
#define __DNS_H__

#define DNS_SERVICE_PORT 53
#define DNS_TIMEOUT      10000UL

/* http://en.wikipedia.org/wiki/List_of_DNS_record_types */
enum dns_query_type {
	DNS_A_RECORD = 0x01,
	DNS_CNAME_RECORD = 0x05,
	DNS_MX_RECORD = 0x0f,
};

/*
 * DNS network packet
 */
struct header {
	uint16_t	tid;		/* Transaction ID */
	uint16_t	flags;		/* Flags */
	uint16_t	nqueries;	/* Questions */
	uint16_t	nanswers;	/* Answers */
	uint16_t	nauth;		/* Authority PRs */
	uint16_t	nother;		/* Other PRs */
	unsigned char	data[1];	/* Data, variable length */
};

extern void DnsStart(void);		/* Begin DNS */

#endif
