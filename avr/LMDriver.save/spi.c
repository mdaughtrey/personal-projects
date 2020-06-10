#include <avr/io.h>
#include <typedefs.h>
#include <spi.h>

u08 mosiBuffer[MOSIBUFFERSIZE];
u08 misoBuffer[MISOBUFFERSIZE];
u08 mosiHead = 0;
u08 mosiTail = 0;
u08 misoHead = 0;
u08 misoTail = 0;
u08 spiEvent = 0;


ISR(SIG_SPI)
{
    mosiBuffer[mosiHead++] = SPDR;
    mosiHead &= (MOSIBUFFERSIZE - 1);
    spiEvent = 1;
    if (SPSR & _BV(SPIF))
    {
        SPDR = 0xaa;
        return;
    }
    if (misoHead != misoTail)
    {
        SPDR = misoBuffer[misoTail++];
        misoTail &= (MISOBUFFERSIZE - 1);
    }
}

void spi_init(void)
{
    volatile char IOReg;
    DDRB |= _BV(PB3); 		/* Enable MISO for output */
    PRR &= ~_BV(PRSPI);		/* Turn on the SPI module */

    SPDR = 0;
    IOReg = SPSR;
    IOReg = SPDR;
    SPCR = _BV(SPE) | _BV(SPIE);		/* Enable SPI interrupts, MSB first */
}

void miso_push(u08 data)
{
    cli();
    misoBuffer[misoHead++] = data;
    misoHead &= (MISOBUFFERSIZE - 1);
    sei();
}

