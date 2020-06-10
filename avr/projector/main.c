#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <serutil.h>
//#include <testmode.h>

// pc0 = relay 3
// pc1 = relay 2
// pc2 = ir
// pc3 = relay 0
// pc4 = relay 1

// 0001 1011 & 0111 1000
#define INIT_PORT_DDR DDRC = 0x1b; 

#define RELAY0_ON  PORTC |= _BV(3)
#define RELAY0_OFF PORTC &= ~_BV(3)
#define RELAY1_ON  PORTC |= _BV(4)
#define RELAY1_OFF PORTC &= ~_BV(4)
//#define RELAY2_ON  PORTC |= _BV(1)
//#define RELAY2_OFF PORTC &= ~_BV(1)
//#define RELAY3_ON  PORTC |= _BV(0)
//#define RELAY3_OFF PORTC &= ~_BV(0)

#define A1_ON  PORTA |= _BV(6)
#define A1_OFF PORTA &= ~_BV(6)
#define B1_ON  PORTA |= _BV(5) 
#define B1_OFF PORTA &= ~_BV(5)
#define A2_ON  PORTA |= _BV(4) 
#define A2_OFF PORTA &= ~_BV(4)
#define B2_ON  PORTA |= _BV(3) 
#define B2_OFF PORTA &= ~_BV(3)

#define MOTOR_FWD  A1_ON; B1_OFF;
#define MOTOR_BACK B1_ON; A1_OFF;
#define MOTOR_OFF  A1_OFF; B1_OFF;

//#define LAMP_ON    A2_OFF; B2_ON;
//#define LAMP_OFF   A2_OFF; B2_OFF;

//#define MOTOR_FWD_ON   RELAY3_ON
//#define MOTOR_FWD_OFF  RELAY3_OFF
//#define MOTOR_BACK_ON  RELAY2_ON
//#define MOTOR_BACK_OFF RELAY2_OFF

#define LAMP_ON  RELAY1_ON
#define LAMP_OFF RELAY1_OFF
#define SHUTTER_ON  RELAY0_ON
#define SHUTTER_OFF RELAY0_OFF

#define STOP 0
#define REVERSE 1
#define FORWARD 2
#define NEXT 3
#define PREV 4

// aA: advance 1 count
// bB: back up 1 count
// fF: free run forward
// rR: free run backward
// sS: stop
// nN: next frame
// pP: previous frame
// cC: camera shutter (100ms pulse)
// l : lamp on
// L : lamp off

// led 1 = pf1
// led 2 = pf2
// led 3 = pf3
// led 4 = pf5
// led 5 = pf6

// 0110 1110
#define INIT_LED_DDR DDRF = 0x6e
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

void delay20ms(unsigned char count)
{
    while (count--)
    {
        _delay_ms(20);
    }
}

void motorForward(void)
{
    MOTOR_FWD;
}

void motorReverse(void)
{
    MOTOR_BACK;
}

void motorOff(void)
{
    MOTOR_OFF;
}

//void ledInit(void)
//{
//    INIT_LED_DDR;
//    LED_ALL_OFF;
//    LED_IR_ON;
//    delay20ms(25);
//    LED_CAMERA_ON;
//    delay20ms(25);
//    LED_LAMP_ON;
//    delay20ms(50);
//    LED_ALL_OFF;
//}

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

void testmain(void)
{
//    DDRE |= _BV(1);
//    while (1)
//    {
//        if (PINE & _BV(0))
//        {
//            PORTE |= _BV(1); 
//        }
//        else
//        {
//            PORTE &= ~_BV(1); 
//        }
//    }

    uart_test();


    uart_init();

    sei();
    while (1)
    {
    uart_send_buffered('a');
    delay20ms(1);
    }
    uart_send_buffered('x');
    uart_send_buffered('y');
    uart_send_buffered('z');
    while (1);
    {
        u08 data = uart_get_buffered();
        if (data & 0x0100)
        {
            uart_send_hex_byte(data & 0xff);
        }
    }

}


void main(void)
{
    //testmain();
    INIT_PORT_DDR;
    INIT_LED_DDR;
    PORTA = 0x78;
    DDRA = 0x78;
    PORTA = 0x00;
    
    uart_init();
    motorOff();
    LAMP_OFF;
    sei();

    while (1)
    {
        u08 data = uart_get_buffered();
        if (data & 0x0100)
        {
            uart_send_hex_byte(data & 0xff);
        }
    }
    char direction = STOP;
    unsigned char irThreshold = 50;	
    unsigned char irData = 0;	
    char advanceCount = 0;

    //uart_send_buffered('x');
    //uart_send_buffered('y');
    //uart_send_buffered('z');

    while (1)
    {
        u16 data = uart_get_buffered(); 
        if (0x0100 & data)
        {
            LED_SERIAL_ON;
            data &= 0xff;
            uart_send_buffered(data);
            switch (data)
            {
                case 'a':
                case 'A':
                    direction = NEXT;
                    advanceCount = 1;
                    break;

                case 'b':
                case 'B':
                    direction = PREV;
                    advanceCount = 1;
                    break;

                case 'f':
                case 'F':
                    direction = FORWARD;
                    break;

                case 'r':
                case 'R':
                    direction = REVERSE;
                    break;

                case 'P':
                case 'p':
                    direction = REVERSE;
                    advanceCount = 3;
                    break;

                case 'N':
                case 'n':
                    direction = NEXT;
                    advanceCount = 3;
                    break;

                case 'C':
                case 'c':
                    SHUTTER_ON;
                    LED_CAMERA_ON;
                    _delay_ms(250);
                    SHUTTER_OFF;
                    LED_CAMERA_OFF;	
                    break;

                case 'l':
                    LED_LAMP_ON;
		            LAMP_ON;
                    break;

                case 'L':
                    LED_LAMP_OFF;
                    LAMP_OFF;
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
                    LAMP_OFF;
                    //ledInit();
                    break;

                case '\n':
                case '\r':
                    break;

                default:
                    ledError();
                    uart_init();
                    break;
            }
        }
        else
        {
            LED_SERIAL_OFF;
        }
        if (direction == FORWARD || direction == NEXT)
        {
            motorForward();
        }
        if (direction == REVERSE || direction == PREV)
        {
            motorReverse();
        }

        if (PINC & _BV(2))
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

        if (irData == 0x0f && (NEXT == direction || PREV == direction))
        {
            if (--advanceCount <= 0)
            {
                direction = STOP;
                motorOff();
            }
        }
    }
}
