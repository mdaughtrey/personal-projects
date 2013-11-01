#include <stepper.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <typedefs.h>
#include <serutil.h>

// pin 16 PB6
// pin 17 PB7
// pin 18 PG3
// pin 19 PG4

#define COILA_PLUS_ON PORTB |= _BV(6)
#define COILA_PLUS_OFF PORTB &= ~_BV(6)
#define COILA_MINUS_ON PORTB |= _BV(7)
#define COILA_MINUS_OFF PORTB &= ~_BV(7)

#define COILB_PLUS_ON PORTG |= _BV(3)
#define COILB_PLUS_OFF PORTG &= ~_BV(3)
#define COILB_MINUS_ON PORTG |= _BV(4)
#define COILB_MINUS_OFF PORTG &= ~_BV(4)

#define LAST_STATE 4

s08 stepSequence;

void coilState(u08 state)
{
//    uart_send_hex_byte(state);
//    uart_send_buffered(' ' );
    switch (state)
    {
    case 0: 
        coilAForward();
        coilBOff();
        break;

    case 1:
        coilAForward();
        coilBForward();
        break;

    case 2:
        coilABackward();
        coilBForward();
        break;

    case 3:
        coilABackward();
        coilBBackward();
        break;

    case 4:
        coilAOff();
        coilBBackward();
        break;
    }
}

void stepperInit(void)
{
    DDRB = 0xc0;
    PORTB &= ~0xc0;

    DDRG = 0x18;
    DDRG &= ~0x18;
    stepSequence = 0;
    coilAOff();
    coilBOff();
}

void stepper1Forward(void)
{
    stepSequence++;
    if (stepSequence > LAST_STATE)
    {
        stepSequence = 0;
    }
    coilState(stepSequence);
}

void stepper1Back(void)
{
    stepSequence--;
    if (stepSequence < 0)
    {
        stepSequence = LAST_STATE;
    }
    coilState(stepSequence);
}

void stepper1Stop(void)
{
    stepSequence = 0;
    coilState(0);
}

void coilAOff(void)
{
	COILA_PLUS_OFF;
	COILA_MINUS_OFF;
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

void coilBOff(void)
{
	COILB_PLUS_OFF;
	COILB_MINUS_OFF;
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

void stepForward(s08 count)
{
    u08 state;
    for (state = 0; state < count * LAST_STATE; state++)
    {
        coilState(state % (LAST_STATE + 1));
        _delay_ms(25);
    }
}

void stepBack(s08 count)
{
    count *= LAST_STATE;
    while (1)
    {
        coilState(count % (LAST_STATE + 1));
        count--;
        _delay_ms(25);
        if (0 == count)
        {
            return;
        }
    }
}

