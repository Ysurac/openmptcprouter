/**
 * \file u-boot-sha1.h
 * based from http://xyssl.org/code/source/sha1/
 *  FIPS-180-1 compliant SHA-1 implementation
 *
 *  Copyright (C) 2003-2006  Christophe Devine
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License, version 2.1 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA	02110-1301  USA
 */
/*
 *  The SHA-1 standard was published by NIST in 1993.
 *
 *  http://www.itl.nist.gov/fipspubs/fip180-1.htm
 */
#ifndef _SHA1_H
#define _SHA1_H

#ifdef __cplusplus
extern "C" {
#endif

#define SHA1_SUM_POS	-0x20
#define SHA1_SUM_LEN	20

/**
 * \brief	   SHA-1 context structure
 */
typedef struct
{
    unsigned long total[2];	/*!< number of bytes processed	*/
    unsigned long state[5];	/*!< intermediate digest state	*/
    unsigned char buffer[64];	/*!< data block being processed */
}
sha1_context;

/**
 * \brief	   SHA-1 context setup
 *
 * \param ctx	   SHA-1 context to be initialized
 */
void sha1_starts( sha1_context *ctx );

/**
 * \brief	   SHA-1 process buffer
 *
 * \param ctx	   SHA-1 context
 * \param input    buffer holding the  data
 * \param ilen	   length of the input data
 */
void sha1_update( sha1_context *ctx, unsigned char *input, int ilen );

/**
 * \brief	   SHA-1 final digest
 *
 * \param ctx	   SHA-1 context
 * \param output   SHA-1 checksum result
 */
void sha1_finish( sha1_context *ctx, unsigned char output[20] );

/**
 * \brief	   Output = SHA-1( input buffer )
 *
 * \param input    buffer holding the  data
 * \param ilen	   length of the input data
 * \param output   SHA-1 checksum result
 */
void sha1_csum( unsigned char *input, int ilen,
		unsigned char output[20] );

/**
 * \brief	   Output = SHA-1( input buffer ), with watchdog triggering
 *
 * \param input    buffer holding the  data
 * \param ilen	   length of the input data
 * \param output   SHA-1 checksum result
 * \param chunk_sz watchdog triggering period (in bytes of input processed)
 */
void sha1_csum_wd (unsigned char *input, int ilen,
		unsigned char output[20], unsigned int chunk_sz);

/**
 * \brief	   Output = SHA-1( file contents )
 *
 * \param path	   input file name
 * \param output   SHA-1 checksum result
 * \return	   0 if successful, or 1 if fopen failed
 */
int sha1_file( char *path, unsigned char output[20] );

/**
 * \brief	   Output = HMAC-SHA-1( input buffer, hmac key )
 *
 * \param key	   HMAC secret key
 * \param keylen   length of the HMAC key
 * \param input    buffer holding the  data
 * \param ilen	   length of the input data
 * \param output   HMAC-SHA-1 result
 */
void sha1_hmac( unsigned char *key, int keylen,
		unsigned char *input, int ilen,
		unsigned char output[20] );

/**
 * \brief	   Checkup routine
 *
 * \return	   0 if successful, or 1 if the test failed
 */
int sha1_self_test( void );

#ifdef __cplusplus
}
#endif

#endif /* u-boot-sha1.h */
