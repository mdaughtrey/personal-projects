#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

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
    unsigned char * binaryBuffer;
    binaryBuffer = (unsigned char *)malloc(fStat.st_size);

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
    usleep(100000);
    write(outDev, &conv.uChar[1], 1);
    usleep(100000);
    write(outDev, &conv.uChar[0], 1);
    usleep(100000);

    read(binary, binaryBuffer, fStat.st_size);

    int ii;
    int numRead;
    unsigned char buffer[129];

    for (ii = 0; ii < fStat.st_size; ii++)
    {
        if (ii % 10 == 0)
        {
            printf("%u of %u bytes uploaded\n", (unsigned int)ii, (unsigned int)fStat.st_size);
        }
//        write(1, binaryBuffer + ii, 1);
        write(outDev, binaryBuffer + ii, 1);
        numRead = read(outDev, buffer, 128);
 //       unsigned char jj;
 //       for (jj = 0; jj < numRead; jj++)
 //       {
 //           write(1, buffer + jj, 1);
 //       }
        usleep(100000);
    }

    while ((numRead = read(outDev, buffer, 128)) > 0)
    {
//        unsigned char jj;
//        for (jj = 0; jj < numRead; jj++)
//        {
//            write(1, buffer + jj, 1);
//        }
    }

    free(binaryBuffer);
    close(outDev);
    close(binary);
}


