#include <nav.h>
#include <utils.h>

NavEvent navWaitForEvent(void)
{
    while (1)
    {
        char ch = utilWaitForKeypress();
        switch (ch)
        {
            case 'h':
            case 'H':
                return NAV_LEFT;

            case 'j':
            case 'J':
                return NAV_DOWN;

            case 'k':
            case 'K':
                return NAV_UP;

            case 'l':
            case 'L':
                return NAV_RIGHT;

        }
    }
}
