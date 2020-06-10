#ifndef INCLUDE_LCD_H
#define INCLUDE_LCD_H

#include <typedefs.h>

#define LCD_COLS 16 
#define LCD_ROWS 2

void lcdInit(void);
void lcdLine(u08 row, u08 column, const char * ptr, u08 length);
void lcdRefresh(void);
void lcdClear(u08 line);

#endif // INCLUDE_LCD_H
