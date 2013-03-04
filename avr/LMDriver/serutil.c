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
#include <util/delay.h>
//#include <cmdproc.h>

/* #define F_CPU            8000000      /\* 4Mhz *\/ */
#define UART_BAUD_RATE      38400      

#define UART_BAUD_SELECT (F_CPU/(UART_BAUD_RATE*16l)-1)
#define BUFFERSIZE 8 

//u08 hexTable[16] PROGMEM = "0123456789abcdef";
extern u08 hexTable[16];

/* uart globals */
//static volatile u08 *uart_data_ptr = 0;
//static volatile u08 uart_counter = 0;
/* volatile u08 dataRx; */
/* volatile u08 dataRxFlag; */
volatile u08 rxBuffer[BUFFERSIZE];
volatile u08 txBuffer[BUFFERSIZE];
volatile u08 rxHead = 0;
volatile u08 rxTail = 0;
volatile u08 txHead = 0;
volatile u08 txTail = 0;


//void uart_TXISR(void)
//{
//  if (--uart_counter)
//    {
//      UDR = pgm_read_byte_near(uart_data_ptr++);
//    }
//  else
//    {
//      UCSRB &= ~_BV(TXCIE);
//    }
//}

/* signal handler for receive complete interrupt */
ISR(USART0_RX_vect)
{
  rxBuffer[rxHead++] = UDR;
  rxHead &= (BUFFERSIZE - 1);
}

ISR(USART0_UDRE_vect)
{
    UDR = txBuffer[txTail];
    txTail++;
    txTail &= (BUFFERSIZE - 1);
    if (txTail == txHead)
    {
        UCSRB &= ~_BV(UDRIE);
    }
}

u16 uart_get_buffered (void)
{
    u16 result = 0;

    if (rxTail != rxHead) 
    { 
        cli();
        result = rxBuffer[rxTail++] | 0x0100;
        rxTail &= (BUFFERSIZE - 1);

        sei();
    } 
    return result;
}

//void uart_send_char (u08 character)
//{
//  while (uart_counter);
//  loop_until_bit_is_set(UCSRA, UDRE);
//  UDR = character;
//}

//void uart_send_async (u08 *buf, u08 size)
//{
//  while (uart_counter);
//  loop_until_bit_is_set(UCSRA, UDRE);
//
//  uart_counter = size - 1;
//  uart_data_ptr = buf + 1;
//  if (size > 1)
//    {
///*       sbi(UCSRB, TXCIE); */
//      UCSRB |= _BV(TXCIE);
//    }
//  UDR = pgm_read_byte_near(buf);
//}
//
//void uart_send_sync(u08 *buf, u08 size)
//{ 
//  while (uart_counter);
//  loop_until_bit_is_set(UCSRA, UDRE);
//
//  while (size)
//    {
//      loop_until_bit_is_set(UCSRA, UDRE);
//      UDR = pgm_read_byte_near(buf);
//      buf++;
//      size--;
//    }
//}

void uart_init(void)
{
  //uart_counter = 0;
  //rxHead = 0;
  //rxTail = 0;
  //txHead = 0;
  //txTail = 0;

  /* set baud rate */
  UBRR = UART_BAUD_SELECT;
  UCSRB = _BV(RXCIE) | _BV(RXEN) | _BV(TXEN);
  //UCSRB =  _BV(RXCIE) | _BV(TXCIE) | _BV(RXEN) | _BV(TXEN);

//  sei();
//  u08 data;
//  while (1)
//  {
//  for (data = 'a'; data <= 'j'; data++)
//  {
//      uart_send_buffered(data);
//  }
//      _delay_ms(100);
//      _delay_ms(100);
//      _delay_ms(100);
//      _delay_ms(100);
//      _delay_ms(100);
//  }
}

void dumpQueue(void)
{
    u08 ii;
    UDR = '\r';
    while (!(UCSRA & _BV(UDRE)));
    UDR = '\n';
    while (!(UCSRA & _BV(UDRE)));
    for (ii = 0; ii < BUFFERSIZE; ii++)
    {
        UDR = '0' + ii;
        while (!(UCSRA & _BV(UDRE)));
        UDR = txBuffer[ii];
        while (!(UCSRA & _BV(UDRE)));
    }
    UDR = 't';
    while (!(UCSRA & _BV(UDRE)));
    UDR = '0' + txTail;
    while (!(UCSRA & _BV(UDRE)));
    UDR = '0' + txHead;
    while (!(UCSRA & _BV(UDRE)));
    UDR = '\r';
    while (!(UCSRA & _BV(UDRE)));
    UDR = '\n';
    while (!(UCSRA & _BV(UDRE)));
}

void uart_send_buffered(u08 data)
{
    txBuffer[txHead] = data;
    u08 tempHead = txHead;
    tempHead++;
    tempHead &= (BUFFERSIZE - 1);
    while (tempHead == txTail) ;
    cli();
    txHead = tempHead;
//    dumpQueue();
    sei();
    UCSRB |= _BV(UDRIE);
}

void uart_send_hex_byte(u08 byte)
{
  uart_send_buffered(pgm_read_byte_near(hexTable + ((byte & 0xf0) >> 4)));
  uart_send_buffered(pgm_read_byte_near(hexTable + (byte & 0x0f)));
}
