#ifndef DIAGNOSTICS_H_
#define DIAGNOSTICS_H_

extern void runDiagnostics(int);
extern void calculateSetPoint(int);
extern void setShortBattery(int);
extern int getShortBattery(void);
extern void setShortGround(int);
extern int getShortGround(void);
extern void setSetPoint(int);
extern int getSetPoint(void);

#endif /* DIAGNOSTICS_H_ */
