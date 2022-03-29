/**
 * @file IxEthDBLearning.c
 * 
 * @par
 * IXP400 SW Release version 2.0
 * 
 * -- Copyright Notice --
 * 
 * @par
 * Copyright 2001-2005, Intel Corporation.
 * All rights reserved.
 * 
 * @par
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * @par
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * @par
 * -- End of Copyright Notice --
 */

#include "IxEthDB_p.h"

/**
 * @brief hashes the mac address in a mac descriptor with a XOR function
 *
 * @param entry pointer to a mac descriptor to be hashed
 * 
 * This function only extracts the mac address and employs ixEthDBKeyXORHash()
 * to do the actual hashing.
 * Used only to add a whole entry to a hash table, as opposed to searching which
 * takes only a key and uses the key hashing directly.
 *
 * @see ixEthDBKeyXORHash()
 *
 * @return the hash value
 *
 * @internal
 */
UINT32 ixEthDBEntryXORHash(void *entry)
{
    MacDescriptor *descriptor = (MacDescriptor *) entry;

    return ixEthDBKeyXORHash(descriptor->macAddress);
}

/**
 * @brief hashes a mac address
 *
 * @param key pointer to a 6 byte structure (typically an IxEthDBMacAddr pointer)
 * to be hashed
 *
 * Given a 6 bytes MAC address, the hash used is:
 *
 * hash(MAC[0:5]) = MAC[0:1] ^ MAC[2:3] ^ MAC[4:5]
 *
 * Used by the hash table to search and remove entries based
 * solely on their keys (mac addresses).
 *
 * @return the hash value
 *
 * @internal
 */
UINT32 ixEthDBKeyXORHash(void *key)
{
    UINT32 hashValue;
    UINT8 *value = (UINT8 *) key;
    
    hashValue  = (value[5] << 8) | value[4];
    hashValue ^= (value[3] << 8) | value[2];
    hashValue ^= (value[1] << 8) | value[0];

    return hashValue;
}

/**
 * @brief mac descriptor match function
 *
 * @param reference mac address (typically an IxEthDBMacAddr pointer) structure
 * @param entry pointer to a mac descriptor whose key (mac address) is to be 
 * matched against the reference key
 *
 * Used by the hash table to retrieve entries. Hashing entries can produce
 * collisions, i.e. descriptors with different mac addresses and the same
 * hash value, where this function is used to differentiate entries.
 *
 * @retval TRUE if the entry matches the reference key (equal addresses)
 * @retval FALSE if the entry does not match the reference key
 *
 * @internal
 */
BOOL ixEthDBAddressMatch(void *reference, void *entry)
{
    return (ixEthDBAddressCompare(reference, ((MacDescriptor *) entry)->macAddress) == 0);
}

/**
 * @brief compares two mac addresses
 *
 * @param mac1 first mac address to compare
 * @param mac2 second mac address to compare
 * 
 * This comparison works in a similar way to strcmp, producing similar results.
 * Used to insert values keyed on mac addresses into binary search trees.
 *
 * @retval -1 if mac1 < mac2
 * @retval 0 if ma1 == mac2
 * @retval 1 if mac1 > mac2
 */
UINT32 ixEthDBAddressCompare(UINT8 *mac1, UINT8 *mac2)
{
    UINT32 local_index;

    for (local_index = 0 ; local_index < IX_IEEE803_MAC_ADDRESS_SIZE ; local_index++)
    {
        if (mac1[local_index] > mac2[local_index])
        {
            return 1;
        }
        else if (mac1[local_index] < mac2[local_index])
        {
            return -1;
        }
    }

    return 0;
}

