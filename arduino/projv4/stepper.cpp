#include <stdint.h>
#include <Arduino.h>
#include "stepper.h"

//namespace stepper
//{
//
//const uint8_t pinControl[] = { // Halfstep
//0b0000,
//0b1010,
//0b0010,
//0b0110,
//0b0100,
//0b0101,
//0b0001,
//0b1001,
//0b1000};
//
//const uint8_t numControl = sizeof(pinControl)/sizeof(pinControl[0]);
//
//const int motorPin4 = 18;
//const int motorPin3 = 19;
//const int motorPin2 = 20;
//const int motorPin1 = 21;
//int8_t delta;
//uint8_t stepIndex;
//uint32_t lastMove;

//int8_t getdelta() { return delta; }
//uint8_t getstepindex() { return stepIndex; }
//uint32_t getlastmove() { return lastMove; }
//uint8_t stepperInterval;


Stepper::Stepper(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4)
    : m_motorPin1(pin1), m_motorPin2(pin2), m_motorPin3(pin3), m_motorPin4(pin4), m_running(false),
    m_minInterval(0), m_maxInterval(0), m_rampUpSteps(0), m_rampDownSteps(0), m_enabled(false)
{
    cw();
}

void Stepper::run(uint16_t position)
{
    if (!m_running)
    {
        return;
    }
    if ((millis() - m_lastStepTime) < (uint32_t)m_currentInterval)
    {
        return;
    }

    digitalWrite(m_motorPin1, Stepper::pinControl[m_stepIndex] & 0b0001);
    digitalWrite(m_motorPin2, Stepper::pinControl[m_stepIndex] & 0b0010);
    digitalWrite(m_motorPin3, Stepper::pinControl[m_stepIndex] & 0b0100);
    digitalWrite(m_motorPin4, Stepper::pinControl[m_stepIndex] & 0b1000);
    m_stepIndex += m_delta;
    if (m_stepIndex == numControl)
    {
        m_stepIndex = 1;
    }
    else if (m_stepIndex == 0)
    {
        m_stepIndex = Stepper::numControl -1;
    }
    m_lastStepTime = millis();
    m_currentInterval = getNextInterval(position);
    Serial.printf("position %u m_stepIndex %u m_currentInterval %u\r\n",
            position, m_stepIndex, m_currentInterval);
}

void Stepper::start(uint16_t steps)
{
    if (0 == m_minInterval || !m_enabled)
    {
        return;
    }
    m_stepIndex = 1;
    m_totalSteps = steps;
    m_running = true;
    m_currentInterval = getNextInterval(0);
}

void Stepper::cw()
{
    m_delta = -1;
}

void Stepper::ccw()
{
    m_delta = 1;
}

uint8_t Stepper::getNextInterval(uint16_t position)
{
    uint8_t intervalRange = m_maxInterval - m_minInterval;
    if (position > m_totalSteps)
    {
        return m_maxInterval;
    }
    if (position < m_rampUpSteps)
    {
        return m_minInterval + static_cast<int>(intervalRange * ((m_rampUpSteps - position) / m_rampUpSteps));
    }
    if (position < (m_totalSteps - m_rampDownSteps))
    {
        return m_minInterval;
    }
    return m_minInterval + static_cast<int>(intervalRange * ((m_rampDownSteps - position) / m_rampDownSteps));
}

void Stepper::enable()
{
    pinMode(m_motorPin1, OUTPUT);
    pinMode(m_motorPin2, OUTPUT);
    pinMode(m_motorPin3, OUTPUT);
    pinMode(m_motorPin4, OUTPUT);
    m_enabled = true;
}

void Stepper::disable()
{
    stop();
    m_enabled = false;
    pinMode(m_motorPin1, INPUT);
    pinMode(m_motorPin2, INPUT);
    pinMode(m_motorPin3, INPUT);
    pinMode(m_motorPin4, INPUT);
}

void Stepper::stop(void)
{
    digitalWrite(m_motorPin1, 0);
    digitalWrite(m_motorPin2, 0);
    digitalWrite(m_motorPin3, 0);
    digitalWrite(m_motorPin4, 0);
    m_running = false;
}

const uint8_t Stepper::pinControl[] = { // Halfstep
0b0000,
0b1010,
0b0010,
0b0110,
0b0100,
0b0101,
0b0001,
0b1001,
0b1000};

const uint8_t Stepper::numControl = sizeof(pinControl)/sizeof(pinControl[0]);
//void init()
//{
//    pinMode(motorPin1, OUTPUT);
//    pinMode(motorPin2, OUTPUT);
//    pinMode(motorPin3, OUTPUT);
//    pinMode(motorPin4, OUTPUT);
//    stop();
//    lastMove = millis();
//    stepIndex = 1;
//}
//
//void cw()
//{
//    delta = -1;
////    stepIndex = numControl - 1;
//}
//
//void coilCtrl(uint8_t a, uint8_t l)
//{
//    if (0 == a)
//    {
//        digitalWrite(motorPin1, l);
//        digitalWrite(motorPin2, !l);
//    }
//    else
//    {
//        digitalWrite(motorPin3, l);
//        digitalWrite(motorPin4, !l);
//    }
//}
//
//void coilAPos()
//{
//    digitalWrite(motorPin1, 1);
//    digitalWrite(motorPin2, 0);
//}
//
//void ccw()
//{
//    delta = 1;
////    stepIndex = 1;
//}
//
//// void stepperGo()
//// {
////     stepIndex = numControl - 1;
//// }
////
//} // namespace stepper
