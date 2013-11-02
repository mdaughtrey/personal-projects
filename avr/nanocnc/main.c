#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <serutil.h>
#include <typedefs.h>
#include <stepper.h>

const u08 StrIAmAlive [] PROGMEM = "I am alive.\r\n";
const u08 StrIAwait [] PROGMEM = "I await.\r\n";


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
    s08 state = 0;

    STRING(StrIAmAlive);

    while (1)
    {
        u08 data = uart_get_buffered();

        switch (data)
        {
            case '=': state = nextState(state); coilState(state); break;
            case '-': state = prevState(state); coilState(state);break;
            case 'a': coilAForward(); break;
            case 'b': coilBForward(); break;
            case 'A': coilABackward(); break;
            case 'B': coilBBackward(); break;
            case ']': stepForward(1); break;
            case '[': stepBack(1); break;
            case 'x': coilAOff(); coilBOff(); break;
            case '>': stepForward(10); break;
            case '<': stepBack(10); break;
            case '0':
            case '1':
            case '2':
            case '3':
                      coilState(data - '0');
                      break;
        }
    }
}

