#include <stepper.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define COILA_PLUS_ON PORTF |= _BV(4)
#define COILA_PLUS_OFF PORTF &= ~_BV(4)
#define COILA_MINUS_ON PORTF |= _BV(3)
#define COILA_MINUS_OFF PORTF &= ~_BV(3)

#define COILB_PLUS_ON PORTF |= _BV(2)
#define COILB_PLUS_OFF PORTF &= ~_BV(2)
#define COILB_MINUS_ON PORTF |= _BV(1)
#define COILB_MINUS_OFF PORTF &= ~_BV(1)

unsigned char stepSequence[] PROGMEM = 
{
    0x00, // idle
	0x10, // 0001 0000
	0x04, // 0000 0100
	0x08, // 0000 1000
	0x02, // 0000 0010
    0x00 //idle
};

static char * stepPtr;

void stepperInit(void)
{
        DDRF = 0x1e; // PF1,2,3,4
        PORTF &= ~0x1e; // set all bits low
        stepPtr = stepSequence;
}

void stepper1Forward(void)
{
        unsigned char data = pgm_read_byte_near(stepPtr);
        if (0 == data)
        {
                stepPtr = stepSequence + 1;
                 data = pgm_read_byte_near(stepPtr);
        }
        stepPtr++;
        PORTF = data;
}

void stepper1Back(void)
{
        unsigned char data = pgm_read_byte_near(stepPtr);
        if (0 == data)
        {
                stepPtr = stepSequence + 4;
                data = pgm_read_byte_near(stepPtr);
        }
        stepPtr--;
        PORTF = data;
}

void stepper1Stop(void)
{
 	stepperInit();
}

void coilAForward(void)
{
	COILA_PLUS_ON;
	COILA_MINUS_OFF;
}

void coilABackward(void)
{
	COILA_PLUS_OFF;
	COILA_MINUS_ON;
}

void coilBForward(void)
{
	COILB_PLUS_ON;
	COILB_MINUS_OFF;
}

void coilBBackward(void)
{
	COILB_PLUS_OFF;
	COILB_MINUS_ON;
}

