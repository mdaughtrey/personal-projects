#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <serutil.h>
#include <typedefs.h>
#include <stepper.h>



const u08 StrIAmAlive [] PROGMEM = "I am alive.\r\n";
const u08 StrIAwait [] PROGMEM = "I await.\r\n";

#define STRING(str) uart_string_buffered(str, sizeof(str));

void delay20ms(unsigned char count)
{
    while (count--)
    {
        _delay_ms(20);
    }
}


void main(void)
{
    stepperInit();
    uart_init();
    sei();

    u08 promptCount = 0;

    STRING(StrIAmAlive);

    while (1)
    {
        u08 data = uart_get_buffered();

        switch (data)
        {
            case 'a': coilAForward(); break;
            case 'A': coilABackward(); break;
            case 'b': coilBForward(); break;
            case 'B': coilBBackward(); break;
            case ']': stepper1Forward(); break;
            case '[': stepper1Back(); break;
            case 'x': coilAOff(); coilBOff(); break;
            case '>': stepForward(10); break;
            case '<': stepBack(10); break;
        }
    }
}

