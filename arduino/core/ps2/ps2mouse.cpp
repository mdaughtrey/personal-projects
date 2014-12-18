#include "WConstants.h"
#include "HardwareSerial.h"
#include "ps2mouse.h"
#include <avr/interrupt.h>
#include <ctype.h>
#include <string.h>
#include <avrlibtypes.h>

#define PIN_DATA  4 // PD4 IC Pin 6 ArdPin 4 PCINT20
#define PIN_CLOCK 3 // PD3 IC Pin 5 ArdPin 3 PCINT19
//#define PIN_TRIGGER 3

#define PIN_INIT(pp)    { PORTD |= _BV(pp); }
#define PIN_LO(pp)      { DDRD |= _BV(pp); PORTD &= ~_BV(pp); }
#define PIN_HI(pp)      { DDRD &= ~_BV(pp); PORTD |= _BV(pp); }

//#define TRIGGER_INIT()  PIN_INIT(PIN_TRIGGER)
//#define TRIGGER_HI()    PIN_HI(PIN_TRIGGER)
//#define TRIGGER_LO()    PIN_LO(PIN_TRIGGER)

#define CLOCK_INIT()    PIN_INIT(PIN_CLOCK)
#define CLOCK_HI()      PIN_HI(PIN_CLOCK)
#define CLOCK_LO()      PIN_LO(PIN_CLOCK)
#define IS_CLOCK_HI()   (PIND & _BV(PIN_CLOCK))
#define IS_CLOCK_LO()   (!(PIND & _BV(PIN_CLOCK)))

#define DATA_INIT()     PIN_INIT(PIN_DATA)
#define DATA_HI()       PIN_HI(PIN_DATA)
#define DATA_LO()       PIN_LO(PIN_DATA)
#define IS_DATA_HI()    (PIND & _BV(PIN_DATA))

#define NUM_LOGBYTES 56
u08 logBytesIndex; //  = 0;
u08 logBytes[NUM_LOGBYTES];

#define logByte(byte) { logBytes[logBytesIndex++] = byte; logBytesIndex %= (NUM_LOGBYTES - 1); }

volatile s16 xPos;
volatile s16 yPos;
u08 bitCount;

u08 packet0;
u08 packet1;
u08 packet2;

#ifdef SDEBUG

void resetByteLog(void)
{
    bitCount = 0;
    //intCount = 0;
    logBytesIndex = 0;
    memset(logBytes, 0xff, NUM_LOGBYTES);
}

void showByteLog(void)
{
    for (u08 ii = 0; ii < NUM_LOGBYTES; ii++)
    {
        if (isprint(logBytes[ii]))
        {
            Serial.print(logBytes[ii]);
        }
        else
        {
            Serial.print('.');
        }
        Serial.println(logBytes[ii], 16);
    }
}

#endif  // SDEBUG

void debugReport()
{
//    Serial.print("Int ");
//    Serial.print((int)intCount);
    Serial.print(" X");
    Serial.print(xPos);
    Serial.print(" Y");
    Serial.println(yPos);
}


ISR(PCINT2_vect)
{
    if (IS_CLOCK_HI())
    {
        return;
    }

    u08 bit = 0;
    if (IS_DATA_HI())
    {
        bit = 1;
    }

    switch (bitCount)
    {
        case 0: // start bit Byte 1
            packet0 = 0;
            packet1 = 0;
            packet2 = 0;

        case 11: // start bit Byte 2
        case 22: // start bit Byte 3
            // start bit must be 0
            if (1 == bit)
            {
                bitCount = 0;
                return;
            }
            break;

        case 20: // parity bit Byte 2
        case 9: // parity bit Byte 1
        case 31: // parity bit Byte 3
            break;

        case 10: // stop bit Byte 1
        case 21: // stop bit Byte 2
            // stop bit must be 1
            if (0 == bit)
            {
                bitCount = 0;
                return;
            }
            break;

        case 32:
#ifdef SDEBUG
            logByte('I');
            logByte(packet0);
            logByte(packet1);
            logByte(packet2);
#endif // SDEBUG
            // stop bit must be 1
            if (0 == bit)
            {
                bitCount = 0;
                return;
            }

            if (!(packet0 & 0x08))
            {
                bitCount = 0;
                return;
            }
            if (packet0 & 0x10)
            {
                xPos += (packet1 - 256);
            }
            else
            {
                xPos += packet1;
            }
            if (packet0 & 0x20)
            {
                yPos += (packet2 - 256);
            }
            else
            {
                yPos += packet2;
            }
            break;

        default:
            if (bitCount > 22)
            {
                packet2 |= (bit << (bitCount - 23));
            }
            else if (bitCount > 11)
            {
                packet1 |= (bit << (bitCount - 12));
            }
            else if (bitCount > 0)
            {
                packet0 |= (bit << (bitCount - 1));
            }
            break;
    }
    bitCount++;
    bitCount %= 33;
}

PS2Mouse::PS2Mouse()
{
}

void PS2Mouse::initialize()
{
    xPos = 0;
    yPos = 0;

    CLOCK_HI();
    DATA_HI();
    delay(20);

    write(0xff);  // Send Reset to the mouse
    read_byte();  // ack byte 
    read_byte();  // BAT completion code
    read_byte();  // mouse id (0)

    write(0xe8); // set resolution
    read_byte(); // read ack
    CLOCK_HI();
    DATA_HI();

    write(0x03); // 8 count/mm
    read_byte(); // read ack
    CLOCK_HI();
    DATA_HI();

    write(0xf4); // enable reporting
    read_byte(); // read ACK

#ifdef SDEBUG
    resetByteLog();
#endif // SDEBUG

    //
    // Enable reporting
    //
    write(0xf4); // enable reporting
    read_byte(); // read ACK

    CLOCK_HI();
    DATA_HI();

    bitCount = 0;
    PCMSK2 |= _BV(PCINT19);
    PCICR  |= _BV(PCIE2);
}

void PS2Mouse::set_stream_mode()
{
//    write(0xea); // set stream mode
//    read_byte(); // read ACK
    write(0xf4); // enable reporting
    read_byte(); // read ACK
 //   PCMSK2 |= _BV(PCINT20);
  //  PCICR  |= _BV(PCIE2);
}

void PS2Mouse::set_remote_mode()
{
    write(0xf0);
    read_byte(); 
}

void PS2Mouse::write_bit(u08 bit)
{
    while (IS_CLOCK_HI()); 
    if (bit)
    {
        DATA_HI();
    }
    else
    {
        DATA_LO(); 
    }
    while (IS_CLOCK_LO());
}

void PS2Mouse::write(u08 data)
{
    u08 parity = 1;
    DATA_HI();
    CLOCK_HI();
    delayMicroseconds(300);
    CLOCK_LO();
    delayMicroseconds(300);
    DATA_LO();
    delayMicroseconds(10);
    CLOCK_HI();
    
    for (u08 ii = 0; ii < 8; ii++)
    {
        write_bit(data & 0x01);
        parity ^= !(data & 0x01);
        data >>= 1;
    }

    write_bit(parity & 0x01);
    write_bit(1); // stop bit
    read_bit(); // read ack bit
    DATA_HI();
    CLOCK_LO(); // communication inhibited
}

// expectation is that clock and data lines are high
u08 PS2Mouse::read_byte()
{
    CLOCK_HI(); // move bus to idle state
    u08 data = 0;

    read_bit(); // skip start bit

    for (u08 ii = 0; ii < 8; ii++)
    {
        data |= (read_bit() << 7);
        data >>= 1;;
    }
    read_bit(); // parity bit
    read_bit(); // stop bit
    CLOCK_LO(); // move clock to inhibit state
    return data;
}

u08 PS2Mouse::read_bit()
{
    u08 bit;
    while (IS_CLOCK_HI()); // wait for mouse to grab the bus
    //delayMicroseconds(10);
    if (IS_DATA_HI())
    {
        bit = 1;
    }
    else
    {
        bit = 0;
    }
    while (IS_CLOCK_LO()); // run out the cycle
    return bit;
}

