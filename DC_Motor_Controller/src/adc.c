#include "adc.h"
#include "main_thread.h"
#include "common.h"

uint16_t uint16_adcValue = 0;
int int_adcReadCount = 0;
int int_adcAccum = 0;
int int_adcAverage = 0;
int int_adcVoltage = 0;

void readADCValue()
{
    g_adc0.p_api->read(g_adc0.p_ctrl, ADC_REG_CHANNEL_0, &uint16_adcValue);

    validateADC();

    int_adcAccum += uint16_adcValue;

    if(int_adcReadCount < 2){
        int_adcReadCount++;
    }else{
        setADC(int_adcAccum / 3);
        calculateVoltage();

        int_adcReadCount = 0;
        int_adcAccum = 0;
    }
}

void validateADC()
{
    if(uint16_adcValue > 255)
    {
        uint16_adcValue = 255;
    }
}

void calculateVoltage()
{
    int v = (33 * getADC()) / 255; //v = 33 means 3.3 volts
    setVoltage(v);
}

void setADC(int adcValue)
{
    int_adcAverage = adcValue;
}

int getADC()
{
    return int_adcAverage;
}

void setVoltage(int v)
{
    int_adcVoltage = v;
}

int getVoltage()
{
    return int_adcVoltage;
}
