#include <diagleds.h>
#include <avr/io.h>

void diagled_init(void)
{
  PORTE |= 0x60;
  PORTD |= 0x78;

  DDRE |= 0x60;
  DDRD |= 0x78;
}

void diagled(unsigned char led, unsigned char state)
{
  switch (led)
    {
    case 0: if (state) { PORTE &= ~_BV(5); } else { PORTE |= _BV(5); } break;
    case 1: if (state) { PORTE &= ~_BV(6); } else { PORTE |= _BV(6); } break;
    case 2: if (state) { PORTD &= ~_BV(3); } else { PORTD |= _BV(3); } break;
    case 3: if (state) { PORTD &= ~_BV(4); } else { PORTD |= _BV(4); } break;
    case 4: if (state) { PORTD &= ~_BV(5); } else { PORTD |= _BV(5); } break;
    case 5: if (state) { PORTD &= ~_BV(6); } else { PORTD |= _BV(6); } break;
    }
}
