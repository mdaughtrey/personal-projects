#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/parity.h>
#include <avr/pgmspace.h>
#include <displaymux.h>
#include <string.h>
#include <golcomm.h>
#include <serutil.h>
#include <typedefs.h>

#define ROWS 7 
#define COLS 10 
const char msgInit[] PROGMEM = "Init\r\n";
const char msgCls[] PROGMEM = "\x1b[2J";
const char msgCrLf[] PROGMEM = "\r\n";

#define SENDSYNC(x) uart_send_sync(x, sizeof(x))

// 000l llll rrrr r000

u16 colony[ROWS + 2];
u16 newColony[ROWS + 2];

const unsigned char bc[] PROGMEM = {
	0, // 0b00000000
	1, // 0b00000001
	1, // 0b00000010
	2, // 0b00000011
	1, // 0b00000100
	2, // 0b00000101
	2, // 0b00000110
	3, // 0b00000111
};

u16 y16;
 
// returns values from 1 to 65535 inclusive, period is 65535
u16 xorshift16(void)
{
    y16 ^= (y16 << 13);
    y16 ^= (y16 >> 9);
    return y16 ^= (y16 << 7);
}

void initColony(void)
{
    u08 ii;

    for (ii = 0; ii < 9; ii++)
    {
#ifdef WRAPTEST
        colony[ii] = 0;
#else
        u16 value = xorshift16();

        uart_send_hex_byte(value >> 8);
        uart_send_hex_byte(value  & 0xff);
        SENDSYNC(msgCrLf);
        colony[ii] = value;
#endif //WRAPTEST
    }
#ifdef WRAPTEST
    colony[2] = 0b0000100000000000;
    colony[7] = 0b0000000001000000;
#endif //WRAPTEST
	return;
}

void printColony(void)
{
	s08 drc; // display row count
	s08 dcc; // display column count
	for (dcc = 0; dcc < 5; dcc++)
	{
		u08 data = 0;
        u08 data2 = 0;
		for (drc = 0; drc < 7; drc++)
		{
			data >>= 1;
			data |= ((colony[drc + 1] >> (7 - dcc)) & 0x01) << 6;
			data2 >>= 1;
			data2 |= ((colony[drc + 1] >> (12 - dcc)) & 0x01) << 6;
		}
        dm_progColumn(0, dcc, data2);
        dm_progColumn(1, dcc, data);
    }
}


void evolve(void)
{
	u08 rc;
	u08 cc;

	memset(newColony, 0, sizeof(u16) * (ROWS + 2));
	colony[0] = colony[7];
	colony[8] = colony[1];

#ifdef WRAPTEST
    int ii;
    for (ii = 1; ii < 7; ii++)
    {
        colony[ii] = colony[ii + 1];
    }
#endif //WRAPTEST

#ifndef NOINT
    for (rc = 1; rc < 8; rc++)
    {
        colony[rc] &= 0b0001111111111000;
        colony[rc] |= (colony[rc] & _BV(3) ? _BV(13) : 0);
        colony[rc] |= (colony[rc] & _BV(12) ? _BV(2) : 0);
    }
#endif
#ifdef WRAPTEST
#else
	for (rc = 1; rc < 8; rc++)
	{
		for (cc = 3; cc < 13; cc++)
		{
			u08 above = (colony[rc - 1] >> (cc -1)) & 0x07;
    		u08 beside = (colony[rc] >> (cc - 1)) & 0x05;
			u08 below = (colony[rc + 1] >> (cc -1)) & 0x07;
			u08 bitcount = pgm_read_byte_near(bc + above) 
                + pgm_read_byte_near(bc + beside) +
                + pgm_read_byte_near(bc + below);
			if (!(colony[rc] & (1 << cc)))
			{
				if (bitcount == 3)
				{
					newColony[rc] |= (1 << cc);
				}
			}
			else if (bitcount == 2 || bitcount == 3)
			{
				newColony[rc] |= (1 << cc);
			}
		}
	}
	memmove(colony, newColony, sizeof(u16) * (ROWS + 2));
#endif // WRAPTEST
	return;
}

void prepare(u08 * one, u08 * two)
{
    u08 temp;
    temp = one[0];
    one[0] = one[ROWS + 1];
    one[ROWS + 1] = temp;

    for (temp = 0; temp < ROWS + 2; temp++)
    {
        one[temp] &= 0b00111110;
        one[temp] |= (two[temp] >> 5) & 1;
        one[temp] |= (two[temp] & 2) << 5;
    }
}


int main(void)
{
    uart_init();
	dm_init();
	dm_setPalette(0, 1);
	dm_setPalette(1, 1);
	initColony();
	printColony();
    commInit();
    sei();

    y16 = 0xa55a;

    SENDSYNC(msgCls);
    SENDSYNC(msgInit);
	unsigned int count = 0;
	while(1)
	{
		dm_timerHandler();
		if (!(count % 500))
		{
            commStep();
            evolve();
			printColony();
		}
#ifndef WRAPTEST
        if (!(count % 30000))
        {
            initColony();
        }
#endif // WRAPTEST
		count++;
	}
    return 0;
}

