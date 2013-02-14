#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <avr/wdt.h>
#include <typedefs.h>
#include <cmdproc.h>
#include <displaymux.h>
#include <spi.h>

u08 disploop = 1;

void mosiPush(unsigned char data)
{
    mosiBuffer[mosiHead++] = data;
    mosiHead &= (MOSIBUFFERSIZE - 1);
    spiEvent = 1;
}

#ifdef DIAG
void dataBit(unsigned char bit, unsigned char col)
{
    if (PINB & _BV(bit)) // SS
    {
        mosiPush('t');
    }
    else
    {
        mosiPush('T');
    }
    mosiPush('0');
    mosiPush('2');
    mosiPush(col + '0');
    spiEvent = 1;
}
#endif

int main(void)
{
unsigned char count = 0;
    /* disable the watchdog */
    MCUSR = 0;
    wdt_disable();
    
    cmd_Init();			/* init command processor */
    dm_init();			/* init displaymux */
#ifndef DIAG
    spi_init();   	/* init serial peripheral interface */
#endif // DIAG
    
    sei();
    u08 data = 0;

#ifdef DIAG
    mosiPush('c');
    mosiPush('0');

    mosiPush('t');
    mosiPush('0');
    mosiPush('4');
    mosiPush('4');
    spiEvent = 1;
#endif // DIAG

    while (1)
    {
        count++;
#ifdef DIAG
        if (!(count % 100)) // SS
        {
            dataBit(0, 1); // test SS (PORTB 0), set LED Y
        }
        else if (!(count % 50)) // SCK
        {
            dataBit(1, 2); // test SCK (PORTB 1), set LED Y
        }
        else if (!(count % 30)) // MOSI
        {
            dataBit(2, 3); // test MOSI (PORTB 2), set LED Y
        }
#endif // DIAG

        if (spiEvent & !(count & 0x0f))
        {
            cli();
            data = mosiBuffer[mosiTail++];
            mosiTail &= (MOSIBUFFERSIZE - 1);
            if (mosiHead == mosiTail)
            {
                spiEvent = 0;
            }
            sei();
            cmd_dataHandler(data);
            continue;
        }
        if (disploop)
        {
            dm_timerHandler();
        }
    }

    return 0;
}
