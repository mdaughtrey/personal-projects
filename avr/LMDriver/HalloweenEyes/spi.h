#ifndef INCLUDE_SPI_H
#define INCLUDE_SPI_H

#include <avr/interrupt.h>
#include <typedefs.h>

#define SET_LEFT_SS PORTC |= _BV(0)
#define RESET_LEFT_SS PORTC &= ~_BV(0) 

#define SET_RIGHT_SS PORTC |= _BV(1)
#define RESET_RIGHT_SS PORTC &= ~_BV(1) 

#define SET_SCK PORTB |= _BV(1)
#define RESET_SCK PORTB &= ~_BV(1)

#define SET_MOSI PORTB |= _BV(2)
#define RESET_MOSI PORTB &= ~_BV(2)
#define MOSIBUFFERSIZE 8
#define MISOBUFFERSIZE 8

void spi_init(void);
void miso_push(u08 data);
void mosi_push(u08 data);

#endif

extern u08 mosiBuffer[MOSIBUFFERSIZE];
extern u08 misoBuffer[MISOBUFFERSIZE];

extern  u08 mosiHead;
extern u08 mosiTail ;
extern u08 misoHead ;
extern u08 misoTail ;
extern u08 spiEvent ;



