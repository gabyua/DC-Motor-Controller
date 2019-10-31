
#include <stdio.h>

#include "gui/guiapp_resources.h"
#include "gui/guiapp_specifications.h"

#include "main_thread.h"
#include "common.h"

UINT uint_dcValue = 0;
UINT uint_speedValue = 0;
UINT uint_setPointValue = 0;
UINT uint_shortValue = 0;

CHAR char_dcDisplay[50] = "";
CHAR char_dcText[5] = "00";

CHAR char_speedDisplay[50] = "";
CHAR char_speedText[10] = "0000";

CHAR char_setPointDisplay[50] = "";
CHAR char_setPointText[10] = "0000";

CHAR char_shortDisplay[50] = "";
CHAR char_shortText[10] = "0000";


CHAR char_shortLabel[15] = "Input: ";
CHAR char_shortUnits[5] = " V";

extern GX_WINDOW_ROOT * p_window_root;

static UINT show_window(GX_WINDOW * p_new, GX_WIDGET * p_widget, bool detach_old);
static void update_text_id(GX_WIDGET * p_widget, GX_RESOURCE_ID id, UINT string_id);
static void update_text_string(GX_WIDGET * p_widget, GX_RESOURCE_ID id, CHAR * string);

UINT window1_handler(GX_WINDOW *widget, GX_EVENT *event_ptr)
{
    UINT result = gx_window_event_process(widget, event_ptr);

    switch (event_ptr->gx_event_type)
    {
    case GX_SIGNAL(ID_WINDOWCHANGER, GX_EVENT_CLICKED):
        show_window((GX_WINDOW*)&window2, (GX_WIDGET*)widget, true);
        break;
    case DC_UPDATE_EVENT:
        uint_dcValue = event_ptr->gx_event_payload.gx_event_timer_id;
        sprintf(char_dcText,"%x",uint_dcValue);

        strcpy(char_dcDisplay, "Duty Cycle: ");
        strcat(char_dcDisplay,char_dcText);
        strcat(char_dcDisplay,"%");

        update_text_string(widget->gx_widget_parent, ID_DUTYCYCLE, char_dcDisplay);
        break;
    case SPEED_UPDATE_EVENT:
        uint_speedValue = event_ptr->gx_event_payload.gx_event_timer_id;
        sprintf(char_speedText,"%x",uint_speedValue);

        strcpy(char_speedDisplay, "Speed: ");
        strcat(char_speedDisplay,char_speedText);
        strcat(char_speedDisplay," RPM");

        update_text_string(widget->gx_widget_parent, ID_SPEED, char_speedDisplay);
        break;
    case SETPOINT_UPDATE_EVENT:
        //uint_setPointValue++;
        uint_setPointValue = event_ptr->gx_event_payload.gx_event_timer_id;
        sprintf(char_setPointText,"%x",uint_setPointValue);

        strcpy(char_setPointDisplay, "Set Point: ");
        strcat(char_setPointDisplay,char_setPointText);
        strcat(char_setPointDisplay," RPM");

        update_text_string(widget->gx_widget_parent, ID_SETPOINT, char_setPointDisplay);
        break;
    case SWVERSION_UPDATE_EVENT:
        update_text_string(widget->gx_widget_parent, ID_SW_VERSION, "SW Version: 0.1");
        break;
    default:
        gx_window_event_process(widget, event_ptr);
        break;
    }

    return result;
}

UINT window2_handler(GX_WINDOW *widget, GX_EVENT *event_ptr)
{
    UINT result = gx_window_event_process(widget, event_ptr);
    ULONG ulong_eventType = event_ptr->gx_event_type;

    switch (ulong_eventType){
        case GX_EVENT_PEN_UP:
            show_window((GX_WINDOW*)&window1, (GX_WIDGET*)widget, true);
            break;
        case SHORT_BATTERY_UPDATE_EVENT:
        case SHORT_GROUND_UPDATE_EVENT:
            //uint_setPointValue++;
            uint_shortValue = event_ptr->gx_event_payload.gx_event_timer_id;

            int int_shortValueUnits = uint_shortValue % 10;
            int int_shortValueDec = uint_shortValue / 10;

            strcpy(char_shortDisplay, "Input: ");

            sprintf(char_shortText,"%d",int_shortValueDec);
            strcat(char_shortDisplay,char_shortText);

            strcat(char_shortDisplay,".");

            sprintf(char_shortText,"%d",int_shortValueUnits);
            strcat(char_shortDisplay,char_shortText);

            strcat(char_shortDisplay," V");

            if(ulong_eventType == SHORT_BATTERY_UPDATE_EVENT){
                update_text_string(widget->gx_widget_parent, ID_SHORT1, "Short to battery detected!");
            }else{
                update_text_string(widget->gx_widget_parent, ID_SHORT1, "Short to ground detected!");
            }

            update_text_string(widget->gx_widget_parent, ID_SHORT2, char_shortDisplay);
            break;
        case NO_SHORT_UPDATE_EVENT:
            update_text_string(widget->gx_widget_parent, ID_SHORT1, "");
            update_text_string(widget->gx_widget_parent, ID_SHORT2, "");
            break;
        default:
            result = gx_window_event_process(widget, event_ptr);
            break;
    }
    return result;
}

static UINT show_window(GX_WINDOW * p_new, GX_WIDGET * p_widget, bool detach_old)
{
    UINT err = GX_SUCCESS;

    if (!p_new->gx_widget_parent)
    {
        err = gx_widget_attach(p_window_root, p_new);
    }
    else
    {
        err = gx_widget_show(p_new);
    }

    gx_system_focus_claim(p_new);

    GX_WIDGET * p_old = p_widget;
    if (p_old && detach_old)
    {
        if (p_old != (GX_WIDGET*)p_new)
        {
            gx_widget_detach(p_old);
        }
    }

    return err;
}

static void update_text_id(GX_WIDGET * p_widget, GX_RESOURCE_ID id, UINT string_id)
{
    GX_PROMPT * p_prompt = NULL;

    ssp_err_t err = gx_widget_find(p_widget, id, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_prompt);
    if (TX_SUCCESS == err)
    {
        gx_prompt_text_id_set(p_prompt, string_id);
    }
}

static void update_text_string(GX_WIDGET * p_widget, GX_RESOURCE_ID id, CHAR * string)
{
    GX_PROMPT * p_prompt = NULL;

    ssp_err_t err = gx_widget_find(p_widget, id, GX_SEARCH_DEPTH_INFINITE, (GX_WIDGET**)&p_prompt);
    if (TX_SUCCESS == err)
    {
        gx_prompt_text_set(p_prompt, string);
    }
}
