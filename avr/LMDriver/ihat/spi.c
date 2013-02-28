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
void spi_init(void)
{
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

void mosi_push_left(u08 data)
{
    volatile char IOReg;
    PORTC &= ~LEFT_SS;
    SPDR = data;
    _delay_ms(1);
    while (!(SPSR & _BV(SPIF)));
    IOReg = SPDR;
    PORTC |= LEFT_SS;
}

void mosi_push_right(u08 data)
{
    volatile char IOReg;
    PORTC &= ~RIGHT_SS;
    SPDR = data;
    _delay_ms(1);
    while (!(SPSR & _BV(SPIF)));
    IOReg = SPDR;
    PORTC |= RIGHT_SS;
}

