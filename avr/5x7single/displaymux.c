#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <displaymux.h>
#include <typedefs.h>

extern u08 font5x7[];
volatile u08 progColumn[5];
volatile u08 charIndex;
volatile u08 colIndex;
volatile u08 flags;

#define NUM_ROWS (7)
#define NUM_COLUMNS (5)

#define FLAG_INVERT (BV(0))	/* upside down */
#define FLAG_MIRROR (BV(1))	/* left/right */
#define FLAG_REVERSE (BV(2))	/* black on white */
#define FLAG_DIM (BV(3))	/* bright/dim */
#define FLAG_PROGCOLS (BV(4))	/* display programmed columns */

void dm_timerHandler(void)
{
  register u08 data;
  if (flags & FLAG_MIRROR)
    {
      if (flags & FLAG_PROGCOLS)
	{
	  data = progColumn[NUM_COLUMNS - colIndex - 1];
	}
      else
	{
	  data = pgm_read_byte_near(font5x7 +
				    (charIndex * NUM_COLUMNS) +
				    (NUM_COLUMNS - colIndex - 1));
	}
    }
  else
    {
      if (flags & FLAG_PROGCOLS)
	{
	  data = progColumn[colIndex];
	}
      else
	{
	  data = pgm_read_byte_near(font5x7 +
				    (charIndex * NUM_COLUMNS) +
				    colIndex);
	}
    }

  if (flags & FLAG_REVERSE)
    {
      data = ~data;
    }
  if (flags & FLAG_INVERT)
    {
      data = ((data & 0xAA) >>  1) | ((data & 0x55) <<  1);
      data = ((data & 0xCC) >>  2) | ((data & 0x33) <<  2);
      data = (data >> 4) | (data << 4);
      data = data >> 1;
  }

  outb(PORTC, 0x1f);
  outb(PORTD, data << 2); 
  outb(PORTB, data >> 6);
  outb(PORTC, ~(1 << colIndex));

  if (flags & FLAG_DIM)
    {
      outb(PORTC, 0xff);
    }


  colIndex++;
  if (colIndex == NUM_COLUMNS)
    {
      colIndex = 0;
    }
}


void dm_init(void)
{
  colIndex = 0;
  charIndex = 0;
  outb(DDRC, 0x1f);		/* columns */
  outb(DDRD, 0xfc); 		/* rows */
  outb(DDRB, 0x01); 		/* rows */
  outb(TCCR0, 0x01);		/* Timer 0 prescaler =CLK */
  sbi(TIMSK, TOIE0);
}

void dm_setChar(u08 set)
{
  cbi(TIMSK, TOIE0); 
  charIndex = set;
  colIndex = 0;
  sei();
  sbi(TIMSK, TOIE0); 
}

void dm_setReverse(u08 set)
{
  if (set == '0')
    {
      flags &= ~FLAG_REVERSE;
    }
  else
    {
      flags |= FLAG_REVERSE;
    }
}

void dm_setInvert(u08 set)
{
  if (set == '0')
    {
      flags &= ~FLAG_INVERT;
    }
  else
    {
      flags |= FLAG_INVERT;
    }
}

void dm_setMirror(u08 set)
{
  if (set == '0')
    {
      flags &= ~FLAG_MIRROR;
    }
  else
    {
      flags |= FLAG_MIRROR;
    }
}

void dm_setDim(u08 set)
{
  if (set == '0')
    {
      flags &= ~FLAG_DIM;
    }
  else
    {
      flags |= FLAG_DIM;
    }
}

void dm_setColumn(u08 column, u08 set)
{
  progColumn[column] = set;
}

void dm_displayProgrammed(u08 set)
{
  if (set == '0')
    {
      flags &= ~FLAG_PROGCOLS;
    }
  else
    {
      flags |= FLAG_PROGCOLS;
    }

}
