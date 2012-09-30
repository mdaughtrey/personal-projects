#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <serutil.h>
//#include <testmode.h>

// pc1 = ir
// pc2 = lamp
// pc3 = camera

#define STOP 0
#define FORWARD 2
#define NEXT 3

/*
"aA: advance 1 count\r\n"
"fF: free run forward\r\n"
"nN: next frame\r\n"
"cC: camera shutter (2500ms pulse)\r\n"
"l : lamp on\r\n"
"L : lamp off\r\n";
*/

// Pin 1 = pe7
// Pin 2 = pb4
// Pin 3 = pb5
// Pin 4 = pb6

#define AC_MOTOR_ON PORTF |= 0x10
#define AC_MOTOR_OFF PORTF &= ~0x10

unsigned char motorIsOn = 0;

unsigned char ledTestBit = 0;
unsigned char ledControl = 0;

#define LED_SERIAL 1
#define LED_MOTOR  2
#define LED_CAMERA 3
#define LED_LAMP   4 

#define LEDBIT_SERIAL _BV(LED_SERIAL)
#define LEDBIT_MOTOR  _BV(LED_MOTOR)
#define LEDBIT_CAMERA _BV(LED_CAMERA)
#define LEDBIT_LAMP   _BV(LED_LAMP)

void delay20ms(unsigned char count)
{
    while (count--)
    {
        _delay_ms(20);
    }
}

void ledIlluminate(unsigned char which)
{
    switch (which)
    {
        case 0:
            PORTE |= _BV(7);
            PORTB |= (_BV(4) | _BV(5) |_BV(6));
            break;

        case LED_SERIAL:
            PORTB &= ~_BV(4);
            PORTE |=  _BV(7);
            break;

        case LED_MOTOR:
            PORTB |=  _BV(4);
            PORTE &= ~_BV(7);
            break;

        case LED_CAMERA:
            PORTB |=  _BV(5);
            PORTB &= ~_BV(6);
            break;

        case LED_LAMP:
            PORTB &= ~_BV(5);
            PORTB |=  _BV(6);
            break;
    }
}

void ledInit(void)
{
    DDRE |= _BV(7);
    DDRB |= (_BV(4) | _BV(5) |_BV(6));
    PORTE |= _BV(7);
    PORTB |= (_BV(4) | _BV(5) |_BV(6));

    unsigned char count;
    for (count = 0; count < 4; count++)
    {
        ledIlluminate((count % 4) + 1);
        delay20ms(5);
        ledIlluminate(0);
    }
}

void motorOn(void)
{
    if (!motorIsOn)
    {
        ledControl |= LEDBIT_MOTOR;
        AC_MOTOR_ON;
    }
    motorIsOn = 1;
}

void motorOff(void)
{
    if (motorIsOn)
    {
        ledControl &= ~LEDBIT_MOTOR;
        AC_MOTOR_OFF;
    }
    motorIsOn = 0;
}


void updateLedStatus(void)
{
    if (ledControl & _BV(ledTestBit))
    {
        ledIlluminate(0);
        ledIlluminate(ledTestBit);
    }
    ledTestBit++;
    ledTestBit %= 4;
}

void testMain(void)
{
    DDRA |= _BV(6);
//    DDRC = _BV(3) | _BV(2);
    DDRC |= _BV(3); // camera
    while (1)
    {
        delay20ms(25);
        PORTA |= _BV(6);
        PORTC |= _BV(3); // camera
        delay20ms(25);
        PORTA &= ~_BV(6);
        PORTC &= ~_BV(3); // camera
//        PORTC |= _BV(2);
//        _delay_ms(5);
//        PORTC &= ~_BV(2);
    }
}


int main(void)
{
    //testMain();
    PORTC = _BV(2);
    DDRC = _BV(3) | _BV(2);
    PORTC |= _BV(3); // lamp off
    DDRF = 0x10; // Motor control
    motorOff();
    uart_init();
    ledInit();

    char direction = STOP;
    unsigned char irData = 0;
    unsigned advanceCount = 0;


    while (1)
    {
        updateLedStatus();

        u16 data = uart_get_buffered(); 
        if (0x0100 & data)
        {
            data &= 0xff;
            switch (data)
            {
                case 'a':
                case 'A':
                    direction = NEXT;
                    advanceCount = 1;
                    break;

                case 'f':
                case 'F':
                    direction = FORWARD;
                    break;

                case 'N':
                case 'n':
                    direction = NEXT;
                    advanceCount = 3;
                    break;

                case 'C':
                case 'c':
                    PORTC &= ~_BV(3);
                    ledIlluminate(0);
                    ledIlluminate(LED_CAMERA);
                    _delay_ms(250);
                    ledIlluminate(0);
                    PORTC |= _BV(3);
                    break;

                case 'l':
                    // lampOn
                    ledControl |= LEDBIT_LAMP;
                    PORTC |= _BV(2);
                    break;

                case 'L':
                    // lampOff
                    ledControl &= ~LEDBIT_LAMP;
                    PORTC &= ~_BV(2);
                    break;

                case 'x':
                case 'X':
                    motorOff();
                    ledControl &= ~LEDBIT_MOTOR;
                    ledControl &= ~LEDBIT_LAMP;
                    PORTC &= ~_BV(2);
                    PORTC |= _BV(3);
                    ledInit();
                    break;
            }
        }
        if (direction == FORWARD || direction == NEXT)
        {
            motorOn();
        }

        irData <<= 1;
        irData |= (PINC & _BV(1)) >> 1;
        //if (0xf0 == irData && direction == NEXT || direction == PREVIOUS)
        if (0xf0 == irData && direction == NEXT)
        {
            if (--advanceCount <= 0)
            {
                direction = STOP;
                motorOff();
                irData = 0;
            }
        }
    }
    return 0;
}
