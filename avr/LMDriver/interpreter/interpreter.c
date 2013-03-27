#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

//#define EMBEDDED
//#define OBJOUT
#define DEBUGOUT 0
//#define DEBUGOUT
#ifdef EMBEDDED
#undef DEBUGOUT
#endif

//!p pause in ms
#define CMD_PAUSE_MS 'p'
//!P pause in secs
#define CMD_PAUSE_S 'P'
//!e - emit constant
#define CMD_EMIT_CONSTANT 'e'
//!E - emit variable
#define CMD_EMIT_VARIABLE 'E'
//!I - emit integer
#define CMD_EMIT_INTEGER 'I'
//!j - jump <mark>
#define CMD_JUMP 'j'
//!d - decrement var
#define CMD_DECREMENT '-'
//!i - increment var
#define CMD_INCREMENT '+'
//!v - declare short int variable a-z
#define CMD_DECLARE_SHORT_VAR 'i'
//!V - declare string variable A-Z
#define CMD_DECLARE_STRING_VAR 's'
//!z - jump if integer is zero
#define CMD_JZ 'z'

#define CMD_END '!'
#define RAM_SIZE 64
#define CONST_OFFSET 2

int outDev;
unsigned short ip;
unsigned char * eeprom;

//const char eeprom[] = "\x00\x07"    // Program start offset big endian
//// ---------- Constants
//    "a\x03t\x02""a"         // 
//// ------- Instructions
//    "sA\x06t\x04""matt"        // 0007:  declare string variable A matt
//    "sB\x01""a"                // 0010: 
//    "ia\x00\x02"               // 0014: declare integer variable a
//    "Ea"                       // 0018:
//    "-a"                       // 001a: dec a
//    "za\x00\x23"               // 001c: jump if a == 0
//    "j\x00\x18"                // 0020: jump to 0018
//    "Ea"                       // 0023
//    "!";                       // end

unsigned char ram[RAM_SIZE] = {0};
unsigned short ramUsed = 0;

short locateVariable(unsigned char name)
{
    short ii = 0;
    while (ii < ramUsed)
    {
        if (ram[ii] == name)
        { 
            return ii;
        }
        ii++;            // ii points to var length
        ii += ram[ii] + 1;   // skip var value
    }
    return ii;
}

void emitConst(char constName)
{
    unsigned short ii = CONST_OFFSET;
    unsigned short constEnd = (eeprom[0] << 8) | eeprom[1];

#ifdef DEBUGOUT
    printf("emitConst %u: ", eeprom[ip] & 0xff);
#endif

    while (ii < constEnd)
    {
        if (constName == eeprom[ii])
        {
            unsigned char count;
            ii++;               // point to const length
            count = eeprom[ii++];
#ifdef DEBUGOUT
            printf(" length %02x ", count);
#endif
            while (--count)
            {
#ifdef DEBUGOUT
        printf(" %02x", eeprom[ii]);
#endif
                write(outDev, &eeprom[ii], 1);
                usleep(1000);
                ii++;
     //           return;
            }
        }
        else
        {
            ii++;               // point to length
            ii += eeprom[ii];   // skip constant
        }
    }
#ifdef DEBUGOUT
    printf("\n");
#endif
    ip++;
}

void emitInteger(void)
{
}

void emitVariable(void)
{
    unsigned short ii = locateVariable(eeprom[ip]);
    unsigned char jj;
#ifdef DEBUGOUT
    printf("emitVariable %u: ", eeprom[ip] & 0xff);
#endif

    if (ram[ii] != eeprom[ip])
    {
#ifdef DEBUGOUT
        printf("Variable %c not found\n", eeprom[ip]);
#endif
        return;
    }
    ii++; // length
    jj = ii + ram[ii];

    ii++; // start of data
    while (ii < jj)
    {
#ifdef DEBUGOUT
        printf(" %02x", ram[ii]);
#endif
        write(outDev, &ram[ii], 1);
                usleep(1000);
        ii++;
    }
    ip++;
#ifdef DEBUGOUT
    printf("\n");
#endif
}

void declareShort(void)
{
    unsigned short ii = locateVariable(eeprom[ip]);
    if (ramUsed + 4 > RAM_SIZE)
    {
#ifdef DEBUGOUT
        printf("Not enough RAM left to declare short var %c: RAM_SIZE %d ramUsed %d\n", 
                eeprom[ii], RAM_SIZE, ramUsed);
#endif
        return;
    }
    ram[ii++] = eeprom[ip++]; // store name
    ram[ii++] = 2;            // length = 2 bytes
    ram[ii++] = eeprom[ip++]; // store MSB
    ram[ii++] = eeprom[ip++]; // store LSB
    ramUsed = ii;
}

void declareString(void)
{
#ifdef DEBUGOUT
    printf("declareString %u length %02x", eeprom[ip], eeprom[ip + 1]);
#endif
    unsigned short ii = locateVariable(eeprom[ip]);
    if (ramUsed + eeprom[ip + 1] > RAM_SIZE)
    {
#ifdef DEBUGOUT
        printf("Not enough RAM left to declare string var %c: RAM_SIZE %d ramUsed %d\n", 
                eeprom[ii], RAM_SIZE, ramUsed);
#endif
        return;
    }
    ram[ii++] = eeprom[ip++]; // store name
    unsigned char jj =  ip + eeprom[ip];
    ram[ii++] = eeprom[ip++]; // store length
    //while (ip <= jj)
    while (ip < jj)
    {
#ifdef DEBUGOUT
        printf(" %02x", eeprom[ip]);
#endif
        ram[ii++] = eeprom[ip++]; // store MSB
    }
    ramUsed = ii;
#ifdef DEBUGOUT
    printf("\n");
#endif
}

void doPauseMs(void)
{
    unsigned short value = (eeprom[ip] << 8) | eeprom[ip + 1];
    usleep(value * 1000);
    ip += 2;
}

void doPauseSecs(void)
{
    unsigned short value = (eeprom[ip] << 8) | eeprom[ip + 1];
    usleep(value * 1e6);
    ip += 2;
}

void decInteger(void)
{
    short ii = locateVariable(eeprom[ip++]) + 2;
    short value = (ram[ii] << 8) | ram[ii + 1];
    value--;
    ram[ii] = value >> 8;
    ram[ii + 1] = value & 0xff;
}

void incInteger(void)
{
    short ii = locateVariable(eeprom[ip++]) + 2;
    short value = (ram[ii] << 8) | ram[ii + 1];
    value++;
    ram[ii] = value >> 8;
    ram[ii + 1] = value & 0xff;
}

void decString(void)
{
    short ii = locateVariable(eeprom[ip++]) + 1;
    ii += ram[ii];
    ram[ii]--;
}

void incString(void)
{
    short ii = locateVariable(eeprom[ip++]) + 1;
    ii += ram[ii];
    ram[ii]++;
}

void jump(void)
{
    ip = (eeprom[ip] << 8) | eeprom[ip + 1];
}

void jz(dump)
{
    unsigned short ii = locateVariable(eeprom[ip]) + 2;
    unsigned value = (ram[ii] << 8) | ram[ii + 1];
#ifdef DEBUGOUT
    printf("jz: var %c is %04x\n", eeprom[ip], value);
#endif

    ip++;
    if (0 == value)
    {
        ip = (eeprom[ip] << 8) | eeprom[ip + 1];
    }
    else
    {
        ip += 2;
    }
#ifdef DEBUGOUT
        printf("ip is now %04x\n", ip);
#endif
}

void interpreter(void)
{
    ip = (eeprom[0] << 8) | eeprom[1];
    while (1)
    {
#ifdef DEBUGOUT
        printf("%04x: %02x %c\n", ip, eeprom[ip], eeprom[ip]);
#endif
        switch (eeprom[ip])
        {
            case CMD_DECREMENT:
                ip++;
                if ((eeprom[ip] & 0xdf) == eeprom[ip])
                {
                    decString();
                }
                else
                {
                    decInteger();
                }
                break;

            case CMD_INCREMENT:
                ip++;
                if ((eeprom[ip] & 0xdf) == eeprom[ip])
                {
                    incString();
                }
                else
                {
                    incInteger();
                }
                break;

            case CMD_EMIT_CONSTANT:
                ip++;
                emitConst(eeprom[ip]);
                break;

            case CMD_EMIT_VARIABLE:
                ip++;
                emitVariable();
                break;

            case CMD_EMIT_INTEGER:
                ip++;
                emitInteger();
                break;

            case CMD_DECLARE_SHORT_VAR:
                ip++;
                declareShort();
                break;

            case CMD_DECLARE_STRING_VAR:
                ip++;
                declareString();
                break;

            case CMD_PAUSE_MS:
                ip++;
                doPauseMs();
                break;

            case CMD_PAUSE_S:
                ip++;
                doPauseSecs();
                break;

            case CMD_JUMP:
                ip++;
                jump();
                break;

            case CMD_JZ:
                ip++;
                jz();
                break;

            case CMD_END:
#ifdef DEBUGOUT
                printf("End\n");
#endif
                return;

            default:
#ifdef DEBUGOUT
                printf("Unknown command %c ip %04x\n", eeprom[ip], ip);
#endif
                return;
        }
    }
}

int main(int argc, char ** argv)
{
    struct stat fStat;
    stat(argv[1], &fStat);
    eeprom = malloc(fStat.st_size);
    FILE * opCodes = fopen(argv[1], "r");
    fread(eeprom, fStat.st_size, 1, opCodes);
    fclose(opCodes);

    struct termios termio;
#ifdef OBJOUT
    outDev = open("obj.out", O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
#else
    outDev = open("/dev/cu.SLAB_USBtoUART", O_RDWR | O_NOCTTY);
    //outDev = open("/dev/cu.SLAB_USBtoUART", O_RDWR | O_NOCTTY | O_NONBLOCK);
   // outDev = open("/dev/tty.SLAB_USBtoUART", O_WRONLY | O_NOCTTY | O_NONBLOCK);
    cfmakeraw(&termio);
    cfsetospeed(&termio, B38400);
    tcsetattr(outDev, TCSANOW, &termio);
#endif

    interpreter();

    close(outDev);
    free(eeprom);

}
