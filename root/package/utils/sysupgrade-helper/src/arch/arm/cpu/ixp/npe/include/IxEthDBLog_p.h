/**
 * @file IxEthDBLog_p.h
 *
 * @brief definitions of log macros and log configuration
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

#include <IxOsal.h>

#ifdef IX_UNIT_TEST
#define NULL_PRINT_ROUTINE(format, arg...) /* nothing */
#else
#define NULL_PRINT_ROUTINE if(0) printf
#endif

/***************************************************
 *                 Globals                         *
 ***************************************************/
/* safe to permanently leave these on */
#define HAS_ERROR_LOG
#define HAS_ERROR_IRQ_LOG
#define HAS_WARNING_LOG

/***************************************************
 *              Log Configuration                  *
 ***************************************************/

/* debug output can be turned on unless specifically
   declared as a non-debug build */
#ifndef NDEBUG

#undef HAS_EVENTS_TRACE
#undef HAS_EVENTS_VERBOSE_TRACE

#undef HAS_SUPPORT_TRACE
#undef HAS_SUPPORT_VERBOSE_TRACE

#undef HAS_NPE_TRACE
#undef HAS_NPE_VERBOSE_TRACE
#undef HAS_DUMP_NPE_TREE

#undef HAS_UPDATE_TRACE
#undef HAS_UPDATE_VERBOSE_TRACE

#endif /* NDEBUG */


/***************************************************
 *              Log Macros                         *
 ***************************************************/

/************** Globals ******************/

#ifdef HAS_ERROR_LOG

    #define ERROR_LOG printf

#else

    #define ERROR_LOG NULL_PRINT_ROUTINE

#endif

#ifdef HAS_ERROR_IRQ_LOG

    #define ERROR_IRQ_LOG(format, arg1, arg2, arg3, arg4, arg5, arg6) ixOsalLog(IX_OSAL_LOG_LVL_ERROR, IX_OSAL_LOG_DEV_STDOUT, format, arg1, arg2, arg3, arg4, arg5, arg6)

#else

    #define ERROR_IRQ_LOG(format, arg1, arg2, arg3, arg4, arg5, arg6) /* nothing */

#endif

#ifdef HAS_WARNING_LOG

    #define WARNING_LOG printf

#else

    #define WARNING_LOG NULL_PRINT_ROUTINE

#endif

/************** Events *******************/

#ifdef HAS_EVENTS_TRACE

    #define IX_ETH_DB_EVENTS_TRACE     printf
    #define IX_ETH_DB_IRQ_EVENTS_TRACE(format, arg1, arg2, arg3, arg4, arg5, arg6) ixOsalLog(IX_OSAL_LOG_LVL_MESSAGE, IX_OSAL_LOG_DEV_STDOUT, format, arg1, arg2, arg3, arg4, arg5, arg6)

    #ifdef HAS_EVENTS_VERBOSE_TRACE

        #define IX_ETH_DB_EVENTS_VERBOSE_TRACE printf
    
    #else

        #define IX_ETH_DB_EVENTS_VERBOSE_TRACE NULL_PRINT_ROUTINE

    #endif /* HAS_EVENTS_VERBOSE_TRACE */

#else

    #define IX_ETH_DB_EVENTS_TRACE         NULL_PRINT_ROUTINE
    #define IX_ETH_DB_EVENTS_VERBOSE_TRACE NULL_PRINT_ROUTINE
    #define IX_ETH_DB_IRQ_EVENTS_TRACE(format, arg1, arg2, arg3, arg4, arg5, arg6) /* nothing */

#endif /* HAS_EVENTS_TRACE */

/************** Support *******************/

#ifdef HAS_SUPPORT_TRACE

    #define IX_ETH_DB_SUPPORT_TRACE     printf
    #define IX_ETH_DB_SUPPORT_IRQ_TRACE(format, arg1, arg2, arg3, arg4, arg5, arg6) ixOsalLog(IX_OSAL_LOG_LVL_MESSAGE, IX_OSAL_LOG_DEV_STDOUT, format, arg1, arg2, arg3, arg4, arg5, arg6)

    #ifdef HAS_SUPPORT_VERBOSE_TRACE

        #define IX_ETH_DB_SUPPORT_VERBOSE_TRACE printf

    #else

        #define IX_ETH_DB_SUPPORT_VERBOSE_TRACE NULL_PRINT_ROUTINE

    #endif /* HAS_SUPPORT_VERBOSE_TRACE */

#else

    #define IX_ETH_DB_SUPPORT_TRACE         NULL_PRINT_ROUTINE
    #define IX_ETH_DB_SUPPORT_VERBOSE_TRACE NULL_PRINT_ROUTINE
    #define IX_ETH_DB_SUPPORT_IRQ_TRACE(format, arg1, arg2, arg3, arg4, arg5, arg6) /* nothing */

#endif /* HAS_SUPPORT_TRACE */

/************** NPE Adaptor *******************/

#ifdef HAS_NPE_TRACE

    #define IX_ETH_DB_NPE_TRACE     printf
    #define IX_ETH_DB_NPE_IRQ_TRACE(format, arg1, arg2, arg3, arg4, arg5, arg6) ixOsalLog(IX_OSAL_LOG_LVL_MESSAGE, IX_OSAL_LOG_DEV_STDOUT, format, arg1, arg2, arg3, arg4, arg5, arg6)

    #ifdef HAS_NPE_VERBOSE_TRACE

        #define IX_ETH_DB_NPE_VERBOSE_TRACE printf

    #else

        #define IX_ETH_DB_NPE_VERBOSE_TRACE NULL_PRINT_ROUTINE

    #endif /* HAS_NPE_VERBOSE_TRACE */

#else

    #define IX_ETH_DB_NPE_TRACE         NULL_PRINT_ROUTINE
    #define IX_ETH_DB_NPE_VERBOSE_TRACE NULL_PRINT_ROUTINE
    #define IX_ETH_DB_NPE_IRQ_TRACE(format, arg1, arg2, arg3, arg4, arg5, arg6) /* nothing */

#endif /* HAS_NPE_TRACE */

#ifdef HAS_DUMP_NPE_TREE

#define IX_ETH_DB_NPE_DUMP_ELT(eltBaseAddress, eltSize) ixEthELTDumpTree(eltBaseAddress, eltSize)

#else

#define IX_ETH_DB_NPE_DUMP_ELT(eltBaseAddress, eltSize) /* nothing */

#endif /* HAS_DUMP_NPE_TREE */

/************** Port Update *******************/

#ifdef HAS_UPDATE_TRACE

    #define IX_ETH_DB_UPDATE_TRACE printf

    #ifdef HAS_UPDATE_VERBOSE_TRACE

        #define IX_ETH_DB_UPDATE_VERBOSE_TRACE printf

    #else

        #define IX_ETH_DB_UPDATE_VERBOSE_TRACE NULL_PRINT_ROUTINE

    #endif

#else /* HAS_UPDATE_VERBOSE_TRACE */

    #define IX_ETH_DB_UPDATE_TRACE         NULL_PRINT_ROUTINE
    #define IX_ETH_DB_UPDATE_VERBOSE_TRACE NULL_PRINT_ROUTINE

#endif /* HAS_UPDATE_TRACE */
