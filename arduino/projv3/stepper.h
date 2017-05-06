#ifndef INCLUDE_STEPPER
#define INCLUDE_STEPPER

void stepperInit(void);
//void stepperDelay(s08 delta);
void stepperPoll(u08 stepDelay);
void stepperStop(void);
void stepperGo();
void coilAForward(void);
void coilABackward(void);
void coilAOff(void);
void coilBForward(void);
void coilBBackward(void);
void coilBOff(void);

#endif // INCLUDE_STEPPER

