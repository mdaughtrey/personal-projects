#include <avr/io.h>
#include <arduino.h>
#define TW
#ifdef TW
#include <TinyWireS.h>
#endif

const unsigned char I2C_SLAVE_ADDRESS = 0x5a;

#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

volatile uint8_t i2c_regs[] =
{
    0x00, 
    0x00
};
// Tracks the current register pointer position
volatile byte reg_position;
const byte reg_size = sizeof(i2c_regs);

/**
 * This is called for each read request we receive, never put more than one byte of data (with TinyWireS.send) to the 
 * send-buffer when using this callback
 */
#ifdef TW
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
    OCR1A = i2c_regs[0];
    OCR1B = i2c_regs[1];
}
#endif // TW

void setup()
{
    // Bump up the clock to 8MHz
    CLKPR = _BV(CLKPCE);
    CLKPR = 0;
    DDRB |= _BV(4) | _BV(1);;
    TCCR1 = _BV(PWM1A) | _BV(COM1A1) | _BV(CS10);
    GTCCR = _BV(PWM1B) | _BV(COM1B1); 
    OCR1A = i2c_regs[0];
    OCR1B = i2c_regs[1];

#ifdef TW
    // 2Wire
    TinyWireS.begin(I2C_SLAVE_ADDRESS);
    TinyWireS.onReceive(receiveEvent);
    TinyWireS.onRequest(requestEvent);
#endif // TW
}

void loop()
{
#ifdef TW
    TinyWireS_stop_check();
#endif // TW
}

