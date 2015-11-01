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
#define SERVO_NEXTFRAME 1000
#define SERVO_STOP 1200
#define SENSOR_VALUE_INIT 128

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

u08 sensorValue(SENSOR_VALUE_INIT);
u08 waitingFor(NONE);

void loop ()
{
    u16 mils = millis();

    if (sensorTimer)
    {
        u08 sData = PINC & _BV(PC_LEDSENSOR);
        if (sData && (sensorValue < 255))
        {
            sensorValue++;
        }
        if (!sData && (sensorValue > 0))
        {
            sensorValue--;
        }
    }
    if ((mils - servoTimer) > 100)
    {
        SERVO_ON;
        delayMicroseconds(servoPulse);
        SERVO_OFF;
        servoTimer = mils;
    }
    switch (waitingFor)
    {
        case NONE:
//            Serial.print("N");
            break;

        case ENDNEXTFRAME:
            if (255 == sensorValue)
            {
                LASER_OFF;
                servoPulse = SERVO_STOP;
                waitingFor--;
                sensorTimer = 0;
                sensorValue = SENSOR_VALUE_INIT;
            }
            break;
            
        case STARTNEXTFRAME:
            LASER_ON;
            sensorTimer = 1;
            motorPulse = MOTOR_PRETENSION_SLOW;
            analogWrite(PIN_MOTOR, motorPulse);
            servoPulse = SERVO_NEXTFRAME;
            if (0 == sensorValue)
            {
                waitingFor--;
                sensorValue = SENSOR_VALUE_INIT;
            }
            break;

        case SHUTTERCLOSED:
//            Serial.print("C");
            // camera control
            SHUTTER_CLOSE;
            delay(100);
            waitingFor--;
            break;

        case EXPOSURESERIES:
    //        Serial.print("X");
            delay(200);
            LAMP_ON;
            delay(2);
            LAMP_OFF;
            delay(400);
            LAMP_ON;
            delay(10);
            LAMP_OFF;
            delay(400);
            LAMP_ON;
            delay(20);
            LAMP_OFF;
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
            break;

        case 'C':
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
