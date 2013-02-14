#include <avr/io.h>
#include <typedefs.h>
#include <spi.h>
#include <util/delay.h>

u08 mosiBuffer[MOSIBUFFERSIZE];
u08 misoBuffer[MISOBUFFERSIZE];
u08 mosiHead = 0;
u08 mosiTail = 0;
u08 misoHead = 0;
u08 misoTail = 0;
u08 spiEvent = 0;

#define LEFT_SS _BV(0)
#define RIGHT_SS _BV(1)

#if 0
//ISR(SIG_SPI)
#define SPIMASTER
#ifndef SPIMASTER

ISR(SPI_STC_vect)
{
    mosiBuffer[mosiHead++] = SPDR;
    mosiHead &= (MOSIBUFFERSIZE - 1);
    spiEvent = 1;
    if (misoHead != misoTail)
    {
        SPDR = misoBuffer[misoTail++];
        misoTail &= (MISOBUFFERSIZE - 1);
    }
    else
    {
        SPDR = 0x00;
    }
}

#else

ISR(SPI_STC_vect)
{
//    unsigned char xx = SPDR;
//    misoBuffer[misoHead++] = SPDR;
//    misoHead &= (MISOBUFFERSIZE - 1);
//    spiEvent = 1;
    if (mosiHead != mosiTail)
    {
        PORTC &= ~LEFT_SS;
        PORTC &= ~RIGHT_SS;
        SPDR = mosiBuffer[mosiTail++];
        PORTC |= LEFT_SS;
        PORTC |= RIGHT_SS;
        mosiTail &= (MOSIBUFFERSIZE - 1);
    }
    else
    {
        SPDR = 0x00;
    }
}

#endif
#endif // 0

void spi_init(void)
{
    volatile char IOReg;
//    DDRB |= _BV(PB3); 		/* Enable MISO for output */
    DDRB |= _BV(PB1) | _BV(PB2); 		/* Enable MOSI/SCK for output */
    PORTB |= _BV(0); // tie the SS line high
//    PRR &= ~_BV(PRSPI);		/* Turn on the SPI module */
    // Clear out any odd crap
//    SPDR = 0;
//    IOReg = SPDR;
//    IOReg = SPDR;
//
//    IOReg = SPSR;
//    IOReg = SPDR;
    //SPCR = _BV(MSTR) | _BV(SPE) | _BV(SPIE) | _BV(SPR0);		/* Enable SPI interrupts, MSB first */
    SPCR = _BV(MSTR) | _BV(SPE) | _BV(SPR0);		/* Enable SPI interrupts, MSB first, clock / 16 */
//    sei();
    //IOReg = SPDR;
    //IOReg = SPDR;

    //IOReg = SPSR;
    //IOReg = SPDR;
    //SPCR = _BV(MSTR) | _BV(SPE);		/* Enable SPI interrupts, MSB first */
}

void miso_push(u08 data)
{
    cli();
    misoBuffer[misoHead++] = data;
    misoHead &= (MISOBUFFERSIZE - 1);
    sei();
}

void mosi_push(u08 data)
{
    volatile char IOReg;
    PORTC &= ~LEFT_SS;
    SPDR = data;
    while (!(SPSR & _BV(SPIF)));
    IOReg = SPDR;
    PORTC |= LEFT_SS;

    PORTC &= ~RIGHT_SS;
    SPDR = data;
    while (!(SPSR & _BV(SPIF)));
    IOReg = SPDR;
    PORTC |= RIGHT_SS;
}
#if 0
void mosi_push(u08 data)
{
    cli();
    mosiBuffer[mosiHead++] = data;
    mosiHead &= (MOSIBUFFERSIZE - 1);
    if (mosiHead == mosiTail)
    {
        PORTC &= ~LEFT_SS;
        PORTC &= ~RIGHT_SS;
        SPDR = mosiBuffer[mosiTail++];
        PORTC |= LEFT_SS;
        PORTC |= RIGHT_SS;
        mosiTail &= (MOSIBUFFERSIZE - 1);
    }
    sei();
}
#endif

