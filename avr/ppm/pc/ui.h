#ifndef INCLUDE_UI_H
#define INCLUDE_UI_H

#include <typedefs.h>

void uiInit(void);
void uiMenuOption(char * left, char * center, char * right); 

void uiPrompt(const char * label, u08 length);
void uiInputString(char * dest, u08 length);

#endif // INCLUDE_UI_H

