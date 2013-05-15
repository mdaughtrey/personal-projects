#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

//#define OBJOUT

int outDev;

union
{
    unsigned short uShort;
    unsigned char uChar[2];
}conv;


int main(int argc, char ** argv)
{
    unsigned char ch;
    struct stat fStat;
    struct termios termio;
    stat(argv[1], &fStat);
    conv.uShort = fStat.st_size;

    int binary = open(argv[1], O_RDONLY);
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

    ch = 'Z';
    write(outDev, &ch, 1);
        sleep(1);
    write(outDev, &conv.uChar[1], 1);
        sleep(1);
    write(outDev, &conv.uChar[0], 1);
        sleep(1);

    while (read(binary, &ch, 1))
    {
        int ii;
        char buffer[10];
        int numRead = read(outDev, buffer, 10);
        for ( ii = 0; ii < numRead; ii++)
        {
//            write(1, buffer + ii,1 );
        }
        char strBuffer[5];
        sprintf(strBuffer, "%02x ", ch);
        write(1, strBuffer, 3);
        write(outDev, &ch, 1);
        sleep(1);
    }
    close(outDev);
    close(binary);
}


