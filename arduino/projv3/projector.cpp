#include <WProgram.h>
#include <avrlibtypes.h>
//#include <twowire.h>
//#define VALUEHISTORY
#ifdef VALUEHISTORY
#include <Adafruit_FRAM_I2C-master/Adafruit_FRAM_I2C.h>
#endif // VALUHISTORY

#pragma GCC diagnostic ignored "-Wwrite-strings"

//#define STOPONGAP
//#define SENSORINT
#define OPTOINT


#define PB_SERVO 3
#define PB_LAMP 1
#define PIN_MOTOR 10  // PB2
#define PB_LASER 0
#define PIN_LEDSENSOR 2 // PC2
#define PC_LEDSENSOR 2 // PC2
#define PB_SHUTTER 4
#ifdef OPTOINT
#define PC_OPTOINT 5 // PC5
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

#define SENSOR_STEP 10 
// must be an even number/SENSOR_STEP
#define SENSOR_VALUE_MAX 255 - SENSOR_STEP
#define SENSOR_VALUE_MIN SENSOR_STEP
#define SENSOR_VALUE_INIT 125
#define SENSOR_VALUE_INIT_MIN SENSOR_VALUE_INIT
#define SENSOR_VALUE_INIT_MAX SENSOR_VALUE_INIT
#define LASER_TIMEOUT_MS 10000
#define LAMP_TIMEOUT_MS 3000
#define NEXT_FRAME_TIMEOUT_MS 8000
#define SERVO_TIMEOUT_MS 2000
#define SENSORTHRESHOLD 200

#define LAMP_OFF PORTB &= ~_BV(PB_LAMP)
#define LAMP_ON PORTB |= _BV(PB_LAMP)
#define SERVO_ON PORTB |= _BV(PB_SERVO)
#define SERVO_OFF PORTB &= ~_BV(PB_SERVO)
#define LASER_ON PORTB |= _BV(PB_LASER);
#define LASER_OFF PORTB &= ~_BV(PB_LASER);
#define SHUTTER_OPEN PORTB &= ~_BV(PB_SHUTTER)
#define SHUTTER_CLOSE PORTB |= _BV(PB_SHUTTER)

#define DELAYEDSTATE(dd, ss) { stateLoop = dd/10; stateSaved = ss; waitingFor = DELAYLOOP; }
#ifdef SENSORINT
#define SENSORINT_ON { PCICR |= _BV(PCIE1); PCMSK1 |= _BV(PCINT10);}
#define SENSORINT_OFF { PCICR &= ~_BV(PCIE1); PCMSK1 &= ~_BV(PCINT10);}
#else // SENSORINT
#define SENSORINT_ON
#define SENSORINT_OFF
#endif // SENSORINT

typedef enum
{ 
        NONE = 0,
//        LASERBLINKON,
//        LASERBLINKOFF,
        FRAMESTOP,
        LOOKFORGAPEND,
        LOOKFORGAPSTART,
        LOOKFORFRAMEEND,
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
        FORWARD_LOOKFORGAPEND,
        FORWARD_LOOKFORFRAMEEND,
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
void laserOn();
void laserOff();
void laserCheck();
void reset();
//u08 sensorActive = 0;
u16 parameter = 0;
u08 verbose = 0;
u08 servoPulse = SERVO_STOP;
u08 motorPulse = 255;
u08 motorPretensionNext = MOTOR_PRETENSION_NEXT;
u08 servoSpeed = SERVO_NEXTFRAME_SLOW;
//u08 motorRewind = MOTOR_OFF;
u32 laserTimeout = 0;
u32 lampTimeout = 0;
//u32 tsServoStart = 0;
u08 sensorValue(0);
u08 lastSensorValue(0);
u08 waitingFor(NONE);
u08 stateLoop;
u08 stateSaved;
u08 lastCommand;
u16 frameCount;
#ifdef VALUEHISTORY
u16 valIndex;
u16 lastValue;
#endif // VALUEHISTORY


//u08 highCount;
u16 servoCount = 0;
u16 servoThreshold = 0;

#ifdef VALUEHISTORY
Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C();
#endif // VALUEHISTORY

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

//ISR(TIMER2_COMPA_vect)
//{
//    SERVO_OFF;
//}


#ifdef SENSORINT
u32 tsFallingSlope = 0;

ISR(PCINT1_vect)
{
    // data = 0, sensor is triggered
    // data != 0, sensor is not triggered
    u08 data = (PINC & _BV(PC_LEDSENSOR));
    if ((LOOKFORFRAMEEND == waitingFor)
        && (0 == data) 
        && (0 == tsFallingSlope))
    {
        tsFallingSlope = millis();
//        highCount = 3;
        waitingFor = LOOKFORGAPEND;
 //       SENSORINT_OFF;
    }
    else if ((LOOKFORGAPEND == waitingFor)
        && (0 != data)
        && (millis() - tsFallingSlope > 120))
    {
        waitingFor = SHUTTEROPEN;
        setServo(SERVO_STOP);
        laserOff();
        SENSORINT_OFF;
    }
}
#endif // SENSORINT

//void incMotor()
//{
//    motorPulse += 5;
//    analogWrite(PIN_MOTOR, motorPulse);
//}
//
//void decMotor()
//{
//    motorPulse -= 5;
//    analogWrite(PIN_MOTOR, motorPulse);
//}

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

void laserOn()
{
#ifndef OPTOINT
    LASER_ON;
    laserTimeout = millis();
#endif // OPTOINT
}

void laserOff()
{
#ifndef OPTOINT
    LASER_OFF;
    laserTimeout = 0;
#endif // OPTOINT
}

void laserCheck()
{
#ifndef OPTOINT
    if (0 == laserTimeout)
    {
        return;
    }
    if ((millis() - laserTimeout) > LASER_TIMEOUT_MS)
    {
        laserOff();
    }
#endif // OPTOINT
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
    if ((PINC & _BV(PC_OPTOINT)) &&
            0 == (sensorValue & _BV(7)))
    {
        sensorValue |= _BV(7);
        return 0;
    }
    else if ((0 == (PINC & _BV(PC_OPTOINT))) &&
            (sensorValue & _BV(7)))
    {
        sensorValue &= ~_BV(7);
        return 1;
    }
    return 0;
}

void reset()
{
    Serial.println("Reset");
    sensorValue = 0;
    setServo(SERVO_STOP);
    lampOff();
    laserOff();
    SHUTTER_CLOSE;
    setMotor(MOTOR_OFF);
    //motorRewind = MOTOR_OFF;
    parameter = 0;
    waitingFor = NONE;
    verbose = 0;
    frameCount = 0;
    SENSORINT_OFF;
}

void setup ()
{ 
    Serial.begin(57600);
    Serial.println("Init Start");
    analogWrite(PIN_MOTOR, MOTOR_OFF);
#ifdef OPTOINT
    PORTC |= (_BV(PC_LEDSENSOR) | _BV(PC_OPTOINT)) ; // tie led sensor line high
#else // OPTOINT
    PORTC |= _BV(PC_LEDSENSOR); // tie led sensor line high
#endif // OPTOINT
    PORTB |= _BV(PB_SHUTTER);
    DDRB |= _BV(PB_LAMP) | _BV(PB_LASER) | _BV(PB_SHUTTER);
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
#ifdef VALUEHISTORY
    if (fram.begin())
    {
        Serial.println("FRAM ok");
    }
#endif // VALUEHISTORY
    Serial.println("Init OK");
}

#ifdef VALUEHISTORY
void framSave(u16 value)
{
    if (valIndex >= 32767)
    {
        valIndex = 32767;
        return;
    }
    fram.write8(valIndex++, value >> 8);
    fram.write8(valIndex++, value & 0xff);
}
#endif // VALUEHISTORY

void loop ()
{
    //laserCheck();
    lampCheck();
    if ('D' == lastCommand)
    {
        Serial.println(analogRead(PIN_LEDSENSOR), 10);
    }

#ifdef OPTOINT
#else // OPTOINT
    if (sensorValue) 
    {
        u16 value = analogRead(PIN_LEDSENSOR);
        if ('<' == lastCommand)
        {
            if (sensorValue >= SENSORTHRESHOLD && lastSensorValue < SENSORTHRESHOLD)
            {
                decFrameCount();
            }
            if (sensorValue < SENSORTHRESHOLD && lastSensorValue >= SENSORTHRESHOLD)
            {
                lastSensorValue = sensorValue;
            }
        }
        else
        {
            if ((value >= SENSORTHRESHOLD) && sensorValue < SENSOR_VALUE_MAX)
            {
                sensorValue += SENSOR_STEP;
            }
            else if ((value < SENSORTHRESHOLD) && sensorValue > SENSOR_VALUE_MIN)
            {
                sensorValue -= SENSOR_STEP;
            }
        }

#ifdef VALUEHISTORY
        if (value != lastValue)
        {
            framSave(value);
            framSave(sensorValue);
            lastValue = value;
        }
#endif // VALUEHISTORY
//        if (verbose)
//        {
//            Serial.print(value, 10);
//            Serial.print(' ');
//            Serial.println(sensorValue, 10);
//        }
    }
#endif // OPTOINT
//#else // ANALOGSENSOR
//    if (sensorValue) 
//    {
//        laserOn();
//        u08 value = PINC & _BV(PC_LEDSENSOR);
//        laserOff();
//        if (value && sensorValue < SENSOR_VALUE_MAX)
//        {
//            sensorValue += SENSOR_STEP;
//        }
//        else if (!value && sensorValue > SENSOR_VALUE_MIN)
//        {
//            sensorValue -= SENSOR_STEP;
//        }
////        if (verbose)
////        {
////            Serial.println(sensorValue, 10);
////        }
//    }
//#endif // ANALOGSENSOR

    switch (waitingFor)
    {
        case NONE:
            break;

//        case LASERBLINKON:
//            laserOn();
//            lampOn();
//            DELAYEDSTATE(10, LASERBLINKOFF);
//            break;

//        case LASERBLINKOFF:
//            laserOff();
//            lampOff();
//            DELAYEDSTATE(100, LASERBLINKON);
//            break;

        case FRAMESTOP:
            setServo(SERVO_STOP);
            if (frameCount == 0)
            {
                setMotor(MOTOR_OFF);
            }
            laserOff();
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

        case LOOKFORGAPEND:
            if (SENSOR_VALUE_MIN < sensorValue)
            {
                break;
            }
            waitingFor = FRAMESTOP;
            break;

        case LOOKFORFRAMEEND:
            if (SENSOR_VALUE_MAX > sensorValue)
            {
                break;
            }
            {
                u08 newSpeed = servoSpeed + ((SERVO_STOP - servoSpeed) >> 1);
                setServo(newSpeed);
            }
            waitingFor = LOOKFORGAPEND;
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
            laserOn();
            setServo(servoSpeed);
#ifdef OPTOINT
            if (PINC & _BV(PC_OPTOINT))
            {
                sensorValue |= _BV(7);
            }
            else
            {
                sensorValue &= ~_BV(7);
            }

            DELAYEDSTATE(200, OPTOINT_CHANGED);
#else // OPTOINT
            DELAYEDSTATE(200, LOOKFORFRAMEEND);
            sensorValue = SENSOR_VALUE_INIT_MAX;
            SENSORINT_ON;
#endif // OPTOINT
#ifdef VALUEHISTORY
            valIndex = 0;
            lastValue = 0;
#endif // VALUEHISTORY
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

        case FORWARD_LOOKFORGAPEND:
            if (SENSOR_VALUE_MIN < sensorValue)
            {
                break;
            }
            if (frameCount)
            {
                frameCount--;
                if (verbose)
                {
                    Serial.print("Frame ");
                    Serial.println(frameCount, 10);
                }
            }
            if (decFrameCount())
            {
                waitingFor = NONE;
            }
            else
            {
                waitingFor = FORWARD_LOOKFORFRAMEEND;
            }
            break;

        case FORWARD_LOOKFORFRAMEEND:
            if (SENSOR_VALUE_MAX > sensorValue)
            {
                break;
            }
            waitingFor = FORWARD_LOOKFORGAPEND;
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
//        case 'b':
//            waitingFor = LASERBLINKON;
//            sensorValue = SENSOR_VALUE_INIT;
//            break;

        case 'c':
            SHUTTER_OPEN;
            delay(20);
            SHUTTER_CLOSE;
            break;

        case 'C': // triple shutter
            //waitingFor = TRIPLESTART;
            waitingFor = SENSORSTART;
            break;

#ifdef VALUEHISTORY
            case 'd': // dump FRAME
                Serial.println(valIndex, 10);
                for (u16 ii = 0; ii < valIndex; ii += 4)
                {
                    // value
                    u16 fValue((fram.read8(ii) << 8) | fram.read8(ii+1));
                    Serial.print(fValue, 10);
                    Serial.print(' ');
                    // sensorValue
                    fValue = (fram.read8(ii+2) << 8) | fram.read8(ii+3);
                    Serial.println(fValue, 10);
                }
                break;
#endif // VALUEHISTORY

            case 'D':
                Serial.print("***");
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

        case 'e':
            laserOn();
            break;

        case 'E':
            laserOff();
//            LASER_OFF;
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
                sensorValue = SENSOR_VALUE_INIT;
            }
            setMotor(MOTOR_PRETENSION_FF);
            setServo(SERVO_MIN);
            waitingFor = STOPONFRAMEZERO;
//            if (frameCount > 0)
//            {
//                laserOn();
//                sensorValue = SENSOR_VALUE_INIT;
//                waitingFor = FORWARD_LOOKFORFRAMEEND;
//            }
            break;

        case 'F': // backward
            setMotor(MOTOR_REWIND_SLOW);
            setServo(SERVO_REVERSE);
            break;

        case 'r':
            sensorValue = SENSOR_VALUE_INIT;
            setMotor(MOTOR_REWIND_FAST);
            waitingFor = STOPONFRAMEZERO;
            break;

//        case ',': // rewind '<'
//            sensorValue = SENSOR_VALUE_INIT;
//            setMotor(MOTOR_REWIND_FAST);
//            if (motorRewind < MOTOR_REWIND_FAST && motorRewind >= MOTOR_REWIND_SLOW)
//            {
//                motorRewind += 10;
//            }
//            else
//            {
//                motorRewind = MOTOR_REWIND_SLOW;
//            }
//            if (verbose)
//            {
//                Serial.println(motorRewind, 10);
//            }
//            setMotor(motorRewind);
//            setServo(SERVO_STOP);
//            break;

        case 'n': // next frame
            waitingFor = SENSORSTART;
            break;

        case 'v': // verbose
            verbose = 1;
//            Serial.print("waitingFor ");
//            Serial.print(waitingFor, 10);
//            Serial.print(" Servo ");
//            Serial.print(servoPulse, 10);
//            Serial.print(" Motor ");
//            Serial.print(motorPulse, 10);
//            Serial.print(" sensorActive ");
//            Serial.println(sensorActive, 10);
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
