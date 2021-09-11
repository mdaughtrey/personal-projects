//#ifndef INCLUDE_STEPPER
//#define INCLUDE_STEPPER

#include "AsyncStepperLib.h"

namespace Stepper 
//class Stepper
{
//public:
//    Stepper();
//    static Stepper * Get();
    void setOutput(int step);
    void rotateCW();
    void rotateCCW();
    void loop();
    void ccw();
    void cw();
    void stop();
    void clockwise();
    void anticlockwise();
//protected:
//    AsyncStepper stepper1;
    void init();
}; // namespace Stepper


//#endif // INCLUDE_STEPPER

