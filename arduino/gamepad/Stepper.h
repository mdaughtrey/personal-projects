// A B a b
// 1 1 0 0
// 1 0 0 1
// 0 0 1 1
// 0 1 1 0
//
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
        uint8_t pinControl[4]; //  = { 0b1100, 0b1001, 0b0011, 0b0110 };

    public:
        Stepper(const uint8_t pin0, const uint8_t pin1, const uint8_t pin2, const uint8_t pin3)
        : p0(pin0), p1(pin1), p2(pin2), p3(pin3),
            pinControl({0b1000, 0b0100,  0b0010, 0b0001})
            //pinControl({0b1000, 0b0000, 0b0100, 0b0000, 0b0010, 0b0000, 0b0001, 0b0000})
        {
            init();
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
            index = 0;
        }
        void dir(const uint8_t pdir) { delta = pdir ? 1: -1; }
        void next() 
        {
            index += delta;
            if (index < 0) { index = 3; }
            if (index > 3) { index = 0; }
            digitalWrite(p0, (pinControl[index] & 0b1000 ? 1: 0));
            digitalWrite(p1, (pinControl[index] & 0b0100 ? 1: 0));
            digitalWrite(p2, (pinControl[index] & 0b0010 ? 1: 0));
            digitalWrite(p3, (pinControl[index] & 0b0001 ? 1: 0));
        }
        void a0Plus() { digitalWrite(p0, 1); }
        void a0Minus() { digitalWrite(p0, 0); }
        void a1Plus() { digitalWrite(p1, 1); }
        void a1Minus() { digitalWrite(p1, 0); }
        void b0Plus() { digitalWrite(p2, 1); }
        void b0Minus() { digitalWrite(p2, 0); }
        void b1Plus() { digitalWrite(p3, 1); }
        void b1Minus() { digitalWrite(p3, 0); }

};



