#include <WProgram.h>
#include <avrlibtypes.h>

#pragma GCC diagnostic ignored "-Wwrite-strings"

//#define NO_PRETENSION
#define STOPONGAP

#define PB_SERVO 3
#define PB_LAMP 1
#define PIN_MOTOR 10  // PB2
#define PB_LASER 0
#define PIN_LEDSENSOR 2 // PC2
#define PC_LEDSENSOR 2 // PC2
#define PB_SHUTTER 4

#define MOTOR_PRETENSION_SLOW 200
#define MOTOR_PRETENSION_INTRAFRAME 180
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
#define LASER_TIMEOUT_MS 4000
#define LAMP_TIMEOUT_MS 3000
#define NEXT_FRAME_TIMEOUT_MS 8000

#define LAMP_OFF PORTB |= _BV(PB_LAMP)
#define LAMP_ON PORTB &= ~_BV(PB_LAMP)
#define SERVO_ON PORTB |= _BV(PB_SERVO)
#define SERVO_OFF PORTB &= ~_BV(PB_SERVO)
#define LASER_ON PORTB |= _BV(PB_LASER);
#define LASER_OFF PORTB &= ~_BV(PB_LASER);
#define SHUTTER_OPEN PORTB &= ~_BV(PB_SHUTTER)
#define SHUTTER_CLOSE PORTB |= _BV(PB_SHUTTER)

#define DELAYEDSTATE(dd, ss) { stateLoop = dd/10; stateSaved = ss; waitingFor = DELAYLOOP; }

typedef enum
{ 
        NONE = 0,
        FRAMESTOP,
        LOOKFORGAPEND,
        LOOKFORGAPSTART,
        LOOKFORFRAMEEND,
        DELAYLOOP,
        SHUTTERCLOSED,
        EXPOSURESERIES5,
        EXPOSURESERIES4,
        EXPOSURESERIES3,
        EXPOSURESERIES2,
        EXPOSURESERIES1,
        EXPOSURESERIES,
        SHUTTEROPEN,
        TRIPLESTART = SHUTTEROPEN
} WaitFor;

u16 sensorTimer = 0;
u16 parameter = 0;
u08 servoPulse = SERVO_STOP;
u08 motorPulse = 255;
//u08 motorPretensionSlow = MOTOR_PRETENSION_SLOW;
u32 laserTimeout = 0;
u32 lampTimeout = 0;
u32 nextFrameTimeout = 0;
u08 sensorValue(SENSOR_VALUE_INIT);
u08 waitingFor(NONE);
u16 ledCount;
u08 stateLoop;
u08 stateSaved;

void incMotor();
void decMotor();
void setMotor(u08 set);
void incServo();
void decServo();
void setServo(u08 set);
void lampOn();
void lampOff();
void lampCheck();
u08 checkNextFrameTimeout();
void laserOn();
void laserOff();
void laserCheck();
void reset();

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
}

void decMotor()
{
    motorPulse -= 5;
    analogWrite(PIN_MOTOR, motorPulse);
}

void setMotor(u08 set)
{
    motorPulse = set;
    analogWrite(PIN_MOTOR, motorPulse);
}

void incServo()
{
    if (servoPulse >= SERVO_MAX)
    {
        return;
    }
    servoPulse++;
    OCR2A = servoPulse;
}

void decServo()
{
    if (servoPulse <= SERVO_MIN)
    {
        return;
    }
    servoPulse--;
    OCR2A = servoPulse;
}

void setServo(u08 set)
{
    if (set < SERVO_MIN || set > SERVO_MAX)
    {
        return;
    }
    servoPulse = set;
    OCR2A = servoPulse;
}

void lampOn()
{
    LAMP_ON;
    lampTimeout = millis();
}

void lampOff()
{
    LAMP_OFF;
    lampTimeout = 0;
}

void lampCheck()
{
    if (0 == lampTimeout)
    {
        return;
    }
    if ((millis() - lampTimeout) > LAMP_TIMEOUT_MS)
    {
        lampOff();
    }
}

u08 checkNextFrameTimeout()
{
    if ((millis() - nextFrameTimeout) < NEXT_FRAME_TIMEOUT_MS)
    {
        return 0;
    }
//    nextFrameTimeout = 0;
    Serial.println("Frame Timeout");
    reset();
    return 1;
}

void laserOn()
{
    LASER_ON;
    laserTimeout = millis();
}

void laserOff()
{
    LASER_OFF;
    laserTimeout = 0;
}

void laserCheck()
{
    if (0 == laserTimeout)
    {
        return;
    }
    if ((millis() - laserTimeout) > LASER_TIMEOUT_MS)
    {
        laserOff();
    }
}

void reset()
{
    Serial.println("Reset");
    setServo(SERVO_STOP);
    lampOff();
    laserOff();
    SHUTTER_CLOSE;
    setMotor(255);
    parameter = 0;
    waitingFor = NONE;
    sensorTimer = 0;
}

void setup ()
{ 
    Serial.begin(57600);
    Serial.println("Init Start");
    PORTB |= _BV(PB_SHUTTER);
    DDRB |= _BV(PB_LAMP) | _BV(PB_LASER) | _BV(PB_SHUTTER);
    lampOff();
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

void loop ()
{
    laserCheck();
    lampCheck();

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

//        case FINETUNEGAPEND:
//            if (checkNextFrameTimeout())
//            {
//                break;
//            }
//            if (SENSOR_VALUE_MIN != sensorValue)
//            {
//                break;
//            }
//            LASER_OFF;
//            setServo(SERVO_STOP);
//            waitingFor = NONE;
//            waitingFor--;
//            sensorTimer = 0;
//            sensorValue = SENSOR_VALUE_INIT;
//            break;

        case FRAMESTOP:
            setServo(SERVO_STOP);
            waitingFor = NONE;
            break;

        case LOOKFORGAPEND:
#ifndef STOPONGAP
            if (checkNextFrameTimeout())
            {
                break;
            }
            if (SENSOR_VALUE_MAX != sensorValue)
            {
                break;
            }
            if (ledCount < 0)
            {
                break;
            }
            laserOff();
            sensorTimer = 0;
            sensorValue = SENSOR_VALUE_INIT;
            DELAYEDSTATE(200, FRAMESTOP);
#ifndef NO_PRETENSION
            setMotor(MOTOR_PRETENSION_SLOW);
#endif //  NO_PRETENSION
#endif // STOPONGAP
            break;

        case LOOKFORGAPSTART:
            if (checkNextFrameTimeout())
            {
                break;
            }
            setServo(SERVO_NEXTFRAME_SLOW);
            if (SENSOR_VALUE_MIN == sensorValue)
            {
                waitingFor = LOOKFORGAPEND;
#ifdef STOPONGAP
                laserOff();
                DELAYEDSTATE(100, FRAMESTOP);
                //setServo(SERVO_STOP);
                sensorTimer = 0;
                sensorValue = SENSOR_VALUE_INIT;
                //Serial.println(ledCount, 10);
//                delay(200);
#else // STOPONGAP
                sensorValue = SENSOR_VALUE_MAX - 20;
#endif // STOPONGAP
            }
            break;

        case LOOKFORFRAMEEND:
            if (checkNextFrameTimeout())
            {
                break;
            }
            laserOn();
            sensorTimer = 1;
#ifndef NO_PRETENSION
            setMotor(MOTOR_PRETENSION_INTRAFRAME);
#endif //  NO_PRETENSION
            setServo(SERVO_NEXTFRAME_FAST);
            if (SENSOR_VALUE_MAX == sensorValue)
            {
                waitingFor = LOOKFORGAPSTART;
                sensorValue = SENSOR_VALUE_INIT;
            }
            break;

        case SHUTTERCLOSED:
            SHUTTER_CLOSE;
            DELAYEDSTATE(200, LOOKFORFRAMEEND);
            break;

        case DELAYLOOP:
            delay(10);
            --stateLoop;
            if (0 == stateLoop)
            {
                waitingFor = stateSaved;
            }
            break;

        case EXPOSURESERIES5:
            lampOff();
            waitingFor = SHUTTERCLOSED;
            break;

        case EXPOSURESERIES4:
            lampOn();
            DELAYEDSTATE(100, EXPOSURESERIES5);
            break;

        case EXPOSURESERIES3:
            lampOff();
            DELAYEDSTATE(260, EXPOSURESERIES4);
            break;

        case EXPOSURESERIES2:
            lampOn();
            DELAYEDSTATE(40, EXPOSURESERIES3);
            break;

        case EXPOSURESERIES1:
            lampOn();
            delay(2);
            lampOff();
            DELAYEDSTATE(350, EXPOSURESERIES2);
            break;

        case EXPOSURESERIES:
            DELAYEDSTATE(200, EXPOSURESERIES1);
            break;

        case SHUTTEROPEN:
            SHUTTER_OPEN;
            waitingFor = EXPOSURESERIES;
            break;

        default:
            break;
    }

    if (!Serial.available())
    {
        return;
    }

    u08 input(Serial.read());
    switch (input)
    {
        case 'c':
            SHUTTER_OPEN;
            delay(20);
            SHUTTER_CLOSE;
            break;

        case 'C': // triple shutter
            nextFrameTimeout = millis();
            waitingFor = TRIPLESTART;
            break;

        case 'x':
            reset();
            break;

        case 'l':
            //LAMP_ON;
            lampOn();
            break;

        case 'L':
            //LAMP_OFF;
            lampOff();
            break;

        case 'e':
            laserOn();
            break;

        case 'E':
            laserOff();
//            LASER_OFF;
            break;

        case 's':
            setServo(parameter);
            break;

        case 'm':
            if (parameter > 0)
            {
                setMotor(parameter);
            }
            break;

        case 't': // pretension
            setServo(SERVO_STOP);
//            if (parameter > 0)
//            {
                setMotor(MOTOR_PRETENSION_SLOW);
//            }
            break;

        case 'u': // untension
            setServo(SERVO_STOP);
            setMotor(255);
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
            nextFrameTimeout = millis();
            waitingFor = LOOKFORFRAMEEND;
            sensorValue = SENSOR_VALUE_INIT;
            ledCount = 0;
            break;

        case 'v': // verbose
            Serial.print("waitingFor ");
            Serial.print(waitingFor, 10);
            Serial.print(" Servo ");
            Serial.print(servoPulse, 10);
            Serial.print(" Motor ");
            Serial.print(motorPulse, 10);
//            Serial.print(" motorPTS ");
//            Serial.println(motorPretensionSlow, 10);
            break;

//        case '[':
//            motorPretensionSlow = parameter;
//            break;

        case '-':
            parameter = 0;
            break;

        default:
            if (input >= '0' && input <= '9')
            {
                parameter *= 10;
                parameter += (input - '0');
            }
            Serial.print(parameter, 10);
            break;
    }
    return;
}
