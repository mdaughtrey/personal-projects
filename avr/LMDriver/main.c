#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <avr/wdt.h>
#include <typedefs.h>
#include <cmdproc.h>
#include <displaymux.h>
#include <spi.h>
#include <serutil.h>
#include <avr/eeprom.h>

#ifdef EMBEDVM
#include <embedvm.h>
#else // EMBEDVM
#include <interpreter.h>
#endif // EMBEDVM

#ifdef EMBEDVM
extern struct embedvm_s vm;
#endif // EMBEDVM


u08 disploop = 1;
u08 node = 0;
u08 programControl = 0;
volatile unsigned char delayCount;

//
// This ISR is used to pause interpreter operations
// when it has encountered a PAUSES or PAUSEMS 
// operation. Period is 8ms.
//
ISR(TIMER0_OVF_vect)
{
    if (delayCount)
    {
        delayCount--;
    }
    else
    {
        TIMSK0 &= ~_BV(TOIE0); // turn off timer interrupt
    }
}

void mosiPush(unsigned char data)
{
    mosiBuffer[mosiHead++] = data;
    mosiHead &= (MOSIBUFFERSIZE - 1);
    spiEvent = 1;
}

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
#ifndef EMBEDVM
    interpreter_init();
#endif // EMBEDVM
    
    sei();
    dm_init();			/* init displaymux */
    u16 data = 0;
    count = 0;

    while (1)
    {
        count++;
#ifdef EMBEDVM
        if (!(count & 0x0f))
        {
            embedvm_exec(&vm);
        }
#else // EMBEDVM

        if ((programControl > 0) && !(count & 0x0f) && !delayCount)
        {
            if ((data = interpreter()) & 0x0100)
            {
                cmd_dataHandler(data & 0xff);
            }
            if (data & 0x0200)
            {
//                uart_send_buffered('*');
                if (1 == programControl)
                {
                    programControl = 0;
                }
                else
                {
                    interpreter_init();
                }
            }
        }
#endif // EMBEDVM

        data = uart_get_buffered();
        if (data & 0x0100)
        {
#ifndef EMBEDVM
            if (data == 0x0120 && programControl > 0)
            {
                programControl = 0;
                interpreter_init();
                continue;
            }
#endif // EMBEDVM
            cmd_dataHandler(data & 0xff);
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
