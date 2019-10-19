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
* File Name    : sf_el_gx.c
* Description  : The definition of SSP GUIX adaptation framework functions
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Copyright [2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
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

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "sf_el_gx.h"
#include "sf_el_gx_cfg.h"

#include "sf_el_gx_private.h"
#include "sf_el_gx_private_api.h"

#if GX_USE_SYNERGY_DRW
#include "dave_driver.h"
#endif


/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/** SSP Error macro */
#ifndef SF_EL_GX_ERROR_RETURN
/*LDRA_INSPECTED 77 S This macro does not work when surrounded by parentheses. */
#define SF_EL_GX_ERROR_RETURN(a, err) SSP_ERROR_RETURN((a), (ssp_err_t)(err), &g_module_name[0], &module_version)
#endif

/** Error macro for user callback to inform error in SSP modules */
/*LDRA_INSPECTED 77 S This macro does not work when surrounded by parentheses. */
#define SF_EL_GX_SSP_USER_ERROR_CALLBACK(p_ctrl, dev, ssp_err) \
        if((p_ctrl)->p_callback){                              \
            if(SSP_SUCCESS != (ssp_err)){                      \
                cb_args.device = (dev);                        \
                cb_args.event  = SF_EL_GX_EVENT_ERROR;         \
                cb_args.error  = (ssp_err);                    \
                (p_ctrl)->p_callback(&cb_args);                \
            }                                                  \
        }

/** Mutex timeout count */
#define SF_EL_GX_MUTEX_WAIT_TIMER (300)

/** Display device timeout count */
#define SF_EL_GX_DISPLAY_HW_WAIT_COUNT_MAX (5000)

/** A macro to mean GUIX color format invalid. */
#ifndef GX_COLOR_FORMAT_INVALID
#define GX_COLOR_FORMAT_INVALID     (0)
#endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
#if (SF_EL_GX_CFG_PARAM_CHECKING_ENABLE)
static ssp_err_t sf_el_gx_open_param_check (sf_el_gx_instance_ctrl_t * const p_ctrl, sf_el_gx_cfg_t const * const p_cfg);

static ssp_err_t sf_el_gx_open_param_check_rotation_angle (sf_el_gx_cfg_t const * const p_cfg);

static ssp_err_t sf_el_gx_open_param_check_canvas_setting (sf_el_gx_cfg_t const * const p_cfg);
#endif

static GX_VALUE  sf_el_gx_setup_display_color_format_set (display_in_format_t format);

static ssp_err_t sf_el_gx_driver_setup (GX_DISPLAY * p_display, sf_el_gx_instance_ctrl_t * const p_ctrl);

static ssp_err_t sf_el_gx_display_open (sf_el_gx_instance_ctrl_t * const p_ctrl);

static ssp_err_t sf_el_gx_display_close (sf_el_gx_instance_ctrl_t * const p_ctrl);

static void      sf_el_gx_canvas_clear (GX_DISPLAY * p_display, sf_el_gx_instance_ctrl_t * const p_ctrl);

static void      sf_el_gx_callback (display_callback_args_t * p_args);

#if GX_USE_SYNERGY_DRW
static d2_s32    sf_el_gx_d2_open_format_set (GX_DISPLAY * p_display);

static ssp_err_t sf_el_gx_d2_open (GX_DISPLAY * p_display, sf_el_gx_instance_ctrl_t * const p_ctrl);

static void      sf_el_gx_d2_open_error_callback (sf_el_gx_instance_ctrl_t * const p_ctrl, d2_s32 d2_err);

static ssp_err_t sf_el_gx_d2_close (sf_el_gx_instance_ctrl_t * const p_ctrl);
#endif

/* GUIX common callback functions setup function */
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
extern UINT _gx_synergy_display_driver_setup(GX_DISPLAY *display);

/* GUIX Synergy Port callback functions */
void   sf_el_gx_frame_pointers_get (ULONG display_handle, GX_UBYTE ** pp_visible, GX_UBYTE ** pp_working);

void   sf_el_gx_frame_toggle (ULONG display_handle, GX_BYTE ** pp_visible_frame);

void * sf_el_gx_jpeg_buffer_get (ULONG display_handle, INT *p_memory_size);

void * sf_el_gx_jpeg_instance_get (ULONG display_handle);

INT    sf_el_gx_display_rotation_get (ULONG handle);

void   sf_el_gx_display_actual_size_get (ULONG display_handle, INT * p_width, INT * p_height);

void   sf_el_gx_display_8bit_palette_assign (ULONG display_handle);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
/** Name of module used by error logger macro */
#if BSP_CFG_ERROR_LOG != 0
static const char g_module_name[] = "sf_el_gx";
#endif
static sf_el_gx_instance_ctrl_t * gp_temp_context = NULL;    ///< Temporary pointer to the GUIX driver control block.

static TX_MUTEX g_sf_el_gx_mutex = { 0 };                    ///< Driver global mutex to protect the context.

/***********************************************************************************************************************
Functions
***********************************************************************************************************************/

#if defined(__GNUC__)
/* This structure is affected by warnings from the GCC compiler bug gcc.gnu.org/bugzilla/show_bug.cgi?id=60784
 * This pragma suppresses the warnings in this structure only, and will be removed when the SSP compiler is updated to
 * v5.3.*/
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
/** SF_EL_GX Framework version data structure */
static const ssp_version_t module_version =
{
    .api_version_minor  = SF_EL_GX_API_VERSION_MINOR,
    .api_version_major  = SF_EL_GX_API_VERSION_MAJOR,
    .code_version_major = SF_EL_GX_CODE_VERSION_MAJOR,
    .code_version_minor = SF_EL_GX_CODE_VERSION_MINOR
};
#if defined(__GNUC__)
/* Restore warning settings for 'missing-field-initializers' to as specified on command line. */
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic pop
#endif

/***********************************************************************************************************************
* Implementation of SF_EL_GX Interface
- **********************************************************************************************************************/
/*LDRA_INSPECTED 27 D This structure must be accessible in user code. It cannot be static. */
const sf_el_gx_api_t sf_el_gx_on_guix =
{
    .open        = SF_EL_GX_Open,
    .close       = SF_EL_GX_Close,
    .versionGet  = SF_EL_GX_VersionGet,
    .setup       = SF_EL_GX_Setup,
    .canvasInit  = SF_EL_GX_CanvasInit
};

/*******************************************************************************************************************//**
 * @addtogroup SF_EL_GX
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework driver for Synergy, open function to configure the framework module. The function
 * initialize RTOS resources used by the module, initialize the control block based on user configuration, and transition
 * the module state to SF_EL_GX_OPENED.
 * This function calls following functions:
 * - sf_el_gx_open_param_check()    Check configuration parameters if parameter check is enabled.
 * - tx_mutex_create()              Creates the mutex for lock the driver during the context update.
 * - tx_mutex_delete()              Deletes the mutex if kernel service calls failed in the process.
 * - tx_semaphore_create()          Creates the semaphore for rendering and displaying synchronization.
 * @retval  SSP_SUCCESS                 Opened the module successfully.
 * @retval  SSP_ERR_ASSERTION           NULL pointer error happened.
 * @retval  SSP_ERR_IN_USE              SF_EL_GX is in-use.
 * @retval  SSP_ERR_INTERNAL            Error happened in kernel service calls.
 * @retval  SSP_ERR_INVALID_ARGUMENT    An invalid argument was passed to the driver.
 * @note This function does not initialize the display or rendering hardware but setup function will do.
 * @note This function registers a user callback function but it is optional. Set NULL to p_cfg::p_callback if user
 *       callback is not required.
 * @note The configuration for the frame buffer B (p_cfg::p_framebuffer_b) is optional. Set NULL to p_framebuffer_b
 *       in case of a single-buffered system.
 **********************************************************************************************************************/
ssp_err_t SF_EL_GX_Open(sf_el_gx_ctrl_t * const p_api_ctrl, sf_el_gx_cfg_t const * const p_cfg)
{
    sf_el_gx_instance_ctrl_t * p_ctrl = (sf_el_gx_instance_ctrl_t *) p_api_ctrl;
    UINT        status;

#if (SF_EL_GX_CFG_PARAM_CHECKING_ENABLE)
    ssp_err_t error = sf_el_gx_open_param_check (p_ctrl, p_cfg);
    if (SSP_SUCCESS != error)
    {
        return error;
    }
#endif

    SF_EL_GX_ERROR_RETURN((SF_EL_GX_CLOSED == p_ctrl->state), SSP_ERR_IN_USE);

    if (SF_EL_GX_OPENED != p_ctrl->state)
    {
        /** Creates global mutex for SF_EL_GX */
        status = tx_mutex_create (&g_sf_el_gx_mutex, (CHAR *)"sf_gx_drv_mtx", TX_INHERIT);
        if ((UINT)TX_SUCCESS != status)
        {
            return SSP_ERR_INTERNAL;
        }
    }

    /** Locks the SF_EL_GX instance until driver setup is done by SF_EL_GX_Setup(). */
    status = tx_mutex_get (&g_sf_el_gx_mutex, SF_EL_GX_MUTEX_WAIT_TIMER);
    if ((UINT)TX_SUCCESS != status)
    {
        tx_mutex_delete (&g_sf_el_gx_mutex);
        return SSP_ERR_INTERNAL;
    }

    /** Creates a semaphore for frame buffer flip */
    status = tx_semaphore_create((TX_SEMAPHORE *) (&p_ctrl->semaphore), (CHAR *)"sf_gx_drv_sem", 0);
    if ((UINT)TX_SUCCESS != status)
    {
        tx_mutex_delete (&g_sf_el_gx_mutex);
        return SSP_ERR_INTERNAL;
    }

    /** Initializes the SF_EL_GX control block */
    p_ctrl->p_display_instance    = p_cfg->p_display_instance;
    p_ctrl->p_callback            = p_cfg->p_callback;
    p_ctrl->p_display_runtime_cfg = p_cfg->p_display_runtime_cfg;
    p_ctrl->p_canvas              = p_cfg->p_canvas;
    p_ctrl->p_framebuffer_read    = p_cfg->p_framebuffer_a;
    p_ctrl->dave2d_buffer_cache_enabled    = p_cfg->dave2d_buffer_cache_enabled;
    if(NULL != p_cfg->p_framebuffer_b)
    {
        p_ctrl->p_framebuffer_write = p_cfg->p_framebuffer_b;
    }
    else
    {   /* If frame buffer B is NULL, specify frame buffer A instead. */
        p_ctrl->p_framebuffer_write = p_cfg->p_framebuffer_a;
    }
    p_ctrl->p_jpegbuffer          = p_cfg->p_jpegbuffer;
    p_ctrl->jpegbuffer_size       = p_cfg->jpegbuffer_size;
    p_ctrl->rendering_enable      = false;
    p_ctrl->rotation_angle        = p_cfg->rotation_angle;
    p_ctrl->p_sf_jpeg_decode_instance = p_cfg->p_sf_jpeg_decode_instance;

    /** Saves the control block to the global pointer inside the module temporarily. Stored data will be used in
     *  sf_el_gx_driver_setup() which will be invoked by GUIX. This pointer will be valid at last but be protected
     *  until SF_EL_GX_Setup() is done.
     */
    gp_temp_context = p_ctrl;

    /** Changes the driver state */
    p_ctrl->state = SF_EL_GX_OPENED;

    return SSP_SUCCESS;
}  /* End of function SF_EL_GX_Open() */

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy, Close function.
 * This function calls following functions:
 * - tx_mutex_get()                Gets the mutex to lock the driver while device access.
 * - tx_mutex_put()                Puts the mutex to unlock the driver while device access.
 * - tx_mutex_delete()             Deletes the mutex if kernel service calls failed in the process.
 * - tx_semaphore_delete()         Deletes the semaphore for rendering and displaying synchronization.
 * - sf_el_gx_d2_close()           Finalizes 2D Drawing Engine hardware.
 * - sf_el_gx_display_close()      Finalizes display hardware.
 * @retval  SSP_SUCCESS               Closed the module successfully.
 * @retval  SSP_ERR_ASSERTION         NULL pointer error happens.
 * @retval  SSP_ERR_NOT_OPEN          SF_EL_GX is not opened.
 * @retval  SSP_ERR_INTERNAL          Error happened in kernel service calls.
 * @retval  SSP_ERR_TIMEOUT           Error occurred in display driver.
 * @retval  SSP_ERR_D2D_ERROR_DEINIT  Error occurred in D/AVE 2D driver.
 * @note    This function is re-entrant.
 **********************************************************************************************************************/
ssp_err_t SF_EL_GX_Close(sf_el_gx_ctrl_t * const p_api_ctrl)
{
    sf_el_gx_instance_ctrl_t * p_ctrl = (sf_el_gx_instance_ctrl_t *) p_api_ctrl;
    UINT      status;
    ssp_err_t error;

#if (SF_EL_GX_CFG_PARAM_CHECKING_ENABLE)
    SSP_ASSERT(p_ctrl);
#endif
    SF_EL_GX_ERROR_RETURN((SF_EL_GX_CLOSED != p_ctrl->state), SSP_ERR_NOT_OPEN);

    /** Locks the driver to update the context. */
    status = tx_mutex_get (&g_sf_el_gx_mutex, SF_EL_GX_MUTEX_WAIT_TIMER);
    if((UINT)TX_SUCCESS != status)
    {
        return SSP_ERR_INTERNAL;
    }

    if (SF_EL_GX_CONFIGURED == p_ctrl->state)
    {
#if GX_USE_SYNERGY_DRW
        /** Finalizes 2D Drawing Engine hardware */
        error = sf_el_gx_d2_close(p_ctrl);
        if (SSP_SUCCESS != error)
        {
            tx_mutex_put (&g_sf_el_gx_mutex);
            return error;
        }
#endif
        /** Finalizes display hardware */
        error = sf_el_gx_display_close(p_ctrl);
        if (SSP_SUCCESS != error)
        {
            tx_mutex_put (&g_sf_el_gx_mutex);
            return error;
        }
    }

    /** Changes the driver state */
    p_ctrl->state = SF_EL_GX_CLOSED;

    /** Deletes a semaphore for frame buffer flip */
    status = tx_semaphore_delete(&p_ctrl->semaphore);
    if((UINT)TX_SUCCESS != status)
    {
        return SSP_ERR_INTERNAL;
    }

    /** Unlocks the SF_EL_GX instance */
    status = tx_mutex_put (&g_sf_el_gx_mutex);
    if((UINT)TX_SUCCESS != status)
    {
        return SSP_ERR_INTERNAL;
    }

    /** Deletes driver global mutex */
    status = tx_mutex_delete (&g_sf_el_gx_mutex);
    if((UINT)TX_SUCCESS != status)
    {
        return SSP_ERR_INTERNAL;
    }

    /** Clears the temporary storage for the pointer to a control block.
     *  This procedure has done in SF_EL_GX_Setup() in the expected function call sequence,
     *  but clear it here as well in case SF_EL_GX_Setup() being not called. */
    gp_temp_context = NULL;

    return SSP_SUCCESS;
}  /* End of function SF_EL_GX_Close() */

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy, Version get function
 * @param[in,out] p_version  The version number.
 * @retval  SSP_SUCCESS          Successfully returned the module version.
 * @retval  SSP_ERR_ASSERTION    NULL pointer is passed to function.
 * @note    This function is re-entrant.
 **********************************************************************************************************************/
ssp_err_t SF_EL_GX_VersionGet (ssp_version_t * p_version)
{
    /* Returns if GUIX display driver context is NULL. */
    SF_EL_GX_ERROR_RETURN(NULL != p_version, SSP_ERR_ASSERTION);

    *p_version = module_version;
    return SSP_SUCCESS;
}  /* End of function SF_EL_GX_VersionGet() */

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy, Setup GUIX low level device drivers for Display and D/AVE 2D interface.
 * This function has to be passed to the GUIX Studio display driver setup function gx_studio_display_configure() to let
 * GUIX call this function and configure the GUIX low level device driver(s).
 * This function calls following functions:
 * - tx_mutex_put()             Puts the driver global mutex when the low level driver setup is done
 * - tx_mutex_delete()          Deletes the mutex if kernel service calls failed in the process
 * - _gx_synergy_display_driver_565rgb_setup()  Setups default GUIX callback functions for RGB565 in case of display
 *                                              format format is RGB565 format
 * - _gx_synergy_display_driver_24xrgb_setup()  Setups default GUIX callback functions for RGB565 in case of display
 *                                              format format is RGB888, unpacked format
 * - _gx_display_driver_32argb_setup()  Setups default GUIX callback functions for RGB565 in case of display
 *                                      format format is ARGB8888, unpacked format
 * - sf_el_gx_driver_setup()    Setups low level device drivers and overrides the GUIX default callback functions
 *                              with hardware accelerated functions.
 * @retval  GX_SUCCESS    Device driver setup is successfully done.
 * @retval  GX_FAILURE    Device driver setup failed.
 * @note    Make sure SF_EL_GX_Open() has been called when this function is called back by GUIX. The behavior is
 *          not defined if this function were not invoked by GUIX.
 **********************************************************************************************************************/
UINT SF_EL_GX_Setup (GX_DISPLAY * p_display)
{
    UINT          status;
    ssp_err_t     error;
    sf_el_gx_callback_args_t cb_args;

    sf_el_gx_instance_ctrl_t * p_ctrl_temp = gp_temp_context;

    /* Returns if temporary driver context is not initialized. It should be initialized by open API. */
    if(NULL == p_ctrl_temp)
    {
        return (UINT)GX_FAILURE;
    }

    /* Returns if GUIX display driver context is NULL. */
    if(NULL == p_display)
    {
        return (UINT)GX_FAILURE;
    }

    p_display->gx_display_color_format = sf_el_gx_setup_display_color_format_set
                                                        (p_ctrl_temp->p_display_instance->p_cfg->input[0].format);
    if ((GX_VALUE)(GX_COLOR_FORMAT_INVALID) == p_display->gx_display_color_format)
    {
        /* Informs user application the driver setup error */
        cb_args.device = SF_EL_GX_DEVICE_NONE;
        cb_args.event  = SF_EL_GX_EVENT_ERROR;
        cb_args.error  = SSP_ERR_INVALID_ARGUMENT;
        if (NULL != gp_temp_context->p_callback)
        {
            gp_temp_context->p_callback(&cb_args);
        }
        return (UINT)GX_FAILURE;
    }

    /** Copies the GX_DISPLAY context for later use. */
    p_display->gx_display_handle = 0UL;
    gp_temp_context->p_display = p_display;

    /** Setups GUIX low level device drivers */
    error = sf_el_gx_driver_setup (p_display, p_ctrl_temp);
    if(SSP_SUCCESS != error)
    {
        cb_args.device = SF_EL_GX_DEVICE_NONE;
        cb_args.event  = SF_EL_GX_EVENT_ERROR;
        cb_args.error  = error;
        if (NULL != gp_temp_context->p_callback)
        {
            p_ctrl_temp->p_callback(&cb_args);
        }
        return (UINT)GX_FAILURE;
    }

    /** Changes the driver state */
    p_ctrl_temp->state = SF_EL_GX_CONFIGURED;

    /** Clears the temporary storage for the pointer to a control block. */
    p_ctrl_temp = NULL;

    /** Unlocks the SF_EL_GX instance since driver setup is done */
    status = tx_mutex_put (&g_sf_el_gx_mutex);
    if((UINT)TX_SUCCESS != status)
    {
        return (UINT)GX_FAILURE;
    }

    return (UINT)GX_SUCCESS;
}  /* End of function SF_EL_GX_Setup() */

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy, Canvas initialization, setup the memory address of first canvas to be
 * rendered.
 * @retval  SSP_SUCCESS          Memory address is successfully configured to a canvas.
 * @retval  SSP_ERR_ASSERTION    Invalid control block (NULL pointer) or window root (NULL pointer) passed to driver.
 * @retval  SSP_ERR_INVALID_CALL Function call was made when the driver is not in SF_EL_GX_CONFIGURED state.
 * @retval  SSP_ERR_INTERNAL     Mutex operation had an error.
 **********************************************************************************************************************/
ssp_err_t SF_EL_GX_CanvasInit (sf_el_gx_ctrl_t * const p_api_ctrl, GX_WINDOW_ROOT * p_window_root)
{
    sf_el_gx_instance_ctrl_t * p_ctrl = (sf_el_gx_instance_ctrl_t *) p_api_ctrl;

    UINT status;
#if (SF_EL_GX_CFG_PARAM_CHECKING_ENABLE)
    SSP_ASSERT(p_ctrl);
    SSP_ASSERT(p_window_root);
#endif

    SF_EL_GX_ERROR_RETURN(SF_EL_GX_CONFIGURED == p_ctrl->state, SSP_ERR_INVALID_CALL);

    /** Locks the driver to update the context. */
    status = tx_mutex_get (&g_sf_el_gx_mutex, SF_EL_GX_MUTEX_WAIT_TIMER);
    if((UINT)TX_SUCCESS != status)
    {
        return SSP_ERR_INTERNAL;
    }

    /** Lets GUIX know the first canvas */
    if(NULL == p_ctrl->p_canvas)
    {   /* If a buffer is not given, set the one of frame buffers as a canvas */
        p_window_root->gx_window_root_canvas->gx_canvas_memory = (GX_COLOR *)p_ctrl->p_framebuffer_write;
    }
    else
    {   /* If a buffer for canvas is given, set it as a canvas */
        p_window_root->gx_window_root_canvas->gx_canvas_memory = (GX_COLOR *)p_ctrl->p_canvas;
    }

    /** Unlocks the driver. */
    status = tx_mutex_put (&g_sf_el_gx_mutex);
    if((UINT)TX_SUCCESS != status)
    {
        return SSP_ERR_INTERNAL;
    }

    return SSP_SUCCESS;
}  /* End of function SF_EL_GX_CanvasInit() */

/*******************************************************************************************************************//**
 * @} (end addtogroup SF_EL_GX)
 **********************************************************************************************************************/

#if (SF_EL_GX_CFG_PARAM_CHECKING_ENABLE)
/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy, parameter check function for SF_EL_GX_Open.
 * This function is called by SF_EL_GX_Open().
 * @param[in]     p_ctrl     Pointer to SF_EL_GX control block
 * @param[in]     p_cfg      Pointer to SF_EL_GX configuration data
 * @retval  SSP_SUCCESS                 All the configuration parameters are valid.
 * @retval  SSP_ERR_ASSERTION           NULL is given to either of p_ctrl, p_cfg or members of configuration structure.
 *                                      Neither of 0, 90, 180, 270 is specified to the rotation angle.
 * @retval  SSP_ERR_INVALID_ARGUMENT    Invalid configuration parameter is given.
 **********************************************************************************************************************/
static ssp_err_t sf_el_gx_open_param_check (sf_el_gx_instance_ctrl_t * const p_ctrl, sf_el_gx_cfg_t const * const p_cfg)
{
    ssp_err_t error;

    /** Check NULL pointers for configuration parameters. */
    SSP_ASSERT(p_ctrl);
    SSP_ASSERT(p_cfg);
    SSP_ASSERT(p_cfg->p_display_instance);
    SSP_ASSERT(p_cfg->p_display_runtime_cfg);
    SSP_ASSERT(p_cfg->p_framebuffer_a);

    /** Check screen rotation setting. */
    error = sf_el_gx_open_param_check_rotation_angle(p_cfg);
    if (SSP_SUCCESS != error)
    {
        /** Check canvas setting. */
        error = sf_el_gx_open_param_check_canvas_setting(p_cfg);
    }

    return error;
}

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy, parameter check function for the screen rotation angle setting.
 * This function is called by sf_el_gx_open_param_check().
 * @param[in]     p_cfg      Pointer to SF_EL_GX configuration data
 * @retval  SSP_SUCCESS                 All the configuration parameters are valid.
 * @retval  SSP_ERR_INVALID_ARGUMENT    Neither of 0, 90, 180, 270 is specified to the rotation angle.
 **********************************************************************************************************************/
static ssp_err_t sf_el_gx_open_param_check_rotation_angle (sf_el_gx_cfg_t const * const p_cfg)
{
    ssp_err_t error;

    /** Check if specified screen rotation angle is valid. */
    if(((0U == (uint32_t)(p_cfg->rotation_angle))
      ||(90U  == (uint32_t)(p_cfg->rotation_angle))
      ||(180U == (uint32_t)(p_cfg->rotation_angle))
      ||(270U == (uint32_t)(p_cfg->rotation_angle))))
    {
        error = SSP_SUCCESS;
    }
    else
    {
        error = SSP_ERR_INVALID_ARGUMENT;
    }

    return error;
}

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy, parameter check function for the GUIX Canvas setting.
 * This function is called by sf_el_gx_open_param_check().
 * @param[in]     p_ctrl     Pointer to SF_EL_GX control block
 * @param[in]     p_cfg      Pointer to SF_EL_GX configuration data
 * @retval  SSP_SUCCESS                 All the configuration parameters are valid.
 * @retval  SSP_ERR_INVALID_ARGUMENT    Invalid configuration parameter is given.
 **********************************************************************************************************************/
static ssp_err_t sf_el_gx_open_param_check_canvas_setting (sf_el_gx_cfg_t const * const p_cfg)
{
    ssp_err_t error = SSP_SUCCESS;

    do
    {
        /** Check if the pointer to a canvas is valid if the screen rotation performed. */
        if ((NULL == p_cfg->p_canvas) && (0 != (uint32_t)p_cfg->rotation_angle))
        {
            error = SSP_ERR_INVALID_ARGUMENT;
            break;
        }

        /** Check if the pointer to a canvas is not same with the pointer to frame buffers. */
        if (p_cfg->p_canvas == p_cfg->p_framebuffer_a)
        {
            error = SSP_ERR_INVALID_ARGUMENT;
            break;
        }
        if ((NULL != p_cfg->p_framebuffer_b) && (p_cfg->p_canvas == p_cfg->p_framebuffer_b))
        {
            error = SSP_ERR_INVALID_ARGUMENT;
        }
    } while (0);

    return error;
}
#endif

/*******************************************************************************************************************//**
 * @brief  SF_EL_GX_Setup sub-routine to return a GUIX display color format corresponding to the SSP display color
 * format. This function is called by SF_EL_GX_Setup().
 * @param   format          SSP display color format
 **********************************************************************************************************************/
static GX_VALUE sf_el_gx_setup_display_color_format_set (display_in_format_t format)
{
    GX_VALUE gx_display_color_format;

    switch (format)
    {
    case DISPLAY_IN_FORMAT_16BITS_RGB565:  ///< Setups generic callback functions for RGB565, 16 bits color format.
        gx_display_color_format = GX_COLOR_FORMAT_565RGB;
        break;

    case DISPLAY_IN_FORMAT_32BITS_RGB888:  ///< Setups generic callback functions for RGB888, 24 bits, unpacked format.
        gx_display_color_format = GX_COLOR_FORMAT_24XRGB;
        break;

    case DISPLAY_IN_FORMAT_32BITS_ARGB8888:  ///< Setups generic callback functions for RGB888, 24 bits, unpacked format.
        gx_display_color_format = GX_COLOR_FORMAT_32ARGB;
        break;

    case DISPLAY_IN_FORMAT_16BITS_ARGB4444:  ///< Setups generic callback functions for ARGB4444, 16 bits color format.
        gx_display_color_format = GX_COLOR_FORMAT_4444ARGB;
        break;

    case DISPLAY_IN_FORMAT_CLUT8:
        gx_display_color_format = GX_COLOR_FORMAT_8BIT_PALETTE;
        break;

    default:
        gx_display_color_format = GX_COLOR_FORMAT_INVALID;
    }

    return gx_display_color_format;
}

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy, get frame buffer address.
 * This function is called by GUIX.
 * @param[in]     display_handle    Pointer to the SF_EL_GX control block
 * @param[in,out] pp_visible        Pointer to a pointer to store visible frame buffer
 * @param[in,out] pp_working        Pointer to a pointer to store working frame buffer
 * @note NULL check for the frame buffers are not performed. This function does not expect NULL for them but does
 *       nothing even if the caller function passed NULL.
 **********************************************************************************************************************/
void sf_el_gx_frame_pointers_get(ULONG display_handle, GX_UBYTE ** pp_visible, GX_UBYTE ** pp_working)
{
    /** Gets control block */
    sf_el_gx_instance_ctrl_t * p_ctrl = (sf_el_gx_instance_ctrl_t *)(display_handle);

#if (SF_EL_GX_CFG_PARAM_CHECKING_ENABLE)
    if(NULL == p_ctrl)
    {
        return;
    }
#endif

    *pp_visible = p_ctrl->p_framebuffer_read;
    *pp_working = p_ctrl->p_framebuffer_write;
}  /* End of function sf_el_gx_frame_pointers_get() */

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy, toggle frame buffer.
 * This function calls display_api_t::layerChange to flips the frame buffer and tx_semaphore_get to suspend a thread
 * until a display device going into blanking period.
 * This function is called by GUIX when following functions are executed.
 * - _gx_canvas_drawing_complete()
 * - _gx_system_canvas_refresh()
 * @param[in]     display_handle     Pointer to the SF_EL_GX control block
 * @param[in,out] pp_visible_frame   Pointer to a pointer to store visible frame buffer
 **********************************************************************************************************************/
void sf_el_gx_frame_toggle(ULONG display_handle, GX_BYTE ** pp_visible_frame)
{
    /** Gets control block */
    sf_el_gx_instance_ctrl_t * p_ctrl = (sf_el_gx_instance_ctrl_t *)display_handle;
    void * p_next_temp                = NULL;

#if (SF_EL_GX_CFG_PARAM_CHECKING_ENABLE)
    if(NULL == p_ctrl)
    {
        return;
    }
    if(NULL == pp_visible_frame)
    {
        return;
    }
#endif

    /** Updates the frame buffer addresses */
    p_next_temp = p_ctrl->p_framebuffer_read;
    p_ctrl->p_display_runtime_cfg->input.p_base = p_ctrl->p_framebuffer_write;
    p_ctrl->p_framebuffer_read  = p_ctrl->p_framebuffer_write;
    p_ctrl->p_framebuffer_write = p_next_temp;

    /** Returns the address of visible frame buffer */
    *((GX_BYTE **)pp_visible_frame) = (GX_BYTE *)p_next_temp;

    /** Requests display driver to toggle frame buffer */
    while (SSP_SUCCESS != p_ctrl->p_display_instance->p_api->layerChange(
                          p_ctrl->p_display_instance->p_ctrl,
                          p_ctrl->p_display_runtime_cfg,
                          DISPLAY_FRAME_LAYER_1))
    {
        tx_thread_sleep(1UL);
    }

    /** Sets rendering_enable flag to the display driver to synchronize the timing */
    p_ctrl->rendering_enable = true;

    /** Waits until the set of semaphore which is set when the display device going into blanking period */
    tx_semaphore_get(&p_ctrl->semaphore, TX_WAIT_FOREVER);

}  /* End of function sf_el_gx_frame_toggle() */

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy, Display interface setup function
 * This function calls display_api_t::open and display_api_t::start.
 * This function is called by sf_el_gx_driver_setup().
 * @param[in]    p_display    Pointer to a GUIX display control block
 * @param[in]    p_ctrl       Pointer to a SF_EL_GX control block
 * @retval  SSP_SUCCESS       Display device was opened successfully.
 * @retval  Others            See @ref Common_Error_Codes for other possible return codes. This function calls
 *                            display_api_t::open and display_api_t::start.
 * @note This function is only allowed to be called by sf_el_gx_driver_setup().
 * @note Parameter check is not required since it is done in SF_EL_GX_Setup().
 **********************************************************************************************************************/
static ssp_err_t sf_el_gx_display_open (sf_el_gx_instance_ctrl_t * const p_ctrl)
{
    ssp_err_t error;
    sf_el_gx_callback_args_t cb_args;
    display_cfg_t   tmp_cfg = *(p_ctrl->p_display_instance->p_cfg);

    /** Registers the callback function for this module */
    tmp_cfg.p_callback = sf_el_gx_callback;

    /** Utilizes p_context to let callback function set the semaphore */
    tmp_cfg.p_context  = (void *)p_ctrl;

    /**  Display driver open */
    error = p_ctrl->p_display_instance->p_api->open(p_ctrl->p_display_instance->p_ctrl,
                                                            (display_cfg_t const *)(&tmp_cfg));
    if(SSP_SUCCESS != error)
    {
        SF_EL_GX_SSP_USER_ERROR_CALLBACK(p_ctrl, SF_EL_GX_DEVICE_DISPLAY, error);
    }
    else
    {
        /**  Display driver start */
        error = p_ctrl->p_display_instance->p_api->start(p_ctrl->p_display_instance->p_ctrl);
        if(SSP_SUCCESS != error)
        {
            SF_EL_GX_SSP_USER_ERROR_CALLBACK(p_ctrl, SF_EL_GX_DEVICE_DISPLAY, error);
        }
    }

    return error;
}  /* End of function sf_el_gx_display_open() */

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy, Display interface setup function
 * This function calls display_api_t::stop and display_api_t::close.
 * This function is called by SF_EL_GX_Close().
 * @param[in]    p_ctrl       Pointer to a SF_EL_GX control block
 * @retval  SSP_SUCCESS       Display device was closed successfully.
 * @retval  SSP_ERR_TIMEOUT   Display device did not stop or be finalized.
 * @note This function is only allowed to be called by SF_EL_GX_Close().
 * @note Parameter check is not required since it is done in SF_EL_GX_Close().
 **********************************************************************************************************************/
static ssp_err_t sf_el_gx_display_close (sf_el_gx_instance_ctrl_t * const p_ctrl)
{
    ssp_err_t error;
    uint32_t  retry_count = 0;

    while(1)
    {
        /**  Stops display driver */
        error = p_ctrl->p_display_instance->p_api->stop(p_ctrl->p_display_instance->p_ctrl);
        if(SSP_ERR_INVALID_UPDATE_TIMING == error)
        {    /* Wait until current display configuration updating done */
            tx_thread_sleep(1UL);
            retry_count++;
            if(SF_EL_GX_DISPLAY_HW_WAIT_COUNT_MAX < retry_count)
            {
                return SSP_ERR_TIMEOUT;
            }
        }
        else
        {
            break;
        }
    }
    retry_count = 0U;
    while(1)
    {
        /**  Closes display driver */
        error = p_ctrl->p_display_instance->p_api->close(p_ctrl->p_display_instance->p_ctrl);
        if(SSP_ERR_INVALID_UPDATE_TIMING == error)
        {    /* Wait until current display configuration updating done */
            tx_thread_sleep(1UL);
            retry_count++;
            if(SF_EL_GX_DISPLAY_HW_WAIT_COUNT_MAX < retry_count)
            {
                return SSP_ERR_TIMEOUT;
            }
        }
        else
        {
            break;
        }
    }
    return SSP_SUCCESS;
}  /* End of function sf_el_gx_display_close() */

/*******************************************************************************************************************//**
 * @brief  Setups GUIX display driver.
 * This function calls _gx_synergy_display_driver_setup(), which is generated by GUIX Studio, and calls subroutines
 * sf_el_gx_d2_open() and sf_el_gx_display_open() to setup graphics hardwares, also calls sf_el_gx_canvas_clear to clear
 * a canvas.
 * This function is called by SF_EL_GX_Setup().
 * @param[in]    p_display     Pointer to the GUIX display control block
 * @param[in]    p_ctrl        Pointer to a SF_EL_GX control block
 * @retval  SSP_SUCCESS  The GUIX drivers are successfully configured.
 * @retval  Others The GUIX drivers setup had a failure. See @ref Common_Error_Codes for other possible return codes.
 *                 This function calls display_api_t::open and display_api_t::start.
 * @note Parameter checks are not required since they are done in SF_EL_GX_Setup().
 **********************************************************************************************************************/
static ssp_err_t sf_el_gx_driver_setup (GX_DISPLAY * p_display, sf_el_gx_instance_ctrl_t * const p_ctrl)
{
    ssp_err_t error;

    /** Setups GUIX draw functions */
    _gx_synergy_display_driver_setup(p_display);

#if GX_USE_SYNERGY_DRW
    /** Setups D/AVE 2D */
    error = sf_el_gx_d2_open (p_display, p_ctrl);
    SF_EL_GX_ERROR_RETURN(error == SSP_SUCCESS, error);
#endif
    /** Clear canvas with zero */
    sf_el_gx_canvas_clear (p_display, p_ctrl);

    /** Setups Display interface */
    error = sf_el_gx_display_open(p_ctrl);
    SF_EL_GX_ERROR_RETURN(error == SSP_SUCCESS, error);

    /** Registers the SF_EL_GX context to GUIX display handle */
    p_display->gx_display_handle = (ULONG)p_ctrl;

    return SSP_SUCCESS;
}  /* End of function sf_el_gx_driver_setup() */

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy, Canvas clear function, clear the frame buffers with zero.
 * This function is called by sf_el_gx_driver_setup().
 * @param[in]    p_display    Pointer to a GUIX display control block
 * @param[in]    p_ctrl       Pointer to a SF_EL_GX control block
 * @retval  none.
 * @note This function is designed to be called by sf_el_gx_driver_setup().
 * @note Parameter checks are not required since they are done in SF_EL_GX_Setup().
 **********************************************************************************************************************/
static void sf_el_gx_canvas_clear (GX_DISPLAY * p_display, sf_el_gx_instance_ctrl_t * const p_ctrl)
{
    int32_t   divisor;
    ULONG   * put;

    switch (p_display->gx_display_color_format)
    {
    case GX_COLOR_FORMAT_565RGB:  ///< RGB565, 16 bits
        /* No break intentionally */
    case GX_COLOR_FORMAT_4444ARGB:   ///< ARGB4444, 16 bits
        divisor = 2;              ///< 16-bit data needs a half times data copy
        break;

    case GX_COLOR_FORMAT_24XRGB:  ///< RGB888, 24 bits, unpacked
        /* No break intentionally */
    case GX_COLOR_FORMAT_32ARGB:  ///< ARGB8888, 32 bits
        divisor = 1;
        break;

    case GX_COLOR_FORMAT_8BIT_PALETTE:
        /* No break intentionally */
    default:    /* Apply this divisor value for any undefined color formats. */
        divisor = 4;
        break;
    }

    /** Clears the frame buffers */
    put = (ULONG *) p_ctrl->p_framebuffer_read;
    for (int32_t loop = 0; loop < ((p_display->gx_display_width * p_display->gx_display_height) / divisor); loop++)
    {
        *put = 0UL;
        ++put;
    }

    put = (ULONG *) p_ctrl->p_framebuffer_write;
    for (int32_t loop = 0; loop < ((p_display->gx_display_width * p_display->gx_display_height) / divisor); loop++)
    {
        *put = 0UL;
        ++put;
    }
}  /* End of function sf_el_gx_canvas_clear() */

#if GX_USE_SYNERGY_DRW
/*******************************************************************************************************************//**
 * @brief  sf_el_gx_d2_open sub-routine to select a D/AVE 2D color format corresponding to the GUIX color format.
 * This function is called by sf_el_gx_d2_open().
 * @param[in]    p_display     Pointer to a GUIX display control block
 * @retval  format             d2_mode_rgb565, d2_mode_argb4444, d2_mode_rgb888, d2_mode_argb8888 or d2_mode_clut.
 * @retval  SSP_ERR_D2D_ERROR_INIT      The D/AVE 2D returns error at the initialization.
 * @retval  SSP_ERR_D2D_ERROR_RENDERING The D/AVE 2D returns error at opening a display list buffer.
 * @retval  SSP_ERR_INVALID_ARGUMENT    Specified display parameter is invalid.
 * @note This function is only allowed to be called by sf_el_gx_driver_setup().
 * @note Parameter check is not required since it is done in SF_EL_GX_Setup().
 **********************************************************************************************************************/
static d2_s32 sf_el_gx_d2_open_format_set (GX_DISPLAY * p_display)
{
    d2_s32 format = d2_mode_rgb565;

    /** Gets output color format of D/AVE 2D interface */
    switch (p_display->gx_display_color_format)
    {
    case GX_COLOR_FORMAT_565RGB:  ///< RGB565, 16 bits
        /* Initial value applied */
        break;

    case GX_COLOR_FORMAT_4444ARGB:    ///< ARGB4444, 16 bits
        format = d2_mode_argb4444;
        break;

    case GX_COLOR_FORMAT_24XRGB:  ///< RGB888, 24 bits, unpacked
        format = d2_mode_rgb888;
        break;

    case GX_COLOR_FORMAT_32ARGB:  ///< ARGB8888, 32 bits
        format = d2_mode_argb8888;
        break;

    case GX_COLOR_FORMAT_8BIT_PALETTE:
        /* No break intentionally */
    default:    /* Apply 8-bit CLUT format for any undefined color formats. */
        format = d2_mode_clut;
        break;
    }

	return format;
}

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy,D/AVE 2D(2D Drawing Engine) open function
 * This function calls following D/AVE 2D functions:
 * - d2_opendevice()           Creates a new device handle of the D/AVE 2D driver
 * - d2_inithw()               Initializes 2D Drawing Engine hardware
 * - d2_startframe()           Mark the begin of a scene
 * - d2_framebuffer()          Specifies the rendering target
 * This function is called by sf_el_gx_driver_setup().
 * @param[in]    p_display     Pointer to a GUIX display control block
 * @param[in]    p_ctrl        Pointer to a SF_EL_GX control block
 * @retval  SSP_SUCCESS             The D/AVE 2D driver is successfully opened.
 * @retval  SSP_ERR_D2D_ERROR_INIT      The D/AVE 2D returns error at the initialization.
 * @retval  SSP_ERR_D2D_ERROR_RENDERING The D/AVE 2D returns error at opening a display list buffer.
 * @retval  SSP_ERR_INVALID_ARGUMENT    Specified display parameter is invalid.
 * @note This function is only allowed to be called by sf_el_gx_driver_setup().
 * @note Parameter checks are not required since they are done in SF_EL_GX_Setup().
 **********************************************************************************************************************/
static ssp_err_t sf_el_gx_d2_open (GX_DISPLAY * p_display, sf_el_gx_instance_ctrl_t * const p_ctrl)
{
    d2_s32 d2_err;

    /** Creates a device handle */
    if(false == p_ctrl->dave2d_buffer_cache_enabled)
    {
        /* Disable the cache of the Dave2D so it doesn't do burst writes. */
        p_display->gx_display_accelerator = d2_opendevice(4);

    }
    else
    {
        /* Enable the cache of the Dave2D so it perform burst writes. */
        p_display->gx_display_accelerator = d2_opendevice(0);
    }

    /** Initializes 2D Drawing Engine hardware */
    d2_err = d2_inithw(p_display->gx_display_accelerator, 0);
    sf_el_gx_d2_open_error_callback(p_ctrl,  d2_err);
    SF_EL_GX_ERROR_RETURN(D2_OK == d2_err, SSP_ERR_D2D_ERROR_INIT);

    /** Opens a display list buffer for drawing commands */
    d2_err = d2_startframe(p_display->gx_display_accelerator);
    sf_el_gx_d2_open_error_callback(p_ctrl,  d2_err);
    SF_EL_GX_ERROR_RETURN(D2_OK == d2_err, SSP_ERR_D2D_ERROR_RENDERING);

    /** Gets output color format of D/AVE 2D interface */
    d2_s32 format = sf_el_gx_d2_open_format_set(p_display);

    /** Defines the framebuffer memory area and layout */
    d2_err = d2_framebuffer(p_display->gx_display_accelerator,
                   p_ctrl->p_framebuffer_write,
                   (d2_u32)p_display->gx_display_width,
                   (d2_u32)p_display->gx_display_width,
                   (d2_u32)p_display->gx_display_height,
                   format);
    sf_el_gx_d2_open_error_callback(p_ctrl,  d2_err);
    SF_EL_GX_ERROR_RETURN(D2_OK == d2_err, SSP_ERR_INVALID_ARGUMENT);

    return SSP_SUCCESS;
}  /* End of function sf_gx_display_dave2d_open() */

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy, callback user function if D/AVE 2D setup failed in sf_el_gx_d2_open().
 * This function is called by sf_el_gx_d2_open().
 * @param[in]    p_ctrl        Pointer to a SF_EL_GX control block
 * @param[in]    d2_err        Error code returned from D/AVE 2D driver call
 */
static void sf_el_gx_d2_open_error_callback (sf_el_gx_instance_ctrl_t * const p_ctrl, d2_s32 d2_err)
{
    sf_el_gx_callback_args_t cb_args;

    if((p_ctrl)->p_callback)
    {
        if(D2_OK != d2_err)
        {
            cb_args.device = SF_EL_GX_DEVICE_DRW;
            cb_args.event  = SF_EL_GX_EVENT_ERROR;
            cb_args.error  = (uint32_t)(d2_err);
            (p_ctrl)->p_callback(&cb_args);
        }
    }
}  /* End of function sf_el_gx_d2_open_error_callback() */

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy, D/AVE 2D(2D Drawing Engine) close function
 * This function calls D/AVE 2D function d2_closedevice() to destroy a device handle.
 * This function is called by SF_EL_GX_Close().
 * @param[in]    p_ctrl        Pointer to a SF_EL_GX control block
 * @retval  SSP_SUCCESS              The D/AVE 2D driver is successfully closed.
 * @retval  SSP_ERR_D2D_ERROR_DEINIT  D/AVE 2D has an error in the initialization.
 * @note This function is only allowed to be called by SF_EL_GX_Close().
 **********************************************************************************************************************/
static ssp_err_t sf_el_gx_d2_close (sf_el_gx_instance_ctrl_t * const p_ctrl)
{
    d2_s32 d2_err;

    /** Destroy a device handle */
    d2_err = d2_closedevice(p_ctrl->p_display->gx_display_accelerator);
    SF_EL_GX_ERROR_RETURN(D2_OK == d2_err, SSP_ERR_D2D_ERROR_DEINIT);

    return SSP_SUCCESS;
}  /* End of function sf_el_gx_d2_close() */
#endif

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy, get screen rotation angle.
 * This function is called by GUIX.
 * @param[in]   display_handle      Pointer to the SF_EL_GX control block
 * @retval      Angle               Screen rotation angle. {0, 90, 180 or 270} is supposed to be returned.
 **********************************************************************************************************************/
INT sf_el_gx_display_rotation_get(ULONG display_handle)
{
    sf_el_gx_instance_ctrl_t *p_ctrl = (sf_el_gx_instance_ctrl_t *)(display_handle);
    return (INT)(p_ctrl->rotation_angle);
}  /* End of function sf_el_gx_display_rotation_get() */

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy, get active video screen size.
 * This function is called by GUIX.
 * @param[in]     display_handle     Pointer to the SF_EL_GX control block
 * @param[out]    p_width            Pointer to an int size memory to return screen width in pixels
 * @param[out]    p_height           Pointer to an int size memory to return screen height in pixels
 **********************************************************************************************************************/
void   sf_el_gx_display_actual_size_get(ULONG display_handle, INT * p_width, INT * p_height)
{
    sf_el_gx_instance_ctrl_t *p_ctrl = (sf_el_gx_instance_ctrl_t *)(display_handle);
    *p_width  = (INT)p_ctrl->p_display_instance->p_cfg->output.htiming.display_cyc;
    *p_height = (INT)p_ctrl->p_display_instance->p_cfg->output.vtiming.display_cyc;
}  /* End of function sf_el_gx_display_actual_size_get() */

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy, get JPEG work buffer address.
 * This function is called by GUIX.
 * @param[in]     display_handle    Pointer to the SF_EL_GX control block
 * @param[in,out] p_memory_size     Pointer to caller defined variable to store JPEG work buffer
 * @retval        Address           JPEG work buffer address
 **********************************************************************************************************************/
void * sf_el_gx_jpeg_buffer_get (ULONG display_handle, INT * p_memory_size)
{
#if GX_USE_SYNERGY_JPEG
    sf_el_gx_instance_ctrl_t * p_ctrl = (sf_el_gx_instance_ctrl_t *)(display_handle);

    if(p_memory_size)
    {
        *p_memory_size = (INT)p_ctrl->jpegbuffer_size;
    }
    return((void*)p_ctrl->p_jpegbuffer);
#else
    SSP_PARAMETER_NOT_USED(display_handle);

    if(p_memory_size)
    {
        *p_memory_size = 0;
    }
    return(NULL);

#endif
}  /* End of function sf_el_gx_jpeg_buffer_get() */

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy, get the instance of JPEG Framework module.
 * This function is called by _gx_synergy_jpeg_draw().
 * @param[in]     display_handle    Pointer to the SF_EL_GX control block.
 * @retval        Address           Pointer to an instance of JPEG Framework module.
 **********************************************************************************************************************/
void * sf_el_gx_jpeg_instance_get (ULONG display_handle)
{
#if GX_USE_SYNERGY_JPEG
    sf_el_gx_instance_ctrl_t * p_ctrl = (sf_el_gx_instance_ctrl_t *)(display_handle);

    return (void *)p_ctrl->p_sf_jpeg_decode_instance;

#else
    SSP_PARAMETER_NOT_USED(display_handle);

    return (NULL);
#endif
}  /* End of function sf_el_gx_jpeg_instance_get() */

/*******************************************************************************************************************//**
 * @brief  GUIX adaptation framework for Synergy, set CLUT table in the display module by calling display_api_t::clut.
 * This function is called by GUIX.
 * @param[in]     display_handle     Pointer to the SF_EL_GX control block
 **********************************************************************************************************************/
void   sf_el_gx_display_8bit_palette_assign(ULONG display_handle)
{
    sf_el_gx_instance_ctrl_t *p_ctrl = (sf_el_gx_instance_ctrl_t *)(display_handle);
    display_clut_cfg_t       clut_cfg;

    clut_cfg.p_base = (uint32_t *)p_ctrl->p_display->gx_display_palette;
    clut_cfg.start  = (uint16_t)0;
    clut_cfg.size   = (uint16_t)p_ctrl->p_display->gx_display_palette_size;

    p_ctrl->p_display_instance->p_api->clut(p_ctrl->p_display_instance->p_ctrl, &clut_cfg, DISPLAY_FRAME_LAYER_1);

}  /* End of function sf_el_gx_display_8bit_palette_assign() */

/*******************************************************************************************************************//**
 * @brief  Callback function for GUIX driver framework. This function is called back from a Display HAL driver module.
 * If DISPLAY_EVENT_LINE_DETECTION is returned from Display HAL driver module, it sets the semaphore for rendering and
 * displaying synchronization. This function invokes a user callback function if registered through SF_EL_GX_Open()
 * function.
 * @param[in]    p_args   Pointer to the Display interface callback argument.
 **********************************************************************************************************************/
static void sf_el_gx_callback (display_callback_args_t * p_args)
{
    sf_el_gx_callback_args_t cb_arg;
    sf_el_gx_instance_ctrl_t * p_ctrl = (sf_el_gx_instance_ctrl_t *)p_args->p_context;

    if (DISPLAY_EVENT_LINE_DETECTION == p_args->event)
    {
        if (p_ctrl->rendering_enable)
        {
            /** Let GUIX know the display been in blanking period */
            tx_semaphore_ceiling_put((TX_SEMAPHORE *)&p_ctrl->semaphore, 1UL);
            p_ctrl->rendering_enable = false;
        }
        cb_arg.event = SF_EL_GX_EVENT_DISPLAY_VSYNC;
    }
    else if (DISPLAY_EVENT_GR1_UNDERFLOW == p_args->event)
    {
        cb_arg.event = SF_EL_GX_EVENT_UNDERFLOW;
    }
    else
    {
        /* Do nothing */
    }

    /** Invoke a user callback function if registered */
    if (p_ctrl->p_callback)
    {
        cb_arg.device = SF_EL_GX_DEVICE_DISPLAY;
        cb_arg.error  = SSP_SUCCESS;
        p_ctrl->p_callback(&cb_arg);
    }
}  /* End of function sf_el_gx_callback() */

