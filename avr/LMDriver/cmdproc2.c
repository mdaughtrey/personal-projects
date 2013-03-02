#include <string.h>
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


void dumpInput(void)
{
    u08 ii;
    uart_send_buffered('i');

    for (ii = 0; ii < LAST; ii++)
    {
        uart_send_hex_byte(input[ii]);
        uart_send_buffered(' ');
    }
    uart_send_buffered('I');
    uart_send_buffered('\r');
    uart_send_buffered('\n');
}

void cmdInit(void)
{
    memset(input, 0, LAST - FIRST);
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

char charToNum(u08 input)
{
    //
    // Validate length parameter (0-9,a-z)
    //
    if (input >= '0' && input <= '9')
    {
        return input  - '0';
    }
    if (input >= 'a' && input <= 'z')
    {
        return input - 'a' + 10;
    }
    return -1;
}

char numToChar(char input)
{
    if (input >= 0 && input <= 9)
    {
        return input + '0';
    }
    if (input > 9 && input < 36)
    {
        return input - 10 + 'a';
    }
    return 0;
}

void cmdIndexed(u08 cmdInput)
{
    switch (state)
    {
        case INDEX:
            //
            // Validate index parameter (0-9,a-z)
            //
            if (-1 == (input[INDEX] = charToNum(cmdInput)))
            {
                cmdInit();
                return;
            }
            state =  PARAM1;
            break;

        case PARAM1:
            input[PARAM1] = cmdInput;
            state = PARAM2;
            break;

        case PARAM2:
            if (input[INDEX] > 1)
            {
                uart_send_buffered(input[COMMAND]);
                uart_send_buffered(numToChar(input[INDEX] - 2));
                uart_send_buffered(input[PARAM1]);
                uart_send_buffered(cmdInput);
            }
            else
            {
                switch (input[COMMAND])
                {
                    case 'x':
                    case 'X':
                        dm_pixel(input[INDEX],
                                (input[COMMAND] == 'x' ? 1 : 0),
                                input[PARAM1] - '0',
                                cmdInput - '0');
                        break;
                }
            }
            cmdInit();
            break;
    }
}

void cmdCountedLength(u08 cmdInput)
{
    char length;
    //
    // If we're expecting the length parameter...
    //
    if (LENGTH == state)
    {
        //
        // Validate length parameter (0-9,a-z)
        //
        if (-1 == (length = charToNum(cmdInput)))
        {
            cmdInit();
            return;
        }
        //
        // if length > 2 then subtract 2 and pass this command on to the
        // next controller in the chain
        //
        input[REMAINING] = length;
        length -= 2;
        if (length > 0)
        {
            uart_send_buffered(input[COMMAND]);
            uart_send_buffered(numToChar(length));
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
        switch (input[COMMAND])
        {
            case 't':
                dm_setChar(input[INDEX], cmdInput);
                break;

            case 'p':
                dm_setPalette(input[INDEX], cmdInput - '0');
                break;

            case 'L':
            case 'R':
            case 'D':
            case 'U':
                dm_shift(input[INDEX], input[COMMAND], cmdInput - '0');
                break;

            case 'l':
            case 'r':
            case 'd':
            case 'u':
                dm_roll(input[INDEX], input[COMMAND], cmdInput - '0');
                break;

            case '!':
                dm_reset(input[INDEX]);
                break;

            case 'c':
            case 'C':
                dm_displayProgrammed(input[INDEX], (input[COMMAND] == 'c' ? 1 : 0)); 
                break;
        }
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

///
/// \brief f = flip character
/// \details { flip character vertically }
/// \example fX
/// \param X - { character position (1-9, a=10, b=11...)}
///
///
/// \brief F = unflip character
/// \details { unflip character vertically }
/// \example FX
/// \param X - { character position (1-9, a=10, b=11...)}
///
void cmdFlip(u08 cmdInput)
{
    char position = charToNum(cmdInput);
    if (0 == position || 1 == position)
    {
        dm_setFlip(position, (input[COMMAND] == 'F' ? 0 : 1)); 
    }
    else
    {
        uart_send_buffered(input[COMMAND]);
        uart_send_buffered(numToChar(position - 2));
    }
    cmdInit();
}

///
/// \brief m = mirror character
/// \details { mirror character horizontally }
/// \example mX
/// \param X - { character position (0-9, a=10, b=11...)}
///
///
/// \brief M = unmirror character
/// \details { unmirror character horizontally }
/// \example MX
/// \param X - { character position (0-9, a=10, b=11...)}
///
void cmdMirror(u08 cmdInput)
{
    char position = charToNum(cmdInput);
    if (position < 2)
    {
        dm_setMirror(position, (input[COMMAND] == 'M' ? 0 : 1)); 
    }
    else
    {
        uart_send_buffered(input[COMMAND]);
        uart_send_buffered(numToChar(position - 2));
    }
    cmdInit();
}

///
/// \brief i = invert character pixels
/// \details { flip each pixel state (on to off, off to on) }
/// \example iX
/// \param X - { character position (0-9, a=10, b=11...)}
///
/// \brief I = uninvert character pixels
/// \details { revert each pixel state to normal }
/// \example IX
/// \param X - { character position (0-9, a=10, b=11...)}
///
void cmdInvert(u08 cmdInput)
{
    char position = charToNum(cmdInput);
    if (position < 2)
    {
        dm_setReverse(position, (input[COMMAND] == 'I' ? 0 : 1)); 
    }
    else
    {
        uart_send_buffered(input[COMMAND]);
        uart_send_buffered(numToChar(position - 2));
    }
    cmdInit();
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
            case 'p':
            case 'l':
            case 'r':
            case 'u':
            case 'd':
            case 'L':
            case 'R':
            case 'U':
            case 'D':
                state = LENGTH;
                break;

            case 'f':
            case 'i':
            case 'm':
                state = POSITION;
                break;

            case 'x':
            case 'X':
                state = INDEX;
                break;

            default:
                cmdInit();
    //            state = POSITION;
                break;
        }
        return;
    }
    switch (input[COMMAND])
    {
        case '*':
            wdt_enable(1);
            break;

///
/// \brief R = shift characters right
/// \details {shift characters right.} 
/// \example Rdnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - number of pixels to shift character
///
///
/// \brief L = shift characters left
/// \details {shift characters left.}
/// \example Ldnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - number of pixels to shift character
///
///
/// \brief U = shift characters up
/// \details {shift characters up.}
/// \example Udnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - number of pixels to shift character
///
///
/// \brief D = shift characters down
/// \details {shift characters down.}
/// \example Ddnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - number of pixels to shift character
///
        case 'L':
        case 'R':
        case 'U':
        case 'D':
///
/// \brief r = roll characters right
/// \details {roll characters right. The pixels on the right will wrap around to the left side.}
/// \example rdnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - number of pixels to roll character
///
///
/// \brief l = roll characters left
/// \details {roll characters left. The pixels on the left will wrap around to the right side.}
/// \example ldnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - number of pixels to roll character
///
///
/// \brief u = roll characters up
/// \details {roll characters up. The pixels on the top will wrap around to the bottom.}
/// \example udnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - number of pixels to roll character
///
///
/// \brief d = roll characters down
/// \details {roll characters down.The pixels on the bottom will wrap around to the top.}
/// \example ddnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - number of pixels to roll character
///
        case 'l':
        case 'r':
        case 'u':
        case 'd':

///
/// \brief t = Set Text
/// \details { Set all text characters in one command }
/// \example tXcccc
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param c - character
///
        case 't': // Set Text
///
/// \brief p = set palette
/// \details { set the palette 
/// \example pX
/// \param X - { character position (0-9, a=10, b=11...)}
///
        case 'p': // Set Palette
///
/// \brief ! = Reset Transforms
/// \details { Reset characters transforms}
/// \example !Xnnnnnnnn
/// \param X - { length (0-9, a=10, b=11...)}
/// \param n - 0 or 1
///
        case '!': // Reset Transforms
///
/// \brief c = Enable custom character
/// \details {Enable custom character}
/// \example cdnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - number of pixels to roll character
///
        case 'c':
///
/// \brief C = Disable custom character
/// \details {Disable custom character}
/// \example Cdnnnn
/// \param X - {number of characters (1-9, a=10, b=11...)}
/// \param n - number of pixels to roll character
///
        case 'C':
            cmdCountedLength(cmdInput);
            break;
///
/// \brief x = Turn on pixel
/// \details {Turn on a single pixel}
/// \example xIrc
/// \param I - character index
/// \param r = row (0-6)
/// \param c = column (0-4)
///
        case 'x':
///
/// \brief X = Turn off pixel
/// \details {Turn off a single pixel}
/// \example XIrc
/// \param I - character index
/// \param r = row (0-6)
/// \param c = column (0-4)
///
        case 'X':
            cmdIndexed(cmdInput);
            break;

        case 'f': // Flip
            cmdFlip(cmdInput);
            break;

        case 'i': // Invert
            cmdInvert(cmdInput);
            break;

        case 'm': // Mirror
            cmdMirror(cmdInput);
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
    
