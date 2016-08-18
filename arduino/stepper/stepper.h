#ifndef INCLUDE_STEPPER
#define INCLUDE_STEPPER

void stepperInit(void);
//void stepper1Forward(void);
//void stepper1Back(void);
void stepperDelay(s08 delta);
void stepperPoll();
void stepperStop(void);
void stepperEStop();
void stepperNext();
void coilAForward(void);
void coilABackward(void);
void coilAOff(void);
void coilBForward(void);
void coilBBackward(void);
void coilBOff(void);

#endif // INCLUDE_STEPPER

