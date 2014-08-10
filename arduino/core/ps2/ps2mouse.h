#ifndef PS2Mouse_h
#include <avrlibtypes.h>

#define PS2Mouse_h
#define REMOTE 1
#define STREAM 2


class PS2Mouse
{
    private:
        u08 read_byte();
        u08 read_bit();
//        int read_movement_x(int);
//        int read_movement_y(int);
//        int read_movement(char);
        void write(u08);
        void write_bit(u08 bit);
    public:
        PS2Mouse();
        void initialize();
        void getIntCount();
//        void report(int * data);
        void set_remote_mode();
        void set_stream_mode();
};

#endif
