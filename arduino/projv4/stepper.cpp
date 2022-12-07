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
    m_minInterval(0), m_maxInterval(0), m_rampUpSteps(0.0), m_rampDownSteps(0.0), m_enabled(false),
    m_stepsPerMove(20)
{
    cw();
}

void Stepper::run()
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
    m_stepCount++;
    m_lastStepTime = millis();
    m_currentInterval = getNextInterval(m_stepCount);
    if (m_verbose) 
    {
        Serial.printf("m_stepcount %4u m_stepsPerMove %4u m_targetSteps %4u m_currentInterval %4u\r\n",
                m_stepCount, m_stepsPerMove, m_targetSteps, m_currentInterval);
    }
}

void Stepper::start(uint16_t moves)
{
    if (0 == m_minInterval || !m_enabled)
    {
        return;
    }
    m_stepCount = 0;
    m_stepIndex = 1;
//    m_moves = moves;
    m_running = true;
    m_targetSteps = int(moves * m_stepsPerMove);
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
    if (position > m_targetSteps)
    {
        if (m_verbose) { Serial.printf("gni 0 "); }
        return m_maxInterval;
    }
    if (position < m_rampUpSteps)
    {
        float m = (m_rampUpSteps - position) / m_rampUpSteps;
        float n = intervalRange * m;
        uint8_t o = m_minInterval + static_cast<int>(n);
        if (m_verbose)
        {
            Serial.printf("GNI Clause 1: m %f n %f o %u\r\n", m, n, o);
        }
        return o;
        //return m_minInterval + static_cast<int>(intervalRange * ((m_rampUpSteps - position) / m_rampUpSteps));
    }
    if (position < (m_targetSteps - m_rampDownSteps))
    {
        if (m_verbose) { Serial.printf("gni 2 "); }
        return m_minInterval;
    }

    float p = 1.0 - (m_targetSteps - position) / m_rampDownSteps;
    float q = intervalRange * p;
    uint8_t r = m_minInterval + static_cast<int>(q);
    if (m_verbose)
    {
        Serial.printf("GNI Clause 3: p %f q %f r %u\r\n", p, q, r);
    }
    return r;
    //return m_minInterval + static_cast<int>(intervalRange * ((m_rampDownSteps - position) / m_rampDownSteps));
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

void Stepper::stop(uint16_t move)
{
    digitalWrite(m_motorPin1, 0);
    digitalWrite(m_motorPin2, 0);
    digitalWrite(m_motorPin3, 0);
    digitalWrite(m_motorPin4, 0);
    m_running = false;
    if (move)
    {
        m_stepsPerMove = int(m_stepCount/move);
    }
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
