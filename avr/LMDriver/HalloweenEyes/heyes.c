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
//#define BOTH_RESET _BV(2)
//#define BOTH_SCK _BV(5)
//#define BOTH_MOSI _BV(3)
//#define RIGHT_SS _BV(1)
//#define LEFT_SS _BV(0)
 

void stringOut(unsigned char * stringPtr)
{
    unsigned char data = pgm_read_byte_near(stringPtr++);
    while (data)
    {
        mosi_push (data);
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


int main(void)
{
    unsigned char data = 0;

    spi_init();

    _delay_ms(100);

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
