#ifndef __DISPLAYMUX_H
#define __DISPLAYMUX_H

#include <typedefs.h>

void dm_init(void);
void dm_setChar(u08 which, u08 set);
void dm_setFlip(u08 which, u08 set);
void dm_setReverse(u08 which, u08 set);
void dm_setMirror(u08 which, u08 set);
void dm_displayProgrammed(u08 which, u08 set);
void dm_blank(u08 which, u08 set);
void dm_shift(u08 which, u08 direction, u08 count);
void dm_roll(u08 which, u08 direction, u08 count);
void dm_setPalette(u08 which, u08 index);
void dm_reset(u08 which);
void dm_progColumn(u08 which, u08 column, u08 data);
void dm_pixel(u08 which, u08 set, u08 row, u08 column);
void dm_copyToCustom(u08 which, u08 charIndex);
void dm_dumpdisp(u08 which);
void dm_setsense(u08 row, u08 column);


#endif /* __DISPLAYMUX_H */

