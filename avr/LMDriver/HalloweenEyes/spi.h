#ifndef INCLUDE_SPI_H
#define INCLUDE_SPI_H

#include <avr/interrupt.h>
#include <typedefs.h>

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



