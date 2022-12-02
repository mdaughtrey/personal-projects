class Stepper
{
    public:
        static const uint8_t pinControl[];
        static const uint8_t numControl;
        uint8_t m_minInterval;
        uint8_t m_maxInterval;
        uint8_t m_rampUpSteps;
        uint8_t m_rampDownSteps;
        uint8_t m_motorPin4;
        uint8_t m_motorPin3;
        uint8_t m_motorPin2;
        uint8_t m_motorPin1;
        uint16_t m_currentInterval;

        uint8_t m_stepIndex;

        uint32_t m_lastStepTime;
        int8_t m_delta;
//        uint16_t m_currentStep;
        uint16_t m_totalSteps;
        bool m_running;
        bool m_enabled;

    public:
        Stepper(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4);
        void run(uint16_t position);
        void start(uint16_t steps);
        void stop();
        void cw();
        void ccw();
        void enable();
        void disable();

        uint8_t minInterval() { return m_minInterval; }
        uint8_t minInterval(uint8_t interval) { m_minInterval = interval; return m_minInterval; }
        uint8_t maxInterval() { return m_maxInterval; }
        uint8_t maxInterval(uint8_t interval) { m_maxInterval = interval; return m_maxInterval; }
        uint8_t rampUpSteps() { return m_rampUpSteps; }
        uint8_t rampUpSteps(uint8_t steps) { m_rampUpSteps = steps; return m_rampUpSteps; }
        uint8_t rampDownSteps() { return m_rampDownSteps; }
        uint8_t rampDownSteps(uint8_t steps) { m_rampDownSteps = steps; return m_rampDownSteps; }
        bool enabled() { return m_enabled; }


    protected:
        uint8_t getNextInterval(uint16_t position);
};
