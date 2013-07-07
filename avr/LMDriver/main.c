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
#include <embedvm2.h>
#endif // EMBEDVM

#include <vminterface.h>
//#include <testbin.h>


u08 disploop = 1;
u08 programControl = 1;
volatile unsigned char delayCount = 16;
extern u16 binOffset;
extern u16 mainOffset;
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
        delayCount = 16;
//        TIMSK0 &= ~_BV(TOIE0); // turn off timer interrupt
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
    
    sei();
    dm_init();			/* init displaymux */
    u16 data = 0;
    count = 0;

#ifdef EMBEDVM
    vminterface_init();
    //testbinInit();
    
//    uart_send_hex_byte(binOffset >> 8);
//    uart_send_hex_byte(binOffset & 0xff);
//    uart_send_buffered('\r');
//    uart_send_buffered('\n');
    embedvm_interrupt(mainOffset);
    programControl = 0;
#endif // EMBEDVM

    while (1)
    {
      //  uart_send_buffered(programControl + 0x30);
        count++;
#ifdef EMBEDVM
//        if (!(count & 0x0f))
        if (!(count & 0x0f) && programControl)
        //if (programControl)
        {
//            uart_send_buffered('x');
            embedvm_exec();
  //          uart_send_buffered('X');
        }
#endif // EMBEDVM

        data = uart_get_buffered();
        if (data & 0x0100)
        {
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
