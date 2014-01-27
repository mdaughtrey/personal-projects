#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <serutil.h>
#include <typedefs.h>

#include <string.h>
#include <stdio.h>

//
// 1. Curtain 1 Pin 51 PA0
#define CURTAIN1 5
// 2. Curtain 2 Pin 50 PA1
#define CURTAIN2 4
// 3. Solenoid 1 Pin 49 PA2
#define SOLENOID1 3
// 4. Solenoid 2 Pin 48 PA3
#define SOLENOID2 2
// 5. Power Pin 47 PA4
#define POWER 1
// 6. Shaft Pin 46 PA5
#define SHAFT 0
// 7. Ground
//
#define PRESCALER_MOD 4

typedef enum
{
    C1_INIT = 0,
    C1_POWERUP,
    C1_WAIT_SOL1,
    C1_WAIT_SOL2, 
    C1_WAIT_128MS,
    C1_LOW_10MS,
    C1_HIGH_322MS,
    C1_POWERDOWN
}C1State;

typedef enum
{
    C2_INIT = 0,
    C2_POWERUP,
    C2_SOLENOID2,
    C2_250,
    C2_POWERDOWN
}C2State;

typedef enum
{
    SHAFT_INIT = 0,
    SHAFT_POWERUP,
    SHAFT_100,
    SHAFT_190,
    SHAFT_POWERDOWN
}ShaftState;

const u08 StrIAmAlive [] PROGMEM = "I am alive.\r\n";
//const u08 StrIAwait [] PROGMEM = "I await.\r\n";
//const u08 StrImmediateMode [] PROGMEM = "Immediate Mode\r\n";
//const u08 StrCoordinateMode [] PROGMEM = "Coordinate Mode\r\n";
//const u08 StrLowLevelMode [] PROGMEM = "Low Level Mode\r\n";
//const u08 StrCoordInputIs [] PROGMEM = "cmdInput is ";
//const u08 StrInvalidCommand [] PROGMEM = "Invalid Command: ";
//const u08 StrParamCount [] PROGMEM = "ParamCount: ";
//const u08 StrOk [] PROGMEM = "OK\r\n";

u08 timerPrescale;
u16 c1TimerMs;
u16 c2TimerMs;
u16 shaftTimerMs;
C1State c1State;
C2State c2State;
ShaftState shaftState;

// 128us period
ISR(TIMER0_OVF_vect)
{
    if (++timerPrescale % PRESCALER_MOD)
    {
        return;
    }
    if (c1TimerMs)
    {
        c1TimerMs--;
    }
    if (c2TimerMs)
    {
        c2TimerMs--;
    }
    if (shaftTimerMs)
    {
        shaftTimerMs--;
    }
}

void delay20ms(unsigned char count)
{
    while (count--)
    {
        _delay_ms(20);
    }
}

void c1StateMachine(void)
{
    switch (c1State)
    {
        case C1_INIT:
            PORTA |= _BV(CURTAIN1);
            DDRA |= _BV(CURTAIN1);
            PORTA &= ~_BV(CURTAIN1);
            c1State = C1_POWERUP;
            break;
        //
        // Trigger - power line high
        //
        case C1_POWERUP:
            if (PINA & _BV(POWER))
            {
                PORTA |= _BV(CURTAIN1);
                c1State = C1_WAIT_SOL1;
            }
            break;

        //
        // Trigger solenoid 1 high
        //
        case C1_WAIT_SOL1: // wait for solenoid 1 trigger
            if (PINA & _BV(SOLENOID1))
            {
                c1State = C1_WAIT_SOL2;
            }
            break;

        //
        // Trigger solenoid 2 high
        //
        case C1_WAIT_SOL2: 
            if (PINA & _BV(SOLENOID2))
            {
                c1State = C1_WAIT_128MS;
                c1TimerMs = 114;
            }
            break;

        case C1_WAIT_128MS:
            if (0 == c1TimerMs)
            {
                PORTA &= ~_BV(CURTAIN1);
                c1State = C1_LOW_10MS;
                c1TimerMs = 10;
            }
            break;

        case C1_LOW_10MS:
            if (0 == c1TimerMs)
            {
                PORTA |= _BV(CURTAIN1);
                c1State = C1_HIGH_322MS;
                c1TimerMs = 120;
            }
            break;

        case C1_HIGH_322MS:
            if (0 == c1TimerMs)
            {
                PORTA &= ~_BV(CURTAIN1);
                c1State = C1_POWERDOWN;
            }
            break;

        //
        // Wait for power up
        //
        case C1_POWERDOWN:
            if (!(PINA & _BV(POWER)))
            {
                c1State = C1_POWERUP;
            }
            break;
    }
}

void c2StateMachine(void)
{
    switch (c2State)
    {
        case C2_INIT:
            PORTA |= _BV(CURTAIN2);
            DDRA |= _BV(CURTAIN2);
            PORTA &= ~_BV(CURTAIN2);
            c2State = C2_POWERUP;
            break;

        case C2_POWERUP:
            if (PINA & _BV(POWER))
            {
                PORTA |= _BV(CURTAIN2);
                c2State = C2_SOLENOID2;
            }
            break;

        case C2_SOLENOID2:
            if (PINA & _BV(SOLENOID2))
            {
                c2State = C2_250;
                c2TimerMs = 240;
            }
            break;

        case C2_250:
            if (0 == c2TimerMs)
            {
                PORTA &= ~_BV(CURTAIN2);
                c2State = C2_POWERDOWN;
            }
            break;

        case C2_POWERDOWN:
            if (!(PINA & _BV(POWER)))
            {
                c2State = C2_POWERUP;
            }
            break;
    }
}

void shaftStateMachine(void)
{
    switch (shaftState)
    {
        case SHAFT_INIT:
            DDRA |= _BV(SHAFT);
            PORTA |= _BV(SHAFT);
            shaftState = SHAFT_POWERUP;
            break;

        case SHAFT_POWERUP:
            if (PINA & _BV(POWER))
            {
                shaftState = SHAFT_100;
                shaftTimerMs = 100;
            }
            break;

        case SHAFT_100:
            if (0 == shaftTimerMs)
            {
                PORTA &= _BV(SHAFT);
                shaftState = SHAFT_190;
                shaftTimerMs = 1186;
            }
            break;

        case SHAFT_190:
            if (0 == shaftTimerMs)
            {
                PORTA |= _BV(SHAFT);
                shaftState = SHAFT_POWERDOWN;
            }
            break;

        case SHAFT_POWERDOWN:
            if (!(PINA & _BV(SHAFT)))
            {
                shaftState = SHAFT_POWERUP;
            }
            break;
    }

}

void main(void)
{
    uart_init();
    //
    // set up interrupt timer
    //
    TCCR0A |= _BV(CS01); // clock/8
    TIMSK0 |= _BV(TOIE0);

    //PORTA |= _BV(CURTAIN1); // tie high

    c1State = C1_INIT;
    c2State = C2_INIT;
    shaftState = SHAFT_POWERUP;
    c1TimerMs = 0;
    c2TimerMs= 0;
    shaftTimerMs = 0;
    timerPrescale = 0;

    STRING(StrIAmAlive);
    sei();

    while (1)
    {
        c1StateMachine();
        c2StateMachine();
        shaftStateMachine();
    }

    // wait for power
    // after 100ms shaft 0 for 190ms then 1
    // energizeCurtainSensors
    // wait for Solenoid 1, wait 140ms, Curtain 1 low for 10ms then back  (maybe curtain 2 zero pulse for 500us) up for 120ms then 0
    // wait for Solenoid 2, wait 250ms Curtain 2 low
    // wait for Solenoid 2 low
    // wait for solenoid 1 low
//void uart_send_buffered(u08 data);

}  



