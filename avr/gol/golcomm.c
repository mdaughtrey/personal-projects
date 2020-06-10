#include <golcomm.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <typedefs.h>
#include <serutil.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

extern u16 colony[9];


//u16 above;
//u16 right;
//u16 below;
//u16 left;
u08 aboveBitStep;
u08 rightCycleStep;
u08 leftCycleStep;
u08 belowBitStep;
//u08 leftBitStep;

// PD5->PD4  PE6 -\
// PD6<-PD3  PE5 -/
// PD5 Temporary Signal
//

// PD4
// PB0 (ss/pcint8)    Above Tx ---+
// PB1 (sck/pcint9)   Above Rx -+ |
// PB2 (mosi/pcint10) Below Tx -+ |
// PB3 (miso/pcint11) Below Rx ---+
// PD3                Right Tx
// PD4                Right Rx
// PD5                Left Tx
// PD6                Left Rx
// PE0 (rxd/pcint0)   Above SCK (incoming) --+
// PE1 (txd/pcint1)   Below SCK (outgoing) --+
// PE6 (pcint6)       Right SCK (outgoing)
// PE5 (pcint5)       Left  SCK (incoming)
// SCK Idle Low
// Module Drives SCK for Right/Below
// Module Accepts SCK for Left/Above
// Present data on low-high
// Read data on high-low
// Protocol:
// 0000000 - header
// 12 (up/down) or 9 (left/right) bits encoded as:
// 0: a=0 b=1
// 1: a=1 b=0
//              1         2         3         4
// CS 012345678901234567890123456789012345678901234567
// Co ------000011112222333344445555666677778888
// DT 000000aabbaabbaabbaabbaabbaabbaabbaabbaabb       (left/right)
// DT 000000xxxxXXXX      (up/down)

// Left SCK incoming
u08 pinHistory;
u08 inRightRaw;
u16 inRightData;

u08 inLeftRaw;
u16 inLeftData;
u08 leftTxBitCount;

#define PIN_LEFTSCK _BV(PCINT5)
#define PIN_LEFTRX  6
#define PIN_LEFTTX  5
#define PIN_RIGHTRX 4
#define RAW_RESET   0b11101111

void setLeftColumn(u16 data)
{
    u08 ii;
    //uart_send_hex_byte(data >> 8);
    //uart_send_hex_byte(data & 0xff);
    //uart_send_sync(crlf, 2);
    for (ii = 0; ii < 9; ii++)
    {
        if (inRightData & _BV(ii))
        {
            colony[ii] |= _BV(13);
        }
        else
        {
            colony[ii] &= ~_BV(13);
        }
    }
    inRightData = 0;
}

void setRightColumn(u16 data)
{
    u08 ii;
    //uart_send_hex_byte(data >> 8);
    //uart_send_hex_byte(data & 0xff);
    //uart_send_sync(crlf, 2);
    for (ii = 0; ii < 9; ii++)
    {
        if (inRightData & _BV(ii))
        {
            colony[ii] |= _BV(2);
        }
        else
        {
            colony[ii] &= ~_BV(2);
        }
    }
    inRightData = 0;
}


ISR(PCINT0_vect)
{
#ifdef NOINT
    return;
#endif
    if ((PINE & PIN_LEFTSCK) != (pinHistory & PIN_LEFTSCK))
    {
        leftTxBitCount++;
        if (PINE & PIN_LEFTSCK) // low to high, present left data
        {
            //uart_send_hex_byte(leftTxBitCount);
            //uart_send_sync(crlf, 2);
            u08 colValue = colony[leftTxBitCount >> 2] >> 12;
            colValue &= 1;
            colValue += ((leftTxBitCount >> 1) & 1);
            if (1 == colValue)
            {
                PORTD &= ~_BV(PIN_LEFTTX); // assert left tx low
            }
            else
            {
                PORTD |= _BV(PIN_LEFTTX); // assert left tx high
            }
        }
        else // high to low, read right data
        {
            u08 count;
            inRightRaw <<= 1;
            if (PIND & _BV(PIN_LEFTRX))
            {
                inRightRaw |= 1;
            }
            // 
            // Check for reset state or complete bit 
            //
            switch (inRightRaw)
            {
            case 0b01111000: // reset state
                inRightRaw = RAW_RESET;
                inRightData = 0;
                leftTxBitCount = 0;
                break;

            case 0b10111101: // logical 0
                count = (inRightData & 0xf000) >> 12;
                inRightData <<= 1;
                count++;
                inRightRaw = RAW_RESET;
                if (9 == count)
                {
                    setLeftColumn(inRightData);
                    break;
                }
                inRightData &= 0x0fff;
                inRightData |= (count << 12);
                break;

            case 0b10111110: // logical 1
                count = (inRightData & 0xf000) >> 12;
                inRightData <<= 1;
                inRightData |= 1;
                count++;
                inRightRaw = RAW_RESET;
                if (9 == count)
                {
                    setLeftColumn(inRightData);
                    break;
                }
                inRightData &= 0x0fff;
                inRightData |= (count << 12);
                break;

            default: // in transit 
                break;

            }
        }
        pinHistory ^= PIN_LEFTSCK;
    }
}

void commInit(void)
{
    PORTD |= 0b00101000; // pullups
    DDRD |= 0b00101000; // left tx right tx
    PORTE |= 0b01000000; // right sck pullup
    DDRE |= 0b01000000; // right sck
    DDRB |= 0b00000001; // sig
    //aboveBitStep = 0;
    rightCycleStep = 0;
    //belowBitStep = 0;
    //leftBitStep = 0;
    pinHistory = 0;
    inLeftRaw = 0b11101111;
    inLeftData = 0;
    inRightRaw = 0b11101111;
    inRightData = 0;
    PORTE &= _BV(6); // right sck high
    PORTD |= _BV(3); // assert right tx high

    PCMSK0 |= _BV(PCINT5); // enable left SCK interrupt
    EIMSK |= _BV(PCIE0);
}

u08 commStep(void)
{
    rightCycleStep = 0;
    PORTE &= ~_BV(6); // right sck low
    PORTD |= _BV(3); // assert right tx high
    while (rightCycleStep < 42)
    {
        if (0 == rightCycleStep ) // sending header
        {
            PORTD &= ~_BV(3); // assert right tx low  for 6 cycles
            inLeftRaw = RAW_RESET;
            inLeftData = 0;
        }
        if (rightCycleStep > 6 && (rightCycleStep & 1)) 
        {
            // 0 is encoded as 01
            // 1 is encoded as 10
            // no other value is valid
            // matrix is stored like this
            // 00xl llll rrrr ry00 <-- not shown
            // 00xl llll rrrr ry00
            // 00xl llll rrrr ry00
            // 00xl llll rrrr ry00
            // 00xl llll rrrr ry00
            // 00xl llll rrrr ry00
            // 00xl llll rrrr ry00
            // 00xl llll rrrr ry00
            // 00xl llll rrrr ry00 <-- not shown
            // x = rightmost column of left-connected matrix
            // y = leftmost column of right-connected matrix
            u08 colValue = colony[(rightCycleStep - 6) >> 2] >> 3;
            colValue &= 1;
            colValue += ((rightCycleStep >> 1) & 1);
            if (1 == colValue)
            {
                PORTD &= ~_BV(3); // assert right tx low
            }
            else
            {
                PORTD |= _BV(3); // assert right tx high
            }
        }
        if (rightCycleStep & 1)
        {
            PORTE |= _BV(6); // right sck high 
        }
        else
        {
            u08 count;
            PORTE &= ~_BV(6); //  right sck low
            // read data from left
            inLeftRaw <<= 1;
            if (PIND & _BV(PIN_RIGHTRX))
            {
                inLeftRaw |= 1;
            }
            switch (inLeftRaw)
            {
            case 0b10111101: // logical 0
                count = (inLeftData & 0xf000) >> 12;
                inLeftData <<= 1;
                count++;
                inLeftRaw = RAW_RESET;
                inLeftData &= 0x0fff;
                inLeftData |= (count << 12);
                break;

            case 0b10111110: // logical 1
                count = (inLeftData & 0xf000) >> 12;
                inLeftData <<= 1;
                inLeftData |= 1;
                count++;
                inLeftRaw = RAW_RESET;
                inLeftData &= 0x0fff;
                inLeftData |= (count << 12);
                break;
            }
        
        }
        ++rightCycleStep;
    }
    setRightColumn(inLeftData);
    PORTE |= _BV(6); // right sck high 

    //if (42 == ++rightCycleStep)
    //{
     //   rightCycleStep = 0;
    //}
    return 0;
}
