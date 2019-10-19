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
/*  11423 West Bernardo Court               www.expresslogic.com          */
/*  San Diego, CA  92127                                                  */
/*                                                                        */
/**************************************************************************/

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

/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** GUIX Display Driver component                                         */
/**************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include    <stdio.h>
#include    <string.h>

#include    "gx_api.h"
#include    "gx_display.h"
#include    "gx_utility.h"

#if (GX_USE_SYNERGY_DRW == 1)
#include    "dave_driver.h"
#endif

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/
#ifndef GX_DISABLE_ERROR_CHECKING
#define LOG_DAVE_ERRORS
#endif

/** Space used to store int to fixed point polygon vertices. */
#define MAX_POLYGON_VERTICES GX_POLYGON_MAX_EDGE_NUM

/*LDRA_INSPECTED 77 S This macro does not work when surrounded by parentheses. */
#define DRAW_PIXEL  if (glyph_data & mask)              \
                    {                                   \
                        *put = text_color;              \
                    }                                   \
                    put++;                              \
                    mask = (GX_UBYTE)(mask << 1);

#if defined(LOG_DAVE_ERRORS)
/** Macro to check for and log status code from Dave2D engine. */
/*LDRA_INSPECTED 77 S This macro does not work when surrounded by parentheses. */
#define CHECK_DAVE_STATUS(a)    gx_log_dave_error(a);
#else
#define CHECK_DAVE_STATUS(a)    a;
#endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/** indicator for the number of visible frame buffer */
typedef enum e_frame_buffers
{
    FRAME_BUFFER_A = 0,
    FRAME_BUFFER_B
} frame_buffers_t;

#if (GX_USE_SYNERGY_DRW == 1)
/** DAVE 2D screen rotation parameters */
typedef struct st_d2_rotation_param
{
    d2_border xmin;
    d2_border ymin;
    d2_border xmax;
    d2_border ymax;
    d2_point  x_texture_zero;
    d2_point  y_texture_zero;
    d2_s32    dxu;
    d2_s32    dxv;
    d2_s32    dyu;
    d2_s32    dyv;
    GX_VALUE  x_resolution;
    GX_VALUE  y_resolution;
    INT       copy_width;
    INT       copy_height;
    INT       copy_width_rotated;
    INT       copy_height_rotated;
    GX_RECTANGLE    copy_clip;
    INT       angle;
} d2_rotation_param_t;
#endif

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
/** indicator for the number of visible frame buffer */
static GX_UBYTE   * visible_frame = NULL;
static GX_UBYTE   * working_frame = NULL;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static VOID     gx_copy_visible_to_working_8bpp(GX_CANVAS * canvas, GX_RECTANGLE * copy);

static VOID     gx_rotate_canvas_to_working_8bpp(GX_CANVAS * canvas, GX_RECTANGLE * copy, INT angle);

static VOID     gx_rotate_canvas_to_working_8bpp_draw(GX_UBYTE * pGetRow, GX_UBYTE * pPutRow, INT width, INT height,
                                                                                                       INT stride);
static VOID     gx_rotate_canvas_to_working_8bpp_draw_rorate90(GX_UBYTE * pGetRow, GX_UBYTE * pPutRow, INT width,
                                                                   INT height, INT canvas_stride, INT disp_stride);
static VOID     gx_rotate_canvas_to_working_8bpp_draw_rorate180(GX_UBYTE * pGetRow, GX_UBYTE * pPutRow, INT width,
                                                                                           INT height, INT stride);
static VOID     gx_rotate_canvas_to_working_8bpp_draw_rorate270(GX_UBYTE * pGetRow, GX_UBYTE * pPutRow, INT width,
                                                                   INT height, INT canvas_stride, INT disp_stride);

#if (GX_USE_SYNERGY_DRW == 1)
static VOID     gx_dave2d_set_texture_8bpp(GX_DRAW_CONTEXT * context,d2_device * dave, INT xpos,
                                                                                        INT ypos, GX_PIXELMAP * map);
static VOID     gx_sw_8bpp_glyph_4bit_draw(GX_DRAW_CONTEXT * context, GX_RECTANGLE * draw_area, GX_POINT * map_offset,
                                                                                                const GX_GLYPH * glyph);
static VOID     gx_sw_8bpp_glyph_1bit_draw(GX_DRAW_CONTEXT * context, GX_RECTANGLE * draw_area, GX_POINT * map_offset,
                                                                                                const GX_GLYPH * glyph);
static UINT     gx_sw_8bpp_glyph_1bit_draw_get_valid_bytes (GX_RECTANGLE * draw_area, GX_POINT * p_map_offset,
                                UINT * p_pixel_in_first_byte, UINT * p_pixel_in_last_byte, GX_UBYTE * p_init_mask);

static VOID     gx_dave2d_copy_visible_to_working_8bpp(GX_CANVAS * canvas, GX_RECTANGLE * copy);
static VOID     gx_dave2d_rotate_canvas_to_working_8bpp(GX_CANVAS * canvas, GX_RECTANGLE * copy, INT rotation_angle);
#endif

/** functions shared in Synergy GUIX display driver files */
#if (GX_USE_SYNERGY_DRW == 1)
#if defined(LOG_DAVE_ERRORS)
extern VOID         gx_log_dave_error(d2_s32 status);
extern INT          gx_get_dave_error(INT get_index);
#endif
extern VOID         gx_display_list_flush(GX_DISPLAY * display);
extern VOID         gx_display_list_open(GX_DISPLAY * display);
extern d2_device  * gx_dave2d_set_clip(GX_DRAW_CONTEXT * context);
extern VOID         gx_dave2d_rotate_canvas_to_working_param_set (d2_rotation_param_t * p_param, GX_DISPLAY *p_display,
                                                                                            GX_CANVAS * p_canvas);
#endif

/** functions provided by sf_el_gx.c */
extern void     sf_el_gx_frame_toggle (ULONG display_handle, GX_UBYTE ** visible);
extern void     sf_el_gx_frame_pointers_get(ULONG display_handle, GX_UBYTE ** visible, GX_UBYTE ** working);
extern INT      sf_el_gx_display_rotation_get(ULONG display_handle);
extern void     sf_el_gx_display_actual_size_get(ULONG display_handle, INT * width, INT * height);
extern void     sf_el_gx_display_8bit_palette_assign(ULONG display_handle);

/***********************************************************************************************************************
 * Synergy GUIX display driver function prototypes (called by GUIX)
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_synergy_buffer_toggle_8bpp(GX_CANVAS * canvas, GX_RECTANGLE * dirty);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_display_driver_8bit_palette_assign(GX_DISPLAY * display, GX_COLOR * palette, INT count);

#if (GX_USE_SYNERGY_DRW == 1)
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_drawing_initiate_8bpp(GX_DISPLAY * display, GX_CANVAS * canvas);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_drawing_complete_8bpp(GX_DISPLAY * display, GX_CANVAS * canvas);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_horizontal_line_8bpp(GX_DRAW_CONTEXT * context,
                               INT xstart, INT xend, INT ypos, INT width, GX_COLOR color);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_vertical_line_8bpp(GX_DRAW_CONTEXT * context,
                             INT ystart, INT yend, INT xpos, INT width, GX_COLOR color);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_simple_line_draw_8bpp(GX_DRAW_CONTEXT * context, INT xstart, INT ystart, INT xend, INT yend);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_simple_wide_line_8bpp(GX_DRAW_CONTEXT * context, INT xstart, INT ystart, INT xend, INT yend);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_horizontal_pattern_line_draw_8bpp(GX_DRAW_CONTEXT * context, INT xstart, INT xend, INT ypos);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_vertical_pattern_line_draw_8bpp(GX_DRAW_CONTEXT * context, INT ystart, INT yend, INT xpos);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_pixelmap_draw_8bpp(GX_DRAW_CONTEXT * context, INT xpos, INT ypos, GX_PIXELMAP * pixelmap);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_polygon_draw_8bpp(GX_DRAW_CONTEXT * context, GX_POINT * vertex, INT num);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_polygon_fill_8bpp(GX_DRAW_CONTEXT * context, GX_POINT * vertex, INT num);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_pixel_write_8bpp(GX_DRAW_CONTEXT * context, INT x, INT y, GX_COLOR color);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_block_move_8bpp(GX_DRAW_CONTEXT * context, GX_RECTANGLE *block, INT xshift, INT yshift);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_glyph_1bit_draw_8bpp(GX_DRAW_CONTEXT * context, GX_RECTANGLE * draw_area, GX_POINT * map_offset,
                                                                                                const GX_GLYPH * glyph);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_glyph_4bit_draw_8bpp(GX_DRAW_CONTEXT * context, GX_RECTANGLE * draw_area, GX_POINT * map_offset,
                                                                                                const GX_GLYPH * glyph);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_circle_draw_8bpp(GX_DRAW_CONTEXT * context, INT xcenter, INT ycenter, UINT r);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_circle_fill_8bpp(GX_DRAW_CONTEXT * context, INT xcenter, INT ycenter, UINT r);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_arc_draw_8bpp(GX_DRAW_CONTEXT * context, INT xcenter, INT ycenter, UINT r, INT start_angle,
                                                                                                    INT end_angle);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_arc_fill_8bpp(GX_DRAW_CONTEXT * context, INT xcenter, INT ycenter, UINT r, INT start_angle,
                                                                                                    INT end_angle);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_pie_fill_8bpp(GX_DRAW_CONTEXT * context, INT xcenter, INT ycenter, UINT r, INT start_angle,
                                                                                                    INT end_angle);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_ellipse_draw_8bpp(GX_DRAW_CONTEXT * context, INT xcenter, INT ycenter, INT a, INT b);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_wide_ellipse_draw_8bpp(GX_DRAW_CONTEXT * context, INT xcenter, INT ycenter, INT a, INT b);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_ellipse_fill_8bpp(GX_DRAW_CONTEXT * context, INT xcenter, INT ycenter, INT a, INT b);
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_buffer_toggle_8bpp (GX_CANVAS * canvas, GX_RECTANGLE * dirty);
#endif

/***********************************************************************************************************************
 * Functions
 ***********************************************************************************************************************/
#if (GX_USE_SYNERGY_DRW == 1)

/*******************************************************************************************************************//**
 * @addtogroup SF_EL_GX
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp draw function to close previous frame and set new canvas drawing
 * address. This function is called by GUIX to initiate canvas drawing.
 * @param   display[in]         Pointer to a GUIX display context
 * @param   canvas[in]          Pointer to a GUIX canvas
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_drawing_initiate_8bpp(GX_DISPLAY * display, GX_CANVAS * canvas)
{
    /** Make sure previous display list is done executing. */
    CHECK_DAVE_STATUS(d2_endframe(display -> gx_display_accelerator))

    /** Trigger execution of previous display list, switch to new display list. */
    CHECK_DAVE_STATUS(d2_startframe(display -> gx_display_accelerator))
    CHECK_DAVE_STATUS(d2_framebuffer(display -> gx_display_accelerator,
                                     canvas -> gx_canvas_memory,
                                     (d2_s32)(canvas -> gx_canvas_x_resolution),
                                     (d2_u32)(canvas -> gx_canvas_x_resolution),
                                     (d2_u32)(canvas -> gx_canvas_y_resolution),
                                     d2_mode_alpha8))
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp draw function to complete drawing.
 * @param   display[in]         Pointer to a GUIX display context
 * @param   canvas[in]          Pointer to a GUIX canvas
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_drawing_complete_8bpp(GX_DISPLAY * display, GX_CANVAS * canvas)
{
    /*LDRA_INSPECTED 57 Statement with no side effect. */
    GX_PARAMETER_NOT_USED(canvas);

    d2_device * dave;
    dave = display->gx_display_accelerator;

    /** Set alpha value to opaque to make sure the alpha value would not influence next draw. */
    CHECK_DAVE_STATUS(d2_setalpha(dave, 0xff))
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp horizontal line draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw a horizontal line.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   xstart[in]          x axis start position of a horizontal line in pixel unit
 * @param   xend[in]            x axis end position of a horizontal line in pixel unit
 * @param   ypos[in]            y position of a horizontal line
 * @param   width[in]           Line width in pixel unit
 * @param   color[in]           GUIX color data
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_horizontal_line_8bpp(GX_DRAW_CONTEXT * context,
                               INT xstart, INT xend, INT ypos, INT width, GX_COLOR color)
{
    gx_display_list_flush(context -> gx_draw_context_display);

    d2_device * dave = gx_dave2d_set_clip(context);

    CHECK_DAVE_STATUS(d2_setfillmode(dave, d2_fm_color))
    CHECK_DAVE_STATUS(d2_selectrendermode(dave, d2_rm_solid))
    CHECK_DAVE_STATUS(d2_setblendmode(dave, d2_bm_zero, d2_bm_zero))
    CHECK_DAVE_STATUS(d2_setantialiasing(dave, 0))
    CHECK_DAVE_STATUS(d2_setalpha(dave, (d2_alpha)color))
    CHECK_DAVE_STATUS(d2_renderbox(dave, (d2_point)(D2_FIX4((USHORT)xstart)),
                                         (d2_point)(D2_FIX4((USHORT)ypos)),
                                         (d2_width)(D2_FIX4((USHORT)(xend - xstart) + 1)),
                                         (d2_width)(D2_FIX4((USHORT)width))))
    gx_display_list_open(context -> gx_draw_context_display);

}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp vertical line draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw a vertical line.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   ystart[in]          y axis start position of a vertical line in pixel unit
 * @param   yend[in]            y axis end position of a vertical line in pixel unit
 * @param   xpos[in]            x position of a vertical line
 * @param   width[in]           Line width in pixel unit
 * @param   color[in]           GUIX color data
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_vertical_line_8bpp(GX_DRAW_CONTEXT * context,
                             INT ystart, INT yend, INT xpos, INT width, GX_COLOR color)
{

    d2_device * dave = gx_dave2d_set_clip(context);
    CHECK_DAVE_STATUS(d2_selectrendermode(dave, d2_rm_solid))
    CHECK_DAVE_STATUS(d2_setfillmode(dave, d2_fm_color))
    CHECK_DAVE_STATUS(d2_setantialiasing(dave, 0))
    CHECK_DAVE_STATUS(d2_setblendmode(dave, d2_bm_zero, d2_bm_zero))
    CHECK_DAVE_STATUS(d2_setalpha(dave, (d2_alpha)color))
    CHECK_DAVE_STATUS(d2_renderbox(dave, (d2_point)(D2_FIX4((USHORT)xpos)),
                                         (d2_point)(D2_FIX4((USHORT)ystart)),
                                         (d2_width)(D2_FIX4((USHORT)width)),
                                         (d2_width)(D2_FIX4((USHORT)((yend - ystart) + 1)))))

}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp aliased simple line draw function with D/AVE 2D acceleration
 * enabled. This function is called by GUIX to draw a aliased simple line.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   xstart[in]          x axis start position of a simple line in pixel unit
 * @param   ystart[in]          y axis start position of a simple line in pixel unit
 * @param   xend[in]            x axis end position of a simple line in pixel unit
 * @param   yend[in]            y axis end position of a simple line in pixel unit
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_simple_line_draw_8bpp(GX_DRAW_CONTEXT * context, INT xstart, INT ystart, INT xend, INT yend)
{
    d2_device * dave = gx_dave2d_set_clip(context);
    CHECK_DAVE_STATUS(d2_setantialiasing(dave, 0))
    CHECK_DAVE_STATUS(d2_selectrendermode(dave, d2_rm_solid))
    CHECK_DAVE_STATUS(d2_setblendmode(dave, d2_bm_zero, d2_bm_zero))
    CHECK_DAVE_STATUS(d2_setalpha(dave, (d2_alpha)(context -> gx_draw_context_brush.gx_brush_line_color)))
    CHECK_DAVE_STATUS(d2_renderline(dave, (d2_point)(D2_FIX4((USHORT)xstart)),
                                          (d2_point)(D2_FIX4((USHORT)ystart)),
                                          (d2_point)(D2_FIX4((USHORT)xend)),
                                          (d2_point)(D2_FIX4((USHORT)yend)),
                                          (d2_width)(D2_FIX4((USHORT)context -> gx_draw_context_brush.gx_brush_width)),
                                          d2_le_exclude_none))
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp aliased wide line draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw a draw a aliased wide line.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   xstart[in]          x axis start position of a wide line in pixel unit
 * @param   ystart[in]          y axis start position of a wide line in pixel unit
 * @param   xend[in]            x axis end position of a wide line in pixel unit
 * @param   yend[in]            y axis end position of a wide line in pixel unit
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_simple_wide_line_8bpp(GX_DRAW_CONTEXT * context, INT xstart, INT ystart,
                                INT xend, INT yend)
{

    d2_device * dave = gx_dave2d_set_clip(context);

    CHECK_DAVE_STATUS(d2_setantialiasing(dave, 0))
    CHECK_DAVE_STATUS(d2_selectrendermode(dave, d2_rm_solid))
    CHECK_DAVE_STATUS(d2_setblendmode(dave, d2_bm_zero, d2_bm_zero))
    CHECK_DAVE_STATUS(d2_setalpha(dave, (d2_alpha)(context -> gx_draw_context_brush.gx_brush_line_color)))
    CHECK_DAVE_STATUS(d2_renderline(dave, (d2_point)(D2_FIX4((USHORT)xstart)),
                                          (d2_point)(D2_FIX4((USHORT)ystart)),
                                          (d2_point)(D2_FIX4((USHORT)xend)),
                                          (d2_point)(D2_FIX4((USHORT)yend)),
                                          (d2_width)(D2_FIX4((USHORT)context -> gx_draw_context_brush.gx_brush_width)),
                                          d2_le_exclude_none))

}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp horizontal pattern draw function with D/AVE 2D acceleration
 * enabled. This function is called by GUIX to draw a horizontal pattern line for 8bpp format.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   xstart[in]          x axis start position of a horizontal pattern line in pixel unit
 * @param   xend[in]            x axis end position of a horizontal pattern line in pixel unit
 * @param   ypos[in]            y position of a horizontal pattern line
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_horizontal_pattern_line_draw_8bpp(GX_DRAW_CONTEXT * context, INT xstart, INT xend, INT ypos)
{
    d2_device * dave = gx_dave2d_set_clip(context);

    CHECK_DAVE_STATUS(d2_setantialiasing(dave, 0))
    CHECK_DAVE_STATUS(d2_selectrendermode(dave, d2_rm_solid))
    CHECK_DAVE_STATUS(d2_setpatternsize(dave, 32))
    CHECK_DAVE_STATUS(d2_setblendmode(dave, d2_bm_zero, d2_bm_zero))
    CHECK_DAVE_STATUS(d2_setpattern(dave, context->gx_draw_context_brush.gx_brush_line_pattern))
    CHECK_DAVE_STATUS(d2_setalpha(dave, (d2_alpha)(context -> gx_draw_context_brush.gx_brush_line_color)))
    CHECK_DAVE_STATUS(d2_renderline(dave, (d2_point)(D2_FIX4((USHORT)xstart)),
                                          (d2_point)(D2_FIX4((USHORT)ypos)),
                                          (d2_point)(D2_FIX4((USHORT)xend)),
                                          (d2_point)(D2_FIX4((USHORT)ypos)),
                                          (d2_width)(D2_FIX4((USHORT)context -> gx_draw_context_brush.gx_brush_width)),
                                          d2_le_exclude_none))
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp vertical pattern draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw a vertical pattern line for 8bpp format.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   ystart[in]          y axis start position of a vertical pattern line in pixel unit
 * @param   yend[in]            y axis end position of a vertical pattern line in pixel unit
 * @param   xpos[in]            x position of a vertical pattern line
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_vertical_pattern_line_draw_8bpp(GX_DRAW_CONTEXT * context, INT ystart, INT yend, INT xpos)
{
    d2_device * dave = gx_dave2d_set_clip(context);

    CHECK_DAVE_STATUS(d2_setantialiasing(dave, 0))
    CHECK_DAVE_STATUS(d2_selectrendermode(dave, d2_rm_solid))
    CHECK_DAVE_STATUS(d2_setpatternsize(dave, 32))
    CHECK_DAVE_STATUS(d2_setblendmode(dave, d2_bm_zero, d2_bm_zero))
    CHECK_DAVE_STATUS(d2_setpattern(dave, context->gx_draw_context_brush.gx_brush_line_pattern))
    CHECK_DAVE_STATUS(d2_setalpha(dave, (d2_alpha)(context -> gx_draw_context_brush.gx_brush_line_color)))
    CHECK_DAVE_STATUS(d2_renderline(dave, (d2_point)(D2_FIX4((USHORT)xpos)),
                                          (d2_point)(D2_FIX4((USHORT)ystart)),
                                          (d2_point)(D2_FIX4((USHORT)xpos)),
                                          (d2_point)(D2_FIX4((USHORT)yend)),
                                          (d2_width)(D2_FIX4((USHORT)context -> gx_draw_context_brush.gx_brush_width)),
                                          d2_le_exclude_none))
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp pixelmap draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw a pixelmap. RLE compression is available as an option.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   xpos[in]            x axis position of a pixelmap in pixel unit
 * @param   ypos[in]            y axis position of a pixelmap in pixel unit
 * @param   pixelmap[in]        Pointer to a pixelmap
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_pixelmap_draw_8bpp(GX_DRAW_CONTEXT * context, INT xpos, INT ypos, GX_PIXELMAP * pixelmap)
{
    d2_u32 mode = d2_mode_alpha8;
    gx_display_list_flush(context -> gx_draw_context_display);

    if (pixelmap -> gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
    {
        mode |= d2_mode_rle;
    }

    d2_device * dave = gx_dave2d_set_clip(context);

    d2_setalphablendmode(dave,d2_bm_one, d2_bm_zero);
    d2_setalpha(dave, 0xff);

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *)pixelmap -> gx_pixelmap_data,
                           pixelmap -> gx_pixelmap_width, pixelmap -> gx_pixelmap_width,
                           pixelmap -> gx_pixelmap_height, mode))

    /*LDRA_INSPECTED 96 S D/AVE 2D uses mixed mode arithmetic for the macro d2_bf_no_blitctxbackup. */
    mode = d2_bf_no_blitctxbackup;

    /*LDRA_INSPECTED 96 S D/AVE 2D uses mixed mode arithmetic for the macro d2_bf_usealpha. */
    mode |= d2_bf_usealpha;

    CHECK_DAVE_STATUS(d2_blitcopy(dave,
                         pixelmap -> gx_pixelmap_width,
                         pixelmap -> gx_pixelmap_height,
                         0,
                         0,
                         (d2_point)(D2_FIX4((USHORT)pixelmap -> gx_pixelmap_width)),
                         (d2_point)(D2_FIX4((USHORT)pixelmap -> gx_pixelmap_height)),
                         (d2_point)(D2_FIX4((USHORT)xpos)),
                         (d2_point)(D2_FIX4((USHORT)ypos)),
                         mode))
    gx_display_list_open(context -> gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp polygon draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw a polygon.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   vertex[in]          Pointer to GUIX point data
 * @param   num[in]             Number of points
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_polygon_draw_8bpp(GX_DRAW_CONTEXT * context, GX_POINT * vertex, INT num)
{
    INT loop;
    INT index;
    GX_VALUE val;
    d2_point data[MAX_POLYGON_VERTICES * 2] = {0};
    GX_BRUSH *brush = &context->gx_draw_context_brush;
    d2_color brush_color = brush->gx_brush_line_color;
    UINT temp_style = 0;

    /** Return to caller if the polygon is filled, the outline has already been drawn. */
    if (brush->gx_brush_style & ((UINT)GX_BRUSH_SOLID_FILL|(UINT)GX_BRUSH_PIXELMAP_FILL))
    {
        temp_style = brush->gx_brush_style;
        brush->gx_brush_style = (UINT)(brush->gx_brush_style
                                                & (UINT)(~((UINT)GX_BRUSH_SOLID_FILL|(UINT)GX_BRUSH_PIXELMAP_FILL)));
    }

    /** Convert incoming point data to d2_point type. */
    gx_display_list_flush(context -> gx_draw_context_display);
    index = 0;
    for (loop = 0; loop < num; loop++)
    {
        val = vertex[loop].gx_point_x;
        data[index++] = (d2_point)(D2_FIX4((USHORT)val));
        val = vertex[loop].gx_point_y;
        data[index++] = (d2_point)(D2_FIX4((USHORT)val));
    }

    d2_device * dave = gx_dave2d_set_clip(context);

    /** Make sure anti-aliasing is off. */
    CHECK_DAVE_STATUS(d2_setantialiasing(dave,0))
    CHECK_DAVE_STATUS(d2_selectrendermode(dave, d2_rm_outline))
    CHECK_DAVE_STATUS(d2_setfillmode(dave, d2_fm_color))
    CHECK_DAVE_STATUS(d2_outlinewidth(dave, (d2_width)(D2_FIX4((USHORT)brush->gx_brush_width))))

    CHECK_DAVE_STATUS(d2_setalpha(dave, (d2_alpha)brush_color))

    CHECK_DAVE_STATUS(d2_setblendmode(dave, d2_bm_zero, d2_bm_zero))

    if (brush->gx_brush_style & GX_BRUSH_ROUND)
    {
        CHECK_DAVE_STATUS(d2_setlinejoin(dave, d2_lj_round))
    }
    else
    {
        CHECK_DAVE_STATUS(d2_setlinejoin(dave, d2_lj_miter))
    }

    CHECK_DAVE_STATUS(d2_renderpolygon(dave, (d2_point *)data, (d2_u32)num, 0))

    if(temp_style != 0U)
    {
        brush->gx_brush_style = temp_style ;
    }
    gx_display_list_open(context -> gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp polygon fill function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to fill a polygon.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   vertex[in]          Pointer to GUIX point data
 * @param   num[in]             Number of points
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_polygon_fill_8bpp(GX_DRAW_CONTEXT * context, GX_POINT * vertex, INT num)
{
    gx_display_list_flush(context -> gx_draw_context_display);

    INT loop;
    INT index;
    GX_VALUE val;
    d2_point data[MAX_POLYGON_VERTICES * 2] = { 0 };
    GX_BRUSH * brush = &context->gx_draw_context_brush;
    GX_COLOR brush_color = brush->gx_brush_fill_color;

    /** Convert incoming point data to d2_point type. */
    index = 0;
    for (loop = 0; loop < num; loop++)
    {
        val = vertex[loop].gx_point_x;
        data[index++] = (d2_point)(D2_FIX4((USHORT)val));
        val = vertex[loop].gx_point_y;
        data[index++] = (d2_point)(D2_FIX4((USHORT)val));
    }

    d2_device * dave = gx_dave2d_set_clip(context);
    gx_display_list_flush(context -> gx_draw_context_display);

    GX_VALUE temp_width = brush->gx_brush_width;
    brush->gx_brush_width = 0;

    CHECK_DAVE_STATUS(d2_selectrendermode(dave, d2_rm_solid))

    CHECK_DAVE_STATUS(d2_setantialiasing(dave,0))
    CHECK_DAVE_STATUS(d2_setalpha(dave, 0xff))

    if (brush->gx_brush_style & GX_BRUSH_PIXELMAP_FILL)
    {
        CHECK_DAVE_STATUS(d2_setfillmode(dave, d2_fm_texture))
        gx_dave2d_set_texture_8bpp(context,
                                   dave,
                                   context->gx_draw_context_clip->gx_rectangle_left,
                                   context->gx_draw_context_clip->gx_rectangle_top,
                                   brush->gx_brush_pixelmap);
    }
    else
    {
        CHECK_DAVE_STATUS(d2_setfillmode(dave, d2_fm_color))
        CHECK_DAVE_STATUS(d2_setalpha(dave, (d2_alpha)brush_color))
    }
    CHECK_DAVE_STATUS(d2_setblendmode(dave, d2_bm_zero, d2_bm_zero))
    CHECK_DAVE_STATUS(d2_renderpolygon(dave, (d2_point *)data, (d2_u32)num, 0))

    brush->gx_brush_width = temp_width;
    gx_display_list_open(context -> gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp pixel write function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to write a pixel. Although D/AVE 2D acceleration enabled, the GUIX generic pixel
 * write routine is used since there is no performance benefit for single pixel write operation.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   x[in]               x position in pixel unit
 * @param   y[in]               y position in pixel unit
 * @param   color[in]           GUIX color data
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_pixel_write_8bpp(GX_DRAW_CONTEXT * context, INT x, INT y, GX_COLOR color)
{
    /** Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context -> gx_draw_context_display);

    /** Call the GUIX generic 8bpp pixel write routine. */
    _gx_display_driver_8bpp_pixel_write(context, x, y, color);

    /** Open next display list before we go. */
    gx_display_list_open(context -> gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp block move function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to move a block of pixels within a working canvas memory. Mainly used for fast
 * scrolling.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   block[in]           Pointer to a block to be moved
 * @param   xshift[in]          x axis shift in pixel unit
 * @param   yshift[in]          y axis shift in pixel unit
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_block_move_8bpp(GX_DRAW_CONTEXT * context, GX_RECTANGLE * block, INT xshift, INT yshift)
{
    /** Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context -> gx_draw_context_display);

    /** Call the GUIX generic 8bpp block move routine. */
    _gx_display_driver_8bpp_block_move(context, block, xshift, yshift);

    /** Open next display list before we go. */
    gx_display_list_open(context -> gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp 1-bit raw glyph draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw 1-bit raw glyph.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   draw_area[in]      Pointer to the draw rectangle area
 * @param   map_offset[in]     Mapping offset
 * @param   glyph[in]          Pointer to glyph data
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_glyph_1bit_draw_8bpp(GX_DRAW_CONTEXT * context, GX_RECTANGLE * draw_area, GX_POINT * map_offset,
                                                                                                const GX_GLYPH * glyph)
{
    /** Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context -> gx_draw_context_display);

    /** Call the GUIX generic 8bpp glyph 1bit draw routine. */
    gx_sw_8bpp_glyph_1bit_draw(context, draw_area, map_offset, glyph);

    /** Open next display list before we go. */
    gx_display_list_open(context -> gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp 4-bit raw glyph draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw 4-bit raw glyph.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   draw_area[in]      Pointer to the draw rectangle area
 * @param   map_offset[in]     Mapping offset
 * @param   glyph[in]          Pointer to glyph data
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_glyph_4bit_draw_8bpp(GX_DRAW_CONTEXT * context, GX_RECTANGLE * draw_area, GX_POINT * map_offset,
                                                                                                const GX_GLYPH * glyph)
{
    /** Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context -> gx_draw_context_display);

    /** Call the GUIX generic 8bpp glyph 4bit draw routine. */
    gx_sw_8bpp_glyph_4bit_draw(context, draw_area, map_offset, glyph);

    /** Open next display list before we go. */
    gx_display_list_open(context -> gx_draw_context_display);
}

#if defined(GX_ARC_DRAWING_SUPPORT)
/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp circle outline draw function with D/AVE 2D
 * acceleration enabled. This function is called by GUIX to render a circle outline.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   xcenter[in]        Center pixel position in the horizontal axis
 * @param   ycenter[in]        Center pixel position in the vertical axis
 * @param   r[in]              Radius in pixel unit
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_circle_draw_8bpp(GX_DRAW_CONTEXT * context, INT xcenter, INT ycenter, UINT r)
{
    GX_BRUSH * brush = &context->gx_draw_context_brush;

    /** Return to caller if the polygon is filled, the outline has already been drawn. */
    if (brush->gx_brush_style & ((UINT)GX_BRUSH_SOLID_FILL|(UINT)GX_BRUSH_PIXELMAP_FILL))
    {
        return;
    }

    context->gx_draw_context_clip->gx_rectangle_top =
                                (GX_VALUE)(context->gx_draw_context_clip->gx_rectangle_top - brush->gx_brush_width);
    context->gx_draw_context_clip->gx_rectangle_left =
                                (GX_VALUE)(context->gx_draw_context_clip->gx_rectangle_left - brush->gx_brush_width);
    context->gx_draw_context_clip->gx_rectangle_right =
                                (GX_VALUE)(context->gx_draw_context_clip->gx_rectangle_right + brush->gx_brush_width);
    context->gx_draw_context_clip->gx_rectangle_bottom =
                                (GX_VALUE)(context->gx_draw_context_clip->gx_rectangle_bottom + brush->gx_brush_width);

    d2_device * dave = gx_dave2d_set_clip(context);

    /** Make sure anti-aliasing is off. */
    CHECK_DAVE_STATUS(d2_setantialiasing(dave, 0))
    CHECK_DAVE_STATUS(d2_selectrendermode(dave, d2_rm_outline))
    CHECK_DAVE_STATUS(d2_outlinewidth(dave, (d2_width)(D2_FIX4((USHORT)brush->gx_brush_width))))
    CHECK_DAVE_STATUS(d2_setfillmode(dave, d2_fm_color))
    CHECK_DAVE_STATUS(d2_setalpha(dave, (d2_alpha)(brush->gx_brush_line_color)))

    CHECK_DAVE_STATUS(d2_rendercircle(dave, (d2_point)(D2_FIX4((USHORT)xcenter)),
                                            (d2_point)(D2_FIX4((USHORT)ycenter)),
                                            (d2_width)(D2_FIX4((USHORT)r)),
                                            0))

}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp circle fill function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to fill circle.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   xcenter[in]        Center pixel position in the horizontal axis
 * @param   ycenter[in]        Center pixel position in the vertical axis
 * @param   r[in]              Radius in pixel unit
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_circle_fill_8bpp(GX_DRAW_CONTEXT * context, INT xcenter, INT ycenter, UINT r)
{
    GX_BRUSH  * brush = &context->gx_draw_context_brush;
    GX_COLOR    brush_color = brush->gx_brush_fill_color;

    d2_device * dave = gx_dave2d_set_clip(context);

    context->gx_draw_context_clip->gx_rectangle_top =
                                (GX_VALUE)(context->gx_draw_context_clip->gx_rectangle_top - brush->gx_brush_width);
    context->gx_draw_context_clip->gx_rectangle_left =
                                (GX_VALUE)(context->gx_draw_context_clip->gx_rectangle_left - brush->gx_brush_width);
    context->gx_draw_context_clip->gx_rectangle_right =
                                (GX_VALUE)(context->gx_draw_context_clip->gx_rectangle_right + brush->gx_brush_width);
    context->gx_draw_context_clip->gx_rectangle_bottom =
                                (GX_VALUE)(context->gx_draw_context_clip->gx_rectangle_bottom + brush->gx_brush_width);

    /** Make sure anti-aliasing is off. */
    CHECK_DAVE_STATUS(d2_setantialiasing(dave, 0))

    CHECK_DAVE_STATUS(d2_setalpha(dave, 0xff))

    GX_VALUE temp_width = brush->gx_brush_width;
    brush->gx_brush_width = 0;
    CHECK_DAVE_STATUS(d2_selectrendermode(dave, d2_rm_solid))

    if (brush->gx_brush_style & GX_BRUSH_PIXELMAP_FILL)
    {
        CHECK_DAVE_STATUS(d2_setfillmode(dave, d2_fm_texture))
        gx_dave2d_set_texture_8bpp(context,
                                   dave,
                                   context->gx_draw_context_clip->gx_rectangle_left,
                                   context->gx_draw_context_clip->gx_rectangle_top,
                                   brush->gx_brush_pixelmap);
    }
    else
    {
        CHECK_DAVE_STATUS(d2_setfillmode(dave, d2_fm_color))
        CHECK_DAVE_STATUS(d2_setalpha(dave, (d2_alpha)brush_color))
    }
    CHECK_DAVE_STATUS(d2_setblendmode(dave, d2_bm_zero, d2_bm_zero))
    CHECK_DAVE_STATUS(d2_rendercircle(dave, (d2_point)(D2_FIX4((USHORT)xcenter)),
                                            (d2_point)(D2_FIX4((USHORT)ycenter)),
                                            (d2_width)(D2_FIX4((USHORT)r)),
                                            0))

    brush->gx_brush_width = temp_width;
    if (brush->gx_brush_width > 0)
    {
        UINT brush_style =  brush->gx_brush_style;
        brush->gx_brush_style = (UINT)(brush->gx_brush_style & 
                                                (UINT)(~((UINT)GX_BRUSH_PIXELMAP_FILL|(UINT)GX_BRUSH_SOLID_FILL)));
        _gx_dave2d_circle_draw_8bpp(context, xcenter, ycenter, r);
        brush->gx_brush_style = brush_style;
    }
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp arc draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw arc.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   xcenter[in]        Center pixel position in the horizontal axis
 * @param   ycenter[in]        Center pixel position in the vertical axis
 * @param   r[in]              Radius in pixel unit
 * @param   start_angle[in]    Start angle in degree
 * @param   end_angle[in]      End angle in degree
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_arc_draw_8bpp(GX_DRAW_CONTEXT * context, INT xcenter, INT ycenter, UINT r, INT start_angle,
                                                                                                        INT end_angle)
{
    GX_BRUSH  * brush = &context->gx_draw_context_brush;
    INT         sin1;
    INT         cos1;
    INT         sin2;
    INT         cos2;
    d2_u32      flags;
    UINT        temp_style = 0;

    if (brush->gx_brush_style & ((UINT)GX_BRUSH_SOLID_FILL|(UINT)GX_BRUSH_PIXELMAP_FILL))
    {
        temp_style = brush->gx_brush_style;
        brush->gx_brush_style = (UINT)(brush->gx_brush_style &
                                                    (UINT)(~((UINT)GX_BRUSH_SOLID_FILL|(UINT)GX_BRUSH_PIXELMAP_FILL)));
    }

    context->gx_draw_context_clip->gx_rectangle_top =
                                (GX_VALUE)(context->gx_draw_context_clip->gx_rectangle_top - brush->gx_brush_width);
    context->gx_draw_context_clip->gx_rectangle_left =
                                (GX_VALUE)(context->gx_draw_context_clip->gx_rectangle_left - brush->gx_brush_width);
    context->gx_draw_context_clip->gx_rectangle_right =
                                (GX_VALUE)(context->gx_draw_context_clip->gx_rectangle_right + brush->gx_brush_width);
    context->gx_draw_context_clip->gx_rectangle_bottom =
                                (GX_VALUE)(context->gx_draw_context_clip->gx_rectangle_bottom + brush->gx_brush_width);

    INT s_angle =  - start_angle;
    INT e_angle =  - end_angle;

    sin1 = gx_utility_math_sin((s_angle - 90) * 256);
    cos1 = gx_utility_math_cos((s_angle - 90) * 256);

    sin2 = gx_utility_math_sin((e_angle + 90) * 256);
    cos2 = gx_utility_math_cos((e_angle + 90) * 256);

    /** Set d2_wf_concave flag if the pie object to draw is concave shape. */
    if (((s_angle - e_angle) > 180) || ((s_angle - e_angle) < 0))
    {
        /*LDRA_INSPECTED 96 S D/AVE 2D uses mixed mode arithmetic for the macro d2_wf_concave. */
        flags = d2_wf_concave;
    }
    else
    {
        flags = 0;
    }

    d2_device * dave = gx_dave2d_set_clip(context);

    CHECK_DAVE_STATUS(d2_setantialiasing(dave, 0))
    CHECK_DAVE_STATUS(d2_setfillmode(dave, d2_fm_color))
    CHECK_DAVE_STATUS(d2_selectrendermode(dave, d2_rm_outline))
    CHECK_DAVE_STATUS(d2_outlinewidth(dave, (d2_width)(D2_FIX4((USHORT)brush->gx_brush_width))))
    CHECK_DAVE_STATUS(d2_setalpha(dave, (d2_alpha)(brush->gx_brush_line_color)))
    CHECK_DAVE_STATUS(d2_setblendmode(dave, d2_bm_zero, d2_bm_zero))
    CHECK_DAVE_STATUS(d2_renderwedge(dave, (d2_point)(D2_FIX4((USHORT)xcenter)),
                                           (d2_point)(D2_FIX4((USHORT)ycenter)),
                                           (d2_width)(D2_FIX4((USHORT)r)),
                                           0,
                                           (d2_s32)((UINT)cos1 << 8),
                                           (d2_s32)((UINT)sin1 << 8),
                                           (d2_s32)((UINT)cos2 << 8),
                                           (d2_s32)((UINT)sin2 << 8),
                                           flags))
    if(0 != temp_style)
    {
        brush->gx_brush_style = temp_style;
    }
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp arc fill function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to fill arc. Although D/AVE 2D acceleration enabled, arc fill is done by GUIX generic
 * software draw routine because chord (filled arc) operation is not supported by D/AVE 2D.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   xcenter[in]        Center pixel position in the horizontal axis
 * @param   ycenter[in]        Center pixel position in the vertical axis
 * @param   r[in]              Radius in pixel unit
 * @param   start_angle[in]    Start angle in degree
 * @param   end_angle[in]      End angle in degree
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_arc_fill_8bpp(GX_DRAW_CONTEXT * context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle)
{
    /** D/AVE 2D doesn't support chord (filled arc), so approximate with pie. */
    gx_display_list_flush(context -> gx_draw_context_display);

    GX_BRUSH *brush = &context->gx_draw_context_brush;
    GX_COLOR temp_color = brush->gx_brush_line_color;
    brush->gx_brush_line_color = brush->gx_brush_fill_color;

    _gx_display_driver_generic_arc_fill(context, xcenter, ycenter, r,start_angle, end_angle);
    brush->gx_brush_line_color = temp_color;
    gx_display_list_open(context -> gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp pie fill function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to fill pie.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   xcenter[in]        Center pixel position in the horizontal axis
 * @param   ycenter[in]        Center pixel position in the vertical axis
 * @param   r[in]              Radius in pixel unit
 * @param   start_angle[in]    Start angle in degree
 * @param   end_angle[in]      End angle in degree
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_pie_fill_8bpp(GX_DRAW_CONTEXT * context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle)
{
    GX_BRUSH  * brush = &context->gx_draw_context_brush;
    INT         sin1;
    INT         cos1;
    INT         sin2;
    INT         cos2;
    d2_u32      flags;
    GX_COLOR    brush_color = brush->gx_brush_fill_color;
    d2_device * dave = gx_dave2d_set_clip(context);

    context->gx_draw_context_clip->gx_rectangle_top =
                                (GX_VALUE)(context->gx_draw_context_clip->gx_rectangle_top - brush->gx_brush_width);
    context->gx_draw_context_clip->gx_rectangle_left =
                                (GX_VALUE)(context->gx_draw_context_clip->gx_rectangle_left - brush->gx_brush_width);
    context->gx_draw_context_clip->gx_rectangle_right =
                                (GX_VALUE)(context->gx_draw_context_clip->gx_rectangle_right + brush->gx_brush_width);
    context->gx_draw_context_clip->gx_rectangle_bottom =
                                (GX_VALUE)(context->gx_draw_context_clip->gx_rectangle_bottom + brush->gx_brush_width);

    INT s_angle =  - start_angle;
    INT e_angle =  - end_angle;

    sin1 = gx_utility_math_sin((s_angle - 90) * 256);
    cos1 = gx_utility_math_cos((s_angle - 90) * 256);

    sin2 = gx_utility_math_sin((e_angle + 90) * 256);
    cos2 = gx_utility_math_cos((e_angle + 90) * 256);

    /** Set d2_wf_concave flag if the pie object to draw is concave shape. */
    if (((s_angle - e_angle) > 180) || ((s_angle - e_angle) < 0))
    {
        /*LDRA_INSPECTED 96 S D/AVE 2D uses mixed mode arithmetic for the macro d2_wf_concave. */
        flags = d2_wf_concave;
    }
    else
    {
        flags = 0;
    }

    CHECK_DAVE_STATUS(d2_setalpha(dave, 0xff))
    CHECK_DAVE_STATUS(d2_setantialiasing(dave, 0))
    CHECK_DAVE_STATUS(d2_setblendmode(dave, d2_bm_zero, d2_bm_zero))

    GX_VALUE temp_width = brush->gx_brush_width;
    brush->gx_brush_width = 0;
    CHECK_DAVE_STATUS(d2_selectrendermode(dave, d2_rm_solid))

    if (brush->gx_brush_style & GX_BRUSH_PIXELMAP_FILL)
    {
        CHECK_DAVE_STATUS(d2_setfillmode(dave, d2_fm_texture))
        gx_dave2d_set_texture_8bpp(context,
                                   dave,
                                   context->gx_draw_context_clip->gx_rectangle_left,
                                   context->gx_draw_context_clip->gx_rectangle_top,
                                   brush->gx_brush_pixelmap);
    }
    else
    {
        CHECK_DAVE_STATUS(d2_setfillmode(dave, d2_fm_color))
        CHECK_DAVE_STATUS(d2_setalpha(dave, (d2_alpha)(brush_color)))
    }
    CHECK_DAVE_STATUS(d2_setblendmode(dave, d2_bm_zero, d2_bm_zero))
    CHECK_DAVE_STATUS(d2_renderwedge(dave, (d2_point)(D2_FIX4((USHORT)xcenter)),
                                           (d2_point)(D2_FIX4((USHORT)ycenter)),
                                           (d2_point)(D2_FIX4((USHORT)r)),
                                           0,
                                           (d2_s32)((UINT)cos1 << 8),
                                           (d2_s32)((UINT)sin1 << 8),
                                           (d2_s32)((UINT)cos2 << 8),
                                           (d2_s32)((UINT)sin2 << 8),
                                           flags))
    brush->gx_brush_width = temp_width;
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp ellipse draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw aliased ellipse. Although D/AVE 2D acceleration enabled, ellipse draw is
 * done by GUIX generic software draw routine because the operation is not supported by D/AVE 2D.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   xcenter[in]        Center pixel position in the horizontal axis
 * @param   ycenter[in]        Center pixel position in the vertical axis
 * @param   a[in]              Semi-major axis
 * @param   b[in]              Semi-minor axis
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_ellipse_draw_8bpp(GX_DRAW_CONTEXT * context, INT xcenter, INT ycenter, INT a, INT b)
{
    /** Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context -> gx_draw_context_display);

    /** Call the GUIX generic ellipse draw routine. */
    _gx_display_driver_generic_ellipse_draw(context, xcenter, ycenter, a, b);

    /** Open next display list before we go. */
    gx_display_list_open(context -> gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp wide ellipse draw function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to draw aliased ellipse. Although D/AVE 2D acceleration enabled, ellipse draw is
 * done by GUIX generic software draw routine because the operation is not supported by D/AVE 2D.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   xcenter[in]        Center pixel position in the horizontal axis
 * @param   ycenter[in]        Center pixel position in the vertical axis
 * @param   a[in]              Semi-major axis
 * @param   b[in]              Semi-minor axis
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_wide_ellipse_draw_8bpp(GX_DRAW_CONTEXT * context, INT xcenter, INT ycenter, INT a, INT b)
{
    /** Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context -> gx_draw_context_display);

    /** Call the GUIX generic wide ellipse draw routine. */
    _gx_display_driver_generic_wide_ellipse_draw(context, xcenter, ycenter, a, b);

    /** Open next display list before we go. */
    gx_display_list_open(context -> gx_draw_context_display);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp ellipse fill function with D/AVE 2D acceleration enabled.
 * This function is called by GUIX to perform ellipse fill. Although D/AVE 2D acceleration enabled, ellipse fill is done
 * by GUIX generic software draw routine.
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   xcenter[in]        Center pixel position in the horizontal axis
 * @param   ycenter[in]        Center pixel position in the vertical axis
 * @param   a[in]              Semi-major axis
 * @param   b[in]              Semi-minor axis
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_ellipse_fill_8bpp(GX_DRAW_CONTEXT * context, INT xcenter, INT ycenter, INT a, INT b)
{
    /** Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context -> gx_draw_context_display);

    /** Call the GUIX generic ellipse fill routine. */
    _gx_display_driver_generic_ellipse_fill(context, xcenter, ycenter, a, b);

    /** Open next display list before we go. */
    gx_display_list_open(context -> gx_draw_context_display);
}
#endif  /* is GUIX arc drawing support enabled? */

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, D/AVE 2D accelerated 8bpp draw function to toggle frame buffer.
 *  This function performs copies canvas memory to working frame buffer if a canvas is used, performs sequence of canvas
 *  refresh drawing commands, toggles frame buffer, and finally copies visible frame buffer to working frame buffer or
 *  copes canvas to working buffer if a canvas is used. This function is called by GUIX if D/AVE 2D hardware rendering
 *  acceleration is enabled.
 * @param   canvas[in]         Pointer to a GUIX canvas
 * @param   dirty[in]          Pointer to a dirty rectangle area
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_dave2d_buffer_toggle_8bpp (GX_CANVAS * canvas, GX_RECTANGLE * dirty)
{
    /*LDRA_INSPECTED 57 Statement with no side effect. */
    GX_PARAMETER_NOT_USED(dirty);

    GX_RECTANGLE    Limit;
    GX_RECTANGLE    Copy;
    GX_DISPLAY    * display;
    INT             rotation_angle;

    display = canvas->gx_canvas_display;
    rotation_angle = sf_el_gx_display_rotation_get(display->gx_display_handle);
    sf_el_gx_frame_pointers_get(display->gx_display_handle, &visible_frame, &working_frame);

    _gx_utility_rectangle_define(&Limit, 0, 0,
                                 (GX_VALUE)(canvas->gx_canvas_x_resolution - 1),
                                 (GX_VALUE)(canvas->gx_canvas_y_resolution - 1));

    if (canvas->gx_canvas_memory != (GX_COLOR *)working_frame)
    {
        if (_gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &Copy))
        {
            gx_dave2d_rotate_canvas_to_working_8bpp(canvas, &Copy, rotation_angle);
        }
    }

    gx_display_list_flush(display);
    gx_display_list_open(display);

    sf_el_gx_frame_toggle(canvas->gx_canvas_display->gx_display_handle, (GX_UBYTE **) &visible_frame);
    sf_el_gx_frame_pointers_get(canvas->gx_canvas_display->gx_display_handle, &visible_frame, &working_frame);

    /* If canvas memory is pointing directly to frame buffer, toggle canvas memory */
    if (canvas->gx_canvas_memory == (GX_COLOR *) visible_frame)
    {
        canvas->gx_canvas_memory = (GX_COLOR *) working_frame;
    }

    if (_gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &Copy))
    {
        if (canvas->gx_canvas_memory == (GX_COLOR *)working_frame)
        {
            /* Copies our canvas to the back buffer */
            gx_dave2d_copy_visible_to_working_8bpp(canvas, &Copy);
        }
        else
        {
            gx_dave2d_rotate_canvas_to_working_8bpp(canvas, &Copy, rotation_angle);
        }
    }
}
#endif /* GX_USE_SYNERGY_DRW */

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8-bit Palette setup for display hardware.
 * This function is called by GUIX.
 * @param   display[in]        Pointer to a GUIX display
 * @param   palette[in]        Pointer to a GUIX color palette entries
 * @param   count[in]          Number of palette entries.
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_display_driver_8bit_palette_assign(GX_DISPLAY * display, GX_COLOR * palette, INT count)
{
    display -> gx_display_palette = palette;
    display -> gx_display_palette_size = (UINT)count;

    sf_el_gx_display_8bit_palette_assign(display->gx_display_handle);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, Software 8bpp draw function to toggle frame buffer.
 *  This function performs copies canvas memory to working frame buffer if a canvas is used, then toggles frame buffer,
 *  finally copies visible frame buffer to working frame buffer. This function is called by GUIX if hardware rendering
 *  acceleration is not enabled.
 * @param   canvas[in]         Pointer to a GUIX canvas
 * @param   dirty[in]          Pointer to a dirty rectangle area
 **********************************************************************************************************************/
/*LDRA_INSPECTED 219 S GUIX defined functions start with underscore. */
VOID _gx_synergy_buffer_toggle_8bpp(GX_CANVAS * canvas, GX_RECTANGLE * dirty)
{
    /*LDRA_INSPECTED 57 Statement with no side effect. */
    GX_PARAMETER_NOT_USED(dirty);

    GX_RECTANGLE Limit;
    GX_RECTANGLE copy;
    GX_DISPLAY * display;
    INT rotation_angle;

    display = canvas->gx_canvas_display;

    _gx_utility_rectangle_define(&Limit, 0, 0,
                                 (GX_VALUE)(canvas->gx_canvas_x_resolution - 1),
                                 (GX_VALUE)(canvas->gx_canvas_y_resolution - 1));

    rotation_angle = sf_el_gx_display_rotation_get(display->gx_display_handle);
    sf_el_gx_frame_pointers_get(display->gx_display_handle, &visible_frame, &working_frame);

    if(canvas->gx_canvas_memory != (GX_COLOR *)working_frame)
    {
        if (_gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &copy))
        {
            gx_rotate_canvas_to_working_8bpp(canvas, &copy, rotation_angle);
        }
    }

    sf_el_gx_frame_toggle(canvas->gx_canvas_display->gx_display_handle, &visible_frame);
    sf_el_gx_frame_pointers_get(canvas->gx_canvas_display->gx_display_handle, &visible_frame, &working_frame);

    if (canvas->gx_canvas_memory == (GX_COLOR *) visible_frame)
    {
        canvas->gx_canvas_memory = (GX_COLOR *) working_frame;
    }

    if (_gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &copy))
    {
        if(canvas->gx_canvas_memory == (GX_COLOR *) working_frame)
        {
            gx_copy_visible_to_working_8bpp(canvas, &copy);
        }
        else
        {
            gx_rotate_canvas_to_working_8bpp(canvas, &copy, rotation_angle);
        }
    }
}


#if (GX_USE_SYNERGY_DRW == 1)
/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, support function used to apply texture source for all shape drawing.
 * @param   context[in]         Pointer to a GUIX draw context
 * @param   dave[in]            Pointer to D/AVE 2D device context
 * @param   xpos[in]            X position in pixel unit
 * @param   ypos[in]            y position in pixel unit
 * @param   map[in]             Pointer to GUIX pixelmap
 **********************************************************************************************************************/
static VOID gx_dave2d_set_texture_8bpp(GX_DRAW_CONTEXT * context, d2_device * dave, INT xpos, INT ypos,
                                                                                                    GX_PIXELMAP * map)
{
    /*LDRA_INSPECTED 57 Statement with no side effect. */
    GX_PARAMETER_NOT_USED(context);

    d2_u32 format = d2_mode_alpha8;

    if (map->gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
    {
        format |= d2_mode_rle;
    }

    CHECK_DAVE_STATUS(d2_settexture(dave,
            (void *) map->gx_pixelmap_data,
            map->gx_pixelmap_width,
            map->gx_pixelmap_width,
            map->gx_pixelmap_height,
            format))

    CHECK_DAVE_STATUS(d2_settextureoperation(dave, d2_to_copy, d2_to_zero, d2_to_zero, d2_to_zero));
    CHECK_DAVE_STATUS(d2_settexelcenter(dave, 0, 0))
    CHECK_DAVE_STATUS(d2_settexturemapping(dave,
                                            (d2_point)((UINT)xpos << 4),
                                            (d2_point)((UINT)ypos << 4),
                                            0,
                                            0,
                                            (d2_s32)(1U << 16),
                                            0,
                                            0,
                                            (d2_s32)(1U << 16)))
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, D/AVE 2D 8bpp draw function sub routine.
 * This function copies canvas memory to frame buffer with screen rotation.
 * @param[in]     canvas             Pointer to a GUIX canvas
 * @param[in]     copy               Pointer to a rectangle area to be copied
 * @param[in]     rotation_angle     Rotation angle (0, 90, 180 or 270)
 **********************************************************************************************************************/
static VOID gx_dave2d_rotate_canvas_to_working_8bpp(GX_CANVAS * canvas, GX_RECTANGLE * copy, INT rotation_angle)
{
    GX_RECTANGLE display_size;
    d2_u32 mode;
    d2_rotation_param_t param = {0};

    uint8_t   *pGetRow;

    GX_DISPLAY * display = canvas->gx_canvas_display;
    d2_device * dave = (d2_device *) display->gx_display_accelerator;

    d2_u8 fillmode_bkup = d2_getfillmode(dave);

    _gx_utility_rectangle_define(&display_size, 0, 0,
            (GX_VALUE)(display->gx_display_width - 1),
            (GX_VALUE)(display->gx_display_height - 1));

    param.copy_clip = *copy;

    /** Align copy region on 32-bit boundary. */
    param.copy_clip.gx_rectangle_left   = (GX_VALUE)((USHORT)param.copy_clip.gx_rectangle_left & 0xfffcU);
    param.copy_clip.gx_rectangle_top    = (GX_VALUE)((USHORT)param.copy_clip.gx_rectangle_top  & 0xfffcU);
    param.copy_clip.gx_rectangle_right  = (GX_VALUE)((USHORT)param.copy_clip.gx_rectangle_right  | 3U);
    param.copy_clip.gx_rectangle_bottom = (GX_VALUE)((USHORT)param.copy_clip.gx_rectangle_bottom | 3U);
    mode = d2_mode_alpha8;

    d2_setalphablendmode(dave,d2_bm_one,d2_bm_zero);
    d2_setalpha(dave,0xff);

    /** Offset canvas within frame buffer. */
    _gx_utility_rectangle_shift(&param.copy_clip,
                                canvas->gx_canvas_display_offset_x,
                                canvas->gx_canvas_display_offset_y);

    _gx_utility_rectangle_overlap_detect(&param.copy_clip, &display_size, &param.copy_clip);
    param.copy_width  = (INT)(param.copy_clip.gx_rectangle_right  - param.copy_clip.gx_rectangle_left) + 1;
    param.copy_height = (INT)(param.copy_clip.gx_rectangle_bottom - param.copy_clip.gx_rectangle_top) + 1;

    if ((param.copy_width <= 0) || (param.copy_height <= 0))
    {
        return;
    }

    pGetRow = (uint8_t *) canvas->gx_canvas_memory;
    pGetRow = pGetRow + 
        (INT)((canvas->gx_canvas_display_offset_y + param.copy_clip.gx_rectangle_top) * display->gx_display_width);
    pGetRow = pGetRow + (INT)(canvas->gx_canvas_display_offset_x + param.copy_clip.gx_rectangle_left);

    /** Set parameters for the screen rotation. */
    param.angle = rotation_angle;
    gx_dave2d_rotate_canvas_to_working_param_set (&param, display, canvas);

    CHECK_DAVE_STATUS(d2_framebuffer(dave,
                                     (uint16_t *) working_frame,
                                     (d2_s32)param.x_resolution,
                                     (d2_u32)param.x_resolution,
                                     (d2_u32)param.y_resolution,
                                     (d2_s32)mode));

    CHECK_DAVE_STATUS(d2_cliprect(dave, param.xmin, param.ymin, param.xmax, param.ymax));

    CHECK_DAVE_STATUS(d2_setfillmode(dave, d2_fm_texture));

    CHECK_DAVE_STATUS(d2_settexture(dave, pGetRow, display->gx_display_width, param.copy_width, param.copy_height, mode));

    CHECK_DAVE_STATUS(d2_settexturemode(dave, 0));
    CHECK_DAVE_STATUS(d2_settextureoperation(dave, d2_to_copy, d2_to_zero, d2_to_zero, d2_to_zero));
    CHECK_DAVE_STATUS(d2_settexelcenter(dave, 0, 0));

    CHECK_DAVE_STATUS(d2_settexturemapping(dave,
                                           (d2_point)D2_FIX4((UINT)param.x_texture_zero),
                                           (d2_point)D2_FIX4((UINT)param.y_texture_zero),
                                           0, 0,
                                           param.dxu, param.dxv, param.dyu, param.dyv));

    if ( (0 == rotation_angle) || (180 == rotation_angle))
    {
        CHECK_DAVE_STATUS(d2_renderbox(dave,
                        (d2_point)D2_FIX4((USHORT)param.xmin),
                        (d2_point)D2_FIX4((USHORT)param.ymin),
                        (d2_width)D2_FIX4((UINT)param.copy_width_rotated),
                        (d2_width)D2_FIX4((UINT)param.copy_height_rotated)));
    }
    else
    {
        /** Render vertical stripes multiple times to reduce the texture cache miss for
         * 90 or 270 degree rotation. */
        for(INT x = param.xmin; x < (param.xmin + param.copy_width_rotated); x++)
        {
            CHECK_DAVE_STATUS(d2_renderbox(dave,
                            (d2_point)D2_FIX4((UINT)x),
                            (d2_point)D2_FIX4((USHORT)param.ymin),
                            (d2_width)D2_FIX4(1U),
                            (d2_width)D2_FIX4((UINT)param.copy_height_rotated)));
        }
    }

    d2_setfillmode(dave, fillmode_bkup);
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, D/AVE 2D 8bpp draw function sub routine to copy visible frame buffer to
 * working frame buffer. This function is called by _gx_dave2d_buffer_toggle_8bpp to perform image data copy between
 * frame buffers after buffer toggle operation.
 * @param   canvas[in]         Pointer to a GUIX canvas
 * @param   copy[in]           Pointer to a rectangle area to be copied
 **********************************************************************************************************************/
static VOID gx_dave2d_copy_visible_to_working_8bpp(GX_CANVAS * canvas, GX_RECTANGLE * copy)
{
    GX_RECTANGLE display_size;
    GX_RECTANGLE copy_clip;

    ULONG      * pGetRow;
    ULONG      * pPutRow;

    INT          copy_width;
    INT          copy_height;

    GX_DISPLAY * display = canvas->gx_canvas_display;
    d2_device * dave = (d2_device *) display->gx_display_accelerator;

    _gx_utility_rectangle_define(&display_size, 0, 0,
            (GX_VALUE)(display->gx_display_width - 1),
            (GX_VALUE)(display->gx_display_height - 1));
    copy_clip = *copy;

    copy_clip.gx_rectangle_left   = (GX_VALUE)((USHORT)copy_clip.gx_rectangle_left & 0xfffcU);
    copy_clip.gx_rectangle_top    = (GX_VALUE)((USHORT)copy_clip.gx_rectangle_top  & 0xfffcU);
    copy_clip.gx_rectangle_right  = (GX_VALUE)((USHORT)copy_clip.gx_rectangle_right  | 3U);
    copy_clip.gx_rectangle_bottom = (GX_VALUE)((USHORT)copy_clip.gx_rectangle_bottom | 3U);

    /** Offset canvas within frame buffer. */
    _gx_utility_rectangle_shift(&copy_clip, canvas->gx_canvas_display_offset_x, canvas->gx_canvas_display_offset_y);

    _gx_utility_rectangle_overlap_detect(&copy_clip, &display_size, &copy_clip);
    copy_width  = (copy_clip.gx_rectangle_right - copy_clip.gx_rectangle_left) + 1;
    copy_height = (copy_clip.gx_rectangle_bottom - copy_clip.gx_rectangle_top) + 1;

    if ((copy_width <= 0) || (copy_height <= 0))
    {
        return;
    }

    pGetRow = (ULONG *) visible_frame;
    pPutRow = (ULONG *) working_frame;

    d2_setalphablendmode(dave,d2_bm_one, d2_bm_zero);
    d2_setalpha(dave,0xff);

    CHECK_DAVE_STATUS(d2_framebuffer(dave, pPutRow,
                      (d2_u32)(canvas -> gx_canvas_x_resolution), (d2_u32)(canvas -> gx_canvas_x_resolution),
                      (d2_u32)(canvas -> gx_canvas_y_resolution), d2_mode_alpha8))

    CHECK_DAVE_STATUS(d2_cliprect(dave,
                      copy_clip.gx_rectangle_left,
                      copy_clip.gx_rectangle_top,
                      copy_clip.gx_rectangle_right,
                      copy_clip.gx_rectangle_bottom))

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *) pGetRow,
                      canvas->gx_canvas_x_resolution,
                      canvas->gx_canvas_x_resolution,
                      canvas->gx_canvas_y_resolution,
                      d2_mode_alpha8))

    CHECK_DAVE_STATUS(d2_blitcopy(dave,
                                  copy_width, copy_height,
                                  (d2_blitpos)(copy_clip.gx_rectangle_left),
                                  (d2_blitpos)(copy_clip.gx_rectangle_top),
                                  (d2_width)(D2_FIX4((UINT)copy_width)),
                                  (d2_width)(D2_FIX4((UINT)copy_height)),
                                  (d2_point)(D2_FIX4((USHORT)copy_clip.gx_rectangle_left)),
                                  (d2_point)(D2_FIX4((USHORT)copy_clip.gx_rectangle_top)),
                                  /*LDRA_INSPECTED 96 S Mixed mode arithmetic is used for d2_bf_no_blitctxbackup. */
                                  /*LDRA_INSPECTED 96 S Mixed mode arithmetic is used for the macro d2_bf_usealpha. */
                                  (d2_u32)d2_bf_no_blitctxbackup | (d2_u32)d2_bf_usealpha))

    CHECK_DAVE_STATUS(d2_endframe(display -> gx_display_accelerator))
    CHECK_DAVE_STATUS(d2_startframe(display -> gx_display_accelerator))
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp 4-bit raw glyph software draw function.
 * This function is called by _gx_dave2d_glyph_4bit_draw_8bpp().
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   draw_area[in]      Pointer to the draw rectangle area
 * @param   map_offset[in]     Mapping offset
 * @param   glyph[in]          Pointer to glyph data
 **********************************************************************************************************************/
static VOID gx_sw_8bpp_glyph_4bit_draw(GX_DRAW_CONTEXT * context, GX_RECTANGLE * draw_area, GX_POINT * map_offset,
                                                                                            const GX_GLYPH * glyph)
{
    GX_UBYTE  * glyph_row;
    GX_UBYTE  * glyph_data_ptr;
    UINT        pixel_width = 0;
    UINT        leading_pixel;
    UINT        trailing_pixel;
    GX_UBYTE    text_color;
    UINT        y_height;
    GX_UBYTE    glyph_data;
    UINT        pitch;
    GX_UBYTE  * put;
    GX_UBYTE  * draw_start;

    draw_start = (GX_UBYTE *)context->gx_draw_context_memory;
    draw_start += context->gx_draw_context_pitch * draw_area->gx_rectangle_top;
    draw_start += draw_area->gx_rectangle_left;

    text_color  = (GX_UBYTE)(context->gx_draw_context_brush.gx_brush_line_color + 15);
    pixel_width = ((UINT)draw_area->gx_rectangle_right - (UINT)draw_area->gx_rectangle_left) + 1U;

    /* Find the width of the glyph */
    pitch = glyph->gx_glyph_width;

    /* Make it byte-aligned. */
    pitch = (pitch + 1) >> 1;

    glyph_row = (GX_UBYTE *)glyph->gx_glyph_map;

    if (map_offset->gx_point_y)
    {
        glyph_row = (GX_UBYTE *)(glyph_row + ((GX_VALUE)pitch * map_offset->gx_point_y));
    }

    glyph_row += ((UINT)map_offset->gx_point_x >> 1);
    y_height = ((UINT)draw_area->gx_rectangle_bottom - (UINT)draw_area->gx_rectangle_top) + 1U;
    leading_pixel = (UINT)map_offset->gx_point_x & 1U;
    pixel_width -= leading_pixel;
    trailing_pixel = pixel_width & 1;
    pixel_width = pixel_width >> 1;

    for (UINT row = 0U; row < y_height; row++)
    {
        glyph_data_ptr = glyph_row;
        put = draw_start;

        if (leading_pixel)
        {
            glyph_data = *glyph_data_ptr;
            ++glyph_data_ptr;
            glyph_data = glyph_data >> 4;
            *put = (GX_UBYTE)(text_color - glyph_data);
            ++put;
        }
        for (UINT index = 0U; index < pixel_width; index++)
        {
            glyph_data = *glyph_data_ptr;
            ++glyph_data_ptr;

            *put = (GX_UBYTE)(text_color - (glyph_data & 0x0FU));
            ++put;
            *put = (GX_UBYTE)(text_color - (glyph_data >> 4));
            ++put;
        }

        if (trailing_pixel)
        {
            glyph_data = *glyph_data_ptr & 0x0F;
            *put = (GX_UBYTE)(text_color - glyph_data);
            ++put;
        }
        glyph_row  += pitch;
        draw_start += context->gx_draw_context_pitch;
    }
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, sub-routine for 8bpp 1-bit raw glyph software draw. Compute the number of
 * useful bytes from the glyph data.
 * This function is called by gx_sw_8bpp_glyph_1bit_draw().
 * @param   draw_area[in]               Pointer to a GUIX draw area
 * @param   p_map_offset[in]            Pointer to pixel offset coordinate
 * @param   p_pixel_in_first_byte[out]  Pointer to output pixel in first byte
 * @param   p_pixel_in_last_byte[out]   Pointer to output pixel in last byte
 * @param   p_init_mask[out]            Pointer to output initial mask value
 * @retval  num_bytes                   Number of useful bytes
 **********************************************************************************************************************/
static UINT gx_sw_8bpp_glyph_1bit_draw_get_valid_bytes (GX_RECTANGLE * draw_area, GX_POINT * p_map_offset,
                                                        UINT * p_pixel_in_first_byte, UINT * p_pixel_in_last_byte,
                                                        GX_UBYTE * p_init_mask)
{
    UINT    num_bytes;
    UINT    pixel_per_row;
    UINT    pixel_in_first_byte;
    UINT    pixel_in_last_byte = 0U;

    pixel_per_row = ((UINT)draw_area->gx_rectangle_right - (UINT)draw_area->gx_rectangle_left) + 1U;

    /** Compute the number of useful bytes from the glyph this routine is going to use.
        Because of map_offset, the first byte may contain pixel bits we don't need to draw,
        and the width of the draw_area may produce part of the last byte in the row to be ignored. */
    num_bytes = (UINT)(((UINT)p_map_offset->gx_point_x + pixel_per_row + 7U) >> 3);

    /** Take into account if map_offset specifies the number of bytes to ignore from the beginning of the row. */
    num_bytes = num_bytes - ((UINT)p_map_offset->gx_point_x >> 3);

    /** Compute the number of pixels to draw from the first byte of the glyph data. */
    pixel_in_first_byte = (UINT)8U - ((UINT)p_map_offset->gx_point_x & (UINT)0x7U);
    *p_init_mask = (GX_UBYTE)(0x80U >> (pixel_in_first_byte - 1));

    /** Compute the number of pixels to draw from the last byte, if there are more than one byte in a row. */
    if (num_bytes != 1U)
    {
        pixel_in_last_byte = ((UINT)(p_map_offset->gx_point_x) + pixel_per_row) & 0x7;
        if (pixel_in_last_byte == 0U)
        {
            pixel_in_last_byte = 8U;
        }
    }
    else
    {
        if (((UINT)(p_map_offset->gx_point_x) + pixel_per_row) < 8U)
        {
            pixel_in_first_byte = pixel_per_row;
        }
        else
        {
            pixel_in_last_byte = 0U;
        }
    }

    *p_pixel_in_first_byte = pixel_in_first_byte;
    *p_pixel_in_last_byte  = pixel_in_last_byte;

    return num_bytes;
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp 1-bit raw glyph software draw function.
 * This function is called by _gx_dave2d_glyph_1bit_draw_8bpp().
 * @param   context[in]        Pointer to a GUIX draw context
 * @param   draw_area[in]      Pointer to the draw rectangle area
 * @param   map_offset[in]     Mapping offset
 * @param   glyph[in]          Pointer to glyph data
 **********************************************************************************************************************/
static VOID gx_sw_8bpp_glyph_1bit_draw(GX_DRAW_CONTEXT * context, GX_RECTANGLE * draw_area, GX_POINT * map_offset,
                                                                                            const GX_GLYPH * glyph)
{
    GX_UBYTE  * glyph_row;
    GX_UBYTE  * glyph_data_ptr;
    UINT        pixel_in_first_byte;
    UINT        pixel_in_last_byte = 0U;
    GX_UBYTE    text_color;
    UINT        y_height;
    GX_UBYTE    glyph_data;
    UINT        glyph_width;
    GX_UBYTE  * put;
    UINT        num_bytes;
    UINT        num_bits;
    GX_UBYTE  * line_start;
    GX_UBYTE    mask;
    GX_UBYTE    init_mask;

    text_color = (GX_UBYTE)context->gx_draw_context_brush.gx_brush_line_color;

    num_bytes = gx_sw_8bpp_glyph_1bit_draw_get_valid_bytes (draw_area,
                                                            map_offset,
                                                            &pixel_in_first_byte,
                                                            &pixel_in_last_byte,
                                                            &init_mask);

    /** Find the width of the glyph, in terms of bytes. */
    glyph_width = glyph->gx_glyph_width;

    /** Make it byte-aligned. */
    glyph_width = (glyph_width + 7) >> 3;

    glyph_row = (GX_UBYTE *)glyph->gx_glyph_map;

    if (map_offset->gx_point_y)
    {
        glyph_row = glyph_row + ((GX_VALUE)glyph_width * map_offset->gx_point_y);
    }

    glyph_row = (GX_UBYTE *)((ULONG)glyph_row + ((ULONG)map_offset->gx_point_x >> 3));

    y_height = ((UINT)draw_area->gx_rectangle_bottom - (UINT)draw_area->gx_rectangle_top) + 1U;

    line_start = (GX_UBYTE *)context->gx_draw_context_memory;
    line_start += context->gx_draw_context_pitch * (draw_area->gx_rectangle_top);
    line_start += draw_area->gx_rectangle_left;

    for (UINT row = 0U; row < y_height; row++)
    {
        glyph_data_ptr = glyph_row;
        glyph_data = *glyph_data_ptr;
        mask = init_mask;
        num_bits = pixel_in_first_byte;
        put = line_start;
        for (UINT i = 0U; i < num_bytes; i++)
        {
            if ((i == (num_bytes - 1U)) && (num_bytes > 1U))
            {
                num_bits = pixel_in_last_byte;
            }

            for (UINT j = 0U; j < num_bits; j++)
            {
                DRAW_PIXEL;
            }

            glyph_data_ptr++;
            glyph_data = *(glyph_data_ptr);
            num_bits = 8U;
            mask = 0x01U;
        }

        glyph_row += glyph_width;
        line_start += context->gx_draw_context_pitch;
    }
}

#endif /* GX_USE_SYNERGY_DRW */

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, Frame buffer toggle operation with copying data by software without
 * D/AVE 2D acceleration and screen rotation.
 * This function is called by _gx_synergy_buffer_toggle_8bpp().
 * @param[in]     canvas            Pointer to a canvas
 * @param[in]     copy              Pointer to a rectangle region to copy
 **********************************************************************************************************************/
static VOID gx_copy_visible_to_working_8bpp(GX_CANVAS * canvas, GX_RECTANGLE * copy)
{
    GX_RECTANGLE display_size;
    GX_RECTANGLE copy_clip;

    ULONG        *pGetRow;
    ULONG        *pPutRow;

    INT          copy_width;
    INT          copy_height;
    INT          canvas_stride;
    INT          display_stride;

    ULONG        *pGet;
    ULONG        *pPut;
    INT          row;
    INT          col;

    GX_DISPLAY * display = canvas->gx_canvas_display;

    _gx_utility_rectangle_define(&display_size, 0, 0,
            (GX_VALUE)(display->gx_display_width - 1),
            (GX_VALUE)(display->gx_display_height - 1));
    copy_clip = *copy;

    /** Align copy region on 32-bit boundary. */
    copy_clip.gx_rectangle_left  = (GX_VALUE)((USHORT)copy_clip.gx_rectangle_left & 0xfffcU);
    copy_clip.gx_rectangle_right = (GX_VALUE)((USHORT)copy_clip.gx_rectangle_right | 3U);

    /** Offset canvas within frame buffer. */
    _gx_utility_rectangle_shift(&copy_clip, canvas->gx_canvas_display_offset_x, canvas->gx_canvas_display_offset_y);

    _gx_utility_rectangle_overlap_detect(&copy_clip, &display_size, &copy_clip);
    copy_width  = (INT)(copy_clip.gx_rectangle_right - copy_clip.gx_rectangle_left) + 1;
    copy_height = (INT)(copy_clip.gx_rectangle_bottom - copy_clip.gx_rectangle_top) + 1;

    if ((copy_width <= 0) || (copy_height <= 0))
    {
        return;
    }

    pGetRow = (ULONG *) visible_frame;
    pPutRow = (ULONG *) working_frame;

    copy_width /= 4;
    canvas_stride = (INT)canvas->gx_canvas_x_resolution / 4;
    pPutRow = pPutRow + ((INT)copy_clip.gx_rectangle_top * canvas_stride);
    pPutRow = pPutRow + ((INT)copy_clip.gx_rectangle_left / 4);

    display_stride = (INT)display->gx_display_width / 4;
    pGetRow = pGetRow + ((INT)(canvas->gx_canvas_display_offset_y + copy_clip.gx_rectangle_top) * display_stride);
    pGetRow = pGetRow + (INT)((canvas->gx_canvas_display_offset_x + copy_clip.gx_rectangle_left) / 4);

    for (row = 0; row < copy_height; row++)
    {
        pGet = pGetRow;
        pPut = pPutRow;

        for (col = 0; col < copy_width; col++)
        {
            *pPut = *pGet;
            ++pPut;
            ++pGet;
        }
        pPutRow += canvas_stride;
        pGetRow += display_stride;
    }
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp Frame buffer software draw function.
 * This function is called by gx_rotate_canvas_to_working_8bpp().
 * @param[in]     pGetRow           Pointer to copy source address
 * @param[in]     pPutRow           Pointer to copy destination address
 * @param[in]     width             Image width to copy
 * @param[in]     height            Image height to copy
 * @param[in]     stride            Image memory stride
 **********************************************************************************************************************/
static VOID gx_rotate_canvas_to_working_8bpp_draw(GX_UBYTE * pGetRow, GX_UBYTE * pPutRow, INT width, INT height,
                                                                                                        INT stride)
{
    GX_UBYTE   * pGet;
    GX_UBYTE   * pPut;

    for (INT row = 0; row < height; row++)
    {
        pGet = pGetRow;
        pPut = pPutRow;
        for (INT col = 0; col < width; col++)
        {
            *pPut = *pGet;
            ++pPut;
            ++pGet;
        }
        pGetRow += stride;
        pPutRow += stride;
    }
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp Frame buffer software draw function with rotating image 90 degree.
 * This function is called by gx_rotate_canvas_to_working_8bpp().
 * @param[in]     pGetRow           Pointer to copy source address
 * @param[in]     pPutRow           Pointer to copy destination address
 * @param[in]     width             Image width to copy
 * @param[in]     height            Image height to copy
 * @param[in]     canvas_stride     Frame buffer memory stride (of the canvas)
 * @param[in]     disp_stride       Frame buffer memory stride (of the destination frame buffer)
 **********************************************************************************************************************/
static VOID gx_rotate_canvas_to_working_8bpp_draw_rorate90(GX_UBYTE * pGetRow, GX_UBYTE * pPutRow, INT width,
                                                            INT height, INT canvas_stride, INT disp_stride)
{
    GX_UBYTE   * pGet;
    GX_UBYTE   * pPut;

    for (INT row = 0; row < height; row++)
    {
        pGet = pGetRow;
        pPut = pPutRow;

        for (INT col = 0; col < width; col++)
        {
            *pPut = *pGet;
            ++pGet;
            pPut += disp_stride;
        }

        pPutRow--;
        pGetRow += canvas_stride;
    }
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp Frame buffer software draw function with rotating image 180 degree.
 * This function is called by gx_rotate_canvas_to_working_8bpp().
 * @param[in]     pGetRow           Pointer to copy source address
 * @param[in]     pPutRow           Pointer to copy destination address
 * @param[in]     width             Image width to copy
 * @param[in]     height            Image height to copy
 * @param[in]     stride            Image memory stride
 **********************************************************************************************************************/
static VOID gx_rotate_canvas_to_working_8bpp_draw_rorate180(GX_UBYTE * pGetRow, GX_UBYTE * pPutRow, INT width,
                                                                                            INT height, INT stride)
{
    GX_UBYTE   * pGet;
    GX_UBYTE   * pPut;

    for (INT row = 0; row < height; row++)
    {
        pGet = pGetRow;
        pPut = pPutRow;
        for (INT col = 0; col < width; col++)
        {
            *pPut = *pGet;
            --pPut;
            ++pGet;
        }
        pGetRow += stride;
        pPutRow -= stride;
    }
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp Frame buffer software draw function with rotating image 270 degree.
 * This function is called by gx_rotate_canvas_to_working_8bpp().
 * @param[in]     pGetRow           Pointer to copy source address
 * @param[in]     pPutRow           Pointer to copy destination address
 * @param[in]     width             Image width to copy
 * @param[in]     height            Image height to copy
 * @param[in]     canvas_stride     Frame buffer memory stride (of the canvas)
 * @param[in]     disp_stride       Frame buffer memory stride (of the destination frame buffer)
 **********************************************************************************************************************/
static VOID gx_rotate_canvas_to_working_8bpp_draw_rorate270(GX_UBYTE * pGetRow, GX_UBYTE * pPutRow, INT width,
                                                             INT height, INT canvas_stride, INT disp_stride)
{
    GX_UBYTE   * pGet;
    GX_UBYTE   * pPut;

    for (INT row = 0; row < height; row++)
    {
        pGet = pGetRow;
        pPut = pPutRow;

        for (INT col = 0; col < width; col++)
        {
            *pPut = *pGet;
            ++pGet;
            pPut -= disp_stride;
        }

        pPutRow++;
        pGetRow += canvas_stride;
    }
}

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for Synergy, 8bpp Frame buffer software copy operation with screen rotation.
 * This function is called by _gx_synergy_buffer_toggle_8bpp().
 * @param[in]     canvas            Pointer to a canvas
 * @param[in]     copy              Pointer to a rectangle region to copy.
 * @param[in]     rotation_angle    Rotation angle (0, 90, 180 or 270)
 **********************************************************************************************************************/
static VOID gx_rotate_canvas_to_working_8bpp(GX_CANVAS * canvas, GX_RECTANGLE * copy, INT angle)
{
    GX_RECTANGLE display_size;
    GX_RECTANGLE copy_clip;

    GX_UBYTE   * pGetRow;
    GX_UBYTE   * pPutRow;

    INT          copy_width;
    INT          copy_height;
    INT          canvas_stride;
    INT          display_stride;

    GX_DISPLAY * display = canvas->gx_canvas_display;
    gx_utility_rectangle_define(&display_size, 0, 0,
                                (GX_VALUE)(display->gx_display_width - 1),
                                (GX_VALUE)(display->gx_display_height - 1));
    copy_clip = *copy;

    /** Align copy region on 32-bit boundary. */
    copy_clip.gx_rectangle_left   = (GX_VALUE)((USHORT)copy_clip.gx_rectangle_left & 0xfffcU);
    copy_clip.gx_rectangle_top    = (GX_VALUE)((USHORT)copy_clip.gx_rectangle_top & 0xfffcU);
    copy_clip.gx_rectangle_right  = (GX_VALUE)((USHORT)copy_clip.gx_rectangle_right  | 3U);
    copy_clip.gx_rectangle_bottom = (GX_VALUE)((USHORT)copy_clip.gx_rectangle_bottom | 3U);

    /** Offset canvas within frame buffer. */
    gx_utility_rectangle_shift(&copy_clip, canvas->gx_canvas_display_offset_x, canvas->gx_canvas_display_offset_y);

    gx_utility_rectangle_overlap_detect(&copy_clip, &display_size, &copy_clip);
    copy_width  = (INT)(copy_clip.gx_rectangle_right - copy_clip.gx_rectangle_left) + 1;
    copy_height = (INT)(copy_clip.gx_rectangle_bottom - copy_clip.gx_rectangle_top) + 1;

    if ((copy_width <= 0) || (copy_height <= 0))
    {
        return;
    }

    pGetRow = (GX_UBYTE *) canvas->gx_canvas_memory;
    pPutRow = (GX_UBYTE *) working_frame;

    display_stride = (INT)display->gx_display_height;
    canvas_stride  = (INT)canvas->gx_canvas_x_resolution;

    if(angle == 0)
    {
        pPutRow = pPutRow + ((INT)copy_clip.gx_rectangle_top * canvas_stride);
        pPutRow = pPutRow + (INT)copy_clip.gx_rectangle_left;

        pGetRow = pGetRow + ((INT)(canvas->gx_canvas_display_offset_y + copy_clip.gx_rectangle_top) * canvas_stride);
        pGetRow = pGetRow + (INT)(canvas->gx_canvas_display_offset_x + copy_clip.gx_rectangle_left);

        gx_rotate_canvas_to_working_8bpp_draw(pGetRow, pPutRow, copy_width, copy_height, canvas_stride);
    }
    else if(angle == 180)
    {
        pPutRow = pPutRow + ((INT)(display->gx_display_height - copy_clip.gx_rectangle_top) * canvas_stride);
        pPutRow = pPutRow - ((INT)copy_clip.gx_rectangle_left + 1);

        pGetRow = pGetRow + ((INT)(canvas->gx_canvas_display_offset_y + copy_clip.gx_rectangle_top) * canvas_stride);
        pGetRow = pGetRow + (INT)(canvas->gx_canvas_display_offset_x + copy_clip.gx_rectangle_left);

        gx_rotate_canvas_to_working_8bpp_draw_rorate180(pGetRow, pPutRow, copy_width, copy_height, canvas_stride);
    }
    else if(angle == 270)
    {
        pPutRow = pPutRow + ((INT)((display->gx_display_width - 1) - copy_clip.gx_rectangle_left) * display_stride);
        pPutRow = pPutRow + (INT)copy_clip.gx_rectangle_top;

        pGetRow = pGetRow + ((INT)(canvas->gx_canvas_display_offset_y + copy_clip.gx_rectangle_top) * canvas_stride);
        pGetRow = pGetRow + (INT)(canvas->gx_canvas_display_offset_x + copy_clip.gx_rectangle_left);

        gx_rotate_canvas_to_working_8bpp_draw_rorate270(pGetRow, pPutRow, copy_width, copy_height,
                                                                                    canvas_stride, display_stride);
    }
    else /* angle = 90 */
    {
        pPutRow = pPutRow + ((INT)copy_clip.gx_rectangle_left * display_stride);
        pPutRow = pPutRow + (INT)(display_stride - 1);
        pPutRow = pPutRow - (INT)copy_clip.gx_rectangle_top;

        pGetRow = pGetRow + ((INT)(canvas->gx_canvas_display_offset_y + copy_clip.gx_rectangle_top) * canvas_stride);
        pGetRow = pGetRow + (INT)(canvas->gx_canvas_display_offset_x + copy_clip.gx_rectangle_left);

        gx_rotate_canvas_to_working_8bpp_draw_rorate90(pGetRow, pPutRow, copy_width, copy_height,
                                                                                    canvas_stride, display_stride);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup SF_EL_GX)
 **********************************************************************************************************************/
