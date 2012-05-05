
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

int main(void)
{
    /* disable the watchdog */
    MCUSR = 0;
    wdt_disable();
    
    cmd_Init();			/* init command processor */
    dm_init();			/* init displaymux */
    spi_init();   	/* init serial peripheral interface */
    
    sei();
    u16 intCount = 0;
    u08 state = 0;
    u08 debugCount = 0;
    u08 data = 0;
  
    while (1)
    {
        if (spiEvent == 1)
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
        dm_timerHandler();
    }

    return 0;
}
