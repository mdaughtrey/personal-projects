#include <avr/io.h>
#include <typedefs.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <cmdproc.h>
#include <displaymux.h>
#include <stdint.h>
#include <avr/wdt.h>
#include <spi.h>

u08 command = 0;
u08 parameter[4];
u08 paramIndex = 0;
u08 paramsRemaining;

u08 hexTable[16] PROGMEM = "0123456789abcdef";

extern u08 disploop;

void cmd_Init(void)
{
    command = 0;
    paramsRemaining = 0;
    paramIndex = 0;
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

void cmd_dataHandler(u08 input)
{
    if (command)
    {
        parameter[paramIndex++] = input;
        paramsRemaining--;
        if (paramsRemaining == 0)
        {
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
            
                /* Dim display - d<01> */
                /* Undim display - D<01> */
            case 'D':
                dm_setDim(which, (command  == 'd' ? 1 : 0)); 
                break;
            
                /* Flip display - f<01> */
                /* Unflip display - F<01> */
            case 'F':
                dm_setFlip(which, (command == 'f' ? 0 : 1)); 
                break;
            
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
            
                /* Copy character to custom character 0 - p<hex> */
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
            
                /* Enable light sense - z<row><column> */
                /* Disable light sense - Z<row><column>*/
                /* columns 0-4: display 0 */
                /* columns 5-9: display 1 */
            case 'Z':
                dm_setsense((command == 'z' ? 1 : 0),
                            parameter[1] - '0',
                            parameter[2] - '0');
                break;
            
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
    case 'd':
    case 'D':
    case 'f':
    case 'F':
    case 'i':
    case 'I':
    case 'm':
    case 'M':
    case 'R':
    case '!':
        command = input;
        paramsRemaining = 1;
        break;
        
    case 'p':
    case 'P':
    case 'l':
    case 'L':
        command = input;
        paramsRemaining = 2;
        break;
      
    case 'r':
    case 's':
    case 't':
    case 'T':
    case 'z':
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
        _wdt_write(1);
        break;
                                /* manual invocation of timer handler */
    case '/ ':
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
}
    
