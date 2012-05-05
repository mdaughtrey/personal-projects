#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <serutil.h>
#include <dmc16207.h>
//#include <testmode.h>

// pc1 = ir
// pc2 = lamp
// pc3 = camera

#define STOP 0
#define BACK 1
#define FORWARD 2
#define NEXT 3
#define PREVIOUS 4

const char msgInit[] PROGMEM = "Init\r\n";
const char helpMsg[] PROGMEM = 
"aA: advance 1 count\r\n"
"fF: free run forward\r\n"
"rR: free run reverse\r\n"
"nN: next frame\r\n"
"pP: previous frame\r\n"
"cC: camera shutter (100ms pulse)\r\n"
"l : lamp on\r\n"
"L : lamp off\r\n";

int main(void)
{
        lcdInit();
        lcdStringPtr(msgInit);
        uart_init();
	uart_send_sync(msgInit, sizeof(msgInit));
	uart_send_sync(helpMsg, sizeof(helpMsg));
        stepperInit();
        sei();
        DDRC=0x6c;
	PORTC |= _BV(3);
        int pause = 2000;
        char direction = STOP;
	unsigned char irData = 0;
	unsigned advanceCount = 0;

        while (1)
        {

                u16 data = uart_get_buffered(); 
                if (0x0100 & data)
                {
                        data &= 0xff;
                        uart_send_char(data);
                        switch (data)
                        {
				case '?':
					uart_send_sync(helpMsg, sizeof(helpMsg));
					break;

				case 'a':
				case 'A':
					direction = NEXT;
					advanceCount = 1;
					break;

                                case 'f':
                                case 'F':
                                        direction = FORWARD;
                                        break;

                                case 'r':
                                case 'R':
                                        direction = BACK;
                                        break;
				case 'N':
				case 'n':
					direction = NEXT;
					advanceCount = 3;
					break;

				case 'P':
				case 'p':
					advanceCount = 3;
					direction = PREVIOUS;
					break;

				case 'C':
				case 'c':
					// cameraOff
					PORTC &= ~_BV(3);
					_delay_ms(100);
					// cameraOn
					PORTC |= _BV(3);
					break;

				case 'l':
					// lampOn
					PORTC |= _BV(2);
					break;

				case 'L':
					// lampOff
					PORTC &= ~_BV(2);
					break;
                        }
                        if (data >= '4' && data <='9')
                        {
                            pause = (data - '0') * 300 + 10;
                        }
			switch (data)
			{
				case '3': pause = 950; break;
				case '2': pause = 1100; break;
				case '1': pause = 1000; break;
				case '0':
					direction = STOP;
					stepperInit();
					break;
			}
                }
                if (direction == FORWARD || direction == NEXT)
		{
                    stepper1Forward();
		}
                else if (direction == BACK || direction == PREVIOUS)
		{
                    stepper1Back();
		}

		irData <<= 1;
		irData |= (PINC & _BV(1)) >> 1;
		if (0xf0 == irData && direction == NEXT || direction == PREVIOUS)
		{
			if (--advanceCount <= 0)
			{
				direction = STOP;
				stepperInit();
				irData = 0;
			}
		}
                _delay_us(pause);
        }
        return 0;
}
