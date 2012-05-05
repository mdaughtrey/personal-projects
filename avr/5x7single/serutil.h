#ifndef __SERUTIL_H
#define __SERUTIL_H

#include <typedefs.h>

void uart_init(void);
void uart_send_char (u08 character);
void uart_send_sync (u08 *buf, u08 size);
void uart_send_async (u08 *buf, u08 size);

#endif /* __SERUTIL_H */

extern volatile u08 dataRxFlag;
extern volatile u08 dataRx;

