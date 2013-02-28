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
#include <serutil.h>

u08 disploop = 1;
u08 node = 0;

//void mosiPush(unsigned char data)
//{
//    mosiBuffer[mosiHead++] = data;
//    mosiHead &= (MOSIBUFFERSIZE - 1);
//    spiEvent = 1;
//}

//unsigned char isMaster(void)
//{
//    PORTE |= _BV(5);
//    _delay_ms(10);
//    if (!(PINE & _BV(6)))
//    {
//        return 0;
//    }
//
//    PORTE &= ~_BV(5);
//    _delay_ms(10);
//    if (PINE & _BV(6))
//    {
//        return 0;
//    }
//    return 1;
//}

void main_setNode(u08 setNode)
{
    node = setNode - '0';
}

int main(void)
{
    unsigned char count = 0;
//    unsigned char master;
    /* disable the watchdog */
    MCUSR = 0;
    wdt_disable();
    
    cmdInit();			/* init command processor */
    spi_init();   	/* init serial peripheral interface */
    uart_init();
    
    //master = isMaster();

    sei();
    dm_init();			/* init displaymux */
    u16 data = 0;

    while (1)
    {
        count++;

        if ((data = uart_get_buffered()) & 0x0100)
        {
            cmd_dataHandler(data & 0xff);
            //uart_send_buffered(data & 0xff); // cascade
            continue;
        }

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
