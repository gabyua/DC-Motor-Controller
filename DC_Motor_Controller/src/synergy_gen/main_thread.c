/* generated thread source file - do not edit */
#include "main_thread.h"

TX_THREAD main_thread;
void main_thread_create(void);
static void main_thread_func(ULONG thread_input);
static uint8_t main_thread_stack[2048] BSP_PLACE_IN_SECTION_V2(".stack.main_thread") BSP_ALIGN_VARIABLE_V2(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void *p_instance, void *p_data);
void tx_startup_common_init(void);
#if !defined(SSP_SUPPRESS_ISR_g_spi_lcdc) && !defined(SSP_SUPPRESS_ISR_SCI0)
SSP_VECTOR_DEFINE_CHAN(sci_spi_rxi_isr, SCI, RXI, 0);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_spi_lcdc) && !defined(SSP_SUPPRESS_ISR_SCI0)
SSP_VECTOR_DEFINE_CHAN(sci_spi_txi_isr, SCI, TXI, 0);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_spi_lcdc) && !defined(SSP_SUPPRESS_ISR_SCI0)
SSP_VECTOR_DEFINE_CHAN(sci_spi_tei_isr, SCI, TEI, 0);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_spi_lcdc) && !defined(SSP_SUPPRESS_ISR_SCI0)
SSP_VECTOR_DEFINE_CHAN(sci_spi_eri_isr, SCI, ERI, 0);
#endif
sci_spi_instance_ctrl_t g_spi_lcdc_ctrl;

/** SPI extended configuration */
const sci_spi_extended_cfg g_spi_lcdc_cfg_extend =
{ .bitrate_modulation = true };

const spi_cfg_t g_spi_lcdc_cfg =
{ .channel = 0, .operating_mode = SPI_MODE_MASTER, .clk_phase = SPI_CLK_PHASE_EDGE_EVEN, .clk_polarity =
          SPI_CLK_POLARITY_HIGH,
  .mode_fault = SPI_MODE_FAULT_ERROR_DISABLE, .bit_order = SPI_BIT_ORDER_MSB_FIRST, .bitrate = 100000,
#define SYNERGY_NOT_DEFINED (1)             
#if (SYNERGY_NOT_DEFINED == SYNERGY_NOT_DEFINED)
  .p_transfer_tx = NULL,
#else
  .p_transfer_tx = &SYNERGY_NOT_DEFINED,
#endif
#if (SYNERGY_NOT_DEFINED == SYNERGY_NOT_DEFINED)
  .p_transfer_rx = NULL,
#else
  .p_transfer_rx = &SYNERGY_NOT_DEFINED,
#endif
#undef SYNERGY_NOT_DEFINED	
  .p_callback = g_lcd_spi_callback,
  .p_context = (void *) &g_spi_lcdc, .rxi_ipl = (3), .txi_ipl = (3), .tei_ipl = (3), .eri_ipl = (3), .p_extend =
          &g_spi_lcdc_cfg_extend, };
/* Instance structure to use this module. */
const spi_instance_t g_spi_lcdc =
{ .p_ctrl = &g_spi_lcdc_ctrl, .p_cfg = &g_spi_lcdc_cfg, .p_api = &g_spi_on_sci };
#if (3) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_touch_irq) && !defined(SSP_SUPPRESS_ISR_ICU9)
SSP_VECTOR_DEFINE( icu_irq_isr, ICU, IRQ9);
#endif
#endif
static icu_instance_ctrl_t g_touch_irq_ctrl;
static const external_irq_cfg_t g_touch_irq_cfg =
{ .channel = 9,
  .trigger = EXTERNAL_IRQ_TRIG_FALLING,
  .filter_enable = true,
  .pclk_div = EXTERNAL_IRQ_PCLK_DIV_BY_64,
  .autostart = true,
  .p_callback = NULL,
  .p_context = &g_touch_irq,
  .p_extend = NULL,
  .irq_ipl = (3), };
/* Instance structure to use this module. */
const external_irq_instance_t g_touch_irq =
{ .p_ctrl = &g_touch_irq_ctrl, .p_cfg = &g_touch_irq_cfg, .p_api = &g_external_irq_on_icu };
sf_external_irq_instance_ctrl_t g_sf_touch_irq_ctrl;
const sf_external_irq_cfg_t g_sf_touch_irq_cfg =
{ .event = SF_EXTERNAL_IRQ_EVENT_SEMAPHORE_PUT, .p_lower_lvl_irq = &g_touch_irq, };
/* Instance structure to use this module. */
const sf_external_irq_instance_t g_sf_touch_irq =
{ .p_ctrl = &g_sf_touch_irq_ctrl, .p_cfg = &g_sf_touch_irq_cfg, .p_api = &g_sf_external_irq_on_sf_external_irq };
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_transfer1) && !defined(SSP_SUPPRESS_ISR_DTCELC_EVENT_IIC2_RXI)
#define DTC_ACTIVATION_SRC_ELC_EVENT_IIC2_RXI
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_0) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_0);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0
#endif
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_1) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_1);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1
#endif
#endif
#endif

dtc_instance_ctrl_t g_transfer1_ctrl;
transfer_info_t g_transfer1_info =
{ .dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
  .repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
  .irq = TRANSFER_IRQ_END,
  .chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
  .src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .size = TRANSFER_SIZE_1_BYTE,
  .mode = TRANSFER_MODE_NORMAL,
  .p_dest = (void *) NULL,
  .p_src = (void const *) NULL,
  .num_blocks = 0,
  .length = 0, };
const transfer_cfg_t g_transfer1_cfg =
{ .p_info = &g_transfer1_info,
  .activation_source = ELC_EVENT_IIC2_RXI,
  .auto_enable = false,
  .p_callback = NULL,
  .p_context = &g_transfer1,
  .irq_ipl = (BSP_IRQ_DISABLED) };
/* Instance structure to use this module. */
const transfer_instance_t g_transfer1 =
{ .p_ctrl = &g_transfer1_ctrl, .p_cfg = &g_transfer1_cfg, .p_api = &g_transfer_on_dtc };
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_transfer0) && !defined(SSP_SUPPRESS_ISR_DTCELC_EVENT_IIC2_TXI)
#define DTC_ACTIVATION_SRC_ELC_EVENT_IIC2_TXI
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_0) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_0);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0
#endif
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_1) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_1);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1
#endif
#endif
#endif

dtc_instance_ctrl_t g_transfer0_ctrl;
transfer_info_t g_transfer0_info =
{ .dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
  .repeat_area = TRANSFER_REPEAT_AREA_SOURCE,
  .irq = TRANSFER_IRQ_END,
  .chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
  .src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
  .size = TRANSFER_SIZE_1_BYTE,
  .mode = TRANSFER_MODE_NORMAL,
  .p_dest = (void *) NULL,
  .p_src = (void const *) NULL,
  .num_blocks = 0,
  .length = 0, };
const transfer_cfg_t g_transfer0_cfg =
{ .p_info = &g_transfer0_info,
  .activation_source = ELC_EVENT_IIC2_TXI,
  .auto_enable = false,
  .p_callback = NULL,
  .p_context = &g_transfer0,
  .irq_ipl = (BSP_IRQ_DISABLED) };
/* Instance structure to use this module. */
const transfer_instance_t g_transfer0 =
{ .p_ctrl = &g_transfer0_ctrl, .p_cfg = &g_transfer0_cfg, .p_api = &g_transfer_on_dtc };
#if !defined(SSP_SUPPRESS_ISR_g_i2c) && !defined(SSP_SUPPRESS_ISR_IIC2)
SSP_VECTOR_DEFINE_CHAN(iic_rxi_isr, IIC, RXI, 2);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_i2c) && !defined(SSP_SUPPRESS_ISR_IIC2)
SSP_VECTOR_DEFINE_CHAN(iic_txi_isr, IIC, TXI, 2);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_i2c) && !defined(SSP_SUPPRESS_ISR_IIC2)
SSP_VECTOR_DEFINE_CHAN(iic_tei_isr, IIC, TEI, 2);
#endif
#if !defined(SSP_SUPPRESS_ISR_g_i2c) && !defined(SSP_SUPPRESS_ISR_IIC2)
SSP_VECTOR_DEFINE_CHAN(iic_eri_isr, IIC, ERI, 2);
#endif
riic_instance_ctrl_t g_i2c_ctrl;
const i2c_cfg_t g_i2c_cfg =
{ .channel = 2, .rate = I2C_RATE_STANDARD, .slave = 0x48, .addr_mode = I2C_ADDR_MODE_7BIT,
#define SYNERGY_NOT_DEFINED (1)            
#if (SYNERGY_NOT_DEFINED == g_transfer0)
  .p_transfer_tx = NULL,
#else
  .p_transfer_tx = &g_transfer0,
#endif
#if (SYNERGY_NOT_DEFINED == g_transfer1)
  .p_transfer_rx = NULL,
#else
  .p_transfer_rx = &g_transfer1,
#endif
#undef SYNERGY_NOT_DEFINED	
  .p_callback = NULL,
  .p_context = (void *) &g_i2c, .rxi_ipl = (3), .txi_ipl = (3), .tei_ipl = (3), .eri_ipl = (3), .p_extend = NULL, };
/* Instance structure to use this module. */
const i2c_master_instance_t g_i2c =
{ .p_ctrl = &g_i2c_ctrl, .p_cfg = &g_i2c_cfg, .p_api = &g_i2c_master_on_riic };
#if defined(__ICCARM__)
#define g_sf_touch_panel_i2c_err_callback_WEAK_ATTRIBUTE
#pragma weak g_sf_touch_panel_i2c_err_callback  = g_sf_touch_panel_i2c_err_callback_internal
#elif defined(__GNUC__)
#define g_sf_touch_panel_i2c_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("g_sf_touch_panel_i2c_err_callback_internal")))
#endif
void g_sf_touch_panel_i2c_err_callback(void *p_instance, void *p_data)
g_sf_touch_panel_i2c_err_callback_WEAK_ATTRIBUTE;
sf_touch_panel_i2c_instance_ctrl_t g_sf_touch_panel_i2c_ctrl;
const sf_touch_panel_i2c_cfg_t g_sf_touch_panel_i2c_cfg_extend =
{ .p_lower_lvl_i2c = &g_i2c, .p_lower_lvl_irq = &g_sf_touch_irq, .pin = IOPORT_PORT_06_PIN_09, .p_chip =
          &g_sf_touch_panel_i2c_chip_sx8654, };
const sf_touch_panel_cfg_t g_sf_touch_panel_i2c_cfg =
{ .hsize_pixels = 240, .vsize_pixels = 320, .priority = 3, .update_hz = 10, .p_message = &g_sf_message0, .p_extend =
          &g_sf_touch_panel_i2c_cfg_extend,
  .event_class_instance = 0, .rotation_angle = 0 };
/* Instance structure to use this module. */
const sf_touch_panel_instance_t g_sf_touch_panel_i2c =
{ .p_ctrl = &g_sf_touch_panel_i2c_ctrl, .p_cfg = &g_sf_touch_panel_i2c_cfg, .p_api =
          &g_sf_touch_panel_on_sf_touch_panel_i2c };
/*******************************************************************************************************************//**
 * @brief      Initialization function that the user can choose to have called automatically during thread entry.
 *             The user can call this function at a later time if desired using the prototype below.

 *             - void g_sf_touch_panel_i2c_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void g_sf_touch_panel_i2c_err_callback_internal(void *p_instance, void *p_data);
void g_sf_touch_panel_i2c_err_callback_internal(void *p_instance, void *p_data)
{
    /** Suppress compiler warning for not using parameters. */
    SSP_PARAMETER_NOT_USED (p_instance);
    SSP_PARAMETER_NOT_USED (p_data);

    /** An error has occurred. Please check function arguments for more information. */
    BSP_CFG_HANDLE_UNRECOVERABLE_ERROR (0);
}
/*******************************************************************************************************************//**
 * @brief     This is sf touch panel initialization function. User Can call this function in the application if required with the below mentioned prototype.
 *            - void sf_touch_panel_i2c_init0(void)
 **********************************************************************************************************************/
void sf_touch_panel_i2c_init0(void)
{
    ssp_err_t ssp_err_g_sf_touch_panel_i2c;
    ssp_err_g_sf_touch_panel_i2c = g_sf_touch_panel_i2c.p_api->open (g_sf_touch_panel_i2c.p_ctrl,
                                                                     g_sf_touch_panel_i2c.p_cfg);
    if (SSP_SUCCESS != ssp_err_g_sf_touch_panel_i2c)
    {
        g_sf_touch_panel_i2c_err_callback ((void *) &g_sf_touch_panel_i2c, &ssp_err_g_sf_touch_panel_i2c);
    }
}
TX_SEMAPHORE g_main_semaphore_lcdc;
extern bool g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;

void main_thread_create(void)
{
    /* Increment count so we will know the number of ISDE created threads. */
    g_ssp_common_thread_count++;

    /* Initialize each kernel object. */
    UINT err_g_main_semaphore_lcdc;
    err_g_main_semaphore_lcdc = tx_semaphore_create (&g_main_semaphore_lcdc, (CHAR *) "New Semaphore", 0);
    if (TX_SUCCESS != err_g_main_semaphore_lcdc)
    {
        tx_startup_err_callback (&g_main_semaphore_lcdc, 0);
    }

    UINT err;
    err = tx_thread_create (&main_thread, (CHAR *) "Main Thread", main_thread_func, (ULONG) NULL, &main_thread_stack,
                            2048, 6, 6, 10, TX_AUTO_START);
    if (TX_SUCCESS != err)
    {
        tx_startup_err_callback (&main_thread, 0);
    }
}

static void main_thread_func(ULONG thread_input)
{
    /* Not currently using thread_input. */
    SSP_PARAMETER_NOT_USED (thread_input);

    /* Initialize common components */
    tx_startup_common_init ();

    /* Initialize each module instance. */
    /** Call initialization function if user has selected to do so. */
#if (1)
    sf_touch_panel_i2c_init0 ();
#endif

    /* Enter user code for this thread. */
    main_thread_entry ();
}
