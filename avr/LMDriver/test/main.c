
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <avr/wdt.h>
#include <typedefs.h>
#include <serutil.h>
#include <cmdproc.h>
#include <displaymux.h>
//#include <diagleds.h>


volatile u08 timer0Event;
#define SPIBUFFERSIZE 16
u08 spiBuffer[SPIBUFFERSIZE];
volatile u08 spiHead = 0;
volatile u08 spiTail = 0;
u08 spiEvent = 0;
u08 comEvent = 0;
/* volatile u08 spiEvent; */
/* volatile u08 spiData; */

extern u08 dataRxBuffer[RXBUFFERSIZE];
extern volatile u08 rxHead;
extern volatile u08 rxTail;



/* ---------------------------------------------------------------------- */
/*      Begin Interrupt Service Routines                                        */
/* ---------------------------------------------------------------------- */
/* Timer 0 overflow interrupt handler */
ISR(SIG_OVERFLOW0)
{
   timer0Event = 1;
}


/* Serial Peripheral Interface interrupt handler */
ISR(SIG_SPI)
{
  spiBuffer[spiHead++] = SPDR;
  spiHead &= (SPIBUFFERSIZE - 1);
  spiEvent = 1;
}

ISR(USART0_TX_vect)
{
  uart_TXISR();
}

/* signal handler for receive complete interrupt */
ISR(USART0_RX_vect)
{
  dataRxBuffer[rxTail++] = UDR;
  rxTail &= (RXBUFFERSIZE - 1);
  comEvent = 1;
}
/* ---------------------------------------------------------------------- */
/*      End Interrupt Service Routines                                        */
/* ---------------------------------------------------------------------- */


void spi_init(void)
{
  volatile char IOReg;
  DDRB |= _BV(PB3); 		/* Enable MISO for output */
  PRR &= ~_BV(PRSPI);		/* Turn on the SPI module */

  SPCR = _BV(SPE) | _BV(SPIE);		/* Enable SPI interrupts, MSB first */

  IOReg = SPSR;
  IOReg = SPDR;

/*   SPCR =  _BV(SPIE) | _BV(SPE) | _BV(DORD); /\* Enable SPI and interrupts, LSB first, other defaults *\/ */
  /* SCK is low when idle, sample on leading edge */
}

/* u08 msgSendingSync[] PROGMEM = "Sending Sync "; */
/* u08 msgSendingAsync[] PROGMEM = "Sending Async "; */

/* u08 msgMainLoop[] PROGMEM = "Main Loop\r\n"; */
/* u08 msgSPIEvent[] PROGMEM = "SPI Event\r\n"; */

int main(void)
{
  /* disable the watchdog */
  MCUSR = 0;
  wdt_disable();

  uart_init();

  cmd_Init();			/* init command processor */

  dm_init();			/* init displaymux */

  spi_init();   	/* init serial peripheral interface */
  //  diagled_init();
  
  sei();
  u16 intCount = 0;
  u08 state = 0;
  u08 debugCount = 0;
  u08 data;

  while (1)
    {

      if (spiEvent == 1)
	{

	  cli();
	  data = spiBuffer[spiTail++];
	  spiTail &= (SPIBUFFERSIZE - 1);
	  if (spiHead == spiTail)
	    {
	      spiEvent = 0;
	    }
	  sei();
 	  cmd_dataHandler(data);
	  continue;
	}

      /* UART receive handling */
      if (comEvent == 1)
	{
	  cli();
	  data = dataRxBuffer[rxHead++];
	  rxHead &= (RXBUFFERSIZE - 1);
	  if (rxHead == rxTail)
	    {
	      comEvent = 0;
	    }
	  sei();
	  cmd_dataHandler(data);
	  continue;

	}
      if (data == '?')
	{
	  while (1)
	    {
	      _delay_loop_2(0); 
	      _delay_loop_2(0); 
	      _delay_loop_2(0); 
	      _delay_loop_2(0); 
	      _delay_loop_2(0); 
	      _delay_loop_2(0);
	      
	      dm_timerHandler();
	    }
	}

#ifndef STEPDISP
      /* update the display if necessary */
/*       if (timer0Event == 1) */
/*       	{ */
	  
/* 	  timer0Event = 0; */
/* 	  continue; */
/* 	} */
#endif

    }
  return 0;
}
