/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2007 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */

/*
 * yaffs_ramdisk.h: yaffs ram disk component
 */

#ifndef __YAFFS_RAMDISK_H__
#define __YAFFS_RAMDISK_H__


#include "yaffs_guts.h"
int yramdisk_EraseBlockInNAND(yaffs_Device *dev, int blockNumber);
int yramdisk_WriteChunkWithTagsToNAND(yaffs_Device *dev,int chunkInNAND,const __u8 *data, yaffs_ExtendedTags *tags);
int yramdisk_ReadChunkWithTagsFromNAND(yaffs_Device *dev,int chunkInNAND, __u8 *data, yaffs_ExtendedTags *tags);
int yramdisk_EraseBlockInNAND(yaffs_Device *dev, int blockNumber);
int yramdisk_InitialiseNAND(yaffs_Device *dev);
int yramdisk_MarkNANDBlockBad(yaffs_Device *dev,int blockNumber);
int yramdisk_QueryNANDBlock(yaffs_Device *dev, int blockNo, yaffs_BlockState *state, int *sequenceNumber);
#endif
