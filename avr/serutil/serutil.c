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
#include <avr/signal.h>
#include <avr/pgmspace.h>
#include <serutil.h>
#include <cmdproc.h>

#define F_CPU            8000000      /* 4Mhz */
#define UART_BAUD_RATE      9600      /* 9600 baud */


#define UART_BAUD_SELECT (F_CPU/(UART_BAUD_RATE*16l)-1)

/* uart globals */
static volatile u08 *uart_data_ptr = 0;
static volatile u08 uart_counter = 0;
volatile u08 dataRx;
volatile u08 dataRxFlag;

SIGNAL(SIG_UART_TRANS)      
     /* signal handler for uart txd ready interrupt */
{
  if (--uart_counter)
    {
      outb(UDR, pgm_read_byte_near(uart_data_ptr++));
    }
  else
    {
      cbi(UCSRB, TXCIE);
    }
}

SIGNAL(SIG_UART_RECV)      
/* signal handler for receive complete interrupt */
{
  dataRx = inp(UDR);
  dataRxFlag = 1;
}

void uart_send_char (u08 character)
{
  while (uart_counter);
  loop_until_bit_is_set(UCSRA, UDRE);
  outb(UDR, character);
}

void uart_send_async (u08 *buf, u08 size)
{
  while (uart_counter);
  loop_until_bit_is_set(UCSRA, UDRE);

  uart_counter = size - 1;
  uart_data_ptr = buf + 1;
  if (size > 1)
    {
      sbi(UCSRB, TXCIE);
    }
  outb(UDR, pgm_read_byte_near(buf));
}

void uart_send_sync(u08 *buf, u08 size)
{ 
  while (uart_counter);
  loop_until_bit_is_set(UCSRA, UDRE);

  while (size)
    {
      loop_until_bit_is_set(UCSRA, UDRE);
      outb(UDR, pgm_read_byte_near(buf));
      buf++;
      size--;
    }
}

void uart_init(void)
{
  uart_counter = 0;
  /* set baud rate */
  outb(UBRR, (u08)UART_BAUD_SELECT);

  /* enable RxD/TxD and ints */
  outb(UCSRB, (1<<RXCIE)|(0<<TXCIE)|(1<<RXEN)|(1<<TXEN));       

}

