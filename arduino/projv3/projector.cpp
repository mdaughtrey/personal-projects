#include <WProgram.h>
#include <avrlibtypes.h>
//#include <avr/interrupt.h>

#pragma GCC diagnostic ignored "-Wwrite-strings"

#define PB_SERVO 3
#define PB_LAMP 1
#define PIN_MOTOR 10  // PB2
#define MOTOR
#define PB_LASER 0
#define PIN_LEDSENSOR 2 // PC2
#define PC_LEDSENSOR 2 // PC2
#define PB_SHUTTER 4

#define MOTOR_PRETENSION_SLOW 180
#define MOTOR_PRETENSION_FF 220
#define MOTOR_REWIND 195 

#define LAMP_OFF PORTB |= _BV(PB_LAMP)
#define LAMP_ON PORTB &= ~_BV(PB_LAMP)
#define SERVO_ON PORTB |= _BV(PB_SERVO);
#define SERVO_OFF PORTB &= ~_BV(PB_SERVO);
#define LASER_ON PORTB |= _BV(PB_LASER);
#define LASER_OFF PORTB &= ~_BV(PB_LASER);
#define SHUTTER_OPEN PORTB &= ~_BV(PB_SHUTTER)
#define SHUTTER_CLOSE PORTB |= _BV(PB_SHUTTER)

typedef enum
{ 
        NONE = 0,
        ENDNEXTFRAME,
        STARTNEXTFRAME,
        SHUTTERCLOSED,
        EXPOSURESERIES,
        SHUTTEROPEN,
        TRIPLESTART = SHUTTEROPEN
} WaitFor;

u16 servoTimer = 0;
u16 sensorTimer = 0;
u16 servoPulse = 1500;
u16 motorPulse = 255;

void setup ()
{ 
    PORTB |= _BV(PB_SHUTTER);
    DDRB |= _BV(PB_SERVO) | _BV(PB_LAMP) | _BV(PB_LASER) | _BV(PB_SHUTTER);
    LAMP_OFF;
    SERVO_OFF;
#ifdef MOTOR
    analogWrite(PIN_MOTOR, motorPulse);
#endif //MOTOR
    Serial.begin(57600);
    servoTimer = millis();
    Serial.println("Init OK");
}

u08 sensorValue = 0;
u08 waitingFor(NONE);

void loop ()
{
    u16 mils = millis();

    if ((mils - servoTimer) > 100)
    {
        SERVO_ON;
        delayMicroseconds(servoPulse);
        SERVO_OFF;
        servoTimer = mils;
//        analogWrite(PIN_MOTOR, motorPulse);
    }
    if (sensorTimer && (mils - sensorTimer) > 10)
    {
        sensorValue |= (PINC & _BV(PC_LEDSENSOR));
        sensorValue <<= 1;

#if 0
        if (sensorValue & 0x01)
        {
            if (!(PINC & _BV(PC_LEDSENSOR)))
            {
                sensorValue <<= 1;
            }
        }
        else
        {
            if (PINC & _BV(PC_LEDSENSOR))
            {
                sensorValue <<= 1;
                sensorValue |= 1;
            }
        }
#endif // 0
        sensorTimer = mils;
    }
    switch (waitingFor)
    {
        case NONE:
//            Serial.print("N");
            break;

        case ENDNEXTFRAME:
//            Serial.print(sensorValue, 2);
//            Serial.print(" ");
            //if (0x03 == (sensorValue & 0x07))
            if (0xf0 == sensorValue)
            {
                LASER_OFF;
                servoPulse = 1500;
                waitingFor--;
                sensorTimer = 0;
                sensorValue = 0;
            }
            break;
            
        case STARTNEXTFRAME:
//            Serial.print("S");
            LASER_ON;
            delay(10);
            //sensorValue = 0;
            sensorTimer = millis();
            motorPulse = MOTOR_PRETENSION_SLOW;
            servoPulse = 1200;
            waitingFor--;
            break;

        case SHUTTERCLOSED:
//            Serial.print("C");
            // camera control
            SHUTTER_CLOSE;
            waitingFor--;
            break;

        case EXPOSURESERIES:
    //        Serial.print("X");
            delay(131);
            LAMP_ON;
            delay(2);
            LAMP_OFF;
            delay(199);
            delay(130);
            LAMP_ON;
            delay(20);
            LAMP_OFF;
            delay(180);
            delay(130);
            LAMP_ON;
            delay(50);
            LAMP_OFF;
            delay(150);
            waitingFor--;
            break;

        case SHUTTEROPEN:
            SHUTTER_OPEN;
            waitingFor--;
            break;

        default:
            break;
    }

    if (!Serial.available())
    {
        return;
    }

    switch (Serial.read())
    {
        case 'c':
            SHUTTER_OPEN;
            delay(50);
            SHUTTER_CLOSE;
            break;

        case 'x':
            servoPulse = 1500;
            LAMP_OFF;
            LASER_OFF;
            SHUTTER_CLOSE;
            motorPulse = 255;
            analogWrite(PIN_MOTOR, motorPulse);
            break;

        case 'l':
            LAMP_ON;
            break;

        case 'L':
            LAMP_OFF;
            break;

        case 'e':
            LASER_ON;
            break;

        case 'E':
            LASER_OFF;
            break;

        case 's':
            if (servoPulse > 1000)
            {
                servoPulse -= 10;
            }
            Serial.print("Servo ");
            Serial.println(servoPulse, 10);
            break;

        case 'S':
            if (servoPulse < 2000)
            {
                servoPulse += 10;
            }
            Serial.print("Servo ");
            Serial.println(servoPulse, 10);
            break;

        case 'M':
            if (motorPulse < 255)
            {
                motorPulse += 5;
            }
            analogWrite(PIN_MOTOR, motorPulse);
#ifdef MOTOR
            Serial.print("Motor ");
            Serial.println(motorPulse, 10);
#endif
            break;

        case 'm':
            if (motorPulse > 0)
            {
                motorPulse -= 5;
            }
            analogWrite(PIN_MOTOR, motorPulse);
#ifdef MOTOR
            Serial.print("Motor ");
            Serial.println(motorPulse,10);
#endif
            break;

        case 't': // pretension
            motorPulse = MOTOR_PRETENSION_SLOW;
            analogWrite(PIN_MOTOR, motorPulse);
            break;

        case 'f': // forward
            motorPulse = MOTOR_PRETENSION_FF;
            servoPulse = 1000;
            analogWrite(PIN_MOTOR, motorPulse);
            break;

        case 'r': // rewind
            motorPulse = MOTOR_REWIND;
            servoPulse = 2000;
            break;

        case 'n': // next frame
            waitingFor = STARTNEXTFRAME;
            break;

        case '3':
            waitingFor = TRIPLESTART;
            break;

    }
    return;
}
