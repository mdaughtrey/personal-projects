#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <serutil.h>
/*
-----------------------------------------------------------------
1. ~rst              28. pc5/adc5 : s1
2. rxd/pd0           27. pc4/adc4 : s0
3. txd/pd1           26. pc3/adc3 : e3
4. int0/pd2          25. pc2/adc2 : e2
5. int1/pd3  : vib1  24. pc1/adc1 : e1
6. t0/pd4    : tilt  23. pc0/adc0 : e0
7. vcc               22. agnd
8. gnd               21. aref
9. xtal1             20. avcc
10. xtal2            19. pb5/sck
11. t1/pd5   : s2    18. pb4/miso
12. ain0/pd6 : s3    17. pb3/mosi
13. ain1/pd7 : play  16. pb2/~ss
14. icp/pb0          15. pb1/oc1
-----------------------------------------------------------------
*/ 

const char CRLF[] PROGMEM = "\r\n";
u08 vib1;
u08 tiltDebounce;
u08 tiltActive;

ISR(INT0_vect)
{
}

ISR(INT1_vect)
{
            uart_send_char('v');
    vib1 = 1;
}

// called approx every 32ms
ISR(TIMER0_OVF_vect)
{
    if (tiltDebounce)
    {
        tiltDebounce++;
        if (7 == tiltDebounce && !tiltActive)
        {
            tiltActive = 1;
        }
    }
}

void setMemory(u08 start, u08 end)
{
    PORTC = end | ((start & 0x03) << 4);
    PORTD &= 0b10011111;
    PORTD |= (start & 0x0c) << 3;
}

const char msgInit[] PROGMEM = "Init2\r\n";
const char msgTilt[] PROGMEM = "Tilt\r\n";
int main(void)
{
    uart_init();
    uart_send_sync(msgInit, 7);

    tiltDebounce = 0;
    tiltActive = 0;

    TCNT0 = 0;          // TCNT0 0 -> 256 to rollover, ~32ms
    TCCR0 = (_BV(CS00) | _BV(CS02)); // CK/1024, ~128us
    TIMSK |= _BV(TOIE0);

    u08 tilt = 0;
    u08 count= 0;
    vib1 = 0;

    PORTD = 0b00010000; // tilt sensor tied high
    DDRD = 0b11100000;
    PORTD = 0b10000000;

    DDRC = 0b00111111;
    PORTC = 0b00000000;
    MCUCR |= ISC10;
    GIMSK |= INT1;
    sei();

	while(1)
	{
        /*
        if ((PIND & _BV(4)) && !tilt)
        {
            tiltDebounce = 1;
            tilt = 1;
        }
        else if (!(PIND & _BV(4)) && tilt)
        {
            tiltDebounce = 1;
            tilt = 0;
        }
        else if (tiltActive)
        {
            uart_send_hex_byte(count++);
            uart_send_sync(msgTilt, sizeof(msgTilt));
            tiltDebounce = 0;
            tiltActive = 0;
        }
        */
        if (vib1)
        {
            uart_send_char('v');
            uart_send_sync(CRLF, 2);
            vib1 = 0;
        }

        u16 data = uart_get_buffered(); 
        if (0x0100 & data)
        {
            data &= 0xff;
            if (data >= '0' && data <= '9')
            {
                uart_send_char('M');
                setMemory(data - '0', data - '0');
                continue;
            }
            switch (data)
            {
                case '[':
                    PORTD &= ~_BV(7); // play on 
                    break;

                case ']':
                    PORTD |= _BV(7); // play off
                    break;
            }
        }
    }
    return 0;
}

