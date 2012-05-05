#include <dmc40457.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

char hexTable[] PROGMEM = "0123456789abcdef";

void e(void)
{
    E1HIGH;
    _delay_ms(1);
    E1LOW;
    _delay_ms(1);
}

void e2(void)
{
    E2HIGH;
    _delay_ms(1);
    E2LOW;
    _delay_ms(1);
}

void e4(unsigned char data, unsigned char display)
{
        if (data & 0x10) D0HIGH; else D0LOW; 
        if (data & 0x20) D1HIGH; else D1LOW; 
        if (data & 0x40) D2HIGH; else D2LOW; 
        if (data & 0x80) D3HIGH; else D3LOW; 
        if (0 == display) e(); else e2();
        if (data & 0x01) D0HIGH; else D0LOW; 
        if (data & 0x02) D1HIGH; else D1LOW; 
        if (data & 0x04) D2HIGH; else D2LOW; 
        if (data & 0x08) D3HIGH; else D3LOW; 
        if (0 == display) e(); else e2();
}

void lcdInit(void)
{
    DDRE = 0x0c; // 0000 1100 data0 and data1 output
    DDRF = 0x7f; // 0001 1111 sck,e,e2,rs,rw,data2,data3 output

    PORTE = 0;
    PORTF = 0; 
    _delay_ms(20);
    D1HIGH;
    e();
    e2();
    // 4-bit interface enabled
    e4(0x28, 0); // Set display lines and font
    e4(0x0e, 0); // display on
    e4(0x01, 0); // clear screen
    e4(0x06, 0); // entry mode
    e4(0x28, 1); // Set display lines and font
    e4(0x0e, 1); // display on
    e4(0x01, 1); // clear screen
    e4(0x06, 1); // entry mode
}

inline void lcdChar2(unsigned char data)
{
    RSHIGH;
    e4(data, 1);
    RSLOW;
}

inline void lcdChar(unsigned char data)
{
    RSHIGH;
    e4(data, 0);
    RSLOW;
}

void lcdStringPtr(unsigned char * string)
{
    unsigned char ch;
    while (ch = pgm_read_byte_near(string))
    {
        lcdChar(ch);
        string++;
    }
}

void lcdStringPtr2(unsigned char * string)
{
    unsigned char ch;
    while (ch = pgm_read_byte_near(string))
    {
        lcdChar2(ch);
        string++;
    }
}

void lcdString(char * string)
{
    unsigned char ch;
    while (ch = *string)
    {
        lcdChar(ch);
        string++;
    }
}

void lcdString2(char * string)
{
    unsigned char ch;
    while (ch = *string)
    {
        lcdChar2(ch);
        string++;
    }
}

void toHex(unsigned short data)
{
    lcdChar(pgm_read_byte_near(hexTable + ((data >> 12) & 0x0f)));
    lcdChar(pgm_read_byte_near(hexTable + ((data >> 8) & 0x0f)));
    lcdChar(pgm_read_byte_near(hexTable + ((data >> 4) & 0x0f)));
    lcdChar(pgm_read_byte_near(hexTable + (data & 0x0f)));
}

void toHex2(unsigned short data)
{
    lcdChar2(pgm_read_byte_near(hexTable + ((data >> 12) & 0x0f)));
    lcdChar2(pgm_read_byte_near(hexTable + ((data >> 8) & 0x0f)));
    lcdChar2(pgm_read_byte_near(hexTable + ((data >> 4) & 0x0f)));
    lcdChar2(pgm_read_byte_near(hexTable + (data & 0x0f)));
}

void byteToHex2(unsigned char data)
{
    lcdChar2(pgm_read_byte_near(hexTable + ((data >> 4) & 0x0f)));
    lcdChar2(pgm_read_byte_near(hexTable + (data & 0x0f)));
}


void lcdLineCol(unsigned char line, unsigned char col, unsigned char disp)
{
    col |= 0x80;
    if (0 == line)
            e4(col, disp);
    else
            e4(0x40 + col, disp);
}

