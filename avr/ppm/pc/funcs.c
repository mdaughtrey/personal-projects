#include <funcs.h>
#include <ui.h>
#include <nvstore.h>
#include <console.h>

char inputString[255];

void funcNewEntry(void)
{
    nvAllocate();
    uiPrompt("Label:", 6);
    uiInputString(inputString, 255);
    nvSetLabel(inputString);
    uiPrompt("URL:", 4);
    uiInputString(inputString, 255);
    nvSetUrl(inputString);
    uiPrompt("Id:", 3);
    uiInputString(inputString, 255);
    nvSetId(inputString);
    uiPrompt("Password:", 9);
    uiInputString(inputString, 255);
    nvSetPassword(inputString);
    //nvCommit(nvIndex);
}
