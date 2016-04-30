#include <WProgram.h>
#include <avrlibtypes.h>
#pragma GCC diagnostic ignored "-Wwrite-strings"

//#define SUPER8
#define OPTOINT

#define PB_SHUTTER 4
#define PB_SERVO 3
#define PB_LAMP 1
#define PIN_MOTOR 10  // PB2
#ifdef OPTOINT
#define PC_OPTOINT 5 // PC5
#define OPTOINT_TIMEOUT 5000
#endif // OPTOINT

#define MOTOR_PRETENSION_NEXT 40 
#define MOTOR_PRETENSION_FF 20
#define MOTOR_REWIND_FAST 100 
#define MOTOR_REWIND_SLOW 10 
#define MOTOR_OFF 0 
#define SERVO_MIN 64
#define SERVO_STOP 94
#define SERVO_MAX 128
#define SERVO_NEXTFRAME_FAST 140 
//#define SERVO_NEXTFRAME_SLOW 70 
#define SERVO_NEXTFRAME_SLOW 74 
#define SERVO_REVERSE 110 

#define LAMP_TIMEOUT_MS 3000
#define NEXT_FRAME_TIMEOUT_MS 8000
#define SERVO_TIMEOUT_MS 2000
#define SENSORTHRESHOLD 200

#define LAMP_OFF PORTB &= ~_BV(PB_LAMP)
#define LAMP_ON PORTB |= _BV(PB_LAMP)
#define SERVO_ON PORTB |= _BV(PB_SERVO)
#define SERVO_OFF PORTB &= ~_BV(PB_SERVO)
#define SHUTTER_OPEN PORTB &= ~_BV(PB_SHUTTER)
#define SHUTTER_CLOSE PORTB |= _BV(PB_SHUTTER)

#define DELAYEDSTATE(dd, ss) { stateLoop = dd/10; stateSaved = ss; waitingFor = DELAYLOOP; }

typedef enum
{ 
        NONE = 0,
        FRAMESTOP,
        DELAYLOOP,
        SENSORSTART,
        OPTOINT_CHANGED,
        STOPONFRAMEZERO,
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
void reset();

u16 parameter = 0;
u08 verbose = 0;
u08 servoPulse = SERVO_STOP;
u08 motorPulse = 255;
u08 motorPretensionNext = MOTOR_PRETENSION_NEXT;
u08 servoSpeed = SERVO_NEXTFRAME_SLOW;
u32 lampTimeout = 0;
volatile u32 optoIntTimeout = 0;
u08 sensorValue(0);
u08 lastSensorValue(0);
u08 waitingFor(NONE);
u08 stateLoop;
u08 stateSaved;
u08 lastCommand;
u16 frameCount;

//u08 highCount;
u16 servoCount = 0;
u16 servoThreshold = 0;

ISR(TIMER2_OVF_vect)
{
    ++servoCount;
    if (servoCount > servoThreshold)
    {
        SERVO_OFF;
    }
    else
    {
        SERVO_ON;
    }
    servoCount &= 0x03ff;
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
    servoCount = 0;
    servoThreshold = set;
    if (0 == servoThreshold)
    {
        TIMSK2 &= ~_BV(TOIE2); // disable counter2 overflow interrupt
    }
    else
    {
        TIMSK2 |= _BV(TOIE2); // enable counter2 overflow interrupt
    }
//    if (set < SERVO_MIN || set > SERVO_MAX)
//    {
//        return;
//    }
//    servoPulse = set;
//    OCR2A = servoPulse;
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

// 1 = made the transition to zero
u08 decFrameCount()
{
    if (0 == frameCount)
    {
        return 0;
    }
    frameCount--;
    if (frameCount == 0)
    {
        setServo(SERVO_STOP);
        sensorValue = 0;
        waitingFor = FRAMESTOP;
        return 1;
    }
    return 0;
}

u08 isOptoTransition()
{
    if (0 == optoIntTimeout)
    {
        optoIntTimeout = millis();
        Serial.print("opto ");
        Serial.println(optoIntTimeout);
    }
    if ((millis() - optoIntTimeout) > OPTOINT_TIMEOUT)
    {
        if (verbose)
        {
            Serial.print("Opto int timeout @");
            Serial.print(millis());
        }
        reset();
        return 0;
    }
#ifdef SUPER8
    if ((PINC & _BV(PC_OPTOINT)) &&  0 == sensorValue)
    {
        sensorValue = 1;
        return 0;
    }
    else if ((0 == (PINC & _BV(PC_OPTOINT))) && 0 != sensorValue)
    {
        sensorValue = 0;
        Serial.print("Opto int reset @");
        Serial.print(millis());
        optoIntTimeout = 0;
        return 1;
    }
    return 0;
#else // SUPER8
    if ((PINC & _BV(PC_OPTOINT)) && 0 == sensorValue)
    {
        sensorValue = 1;
        Serial.print("Opto int reset @");
        Serial.print(millis());
        optoIntTimeout = 0;
        return 1;
    }
    else if ((0 == (PINC & _BV(PC_OPTOINT))) && 0 != sensorValue)
    {
        sensorValue = 0;
        return 0;
    }
    return 0;
#endif // SUPER8
}

void reset()
{
    Serial.println("Reset");
    sensorValue = 0;
    setServo(SERVO_STOP);
    lampOff();
    SHUTTER_CLOSE;
    setMotor(MOTOR_OFF);
    //motorRewind = MOTOR_OFF;
    parameter = 0;
    waitingFor = NONE;
    verbose = 0;
    frameCount = 0;
    optoIntTimeout = 0;
}

void setup ()
{ 
    Serial.begin(57600);
    Serial.println("Init Start");
    analogWrite(PIN_MOTOR, MOTOR_OFF);
    PORTC |=  _BV(PC_OPTOINT); // tie led sensor line high
    PORTB |= _BV(PB_SHUTTER);
    DDRB |= _BV(PB_LAMP) | _BV(PB_SHUTTER);
    lampOff();
    cli();
    TCCR2B |= _BV(CS20); // no prescaler   
    TIFR2 &= ~_BV(TOV2); // clear overflow interrupt flag
    TIMSK2 |= _BV(TOIE2); // enable counter2 overflow interrupt
    OCR2A = SERVO_STOP;
    servoPulse = SERVO_STOP;
    TCNT2 = 0x00;
    DDRB |= _BV(PB_SERVO);
    sei();
    Serial.println("Init OK");
}

void loop ()
{
    lampCheck();

    switch (waitingFor)
    {
        case NONE:
            break;

        case FRAMESTOP:
            setServo(SERVO_STOP);
            if (frameCount == 0)
            {
                setMotor(MOTOR_OFF);
            }
            sensorValue = 0;
            if ('n' == lastCommand)
            {
                waitingFor = NONE;
            }
            else
            {
                DELAYEDSTATE(1000, SHUTTEROPEN);
            }
            break;

        case OPTOINT_CHANGED:
            if (isOptoTransition())
            {
                waitingFor = FRAMESTOP;
            }
            break;

        case SENSORSTART:
            if (frameCount > 0)
            {
                setMotor(motorPretensionNext);
            }
            delay(400);
            setServo(servoSpeed);
            sensorValue = PINC & _BV(PC_OPTOINT);
            DELAYEDSTATE(200, OPTOINT_CHANGED);
            break;

        case SHUTTERCLOSED:
            SHUTTER_CLOSE;
            //DELAYEDSTATE(400, SENSORSTART);
            //tsServoStart = millis();
            if (frameCount > 0)
            {
                --frameCount;
                if (verbose)
                {
                    Serial.print("Frame ");
                    Serial.println(frameCount, 10);
                }
                waitingFor = SENSORSTART;
            }
            else
            {
                if (verbose)
                {
                    Serial.println("Frames Done");
                }
                waitingFor = NONE;
            }
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
            DELAYEDSTATE(80, EXPOSURESERIES5);
            break;

        case EXPOSURESERIES3:
            lampOff();
            DELAYEDSTATE(260, EXPOSURESERIES4);
            break;

        case EXPOSURESERIES2:
            lampOn();
            DELAYEDSTATE(20, EXPOSURESERIES3);
            break;

        case EXPOSURESERIES1:
            lampOn();
            delay(4);
            lampOff();
            DELAYEDSTATE(350, EXPOSURESERIES2);
            break;

        case EXPOSURESERIES:
            DELAYEDSTATE(100, EXPOSURESERIES1);
//            DELAYEDSTATE(200, EXPOSURESERIES1);
            break;

        case SHUTTEROPEN:
            SHUTTER_OPEN;
            waitingFor = EXPOSURESERIES;
            break;


       case STOPONFRAMEZERO:
            if (isOptoTransition())
            {
                if (decFrameCount())
                {
                    setMotor(MOTOR_OFF);
                    waitingFor = NONE;
                }
            }
        break;

        default:
            break;
    }

    if (!Serial.available())
    {
        return;
    }

    lastCommand = Serial.read();
    switch (lastCommand)
    {

        case 'c':
            SHUTTER_OPEN;
            delay(20);
            SHUTTER_CLOSE;
            break;

        case 'C': // triple shutter
            //waitingFor = TRIPLESTART;
            waitingFor = SENSORSTART;
            break;

        case ' ':
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

        case 'o':
            if (parameter > 0)
            {
                frameCount = parameter;
                parameter = 0;
            }
            break;

        case 's':
            setServo(parameter);
            parameter = 0;
            break;

        case 'm':
            if (parameter > 0)
            {
                setMotor(parameter);
                parameter = 0;
            }
            break;

        case 't': // pretension
            setServo(SERVO_STOP);
//            if (parameter > 0)
//            {
                setMotor(motorPretensionNext);
//            }
            break;

        case 'u': // untension
            setServo(SERVO_STOP);
            setMotor(MOTOR_OFF);
            break;

        case 'f': // forward
            if (frameCount)
            {
                sensorValue = 1;
            }
            setMotor(MOTOR_PRETENSION_FF);
            setServo(SERVO_MIN);
            if (frameCount)
            {
                waitingFor = STOPONFRAMEZERO;
            }
            break;

        case 'F': // backward
            setMotor(MOTOR_REWIND_SLOW);
            setServo(SERVO_REVERSE);
            break;

        case 'r':
            sensorValue = 1;
            setMotor(MOTOR_REWIND_FAST);
            if (frameCount)
            {
                waitingFor = STOPONFRAMEZERO;
            }
            break;

        case 'n': // next frame
            waitingFor = SENSORSTART;
            break;

        case 'v': // verbose
            verbose = 1;
            break;

        case '[':
            motorPretensionNext = parameter;
            parameter = 0;
            break;

        case ']':
            servoSpeed = parameter;
            parameter = 0;
            break;

        case '-':
            parameter = 0;
            break;

        default:
            if (lastCommand >= '0' && lastCommand <= '9')
            {
                parameter *= 10;
                parameter += (lastCommand - '0');
            }
            Serial.print(parameter, 10);
            break;
    }
    return;
}
