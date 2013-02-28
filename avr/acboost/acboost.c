#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

int main(void)
{
    DDRC |= _BV(0);
    PORTC |= _BV(0);
    _delay_ms(100);
    _delay_ms(100);
    PORTC &= ~_BV(0);
    while (1);
}
