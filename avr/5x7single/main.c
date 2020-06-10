#include <avr/io.h>
#include <avr/signal.h>
#include <avr/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <typedefs.h>
#include <serutil.h>
#include <cmdproc.h>
#include <displaymux.h>

volatile u08 timer0Event;
volatile u08 spiEvent;
volatile u08 spiData;

/* Timer 0 overflow interrupt handler */
SIGNAL(SIG_OVERFLOW0)
{
  timer0Event = 1;
}

/* Serial Peripheral Interface interrupt handler */
SIGNAL(SIG_SPI)
{
  spiEvent = 1;
  spiData = inp(SPDR);
}

void spi_init(void)
{
  outb(SPCR, BV(SPIE) | BV(SPE)); /* Enable SPI and interrupts, other defaults */
}

#define CURRENTCHAR_COUNT 5000
#define CURRENTMIRROR_COUNT 12345
#define CURRENTINVERT_COUNT 25678
#define CURRENTREVERSE_COUNT 33456
#define CURRENTDIM_COUNT 54321

int main(void)
{
  u08 currentChar = 0;
  u08 currentMirror = 0;
  u08 currentInvert = 0;
  u08 currentReverse = 0;
  u08 currentDim = 0;
  u16 char_count = 0;
  u16 mirror_count = 0;
  u16 invert_count = 0;
  u16 reverse_count = 0;
  u16 dim_count = 0;
  uart_init();			/* init serial interface */
  dm_init();			/* init displaymux */
  cmd_Init();			/* init command processor */
  spi_init();			/* init serial peripheral interface */

  

  sei();

  while (1)
    {
      if (++char_count == CURRENTCHAR_COUNT)
	{
	  dm_setChar(++currentChar);
	  char_count = 0;
	}
      if (++mirror_count == CURRENTMIRROR_COUNT)
	{
	  dm_setMirror(++currentMirror & 0x01 ? '1' : '0');
	  mirror_count = 0;
	}
      if (++invert_count == CURRENTINVERT_COUNT)
	{
	  dm_setInvert(++currentInvert & 0x01 ? '1' : '0');
	  invert_count = 0;
	}
      if (++reverse_count == CURRENTREVERSE_COUNT)
	{
	  dm_setReverse(++currentReverse & 0x01 ? '1' : '0');
	  reverse_count = 0;
	}
      if (++dim_count == CURRENTDIM_COUNT)
	{
	  dm_setDim(++currentDim & 0x01 ? '1' : '0');
	  dim_count = 0;
	}

      /* update the display if necessary */
      if (timer0Event == 1)
	{
	  dm_timerHandler();
	  timer0Event = 0;
	}
      /* UART receive handling */
      if (dataRxFlag == 1)
	{
	  cmd_dataHandler(dataRx);
	  dataRxFlag = 0;
	}
      if (spiEvent == 1)
	{
	  cmd_dataHandler(spiData);
	  spiEvent = 0;
	}
    }
  return 0;
}
