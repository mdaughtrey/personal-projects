#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <spi.h>

// PORTC
//#define RIGHT_SS _BV(5)
//#define RIGHT_MOSI _BV(4)
//#define LEFT_SS _BV(3)
//#define LEFT_MOSI _BV(2)
//#define BOTH_SCK _BV(1)
// 
//#define SPIBIT_DELAYUS 80
unsigned char text[] PROGMEM = {
"lol "
"mp3 "
"fyi "
"digg"
"rss "
"html"
"wiki"
"FAQ "
"blog"
"jpg "
"ymmv"
"kb/s"
"wtf "
"@you"
"pwnd"
"brb "
"imho"
"OMG "
"rofl"
"lmao"
"noob"
"hx0r"
"gtg "
"ttyl"
"IIRC"
"btw "
"l8r "
"ftw "
"fwiw"
"http"
"404 "
"j/k "
"nsfw"
"rtfm"
" :) "
" ;) "
"^-^ "
"FAIL"
" 8) "
" :/ "
" :( "
" :O "
" :P "
" <3 "
"#_# "
"www."
".com"
".net"
".org"
"lulz"
};

unsigned char txStrShiftUp[] PROGMEM = { "s0u1s1u1s0u2s1u2s0u3s1u3s0u4s1u4s0u5s1u5s0u6s1u6a A !0!1" };
unsigned char txStrShiftDown[] PROGMEM = { "s0d1s1d1s0d2s1d2s0d3s1d3s0d4s1d4s0d5s1d5s0d6s1d6a A !0!1" };
unsigned char txStrShiftLeft[] PROGMEM = { "s0l1s1l1s0l2s1l2s0l3s1l3s0l4s1l4a A !0!1" };
unsigned char txStrShiftRight[] PROGMEM = { "s0r1s1r1s0r2s1r2s0r3s1r3s0r4s1r4a A !0!1" };
unsigned char txStrShiftUpDown[] PROGMEM = { "s0u1s1d1s0u2s1d2s0u3s1d3s0u4s1d4s0u5s1d5s0u6s1d6a A !0!1" };
unsigned char txStrShiftLeftRight[] PROGMEM = { "s0l1s1r1s0l2s1r2s0l3s1r3s0l4s1r4a A !0!1" };

// msb first, sck low on idle, sample on leading edge
void spiOut(unsigned char dataleft, unsigned char dataright)
{
    mosi_push_left(dataleft);
    mosi_push_right(dataright);
}

unsigned char pickTransform(int random, unsigned char ** ptr)
{
    switch (random % 6)
    {
        case 0: *ptr = txStrShiftUp; return sizeof(txStrShiftUp);
        case 1: *ptr = txStrShiftDown; return sizeof(txStrShiftDown);
        case 2: *ptr = txStrShiftLeft; return sizeof(txStrShiftLeft);
        case 3: *ptr = txStrShiftRight; return sizeof(txStrShiftRight);
        case 4: *ptr = txStrShiftLeftRight; return sizeof(txStrShiftLeftRight);
        case 5: *ptr = txStrShiftUpDown; return sizeof(txStrShiftUpDown);
    }
    return 0;
}


int main(void)
{
    unsigned long x = 0;
    spi_init();
    // initialize

    spiOut('*', '*');
    _delay_ms(100);
    spiOut('L', 'L');
    spiOut('0', '0');
    spiOut('1', '1');
    spiOut('l', 'l');
    spiOut('0', '0');
    spiOut('1', '1');
    while (1)
    {
        // Pick a random message...
        unsigned char * ptr = text;
        x = x * 1664525L + 1013904223L;
        ptr += (x >> 17 & 0x7fff) % (sizeof(text)/4) * 4;
        
        unsigned char data1 = pgm_read_byte_near(ptr++);
        unsigned char data2 = pgm_read_byte_near(ptr++);
        unsigned char data3 = pgm_read_byte_near(ptr++);
        unsigned char data4 = pgm_read_byte_near(ptr++);

        spiOut('a', 'a');
        spiOut(data1, data3);
        spiOut('A', 'A');
        spiOut(data2, data4);
        _delay_ms(3000);

        // Pick a random transform...
        unsigned char ii;
        unsigned char jj;
        x = x * 1664525L + 1013904223L;
        ii = pickTransform(x >> 17 & 0x7fff, &ptr);

        for (jj = 0; jj < ii; jj++)
        {
            data1 = pgm_read_byte_near(ptr++);
            spiOut(data1, data1);
            _delay_ms(10);
        }
        _delay_ms(500);
    }
    return 0;
}

