#include <stdio.h>
#include <lcd.h>
#include <string.h>

// lets pretend we're a 2x16 LCD

char line0[LCD_COLS + 1];
char line1[LCD_COLS + 1];


void lcdInit(void)
{
    u08 ii;
    for (ii = 0; ii < LCD_COLS; ii++)
    {
        line0[ii] = '.';
        line1[ii] = '.';
    }
    line0[16] = 0;
    line1[16] = 0;
}

void lcdLine(u08 row, u08 column, const char * ptr, u08 length)
{
    char * line = (row == 0 ? line0 : line1 );
    while (length)
    {
        line[column++] = (*ptr ? *ptr : ' ');
        ptr++;
        length--;
    }
}

void lcdRefresh(void)
{
    printf("+----------------+\n");
    printf("|%s|\n", line0);
    printf("|%s|\n", line1);
    printf("+----------------+\n");
    printf("\n");
}

void lcdClear(u08 line)
{
    if (0 == line)
    {
        memset(line0, 0, LCD_COLS + 1);
    }
    else
    {
        memset(line0, 0, LCD_COLS + 1);
    }
    lcdRefresh();
}


