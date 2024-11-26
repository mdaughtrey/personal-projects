#include <avr/io.h>
#include <arduino.h>

#include <TinyWireS.h>

#define I2C_SLAVE_ADDRESS 0x5b

#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE ( 16 )
#endif

//volatile uint8_t i2c_regs[] =
//{
//    0, 0
//};
// Tracks the current register pointer position
//volatile uint8_t reg_position = 0;
//kconst uint8_t reg_size = sizeof(i2c_regs);

/**
 * This is called for each read request we receive, never put more than one byte of data (with TinyWireS.send) to the 
 * send-buffer when using this callback
 */
//void requestEvent()
//{  
//    TinyWireS.send(i2c_regs[reg_position]);
//    // Increment the reg position on each read, and loop back to zero
//    reg_position++;
//    if (reg_position >= reg_size)
//    {
//        reg_position = 0;
//    }
//}

void receiveEvent(uint8_t howMany)
{
    OCR0B = 0x7f;
    OCR1B = 0x7f;
    if (howMany == 2)
    {
        // PWM 0
//        uint8_t val = TinyWireS.receive();
//        if ((0 == val) & OCR0B)
//        {
//            // turn pwm 0b off, pin high output
//            TCCR0A &= ~(_BV(COM0B1) | _BV(COM0B0));
//            DDRB |= _BV(1);
//            PORTB |= _BV(1);
//        }
//        else if (val & (0 == OCR0B))
//        {
//            // turn pwm 0b on
//            TCCR0A |= _BV(COM0B1) | _BV(COM0B0);
//        }
//        OCR0B = val;
        OCR0B = TinyWireS.receive();

        // PWM 1
//        val = TinyWireS.receive();
//        if ((0 == val) & OCR1B)
//        {
//            // turn pwm 1b off, pin high output
//            GTCCR &= ~_BV(PWM1B);
//            PORTB |= _BV(4);
//        }
//        else if (val & (0 == OCR1B))
//        {
//            // turn pwm 1b on
//            GTCCR &= _BV(PWM1B);
//        }
        OCR1B = TinyWireS.receive();
    }
    else while(howMany--)
    {
        TinyWireS.receive();
    }

//    if (howMany > TWI_RX_BUFFER_SIZE)
//    {
//        // Also insane number
//        return;
//    }

//    howMany--;
//    if (!howMany)
//    {
//        // This write was only to set the buffer for next read
//        return;
//    }
//    while(howMany--)
//    {
//        i2c_regs[reg_position] = TinyWireS.receive();
//        reg_position++;
//        if (reg_position >= reg_size)
//        {
//            reg_position = 0;
//        }
//    }
//    OCR0B = i2c_regs[0];
//    OCR1B = i2c_regs[1];
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
    //OCR0B = 0x7f;
    //OCR1B = 0x10;
    OCR0B = 0xff;
    OCR1B = 0xff;

    // 2Wire
    TinyWireS.begin(I2C_SLAVE_ADDRESS);
    TinyWireS.onReceive(receiveEvent);
//    TinyWireS.onRequest(requestEvent);
}

void loop()
{
    TinyWireS_stop_check();
//    OCR0B++;
//    OCR1B++;
//    tws_delay(400);
}

