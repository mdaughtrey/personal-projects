#ifndef __SERUTIL_H
#define __SERUTIL_H

#include <typedefs.h>


//void uart_TXISR(void);
//void uart_RXISR(void);

void uart_init(void);
//void uart_send_char (u08 character);
//void uart_send_sync (u08 *buf, u08 size);
void uart_string_buffered (const u08 *buf, u08 size);
void uart_send_hex_byte(u08 byte);
u08 uart_get_buffered (void);
void uart_send_buffered(u08 data);
void uart_send_ram_string(char * str);
void uart_send_u16(u16 u);
//void uart_send_hex_byte(u08 byte);


#define STRING(str) uart_string_buffered(str, sizeof(str));

#endif /* __SERUTIL_H */

/* extern volatile u08 dataRxFlag; */
/* extern volatile u08 dataRx; */

