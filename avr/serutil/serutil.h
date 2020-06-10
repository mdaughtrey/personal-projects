#ifndef __SERUTIL_H
#define __SERUTIL_H

typedef unsigned char  u08;
typedef          char  s08;
typedef unsigned short u16;
typedef          short s16;

void uart_init(void);
void uart_send(u08 *buf, u08 size);

#endif /* __SERUTIL_H */
