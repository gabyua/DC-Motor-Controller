/***********************************************************************************************************************
 * Copyright [2015-2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * This file is part of Renesas SynergyTM Software Package (SSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas SSP license agreement. Unless otherwise agreed in an SSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : sf_tes_2d_drw_base.h
 * Description  : D/AVE D1 low-level driver function header file.
 **********************************************************************************************************************/

#ifndef SF_TES_2D_DRW_BASE_H
#define SF_TES_2D_DRW_BASE_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"
#include "dave_base.h"
#include "tx_api.h"

/** Common macro for SSP header files. There is also a corresponding SSP_FOOTER macro at the end of this file. */
SSP_HEADER

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/** The mode to support lists of display list addresses is disabled and not exposed the feature to users. */
#define SF_TES_2D_DRW_USE_DLIST_INDIRECT  	(0)

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
typedef long            d1_long_t;
typedef int             d1_int_t;
typedef unsigned int    d1_uint_t;

/** Device handle type definition for Synergy implementation. */
typedef struct _d1_device_s7
{
    volatile uint32_t * dlist_start;        /* Display list start address */
    int32_t 			dlist_indirect;     /* Set to 1 when supporting lists of dlist addresses */
    TX_SEMAPHORE        dlist_semaphore;    /* Semaphore used for synchronizing to the display list interrupt timing */
    R_G2D_Type        * dave_reg;           /* Used to access D/AVE 2D hardware registers */

} d1_device_synergy;

d1_int_t d1_initirq_intern( d1_device_synergy *handle );
d1_int_t d1_shutdownirq_intern( d1_device_synergy *handle );

/** Common macro for SSP header files. There is also a corresponding SSP_HEADER macro at the top of this file. */
SSP_FOOTER

#endif
