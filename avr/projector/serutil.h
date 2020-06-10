#ifndef __SERUTIL_H
#define __SERUTIL_H

#include <typedefs.h>


//void uart_TXISR(void);
//void uart_RXISR(void);

void uart_init(void);
//void uart_send_char (u08 character);
//void uart_send_sync (u08 *buf, u08 size);
//void uart_send_async (u08 *buf, u08 size);
void uart_send_hex_byte(u08 byte);
u16 uart_get_buffered (void);
void uart_send_buffered(u08 data);
//void uart_send_hex_byte(u08 byte);



#endif /* __SERUTIL_H */

/* extern volatile u08 dataRxFlag; */
/* extern volatile u08 dataRx; */

