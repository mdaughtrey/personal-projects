#include "defs.h"
#include <stdint.h>
#include <Arduino.h>
#include "stepper.h"
//#include "qserialout.h"
#ifdef SIGMOID
#include "sigmoid.h"
#endif // SIGMOID


Stepper::Stepper(uint8_t stepperEnable, uint8_t stepperDir, uint8_t stepperPulse)
    : m_stepperEnable(stepperEnable), m_stepperDir(stepperDir), m_stepperPulse(stepperPulse), m_running(false),
    //m_minInterval(0), m_maxInterval(0), m_rampUpSteps(0.0), m_rampDownSteps(0.0), m_enabled(false),
#ifdef SIGMOID
    m_minInterval64(0), m_maxInterval64(0), m_rampUpSteps(0), m_rampDownSteps(0.0), m_enabled(false),
#else
    m_minInterval64(0), m_maxInterval64(0), m_rampUpSteps(0), m_rampDownSteps(0.0), m_enabled(false),
#endif // SIGMOID
    m_stepsPerMove(25) 
{
    pinMode(m_stepperPulse, INPUT_PULLUP);
    pinMode(m_stepperDir, INPUT_PULLUP);
    pinMode(m_stepperEnable, INPUT_PULLUP);
    digitalWrite(m_stepperPulse, 0);
    digitalWrite(m_stepperDir, 0);
    digitalWrite(m_stepperEnable, 0);
    cw();
}

uint8_t Stepper::run(uint16_t encoderPos)
{
    if (!m_running)
    {
        return 0;
    }

//    if (m_verbose) 
//    {
//        //Serial.printf("m_lastStepTime64 %lu delta %u\r\n", m_lastStepTime64, time_us_64() - m_lastStepTime64);
//        Serial.printf("m_lastStepTime64 %llu delta %llu\r\n", m_lastStepTime64, time_us_64() - m_lastStepTime64);
//    }
//    if ((millis() - m_lastStepTime) < (uint32_t)m_currentInterval)
//    {
//        return;
//    }
    if (encoderPos != m_lastEncoderPos)
    {
        m_stepsPerMove = m_stepCount -  m_stepCountAtLastPos;
        m_targetSteps = m_stepsPerMove * m_targetMove;
        m_lastEncoderPos = encoderPos;
        m_stepCountAtLastPos = m_stepCount;
    }
    if ((time_us_64() - m_lastStepTime64) < m_currentInterval64)
    {
        return 0;
    }

    pinMode(m_stepperPulse, OUTPUT);
    delayMicroseconds(100);
    pinMode(m_stepperPulse, INPUT_PULLUP);
    //digitalWrite(m_stepperPulse, 1);
    m_stepCount++;
    m_lastStepTime64 = time_us_64();
    m_currentInterval64 = getNextInterval64(m_stepCount);
    if (m_verbose) 
    {
        Serial.printf("encoderPos %u m_stepcount %4u m_stepsPerMove %u m_targetSteps %4u m_currentInterval64 %llu\r\n",
                encoderPos, m_stepCount, m_stepsPerMove, m_targetSteps, m_currentInterval64);
    }
    return 0;
}

uint8_t Stepper::run(void)
{
    if (!m_running)
    {
        return 0;
    }

    if ((time_us_64() - m_lastStepTime64) < m_currentInterval64)
    {
        return 0;
    }

    pinMode(m_stepperPulse, OUTPUT);
    delayMicroseconds(100);
    pinMode(m_stepperPulse, INPUT_PULLUP);
    m_lastStepTime64 = time_us_64();
    return 1;
}


void Stepper::start(uint16_t moves)
{
    if (0 == m_minInterval64 || !m_enabled)
    {
        return;
    }
    m_stepCount = 0;
    m_targetMove = moves;
    m_stepCountAtLastPos = 0;
    m_lastEncoderPos = 0;
    m_running = true;
    m_targetSteps = moves * m_stepsPerMove;
    m_currentInterval64 = getNextInterval64(0);
    if (m_verbose)
    {
        Serial.printf("Stepper::start m_targetSteps %u moves %u m_stepsPerMove %u\r\n",
                m_targetSteps, moves, m_stepsPerMove);
    }
    saveMemo(moves);
}

void Stepper::start(void)
{
    if (!m_enabled)
    {
        return;
    }
//    m_stepCount = 0;
//    m_targetMove = moves;
//    m_stepCountAtLastPos = 0;
//    m_lastEncoderPos = 0;
    m_lastStepTime64 = time_us_64();
    m_running = true;
    m_currentInterval64 = 500;
//    m_targetSteps = moves * m_stepsPerMove;
//    m_currentInterval64 = getNextInterval64(0);
//    if (m_verbose)
//    {
//        Serial.printf("Stepper::start m_targetSteps %u moves %u m_stepsPerMove %u\r\n",
//                m_targetSteps, moves, m_stepsPerMove);
//    }
//    saveMemo(moves);
}

void Stepper::cw()
{
    pinMode(m_stepperDir, INPUT_PULLUP);
}

void Stepper::ccw()
{
    pinMode(m_stepperDir, OUTPUT);
}

#ifdef MEMO
void Stepper::saveMemo(uint16_t requestedmoves)
{
    Memo memo = {requestedmoves, m_targetSteps, m_stepCount, m_currentInterval64, m_stepsPerMove, m_minInterval64, m_maxInterval64};
    m_memo.push_back(memo);
    //m_memo.push_back({m_targetSteps, m_stepCount, m_currentInterval64, m_stepsPerMove});
    while(m_memo.size() > 4000)
    {
        m_memo.pop_front();
    }
}
#endif // MEMO
#ifdef SIGMOID
uint64_t Stepper::getNextInterval64(uint16_t position)
{
    uint64_t intervalRange = m_maxInterval64 - m_minInterval64;
    if (m_verbose)
    {
        Serial.printf("GNI: position %u m_rampUpSteps %u m_rampDownSteps %u m_targetSteps %u\r\n",
                position, m_rampUpSteps, m_rampDownSteps, m_targetSteps);
    }
    if (position > m_targetSteps)
    {
        if (m_verbose) { Serial.printf("GNI0:"); }
        return m_maxInterval64;
    }
    if (position < m_rampUpSteps)
    {
        uint16_t sigIndex = NUM_SIGMOID - 1 - (position * static_cast<int>((NUM_SIGMOID/m_rampUpSteps)));
        float m = sigmoid[sigIndex];
        float n = intervalRange * m;
        uint64_t o = m_minInterval64 + static_cast<int>(n);
        if (m_verbose)
        {
            Serial.printf("GNI1: position %u sigIndex %u m %f n %f o %u\r\n", position, sigIndex, m, n, o);
        }
        return o;
    }
    if (position < (m_targetSteps - m_rampDownSteps))
    {
        if (m_verbose) { Serial.printf("GNI2:"); }
        return m_minInterval64;
    }
    uint16_t sigIndex = (position % (m_targetSteps - m_rampDownSteps)) * static_cast<int>(NUM_SIGMOID/m_rampDownSteps);
    float m = sigmoid[sigIndex];
    float n = intervalRange * m;
    uint64_t o = m_minInterval64 + static_cast<int>(n);
    if (m_verbose)
    {
        Serial.printf("GNI3: position %u sigIndex %u m %f n %f o %u\r\n", position, sigIndex, m, n, o);
    }
    return o;

//    return r;
}
#else // SIGMOID
#ifdef SIMPLEINTERVAL
uint64_t Stepper::getNextInterval64(uint16_t position)
{
    return m_minInterval64;
}

#else // SIMPLEINTERVAL
uint64_t Stepper::getNextInterval64(uint16_t position)
{
    uint64_t intervalRange = m_maxInterval64 - m_minInterval64;
    if (position > m_targetSteps)
    {
        if (m_verbose) { Serial.printf("gni 0 "); }
        return m_maxInterval64;
    }
    if (position < m_rampUpSteps)
    {
        float m = (m_rampUpSteps - position) / m_rampUpSteps;
        float n = intervalRange * m;
        uint8_t o = m_minInterval64 + static_cast<int>(n);
        if (m_verbose)
        {
            Serial.printf("GNI Clause 1: m %f n %f o %u\r\n", m, n, o);
        }
        return o;
    }
    if (position < (m_targetSteps - m_rampDownSteps))
    {
        if (m_verbose) { Serial.printf("gni 2 "); }
        return m_minInterval64;
    }

    float p = 1.0 - (m_targetSteps - position) / m_rampDownSteps;
    float q = intervalRange * p;
    uint8_t r = m_minInterval64 + static_cast<int>(q);
    if (m_verbose)
    {
        Serial.printf("GNI Clause 3: p %f q %f r %u\r\n", p, q, r);
    }
    return r;
}
#endif // SIMPLEINTERVAL
#endif // SIGMOID

void Stepper::pulse(bool v)
{
    if (v)
    {
        pinMode(m_stepperPulse, INPUT_PULLUP);
        delayMicroseconds(10);
        pinMode(m_stepperPulse, OUTPUT);
    }
    else
    {
        pinMode(m_stepperPulse, OUTPUT);
        delayMicroseconds(10);
        pinMode(m_stepperPulse, INPUT_PULLUP);
    }
}

void Stepper::enable()
{
    pinMode(m_stepperEnable, INPUT_PULLUP);
    m_enabled = true;
}

void Stepper::disable()
{
    stop();
    m_enabled = false;
    pinMode(m_stepperEnable, OUTPUT);
}

void Stepper::stop(uint16_t move)
{
    m_running = false;
    if (m_verbose)
    {
        Serial.printf("Stepper::stop m_stepCount %u move %u m_stepsPerMove %u\r\n", m_stepCount, move, m_stepsPerMove);
    }
//    if (move)
//    {
//        m_stepsPerMove = m_stepCount / (float)move;
//    }
}

//void Stepper::reset()
//{
//    m_minInterval64 = 100;
//    m_maxInterval64 = 1000;
//    m_rampUpSteps = 10;
//    m_rampDownSteps = 10;
//    m_stepCount = 0;
//    cw(); 
//}

#ifdef MEMO
void Stepper::dumpMemo()
{
    std::deque<Memo>::iterator iter;

    for (iter = m_memo.begin(); iter != m_memo.end(); iter++)
    {
        Serial.printf("requestedmoves %u, target %4u count %4u interval %llu permove %u mini %llu maxi %llu\r\n",
                iter->requestedmoves, iter->targetsteps, iter->stepcount, iter->currentinterval, iter->stepspermove,
                    iter->m_minInterval64, iter->m_maxInterval64);
    }

}
#endif // MEMO
