/*
 * Copyright (C) 2004-2007 Freescale Semiconductor, Inc.
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

#ifndef _MCD_API_H
#define _MCD_API_H

/* Turn Execution Unit tasks ON (#define) or OFF (#undef) */
#undef MCD_INCLUDE_EU

/* Number of DMA channels */
#define NCHANNELS	16

/* Total number of variants */
#ifdef MCD_INCLUDE_EU
#define NUMOFVARIANTS	6
#else
#define NUMOFVARIANTS	4
#endif

/* Define sizes of the various tables */
#define TASK_TABLE_SIZE		(NCHANNELS*32)
#define VAR_TAB_SIZE		(128)
#define CONTEXT_SAVE_SIZE	(128)
#define FUNCDESC_TAB_SIZE	(256)

#ifdef MCD_INCLUDE_EU
#define FUNCDESC_TAB_NUM	16
#else
#define FUNCDESC_TAB_NUM	1
#endif

#ifndef DEFINESONLY

/* Portability typedefs */
#if 1
#include "common.h"
#else
#ifndef s32
typedef int s32;
#endif
#ifndef u32
typedef unsigned int u32;
#endif
#ifndef s16
typedef short s16;
#endif
#ifndef u16
typedef unsigned short u16;
#endif
#ifndef s8
typedef char s8;
#endif
#ifndef u8
typedef unsigned char u8;
#endif
#endif

/*
 * These structures represent the internal registers of the
 * multi-channel DMA
 */
struct dmaRegs_s {
	u32 taskbar;		/* task table base address */
	u32 currPtr;
	u32 endPtr;
	u32 varTablePtr;
	u16 dma_rsvd0;
	u16 ptdControl;		/* ptd control */
	u32 intPending;		/* interrupt pending */
	u32 intMask;		/* interrupt mask */
	u16 taskControl[16];	/* task control */
	u8 priority[32];	/* priority */
	u32 initiatorMux;	/* initiator mux control */
	u32 taskSize0;		/* task size control 0. */
	u32 taskSize1;		/* task size control 1. */
	u32 dma_rsvd1;		/* reserved */
	u32 dma_rsvd2;		/* reserved */
	u32 debugComp1;		/* debug comparator 1 */
	u32 debugComp2;		/* debug comparator 2 */
	u32 debugControl;	/* debug control */
	u32 debugStatus;	/* debug status */
	u32 ptdDebug;		/* priority task decode debug */
	u32 dma_rsvd3[31];	/* reserved */
};
typedef volatile struct dmaRegs_s dmaRegs;

#endif

/* PTD contrl reg bits */
#define PTD_CTL_TSK_PRI		0x8000
#define PTD_CTL_COMM_PREFETCH	0x0001

/* Task Control reg bits and field masks */
#define TASK_CTL_EN		0x8000
#define TASK_CTL_VALID		0x4000
#define TASK_CTL_ALWAYS		0x2000
#define TASK_CTL_INIT_MASK	0x1f00
#define TASK_CTL_ASTRT		0x0080
#define TASK_CTL_HIPRITSKEN	0x0040
#define TASK_CTL_HLDINITNUM	0x0020
#define TASK_CTL_ASTSKNUM_MASK	0x000f

/* Priority reg bits and field masks */
#define PRIORITY_HLD		0x80
#define PRIORITY_PRI_MASK	0x07

/* Debug Control reg bits and field masks */
#define DBG_CTL_BLOCK_TASKS_MASK	0xffff0000
#define DBG_CTL_AUTO_ARM		0x00008000
#define DBG_CTL_BREAK			0x00004000
#define DBG_CTL_COMP1_TYP_MASK		0x00003800
#define DBG_CTL_COMP2_TYP_MASK		0x00000070
#define DBG_CTL_EXT_BREAK		0x00000004
#define DBG_CTL_INT_BREAK		0x00000002

/*
 * PTD Debug reg selector addresses
 * This reg must be written with a value to show the contents of
 * one of the desired internal register.
 */
#define PTD_DBG_REQ		0x00	/* shows the state of 31 initiators */
#define PTD_DBG_TSK_VLD_INIT	0x01	/* shows which 16 tasks are valid and
					   have initiators asserted */

/* General return values */
#define MCD_OK			0
#define MCD_ERROR		-1
#define MCD_TABLE_UNALIGNED	-2
#define MCD_CHANNEL_INVALID	-3

/* MCD_initDma input flags */
#define MCD_RELOC_TASKS		0x00000001
#define MCD_NO_RELOC_TASKS	0x00000000
#define MCD_COMM_PREFETCH_EN	0x00000002	/* MCF547x/548x ONLY */

/*
 * MCD_dmaStatus Status Values for each channel:
 * MCD_NO_DMA	- No DMA has been requested since reset
 * MCD_IDLE	- DMA active, but the initiator is currently inactive
 * MCD_RUNNING	- DMA active, and the initiator is currently active
 * MCD_PAUSED	- DMA active but it is currently paused
 * MCD_HALTED	- the most recent DMA has been killed with MCD_killTask()
 * MCD_DONE	- the most recent DMA has completed
 */
#define MCD_NO_DMA		1
#define MCD_IDLE		2
#define MCD_RUNNING		3
#define MCD_PAUSED		4
#define MCD_HALTED		5
#define MCD_DONE		6

/* MCD_startDma parameter defines */

/* Constants for the funcDesc parameter */
/*
 * MCD_NO_BYTE_SWAP	- to disable byte swapping
 * MCD_BYTE_REVERSE	- to reverse the bytes of each u32 of the DMAed data
 * MCD_U16_REVERSE	- to reverse the 16-bit halves of each 32-bit data
 *			  value being DMAed
 * MCD_U16_BYTE_REVERSE	- to reverse the byte halves of each 16-bit half of
 *			  each 32-bit data value DMAed
 * MCD_NO_BIT_REV	- do not reverse the bits of each byte DMAed
 * MCD_BIT_REV		- reverse the bits of each byte DMAed
 * MCD_CRC16		- to perform CRC-16 on DMAed data
 * MCD_CRCCCITT		- to perform CRC-CCITT on DMAed data
 * MCD_CRC32		- to perform CRC-32 on DMAed data
 * MCD_CSUMINET		- to perform internet checksums on DMAed data
 * MCD_NO_CSUM		- to perform no checksumming
 */
#define MCD_NO_BYTE_SWAP	0x00045670
#define MCD_BYTE_REVERSE	0x00076540
#define MCD_U16_REVERSE		0x00067450
#define MCD_U16_BYTE_REVERSE	0x00054760
#define MCD_NO_BIT_REV		0x00000000
#define MCD_BIT_REV		0x00088880
/* CRCing: */
#define MCD_CRC16		0xc0100000
#define MCD_CRCCCITT		0xc0200000
#define MCD_CRC32		0xc0300000
#define MCD_CSUMINET		0xc0400000
#define MCD_NO_CSUM		0xa0000000

#define MCD_FUNC_NOEU1		(MCD_NO_BYTE_SWAP | MCD_NO_BIT_REV | \
				 MCD_NO_CSUM)
#define MCD_FUNC_NOEU2		(MCD_NO_BYTE_SWAP | MCD_NO_CSUM)

/* Constants for the flags parameter */
#define MCD_TT_FLAGS_RL		0x00000001	/* Read line */
#define MCD_TT_FLAGS_CW		0x00000002	/* Combine Writes */
#define MCD_TT_FLAGS_SP		0x00000004	/* MCF547x/548x ONLY  */
#define MCD_TT_FLAGS_MASK	0x000000ff
#define MCD_TT_FLAGS_DEF	(MCD_TT_FLAGS_RL | MCD_TT_FLAGS_CW)

#define MCD_SINGLE_DMA		0x00000100	/* Unchained DMA */
#define MCD_CHAIN_DMA		/* TBD */
#define MCD_EU_DMA		/* TBD */
#define MCD_FECTX_DMA		0x00001000	/* FEC TX ring DMA */
#define MCD_FECRX_DMA		0x00002000	/* FEC RX ring DMA */

/* these flags are valid for MCD_startDma and the chained buffer descriptors */
/*
 * MCD_BUF_READY	- indicates that this buf is now under the DMA's ctrl
 * MCD_WRAP		- to tell the FEC Dmas to wrap to the first BD
 * MCD_INTERRUPT	- to generate an interrupt after completion of the DMA
 * MCD_END_FRAME	- tell the DMA to end the frame when transferring
 *			  last byte of data in buffer
 * MCD_CRC_RESTART	- to empty out the accumulated checksum prior to
 *			  performing the DMA
 */
#define MCD_BUF_READY		0x80000000
#define MCD_WRAP		0x20000000
#define MCD_INTERRUPT		0x10000000
#define MCD_END_FRAME		0x08000000
#define MCD_CRC_RESTART		0x40000000

/* Defines for the FEC buffer descriptor control/status word*/
#define MCD_FEC_BUF_READY	0x8000
#define MCD_FEC_WRAP		0x2000
#define MCD_FEC_INTERRUPT	0x1000
#define MCD_FEC_END_FRAME	0x0800

/* Defines for general intuitiveness */

#define MCD_TRUE		1
#define MCD_FALSE		0

/* Three different cases for destination and source. */
#define MINUS1			-1
#define ZERO			0
#define PLUS1			1

#ifndef DEFINESONLY

/* Task Table Entry struct*/
typedef struct {
	u32 TDTstart;		/* task descriptor table start */
	u32 TDTend;		/* task descriptor table end */
	u32 varTab;		/* variable table start */
	u32 FDTandFlags;	/* function descriptor table start & flags */
	volatile u32 descAddrAndStatus;
	volatile u32 modifiedVarTab;
	u32 contextSaveSpace;	/* context save space start */
	u32 literalBases;
} TaskTableEntry;

/* Chained buffer descriptor:
 * flags	- flags describing the DMA
 * csumResult	- checksum performed since last checksum reset
 * srcAddr	- the address to move data from
 * destAddr	- the address to move data to
 * lastDestAddr	- the last address written to
 * dmaSize	- the no of bytes to xfer independent of the xfer sz
 * next		- next buffer descriptor in chain
 * info		- private info about this descriptor;  DMA does not affect it
 */
typedef volatile struct MCD_bufDesc_struct MCD_bufDesc;
struct MCD_bufDesc_struct {
	u32 flags;
	u32 csumResult;
	s8 *srcAddr;
	s8 *destAddr;
	s8 *lastDestAddr;
	u32 dmaSize;
	MCD_bufDesc *next;
	u32 info;
};

/* Progress Query struct:
 * lastSrcAddr	- the most-recent or last, post-increment source address
 * lastDestAddr	- the most-recent or last, post-increment destination address
 * dmaSize	- the amount of data transferred for the current buffer
 * currBufDesc	- pointer to the current buffer descriptor being DMAed
 */

typedef volatile struct MCD_XferProg_struct {
	s8 *lastSrcAddr;
	s8 *lastDestAddr;
	u32 dmaSize;
	MCD_bufDesc *currBufDesc;
} MCD_XferProg;

/* FEC buffer descriptor */
typedef volatile struct MCD_bufDescFec_struct {
	u16 statCtrl;
	u16 length;
	u32 dataPointer;
} MCD_bufDescFec;

/*************************************************************************/
/* API function Prototypes  - see MCD_dmaApi.c for further notes */

/* MCD_startDma starts a particular kind of DMA:
 * srcAddr	- the channel on which to run the DMA
 * srcIncr	- the address to move data from, or buffer-descriptor address
 * destAddr	- the amount to increment the source address per transfer
 * destIncr	- the address to move data to
 * dmaSize	- the amount to increment the destination address per transfer
 * xferSize	- the number bytes in of each data movement (1, 2, or 4)
 * initiator	- what device initiates the DMA
 * priority	- priority of the DMA
 * flags	- flags describing the DMA
 * funcDesc	- description of byte swapping, bit swapping, and CRC actions
 */
int MCD_startDma(int channel, s8 * srcAddr, s16 srcIncr, s8 * destAddr,
		 s16 destIncr, u32 dmaSize, u32 xferSize, u32 initiator,
		 int priority, u32 flags, u32 funcDesc);

/*
 * MCD_initDma() initializes the DMA API by setting up a pointer to the DMA
 * registers, relocating and creating the appropriate task structures, and
 * setting up some global settings
 */
int MCD_initDma(dmaRegs * sDmaBarAddr, void *taskTableDest, u32 flags);

/* MCD_dmaStatus() returns the status of the DMA on the requested channel. */
int MCD_dmaStatus(int channel);

/* MCD_XferProgrQuery() returns progress of DMA on requested channel */
int MCD_XferProgrQuery(int channel, MCD_XferProg * progRep);

/*
 * MCD_killDma() halts the DMA on the requested channel, without any
 * intention of resuming the DMA.
 */
int MCD_killDma(int channel);

/*
 * MCD_continDma() continues a DMA which as stopped due to encountering an
 * unready buffer descriptor.
 */
int MCD_continDma(int channel);

/*
 * MCD_pauseDma() pauses the DMA on the given channel ( if any DMA is
 * running on that channel).
 */
int MCD_pauseDma(int channel);

/*
 * MCD_resumeDma() resumes the DMA on a given channel (if any DMA is
 * running on that channel).
 */
int MCD_resumeDma(int channel);

/* MCD_csumQuery provides the checksum/CRC after performing a non-chained DMA */
int MCD_csumQuery(int channel, u32 * csum);

/*
 * MCD_getCodeSize provides the packed size required by the microcoded task
 * and structures.
 */
int MCD_getCodeSize(void);

/*
 * MCD_getVersion provides a pointer to a version string and returns a
 * version number.
 */
int MCD_getVersion(char **longVersion);

/* macro for setting a location in the variable table */
#define MCD_SET_VAR(taskTab,idx,value) ((u32 *)(taskTab)->varTab)[idx] = value
/* Note that MCD_SET_VAR() is invoked many times in firing up a DMA function,
   so I'm avoiding surrounding it with "do {} while(0)" */

#endif				/* DEFINESONLY */

#endif				/* _MCD_API_H */
