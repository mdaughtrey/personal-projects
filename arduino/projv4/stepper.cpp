#include "AsyncStepperLib.h"
#include "stepper.h"

namespace Stepper {
const int motorPin1 = 24;
const int motorPin2 = 25;
const int motorPin3 = 26;
const int motorPin4 = 27;
const int numSteps = 8;
const int stepsLookup[8] = { 0b1000, 0b1100, 0b0100, 0b0110, 0b0010, 0b0011, 0b0001, 0b1001 };
int stepCounter = 0;
const int stepsPerRevolution = 4076;

AsyncStepper stepper1(stepsPerRevolution, clockwise, anticlockwise);
//void jumpCW()
//{
//    Stepper::Get()->clockwise();
//}
//
//void jumpCCW()
//{
//    Stepper::Get()->anticlockwise();
//}

//Stepper::Stepper()
//    : stepper1(stepsPerRevolution, Stepper::clockwise, Stepper::anticlockwise)
////        [](Stepper *self) {Stepper::clockwise(self); },
////        [](Stepper *self) {Stepper::anticlockwise(self); })
//{
//    init();
//}

void clockwise()
{
    stepCounter++;
    if (stepCounter >= numSteps) stepCounter = 0;
    setOutput(stepCounter);
}

void anticlockwise()
{
    stepCounter--;
    if (stepCounter < 0) stepCounter = numSteps - 1;
    setOutput(stepCounter);
}

void setOutput(int step)
{
    digitalWrite(motorPin1, bitRead(stepsLookup[step], 0));
    digitalWrite(motorPin2, bitRead(stepsLookup[step], 1));
    digitalWrite(motorPin3, bitRead(stepsLookup[step], 2));
    digitalWrite(motorPin4, bitRead(stepsLookup[step], 3));
}

// AsyncStepper stepper1(stepsPerRevolution,
//         []() {clockwise(); },
//         []() {anticlockwise(); }
// );

void rotateCW()
{
    stepper1.Rotate(90, AsyncStepper::CW, rotateCCW);
}

void rotateCCW()
{
    stepper1.Rotate(90, AsyncStepper::CCW, rotateCW);
}

void init()
{
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(motorPin3, OUTPUT);
    pinMode(motorPin4, OUTPUT);

    stepper1.SetSpeedRpm(10);
//    stepper1.RotateContinuos(AsyncCCW);
}

void cw()
{
    stepper1.RotateContinuos(AsyncStepper::CW);
}

void ccw()
{
    stepper1.RotateContinuos(AsyncStepper::CCW);
}

void stop()
{
    stepper1.Stop();
}

void loop()
{
    stepper1.Update();
}

// Statics
//Stepper * Get()
//{
//    static Stepper * self;
//    if (NULL == self)
//    {
//        self = new Stepper();
//    }
//    return self;
//}

} // namespace Stepper
