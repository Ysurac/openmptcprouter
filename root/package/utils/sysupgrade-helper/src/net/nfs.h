/*
 * (C) Masami Komiya <mkomiya@sonare.it> 2004
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 */

#ifndef __NFS_H__
#define __NFS_H__

#define SUNRPC_PORT     111

#define PROG_PORTMAP    100000
#define PROG_NFS        100003
#define PROG_MOUNT      100005

#define MSG_CALL        0
#define MSG_REPLY       1

#define PORTMAP_GETPORT 3

#define MOUNT_ADDENTRY  1
#define MOUNT_UMOUNTALL 4

#define NFS_LOOKUP      4
#define NFS_READLINK    5
#define NFS_READ        6

#define NFS_FHSIZE      32

#define NFSERR_PERM     1
#define NFSERR_NOENT    2
#define NFSERR_ACCES    13
#define NFSERR_ISDIR    21
#define NFSERR_INVAL    22

/* Block size used for NFS read accesses.  A RPC reply packet (including  all
 * headers) must fit within a single Ethernet frame to avoid fragmentation.
 * However, if CONFIG_IP_DEFRAG is set, the config file may want to use a
 * bigger value. In any case, most NFS servers are optimized for a power of 2.
 */
#ifdef CONFIG_NFS_READ_SIZE
#define NFS_READ_SIZE CONFIG_NFS_READ_SIZE
#else
#define NFS_READ_SIZE 1024 /* biggest power of two that fits Ether frame */
#endif

#define NFS_MAXLINKDEPTH 16

struct rpc_t {
	union {
		uint8_t data[2048];
		struct {
			uint32_t id;
			uint32_t type;
			uint32_t rpcvers;
			uint32_t prog;
			uint32_t vers;
			uint32_t proc;
			uint32_t data[1];
		} call;
		struct {
			uint32_t id;
			uint32_t type;
			uint32_t rstatus;
			uint32_t verifier;
			uint32_t v2;
			uint32_t astatus;
			uint32_t data[19];
		} reply;
	} u;
};
extern void NfsStart(void);	/* Begin NFS */


/**********************************************************************/

#endif /* __NFS_H__ */
