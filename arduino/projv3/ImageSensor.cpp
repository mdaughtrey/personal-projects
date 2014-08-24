#include <ImageSensor.h>
#include "WConstants.h"
#include "HardwareSerial.h"
#include <avr/interrupt.h>
#include <ctype.h>
#include <string.h>
#include <avrlibtypes.h>

#define PIN_CLOCK 4 // PD4 IC Pin 6 ArdPin 4 PCINT20
#define PIN_DATA 3 // PD3 IC Pin 5 ArdPin 3 PCINT19

#define PIN_INIT(pp)    { PORTD |= _BV(pp); }
#define PIN_LO(pp)      { DDRD |= _BV(pp); PORTD &= ~_BV(pp); }
#define PIN_HI(pp)      { DDRD &= ~_BV(pp); PORTD |= _BV(pp); }

#define CLOCK_INIT()    PIN_INIT(PIN_CLOCK)
#define CLOCK_HI()      PIN_HI(PIN_CLOCK)
#define CLOCK_LO()      PIN_LO(PIN_CLOCK)
#define IS_CLOCK_HI()   (PIND & _BV(PIN_CLOCK))
#define IS_CLOCK_LO()   (!(PIND & _BV(PIN_CLOCK)))

#define DATA_INIT()     PIN_INIT(PIN_DATA)
#define DATA_HI()       PIN_HI(PIN_DATA)
#define DATA_LO()       PIN_LO(PIN_DATA)
#define IS_DATA_HI()    (PIND & _BV(PIN_DATA))

namespace ImageSensor
{
void writeAddress(u08 address, u08 data);
u08 readAddress(u08 address);
void writeByte(u08 data);
u08 readByte();

void readRegisters()
{
    for (u08 ii = 0; ii < 0x44; ii++)
    {
        Serial.print((int)ii);
        Serial.print(": ");
        Serial.print((int)readAddress(ii));
        if (ii && (0 == ii % 8))
        {
            Serial.println(" ");
        }
        else
        {
            Serial.print(" ");
        }
    }
    Serial.println(" ");
}


void writeAddress(u08 address, u08 data)
{
    writeByte(address | 0x80);
    writeByte(data);
}

u08 readAddress(u08 address)
{
    writeByte(address);
    return readByte();
}

u08 init()
{
    CLOCK_HI();
    DATA_HI();
    CLOCK_LO();
    delayMicroseconds(10);
    CLOCK_HI();
    delay(10);
    return 0;
}

u08 quality()
{
}


void writeByte(u08 data)
{
    for (u08 ii = 0; ii < 8; ii++)
    {
        if (data & 0x80)
        {
            DATA_HI();
        }
        else
        {
            DATA_LO();
        }
        CLOCK_LO();
        delayMicroseconds(50);
        CLOCK_HI();
        delayMicroseconds(50);
        data <<= 1;
    }
    DATA_HI();
}

u08 readByte()
{
    u08 data = 0;

    for (u08 ii = 0; ii < 8; ii++)
    {
        CLOCK_LO();
        delayMicroseconds(50);
        CLOCK_HI();
        if (IS_DATA_HI())
        {
            data |= 1;
        }
        data <<= 1;;
        delayMicroseconds(50);
    }
    return data;
}



} // namespace ImageSensor
