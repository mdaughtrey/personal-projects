#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <serutil.h>
//#include <testmode.h>

// pc1 = ir
// pc2 = lamp
// pc3 = camera

#define STOP 0
#define FORWARD 2
#define NEXT 3

/*
"aA: advance 1 count\r\n"
"fF: free run forward\r\n"
"nN: next frame\r\n"
"cC: camera shutter (2500ms pulse)\r\n"
"l : lamp on\r\n"
"L : lamp off\r\n";
*/


#define AC_MOTOR_ON PORTF |= 0x10
#define AC_MOTOR_OFF PORTF &= ~0x10

//unsigned char motorIsOn = 0;

//unsigned char ledTestBit = 1;
//unsigned char ledControl = 0;

// led 1 = pf1
// led 2 = pf2
// led 3 = pf3
// led 4 = pf5
// led 5 = pf6

#define LED_IR_ON      PORTF &= ~_BV(1)
#define LED_CAMERA_ON  PORTF &= ~_BV(2)
#define LED_LAMP_ON    PORTF &= ~_BV(3) 
#define LED_MOTOR_ON   PORTF &= ~_BV(5)
#define LED_SERIAL_ON  PORTF &= ~_BV(6)

#define LED_IR_OFF      PORTF |= _BV(1)
#define LED_CAMERA_OFF  PORTF |= _BV(2)
#define LED_LAMP_OFF    PORTF |= _BV(3) 
#define LED_MOTOR_OFF   PORTF |= _BV(5)
#define LED_SERIAL_OFF  PORTF |= _BV(6)

#define LED_ALL_ON      PORTF &= ~0x6e
#define LED_ALL_OFF     PORTF |= 0x6e
//#define LEDBIT_IR     _BV(LED_IR)
//#define LEDBIT_MOTOR  _BV(LED_MOTOR)
//#define LEDBIT_CAMERA _BV(LED_CAMERA)
//#define LEDBIT_LAMP   _BV(LED_LAMP)

void delay20ms(unsigned char count)
{
    while (count--)
    {
        _delay_ms(20);
    }
}

//void ledIlluminate(unsigned char which)
//{
//    switch (which)
//    {
//        case 0:
//            PORTF |= 0x6e;
//            break;
//
//        default:
//            PORTF &= ~_BV(which);
//            break;
//    }
//}

void ledInit(void)
{
	DDRF = 0x7e; // 0110 1110  bit 4 is ac motor control

    LED_ALL_OFF;
	LED_IR_ON;
    delay20ms(25);
LED_CAMERA_ON;
    delay20ms(25);
LED_LAMP_ON;
    delay20ms(50);
    LED_ALL_OFF;

}

void ledError(void)
{
  unsigned char count;
  for (count = 0; count < 5; count++)
{
	LED_ALL_ON;
        delay20ms(5);
	LED_ALL_OFF;
        delay20ms(5);
}
}

void motorOn(void)
{
//    if (!motorIsOn)
//    {
LED_MOTOR_ON;
        AC_MOTOR_ON;
//    }
//    motorIsOn = 1;
}

void motorOff(void)
{
 //   if (motorIsOn)
  //  {
LED_MOTOR_OFF;
        AC_MOTOR_OFF;
 //   }
 //   motorIsOn = 0;
}


//void updateLedStatus(void)
//{
//
//    if (ledControl == 0)
//    {
//	ledIlluminate(0);
//	return;
//    }
//    if (ledControl & _BV(ledTestBit))
//    {
//        ledIlluminate(0);
//        ledIlluminate(ledTestBit);
//    }
//    ledTestBit++;
//    if (ledTestBit == 5)
//    {
//	    ledTestBit = 1;
//    }
//}
//
void testMain(void)
{
    DDRA |= _BV(6);
//    DDRC = _BV(3) | _BV(2);
    DDRC |= _BV(3); // camera
    while (1)
    {
        delay20ms(25);
        PORTA |= _BV(6);
        PORTC |= _BV(3); // camera
        delay20ms(25);
        PORTA &= ~_BV(6);
        PORTC &= ~_BV(3); // camera
//        PORTC |= _BV(2);
//        _delay_ms(5);
//        PORTC &= ~_BV(2);
    }
}


int main(void)
{
    //testMain();
    //PORTC = _BV(2);
    DDRC = _BV(3) | _BV(2); // camera + lamp
    PORTC |= _BV(3); // lamp off
    PORTC &= ~_BV(2); // camera 0ff
    DDRF = 0x10; // Motor control
    uart_init();
//    sei(); // need for serial
    ledInit();
    motorOff();

    char direction = STOP;
    unsigned char irThreshold = 50;	
    unsigned char irData = 0;	
    char advanceCount = 0;
//    unsigned char bit;


    while (1)
    {
//        updateLedStatus();

        u16 data = uart_get_buffered(); 
        if (0x0100 & data)
        {
            LED_SERIAL_ON;
            data &= 0xff;
            switch (data)
            {
                case 'a':
                case 'A':
						direction = NEXT;
                    advanceCount = 1;
                    break;

                case 'f':
                case 'F':
                    direction = FORWARD;
                    break;

                case 'N':
                case 'n':
                    direction = NEXT;
                    advanceCount = 3;
                    break;

                case 'C':
                case 'c':
                    PORTC &= ~_BV(3);
		    LED_CAMERA_ON;
                    _delay_ms(250);
		    LED_CAMERA_OFF;	
                    PORTC |= _BV(3);
                    break;

                case 'l':
                    // lampOn
                    LED_LAMP_ON;
                    PORTC |= _BV(2);
                    break;

                case 'L':
                    // lampOff
                    LED_LAMP_OFF;
                    PORTC &= ~_BV(2);
                    break;

		case 's':
		case 'S':
                    motorOff();
	            direction = STOP;
			break;

                case 'x':
                case 'X':
                    motorOff();
	            direction = STOP;
                    PORTC &= ~_BV(2);
                    PORTC |= _BV(3);
	            ledInit();
                    break;

		case '\n':
		case '\r':
			break;

                 default:
		    ledError();
                    //uart_init();
		    break;
            }
        }
        else
        {
	    LED_SERIAL_OFF;
        }
        if (direction == FORWARD || direction == NEXT)
        {
            motorOn();
        }

	if (PINC & _BV(1))
	{
		LED_IR_ON;
		if (irThreshold)
			irThreshold--;
	}
	else
	{
	        LED_IR_OFF;
		if (irThreshold < 100)
			irThreshold++;
	}
	if (irThreshold == 0)
        {
		irData <<= 1;
        }
	if (irThreshold == 100)
        {
		irData <<= 1;
		irData |= 1;
        }
	//if (irData == 0xf0
	if (irData == 0x0f
		&& direction == NEXT)
	{
            if (--advanceCount <= 0)
            {
                direction = STOP;
                motorOff();
            }
        }
    }
    return 0;
}
