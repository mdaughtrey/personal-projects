#include "defs.h"
#include <pico/stdlib.h>

#define MEMO
#ifdef MEMO
#include <deque>

typedef struct
{
    uint16_t requestedmoves;
    uint16_t targetsteps;
    uint16_t stepcount;
    uint64_t currentinterval;
    uint64_t stepspermove;
    uint64_t m_minInterval64;
    uint64_t m_maxInterval64;
} Memo;
#endif // MEMO


class Stepper
{
    public:
        static const uint8_t pinControl[];
        static const uint8_t numControl;
#ifdef MEMO
        std::deque<Memo> m_memo;
#endif // MEMO
#ifdef SIGMOID
        uint16_t  m_rampUpSteps;
        uint16_t  m_rampDownSteps;
#else // SIGMOID
        float  m_rampUpSteps;
        float  m_rampDownSteps;
#endif  // SIGMOID
        uint64_t m_minInterval64;
        uint64_t m_maxInterval64;
        uint64_t m_testmember;
        uint8_t m_stepperEnable;
        uint8_t m_stepperDir;
        uint8_t m_stepperPulse;
//        uint16_t m_currentInterval;
        uint64_t m_currentInterval64;
//        uint64_t m_lastStepTime;
        uint64_t m_lastStepTime64;
//        uint16_t m_currentStep;
        uint16_t m_stepCount;
        uint16_t m_targetSteps;
        uint16_t m_targetMove;
        uint16_t m_stepsPerMove;
        uint16_t m_stepCountAtLastPos;
        uint16_t m_lastEncoderPos;
       // float m_stepsPerMove;

        bool m_running;
        bool m_enabled;
        bool m_verbose;

    public:
        Stepper(uint8_t stepperEnable, uint8_t stepperDir, uint8_t stepperPulse);
        uint8_t run(uint16_t pos);
        uint8_t run(void);
        void start(uint16_t moves);
        void start(void);
        void stop(uint16_t move = 0);
        void cw();
        void ccw();
        void enable();
        void disable();
#ifdef MEMO
        void dumpMemo();
        void saveMemo(uint16_t);
#endif // MEMO
//        uint8_t minInterval() { return m_minInterval; }
//        uint8_t minInterval(uint8_t interval) { m_minInterval = interval; return m_minInterval; }
//        uint8_t maxInterval() { return m_maxInterval; }
//        uint8_t maxInterval(uint8_t interval) { m_maxInterval = interval; return m_maxInterval; }

//        uint64_t minInterval64() { Serial.printf("getminInterval64()\r\n"); return m_minInterval64; }
//        uint64_t minInterval64(uint64_t interval) { Serial.printf("setminInterval64 %lu\r\n", interval);
//            m_minInterval64 = interval; dumpInternalState(); return m_minInterval64; }
//
//        uint64_t maxInterval64() { Serial.printf("getmaxInterval64\r\n"); return m_maxInterval64; }
//        uint64_t maxInterval64(uint64_t interval) { Serial.printf("setmaxInterval64 this %lu\r\n", interval); 
//            m_maxInterval64 = interval; dumpInternalState(); return m_maxInterval64; }

//        uint8_t rampUpSteps() { return static_cast<int>(m_rampUpSteps); }
//        uint8_t rampUpSteps(uint8_t steps) { m_rampUpSteps = static_cast<float>(steps); return steps; }
//        uint8_t rampDownSteps() { return static_cast<int>(m_rampDownSteps); }
//        uint8_t rampDownSteps(uint8_t steps) { m_rampDownSteps = static_cast<float>(steps); return steps; }
        bool enabled() { return m_enabled; }
        void verbose(uint8_t v) { m_verbose = v; };
        void pulse(bool v);
        void dumpInternalState(void);
//        void reset();


    protected:
        uint8_t getNextInterval(uint16_t position);
        uint64_t getNextInterval64(uint16_t position);
};
