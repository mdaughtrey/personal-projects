#ifndef __DISPLAYMUX_H
#define __DISPLAYMUX_H

#include <typedefs.h>

void dm_init(void);
void dm_blank(u08 which, u08 set);
void dm_progColumn(u08 which, u08 column, u08 data);
void dm_pixel(u08 which, u08 set, u08 row, u08 column);


#endif /* __DISPLAYMUX_H */

