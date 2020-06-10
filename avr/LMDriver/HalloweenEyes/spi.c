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
#define BOTH_RESET _BV(2)

void spi_init(void)
{
    volatile char IOReg;
    DDRC = LEFT_SS | RIGHT_SS | BOTH_RESET;
    PORTC = LEFT_SS | RIGHT_SS | BOTH_RESET;
    DDRB |= _BV(PB1) | _BV(PB2); 		/* Enable MOSI/SCK for output */
    PORTB |= _BV(0); // tie the SS line high
    SPCR = _BV(MSTR) | _BV(SPE) | _BV(SPR0); /* Enable SPI interrupts, MSB first, clock / 16 */
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
    RESET_LEFT_SS;
    SPDR = data;
    while (!(SPSR & _BV(SPIF)));
    IOReg = SPDR;
    SET_LEFT_SS;

    RESET_RIGHT_SS;
    SPDR = data;
    while (!(SPSR & _BV(SPIF)));
    IOReg = SPDR;
    SET_RIGHT_SS;
}

