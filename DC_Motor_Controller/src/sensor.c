#include "sensor.h"

int int_pulseReadCount = 0;
int int_pulseAccum = 0;
int int_sensorSpeed = 0;

void readSensor(int int_pulseCount){
    int_pulseAccum += int_pulseCount;

    if(int_pulseReadCount < 1){
        int_pulseReadCount++;
    }else{
        calculateSpeed(int_pulseAccum / 2);

        int_pulseReadCount = 0;
        int_pulseAccum = 0;
    }
}

void calculateSpeed(int pulseAverage){
    int int_spd = (pulseAverage*10*60)/4;
    //int int_spd = pulseAverage;
    setSpeed(int_spd);
}

void setSpeed(int int_spd){
    int_sensorSpeed = int_spd;
}

int getSpeed(){
    return int_sensorSpeed;
}
