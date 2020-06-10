#include <stdio.h>
#include <stddef.h>
#include <menu.h>
#include <ui.h>
#include <nvstore.h>
#include <funcs.h>

#include <assert.h>
#include <string.h>

typedef struct _MenuEntry
{
    char * text;
    u08 funcId;
    union
    {
        u08 length;
        u08 numSubs;
    }uu;
    struct _MenuEntry * sub;
}MenuEntry;

#define NUM_ENTRIES(arr) sizeof(arr)/sizeof(*arr)
#define TOBYTE(msb, lsb) ((msb & 0x0f) << 4) | (lsb & 0x0f)
#define MSB(byte) ((byte >> 4) & 0x0f)
#define LSB(byte) (byte & 0x0f)

enum
{
    FUNC_NEW_ENTRY = 1
};

MenuEntry fingerprintItems[] = 
{
    { "Unlock 1", 0, TOBYTE(sizeof("Unlock 1"), 0), NULL },
    { "Unlock 2", 0, TOBYTE(sizeof("Unlock 2"), 0), NULL },
    { "Go 1", 0, TOBYTE(sizeof("Go 1"), 0), NULL },
    { "Go 2", 0, TOBYTE(sizeof("Go 2"), 0), NULL },
    { "Go 3", 0, TOBYTE(sizeof("Go 3"), 0), NULL },
    { "Go 4", 0, TOBYTE(sizeof("Go 4"), 0), NULL },
    { "Go 5", 0, TOBYTE(sizeof("Go 5"), 0), NULL },
    { "Go 6", 0, TOBYTE(sizeof("Go 6"), 0), NULL },
    { "Go 7", 0, TOBYTE(sizeof("Go 7"), 0), NULL },
    { "Go 8", 0, TOBYTE(sizeof("Go 8"), 0), NULL }
};

MenuEntry settingsItems[] = 
{
    { "Info", 0, TOBYTE(sizeof("Info"), 0), NULL },
    { "Finger", 0, TOBYTE(sizeof("Finger"), NUM_ENTRIES(fingerprintItems)), fingerprintItems },
};

//MenuEntry newUrlItems[] = 
//{
//    { MENUTEXT("From Console"), 0, 0, NULL }
//};

//MenuEntry newIdItems[] = 
//{
//    { MENUTEXT("From Console"), 0, 0, NULL },
//    { MENUTEXT("Email Address"), 0, 0, NULL },
//    { MENUTEXT("Copy Existing"), 0, 0, NULL },
//    { MENUTEXT("Random"), 0, 0, NULL }
//};

//MenuEntry newPasswordItems[] = 
//{
//    { MENUTEXT("From Console"), 0, 0, NULL },
//    { MENUTEXT("Copy Existing"), 0, 0, NULL },
//    { MENUTEXT("Random"), 0, 0, NULL }
//};

//MenuEntry newItems[] = 
//{
//    { MENUTEXT("Label"), 0, 0, NULL },
//    { MENUTEXT("URL"), 0, 0, NULL },
//    { MENUTEXT("ID"), 0, NUM_ENTRIES(newIdItems), newIdItems },
//    { MENUTEXT("Password"), 0, NUM_ENTRIES(newPasswordItems), newPasswordItems }
//};

//MenuEntry entryItems[] = 
//{
//    { MENUTEXT("Send ID"), 0, 0, NULL },
//    { MENUTEXT("Send Pwd"), 0, 0, NULL },
//    { MENUTEXT("Delete"), 0, 0, NULL },
//    { MENUTEXT("Edit"), 0, 0, NULL }
//};

MenuEntry mainItems[] = 
{
    { "Settings", 0, TOBYTE(sizeof("Settings"), NUM_ENTRIES(settingsItems)), settingsItems },
    { "New", FUNC_NEW_ENTRY, TOBYTE(sizeof("New"), 0), 0 }
};

MenuEntry menuRoot[] = 
{
    { "", 0, TOBYTE(0, NUM_ENTRIES(mainItems)), mainItems }
};

typedef struct
{
    MenuEntry * menu;
    u08 focusId;

}MenuStackEntry;

MenuStackEntry menu0; // current menu
MenuStackEntry menu1; // parent menu
MenuStackEntry menu2; // grandparent menu

void menuStackPush(MenuEntry * entry, u08 focusId)
{
    menu2.menu = menu1.menu;
    menu2.focusId = menu1.focusId;
    menu1.menu = menu0.menu;
    menu1.focusId = menu0.focusId;
    menu0.menu = entry;
    menu0.focusId = focusId;
}

void menuStackPop(void)
{
    menu0.menu = menu1.menu;
    menu0.focusId = menu1.focusId;
    menu1.menu = menu2.menu;
    menu1.focusId = menu2.focusId;
}

u08 getLengthOf(MenuEntry * entry)
{
    u08 value;
    void * pValue = entry;
    pValue += offsetof(MenuEntry, uu.length);
    value = *(u08 *)pValue;
    return value >> 4;
}

u08 getNumSubsOf(MenuEntry * entry)
{
    u08 value;
    void * pValue = entry;
    pValue += offsetof(MenuEntry, uu.numSubs);
    value = *(u08 *)pValue;
    return value & 0x0f;
}

/*
void copyNvToLocal(u08 id, char * local)
{
    u08 ii;
    char * ptr = nvLabel(id);
    u08 length = nvLabelLength(id);
    for (ii = 0; ii < length; ii++)
    {
        local[ii] = *ptr;
        *ptr++;
    }
    local[ii] = 0;
}

void copyFlashToLocal(u08 id, char * local)
{
    u08 ii;
    char * ptr = mainMenu[id].text;
    u08 length = mainMenu[id].length;
    assert(length < 11);
    for (ii = 0; ii < length; ii++)
    {
        local[ii] = *ptr;
        *ptr++;
    }
    local[ii] = 0;
}
*/

void populateMainMenuItem(u08 id, char * label, u08 length)
{
    if (id >= 0 && id < getNumSubsOf(menuRoot))
    {
        strncpy(label, mainItems[id].text, getLengthOf(&mainItems[id]));
        return;
    }
    nvSelect(id - getNumSubsOf(menuRoot));
    nvLoadLabel(label, length);
}

void showMenu(void)
{
    char leftLabel[11];
    char centerLabel[11];
    char rightLabel[11];

    memset(leftLabel, 0, 11);
    memset(centerLabel, 0, 11);
    memset(rightLabel, 0, 11);

    u08 focus = menu0.focusId;
    MenuEntry * cMenu = menu0.menu;
    MenuEntry * parentPtr = &menu1.menu[menu1.focusId];

    if (cMenu == mainItems && nvNumEntries())
    {
        u08 wrapIt = getNumSubsOf(parentPtr) + nvNumEntries();
        populateMainMenuItem(focus, centerLabel, 11);
        populateMainMenuItem((focus + 1) % wrapIt, rightLabel, 11);
        populateMainMenuItem((focus + wrapIt - 1) % wrapIt, leftLabel, 11);
    }
    else
    {
        MenuEntry * current = &cMenu[focus];
        assert(getLengthOf(current) < 11);

        strncpy(centerLabel, current->text, getLengthOf(current));
        u08 id = (focus + 1) % getNumSubsOf(parentPtr);
        assert(getLengthOf(&cMenu[id]) < 11);
        strncpy(rightLabel, cMenu[id].text, getLengthOf(&cMenu[id]));

        id = (focus + getNumSubsOf(parentPtr) - 1) % getNumSubsOf(parentPtr);
        assert(getLengthOf(&cMenu[id]) < 11);
        strncpy(leftLabel, cMenu[id].text, getLengthOf(&cMenu[id]));
    }

    uiMenuOption(leftLabel, centerLabel, rightLabel);
}

void runFunc(u08 funcId)
{
    switch (funcId)

    {
        case FUNC_NEW_ENTRY:
            funcNewEntry();
            break;
    }
}


void menuInit(void)
{
    uiInit();
    menuStackPush(menuRoot, 0);
    menuDown();
    //menuStackPush(menuRoot->sub, 0);
    //showMenu();
}

void menuLeft(void)
{
    u08 wrapIt = getNumSubsOf(&menu1.menu[menu1.focusId]);
    if (mainItems == menu0.menu)
    {
        wrapIt += nvNumEntries();
    }
    menu0.focusId += wrapIt - 1;
    menu0.focusId %= wrapIt;
    showMenu();
}

void menuRight(void)
{
    u08 wrapIt = getNumSubsOf(&menu1.menu[menu1.focusId]);
    if (mainItems == menu0.menu)
    {
        wrapIt += nvNumEntries();
    }
    menu0.focusId++;
    menu0.focusId %= wrapIt;
    showMenu();
}

void menuUp(void)
{
    menuStackPop();
    showMenu();
}

void menuDown(void)
{
    u08 id = menu0.menu[menu0.focusId].funcId;
    if (id)
    {
        runFunc(id);
        showMenu();
        return;
    }
   // if (menu0[focusId].funcId)
   // {
   //     runFunc(menu0[focusId].funcId);
   // }
//    menuStackPush(&(menu0.menu->sub[menu0.focusId]), 0);
    menuStackPush(menu0.menu[menu0.focusId].sub, 0);

//    MenuEntry * temp = &currentMenu[focusId];
//    currentMenu = currentMenu[focusId].sub;
//    menu1 = temp;
    showMenu();
}

