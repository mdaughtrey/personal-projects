#include <avr/io.h>
#include <avr/interrupt.h>
#include <HardwareSerial.h>

#define SHIFTER_INIT 0x03 << 4
typedef unsigned char u08;
typedef unsigned int u16;

u08 shifter;
u08 ocr2a;

ISR(TIMER2_OVF_vect)
{
    if (shifter & 0x01)
    {
        PORTB |= _BV(3);
    }
    shifter >>= 1;
    if (0 == shifter)
    {
        shifter = SHIFTER_INIT;
    }
}

ISR(TIMER2_COMPA_vect)
{
    if (shifter & 0x01)
    {
        PORTB &= ~_BV(3);
    }
    shifter >>= 1;
    if (0 == shifter)
    {
        shifter = SHIFTER_INIT;
    }
}

int main()
{
    Serial.begin(57600);
    Serial.println("Init");
    ocr2a = 0x80;

    cli();
//    TCCR2A |= _BV(COM2A1) | _BV(COM2A0); // set OC2A on compare match, 'normal' mode
//    TCCR2A |= _BV(COM2A0); // toggle OC2A on compare match, 'normal' mode
    TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20); // prescaler 1024
    TIFR2 &= ~_BV(TOV2); // clear overflow interrupt flag
    TIFR2 &= ~_BV(OCF2A); // clear compare A interrupt flag
    TIMSK2 |= _BV(OCIE2A) | _BV(TOIE2); // enable counter2 overflow interrupt
    OCR2A = ocr2a;
    TCNT2 = 0x00;
    DDRB |= _BV(3) | _BV(5);
    shifter = SHIFTER_INIT;
    sei();

    while (1)
    {
        if (!Serial.available())
        {
            continue;
        }

        switch (Serial.read())
        {
            case 'j': ocr2a--; break;
            case 'k': ocr2a++; break;
        }
        Serial.print(ocr2a, 10);
        Serial.print(" ");
        cli(); OCR2A = ocr2a; sei(); 
    }
}
