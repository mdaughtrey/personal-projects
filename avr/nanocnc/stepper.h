#ifndef INCLUDE_STEPPER
#define INCLUDE_STEPPER

#include <typedefs.h>

void stepperInit(void);
void stepper1Forward(void);
void stepper1Back(void);
void stepper1Stop(void);
void coilAForward(void);
void coilABackward(void);
void coilBForward(void);
void coilBBackward(void);
void coilAOff(void);
void coilBOff(void);
void stepForward(s08 count);
void stepBack(s08 count);
u08 nextState(s08 state);
u08 prevState(s08 state);

#endif // INCLUDE_STEPPER

