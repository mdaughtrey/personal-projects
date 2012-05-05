/*
    Title:    AVR-GCC test program #4 for the STK200 eva board
    Author:   Volker Oth
    Date:     4/1999
    Purpose:  Uses the UART to communicate with a terminal program on the pc.
              The "tranceive complete" interrupt is used to send the string 
              "Serial Data from AVR receiced###" continuously to the pc.
              When a byte was received from the pc, the "receive complete"
              interrupt is called, which outputs the byte to PortB where the
              LEDs visualize the 8 bits it consists of.
              UART format: 9600 baud, 8bit, 1 stopbit, no parity
    needed
    Software: AVR-GCC to compile
    needed
    Hardware: ATS90S8515/8535/2313/mega(?) on STK200/300 board
    Note:     To contact me, mail to
                  volkeroth@gmx.de
              You might find more AVR related stuff at my homepage:
                  http://members.xoom.com/volkeroth
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <serutil.h>
#include <cmdproc.h>

/* #define F_CPU            8000000      /\* 4Mhz *\/ */
#define UART_BAUD_RATE      19200      /* 9600 baud */


#define UART_BAUD_SELECT (F_CPU/(UART_BAUD_RATE*16l)-1)

u08 hexTable[16] PROGMEM = "0123456789abcdef";

/* uart globals */
static volatile u08 *uart_data_ptr = 0;
static volatile u08 uart_counter = 0;
/* volatile u08 dataRx; */
/* volatile u08 dataRxFlag; */
u08 dataRxBuffer[RXBUFFERSIZE];
volatile u08 rxHead;
volatile u08 rxTail;


void uart_TXISR(void)
{
  if (--uart_counter)
    {
      UDR = pgm_read_byte_near(uart_data_ptr++);
    }
  else
    {
      UCSRB &= ~_BV(TXCIE);
    }
}

/* signal handler for receive complete interrupt */
void uart_RXISR(void)
{
  dataRxBuffer[rxTail++] = UDR;
  rxTail &= (RXBUFFERSIZE - 1);
}

u16 uart_get_buffered (void)
{
  u16 result = 0;

/*   if (rxTail != rxHead) */
/*     { */
      cli();
      result = dataRxBuffer[rxHead++] | 0x0100;
      rxHead &= (RXBUFFERSIZE - 1);
      
      sei();
/*     } */
  return result;
}

void uart_send_char (u08 character)
{
  while (uart_counter);
  loop_until_bit_is_set(UCSRA, UDRE);
  UDR = character;
}

void uart_send_async (u08 *buf, u08 size)
{
  while (uart_counter);
  loop_until_bit_is_set(UCSRA, UDRE);

  uart_counter = size - 1;
  uart_data_ptr = buf + 1;
  if (size > 1)
    {
/*       sbi(UCSRB, TXCIE); */
      UCSRB |= _BV(TXCIE);
    }
  UDR = pgm_read_byte_near(buf);
}

void uart_send_sync(u08 *buf, u08 size)
{ 
  while (uart_counter);
  loop_until_bit_is_set(UCSRA, UDRE);

  while (size)
    {
      loop_until_bit_is_set(UCSRA, UDRE);
      UDR = pgm_read_byte_near(buf);
      buf++;
      size--;
    }
}

void uart_init(void)
{
  uart_counter = 0;
  rxHead = 0;
  rxTail = 0;

  /* set baud rate */
  UBRR = UART_BAUD_SELECT;

  UCSRB = _BV(RXCIE) | _BV(RXEN) | _BV(TXEN);

}

void uart_send_hex_byte(u08 byte)
{
  uart_send_char(pgm_read_byte_near(hexTable + ((byte & 0xf0) >> 4)));
  uart_send_char(pgm_read_byte_near(hexTable + (byte & 0x0f)));
}
