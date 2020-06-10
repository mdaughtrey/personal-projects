#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

//#define OBJOUT
#define DEBUGOUT

int outDev;

union
{
    unsigned short uShort;
    unsigned char uChar[2];
}conv;

void debugOut(unsigned char ch)
{
    char buffer[10];
    sprintf(buffer, "%02x ", ch);
    write(1, buffer, 3);
}

int main(int argc, char ** argv)
{
    unsigned char ch;
    struct stat fStat;
    struct termios termio;
    stat(argv[1], &fStat);
    conv.uShort = fStat.st_size;

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

    for (ch = 0; ch < strlen(argv[1]); ch++)
    {
        debugOut(argv[1][ch]);
        write(outDev, argv[1] + ch, 1);
        sleep(1);
    }
    close(outDev);
}


