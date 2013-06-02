#include <stdio.h>
#ifndef EMBEDDED
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#else // EMBEDDED
//#define EMBEDDED_DEBUG
#include <avr/eeprom.h>
#endif // EMBEDDED

//#define STATEFUL
//#define EMBEDDED
//#define OBJOUT
//#define DEBUGOUT 0
//#define DEBUGOUT
#ifdef EMBEDDED
#undef DEBUGOUT
#else
typedef unsigned char uint8_t;
#endif

//#ifdef STATEFUL
#ifndef EMBEDDED
#include <sys/time.h>
struct timeval pauseStart;
#else
extern volatile unsigned char delayCount;
#endif // EMBEDDED
//#endif // STATEFUL

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

//!z - jump if integer is less than or equal to zero
#define CMD_JLE 'z'

#define CMD_END '!'
#define RAM_SIZE 64
#define CONST_OFFSET 2

//#ifdef STATEFUL
typedef enum
{
    INTERPRETER = 0,
    EMIT_VAR,
    EMIT_CONST,
    PAUSE_MS,
    PAUSE_S
} StateType;

StateType iState;
unsigned char length;
unsigned short remaining;
unsigned short stateLocation;

//#endif // STATEFUL

int outDev;
unsigned short ip;
#ifndef EMBEDDED
unsigned char * eeprom;
#endif // EMBEDDED

unsigned char ram[RAM_SIZE] = {0};
unsigned short ramUsed = 0;
unsigned short msTick;

//ISR(SIG_OVERFLOW0)
//{
//    if (msTick)
//    {
//        msTick--;
//    }
//}

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

unsigned char readEeprom(unsigned short address)
{
#ifdef EMBEDDED
    unsigned char ch = eeprom_read_byte((uint8_t*)address);
//#ifdef EMBEDDED_DEBUG
//    uart_send_buffered('R');
//    uart_send_hex_byte(ch);
//    uart_send_buffered('@');
//    uart_send_hex_byte(address >> 8);
//    uart_send_hex_byte(address & 0xff);
//    uart_send_buffered(' ');
//#endif // EMBEDDED_DEBUG
    return ch;
#else // EMBEDDED
    return eeprom[address];
#endif // EMBEDDED
}

void emitConst(char constName)
{
    unsigned short ii = CONST_OFFSET;
    unsigned short constEnd = (readEeprom(0) << 8) | readEeprom(1);

#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
    char txt[64];
    sprintf(txt, "emitConst %u: ", readEeprom(ip) & 0xff);
    write(1, txt, strlen(txt));
#endif // EMBEDDED
#endif

    while (ii < constEnd)
    {
        if (constName == readEeprom(ii))
        {
//#ifndef STATEFUL
//            unsigned char count;
//            ii++;               // point to const length
//            count = readEeprom(ii++);
//#ifdef DEBUGOUT
//            printf(" length %02x ", count);
//#endif
//            while (--count)
//            {
//#ifdef DEBUGOUT
//        printf(" %02x", readEeprom(ii));
//#endif
//                write(outDev, eeprom + ii, 1);
//                usleep(100000);
//                ii++;
//            }
//#ifdef DEBUGOUT
//            printf("\n");
//#endif
//#else // STATEFUL
            ii++;     // point to const length
            iState = EMIT_CONST;
            remaining = readEeprom(ii++);
            stateLocation = ii;
//#endif // STATEFUL
            ip++;
            return;
        }
        else
        {
            ii++;               // point to length
            ii += readEeprom(ii);   // skip constant
        }
    }
#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
    strcpy(txt, "\n");
    write(1, txt, 1);
#endif // EMBEDDED
#endif
    ip++;
}

void emitInteger(void)
{
    unsigned short location = (readEeprom(ip) << 8) | readEeprom(ip + 1);
    unsigned short value = (ram[location] << 8) | ram[location + 1];
#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
    char txt[64];
    sprintf(txt, "emitInteger Loc %04x Value %04x", location, value);
    write(1, txt, strlen(txt));
#endif // EMBEDDED
#endif
//#ifdef STATEFUL
    iState = EMIT_VAR;
    if (ram[location])
    {
        remaining = 3;
        stateLocation = location;
    }
    else
    {
        remaining = 2;
        stateLocation = location + 1;
    }
//#else // STATEFUL
//    if (ram[location])
//    {
//#ifdef DEBUGOUT
//        printf(" %02x", ram[location]);
//#endif
//        write(outDev, ram + location, 1);
//        usleep(100000);
//    }
//#ifdef DEBUGOUT
//    printf(" %02x", ram[location + 1]);
//#endif
//    write(outDev, ram + location + 1, 1);
//    usleep(100000);
//#endif // STATEFUL
    ip += 2;
#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
    sprintf(txt, "\n");
    write(1, txt, 1);
#endif // EMBEDDED
#endif
}

void emitString(void)
{
    unsigned short location = (readEeprom(ip) << 8) | readEeprom(ip + 1);
    unsigned short length = ram[location++];
#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
    char txt[64];
    sprintf(txt, "emitString Loc %04x Len %02x", location, length);
    write(1, txt, strlen(txt));
#endif // EMBEDDED
#endif
    ip += 2;
//#ifdef STATEFUL
    iState = EMIT_VAR;
    remaining = length;
    stateLocation = location;
//#else // STATEFUL 
//    while (--length)
//    {
//#ifdef DEBUGOUT
//        printf(" %02x", ram[location]);
//#endif
//        write(outDev, ram + location++, 1);
//    }
//#endif // STATEFUL
#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
    sprintf(txt, "\n");
    write(1, txt, strlen(txt));
#endif // EMBEDDED
#endif
}

void declareShort(void)
{
    unsigned short location = (readEeprom(ip) << 8) | readEeprom(ip + 1);
    ram[location] = readEeprom(ip + 2);     // store MSB
    ram[location + 1] = readEeprom(ip + 3); // store LSB
    ip += 4;
#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
    char txt[64];
    sprintf(txt, "declareShort Loc %04x Value %04x\n", location, (ram[location] << 8) | ram[location + 1]);
    write(1, txt, strlen(txt));
#endif // EMBEDDED
#endif
}

void declareString(void)
{
    unsigned char location = (readEeprom(ip) << 8) | readEeprom(ip + 1);
    unsigned char length = readEeprom(ip + 2);
#ifdef DEBUGOUT
#ifdef EMBEDDED
    char txt[64];
    sprintf(txt, "declareString Loc %04x Len %02x", location, length);
    write(1, txt, strlen(txt));
#else // EMBEDDED
#endif // EMBEDDED
#endif // DEBUGOUT
    ip += 3;
    ram[location++] = length;
    while (--length)
    {
#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
        char txt[64];
        sprintf(txt, " %02x", readEeprom(ip));
        write(1, txt, strlen(txt));
#endif // EMBEDDED
#endif // DEBUGOUT
        ram[location++] = readEeprom(ip++);
    }
#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
        char txt[64];
        sprintf(txt, "\n");
        write(1, txt, strlen(txt));
#endif // EMBEDDED
#endif // DEBUGOUT
}

void doPauseMs(void)
{
    unsigned short value = (readEeprom(ip) << 8) | readEeprom(ip + 1);
#ifdef EMBEDDED
    delayCount = value >> 4; // convert to 8ms intervals
    if (delayCount == 0)
    {
        delayCount = 1;
    }
    TIMSK0 |= _BV(TOIE0); // enable timer
#else // EMBEDDED
//#ifdef STATEFUL
    remaining = value;
    iState = PAUSE_MS;
    gettimeofday(&pauseStart, NULL);
//#else // STATEFUL
//    usleep(value * 1000);
//#endif // STATEFUL
#endif // EMBEDDED
    ip += 2;
}

void doPauseSecs(void)
{
    unsigned short value = (readEeprom(ip) << 8) | readEeprom(ip + 1);
#ifdef EMBEDDED
    delayCount = value << 7; // convert to 8ms intervals
    if (delayCount == 0)
    {
        delayCount = 1;
    }
    TIMSK0 |= _BV(TOIE0); // enable timer
    // start the tick interrupt
#else // EMBEDDED
//#ifdef STATEFUL
    remaining = value;
    iState = PAUSE_S;
    gettimeofday(&pauseStart, NULL);
//#else // STATEFUL
//    usleep(value * 1e6);
//#endif // STATEFUL
#endif // EMBEDDED
    ip += 2;
}

void decShort(void)
{
    unsigned short location = (readEeprom(ip) << 8) | readEeprom(ip + 1);
#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
    char txt[64];
    sprintf(txt, "decShort @ %04x", location);
    write(1, txt, strlen(txt));
#endif // EMBEDDED
#endif


    unsigned short value = (ram[location] << 8) | ram[location + 1];
    value--;
    ram[location] = (value >> 8) & 0xff;
    ram[location + 1] = value & 0xff;

#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
    sprintf(txt, " new value %04x\n", value);
    write(1, txt, strlen(txt));
#endif // EMBEDDED
#endif
    ip += 2;
}

void incShort(void)
{
    unsigned short location = (readEeprom(ip) << 8) | readEeprom(ip + 1);
#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
    char txt[64];
    sprintf(txt, "incShort @ %04x", location);
    write(1, txt, strlen(txt));
#endif // EMBEDDED
#endif

    unsigned short value = (ram[location] << 8) | ram[location + 1];
    value++;
    ram[location] = (value >> 8) & 0xff;
    ram[location + 1] = value & 0xff;

#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
    sprintf(txt, " new value %04x\n", value);
    write(1, txt, strlen(txt));
#endif // EMBEDDED
#endif
    ip += 2;
}

void jump(void)
{
    ip = (readEeprom(ip) << 8) | readEeprom(ip + 1);
#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
    char txt[64];
    sprintf(txt, "Jumping to %04x\n", ip);
    write(1, txt, strlen(txt));
#endif // EMBEDDED
#endif
}

void jle(void)
{
    unsigned short location = (readEeprom(ip) << 8) | readEeprom(ip + 1);
    short varValue = (ram[location] << 8) | ram[location + 1];
    if (0 > varValue)
    {
        location = (readEeprom(ip + 2) << 8) | readEeprom(ip + 3);
        ip = location;
    }
    else
    {
        ip += 4;
    }
}

//void jlt(void)
//{
//    unsigned short location = (readEeprom(ip) << 8) | readEeprom(ip + 1);
//    short varValue = (ram[location] << 8) | ram[location + 1];
//    if (0 >= varValue)
//    {
//        location = (readEeprom(ip + 2) << 8) | readEeprom(ip + 3);
//        ip = location;
//    }
//    else
//    {
//        ip += 4;
//    }
//}
//
//void jz(void)
//{
//    unsigned short location = (readEeprom(ip) << 8) | readEeprom(ip + 1);
//    short varValue = (ram[location] << 8) | ram[location + 1];
//    if (0 == varValue)
//    {
//        location = (readEeprom(ip + 2) << 8) | readEeprom(ip + 3);
//        ip = location;
//    }
//    else
//    {
//        ip += 4;
//    }
//}

void add(void)
{
    unsigned short location1 = (readEeprom(ip) << 8) | readEeprom(ip + 1);
    unsigned short varValue1 = (ram[location1] << 8) | ram[location1 + 1];
    unsigned short location2 = (readEeprom(ip + 2) << 8) | readEeprom(ip + 3);
    unsigned short varValue2 = (ram[location2] << 8) | ram[location2 + 1];

#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
    char txt[64];
    sprintf(txt, "%d += %d\n", varValue1, varValue2);
    write(1, txt, strlen(txt));
#endif // EMBEDDED
#endif
    varValue1 += varValue2;
    ram[location1] = (varValue1 >> 8) & 0xff;
    ram[location1 + 1] = varValue1 & 0xff;
    ip += 4;
}

void sub(void)
{
    unsigned short location1 = (readEeprom(ip) << 8) | readEeprom(ip + 1);
    unsigned short varValue1 = (ram[location1] << 8) | ram[location1 + 1];
    unsigned short location2 = (readEeprom(ip + 2) << 8) | readEeprom(ip + 3);
    unsigned short varValue2 = (ram[location2] << 8) | ram[location2 + 1];

#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
    char txt[64];
    sprintf(txt, "%d -= %d\n", varValue1, varValue2);
    write(1, txt, strlen(txt));
#endif // EMBEDDED
#endif
    varValue1 -= varValue2;
    ram[location1] = (varValue1 >> 8) & 0xff;
    ram[location1 + 1] = varValue1 & 0xff;
    ip += 4;
}

void assignIvarIvar(void)
{
    unsigned short location1 = (readEeprom(ip) << 8) | readEeprom(ip + 1);
    unsigned short location2 = (readEeprom(ip + 2) << 8) | readEeprom(ip + 3);
    unsigned short value = (ram[location2] << 8) | ram[location2 + 1];

    ram[location1] = (value >> 8) & 0xff;
    ram[location1 + 1] = value & 0xff;
    ip += 4;
}


#if 0
//#ifdef STATEFUL
unsigned char stateVar(void)
{
    unsigned char result;
    //remaining--;
#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
    char txt[64];
    sprintf(txt,"stateVar location %04d Remaining %d [%02x]\n", stateLocation, remaining, *(ram + stateLocation));
    write(1, txt, strlen(txt));
#endif // EMBEDDED
#endif
    if (remaining--)
    {
//#ifndef EMBEDDED
//        write(outDev, ram + stateLocation, 1);
//#else // EMBEDDED
        result = ram[stateLocation];
//        usleep(1e5);
//#endif // EMBEDDED
        stateLocation++;
    }
    else
    {
        iState = INTERPRETER;
    }
    return result;
}
#endif // 0

#if 0
unsigned char stateConst(void)
{
    unsigned char result;
#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
    char txt[64];
    sprintf(txt, "stateConst location %04d Remaining %d [%02x]\n", stateLocation, remaining, *(eeprom + stateLocation));
    write(1, txt, strlen(txt));
#endif // EMBEDDED
#endif
    if (remaining--)
    {
        result = readEeprom(stateLocation);
        stateLocation++;
    }
    else
    {
        iState = INTERPRETER;
    }
    return result;
}
#endif // 0

void statePauseS(void)
{
#ifdef EMBEDDED
    if (0 == delayCount)
    {
        iState = INTERPRETER;
    }
#else // EMBEDDED
    struct timeval timeNow;
    gettimeofday(&timeNow, NULL);

    if ((timeNow.tv_sec - pauseStart.tv_sec) > remaining)
    {
#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
    char txt[64];
    sprintf(txt, "statePauseS end\n");
    write(1, txt, strlen(txt));
#endif // EMBEDDED
#endif
    // start the tick interrupt
        iState = INTERPRETER;
    }
#endif // EMBEDDED
}

void statePauseMs()
{
#ifdef EMBEDDED
    if (0 == delayCount)
    {
        iState = INTERPRETER;
    }
#else // EMBEDDED
    struct timeval timeNow;
    gettimeofday(&timeNow, NULL);
    int startedAt = pauseStart.tv_usec / 1000;
    int now = timeNow.tv_usec / 1000;
    if (timeNow.tv_sec > pauseStart.tv_sec)
    {
        now += 1000;
    }

    if ((now - startedAt) > remaining)
    {
#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
    char txt[64];
    sprintf(txt, "statePauseMs end\n");
    write(1, txt, strlen(txt));
#endif // EMBEDDED
#endif
    // start the tick interrupt
        iState = INTERPRETER;
    }
#endif // EMBEDDED
}

//#endif // STATEFUL


unsigned short interpreter(void)
{
#ifndef EMBEDDED
        // read dev and throw away stuff
        char buffer[10];
        int numRead = read(outDev, buffer, 10);
#ifdef EMBEDDED_DEBUG
        uart_send_buffered('I');
        uart_send_hex_byte(stateLocation);
#endif // EMBEDDED_DEBUG
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
#endif
#endif // EMBEDDED
        if (EMIT_VAR == iState)
        {
            if (--remaining)
            {
                return ram[stateLocation] | 0x0100;
            }
            else
            {
                iState = INTERPRETER;
                return 0;
            }
        }
        else if (EMIT_CONST == iState)
        {
            if (--remaining)
            {
                return readEeprom(stateLocation++) | 0x0100;
            }
            else
            {
                iState = INTERPRETER;
                return 0;
            }
        }
        else if (PAUSE_S == iState)
        {
            statePauseS();
        }
        else if (PAUSE_MS == iState)
        {
            statePauseMs();
        }
        else
        {
#ifdef DEBUGOUT
#ifdef EMBEDDED
#else // EMBEDDED
    char txt[64];
    sprintf(txt, "%04x: %02x %c\n", ip, readEeprom(ip), readEeprom(ip));
    write(1, txt, strlen(txt));
#endif // EMBEDDED
#endif
            switch (readEeprom(ip))
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
                    emitConst(readEeprom(ip));
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

                case CMD_JLE:
                    ip++;
                    jle();
                    break;

//                case CMD_JLT:
//                    ip++;
//                    jlt();
//                    break;
//
//                case CMD_JZ:
//                    ip++;
//                    jz();
//                    break;

                case CMD_ASSIGN_IVAR_IVAR:
                    ip++;
                    assignIvarIvar();
                    break;

                case CMD_END:
                    return 0x0200;

                default:
#ifdef DEBUGOUT
                    printf("Unknown command %c ip %04x\n", readEeprom(ip), ip);
#endif
                    return 0;
            }
        }
        return 0;
}

//#ifdef STATEFUL
void interpreter_init(void)
{
    ip = (readEeprom(0) << 8) | readEeprom(1);
    iState = INTERPRETER;
    // Initialize the timer interrupt parameters
#ifdef EMBEDDED
    TCNT0 = 0;
    TCCR0A |= _BV(CS02);
#endif // EMBEDDED
}
//#endif // STATEFUL

#ifndef EMBEDDED

int main(int argc, char ** argv)
{
    struct stat fStat;
    stat(argv[1], &fStat);
    eeprom = malloc(fStat.st_size);
    FILE * opCodes = fopen(argv[1], "r");
    fread(eeprom, fStat.st_size, 1, opCodes);
    fclose(opCodes);

//#ifdef STATEFUL
    interpreter_init();
//#endif // STATEFUL

    struct termios termio;
#ifdef OBJOUT
    outDev = open("obj.out", O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
#else
    outDev = open("/dev/cu.SLAB_USBtoUART", O_RDWR | O_NOCTTY | O_NONBLOCK);
    cfmakeraw(&termio);
    tcgetattr(outDev, &termio);
    cfsetospeed(&termio, B38400);
    cfsetispeed(&termio, B38400);
    tcsetattr(outDev, TCSANOW, &termio);
#ifdef DEBUGOUT
    printf("outDev %d\n", outDev);
#endif // DEBUGOUT
#endif

    while(1) 
    {
        char txt[32];
        unsigned short ushort = interpreter();
        if (ushort & 0x0200)
        {
            break;
        }
        if (ushort & 0x0100)
        {
            char ch = ushort & 0xff;
            sprintf(txt, "Emit %02x\n", ch);
            write(1, txt, strlen(txt));
            write(outDev, &ch, 1);
            usleep(100000);
        }
    }

    close(outDev);
    free(eeprom);

}

#endif // EMBEDDED
