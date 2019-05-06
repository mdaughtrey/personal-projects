#include <stdio.h>
#include <fcntl.h>
#include <termio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

const char * PORT = "/dev/ttyUSB0";
int SPEED = 57600;

int waitfor(int port, const char * target, int timeout)
{
    char buffer[64];
    char accum[64];
    time_t start = time(NULL);
    while(!strstr(accum, target))
    {
        int nr = read(port, buffer, sizeof(buffer));
        if (nr > 0)
        {
            strncat(accum, buffer, nr);
            printf("accum is %s\n", accum);
        }
        if ((time(NULL) - start) > timeout)
        {
            return 1;
        }
    }
    printf("received %s\n", accum);
    return 0;
}


int serinit(const char * portname, int speed)
{
    int port = open(portname, O_RDWR | O_NONBLOCK | O_NDELAY);
    if (port < -1)
    {
        printf("port open failed");
        return -1;
    }
    printf("port is %d\n", port);

    struct termios tty;
    memset(&tty, 0, sizeof(tty));

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag     &=  ~PARENB;        // Make 8n1
    tty.c_cflag     &=  ~CSTOPB;
    tty.c_cflag     &=  ~CSIZE;
    tty.c_cflag     |=  CS8;
    tty.c_cflag     &=  ~CRTSCTS;       // no flow control
    tty.c_lflag     =   0;          // no signaling chars, no echo, no canonical processing
    tty.c_oflag     =   0;                  // no remapping, no delays
    tty.c_cc[VMIN]      =   0;                  // read doesn't block
    tty.c_cc[VTIME]     =   5;                  // 0.5 seconds read timeout

    tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines
    tty.c_iflag     &=  ~(IXON | IXOFF | IXANY);// turn off s/w flow ctrl
    tty.c_lflag     &=  ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    tty.c_oflag     &=  ~OPOST;              // make raw

    /* Flush Port, then applies attributes */
    tcflush(port, TCIFLUSH );

    if (tcsetattr(port, TCSANOW, &tty) != 0)
    {
        printf("tcsetattr failed errno %d %s\n", errno, strerror(errno));
        return -1;
    }
    return port;
}

int main(int argc, char ** argv)
{
    int port = serinit(PORT, SPEED);
    if (port < 0)
    {
        printf("serial port init failed\n");
        return 1;
    }
    char buffer[256] = " v";
    int nw = write(port, &buffer, 2);
//    sleep(2);
    if(waitfor(port, "{State:Ready}\r\n", 5))
    {
        printf("timed out\n");
    }
    for (int ii = 0; ii < 50; ii++)
    {
        write(port, "l", 1);
        usleep(100);
        write(port, "L", 1);
        usleep(100000);
    }
    close(port);
}
