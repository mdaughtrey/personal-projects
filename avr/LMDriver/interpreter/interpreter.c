#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

//#define EMBEDDED
//#define OBJOUT
//#define DEBUGOUT 0
#define DEBUGOUT
#ifdef EMBEDDED
#undef DEBUGOUT
#endif

//!a - add ivar2 to ivar1
#define CMD_ADD 'a'
//!u - sub ivar2 from ivar1
#define CMD_SUB 'u'
//!p pause in ms
#define CMD_PAUSE_MS 'p'
//!P pause in secs
#define CMD_PAUSE_S 'P'
//!e - emit constant
#define CMD_EMIT_CONSTANT 'e'
//!E - emit variable
#define CMD_EMIT_STRING 'S'
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
// assign ivar to ivar
#define CMD_ASSIGN_IVAR_IVAR 'm'
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
    unsigned short location = (eeprom[ip] << 8) | eeprom[ip + 1];
    unsigned short value = (ram[location] << 8) | ram[location + 1];
#ifdef DEBUGOUT
    printf("emitInteger Loc %04x Value %04x", location, value);
#endif
    if (ram[location])
    {
#ifdef DEBUGOUT
        printf(" %02x", ram[location]);
#endif
        write(outDev, &ram[location], 1);
        usleep(1000);
    }
#ifdef DEBUGOUT
    printf(" %02x", ram[location + 1]);
#endif
    write(outDev, &ram[location + 1], 1);
    usleep(1000);
    ip += 2;
#ifdef DEBUGOUT
    printf("\n");
#endif
}

void emitString(void)
{
    unsigned short location = (eeprom[ip] << 8) | eeprom[ip + 1];
    unsigned short length = ram[location++];
#ifdef DEBUGOUT
    printf("emitString Loc %04x Len %02x", location, length);
#endif
    ip += 2;
    while (--length)
    {
#ifdef DEBUGOUT
        printf(" %02x", ram[location]);
#endif
        write(outDev, &ram[location++], 1);
    }
#ifdef DEBUGOUT
    printf("\n");
#endif
}

void declareShort(void)
{
    unsigned short location = (eeprom[ip] << 8) | eeprom[ip + 1];
    ram[location] = eeprom[ip + 2];     // store MSB
    ram[location + 1] = eeprom[ip + 3]; // store LSB
    ip += 4;
#ifdef DEBUGOUT
    printf("declareShort Loc %04x Value %04x\n", location, (ram[location] << 8) | ram[location + 1]);
#endif
}

void declareString(void)
{
    unsigned char location = (eeprom[ip] << 8) | eeprom[ip + 1];
    unsigned char length = eeprom[ip + 2];
#ifdef DEBUGOUT
    printf("declareString Loc %04x Len %02x", location, length);
#endif // DEBUGOUT
    ip += 3;
    ram[location++] = length;
    while (--length)
    {
#ifdef DEBUGOUT
        printf(" %02x", eeprom[ip]);
#endif // DEBUGOUT
        ram[location++] = eeprom[ip++];
    }
#ifdef DEBUGOUT
        printf("\n");
#endif // DEBUGOUT
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

void decShort(void)
{
    unsigned short location = (eeprom[ip] << 8) | eeprom[ip + 1];
#ifdef DEBUGOUT
    printf("decShort @ %04x", location);
#endif

    unsigned short value = (ram[location] << 8) | ram[location + 1];
    value--;
    ram[location] = (value >> 8) & 0xff;
    ram[location + 1] = value & 0xff;

#ifdef DEBUGOUT
    printf(" new value %04x\n", value);
#endif
    ip += 2;
}

void incShort(void)
{
    unsigned short location = (eeprom[ip] << 8) | eeprom[ip + 1];
#ifdef DEBUGOUT
    printf("incShort @ %04x", location);
#endif

    unsigned short value = (ram[location] << 8) | ram[location + 1];
    value++;
    ram[location] = (value >> 8) & 0xff;
    ram[location + 1] = value & 0xff;

#ifdef DEBUGOUT
    printf(" new value %04x\n", value);
#endif
    ip += 2;
}

//void decShort(void)
//{
//    short ii = locateVariable(eeprom[ip++]) + 1;
//    ii += ram[ii];
//    ram[ii]--;
//}
//
//void incShort(void)
//{
//    short ii = locateVariable(eeprom[ip++]) + 1;
//    ii += ram[ii];
//    ram[ii]++;
//}

void jump(void)
{
    ip = (eeprom[ip] << 8) | eeprom[ip + 1];
#ifdef DEBUGOUT
    printf("Jumping to %04x\n", ip);
#endif
}

void jz(void)
{
    unsigned short location = (eeprom[ip] << 8) | eeprom[ip + 1];
    unsigned short varValue = (ram[location] << 8) | ram[location + 1];
    if (0 == varValue)
    {
        location = (eeprom[ip + 2] << 8) | eeprom[ip + 3];
     //   unsigned short jumpTo = (ram[location] << 8) | ram[location + 1];
        ip = location;
    }
    else
    {
        ip += 4;
    }
}

void add(void)
{
    unsigned short location1 = (eeprom[ip] << 8) | eeprom[ip + 1];
    unsigned short varValue1 = (ram[location1] << 8) | ram[location1 + 1];
    unsigned short location2 = (eeprom[ip + 2] << 8) | eeprom[ip + 3];
    unsigned short varValue2 = (ram[location2] << 8) | ram[location2 + 1];

#ifdef DEBUGOUT
    printf("%d += %d\n", varValue1, varValue2);
#endif
    varValue1 += varValue2;
    ram[location1] = (varValue1 >> 8) & 0xff;
    ram[location1 + 1] = varValue1 & 0xff;
    ip += 4;
}

void sub(void)
{
    unsigned short location1 = (eeprom[ip] << 8) | eeprom[ip + 1];
    unsigned short varValue1 = (ram[location1] << 8) | ram[location1 + 1];
    unsigned short location2 = (eeprom[ip + 2] << 8) | eeprom[ip + 3];
    unsigned short varValue2 = (ram[location2] << 8) | ram[location2 + 1];

#ifdef DEBUGOUT
    printf("%d -= %d\n", varValue1, varValue2);
#endif
    varValue1 -= varValue2;
    ram[location1] = (varValue1 >> 8) & 0xff;
    ram[location1 + 1] = varValue1 & 0xff;
    ip += 4;
}

void assignIvarIvar(void)
{
    unsigned short location1 = (eeprom[ip] << 8) | eeprom[ip + 1];
    unsigned short location2 = (eeprom[ip + 2] << 8) | eeprom[ip + 3];
    unsigned short value = (ram[location2] << 8) | ram[location2 + 1];

    ram[location1] = (value >> 8) & 0xff;
    ram[location1 + 1] = value & 0xff;
    ip += 4;
}


void interpreter(void)
{
    ip = (eeprom[0] << 8) | eeprom[1];
    while (1)
    {
        // read dev and throw away stuff
        char buffer[10];
        int numRead = read(outDev, buffer, 10);
#ifdef DEBUGOUT
        if (-1 != numRead)
        {
            printf("Read:");
            while (numRead-- > 0)
            {
                printf(" %02x", buffer[numRead]);
            }
            printf("\n");
        }
        printf("%04x: %02x %c\n", ip, eeprom[ip], eeprom[ip]);
#endif
        switch (eeprom[ip])
        {
            case CMD_ADD:
                ip++;
                add();
                break;

            case CMD_SUB:
                ip++;
                sub();
                break;

            case CMD_DECREMENT:
                ip++;
                decShort();
                break;

            case CMD_INCREMENT:
                ip++;
                incShort(); 
                break;

            case CMD_EMIT_CONSTANT:
                ip++;
                emitConst(eeprom[ip]);
                break;

            case CMD_EMIT_STRING:
                ip++;
                emitString();
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

            case CMD_ASSIGN_IVAR_IVAR:
                ip++;
                assignIvarIvar();
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
    outDev = open("/dev/cu.SLAB_USBtoUART", O_RDWR | O_NOCTTY | O_NONBLOCK);
#ifdef DEBUGOUT
    printf("outDev %d\n", outDev);
#endif
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
