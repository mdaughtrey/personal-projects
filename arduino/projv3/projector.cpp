#include <WProgram.h>
#include <avrlibtypes.h>
//#include <avr/interrupt.h>

#pragma GCC diagnostic ignored "-Wwrite-strings"

#define PB_SERVO 3
#define PB_LAMP 1
#define PIN_MOTOR 10  // PB2
#define PB_LASER 0
#define PIN_LEDSENSOR 2 // PC2
#define PC_LEDSENSOR 2 // PC2
#define PB_SHUTTER 4

#define MOTOR_PRETENSION_SLOW 179
#define MOTOR_PRETENSION_FF 220
#define MOTOR_REWIND 180 
#define SERVO_MIN 14
#define SERVO_STOP 22
#define SERVO_MAX 30
#define SERVO_NEXTFRAME_FAST 16 
#define SERVO_NEXTFRAME_SLOW 18 
#define SERVO_NEXTFRAME_REVERSE 25
#define SENSOR_VALUE_INIT 128
#define SENSOR_VALUE_MAX 192
#define SENSOR_VALUE_MIN 64

#define LAMP_OFF PORTB |= _BV(PB_LAMP)
#define LAMP_ON PORTB &= ~_BV(PB_LAMP)
#define SERVO_ON PORTB |= _BV(PB_SERVO)
#define SERVO_OFF PORTB &= ~_BV(PB_SERVO)
#define LASER_ON PORTB |= _BV(PB_LASER);
#define LASER_OFF PORTB &= ~_BV(PB_LASER);
#define SHUTTER_OPEN PORTB &= ~_BV(PB_SHUTTER)
#define SHUTTER_CLOSE PORTB |= _BV(PB_SHUTTER)

typedef enum
{ 
        NONE = 0,
        FINETUNEGAPEND,
        LOOKFORGAPEND,
        LOOKFORGAPSTART,
        LOOKFORFRAMEEND,
        SHUTTERCLOSED,
        EXPOSURESERIES,
        SHUTTEROPEN,
        TRIPLESTART = SHUTTEROPEN
} WaitFor;

u16 sensorTimer = 0;
u08 servoPulse = SERVO_STOP;
u08 motorPulse = 255;


ISR(TIMER2_OVF_vect)
{
        SERVO_ON;
}

ISR(TIMER2_COMPA_vect)
{
    SERVO_OFF;
}

void incMotor()
{
    motorPulse += 5;
    analogWrite(PIN_MOTOR, motorPulse);
//    Serial.print("Motor ");
//    Serial.println(motorPulse,10);
}

void decMotor()
{
    motorPulse -= 5;
    analogWrite(PIN_MOTOR, motorPulse);
//    Serial.print("Motor ");
//    Serial.println(motorPulse,10);
}

void setMotor(u08 set)
{
    motorPulse = set;
    analogWrite(PIN_MOTOR, motorPulse);
//    Serial.print("Motor ");
//    Serial.println(motorPulse,10);
}

void incServo()
{
    if (servoPulse >= SERVO_MAX)
    {
        return;
    }
    servoPulse++;
    OCR2A = servoPulse;
//    Serial.print("Servo ");
//    Serial.println(servoPulse, 10);
}

void decServo()
{
    if (servoPulse <= SERVO_MIN)
    {
        return;
    }
    servoPulse--;
    OCR2A = servoPulse;
//    Serial.print("Servo ");
//    Serial.println(servoPulse, 10);
}

void setServo(u08 set)
{
    if (set < SERVO_MIN || set > SERVO_MAX)
    {
        return;
    }
    servoPulse = set;
    OCR2A = servoPulse;
//    Serial.print("Servo ");
//    Serial.println(servoPulse, 10);
}

void setup ()
{ 
    Serial.begin(57600);
    Serial.println("Init Start");
    PORTB |= _BV(PB_SHUTTER);
    DDRB |= _BV(PB_LAMP) | _BV(PB_LASER) | _BV(PB_SHUTTER);
    LAMP_OFF;


    cli();
    TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20); // prescaler 1024
    TIFR2 &= ~_BV(TOV2); // clear overflow interrupt flag
    TIFR2 &= ~_BV(OCF2A); // clear compare A interrupt flag
    TIMSK2 |= _BV(OCIE2A) | _BV(TOIE2); // enable counter2 overflow interrupt
    OCR2A = SERVO_STOP;
    servoPulse = SERVO_STOP;
//    setServo(SERVO_STOP);
    TCNT2 = 0x00;
    DDRB |= _BV(PB_SERVO);
    //shifter = SHIFTER_INIT;
    sei();
    analogWrite(PIN_MOTOR, motorPulse);

    Serial.println("Init OK");
}

u08 sensorValue(SENSOR_VALUE_INIT);
u08 waitingFor(NONE);

u16 ledCount;

void loop ()
{
    u16 mils = millis();

    if (sensorTimer)
    {
        ledCount++;
        u08 sData = PINC & _BV(PC_LEDSENSOR);
        if (sData && (sensorValue < SENSOR_VALUE_MAX))
        {
            sensorValue++;
        }
        if (!sData && (sensorValue > SENSOR_VALUE_MIN))
        {
            sensorValue--;
        }
    }

    switch (waitingFor)
    {
        case NONE:
//            Serial.print("N");
            break;

        case FINETUNEGAPEND:
//            if (SENSOR_VALUE_MIN != sensorValue)
//            {
//                break;
//            }
//            LASER_OFF;
//            setServo(SERVO_STOP);
            waitingFor--;
//            sensorTimer = 0;
//            sensorValue = SENSOR_VALUE_INIT;
            break;

        case LOOKFORGAPEND:
            if (SENSOR_VALUE_MAX != sensorValue)
            {
                break;
            }
            if (ledCount < 0)
            {
                break;
            }
            LASER_OFF;
            setServo(SERVO_STOP);
            sensorTimer = 0;
            waitingFor--;
            sensorValue = SENSOR_VALUE_INIT;
            Serial.println(ledCount, 10);
            //setServo(SERVO_NEXTFRAME_REVERSE);
            delay(200);
            break;

        case LOOKFORGAPSTART:
            setServo(SERVO_NEXTFRAME_SLOW);
            if (SENSOR_VALUE_MIN == sensorValue)
            {
                waitingFor--;
                sensorValue = SENSOR_VALUE_MAX - 20;
            }
            break;

        case LOOKFORFRAMEEND:
            LASER_ON;
            sensorTimer = 1;
            setMotor(MOTOR_PRETENSION_SLOW);
            setServo(SERVO_NEXTFRAME_FAST);
            if (SENSOR_VALUE_MAX == sensorValue)
            {
                waitingFor--;
                sensorValue = SENSOR_VALUE_INIT;
            }
            break;

        case SHUTTERCLOSED:
//            Serial.print("C");
            // camera control
            SHUTTER_CLOSE;
            delay(200);
            waitingFor--;
            break;

        case EXPOSURESERIES:
            delay(400);
            LAMP_ON;
            delay(4);
            LAMP_OFF;
//            delay(800);
            delay(350);
            LAMP_ON;
            delay(10);
            LAMP_OFF;
            delay(260);
            LAMP_ON;
            delay(60);
            LAMP_OFF;
            delay(120);
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
            delay(20);
            SHUTTER_CLOSE;
            break;

        case 'C':
            SHUTTER_CLOSE;
            break;

        case 'x':
            Serial.println("Reset");
            setServo(SERVO_STOP);
            LAMP_OFF;
            LASER_OFF;
            SHUTTER_CLOSE;
            setMotor(255);
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
            decServo();
            break;

        case 'S':
            incServo();
            break;

        case 'M':
            incMotor();
            break;

        case 'm':
            decMotor();
            break;

        case 't': // pretension
            setServo(SERVO_STOP);
            setMotor(MOTOR_PRETENSION_SLOW);
            break;

        case 'f': // forward
            setMotor(MOTOR_PRETENSION_FF);
            setServo(SERVO_MIN);
            break;

        case 'r': // rewind
            setMotor(MOTOR_REWIND);
            setServo(SERVO_STOP);
            break;

        case 'n': // next frame
            waitingFor = LOOKFORFRAMEEND;
            sensorValue = SENSOR_VALUE_INIT;
            ledCount = 0;
            break;

        case 'v': // verbose
            Serial.print("Servo ");
            Serial.print(servoPulse, 10);
            Serial.print(" Motor ");
            Serial.print(motorPulse, 10);
            break;

        case '3':
            waitingFor = TRIPLESTART;
            break;

    }
    return;
}
