#include <avr/io.h>
#include <typedefs.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <cmdproc.h>
#include <displaymux.h>
#include <stdint.h>
#include <avr/wdt.h>
#include <spi.h>
#include <serutil.h>

//u08 position = 0;
//u08 command = 0;
//u08 parameter[4];
//u08 paramIndex = 0;
//u08 paramsRemaining;

u08 hexTable[16] PROGMEM = "0123456789abcdef";

extern u08 disploop;
extern u08 node;

typedef enum
{
    FIRST = 0,
    COMMAND = FIRST,
    POSITION, 
    LENGTH = POSITION,  
    PARAM1,
    PARAM2,
    PARAM3,
    INDEX = PARAM3,
    PARAM4,
    REMAINING = PARAM4,
    LAST
} State;

u08 state;
u08 input[LAST];

void cmdInit(void)
{
    for (state = FIRST; state < LAST; state++)
    {
        input[state] = 0;
    }
    state = FIRST;
}

u08 asciiToHex(u08 ascii1, u08 ascii2)
{
    u08 result = 0;
    u08 index;
    
    for (index = 0; index < 16; index++)
    {
        if (ascii1 == pgm_read_byte_near(hexTable + index))
        {
            result = index;
            break;
        }
    }
    result <<= 4;
    for (index = 0; index < 16; index++)
    {
        if (ascii2 == pgm_read_byte_near(hexTable + index))
        {
            result |= index;
            break;
        }
    }
    return result;
    
}

///
/// \brief t = Set Text
/// \details { Set all text characters in one command }
/// \example tXcccc
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param c - character
///
void cmdSetText(u08 cmdInput)
{
    u08 length;
    //
    // If we're expecting the length parameter...
    //
    if (LENGTH == state)
    {
        //
        // Validate length parameter (0-9,a-z)
        //
        if (cmdInput >= '0' && cmdInput <= '9')
        {
            length = cmdInput  - '0';
        }
        else if (cmdInput >= 'a' && cmdInput <= 'z')
        {
            length = cmdInput - 'a' + 10;
        }
        else
        {
            cmdInit();
            return;
        }
        //
        // if length > 2 then subtract 2 and pass this command on to the
        // next controller in the chain
        //
        input[REMAINING] = length;
        if (length > 2)
        {
            uart_send_buffered('t');
            if (cmdInput >= '0' && cmdInput <= '9')
            {
                cmdInput -= 2;
            }
            else if (cmdInput >= 'a' && cmdInput <= 'z')
            {
                cmdInput -= 2;
                if (0x60 == cmdInput)
                {
                    cmdInput = '9';
                }
                if (0x5f == cmdInput)
                {
                    cmdInput = '8';
                }
            }
            uart_send_buffered(cmdInput);
        }
        input[INDEX] = 0;
        state = PARAM1;
        return;
    }
    //
    // We are accepting parameters, maybe for us, maybe to be passed along
    //
    if (0 == input[INDEX] || 1 == input[INDEX])
    {
        dm_setChar(input[INDEX], cmdInput);
    }
    else
    {
        uart_send_buffered(cmdInput);
    }
    input[INDEX]++;
    input[REMAINING]--;
    if (0 == input[REMAINING])
    {
        cmdInit();
    }
}

void cmd_dataHandler(u08 cmdInput)
{
    u08 ii;
    if (COMMAND == state)
    {
        input[COMMAND] = cmdInput;
        state = PARAM1;
        switch (cmdInput)
        {
            case 't':
                state = LENGTH;
                break;

            default:
                cmdInit();
                state = POSITION;
                break;
        }
        return;
    }
    switch (input[COMMAND])
    {
        case 't': // Set Text
            cmdSetText(cmdInput);
            break;
    }
//    if (POSITION == state)
//    {
//        if (input >= '0' && input <= '9')
//        {
//            command[POSITION] = input;
//            state = COMMAND;
//        }
//        return;
//    }

#if 0
    if (command[state] != 0xff)
    {
        command[state++] = input;
        return;
    }

    // we have a complete command
    command[state] = input;
    
    switch(command[COMMAND])
    {
        case 'c': // Set Character <pos>C<char>
            dm_setChar(command[POSITION] == '0' ? 0 : 1, command[PARAM1]);
            break;
    }

    for (ii = 0; ii < LAST; ii++)
    {
        command[ii] = 0;
    }
    state = POSITION;


        parameter[paramIndex++] = input;
        paramsRemaining--;
        if (paramsRemaining)
        {
            return;
        }
        u08 which = (parameter[0] == '0' ? 0 : 1);
        switch (command & 0xdf)
        {
            /* Set display 0 character - a<char> */
            /* Set display 1 character - A<char> */
        case 'A':
            dm_setChar((command == 'a' ? 0 : 1), parameter[0]); 
            break;

            /* Blank character - b<01> */
            /* Unblank character - b<01> */
        case 'B':
            dm_blank(which, (command == 'b' ? 1: 0));
            break;
            
            /* Enable Custom character - c<01>  */
            /* Disable Custom character - C<01> */
        case 'C':
            dm_displayProgrammed(which, (command == 'c' ? 1 : 0)); 
            break;
            
            /* Flip display - f<01> */
            /* Unflip display - F<01> */
        case 'F':
            dm_setFlip(which, (command == 'f' ? 0 : 1)); 
            break;
        
            /* General Purpose Output: g<0-F> */
            /* Returns General Purpose Input via SPI */
//        case 'G':
//            which = asciiToHex(0, parameter[0]) << 3;
//            unsigned char data = PORTD;
//            data &= (0x87 | which);
//            data |= which;
//            PORTD = data;
//            which = PINE & 0xc3;
////            dm_setChar(0, pgm_read_byte_near(hexTable + ((which & 0xf0 )>> 4))); 
// //           dm_setChar(1, pgm_read_byte_near(hexTable + (which & 0x0f))); 
//            miso_push(pgm_read_byte_near(hexTable + ((which & 0x03) | (which >> 4))));
//            break;
            
            /* Invert display bits - i<01> */
            /* Uninvert display bits - I<01> */
        case 'I':
            dm_setReverse(which, (command == 'i' ? 1 : 0)); 
            break;
            
            /* Set Pallete Index for display 0- l<HH> */
            /* This is ignored for single-color displays */
            /* Set Pallete Index for display 1- L<HH> */
            /* This is ignored for single-color displays */
        case 'L':
            dm_setPalette((command == 'l' ? 0 : 1), 
                          asciiToHex(parameter[0], parameter[1]));
            break;

            /* Mirror display - m<01> */
            /* Unmirror display M<01> */
        case 'M':
            dm_setMirror(which, (command == 'm' ? 1 : 0)); 
            break;

        case 'N': /* Node setting - nX - you are node X */
            main_setNode(parameter[0] - '0');
            break;

            /* Copy character to custom character 0 - p<hex> */
            /* Copy character to custom character 1 - P<hex> */
        case 'P':
            dm_copyToCustom((command == 'p' ? 0 : 1), asciiToHex(parameter[0], parameter[1])); 
            break;
            
            /* Roll matrix n row<s> - r<01><udlr><n> */
        case 'R': dm_roll(which,
                          parameter[1],
                          parameter[2] - '0');
            break;
            
            /* Shift matrix <n> rows  - s<01><udlr><n> */
        case 'S': dm_shift(which,
                           parameter[1],
                           parameter[2] - '0');
            break;
            
            /* Turn on pixel in custom character - t<01><row><column> */
            /* Turn off pixel in custom character - T<01><row><column> */
        case 'T':
            dm_pixel(which, (command == 't' ? 1 : 0), parameter[1] - '0', parameter[2] - '0');
            break;
            
            // Z<#><# of characters>
        case 'z':
            break;

            /* Enable light sense - z<row><column> */
            /* Disable light sense - Z<row><column>*/
            /* columns 0-4: display 0 */
            /* columns 5-9: display 1 */
//        case 'Z':
//            dm_setsense('Z' == command ? 'X':  parameter[0] - '0',
//                        'Z' == command ? 'X':  parameter[1] - '0');
//            break;
            
        default:
            /* Reset all transforms - !<01> */
            if ('!' == command)
            {
                dm_reset(which); 
            }
            /* Program custom character columns, display 0 - [01234]<HexHex> */
            else if (command >= '0' && command <= '4')
            {
                dm_progColumn(0, command - '0', asciiToHex(parameter[0], parameter[1])); 
            }
            /* Program custom character columns, display 1 - [56789]<data> */
            else if (command >= '5' && command <= '9')
            {
                dm_progColumn(1, command -'5', asciiToHex(parameter[0], parameter[1])); 
            }
        }
        command = 0;
        paramIndex = 0;
        return;
    }
    
    paramsRemaining = 0;
    paramIndex = 0;
    switch (input)
    {
    case 'a':
    case 'A':
    case 'b':
    case 'B':
    case 'c':
    case 'C':
    case 'f':
    case 'F':
    case 'G':
    case 'g':
    case 'i':
    case 'I':
    case 'm':
    case 'M':
    case 'n':
    case 'N':
    case 'R':
    case '!':
        command = input;
        paramsRemaining = 1;
        break;
        
    case 'p':
    case 'P':
    case 'l':
    case 'L':
//    case 'z':
//    case 'Z':
        command = input;
        paramsRemaining = 2;
        break;
      
    case 'r':
    case 's':
    case 't':
    case 'T':
        command = input;
        paramsRemaining = 3;
        break;
        
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        command = input;
        paramsRemaining = 2;
        break;
        
    case '*': 
        //_wdt_write(1);
        wdt_enable(1);
        break;
                                /* manual invocation of timer handler */
    case '/':
        dm_timerHandler();
        command = 0;
        break;
    case '+':                   /* toggle auto timer handler */
        disploop = !disploop;
        break;
        
    default:
        command = 0;
        break;
    }
#endif
}
    
