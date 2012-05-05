#include <stdio.h>
#include <nvstore.h>
#include <menu.h>
#include <nav.h>

int main (int argc, char ** argv)
{ 
    nvInit();
    menuInit();

    NavEvent nav;

    while (1)
    {
        nav = navWaitForEvent();
        switch (nav)
        {
            case NAV_LEFT: menuLeft(); break;
            case NAV_RIGHT: menuRight(); break;
            case NAV_UP: menuUp(); break;
            case NAV_DOWN: menuDown(); break;
            //case NAV_TAP: menuSelect(); break;
            case NAV_ACCESS_1:
            case NAV_ACCESS_2:
            case NAV_HOTKEY_1:
            case NAV_HOTKEY_2:
            case NAV_HOTKEY_3:
            case NAV_HOTKEY_4:
            case NAV_HOTKEY_5:
            case NAV_HOTKEY_6:
            case NAV_HOTKEY_7:
            case NAV_HOTKEY_8:
            case NAV_HOTKEY_9:
            default:
                printf("Unknown NAV option\n");
                return 1;
                break;
        }
    }
}
