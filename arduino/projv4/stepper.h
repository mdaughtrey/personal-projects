namespace stepper
{
void init(void);
uint8_t poll(uint8_t stepDelay);
void stop(void);
void cw();
void ccw();
int8_t getdelta();
uint8_t getstepindex();
uint32_t getlastmove();
void coilCtrl(uint8_t a, uint8_t l);
}
