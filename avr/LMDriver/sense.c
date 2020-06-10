#include <sense.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <displaymux.h>

u08 * sensePort;
u08 * senseBit;

typedef enum
{
    IDLE = 0,
    CHARGE,
    READ
}State;
u08 count = 0;

State senseState;
/* Timer 0 overflow interrupt handler */
//ISR(SIG_OVERFLOW0)
//{
//}
//

u08 sense_get(void)
{
    return 0;
}
