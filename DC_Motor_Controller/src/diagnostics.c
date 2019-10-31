#include "diagnostics.h"
#include "common.h"

int int_shortBatteryDetected = 0;
int int_shortGroundDetected = 0;
int int_adcSetPoint = 0;

void runDiagnostics(int int_adcValue){
    if(int_adcValue > 230){
        setShortBattery(1);
        setShortGround(0);
        calculateSetPoint(255);
    }else if(int_adcValue < 26){
        setShortBattery(0);
        setShortGround(1);
        calculateSetPoint(0);
    }else{
        setShortBattery(0);
        setShortGround(0);
        calculateSetPoint(int_adcValue);
    }
}

void calculateSetPoint(int adcValue){
    int setPoint = (MAX_SPEED*adcValue)/255;
    setSetPoint(setPoint);
}

void setShortBattery(int value){
    int_shortBatteryDetected = value;
}

int getShortBattery(){
    return int_shortBatteryDetected;
}

void setShortGround(int value){
    int_shortGroundDetected = value;
}

int getShortGround(){
    return int_shortGroundDetected;
}

void setSetPoint(int setPoint){
    int_adcSetPoint = setPoint;
}

int getSetPoint(){
    return int_adcSetPoint;
}
