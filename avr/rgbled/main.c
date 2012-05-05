#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <avr/wdt.h>

#define DELAYCOUNT_INIT 2000

char progCycle1[] PROGMEM =
  {
    0xff, 0x00, 0x00,           /* red */
    0xff, 0x00, 0xff,           /* magenta */
    0x00, 0x00, 0xff,           /* blue */
    0x00, 0xff, 0xff,           /* cyan */
    0x00, 0xff, 0x00,           /* green */
    0xff, 0xff, 0x00,           /* yellow */
    0xff, 0xff, 0xff,           /* white */
  };

#define STEPCOUNT_LIMIT (sizeof(progCycle1)/3)

void allOff(void)
{
  PORTF = 0;
  PORTA = 0;
}

void redOn(void)
{
  PORTF |= _BV(2);
  PORTA |= _BV(0) | _BV(5);
}

void greenOn(void)
{
  PORTF |= _BV(1);
  PORTA |= _BV(2) | _BV(4);
}

void blueOn(void)
{
  PORTF |= _BV(3);
  PORTA |= _BV(3) | _BV(6);

}


int main(void)
{
  char x = 0;
  DDRF = 0xff;
  DDRA = 0xff;

  int redTemp = 0;
  int greenTemp = 0;
  int blueTemp = 0;
  int redTarget = 0;
  int greenTarget = 0;
  int blueTarget = 0;
  int delayCount = DELAYCOUNT_INIT;
  unsigned char * memPtr = progCycle1;
  int stepCount = 0;
  
  while (1)    
    {
      int count;
      allOff();
      for (count = 512; count > 0; count -= 4)
	{        
	  if (count < redTemp) 
	    {
	      PORTF |= _BV(2);
	      PORTA |= _BV(0) | _BV(5);
	    }
	  if (count < greenTemp) 
	    {
	      PORTF |= _BV(1);
	      PORTA |= _BV(2) | _BV(4);
	    }
	  if (count < blueTemp)
	    { 
	        PORTF |= _BV(3);
		PORTA |= _BV(3) | _BV(6);
	    }
	}
      if (redTemp < redTarget) { redTemp++; }
      else if (redTemp > redTarget) { redTemp--; }
      
      if (greenTemp < greenTarget) { greenTemp++; }
      else if (greenTemp > greenTarget) { greenTemp--; }
      
      if (blueTemp < blueTarget) { blueTemp++; }
      else if (blueTemp > blueTarget) { blueTemp--; }
      
      if (redTemp == redTarget && blueTemp == blueTarget && greenTemp == greenTarget && --delayCount == 0)
	{
	  redTarget = pgm_read_byte_near(memPtr++) * 2;
	  greenTarget = pgm_read_byte_near(memPtr++) * 2;
	  blueTarget = pgm_read_byte_near(memPtr++) * 2;
	  delayCount = DELAYCOUNT_INIT;
	  if (++stepCount == STEPCOUNT_LIMIT)
	    {
	      stepCount = 0;
	      memPtr = progCycle1;
	    }
	}
    }
    return 0;
}
