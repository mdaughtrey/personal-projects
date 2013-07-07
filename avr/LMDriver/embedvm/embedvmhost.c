#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <embedvm.h>
#include <cmdproc2.h>
#ifndef EMBEDDED
#include <sys/time.h>
struct timeval pauseStart;
#endif // EMBEDDED

#define RAM_SIZE 512 

extern volatile unsigned char delayCount;

//typedef enum
//{
//    INTERPRETER = 0,
//    EMIT_VAR,
//    EMIT_CONST,
//    PAUSE_MS,
//    PAUSE_S
//} StateType;
//
//StateType iState;
//unsigned char length;
unsigned short remaining;
//unsigned short stateLocation;
bool stop = false;

int outDev;
//unsigned short ip;
unsigned char * eeprom;
unsigned char * memory;
//unsigned char memory[RAM_SIZE] = {0};
unsigned short msTick;
unsigned char inDelay = 0;

#define UNUSED __attribute__((unused))

static int16_t mem_read(uint16_t addr, bool is16bit, void *ctx UNUSED);
static void mem_write(uint16_t addr, int16_t value, bool is16bit, void *ctx UNUSED);
static int16_t call_user(uint8_t funcid, uint8_t argc, int16_t *argv, void *ctx UNUSED);
static int8_t rom_read(uint16_t addr, void *ctx UNUSED);

struct embedvm_s vm = {
    0xffff, RAM_SIZE, RAM_SIZE, NULL,
	&mem_read, &mem_write, &call_user, &rom_read
};

static int16_t mem_read(uint16_t addr, bool is16bit, void *ctx UNUSED)
{
	if (is16bit)
    {
		return (memory[addr] << 8) | memory[addr+1];
    }
	return memory[addr];
}

static void mem_write(uint16_t addr, int16_t value, bool is16bit, void *ctx UNUSED)
{
	if (is16bit)
    {
		memory[addr] = value >> 8;
		memory[addr+1] = value;
	}
    else
    {
		memory[addr] = value;
    }
}

static int16_t call_user(uint8_t funcid, uint8_t argc, int16_t *argv, void *ctx UNUSED)
{
	int i;
    char  tmp;

    switch (funcid)
    {
        case 0: // stop
            stop = true;
//            printf("Called user function 0 => stop.\n");
//            fflush(stdout);
            return 0;

        case 1: // emit
            tmp = *(char *)argv;
            printf("Writing out %c %02x\n", tmp, tmp);
            write(outDev, &tmp, 1);
            break;

        case 2: // printf
            tmp = *(char *)argv;
 //           printf("Result %02x\n", tmp);
  //          fflush(stdout);
            break;

        case 3: // pause ms
            gettimeofday(&pauseStart, NULL);
            inDelay = 1;
            remaining = *(int *)argv;
            break;
    }

//	printf("Called user function %d with %d args:\n", funcid, argc);
//	fflush(stdout);

	return 0;
}

static int8_t rom_read(uint16_t addr, void *ctx UNUSED)
{
    return eeprom[addr];
}

void doVm(struct embedvm_s *vm)
{
    if (inDelay)
    {
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
            inDelay = 0;
        }
    }
    else
    {
		embedvm_exec(vm);
    }
}

//ISR(SIG_OVERFLOW0)
//{
//    if (msTick)
//    {
//        msTick--;
//    }
//}

void doPauseMs(void)
{
//    unsigned short value = (readEeprom(ip) << 8) | readEeprom(ip + 1);
//#ifdef EMBEDDED
//    delayCount = value >> 4; // convert to 8ms intervals
//    if (delayCount == 0)
//    {
//        delayCount = 1;
//    }
//    TIMSK0 |= _BV(TOIE0); // enable timer
//#else // EMBEDDED
////#ifdef STATEFUL
//    remaining = value;
//    iState = PAUSE_MS;
//    gettimeofday(&pauseStart, NULL);
////#else // STATEFUL
////    usleep(value * 1000);
////#endif // STATEFUL
//#endif // EMBEDDED
//    ip += 2;
}

void doPauseSecs(void)
{
//    unsigned short value = (readEeprom(ip) << 8) | readEeprom(ip + 1);
//#ifdef EMBEDDED
//    delayCount = value << 7; // convert to 8ms intervals
//    if (delayCount == 0)
//    {
//        delayCount = 1;
//    }
//    TIMSK0 |= _BV(TOIE0); // enable timer
//    // start the tick interrupt
//#else // EMBEDDED
////#ifdef STATEFUL
//    remaining = value;
//    iState = PAUSE_S;
//    gettimeofday(&pauseStart, NULL);
////#else // STATEFUL
////    usleep(value * 1e6);
////#endif // STATEFUL
//#endif // EMBEDDED
//    ip += 2;
}

void statePauseS(void)
{
//#ifdef EMBEDDED
//    if (0 == delayCount)
//    {
//        iState = INTERPRETER;
//    }
//#else // EMBEDDED
//    struct timeval timeNow;
//    gettimeofday(&timeNow, NULL);
//
//    if ((timeNow.tv_sec - pauseStart.tv_sec) > remaining)
//    {
//#ifdef DEBUGOUT
//#ifdef EMBEDDED
//#else // EMBEDDED
//    char txt[64];
//    sprintf(txt, "statePauseS end\n");
//    write(1, txt, strlen(txt));
//#endif // EMBEDDED
//#endif
//    // start the tick interrupt
//        iState = INTERPRETER;
//    }
//#endif // EMBEDDED
}

void statePauseMs()
{
//#ifdef EMBEDDED
//    if (0 == delayCount)
//    {
//        iState = INTERPRETER;
//    }
//#else // EMBEDDED
//    struct timeval timeNow;
//    gettimeofday(&timeNow, NULL);
//    int startedAt = pauseStart.tv_usec / 1000;
//    int now = timeNow.tv_usec / 1000;
//    if (timeNow.tv_sec > pauseStart.tv_sec)
//    {
//        now += 1000;
//    }
//
//    if ((now - startedAt) > remaining)
//    {
//#ifdef DEBUGOUT
//#ifdef EMBEDDED
//#else // EMBEDDED
//    char txt[64];
//    sprintf(txt, "statePauseMs end\n");
//    write(1, txt, strlen(txt));
//#endif // EMBEDDED
//#endif
//    // start the tick interrupt
//        iState = INTERPRETER;
//    }
//#endif // EMBEDDED
}

int main(int argc, char ** argv)
{
    bool verbose = false;
    struct stat fStat;
    stat(argv[1], &fStat);
    if (fStat.st_size > RAM_SIZE)
    {
        fprintf(stderr, "image too large for RAM\n");
        exit(1);
    }
        
    memory = malloc(RAM_SIZE);
    //eeprom = malloc(fStat.st_size);
    FILE * opCodes = fopen(argv[1], "r");
    //fread(eeprom, fStat.st_size, 1, opCodes);
    fread(memory, fStat.st_size, 1, opCodes);
    fclose(opCodes);

    struct termios termio;
#ifdef OBJOUT
    outDev = open("obj.out", O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
#else
    outDev = open("/dev/cu.SLAB_USBtoUART", O_RDWR | O_NOCTTY | O_NONBLOCK);
    printf("outDev %d\n", outDev);
    fflush(stdout);
    cfmakeraw(&termio);
    tcgetattr(outDev, &termio);
    cfsetospeed(&termio, B38400);
    cfsetispeed(&termio, B38400);
    tcsetattr(outDev, TCSANOW, &termio);
#endif
	embedvm_interrupt(&vm, strtol(argv[2], NULL, 16));
	stop = false;
	while (!stop)
    {
		if (vm.ip == 0xffff)
        {
			printf("Main function returned => Terminating.\n");
			if (vm.sp != 0 || vm.sfp != 0)
            {
				printf("Unexpected stack configuration on program exit: SP=%04x, SFP=%04x\n", vm.sp, vm.sfp);
            }
			fflush(stdout);
			break;
		}
		if (verbose)
        {
			fprintf(stderr, "IP: %04x (%02x %02x %02x %02x),  ", vm.ip,
					memory[vm.ip], memory[vm.ip+1], memory[vm.ip+2], memory[vm.ip+3]);
			fprintf(stderr, "SP: %04x (%02x%02x %02x%02x %02x%02x %02x%02x), ", vm.sp,
					memory[vm.sp + 0], memory[vm.sp + 1],
					memory[vm.sp + 2], memory[vm.sp + 3],
					memory[vm.sp + 4], memory[vm.sp + 5],
					memory[vm.sp + 6], memory[vm.sp + 7]);
			fprintf(stderr, "SFP: %04x\n", vm.sfp);
			fflush(stderr);
		}
        doVm(&vm);
	}

//    while(1) 
//    {
//        char txt[32];
//        unsigned short ushort = interpreter();
//        if (ushort & 0x0200)
//        {
//            break;
//        }
//        if (ushort & 0x0100)
//        {
//            char ch = ushort & 0xff;
//            sprintf(txt, "Emit %02x\n", ch);
//            write(1, txt, strlen(txt));
//            write(outDev, &ch, 1);
//            usleep(100000);
//        }
//    }

    close(outDev);
    //free(eeprom);
    free(memory);

}

