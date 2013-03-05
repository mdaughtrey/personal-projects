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
#include <cmdproc2.h>

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
                    case CMD_PIXEL_ON:
                    case CMD_PIXEL_OFF:
                        dm_pixel(input[INDEX],
                                (input[COMMAND] == CMD_PIXEL_ON ? 1 : 0),
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
    char indexOffset;
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

            case CMD_SHIFT_LEFT:
            case CMD_SHIFT_RIGHT:
            case CMD_SHIFT_DOWN:
            case CMD_SHIFT_UP:
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

            case 's':
                dm_copyToCustom(input[INDEX], cmdInput);
                break;

            case CMD_FLIP:
                dm_setFlip(input[INDEX], cmdInput - '0'); 
                break;

            case 'i':
                dm_setReverse(input[INDEX], cmdInput - '0'); 
                break;

            case CMD_MIRROR:
                dm_setMirror(input[INDEX], cmdInput - '0'); 
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

void cmdSetBits(u08 cmdInput)
{
    char length;
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
        input[REMAINING] = length * 5;
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
    if (input[INDEX] < 10)
    {
        dm_progColumn(input[INDEX] / 5, input[INDEX] % 5, cmdInput);
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
            case 'p':
            case 'l':
            case 'r':
            case 'u':
            case 'd':
            case CMD_SHIFT_RIGHT:
            case CMD_SHIFT_LEFT:
            case CMD_SHIFT_UP:
            case CMD_SHIFT_DOWN:
            case 'c':
            case 'C':
            case 's':
            case 'b':
            case CMD_FLIP:
            case CMD_INVERT:
            case CMD_MIRROR:
                state = LENGTH;
                break;

                state = POSITION;
                break;

            case CMD_PIXEL_ON:
            case CMD_PIXEL_OFF:
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

        case CMD_SHIFT_RIGHT:
        case 'L':
        case 'U':
        case 'D':
        case 'l':
        case 'r':
        case 'u':
        case 'd':

        case 't': // Set Text
        case 'p': // Set Palette
        case '!': // Reset Transforms
        case 'c':
        case 'C':

        case 's':

            ///
            /// \brief b = set/reset individual bits
            /// \details {bits}
            /// \example sXccccddddeeee
            /// \param X - {number of characters (1-9, a=10, b=11...)}
            /// \param ccccc - bits for columns 0-4, character 0
            /// \param ddddd - bits for columns 0-4, character 1
            ///

            cmdCountedLength(cmdInput);
            break;

        case 'b':
            cmdSetBits(cmdInput);
            break;

        case CMD_PIXEL_ON:
        case CMD_PIXEL_OFF:
            cmdIndexed(cmdInput);
            break;
    }
}
