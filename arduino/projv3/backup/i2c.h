#ifndef I2C_H

#include <WProgram.h>
#include <avrlibtypes.h>

class I2C
{
public:
    void init();
    void write(u16 addr, u08 byte) {}
    u08 read(u16){ return 0; }
};
#endif // I2C_H
