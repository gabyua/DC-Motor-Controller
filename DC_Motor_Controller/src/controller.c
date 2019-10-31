#include "controller.h"
#include "main_thread.h"
#include "common.h"

int int_eSpeed = 0;
int int_eSpeedPrev = 0;
int int_iSpeed = 0;
int int_iSpeedPrev = 0;
int int_dSpeed = 0;
int int_newSpeed = 0;
int int_dutyCycle = 0;
int int_pwmValue = 0;

double kp=5;
double ki=0.001;
double kd=0;

void calculatePWM(int int_setSpeed, int int_actualSpeed){
    /*
    //PID CONTROLLER
    int_eSpeed = int_setSpeed - int_actualSpeed;
    int_iSpeed = int_eSpeed * ki + int_iSpeedPrev;
    int_dSpeed = kd * (int_eSpeed - int_eSpeedPrev);
    int_newSpeed = kp * int_eSpeed + int_iSpeed + int_dSpeed;

    if(int_newSpeed > MAX_SPEED){
        int_newSpeed = MAX_SPEED;
    }else if(int_newSpeed < 0){
        int_newSpeed = 0;
    }

    //Set Duty Cycle
    int_dutyCycle = ((int_newSpeed * 100) / MAX_SPEED);
    setPWM(int_dutyCycle);

    //Duty Cycle adjustment where 100% means no movement
    int_dutyCycle = 100 - ((int_newSpeed * 100) / MAX_SPEED);

    //PWM.p_api->dutyCycleSet(PWM.p_ctrl,int_dutyCycle,TIMER_PWM_UNIT_PERCENT, IOPORT_PORT_04_PIN_06);
    g_timer1.p_api->dutyCycleSet(g_timer1.p_ctrl, int_dutyCycle, TIMER_PWM_UNIT_PERCENT, 1);

    int_eSpeedPrev = int_eSpeed;
    int_iSpeedPrev = int_iSpeed;
    */

    //Set Duty Cycle
    int_dutyCycle = ((int_newSpeed * 100) / MAX_SPEED);
    setPWM(int_dutyCycle);

    int_newSpeed = int_setSpeed;
    int_dutyCycle = 100 - ((int_newSpeed * 100) / MAX_SPEED);
    g_timer1.p_api->dutyCycleSet(g_timer1.p_ctrl, int_dutyCycle, TIMER_PWM_UNIT_PERCENT, 1);

}

void setPWM(int pwm){
    int_pwmValue = pwm;
}

int getPWM(){
    return int_pwmValue;
}
