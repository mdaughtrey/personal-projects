#include <WProgram.h>
#include <avrlibtypes.h>

#pragma GCC diagnostic ignored "-Wwrite-strings"

#define PB_SERVO 3
#define PB_LAMP 1
#define PIN_MOTOR 10  // PB2
#define MOTOR
#define PB_LASER 0
#define PIN_LEDSENSOR 2

#define MOTOR_PRETENSION_SLOW 180
#define MOTOR_PRETENSION_FF 220
#define MOTOR_REWIND 195 

#define LAMP_OFF PORTB |= _BV(PB_LAMP)
#define LAMP_ON PORTB &= ~_BV(PB_LAMP)
#define SERVO_ON PORTB |= _BV(PB_SERVO);
#define SERVO_OFF PORTB &= ~_BV(PB_SERVO);
#define LASER_ON PORTB |= _BV(PB_LASER);
#define LASER_OFF PORTB &= ~_BV(PB_LASER);


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
    DDRB |= _BV(PB_SERVO) | _BV(PB_LAMP) | _BV(PB_LASER);
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
u08 waitingFor(0);
u08 showSensorValue = 0;
u16 sensorRaw[4] = { 0 };
void loop ()
{
    switch (waitingFor)
    {
        case NONE:
            break;

        case ENDNEXTFRAME:
            if (0x01 == (sensorValue & 0x03))
            {
                LASER_OFF;
                servoPulse = 1500;
                waitingFor--;
                sensorTimer = 0;
            }
            break;
            
        case STARTNEXTFRAME:
            LASER_ON;
            delay(10);
            //sensorValue = 0;
            sensorTimer = millis();
            motorPulse = MOTOR_PRETENSION_SLOW;
            servoPulse = 1200;
            waitingFor--;
            break;

        case SHUTTERCLOSED:
            // camera control
            waitingFor--;
            break;

        case EXPOSURESERIES:
            delay(130);
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
            waitingFor--;
            break;

        default:
            break;
    }
    u16 mils = millis();

    if ((mils - servoTimer) > 100)
    {
        SERVO_ON;
        delayMicroseconds(servoPulse);
        SERVO_OFF;
        servoTimer = mils;
        analogWrite(PIN_MOTOR, motorPulse);
    }
    if (sensorTimer && (mils - sensorTimer) > 10)
    {
        sensorRaw[3] = sensorRaw[2];
        sensorRaw[2] = sensorRaw[1];
        sensorRaw[1] = sensorRaw[0];
        sensorRaw[0] = analogRead(PIN_LEDSENSOR);
        u16 average = (sensorRaw[3] + sensorRaw[2] + sensorRaw[1] + sensorRaw[0]) / 4;

        if (average)
        {
        Serial.print(average, 10);
        Serial.print(" ");
        }
        if ((average > 700) && !(sensorValue & 0x0001))
        {
            sensorValue <<= 1; 
            sensorValue |= 1;
        }    
        if ((average <= 700) && (sensorValue & 0x0001))
        {
            sensorValue <<= 1; 
        }
        sensorTimer = mils;
    }

    if (!Serial.available())
    {
        return;
    }

    switch (Serial.read())
    {
        case 'v':
            showSensorValue = !showSensorValue;
            break;

        case 'x':
            servoPulse = 1500;
            LAMP_OFF;
            LASER_OFF;
            motorPulse = 255;
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
#ifdef MOTOR
            Serial.print("Motor ");
            Serial.println(motorPulse,10);
#endif
            break;

        case 't': // pretension
            motorPulse = MOTOR_PRETENSION_SLOW;
            break;

        case 'f': // forward
            motorPulse = MOTOR_PRETENSION_FF;
            servoPulse = 1000;
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
