#include <stdint.h>
#include <Arduino.h>

namespace stepper
{

const uint8_t pinControl[] = { // Halfstep
0b0000,
0b1010,
0b0010,
0b0110,
0b0100,
0b0101,
0b0001,
0b1001,
0b1000};

const uint8_t numControl = sizeof(pinControl)/sizeof(pinControl[0]);

const int motorPin4 = 18;
const int motorPin3 = 19;
const int motorPin2 = 20;
const int motorPin1 = 21;
int8_t delta;
uint8_t stepIndex;
uint32_t lastMove;

int8_t getdelta() { return delta; }
uint8_t getstepindex() { return stepIndex; }
uint32_t getlastmove() { return lastMove; }

uint8_t poll(uint8_t stepDelay)
{
    if (((millis() - lastMove) < (uint32_t)stepDelay) | !stepIndex)
    {
        return 0;
    }

    lastMove = millis();
    digitalWrite(motorPin1, pinControl[stepIndex] & 0b0001);
    digitalWrite(motorPin2, pinControl[stepIndex] & 0b0010);
    digitalWrite(motorPin3, pinControl[stepIndex] & 0b0100);
    digitalWrite(motorPin4, pinControl[stepIndex] & 0b1000);
    stepIndex += delta;
    if (stepIndex == numControl)
    {
        stepIndex = 1;
    }
    else if (stepIndex == 0)
    {
        stepIndex = numControl -1;
    }
    return 1;
}

void stop(void)
{
    digitalWrite(motorPin1, 0);
    digitalWrite(motorPin2, 0);
    digitalWrite(motorPin3, 0);
    digitalWrite(motorPin4, 0);
//    delta = 0;
//    stepIndex = 0;
}

void init()
{
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(motorPin3, OUTPUT);
    pinMode(motorPin4, OUTPUT);
    stop();
    lastMove = millis();
    stepIndex = 1;
}

void cw()
{
    delta = -1;
//    stepIndex = numControl - 1;
}

void coilCtrl(uint8_t a, uint8_t l)
{
    if (0 == a)
    {
        digitalWrite(motorPin1, l);
        digitalWrite(motorPin2, !l);
    }
    else
    {
        digitalWrite(motorPin3, l);
        digitalWrite(motorPin4, !l);
    }
}

void coilAPos()
{
    digitalWrite(motorPin1, 1);
    digitalWrite(motorPin2, 0);
}

void ccw()
{
    delta = 1;
//    stepIndex = 1;
}

// void stepperGo()
// {
//     stepIndex = numControl - 1;
// }
//
} // namespace stepper
