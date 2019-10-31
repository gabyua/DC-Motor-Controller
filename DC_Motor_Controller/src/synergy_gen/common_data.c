/* generated common source file - do not edit */
#include "common_data.h"
#if SYNERGY_NOT_DEFINED != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_DRW)
SSP_VECTOR_DEFINE(drw_int_isr, DRW, INT);
#endif
#endif
SSP_VECTOR_DEFINE( jpeg_jdti_isr, JPEG, JDTI);
SSP_VECTOR_DEFINE( jpeg_jedi_isr, JPEG, JEDI);
static jpeg_decode_instance_ctrl_t g_jpeg_decode0_ctrl;
const jpeg_decode_cfg_t g_jpeg_decode0_cfg =
{ .input_data_format = JPEG_DECODE_DATA_FORMAT_NORMAL,
  .output_data_format = JPEG_DECODE_DATA_FORMAT_NORMAL,
  .pixel_format = JPEG_DECODE_PIXEL_FORMAT_RGB565,
  .alpha_value = 255,
  .p_callback = NULL,
  .jedi_ipl = (3),
  .jdti_ipl = (3), };
const jpeg_decode_instance_t g_jpeg_decode0 =
{ .p_api = (jpeg_decode_api_t const *) &g_jpeg_decode_on_jpeg_decode, .p_ctrl = &g_jpeg_decode0_ctrl, .p_cfg =
          &g_jpeg_decode0_cfg };
static sf_jpeg_decode_instance_ctrl_t g_sf_jpeg_decode0_ctrl;

static const sf_jpeg_decode_cfg_t g_sf_jpeg_decode0_cfg =
{ .p_lower_lvl_jpeg_decode = (jpeg_decode_instance_t const *) &g_jpeg_decode0 };
const sf_jpeg_decode_instance_t g_sf_jpeg_decode0 =
{ .p_api = &g_sf_jpeg_decode_on_sf_jpeg_decode, .p_ctrl = &g_sf_jpeg_decode0_ctrl, .p_cfg =
          (sf_jpeg_decode_cfg_t const *) &g_sf_jpeg_decode0_cfg };
#if (3) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_display) && !defined(SSP_SUPPRESS_ISR_GLCD)
SSP_VECTOR_DEFINE( glcdc_line_detect_isr, GLCDC, LINE_DETECT);
#endif
#endif
#if (3) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_display) && !defined(SSP_SUPPRESS_ISR_GLCD)
SSP_VECTOR_DEFINE( glcdc_underflow_1_isr, GLCDC, UNDERFLOW_1);
#endif
#endif
#if (3) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_display) && !defined(SSP_SUPPRESS_ISR_GLCD)
SSP_VECTOR_DEFINE( glcdc_underflow_2_isr, GLCDC, UNDERFLOW_2);
#endif
#endif

/** Display frame buffer */
#if (true)
uint8_t g_display_fb_background[2][((256 * 320) * DISPLAY_BITS_PER_PIXEL_INPUT0) >> 3] BSP_ALIGN_VARIABLE_V2(64) BSP_PLACE_IN_SECTION_V2(".bss");
#else
/** Graphics screen1 is specified not to be used when starting */
#endif
#if (false)
uint8_t g_display_fb_foreground[2][((800 * 480) * DISPLAY_BITS_PER_PIXEL_INPUT1) >> 3] BSP_ALIGN_VARIABLE_V2(64) BSP_PLACE_IN_SECTION_V2(".sdram");
#else
/** Graphics screen2 is specified not to be used when starting */
#endif

#if (false)
/** Display CLUT buffer to be used for updating CLUT */
static uint32_t CLUT_buffer[256];

/** Display CLUT configuration(only used if using CLUT format) */
display_clut_cfg_t g_display_clut_cfg_glcd =
{
    .p_base = (uint32_t *)CLUT_buffer,
    .start = 0, /* User have to update this setting when using */
    .size = 256 /* User have to update this setting when using */
};
#else
/** CLUT is specified not to be used */
#endif

#if (false | false | false)
/** Display interface configuration */
static const display_gamma_correction_t g_display_gamma_cfg =
{
    .r =
    {
        .enable = false,
        .gain =
        {   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        .threshold =
        {   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    },
    .g =
    {
        .enable = false,
        .gain =
        {   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        .threshold =
        {   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    },
    .b =
    {
        .enable = false,
        .gain =
        {   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        .threshold =
        {   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    }
};
#endif

/** Display device extended configuration */
static const glcd_cfg_t g_display_extend_cfg =
{ .tcon_hsync = GLCD_TCON_PIN_2,
  .tcon_vsync = GLCD_TCON_PIN_1,
  .tcon_de = GLCD_TCON_PIN_0,
  .correction_proc_order = GLCD_CORRECTION_PROC_ORDER_BRIGHTNESS_CONTRAST2GAMMA,
  .clksrc = GLCD_CLK_SRC_INTERNAL,
  .clock_div_ratio = GLCD_PANEL_CLK_DIVISOR_32,
  .dithering_mode = GLCD_DITHERING_MODE_TRUNCATE,
  .dithering_pattern_A = GLCD_DITHERING_PATTERN_11,
  .dithering_pattern_B = GLCD_DITHERING_PATTERN_11,
  .dithering_pattern_C = GLCD_DITHERING_PATTERN_11,
  .dithering_pattern_D = GLCD_DITHERING_PATTERN_11 };

/** Display control block instance */
glcd_instance_ctrl_t g_display_ctrl;

/** Display interface configuration */
const display_cfg_t g_display_cfg =
        {
        /** Input1(Graphics1 screen) configuration */
        .input[0
                  ] =
                  {
#if (true)
                    .p_base = (uint32_t *)&g_display_fb_background[0],
#else
                    .p_base = NULL,
#endif
                    .hsize = 256,
                    .vsize = 320, .hstride = 256, .format = DISPLAY_IN_FORMAT_16BITS_RGB565, .line_descending_enable =
                            false,
                    .lines_repeat_enable = false, .lines_repeat_times = 0 },

          /** Input2(Graphics2 screen) configuration */
          .input[1
                  ] =
                  {
#if (false)
                    .p_base = (uint32_t *)&g_display_fb_foreground[0],
#else
                    .p_base = NULL,
#endif
                    .hsize = 800,
                    .vsize = 480, .hstride = 800, .format = DISPLAY_IN_FORMAT_16BITS_RGB565, .line_descending_enable =
                            false,
                    .lines_repeat_enable = false, .lines_repeat_times = 0 },

          /** Input1(Graphics1 screen) layer configuration */
          .layer[0] =
          { .coordinate =
          { .x = 0, .y = 0 },
            .bg_color =
            { .byte =
            { .a = 255, .r = 255, .g = 255, .b = 255 } },
            .fade_control = DISPLAY_FADE_CONTROL_NONE, .fade_speed = 0 },

          /** Input2(Graphics2 screen) layer configuration */
          .layer[1] =
          { .coordinate =
          { .x = 0, .y = 0 },
            .bg_color =
            { .byte =
            { .a = 255, .r = 255, .g = 255, .b = 255 } },
            .fade_control = DISPLAY_FADE_CONTROL_NONE, .fade_speed = 0 },

          /** Output configuration */
          .output =
                  { .htiming =
                  { .total_cyc = 320, .display_cyc = 240, .back_porch = 6, .sync_width = 4, .sync_polarity =
                            DISPLAY_SIGNAL_POLARITY_LOACTIVE },
                    .vtiming =
                    { .total_cyc = 328, .display_cyc = 320, .back_porch = 4, .sync_width = 4, .sync_polarity =
                              DISPLAY_SIGNAL_POLARITY_LOACTIVE },
                    .format = DISPLAY_OUT_FORMAT_16BITS_RGB565, .endian = DISPLAY_ENDIAN_LITTLE, .color_order =
                            DISPLAY_COLOR_ORDER_RGB,
                    .data_enable_polarity = DISPLAY_SIGNAL_POLARITY_HIACTIVE, .sync_edge =
                            DISPLAY_SIGNAL_SYNC_EDGE_RISING,
                    .bg_color =
                    { .byte =
                    { .a = 255, .r = 0, .g = 0, .b = 0 } },
                    .brightness =
                    { .enable = false, .r = 512, .g = 512, .b = 512 },
                    .contrast =
                    { .enable = false, .r = 128, .g = 128, .b = 128 },
#if (false | false | false)
                    .p_gamma_correction = (display_gamma_correction_t *)(&g_display_gamma_cfg),
#else
                    .p_gamma_correction = NULL,
#endif
                    .dithering_on = false },

          /** Display device callback function pointer */
          .p_callback = NULL,
          .p_context = (void *) &g_display,

          /** Display device extended configuration */
          .p_extend = (void *) (&g_display_extend_cfg),

          .line_detect_ipl = (3),
          .underflow_1_ipl = (3), .underflow_2_ipl = (3), };

#if (true)
/** Display on GLCD run-time configuration(for the graphics1 screen) */
display_runtime_cfg_t g_display_runtime_cfg_bg =
{
    .input =
    {
#if (true)
        .p_base = (uint32_t *)&g_display_fb_background[0],
#else
        .p_base = NULL,
#endif
        .hsize = 256,
        .vsize = 320,
        .hstride = 256,
        .format = DISPLAY_IN_FORMAT_16BITS_RGB565,
        .line_descending_enable = false,
        .lines_repeat_enable = false,
        .lines_repeat_times = 0
    },
    .layer =
    {
        .coordinate =
        {
            .x = 0,
            .y = 0
        },
        .bg_color =
        {
            .byte =
            {
                .a = 255,
                .r = 255,
                .g = 255,
                .b = 255
            }
        },
        .fade_control = DISPLAY_FADE_CONTROL_NONE,
        .fade_speed = 0
    }
};
#endif
#if (false)
/** Display on GLCD run-time configuration(for the graphics2 screen) */
display_runtime_cfg_t g_display_runtime_cfg_fg =
{
    .input =
    {
#if (false)
        .p_base = (uint32_t *)&g_display_fb_foreground[0],
#else
        .p_base = NULL,
#endif
        .hsize = 800,
        .vsize = 480,
        .hstride = 800,
        .format = DISPLAY_IN_FORMAT_16BITS_RGB565,
        .line_descending_enable = false,
        .lines_repeat_enable = false,
        .lines_repeat_times = 0
    },
    .layer =
    {
        .coordinate =
        {
            .x = 0,
            .y = 0
        },
        .bg_color =
        {
            .byte =
            {
                .a = 255,
                .r = 255,
                .g = 255,
                .b = 255
            }
        },
        .fade_control = DISPLAY_FADE_CONTROL_NONE,
        .fade_speed = 0
    }
};
#endif

/* Instance structure to use this module. */
const display_instance_t g_display =
{ .p_ctrl = &g_display_ctrl, .p_cfg = (display_cfg_t *) &g_display_cfg, .p_api = (display_api_t *) &g_display_on_glcd };
/** GUIX Canvas Buffer */
#if false
#if (1 == 1) /* Inherit Frame Buffer Name from Graphics Screen 1 */
uint8_t g_sf_el_gx_canvas[sizeof(g_display_fb_background[0])] BSP_ALIGN_VARIABLE_V2(4) BSP_PLACE_IN_SECTION_V2(".sdram");
#else /* Inherit Frame Buffer Name from Graphics Screen 2 */
uint8_t g_sf_el_gx_canvas[sizeof(g_display_fb_foreground[0])] BSP_ALIGN_VARIABLE_V2(4) BSP_PLACE_IN_SECTION_V2(".sdram");
#endif
#endif

/** JPEG Work Buffer */
#if GX_USE_SYNERGY_JPEG
#if (768000)
uint8_t g_sf_el_gx_jpegbuffer[768000] BSP_ALIGN_VARIABLE_V2(64) BSP_PLACE_IN_SECTION_V2(".sdram");
#endif
#endif

/** GUIX Port module control block instance */
static sf_el_gx_instance_ctrl_t g_sf_el_gx_ctrl;

/** GUIX Port module configuration */
static const sf_el_gx_cfg_t g_sf_el_gx_cfg =
{
/* Display Instance Configuration */
.p_display_instance = (display_instance_t *) &g_display,

  /* Display Driver Runtime Configuration */
#if (1 == 1) /* Inherit Frame Buffer Name from Graphics Screen 1 */
  .p_display_runtime_cfg = &g_display_runtime_cfg_bg,
#else /* Inherit Frame Buffer Name from Graphics Screen 2 */
  .p_display_runtime_cfg = &g_display_runtime_cfg_fg,
#endif

  /* GUIX Canvas Configuration */
#if (false)
  .p_canvas = g_sf_el_gx_canvas,
#else
  .p_canvas = NULL,
#endif

  /* Display Driver Frame Buffer A Configuration */
#if (1 == 1) /* Inherit Frame Buffer Name from Graphics Screen 1 */
  .p_framebuffer_a = &g_display_fb_background[0], /* Always array[0] is used */
#else /* Inherit Frame Buffer Name from Graphics Screen 2 */
  .p_framebuffer_a = &g_display_fb_foreground[0], /* Always array[0] is used */
#endif

  /* Display Driver Frame Buffer B Configuration */
#if (1 == 1) /* Inherit Frame Buffer Name from Graphics Screen 1 */
#if (2 > 1) /* Multiple frame buffers are used for Graphics Screen 1 */
  .p_framebuffer_b = &g_display_fb_background[1], /* Always array[1] is used */
#else /* Single Frame Buffer is used for Graphics Screen 1 */
  .p_framebuffer_b = NULL,
#endif
#else /* Inherit Frame Buffer Name from Graphics Screen 2 */
#if (2 > 1) /* Multiple frame buffers are used for Graphics Screen 2 */
  .p_framebuffer_b = &g_display_fb_foreground[1], /* Always array[1] is used */
#else /* Single Frame Buffer is used for Graphics Screen 2 */
  .p_framebuffer_b = NULL,
#endif
#endif

  /* User Callback Configuration */
  .p_callback = NULL,

  /* JPEG Work Buffer Configuration */
#if GX_USE_SYNERGY_JPEG
  .p_jpegbuffer = g_sf_el_gx_jpegbuffer,
  .jpegbuffer_size = 768000,
  .p_sf_jpeg_decode_instance = (void *)&g_sf_jpeg_decode0,
#else
  .p_jpegbuffer = NULL,
  .jpegbuffer_size = 0, .p_sf_jpeg_decode_instance = NULL,
#endif

  /* Screen Rotation Angle Configuration */
  .rotation_angle = 0,

  /* D/AVE 2D Buffer Cache */
  .dave2d_buffer_cache_enabled = true };

/** GUIX Port module instance */
sf_el_gx_instance_t g_sf_el_gx =
{ .p_api = &sf_el_gx_on_guix, .p_ctrl = &g_sf_el_gx_ctrl, .p_cfg = &g_sf_el_gx_cfg };
#if defined(__ICCARM__)
#define g_sf_message0_err_callback_WEAK_ATTRIBUTE
#pragma weak g_sf_message0_err_callback  = g_sf_message0_err_callback_internal
#elif defined(__GNUC__)
#define g_sf_message0_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("g_sf_message0_err_callback_internal")))
#endif
void g_sf_message0_err_callback(void *p_instance, void *p_data)
g_sf_message0_err_callback_WEAK_ATTRIBUTE;
extern sf_message_subscriber_list_t *p_subscriber_lists[];
sf_message_instance_ctrl_t g_sf_message0_ctrl;
static uint8_t g_sf_message0_work_buffer[2048];
/* Configures the messaging framework */
sf_message_cfg_t g_sf_message0_cfg =
{ .p_work_memory_start = &g_sf_message0_work_buffer, .work_memory_size_bytes = 2048, .buffer_size =
          sizeof(sf_message_payload_t),
  .pp_subscriber_lists = p_subscriber_lists, .p_block_pool_name = (uint8_t *) "sf_msg_blk_pool" };
/* Instance structure to use this module. */
const sf_message_instance_t g_sf_message0 =
{ .p_ctrl = &g_sf_message0_ctrl, .p_cfg = &g_sf_message0_cfg, .p_api = &g_sf_message_on_sf_message };
/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
 *             with the prototype below.
 *             - void g_sf_message0_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void g_sf_message0_err_callback_internal(void *p_instance, void *p_data);
void g_sf_message0_err_callback_internal(void *p_instance, void *p_data)
{
    /** Suppress compiler warning for not using parameters. */
    SSP_PARAMETER_NOT_USED (p_instance);
    SSP_PARAMETER_NOT_USED (p_data);

    /** An error has occurred. Please check function arguments for more information. */
    BSP_CFG_HANDLE_UNRECOVERABLE_ERROR (0);
}
/*******************************************************************************************************************//**
 * @brief     Initialization function that the user can choose to have called automatically during thread entry.
 *            The user can call this function at a later time if desired using the prototype below.

 *            - void sf_message_init0(void)
 **********************************************************************************************************************/
void sf_message_init0(void)
{
    ssp_err_t ssp_err_g_sf_message0;

    /* Initializes Messaging Framework Queues */
    g_message_init ();

    /* Opens the messaging framework */
    ssp_err_g_sf_message0 = g_sf_message0.p_api->open (g_sf_message0.p_ctrl, g_sf_message0.p_cfg);
    if (SSP_SUCCESS != ssp_err_g_sf_message0)
    {
        /* Error returns, check the cause. */
        g_sf_message0_err_callback ((void *) &g_sf_message0, &ssp_err_g_sf_message0);
    }
}
const ioport_instance_t g_ioport =
{ .p_api = &g_ioport_on_ioport, .p_cfg = NULL };
const elc_instance_t g_elc =
{ .p_api = &g_elc_on_elc, .p_cfg = NULL };
const cgc_instance_t g_cgc =
{ .p_api = &g_cgc_on_cgc, .p_cfg = NULL };
/* Instance structure to use this module. */
const fmi_instance_t g_fmi =
{ .p_api = &g_fmi_on_fmi };
void g_common_init(void)
{
    /** Call initialization function if user has selected to do so. */
#if (1)
    sf_message_init0 ();
#endif
}
