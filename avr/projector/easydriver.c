#include <stepper.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>

#define DRIVER_STEP_ON PORTF |= _BV(4)
#define DRIVER_STEP_OFF PORTF &= ~_BV(4)
#define DRIVER_FORWARD PORTF |= _BV(3)
#define DRIVER_REVERSE PORTF &= ~_BV(3)


void stepperInit(void)
{
        DDRF = 0x18; // PF3,4
        PORTF &= ~0x18; // set all bits low
}

void stepper1Forward(unsigned char speed, unsigned int steps)
{
    DRIVER_FORWARD;
    while (steps--)
    {
        DRIVER_STEP_ON;
        _delay_us(speed);
        DRIVER_STEP_OFF;
        _delay_us(speed);
    }
}

void stepper1Back(unsigned char speed, unsigned int steps)
{
    DRIVER_REVERSE;
    while (steps--)
    {
        DRIVER_STEP_ON;
        _delay_us(speed);
        DRIVER_STEP_OFF;
        _delay_us(speed);
    }
}

void stepper1Stop(void)
{
}

