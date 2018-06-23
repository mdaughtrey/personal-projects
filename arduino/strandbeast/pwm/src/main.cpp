#include <avr/io.h>
#include <arduino.h>

#include <TinyWireS.h>

#define I2C_SLAVE_ADDRESS 0x5a

#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

volatile uint8_t i2c_regs[] =
{
    0, 0
};
// Tracks the current register pointer position
volatile uint8_t reg_position = 0;
const uint8_t reg_size = sizeof(i2c_regs);

/**
 * This is called for each read request we receive, never put more than one byte of data (with TinyWireS.send) to the 
 * send-buffer when using this callback
 */
void requestEvent()
{  
    TinyWireS.send(i2c_regs[reg_position]);
    // Increment the reg position on each read, and loop back to zero
    reg_position++;
    if (reg_position >= reg_size)
    {
        reg_position = 0;
    }
}

void receiveEvent(uint8_t howMany)
{
    if (howMany < 1)
    {
        // Sanity-check
        return;
    }

    if (howMany > TWI_RX_BUFFER_SIZE)
    {
        // Also insane number
        return;
    }

    reg_position = TinyWireS.receive();
    howMany--;
    if (!howMany)
    {
        // This write was only to set the buffer for next read
        return;
    }
    while(howMany--)
    {
        i2c_regs[reg_position] = TinyWireS.receive();
        reg_position++;
        if (reg_position >= reg_size)
        {
            reg_position = 0;
        }
    }
    OCR0B = i2c_regs[0];
    OCR1B = i2c_regs[1];
}

void setup()
{
    //TCCR0A |= 3<<COM0B0 | 3<<WGM00;
    //TCCR0A |= _BV(COM0B1) | _BV(COM0B0) | _BV(WGM01) | _BV(WGM00);
    // TCNT0 Fast PWM
    TCCR0A |= _BV(COM0B1) | _BV(COM0B0) | _BV(WGM02) | _BV(WGM01) | _BV(WGM00);;
//    TCCR0B |= 0<<WGM02 | 3<<CS00;
    //TCCR0B |= _BV(CS00) | _BV(WGM01) | _BV(WGM00);
    TCCR0B  = _BV(CS00); // | _BV(WGM02);
    GTCCR |= 1<<PWM1B | 3<<COM1B0;
    //TCCR1 |= 3<<COM1A0 | 7 << CS10;
    TCCR1 |= 3<<COM1A0 | 0 << CS10;
//    PLLCSR |= _BV(PLLE) | _BV(PCKE);

//    TCCR0A |= _BV(COM0B0) | _BV(WGM00) | _BV(WGM01);
////    TCCR0B |= 0<<WGM02 | 3<<CS00;
////    0000 0001
//    TCCR0B = 1;
//    //GTCCR |= _BV(PWM1B) | _BV(COM1B0);
//    TCCR1 |= _BV(COM1A0) | _BV(CS10);
    DDRB |= _BV(1) | _BV(4);
    OCR0B = 0x00;
    OCR1B = 0x00;

    // 2Wire
    TinyWireS.begin(I2C_SLAVE_ADDRESS);
    TinyWireS.onReceive(receiveEvent);
    TinyWireS.onRequest(requestEvent);
}

void loop()
{
    TinyWireS_stop_check();
//    OCR0B++;
//    OCR1B++;
//    tws_delay(400);
}

