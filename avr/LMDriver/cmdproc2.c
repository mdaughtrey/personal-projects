#include <string.h>
#include <avr/io.h>
#include <typedefs.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <cmdproc.h>
#include <displaymux.h>
#include <stdint.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <spi.h>
#include <serutil.h>
#include <cmdproc2.h>
#include <vminterface.h>

u08 hexTable[16] PROGMEM = "0123456789abcdef";

extern u08 disploop;
extern u08 node;
extern unsigned char programControl;
extern unsigned char memory[RAM_SIZE];
//extern unsigned char stop;
//extern void mosiPush(unsigned char data);

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
    UPLOAD_LENGTH_MSB,
    UPLOAD_LENGTH_LSB,
    UPLOAD_MAINOFFSET_MSB,
    UPLOAD_MAINOFFSET_LSB,
    UPLOAD_BINOFFSET_MSB,
    UPLOAD_BINOFFSET_LSB,
    UPLOAD_CODE,
    LAST
} State;

u08 cpState;
u08 input[LAST];
u16 uploadRemaining;
u16 uploadVar;

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
    cpState = FIRST;
    programControl = eeprom_read_byte ((uint8_t*)EEPROM_OFFSET_PROGRAM_CONTROL);

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
    switch (cpState)
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
            cpState =  PARAM1;
            break;

        case PARAM1:
            input[PARAM1] = cmdInput;
            cpState = PARAM2;
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
    if (LENGTH == cpState)
    {
        //
        // Validate length parameter (0-9,a-z)
        //
//        switch (input[COMMAND])
//        {
//            case CMD_SET_TEXT:
                length = cmdInput;
//                break;
//
//            default:
//                if (-1 == (length = charToNum(cmdInput)))
//                {
//                    cmdInit();
//                    return;
//                }
//                break;
//        }
        //
        // if length > 2 then subtract 2 and pass this command on to the
        // next controller in the chain
        //
        input[REMAINING] = length;
        length -= 2;
        if (length > 0)
        {
            uart_send_buffered(input[COMMAND]);
            //uart_send_buffered(numToChar(length));
            uart_send_buffered(length);
        }
        input[INDEX] = 0;
        cpState = PARAM1;
        return;
    }
    //
    // We are accepting parameters, maybe for us, maybe to be passed along
    //
    if (0 == input[INDEX] || 1 == input[INDEX])
    {
        switch (input[COMMAND])
        {
            case CMD_SET_TEXT:
                dm_setChar(input[INDEX], cmdInput);
                break;

            case CMD_SET_PALETTE:
                dm_setPalette(input[INDEX], cmdInput - '0');
                break;

            case CMD_SHIFT_LEFT:
            case CMD_SHIFT_RIGHT:
            case CMD_SHIFT_DOWN:
            case CMD_SHIFT_UP:
                dm_shift(input[INDEX], input[COMMAND], cmdInput - '0');
                break;

            case CMD_ROLL_LEFT:
            case CMD_ROLL_RIGHT:
            case CMD_ROLL_DOWN:
            case CMD_ROLL_UP:
                dm_roll(input[INDEX], input[COMMAND], cmdInput - '0');
                break;

            case CMD_RESET_TRANSFORMS:
                dm_reset(input[INDEX]);
                break;

            case CMD_CUSTOM_CHARACTER:
                dm_displayProgrammed(input[INDEX], cmdInput - '0');
                break;

            case CMD_COPY_CUSTOM:
                dm_copyToCustom(input[INDEX], cmdInput);
                break;

            case CMD_FLIP:
                dm_setFlip(input[INDEX], cmdInput - '0'); 
                break;

            case CMD_INVERT:
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
    if (LENGTH == cpState)
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
        cpState = PARAM1;
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

void dumpEeprom(void)
{
    unsigned short addr = 0;
    for (addr = 0; addr < ROM_SIZE; addr++)
    {
        if (addr % 32 == 0)
        {
            uart_send_buffered('\r');
            uart_send_buffered('\n');
            uart_send_hex_byte(addr >> 8);
            uart_send_hex_byte(addr & 0xff);
            uart_send_buffered(':');
        }
        uart_send_hex_byte(eeprom_read_byte((uint8_t *)addr));
        uart_send_buffered(' ');
    }

    for (addr = 0; addr < RAM_SIZE; addr++)
    {
        if (addr % 32 == 0)
        {
            uart_send_buffered('\r');
            uart_send_buffered('\n');
            uart_send_hex_byte(addr >> 8);
            uart_send_hex_byte(addr & 0xff);
            uart_send_buffered(':');
        }
        uart_send_hex_byte(memory[addr]);
        uart_send_buffered(' ');
    }
}


void cmd_dataHandler(u08 cmdInput)
{
    u08 ii;

//#ifdef EMBEDDED_DEBUG   
//    uart_send_hex_byte(cmdInput);
//    uart_send_buffered('S');
//    uart_send_hex_byte(cpState);
//    uart_send_buffered('P');
//    uart_send_hex_byte(programControl);
//    uart_send_buffered('\r');
//    uart_send_buffered('\n');
//#endif // EMBEDDED_DEBUG

    if (COMMAND == cpState)
    {
        input[COMMAND] = cmdInput;
        cpState = PARAM1;
        switch (cmdInput)
        {
            case CMD_DUMP_EEPROM:
                dumpEeprom();
                cpState = COMMAND;
                break;

            case CMD_SET_TEXT:
            case CMD_SET_PALETTE:
            case CMD_ROLL_LEFT:
            case CMD_ROLL_RIGHT:
            case CMD_ROLL_UP:
            case CMD_ROLL_DOWN:
            case CMD_SHIFT_RIGHT:
            case CMD_SHIFT_LEFT:
            case CMD_SHIFT_UP:
            case CMD_SHIFT_DOWN:
            case CMD_CUSTOM_CHARACTER:
            case CMD_COPY_CUSTOM:
            case CMD_BITMAP:
            case CMD_FLIP:
            case CMD_INVERT:
            case CMD_MIRROR:
                cpState = LENGTH;
                break;

            case CMD_PIXEL_ON:
            case CMD_PIXEL_OFF:
                cpState = INDEX;
                break;

            case CMD_UPLOAD:
                programControl = 0;
                eeprom_write_byte((uint8_t*)EEPROM_OFFSET_PROGRAM_CONTROL, 0);
                cpState = UPLOAD_LENGTH_MSB;
                break;

            case CMD_RUN_PROGRAM:
//                state = PARAM1;
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
        case CMD_SHIFT_LEFT:
        case CMD_SHIFT_UP:
        case CMD_SHIFT_DOWN:
        case CMD_ROLL_LEFT:
        case CMD_ROLL_RIGHT:
        case CMD_ROLL_UP:
        case CMD_ROLL_DOWN:
        case CMD_SET_TEXT:
        case CMD_SET_PALETTE:
        case CMD_RESET_TRANSFORMS:
        case CMD_CUSTOM_CHARACTER:
        case CMD_COPY_CUSTOM:
            cmdCountedLength(cmdInput);
            break;

        case CMD_BITMAP:
            cmdSetBits(cmdInput);
            break;

        case CMD_PIXEL_ON:
        case CMD_PIXEL_OFF:
            cmdIndexed(cmdInput);
            break;

        case CMD_RUN_PROGRAM:
            programControl = cmdInput - '0';
            eeprom_write_byte ((uint8_t*)EEPROM_OFFSET_PROGRAM_CONTROL, programControl);
            cpState = COMMAND;
            break;

        case CMD_UPLOAD:
            switch (cpState)
            {
                case UPLOAD_LENGTH_MSB:
//#ifdef EMBEDDED_DEBUG
//                    uart_send_buffered('M');
//                    uart_send_hex_byte(cmdInput);
//#endif // EMBEDDED_DEBUG
                    uploadRemaining = cmdInput << 8;
                    cpState = UPLOAD_LENGTH_LSB;
                    break;

                case UPLOAD_LENGTH_LSB:
//#ifdef EMBEDDED_DEBUG
//                    uart_send_buffered('L');
//                    uart_send_hex_byte(cmdInput);
//#endif // EMBEDDED_DEBUG
                    uploadRemaining |= cmdInput;
                    cpState = UPLOAD_MAINOFFSET_MSB;
                    break;

                case UPLOAD_MAINOFFSET_MSB:
                    uploadVar = cmdInput << 8;
                    cpState = UPLOAD_MAINOFFSET_LSB;
                    break;

                case UPLOAD_MAINOFFSET_LSB:
                    uploadVar |= cmdInput;
                    cpState = UPLOAD_BINOFFSET_MSB;
                    eeprom_write_byte((uint8_t*)EEPROM_OFFSET_MAINOFFSET_MSB, uploadVar >> 8);
                    eeprom_write_byte((uint8_t*)EEPROM_OFFSET_MAINOFFSET_LSB, uploadVar & 0xff);
                    break;

                case UPLOAD_BINOFFSET_MSB:
                    uploadVar = cmdInput << 8;
                    cpState = UPLOAD_BINOFFSET_LSB;
                    break;

                case UPLOAD_BINOFFSET_LSB:
                    uploadVar |= cmdInput;
                    cpState = UPLOAD_CODE;
                    eeprom_write_byte((uint8_t*)EEPROM_OFFSET_BINOFFSET_MSB, uploadVar >> 8);
                    eeprom_write_byte((uint8_t*)EEPROM_OFFSET_BINOFFSET_LSB, uploadVar & 0xff);
                    uploadVar = 0;
                    break;

                case UPLOAD_CODE:
//#ifdef EMBEDDED_DEBUG
//                    uart_send_hex_byte(cmdInput);
//                    uart_send_buffered('@');
//                    uart_send_hex_byte((int)eepromAddress >> 8);
//                    uart_send_hex_byte((int)eepromAddress & 0xff);
//                    uart_send_buffered('\r');
//                    uart_send_buffered('\n');
//#endif // EMBEDDED_DEBUG

                    eeprom_write_byte ((uint8_t*)uploadVar++, cmdInput);
                    uploadRemaining--;
                    if (0 == uploadRemaining)
                    {
                        eeprom_write_byte((uint8_t*)EEPROM_OFFSET_PROGRAM_CONTROL, 0);
                        cpState = COMMAND;
                    }
                    break;
            }
            break;
    }
}
