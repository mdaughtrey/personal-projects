#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <spi.h>

unsigned char C0C1[] PROGMEM = { "C0C1"};
unsigned char c0c1[] PROGMEM = { "c0c1"};
unsigned char strReset[] PROGMEM = { "*"};
unsigned char strSimpleTest[] PROGMEM = { "aaA i1"};
#ifdef SMALL_PUPIL
//unsigned char strLookStraightAhead[] PROGMEM = { "g0c0c101c91c122822241741341449549641a-A-" };
unsigned char strLookStraightAhead[] PROGMEM = { "g001c91c122822241741341449549641a-A-c0c1" };
unsigned char strSlowBlink[] PROGMEM = {
    "00890811481422272232262242a52a"
    "10880821471431461441c51c"
    "208308408508608708"
    "10880821471431461441c51c"
    "11481422272232262243e53e"
    "01c91c122822241741341449549641"
};

unsigned char strGlanceRight1[] PROGMEM = { "349541" };
unsigned char strGlanceRight2[] PROGMEM = { "249441" };
unsigned char strGlanceRight3[] PROGMEM = { "449241" };
unsigned char strGlanceRight4[] PROGMEM = { "549341" };

unsigned char strGlanceLeft1[] PROGMEM = { "649441" };
unsigned char strGlanceLeft2[] PROGMEM = { "749541" };
unsigned char strGlanceLeft3[] PROGMEM = { "549741" };
unsigned char strGlanceLeft4[] PROGMEM = { "449641" };
#else
unsigned char strLookStraightAhead[] PROGMEM = { "g001c91c12282224174134145d55d641c0c1a-A-" };
unsigned char strSlowBlink[] PROGMEM = {
    "00890811481422272232262243e53e"
    "10880821471431461441c51c"
    "208308408508608708"
    "10880821471431461441c51c"
    "114814222722322622418518"
    "01c91c12282224174134145d55d641"
};

unsigned char strGlanceRight1[] PROGMEM = { "35d541" };
unsigned char strGlanceRight2[] PROGMEM = { "25d441" };
unsigned char strGlanceRight3[] PROGMEM = { "45d241" };
unsigned char strGlanceRight4[] PROGMEM = { "55d341" };

unsigned char strGlanceLeft1[] PROGMEM = { "65d441" };
unsigned char strGlanceLeft2[] PROGMEM = { "75d541" };
unsigned char strGlanceLeft3[] PROGMEM = { "55d741" };
unsigned char strGlanceLeft4[] PROGMEM = { "45d641" };
#endif

// PORTC
#define BOTH_RESET _BV(2)
#define BOTH_SCK _BV(5)
#define BOTH_MOSI _BV(3)
#define RIGHT_SS _BV(1)
#define LEFT_SS _BV(0)
 
#define SPIBIT_DELAYUS 80
// msb first, sck low on idle, sample on leading edge
#if 0
void spiOut(unsigned char data)
{
    // set the SS lines
    PORTC &= ~LEFT_SS;
    PORTC &= ~RIGHT_SS;
//    _delay_us(SPIBIT_DELAYUS);

    unsigned char ii;
    for (ii = 0; ii < 8; ii++)
    {
        if (data & 0x80) { PORTC |= BOTH_MOSI; } 
        else { PORTC &= ~BOTH_MOSI; }
        _delay_us(SPIBIT_DELAYUS);
        PORTC |= BOTH_SCK;
        _delay_us(SPIBIT_DELAYUS);
        PORTC &= ~BOTH_SCK;
        data <<= 1;
    }
 //   _delay_us(SPIBIT_DELAYUS);
    // reset the SS lines
    PORTC |= LEFT_SS;
    PORTC |= RIGHT_SS;
}
#endif // 0
void spiOut(unsigned char data)
{
    mosi_push(data);
}

void stringOut(unsigned char * stringPtr)
{
    unsigned char data = pgm_read_byte_near(stringPtr++);
    while (data)
    {
        spiOut (data);
        data = pgm_read_byte_near(stringPtr++);
    //    _delay_ms(1);
    }
}

void fastBlink()
{
    stringOut(C0C1);
    _delay_ms(50);
    stringOut(c0c1);
}


void slowBlink(void)
{ 
    stringOut(strSlowBlink);
}

void glanceRight(void)
{
    stringOut(strGlanceRight1);
    _delay_ms(100);
    stringOut(strGlanceRight2);
    _delay_ms(3000);
    stringOut(strGlanceRight3);
    _delay_ms(200);
    stringOut(strGlanceRight4);
    _delay_ms(200);
}

void glanceLeft(void)
{
    stringOut(strGlanceLeft1);
    _delay_ms(100);
    stringOut(strGlanceLeft2);
    _delay_ms(3000);
    stringOut(strGlanceLeft3);
    _delay_ms(200);
    stringOut(strGlanceLeft4);
    _delay_ms(200);
}

#define SET_LEFT_SS PORTC |= _BV(0)
#define RESET_LEFT_SS PORTC &= ~_BV(0) 

#define SET_RIGHT_SS PORTC |= _BV(1)
#define RESET_RIGHT_SS PORTC &= ~_BV(1) 

#define SET_SCK PORTB |= _BV(1)
#define RESET_SCK PORTB &= ~_BV(1)

#define SET_MOSI PORTB |= _BV(2)
#define RESET_MOSI PORTB &= ~_BV(2)

void delay1s(void)
{
    _delay_ms(100);
    _delay_ms(100);
    _delay_ms(100);
    _delay_ms(100);
    _delay_ms(100);
    _delay_ms(100);
    _delay_ms(100);
    _delay_ms(100);
    _delay_ms(100);
    _delay_ms(100);
}

void testMain(void)
{
    DDRC |= _BV(0) | _BV(1) |_BV(2);
    DDRB |= _BV(1) | _BV(2);
    PORTC |= _BV(2); // leave reset high
    while (1)
    {
        SET_LEFT_SS;
        delay1s();
        RESET_LEFT_SS;
        delay1s();
        SET_RIGHT_SS;
        delay1s();
        RESET_RIGHT_SS;

        SET_SCK;
        delay1s();
        RESET_SCK;

        SET_MOSI;
        delay1s();
        RESET_MOSI;
    }
}

int main(void)
{
    unsigned char data = 0;
    //testMain();
    // A little settling time...
    //_delay_ms(10);
    // initialize

//    DDRC = _BV(0) | _BV(1) | _BV(2); // left ss right ss rst
    DDRC = LEFT_SS | RIGHT_SS | BOTH_RESET;
    PORTC = LEFT_SS | RIGHT_SS | BOTH_RESET;
    spi_init();

    delay1s();
    data = 'x';
    while (1)
    {
        spiOut('a');
        //delay1s();
//        spiOut('x');
        _delay_ms(1);
        spiOut(data);
        //delay1s();
        _delay_ms(1);
    //delay1s();
        data++;
//        stringOut(strSimpleTest);
    }


    stringOut(strReset);
    _delay_ms(100);
    stringOut(strLookStraightAhead);
    while (1)
    {
        _delay_ms(5000);
        glanceLeft();
        _delay_ms(5000);
        glanceRight();
        _delay_ms(5000);
        slowBlink();
    }
    return 0;
}
