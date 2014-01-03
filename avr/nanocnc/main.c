#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <serutil.h>
#include <typedefs.h>
#include <stepper.h>

#include <string.h>
#include <stdio.h>


const u08 StrIAmAlive [] PROGMEM = "I am alive.\r\n";
const u08 StrIAwait [] PROGMEM = "I await.\r\n";
const u08 StrImmediateMode [] PROGMEM = "Immediate Mode\r\n";
const u08 StrCoordinateMode [] PROGMEM = "Coordinate Mode\r\n";
const u08 StrLowLevelMode [] PROGMEM = "Low Level Mode\r\n";
const u08 StrCoordInputIs [] PROGMEM = "cmdInput is ";
const u08 StrInvalidCommand [] PROGMEM = "Invalid Command: ";
const u08 StrParamCount [] PROGMEM = "ParamCount: ";
const u08 StrOk [] PROGMEM = "OK\r\n";

typedef enum
{
    IMMEDIATE,
    COORDINATE,
    LOWLEVEL
} Mode;

Mode mode = IMMEDIATE;
char cmdInput[32];

void delay20ms(unsigned char count)
{
    while (count--)
    {
        _delay_ms(20);
    }
}

void processCommand(void)
{
    uart_crlf();
//    uart_send_ram_string(cmdInput);
//    uart_crlf();
    if (!strncmp(cmdInput, "!", 1))
    {
        STRING(StrIAmAlive);
        return;
    }
    if (!strncmp(cmdInput, "i", 1))
    {
        STRING(StrImmediateMode);
        mode = IMMEDIATE;
        return;
    }
    if (!strncmp(cmdInput, "setzero", 7))
    {
        stepSetZero();
        STRING(StrOk);
        return;
    }
    if (!strncmp(cmdInput, "goto", 4))
    {
        u16 x;
        u16 y;
        u16 z;
        u08 paramCount;
        paramCount = sscanf(cmdInput, "%s %u %u %u",
                cmdInput, &x, &y, &z);
        if (paramCount < 4)
        {
            STRING(StrInvalidCommand);
            uart_send_ram_string(cmdInput);
            uart_crlf();
            return;
        }
        stepGoto(x, y, z);
        STRING(StrOk);

    //    stepReportAxes();
    }
}


void lowLevelMode(u08 data)
{
    switch (data)
    {
        case 'i':
            mode = IMMEDIATE;
            break;

        case 'a': coilAxisXAForward(); break;
        case 'A': coilAxisXABack(); break;
        case 'b': coilAxisXBForward(); break;
        case 'B': coilAxisXBBack(); break;

        case 'c': coilAxisYAForward(); break;
        case 'C': coilAxisYABack(); break;
        case 'd': coilAxisYBForward(); break;
        case 'D': coilAxisYBBack(); break;
        case 'x':
                  coilAxisXAOff();
                  coilAxisXBOff();
                  coilAxisYAOff();
                  coilAxisYBOff();
                  break;
    }
}

void main(void)
{
    uart_init();
    stepperInit();
    sei();

    s08 state = 0;

    STRING(StrIAmAlive);

    while (1)
    {
        u08 data = uart_get_buffered();
        if (IMMEDIATE == mode)
        {
            switch (data)
            {
                case '!': STRING(StrIAmAlive); break;
                case '9': stepForward(); stepRight(); break;
                case '7': stepForward(); stepLeft(); break;
                case '1': stepBack(); stepLeft(); break;
                case '3': stepBack(); stepRight(); break;
                case '8': stepForward(); break;
                case '2': stepBack(); break;
                case '4': stepLeft(); break;
                case '6': stepRight(); break;
                case '5': stepOff(); break;
                case '?': stepReportAxes(); break;
                case 'z': stepSetZero(); break;
                case 'c': 
                          STRING(StrCoordinateMode);
                          memset(cmdInput, 0, sizeof(cmdInput));
                          mode = COORDINATE;
                          break;
                case 'l': 
                          STRING(StrLowLevelMode);
                          memset(cmdInput, 0, sizeof(cmdInput));
                          mode = LOWLEVEL;
                          break;
            }
        }
        else if (COORDINATE == mode)
        {
            if (0x00 == data)
            {
                continue;
            }
            uart_send_buffered(data);
            switch (data)
            {
            case 0x1b: // escape, clear line
                memset(cmdInput, 0, sizeof(cmdInput));
                continue;

            case 0x0d:
                processCommand();
                memset(cmdInput, 0, sizeof(cmdInput));
                continue;

            default:
                cmdInput[strlen(cmdInput)] = data;
                continue;
            }
        }
        else if (LOWLEVEL == mode)
        {
            if (data)
            {
                lowLevelMode(data);
            }
        }
    }
}  

