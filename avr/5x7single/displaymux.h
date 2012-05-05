#ifndef __DISPLAYMUX_H
#define __DISPLAYMUX_H

#include <typedefs.h>

void dm_init(void);
void dm_setChar(u08 set);
void dm_setInvert(u08 set);
void dm_setReverse(u08 set);
void dm_setMirror(u08 set);
void dm_setColumn(u08 column, u08 set);
void dm_displayProgrammed(u08 set);

#endif /* __DISPLAYMUX_H */
