#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#define FADE_INIT 0xc0
//
// States:
// Off
// Fade up
// hold
// Bright
// fade down 1/2
// blink
// wait
// lopsided blink
// wait
// fade down 0
enum
{
    STATE_NULL = 0,
    STATE_OFF,
    STATE_FADEUP,
    STATE_HOLD1,
    STATE_BRIGHT,
    STATE_HOLD2,
//    STATE_DIM,
//    STATE_HOLD5,
    STATE_BLINK,
//    STATE_HOLD3,
//    STATE_LBLINK,
    STATE_HOLD4,
    STATE_DONE
};


#define EYESOFF PORTB &= 0b11100111
#define EYESON PORTB |= 0b00011000


void delayMs(unsigned char ms, unsigned char repeat)
{
    do
    {
        _delay_loop_2(ms * 240);
    } while (--repeat);
}

main(void)
{
    unsigned char state = STATE_OFF;

    // init
    TCCR0A |= _BV(COM0A1) | _BV(COM0A0) | _BV(COM0B1) | _BV(COM0B0) | _BV(WGM01) | _BV(WGM00);
    TCCR0B |=  _BV(CS00);
    DDRB |= _BV(4) | _BV(3) | _BV(1) | _BV(0);
   
    OCR0A = 0;
    OCR0B = 0;

    unsigned char ii;
    OCR0A = OCR0B = 0xff;
    //init sign
    for (ii = 0; ii < 3; ii++)
    {
        EYESON;
        delayMs(100, 1);
        EYESOFF;
        delayMs(100, 1);
    }
    OCR0A = OCR0B = 0x0;

    while (1)
    {
        /*
        EYESON;
        delayMs(200, 5);
        EYESOFF;
       delayMs(200, 5);
        continue;
        */
        switch (state)
        {
            case STATE_OFF:
                EYESOFF;
                OCR0A = OCR0B = 0;
                delayMs(200, 200);
                break;

            case STATE_FADEUP:
                OCR0A = OCR0B = 0;
                EYESON;
                for (ii = 0; ii < 0x20; ii++)
                {
                    ++OCR0A;
                    ++OCR0B;
                    delayMs(100, 10);
                }
                break;

            case STATE_HOLD1:
            case STATE_HOLD2:
//            case STATE_HOLD3:
            case STATE_HOLD4:
//            case STATE_HOLD5:
                delayMs(200, 20);
                break;

            case STATE_BRIGHT:
                OCR0A = OCR0B = 0xff;
        //        delayMs(200, 10);
                break;

/*

            case STATE_DIM:
                OCR0A = OCR0B = FADE_INIT;
                delayMs(200, 10);
                break;
*/
            case STATE_BLINK:
                for (ii = 0; ii < 5; ii++)
                {
                    EYESOFF;
                    delayMs(60, 1);
                    EYESON;
                    delayMs(200, 40);
                }
                break;
/*
            case STATE_LBLINK:
                PORTB &= 0b1110111;
                delayMs(80, 1);
                EYESOFF;
                delayMs(40, 1);
                PORTB |= 0b0001000;
                delayMs(80, 1);
                EYESON;
                delayMs(200, 100);
                break;
                */

            default:
                state = STATE_NULL;
                break;
        }
        ++state;
    }
}
