#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// /usr/local?CrossPack-AVR/include/avr


void delayMs(unsigned char ms, unsigned char repeat)
{
    do
    {
        _delay_loop_2(ms * 240);
    } while (--repeat);
}

// PORTB3 = Power Testpoint I
#define POWER _BV(3)

// PORTB4 = Shutter Testpoint C
#define SHUTTER _BV(4)

// PORTB5 = LED
#define LED _BV(2)

#define WAIT_TIL_LED_ON while ((PINB & LED) == LED);
#define WAIT_TIL_LED_OFF while ((PINB & LED) == 0);
#define INTERVAL_INIT (30 << 2) // 30s in 250ms intervals

volatile unsigned char interval = INTERVAL_INIT; // 250ms units 

ISR(TIMER0_OVF_vect) // Called ~250ms
{
    if (interval)
    {
        interval--;
    }
}

void flashLed(void)
{
    DDRB |= POWER;
    unsigned char ii;
    for (ii = 0; ii < 5; ii++)
    {
        PORTB |= POWER;
        delayMs(100, 1);
        PORTB &= ~POWER;
        delayMs(100, 1);
    }
    DDRB &= ~POWER;
}

main(void)
{
    while (1)
    {
        while (!(PINB & POWER)); // wait for a power button keypress
        delayMs(100, 1);
        flashLed();

        delayMs(100, 30); // 100ms * 30 times = 3s

        TCNT0 = 0;
        TCCR0B |= (_BV(CS00) | _BV(CS02));    // clock enabled, /1024
        TIMSK |= _BV(TOIE0);         // timer 0 overflow
        sei();

        volatile unsigned char breakit = 0;
        while (!breakit)
        {
            interval = INTERVAL_INIT;
            // Power On
            DDRB |= (POWER | SHUTTER);
            PORTB |= POWER;
            delayMs(100, 13); // 100ms * 13 times = 1.3s
            PORTB &= ~POWER;

            WAIT_TIL_LED_OFF;
            WAIT_TIL_LED_ON;

            // Shutter
            delayMs(100, 1); // 100ms
            PORTB |= SHUTTER;
            delayMs(50, 1); // 50ms
            PORTB &= ~SHUTTER;
            WAIT_TIL_LED_OFF;

            // Power Off
            WAIT_TIL_LED_ON;
            delayMs(100, 1);
            PORTB |= POWER;
            WAIT_TIL_LED_OFF;
            PORTB &= ~POWER;
            DDRB &= ~(POWER | SHUTTER);

            while (interval) // wait for timer to expire
            {
                if (PINB & POWER)
                {
                    flashLed();
                    breakit = 1;
                    break;
                }
            }
        }
    }
}
