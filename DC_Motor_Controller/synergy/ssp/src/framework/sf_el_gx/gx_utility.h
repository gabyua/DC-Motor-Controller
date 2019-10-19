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
/**   Utility (Utility)                                                   */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    gx_utility.h                                        PORTABLE C      */
/*                                                           5.3.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Express Logic, Inc.                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the GUIX utility component,                       */
/*    including all data types and external references.  It is assumed    */
/*    that gx_api.h and gx_port.h have already been included.             */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  11-24-2014     William E. Lamie         Initial Version 5.2           */
/*  01-16-2015     William E. Lamie         Modified comment(s),          */
/*                                            resulting in version 5.2.1  */
/*  01-26-2015     William E. Lamie         Modified comment(s),          */
/*                                            resulting in version 5.2.2  */
/*  03-01-2015     William E. Lamie         Modified comment(s),          */
/*                                            resulting in version 5.2.3  */
/*  04-15-2015     William E. Lamie         Modified comment(s),          */
/*                                            resulting in version 5.2.4  */
/*  08-21-2015     William E. Lamie         Modified comment(s), added    */
/*                                            function prototype,         */
/*                                            resulting in version 5.2.5  */
/*  09-21-2015     William E. Lamie         Modified comment(s),          */
/*                                            resulting in version 5.2.6  */
/*  02-22-2016     William E. Lamie         Modified comment(s), and      */
/*                                            fixed compiler warnings,    */
/*                                            resulting in version 5.3    */
/*  04-05-2016     William E. Lamie         Modified comment(s), added    */
/*                                            new features and APIs,      */
/*                                            resulting in version 5.3.1  */
/*  06-15-2016     William E. Lamie         Modified comment(s),          */
/*                                            resulting in version 5.3.2  */
/*  03-01-2017     William E. Lamie         Modified comment(s), cleaned  */
/*                                            up function prototypes,     */
/*                                            resulting in version 5.3.3  */
/*                                                                        */
/**************************************************************************/

#ifndef GX_UTILITY_H
#define GX_UTILITY_H


/* Define utility component function prototypes.  */

UINT    _gx_utility_8bpp_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height);
UINT    _gx_utility_8bpp_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT    _gx_utility_8bpp_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT    _gx_utility_8bit_alphamap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT    _gx_utility_16bpp_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height);
UINT    _gx_utility_32argb_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height);
UINT    _gx_utility_32argb_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT    _gx_utility_32argb_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT    _gx_utility_332rgb_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT    _gx_utility_332rgb_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT    _gx_utility_565rgb_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT    _gx_utility_565rgb_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT    _gx_utility_4444argb_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height);
UINT    _gx_utility_4444argb_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT    _gx_utility_4444argb_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT    _gx_utility_alphamap_create(INT width, INT height, GX_PIXELMAP *map);

VOID    _gx_utility_glyph_1bpp_to_alphamap_draw(GX_PIXELMAP *map, INT xpos, INT ypos, GX_CONST GX_GLYPH *glyph);
VOID    _gx_utility_glyph_4bpp_to_alphamap_draw(GX_PIXELMAP *map, INT xpos, INT ypos, GX_CONST GX_GLYPH *glyph);
VOID    _gx_utility_glyph_8bpp_to_alphamap_draw(GX_PIXELMAP *map, INT xpos, INT ypos, GX_CONST GX_GLYPH *glyph);

VOID    _gx_utility_ltoa(LONG value, GX_CHAR *return_buffer, UINT return_buffer_size);
INT     _gx_utility_math_acos(INT x);
INT     _gx_utility_math_asin(INT x);
INT     _gx_utility_math_cos(INT angle);
INT     _gx_utility_math_sin(INT angle);
UINT    _gx_utility_math_sqrt(UINT n);
UINT    _gx_utility_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height);
UINT    _gx_utility_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT    _gx_utility_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
VOID    _gx_utility_rectangle_center(GX_RECTANGLE *rectangle, GX_RECTANGLE *within);
VOID    _gx_utility_rectangle_center_find(GX_RECTANGLE *rectangle, GX_POINT *return_center);
VOID    _gx_utility_rectangle_combine(GX_RECTANGLE *first_rectangle, GX_RECTANGLE *second_rectangle);
GX_BOOL _gx_utility_rectangle_compare(GX_RECTANGLE *first_rectangle, GX_RECTANGLE *second_rectangle);
VOID    _gx_utility_rectangle_define(GX_RECTANGLE *rectangle, GX_VALUE left, GX_VALUE top, GX_VALUE right, GX_VALUE bottom);
GX_BOOL _gx_utility_rectangle_inside_detect(GX_RECTANGLE *outer, GX_RECTANGLE *inner);
GX_BOOL _gx_utility_rectangle_overlap_detect(GX_RECTANGLE *first_rectangle, GX_RECTANGLE *second_rectangle, GX_RECTANGLE *return_overlap_area);
GX_BOOL _gx_utility_rectangle_point_detect(GX_RECTANGLE *rectangle, GX_POINT point);
VOID    _gx_utility_rectangle_resize(GX_RECTANGLE *rectangle, GX_VALUE adjust);
VOID    _gx_utility_rectangle_shift(GX_RECTANGLE *rectangle, GX_VALUE x_shift, GX_VALUE y_shift);


UINT    _gx_utility_string_to_alphamap(GX_CONST GX_CHAR *text, GX_CONST GX_FONT *font, GX_PIXELMAP *textmap);
VOID    _gx_utility_string_to_alphamap_draw(GX_CONST GX_CHAR *text, GX_CONST GX_FONT *font, GX_PIXELMAP *map);
#ifdef GX_UTF8_SUPPORT
UINT    _gx_utility_unicode_to_utf8(ULONG unicode, GX_UBYTE *return_utf8_str, UINT *return_utf8_size);
UINT    _gx_utility_utf8_string_character_count_get(GX_CONST GX_CHAR *utf8_str, UINT byte_count, UINT *character_count);
UINT    _gx_utility_utf8_string_character_get(GX_CONST GX_CHAR **utf8_str, UINT byte_count, GX_CHAR_CODE *glyph_value, UINT *glyph_len);
#endif /* GX_UTF8_SUPPORT */


/* Define error checking shells for API services.  These are only referenced by the
   application.  */
VOID    _gxe_utility_ltoa(LONG value, GX_CHAR *return_buffer, UINT return_buffer_size);
INT     _gxe_utility_math_asin(INT x);
INT     _gxe_utility_math_acos(INT x);
UINT    _gxe_utility_math_sqrt(UINT n);
UINT    _gxe_utility_pixelmap_resize(GX_PIXELMAP *src, GX_PIXELMAP *destination, INT width, INT height);
UINT    _gxe_utility_pixelmap_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
UINT    _gxe_utility_pixelmap_simple_rotate(GX_PIXELMAP *src, INT angle, GX_PIXELMAP *destination, INT *rot_cx, INT *rot_cy);
VOID    _gxe_utility_rectangle_center(GX_RECTANGLE *rectangle, GX_RECTANGLE *within);
VOID    _gxe_utility_rectangle_center_find(GX_RECTANGLE *rectangle, GX_POINT *return_center);
VOID    _gxe_utility_rectangle_combine(GX_RECTANGLE *first_rectangle, GX_RECTANGLE *second_rectangle);
GX_BOOL _gxe_utility_rectangle_compare(GX_RECTANGLE *first_rectangle, GX_RECTANGLE *second_rectangle);
VOID    _gxe_utility_rectangle_define(GX_RECTANGLE *rectangle, GX_VALUE left, GX_VALUE top, GX_VALUE right, GX_VALUE bottom);
VOID    _gxe_utility_rectangle_resize(GX_RECTANGLE *rectangle, GX_VALUE adjust);
GX_BOOL _gxe_utility_rectangle_overlap_detect(GX_RECTANGLE *first_rectangle, GX_RECTANGLE *second_rectangle, GX_RECTANGLE *return_overlap_area);
GX_BOOL _gxe_utility_rectangle_point_detect(GX_RECTANGLE *rectangle, GX_POINT point);
VOID    _gxe_utility_rectangle_shift(GX_RECTANGLE *rectangle, GX_VALUE x_shift, GX_VALUE y_shift);
UINT    _gxe_utility_string_to_alphamap(GX_CONST GX_CHAR *text, GX_CONST GX_FONT *font, GX_PIXELMAP *textmap);
#endif

