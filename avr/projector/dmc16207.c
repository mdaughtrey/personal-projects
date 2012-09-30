#include <dmc16207.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

extern char hexTable[]; // PROGMEM = "0123456789abcdef";


void e(void)
{
    E1HIGH;
    _delay_ms(10);
    E1LOW;
    _delay_ms(10);
}

/* void e2(void)

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
*/


void lcdData(unsigned char data)
{
    PORTA &= 0x87;
    PORTA |= ((data >> 1) & 0x78);
    e();
    PORTA &= 0x87;
    PORTA |= ((data << 3) & 0x78);
    e();
}

void lcdChar(unsigned char data)
{
    RSHIGH;
    PORTA &= 0x87;
    PORTA |= ((data >> 1) & 0x78);
    e();
    PORTA &= 0x87;
    PORTA |= ((data << 3) & 0x78);
    e();
    RSLOW;
}


void lcdInit(void)
{
    DDRA = 0x7f; // 0000 1100 data0 and data1 output
    PORTA = 0;

    D1HIGH;
    _delay_ms(500);
    e();
    _delay_ms(500);
    // 4-bit interface enabled
    lcdData(0x20); // Set display lines and font
    lcdData(0x0c); // display on
    lcdData(0x01); // clear screen
    lcdData(0x06); // entry mode
}

void lcdStringPtr(unsigned char * string)
{
    unsigned char ch;
    lcdData(0x01); // clear screen
    while (ch = pgm_read_byte_near(string))
    {
        lcdChar(ch);
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

/*
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
*/
