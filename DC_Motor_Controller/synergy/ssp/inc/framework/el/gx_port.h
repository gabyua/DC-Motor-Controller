/**************************************************************************/
/*                                                                        */
/*            Copyright (c) 1996-2017 by Express Logic Inc.               */
/*                                                                        */
/*  This software is copyrighted by and is the sole property of Express   */
/*  Logic, Inc.  All rights, title, ownership, or other interests         */
/*  in the software remain the property of Express Logic, Inc.  This      */
/*  software may only be used in accordance with the corresponding        */
/*  license agreement.  Any unauthorized use, duplication, transmission,  */
/*  distribution, or disclosure of this software is expressly forbidden.  */
/*                                                                        */
/*  This Copyright notice may not be removed or modified without prior    */
/*  written consent of Express Logic, Inc.                                */
/*                                                                        */
/*  Express Logic, Inc. reserves the right to modify this software        */
/*  without notice.                                                       */
/*                                                                        */
/*  Express Logic, Inc.                     info@expresslogic.com         */
/*  11423 West Bernardo Court               http://www.expresslogic.com   */
/*  San Diego, CA  92127                                                  */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** GUIX Component                                                        */
/**                                                                       */
/**   Port Specific                                                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/*                                                                        */
/*  APPLICATION INTERFACE DEFINITION                       RELEASE        */
/*                                                                        */
/*    gx_port.h                                            Synergy        */
/*                                                           5.4          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Express Logic, Inc.                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file contains data type definitions and constants specific     */
/*    to the implementation of high-performance GUIX UI framework.        */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-25-2015     William E. Lamie         Initial Version 5.0           */
/*  11-23-2015     William E. Lamie         Modified comment(s), use      */
/*                                            TX_THREAD_GET_SYSTEM_STATE  */
/*                                            to detect interrupt context */
/*                                            resulting in version 5.1    */
/*  02-22-2016     William E. Lamie         Modified comment(s),          */
/*                                            resulting in version 5.3    */
/*  06-07-2017     William E. Lamie         Modified comment(s), changed  */
/*                                            GX_TICKS_SECOND value,      */
/*                                            resulting in version 5.4    */
/*                                                                        */
/**************************************************************************/

#ifndef GX_PORT_H
#define GX_PORT_H

/* Determine if the optional GUIX user define file should be used.  */

#define GX_INCLUDE_USER_DEFINE_FILE
#ifdef GX_INCLUDE_USER_DEFINE_FILE

/* Yes, include the user defines in gx_user.h. The defines in this file may
   alternately be defined on the command line.  */

#include "gx_user.h"
#endif

typedef INT    GX_BOOL;
typedef signed   char GX_CHAR;
typedef unsigned char GX_UCHAR;
typedef SHORT  GX_VALUE;

#define GX_VALUE_MAX                        0x7FFF


/* Define the basic system parameters.  */

#ifndef GX_THREAD_STACK_SIZE
#define GX_THREAD_STACK_SIZE                4096
#endif

#ifndef GX_TICKS_SECOND
#define GX_TICKS_SECOND                     50
#endif


#define GX_CONST                            const

#define GX_COLOR_FORMAT                     GX_COLOR_FORMAT_24BIT_RGB


#define GX_INCLUDE_DEFAULT_COLORS

#define GX_MAX_ACTIVE_TIMERS                32

#define GX_MAX_VIEWS                        32
#define GX_MAX_DISPLAY_HEIGHT               800

#define GX_CUSTOM_FONTS                     16
#define GX_CUSTOM_PIXELMAPS                 32

/* Define several macros for the error checking shell in GUIX.  */

#ifndef TX_TIMER_PROCESS_IN_ISR

#define GX_CALLER_CHECKING_EXTERNS          extern  TX_THREAD           *_tx_thread_current_ptr; \
                                            extern  TX_THREAD           _tx_timer_thread; \
                                            extern  volatile ULONG      _tx_thread_system_state;

#define GX_THREADS_ONLY_CALLER_CHECKING     if ((TX_THREAD_GET_SYSTEM_STATE()) || \
                                                (_tx_thread_current_ptr == TX_NULL) || \
                                                (_tx_thread_current_ptr == &_tx_timer_thread)) \
                                                return(GX_CALLER_ERROR);

#define GX_INIT_AND_THREADS_CALLER_CHECKING if (((TX_THREAD_GET_SYSTEM_STATE()) && (TX_THREAD_GET_SYSTEM_STATE() < ((ULONG) 0xF0F0F0F0))) || \
                                                (_tx_thread_current_ptr == &_tx_timer_thread)) \
                                                return(GX_CALLER_ERROR);


#define GX_NOT_ISR_CALLER_CHECKING          if ((TX_THREAD_GET_SYSTEM_STATE()) && (TX_THREAD_GET_SYSTEM_STATE() < ((ULONG) 0xF0F0F0F0))) \
                                                return(GX_CALLER_ERROR);

#define GX_THREAD_WAIT_CALLER_CHECKING      if ((wait_option) && \
                                                ((_tx_thread_current_ptr == NX_NULL) || (TX_THREAD_GET_SYSTEM_STATE()) || (_tx_thread_current_ptr == &_tx_timer_thread))) \
                                            return(GX_CALLER_ERROR);


#else



#define GX_CALLER_CHECKING_EXTERNS          extern  TX_THREAD           *_tx_thread_current_ptr; \
                                            extern  volatile ULONG      _tx_thread_system_state;

#define GX_THREADS_ONLY_CALLER_CHECKING     if ((TX_THREAD_GET_SYSTEM_STATE()) || \
                                                (_tx_thread_current_ptr == TX_NULL)) \
                                                return(GX_CALLER_ERROR);

#define GX_INIT_AND_THREADS_CALLER_CHECKING if (((TX_THREAD_GET_SYSTEM_STATE()) && (TX_THREAD_GET_SYSTEM_STATE() < ((ULONG) 0xF0F0F0F0)))) \
                                                return(GX_CALLER_ERROR);

#define GX_NOT_ISR_CALLER_CHECKING          if ((TX_THREAD_GET_SYSTEM_STATE()) && (TX_THREAD_GET_SYSTEM_STATE() < ((ULONG) 0xF0F0F0F0))) \
                                                return(GX_CALLER_ERROR);

#define GX_THREAD_WAIT_CALLER_CHECKING      if ((wait_option) && \
                                                ((_tx_thread_current_ptr == NX_NULL) || (TX_THREAD_GET_SYSTEM_STATE()))) \
                                            return(GX_CALLER_ERROR);

#endif


/* Define the version ID of GUIX.  This may be utilized by the application.  */

#ifdef GX_SYSTEM_INIT
CHAR _gx_version_id[] =
    "Copyright (c) 1996-2017 Express Logic Inc. * GUIX Synergy Version G5.4.5.4 SN: 4154-280-5000 *";
#else
extern  CHAR _nx_version_id[];
#endif

#endif

