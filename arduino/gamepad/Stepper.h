// A B a b
// 1 1 0 0
// 1 0 0 1
// 0 0 1 1
// 0 1 1 0
//
namespace {
const uint8_t fullStep[] PROGMEM  = { 4, 0b1000, 0b0100, 0b0010, 0b0001 };
}

class Stepper
{
    protected:
//        const uint8_t p0;
//        const uint8_t p1;
//        const uint8_t p2;
//        const uint8_t p3;
        const uint8_t p0, p1, p2, p3;
        int8_t delta;
        int8_t index;
        uint16_t stepCycle;
        uint8_t lastCycle;

    public:
        Stepper(const uint8_t pin0, const uint8_t pin1, const uint8_t pin2, const uint8_t pin3)
        : p0(pin0), p1(pin1), p2(pin2), p3(pin3)
//            fullStep({0b1000, 0b0100,  0b0010, 0b0001}),
            //pinControl({0b1000, 0b0000, 0b0100, 0b0000, 0b0010, 0b0000, 0b0001, 0b0000})
        {
            init();
            mode(0);
        }
        void mode(uint8_t stepMode)
        {
            switch (stepMode) {
                case 0: // fullstep
                    stepCycle = fullStep;
                    lastCycle = pgm_read_byte_near(stepCycle);
                    break;
            }
        }
        void init()
        {
            pinMode(p0, OUTPUT);
            digitalWrite(p0, 0);
            pinMode(p1, OUTPUT);
            digitalWrite(p1, 0);
            pinMode(p2, OUTPUT);
            digitalWrite(p2, 0);
            pinMode(p3, OUTPUT);
            digitalWrite(p3, 0);
            delta = 0;
            index = 1;
        }
        void dir(const uint8_t pdir) { delta = pdir ? 1: -1; }
        void next() 
        {
            index += delta;
            if (index < 1) { index = lastCycle; }
            if (index == lastCycle) { index = 1; }
            uint8_t thisCycle = pgm_read_byte_near(stepCycle + index);
            digitalWrite(p0, (thisCycle & 0b1000 ? 1: 0));
            digitalWrite(p1, (thisCycle & 0b0100 ? 1: 0));
            digitalWrite(p2, (thisCycle & 0b0010 ? 1: 0));
            digitalWrite(p3, (thisCycle & 0b0001 ? 1: 0));
        }
        void a0Plus() { digitalWrite(p0, 1); }
        void a0Minus() { digitalWrite(p0, 0); }
        void a1Plus() { digitalWrite(p1, 1); }
        void a1Minus() { digitalWrite(p1, 0); }
        void b0Plus() { digitalWrite(p2, 1); }
        void b0Minus() { digitalWrite(p2, 0); }
        void b1Plus() { digitalWrite(p3, 1); }
        void b1Minus() { digitalWrite(p3, 0); }

//        static uint8_t fullStep[5];

};



