/*===========================================================================
FILE:
   QMIDevice.h

DESCRIPTION:
   Functions related to the QMI interface device
   
FUNCTIONS:
   Generic functions
      IsDeviceValid
      PrintHex
      GobiSetDownReason
      GobiClearDownReason
      GobiTestDownReason

   Driver level asynchronous read functions
      ResubmitIntURB
      ReadCallback
      IntCallback
      StartRead
      KillRead

   Internal read/write functions
      ReadAsync
      UpSem
      ReadSync
      WriteSyncCallback
      WriteSync

   Internal memory management functions
      GetClientID
      ReleaseClientID
      FindClientMem
      AddToReadMemList
      PopFromReadMemList
      AddToNotifyList
      NotifyAndPopNotifyList
      AddToURBList
      PopFromURBList

   Internal userspace wrapper functions
      UserspaceunlockedIOCTL

   Userspace wrappers
      UserspaceOpen
      UserspaceIOCTL
      UserspaceClose
      UserspaceRead
      UserspaceWrite
      UserspacePoll

   Initializer and destructor
      RegisterQMIDevice
      DeregisterQMIDevice

   Driver level client management
      QMIReady
      QMIWDSCallback
      SetupQMIWDSCallback
      QMIDMSGetMEID

Copyright (c) 2011, Code Aurora Forum. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Code Aurora Forum nor
      the names of its contributors may be used to endorse or promote
      products derived from this software without specific prior written
      permission.


THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
===========================================================================*/

//---------------------------------------------------------------------------
// Pragmas
//---------------------------------------------------------------------------
#pragma once

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "Structs.h"
#include "QMI.h"

/*=========================================================================*/
// Generic functions
/*=========================================================================*/

// Basic test to see if device memory is valid
bool IsDeviceValid( sGobiUSBNet * pDev );

// Print Hex data, for debug purposes
void PrintHex(
   void *         pBuffer,
   u16            bufSize );

// Sets mDownReason and turns carrier off
void GobiSetDownReason(
   sGobiUSBNet *    pDev,
   u8                 reason );

// Clear mDownReason and may turn carrier on
void GobiClearDownReason(
   sGobiUSBNet *    pDev,
   u8                 reason );

// Tests mDownReason and returns whether reason is set
bool GobiTestDownReason(
   sGobiUSBNet *    pDev,
   u8                 reason );

/*=========================================================================*/
// Driver level asynchronous read functions
/*=========================================================================*/

// Resubmit interrupt URB, re-using same values
int ResubmitIntURB( struct urb * pIntURB );

// Read callback
//    Put the data in storage and notify anyone waiting for data
#if (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,18 ))
void ReadCallback( struct urb * pReadURB );
#else
void ReadCallback(struct urb *pReadURB, struct pt_regs *regs);
#endif

// Inturrupt callback
//    Data is available, start a read URB
#if (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,18 ))
void IntCallback( struct urb * pIntURB );
#else
void IntCallback(struct urb *pIntURB, struct pt_regs *regs);
#endif

// Start continuous read "thread"
int StartRead( sGobiUSBNet * pDev );

// Kill continuous read "thread"
void KillRead( sGobiUSBNet * pDev );

/*=========================================================================*/
// Internal read/write functions
/*=========================================================================*/

// Start asynchronous read
//     Reading client's data store, not device
int ReadAsync(
   sGobiUSBNet *    pDev,
   u16                clientID,
   u16                transactionID,
   void               (*pCallback)(sGobiUSBNet *, u16, void *),
   void *             pData );

// Notification function for synchronous read
void UpSem( 
   sGobiUSBNet *    pDev,
   u16                clientID,
   void *             pData );

// Start synchronous read
//     Reading client's data store, not device
int ReadSync(
   sGobiUSBNet *    pDev,
   void **            ppOutBuffer,
   u16                clientID,
   u16                transactionID );

// Write callback
#if (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,18 ))
void WriteSyncCallback( struct urb * pWriteURB );
#else
void WriteSyncCallback(struct urb *pWriteURB, struct pt_regs *regs);
#endif

// Start synchronous write
int WriteSync(
   sGobiUSBNet *    pDev,
   char *             pInWriteBuffer,
   int                size,
   u16                clientID );

/*=========================================================================*/
// Internal memory management functions
/*=========================================================================*/

// Create client and allocate memory
int GetClientID( 
   sGobiUSBNet *      pDev,
   u8                   serviceType );

// Release client and free memory
void ReleaseClientID(
   sGobiUSBNet *      pDev,
   u16                  clientID );

// Find this client's memory
sClientMemList * FindClientMem(
   sGobiUSBNet *      pDev,
   u16                  clientID );

// Add Data to this client's ReadMem list
bool AddToReadMemList( 
   sGobiUSBNet *      pDev,
   u16                  clientID,
   u16                  transactionID,
   void *               pData,
   u16                  dataSize );

// Remove data from this client's ReadMem list if it matches 
// the specified transaction ID.
bool PopFromReadMemList( 
   sGobiUSBNet *      pDev,
   u16                  clientID,
   u16                  transactionID,
   void **              ppData,
   u16 *                pDataSize );

// Add Notify entry to this client's notify List
bool AddToNotifyList( 
   sGobiUSBNet *      pDev,
   u16                  clientID,
   u16                  transactionID,
   void                 (* pNotifyFunct)(sGobiUSBNet *, u16, void *),
   void *               pData );

// Remove first Notify entry from this client's notify list 
//    and Run function
bool NotifyAndPopNotifyList( 
   sGobiUSBNet *      pDev,
   u16                  clientID,
   u16                  transactionID );

// Add URB to this client's URB list
bool AddToURBList( 
   sGobiUSBNet *      pDev,
   u16                  clientID,
   struct urb *         pURB );

// Remove URB from this client's URB list
struct urb * PopFromURBList( 
   sGobiUSBNet *      pDev,
   u16                  clientID );

/*=========================================================================*/
// Internal userspace wrappers
/*=========================================================================*/

// Userspace unlocked ioctl
long UserspaceunlockedIOCTL(
   struct file *     pFilp,
   unsigned int      cmd,
   unsigned long     arg );

/*=========================================================================*/
// Userspace wrappers
/*=========================================================================*/

// Userspace open
int UserspaceOpen( 
   struct inode *   pInode, 
   struct file *    pFilp );

// Userspace ioctl
int UserspaceIOCTL( 
   struct inode *    pUnusedInode, 
   struct file *     pFilp,
   unsigned int      cmd, 
   unsigned long     arg );

// Userspace close
#if (LINUX_VERSION_CODE > KERNEL_VERSION( 2,6,14 ))
int UserspaceClose( 
   struct file *       pFilp,
   fl_owner_t          unusedFileTable );
#else
int UserspaceClose( struct file *       pFilp );
#endif

// Userspace read (synchronous)
ssize_t UserspaceRead( 
   struct file *        pFilp,
   char __user *        pBuf, 
   size_t               size,
   loff_t *             pUnusedFpos );

// Userspace write (synchronous)
ssize_t UserspaceWrite(
   struct file *        pFilp, 
   const char __user *  pBuf, 
   size_t               size,
   loff_t *             pUnusedFpos );

unsigned int UserspacePoll(
   struct file *                  pFilp,
   struct poll_table_struct *     pPollTable );

/*=========================================================================*/
// Initializer and destructor
/*=========================================================================*/

// QMI Device initialization function
int RegisterQMIDevice( sGobiUSBNet * pDev );

// QMI Device cleanup function
void DeregisterQMIDevice( sGobiUSBNet * pDev );

/*=========================================================================*/
// Driver level client management
/*=========================================================================*/

// Check if QMI is ready for use
bool QMIReady(
   sGobiUSBNet *    pDev,
   u16                timeout );

// QMI WDS callback function
void QMIWDSCallback(
   sGobiUSBNet *    pDev,
   u16                clientID,
   void *             pData );

// Fire off reqests and start async read for QMI WDS callback
int SetupQMIWDSCallback( sGobiUSBNet * pDev );

// Register client, send req and parse MEID response, release client
int QMIDMSGetMEID( sGobiUSBNet * pDev );

// Register client, send req and parse Data format response, release client
int QMIWDASetDataFormat( sGobiUSBNet * pDev );
