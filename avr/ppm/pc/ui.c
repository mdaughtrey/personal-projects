#include <stdio.h>
#include <ui.h>
#include <menu.h>
#include <lcd.h>

#include <string.h>
#include <stdio.h>

void uiInit(void)
{
    lcdInit();
}

void uiMenuOption(char * left, char * center, char * right)
{
    u08 ctrLength = strlen(center);
    u08 pos = (LCD_COLS / 2) - (ctrLength / 2) - 1;
    lcdLine(0, 0, left, pos); 
    lcdLine(0, pos++, "[", 1);
    lcdLine(0, pos, center, ctrLength);
    pos += ctrLength;
    lcdLine(0, pos++, "]", 1);
    lcdLine(0, pos, right, LCD_COLS - pos);

    lcdRefresh();
}

void uiPrompt(const char * label, u08 length)
{
//    lcdClear(0);
//    lcdLine(0, 0, label, length);
//    lcdRefresh();
    printf("%s\n", label);
}

void uiInputString(char * dest, u08 length)
{
    memset(dest, 0, length);
    while (0 == strlen(dest))
    {
        fgets(dest, length, stdin);
        while (dest[strlen(dest) - 1] == '\n')
        {
            dest[strlen(dest) - 1] = 0;
        }
    }
    printf ("result [%s]\n", dest);
}
