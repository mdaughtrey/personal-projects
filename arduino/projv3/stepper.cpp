#include <WProgram.h>
#include <avrlibtypes.h>
#include <stepper.h>

// COILA+ 1a Pin PD5
// COILA- 1b Pin PD4
// COILB+ 2a Pin PD3
// COILB- 2b Pin PD2

#define COILA_PLUS_ON PORTD |= _BV(5)
#define COILA_PLUS_OFF PORTD &= ~_BV(5)
#define COILA_MINUS_ON PORTD |= _BV(4)
#define COILA_MINUS_OFF PORTD &= ~_BV(4)

#define COILB_PLUS_ON PORTD |= _BV(3)
#define COILB_PLUS_OFF PORTD &= ~_BV(3)
#define COILB_MINUS_ON PORTD |= _BV(2)
#define COILB_MINUS_OFF PORTD &= ~_BV(2)

//  AQDE
//        coilBForward();
//	COILB_PLUS_ON;
//	COILB_MINUS_OFF;
//        coilAForward();
//	COILA_PLUS_ON;
//	COILA_MINUS_OFF;
//        coilBBackward();
//	COILB_PLUS_OFF;
//	COILB_MINUS_ON;
//        coilABackward();
//	COILA_PLUS_OFF;
//	COILA_MINUS_ON;

const unsigned char stepPortD[] = 
{
    0x00, // idle
	0x14, // 0001 0100
	0x24, // 0010 0100
	0x28, // 0010 1000
	0x18, // 0001 1000
    0x00 //idle
};

u08 stepIndex;
u32 lastMove;
s08 stepDelay;

void stepperDelay(s08 delta)
{
    stepDelay += delta;
    if (stepDelay < 0)
    {
        stepDelay = 0;
    }
    if (stepDelay > 30)
    {
        stepDelay = 30;
    }
}

void stepperPoll()
{
//    if (0 == stepIndex || 5 == stepIndex)
//    {
//        if (millis() - lastMove < stepDelay)
//        {
//            return;
//        }
//    }

    if ((millis() - lastMove > 1) && stepIndex)
    {
        lastMove = millis();
        PORTD |= stepPortD[stepIndex];
        PORTD &= ~(~stepPortD[stepIndex] & 0x3c);
        stepIndex--;
        if (0 == stepIndex)
        {
            stepIndex = 4;
        }
    }
}

void stepperInit()
{
    DDRD |= 0x3c; // 0011 1100
    PORTD &= ~0x3c; // set all bits low
    stepIndex = 0;
    lastMove = 0;
    stepDelay = 1;
}

void stepperGo()
{
    stepIndex = 4;
}

void stepperStop(void)
{
    stepIndex = 0;
    PORTD &= ~0x3c;
    DDRD &= ~0x3c;
 	//stepperInit();
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

void coilAOff(void)
{
	COILA_PLUS_OFF;
	COILA_MINUS_OFF;
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

void coilBOff(void)
{
	COILB_PLUS_OFF;
	COILB_MINUS_OFF;
}

