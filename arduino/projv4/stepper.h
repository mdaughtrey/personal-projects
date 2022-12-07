class Stepper
{
    public:
        static const uint8_t pinControl[];
        static const uint8_t numControl;
        uint8_t m_minInterval;
        uint8_t m_maxInterval;
        float  m_rampUpSteps;
        float  m_rampDownSteps;
        uint8_t m_motorPin4;
        uint8_t m_motorPin3;
        uint8_t m_motorPin2;
        uint8_t m_motorPin1;
        uint16_t m_currentInterval;

        uint8_t m_stepIndex;

        uint32_t m_lastStepTime;
        int8_t m_delta;
//        uint16_t m_currentStep;
        uint16_t m_stepCount;
        uint16_t m_targetSteps;
        uint16_t m_stepsPerMove;

        bool m_running;
        bool m_enabled;
        bool m_verbose;

    public:
        Stepper(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4);
        void run();
        void start(uint16_t moves);
        void stop(uint16_t move = 0);
        void cw();
        void ccw();
        void enable();
        void disable();

        uint8_t minInterval() { return m_minInterval; }
        uint8_t minInterval(uint8_t interval) { m_minInterval = interval; return m_minInterval; }
        uint8_t maxInterval() { return m_maxInterval; }
        uint8_t maxInterval(uint8_t interval) { m_maxInterval = interval; return m_maxInterval; }
        uint8_t rampUpSteps() { return static_cast<int>(m_rampUpSteps); }
        uint8_t rampUpSteps(uint8_t steps) { m_rampUpSteps = static_cast<float>(steps); return steps; }
        uint8_t rampDownSteps() { return static_cast<int>(m_rampDownSteps); }
        uint8_t rampDownSteps(uint8_t steps) { m_rampDownSteps = static_cast<float>(steps); return steps; }
        bool enabled() { return m_enabled; }
        void verbose(uint8_t v) { m_verbose = v; };


    protected:
        uint8_t getNextInterval(uint16_t position);
};
