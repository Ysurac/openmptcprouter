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

#ifndef MCD_TSK_INIT_H
#define MCD_TSK_INIT_H 1

/*
 * Do not edit!
 */

/* Task 0 */
void MCD_startDmaChainNoEu(int *currBD, short srcIncr, short destIncr,
			   int xferSize, short xferSizeIncr, int *cSave,
			   volatile TaskTableEntry * taskTable, int channel);

/* Task 1 */
void MCD_startDmaSingleNoEu(char *srcAddr, short srcIncr, char *destAddr,
			    short destIncr, int dmaSize, short xferSizeIncr,
			    int flags, int *currBD, int *cSave,
			    volatile TaskTableEntry * taskTable, int channel);

/* Task 2 */
void MCD_startDmaChainEu(int *currBD, short srcIncr, short destIncr,
			 int xferSize, short xferSizeIncr, int *cSave,
			 volatile TaskTableEntry * taskTable, int channel);

/* Task 3 */
void MCD_startDmaSingleEu(char *srcAddr, short srcIncr, char *destAddr,
			  short destIncr, int dmaSize, short xferSizeIncr,
			  int flags, int *currBD, int *cSave,
			  volatile TaskTableEntry * taskTable, int channel);

/* Task 4 */
void MCD_startDmaENetRcv(char *bDBase, char *currBD, char *rcvFifoPtr,
			 volatile TaskTableEntry * taskTable, int channel);

/* Task 5 */
void MCD_startDmaENetXmit(char *bDBase, char *currBD, char *xmitFifoPtr,
			  volatile TaskTableEntry * taskTable, int channel);

#endif				/* MCD_TSK_INIT_H */
