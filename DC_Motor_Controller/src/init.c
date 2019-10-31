/*
 * init.c
 *
 *  Created on: 28/10/2019
 *      Author: SEI301
 */

#include "init.h"
#include "adc.h"
#include "main_thread.h"
#include "common.h"

/* Define initialization functions */


void initializeDCMotorModules(void)
{
    /* Initialize required DC Motor Controller modules */
    startTimerInterrupt();
    startPWMTimer();
    startADCModule();
}


void startTimerInterrupt(void)
{
    /* Start timer interruption to manage scheduler activities */
    g_timer0.p_api->open(g_timer0.p_ctrl,g_timer0.p_cfg);
}


void startPWMTimer(void)
{
    /* Start PWM timer module activity*/
    g_timer1.p_api->open (g_timer1.p_ctrl, g_timer1.p_cfg);
    g_timer1.p_api->start (g_timer1.p_ctrl);
    g_timer1.p_api->dutyCycleSet(g_timer1.p_ctrl, 90, TIMER_PWM_UNIT_PERCENT, 1);
}


void startADCModule(void)
{
    /* Start ADC module activity */
    g_adc0.p_api->open(g_adc0.p_ctrl, g_adc0.p_cfg);
    g_adc0.p_api->scanCfg(g_adc0.p_ctrl, g_adc0.p_channel_cfg);
    g_adc0.p_api->scanStart(g_adc0.p_ctrl);
}


