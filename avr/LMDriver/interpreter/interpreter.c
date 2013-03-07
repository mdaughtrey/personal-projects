#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

//!c - declare const a-zA-Z
#define CMD_PAUSE 'p'
//!e - emit constant
#define CMD_EMIT_CONSTANT 'e'
//!E - emit variable
#define CMD_EMIT_VARIABLE 'E'
//!m - mark a-zA-Z
//!j - jump <mark>
#define CMD_JUMP 'j'
//!d - decrement var
//!i - increment var
//!v - declare short int variable a-z
#define CMD_DECLARE_SHORT_VAR 'i'
//!V - declare string variable A-Z
#define CMD_DECLARE_STRING_VAR 's'
//!z - jump if var is zero
//!d - delay ms
//!f - FOR iter step
//!F - end FOR

#define CMD_END '!'


#define RAM_SIZE     64
#define CONST_OFFSET 2
int outDev;
unsigned short ip;

const char eeprom[] = "\x00\x0a"    // Program start offset big endian
// ---------- Constants
    "a\x06t\x04""abcd"         // 
// ------- Instructions
    "sA\x06t\x04""matt"                 // declare string variable A matt
    "EA"                            // emit variable A
    "p\x04\00"                      // pause 1024ms
    "ea"                            // emit constant
    "p\x04\00"                      // pause 1024ms
    "j\x00\x13"                   // jump to 0x000a
//    "ia\xef\xfe"                // declare integer variable a 0xeffe
//    "Ea"                        // emit variable a
//    "ea"                       // emit constant a
    "!";                       // end

char ram[RAM_SIZE] = {0};
unsigned short ramUsed = 0;

void emitConst(char constName)
{
    unsigned short ii = CONST_OFFSET;
    while (constName != eeprom[ii])
    {
        ii++;               // point to const length
        ii += eeprom[ii];   // skip the const
    }
    // this is the correct var name
    ii++;
    unsigned char count = eeprom[ii++];
    while (count--)
    {
//        write(outDev, &eeprom[ii], 1);
//        write(1, &eeprom[ii], 1);
        printf("%02x/%c ", eeprom[ii] & 0xff, eeprom[ii] & 0xff);
        ii++;
    }
}

void emitVariable(void)
{
    unsigned short ii = 0;
    while (ii < ramUsed)
    {
        if (ram[ii] == eeprom[ip])
        { 
            unsigned char jj;
            ii++;               // ii points to var length
            jj = ii + ram[ii];  // jj points to end of var value
            ii++;               // ii points to start of var value
            while (ii <= jj)
            {
        //        write(outDev, &ram[ii], 1);
        printf("%02x/%c ", ram[ii] & 0xff, ram[ii] & 0xff);
                ii++;
            }
     //       ip = jj + 1;
            ip++;
            return;
        }
        ii++;
        ii += eeprom[ii];
    }
    printf("Didnt find variable %c in RAM\n", eeprom[ip]);
}

void declareShort(void)
{
    unsigned short ii = 0;
    while (ii < ramUsed)
    {
        if (ram[ii] == eeprom[ip])
        { 
            printf("Short var %c already declared at RAM %08x\n", ram[ii], ram[ii]);
            return;
        } 
        ii++;
        ii+= ram[ii];
    }
    if (ramUsed + 4 > RAM_SIZE)
    {
        printf("Not enough RAM left to declare short var %c: RAM_SIZE %d ramUsed %d\n", 
                eeprom[ii], RAM_SIZE, ramUsed);
        return;
    }
    ram[ii++] = eeprom[ip++]; // store name
    ram[ii++] = 2;           // length = 2 bytes
    ram[ii++] = eeprom[ip++]; // store MSB
    ram[ii++] = eeprom[ip++]; // store LSB
    ramUsed = ii;
}

void declareString(void)
{
    unsigned short ii = 0;
    while (ii < ramUsed)
    {
        if (ram[ii] == eeprom[ip])
        { 
            printf("String var %c already declared at RAM %08x\n", ram[ii], ram[ii]);
            return;
        } 
        ii++; // length
        ii+= ram[ii];
    }
    if (ramUsed + eeprom[ip + 1] > RAM_SIZE)
    {
        printf("Not enough RAM left to declare string var %c: RAM_SIZE %d ramUsed %d\n", 
                eeprom[ii], RAM_SIZE, ramUsed);
        return;
    }
    ram[ii++] = eeprom[ip++]; // store name
    unsigned char jj =  ip + eeprom[ip];
    ram[ii++] = eeprom[ip++]; // store length
    while (ip <= jj)
    {
        ram[ii++] = eeprom[ip++]; // store MSB
    }
    ramUsed = ii;
}

void doPause(void)
{
    unsigned short value = (eeprom[ip] << 8) | eeprom[ip + 1];
    usleep(value * 1000);
    ip += 2;
}

void jump(void)
{
    ip = (eeprom[ip] << 8) | eeprom[ip + 1];
}

void interpreter(void)
{
    ip = (eeprom[0] << 8) | eeprom[1];
    while (1)
    {
        printf("\n%04x: %02x %c\n", ip, eeprom[ip], eeprom[ip]);
        switch (eeprom[ip])
        {
            case CMD_EMIT_CONSTANT:
                ip++;
                emitConst(eeprom[ip]);
                ip++;
                break;

            case CMD_EMIT_VARIABLE:
                ip++;
                emitVariable();
                break;

            case CMD_DECLARE_SHORT_VAR:
                ip++;
                declareShort();
                break;

            case CMD_DECLARE_STRING_VAR:
                ip++;
                declareString();
                break;

            case CMD_PAUSE:
                ip++;
                doPause();
                break;

            case CMD_JUMP:
                ip++;
                jump();
                break;

            case CMD_END:
                printf("End");
                return;

            default:
                printf("Unknown command %c ip %04x", eeprom[ip], ip);
                return;
        }
    }
}

int main(int argc, char ** argv)
{
    struct termios termio;
    outDev = open("/dev/cu.SLAB_USBtoUART", O_WRONLY | O_NOCTTY | O_NONBLOCK);
    cfmakeraw(&termio);
    cfsetospeed(&termio, B38400);
    tcsetattr(outDev, TCSANOW, &termio);

    interpreter();

//    emitConst('a');
    close(outDev);

}
