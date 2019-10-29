/*
 * init.h
 *
 *  Created on: 28/10/2019
 *      Author: SEI301
 */

#ifndef INIT_H_
#define INIT_H_

/* Declare initialization function prototypes */

extern void initializeDCMotorModules(void);
extern void startTimerInterrupt(void);
extern void startPWMTimer(void);
extern void startADCModule(void);

#endif /* INIT_H_ */
