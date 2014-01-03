#ifndef INCLUDE_STEPPER
#define INCLUDE_STEPPER

#include <typedefs.h>


void stepperInit(void);
void stepForward(void);
void stepBack(void);
void stepLeft(void);
void stepRight(void);
void stepOff(void);
void stepGoto(s16 x, s16 y, s16 z);
void stepSetZero(void);
void stepReportAxes(void);

void coilAxisXAOff(void);
void coilAxisXAForward(void);
void coilAxisXABack(void);
void coilAxisXBOff(void);
void coilAxisXBForward(void);
void coilAxisXBBack(void);

void coilAxisYAOff(void);
void coilAxisYAForward(void);
void coilAxisYABack(void);
void coilAxisYBOff(void);
void coilAxisYBForward(void);
void coilAxisYBBack(void);

#endif // INCLUDE_STEPPER

