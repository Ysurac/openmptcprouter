
/* * Copyright (c) 2012 The Linux Foundation. All rights reserved.* */

/**\file
 *  This file serves as the wrapper for the platform/OS dependent functions
 *  It is needed to modify these functions accordingly based on the platform and the
 *  OS. Whenever the synopsys GMAC driver ported on to different platform, this file
 *  should be handled at most care.
 *  The corresponding function definitions for non-inline functions are available in
 *  synopGMAC_plat.c file.
 * \internal
 * -------------------------------------REVISION HISTORY---------------------------
 * Ubicom		01/Mar/2010			Modified for Ubicom32
 * Synopsys		01/Aug/2007			Created
 */

#ifndef SYNOP_GMAC_PLAT_H
#define SYNOP_GMAC_PLAT_H 1

#include <common.h>
#include <net.h>
#include <asm-generic/errno.h>
#include "uboot_skb.h"
#include <asm/io.h>

#define CACHE_LINE_SHIFT        5
#define CACHE_LINE_SIZE         (1 << CACHE_LINE_SHIFT) /* in bytes */

#define TR0(fmt, args...) printf("SynopGMAC: " fmt, ##args)
# define TR(fmt, args...) /* not debugging: nothing */

typedef int bool;

#define	virt_to_phys(x) ((unsigned long)(x))

#ifdef __GNUC__
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#else
#define likely(x)	(x)
#define unlikely(x)	(x)
#endif

#define NETDEV_TX_OK 0
#define NETDEV_TX_BUSY 1	/* driver tx path was busy*/

#define DEFAULT_DELAY_VARIABLE  10
#define DEFAULT_LOOP_VARIABLE   10/*000*/

/* There are platform related endian conversions
 *
 */

/* Error Codes */
#define ESYNOPGMACNOERR   0
#define ESYNOPGMACNOMEM   ENOMEM
#define ESYNOPGMACPHYERR  EIO

static void __inline__ *plat_alloc_consistent_dmaable_memory(void *dev, size_t size, dma_addr_t *dma_addr)
{
	void *buf = memalign(CACHE_LINE_SIZE, size);
	*dma_addr = (dma_addr_t)(virt_to_phys(buf));
	return buf;
}

/**
 * The Low level function to read register contents from Hardware.
 *
 * @param[in] pointer to the base of register map
 * @param[in] Offset from the base
 * \return  Returns the register contents
 */
static u32 __inline__ synopGMACReadReg(u32 *RegBase, u32 RegOffset)
{
	u32 addr = (u32)RegBase + RegOffset;
	u32 data;

        data = readl(addr);
	TR("%s RegBase = 0x%08x RegOffset = 0x%08x RegData = 0x%08x\n", __FUNCTION__, (u32)RegBase, RegOffset, data );
	return data;
}

/**
 * The Low level function to write to a register in Hardware.
 *
 * @param[in] pointer to the base of register map
 * @param[in] Offset from the base
 * @param[in] Data to be written
 * \return  void
 */
static void  __inline__ synopGMACWriteReg(u32 *RegBase, u32 RegOffset, u32 RegData)
{
	u32 addr = (u32)RegBase + RegOffset;

        writel(RegData, addr);
	TR("%s RegBase = 0x%08x RegOffset = 0x%08x RegData = 0x%08x\n", __FUNCTION__,(u32) RegBase, RegOffset, RegData );
}

/**
 * The Low level function to set bits of a register in Hardware.
 *
 * @param[in] pointer to the base of register map
 * @param[in] Offset from the base
 * @param[in] Bit mask to set bits to logical 1
 * \return  void
 */
static void __inline__ synopGMACSetBits(u32 *RegBase, u32 RegOffset, u32 BitPos)
{
	u32 addr = (u32)RegBase + RegOffset;

        setbits_le32(addr,BitPos);
	TR("%s !!!!!!!!!!!!! RegOffset = 0x%08x RegData = 0x%08x (|  0x%08x)\n", __FUNCTION__, RegOffset, data, BitPos);
}

/**
 * The Low level function to clear bits of a register in Hardware.
 *
 * @param[in] pointer to the base of register map
 * @param[in] Offset from the base
 * @param[in] Bit mask to clear bits to logical 0
 * \return  void
 */
static void __inline__ synopGMACClearBits(u32 *RegBase, u32 RegOffset, u32 BitPos)
{
	u32 addr = (u32)RegBase + RegOffset;

	clrbits_le32( addr, BitPos);
	TR("%s !!!!!!!!!!! RegOffset = 0x%08x RegData = 0x%08x (& ~0x%08x)\n", __FUNCTION__, RegOffset, data, BitPos);
}

/**
 * The Low level function to Check the setting of the bits.
 *
 * @param[in] pointer to the base of register map
 * @param[in] Offset from the base
 * @param[in] Bit mask to set bits to logical 1
 * \return  returns TRUE if set to '1' returns FALSE if set to '0'. Result undefined there are no bit set in the BitPos argument.
 *
 */
static bool __inline__ synopGMACCheckBits(u32 *RegBase, u32 RegOffset, u32 BitPos)
{
	u32 addr = (u32)RegBase + RegOffset;
	u32 data;

        data = readl(addr) & BitPos ;
	return (data != 0);
}

#endif
