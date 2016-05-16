#include <I2C.h>

#define TWI_FREQ 100000L

void I2C::init()
{
    TWCR |= _BV(TWEN);
    // initialize twi prescaler and bit rate
    TWSR &= ~(_BV(TWPS0) | _BV(TWPS1));
    TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;

    /* twi bit rate formula from atmega128 manual pg 204
    SCL Frequency = CPU Clock Frequency / (16 + (2 * TWBR))
    note: TWBR should be 10 or higher for master mode
    It is 72 for a 16mhz Wiring board with 100kHz TWI */

    // enable twi module, acks, and twi interrupt
    TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA);
}

