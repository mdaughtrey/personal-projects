#include <WProgram.h>
#include <avrlibtypes.h>
#ifdef USESTEPPER
#include <stepper.h>
#endif // USESTEPPER
#ifdef USEFRAM
#define LOGSTATE
#include <Adafruit_FRAM_I2C-master/Adafruit_FRAM_I2C.h>
#endif // USEFRAM
#pragma GCC diagnostic ignored "-Wwrite-strings"

#define PB_CAMERABUSY 0
#define PC_SHUTTER 2
#define PD_CAMERAPOWER 7
#define PD_SHUTTERREADY 6
#ifndef USESTEPPER
#define PB_SERVO 3
#endif // USESTEPPER
#define PB_LAMP 1
#define PIN_MOTOR 10  // PB2
#define PC_OPTOINT 1 // PC1
#define OPTOINT_TIMEOUT 5000

#define MOTOR_PRETENSION_NEXT 40 
#define MOTOR_PRETENSION_FF 20
#define MOTOR_REWIND_FAST 100 
#define MOTOR_REWIND_SLOW 70 
#define MOTOR_OFF 0 
#ifndef USESTEPPER
#define SERVO_MIN 64
#define SERVO_STOP 94
#define SERVO_MAX 128
#define SERVO_NEXTFRAME_FAST 74 
//#define SERVO_NEXTFRAME_SLOW 70 
#define SERVO_NEXTFRAME_SLOW 84 
#define SERVO_NEXTFRAME_VERY_SLOW 87
#define SERVO_REVERSE 110 
#endif // USESTEPPER

#define CAMERABUSYTIMEOUT 1000
#define LAMP_TIMEOUT_MS 3000
#define NEXT_FRAME_TIMEOUT_MS 8000
#define SENSORTHRESHOLD 200
#define LAMP_OFF PORTB &= ~_BV(PB_LAMP)
#define LAMP_ON PORTB |= _BV(PB_LAMP)

#ifndef USESTEPPER
#define SERVO_TIMEOUT_MS 2000
#define SERVO_ON PORTB |= _BV(PB_SERVO)
#define SERVO_OFF PORTB &= ~_BV(PB_SERVO)
#endif // USESTEPPER

#define SHUTTER_OPEN PORTC &= ~_BV(PC_SHUTTER)
#define SHUTTER_CLOSE PORTC |= _BV(PC_SHUTTER)
#define SENSORVALUEINIT { sensorValue = PINC & _BV(PC_OPTOINT); }
#define CAMERA_ON PORTD |= _BV(PD_CAMERAPOWER);
#define CAMERA_OFF PORTD &= ~_BV(PD_CAMERAPOWER);
#define SHUTTERINT_ON { PCMSK2 |= _BV(PCINT22); PCICR |= _BV(PCIE2); }
#define SHUTTERINT_OFF { PCMSK2 &= ~_BV(PCINT22); PCICR &= ~_BV(PCIE2); }

#ifdef USEFRAM
#define FHEADER_ISR 0x5a
#define FHEADER_STATE 0xa5
#endif // USEFRAM

//#define DELAYEDSTATE(dd, ss) { stateLoop = dd/10; stateSaved = ss; waitingFor = DELAYLOOP; }

typedef enum
{ 
        NONE = 0,           // 0
        FRAMESTOP,          // 1
        SENSORSTART,        // 2
        OPTOINT_CHANGED,    // 3
        STOPONFRAMEZERO,    // 4
        SHUTTERCLOSED,      // 5
        EXPOSURESERIES5,    // 6
        EXPOSURESERIES4,    // 7
        EXPOSURESERIES3A,   // 8
        EXPOSURESERIES3,    // 9
        EXPOSURESERIES2A,   // a
        EXPOSURESERIES2,    // b
        EXPOSURESERIES1A,   // c
        EXPOSURESERIES1,    // d
        SHUTTEROPEN,        // e
        OPTOINT_HALF,       // f
        OPTOINT_FULL,       // 10
        OPTOINT_REVERSE,    // 11
        OPTOINT_FULL2,      // 12
        WAITCAMERABUSY      // 13
        //TRIPLESTART = SHUTTEROPEN
} WaitFor;

typedef enum
{
    FILM_NONE = 0,
    FILM_8MM,
    FILM_SUPER8
} FilmMode;

void incMotor();
void decMotor();
void setMotor(u08 set);

#ifndef USESTEPPER
void incServo();
void decServo();
void setServo(u08 set);
#endif // USESTEPPER
void lampOn();
void lampOff();
void lampCheck();
void reset();

u16 parameter = 0;
volatile u08 intCount = 0;
volatile u08 lastIntCount = 0;
u08 verbose = 1;
#ifndef USESTEPPER
u08 servoPulse = SERVO_STOP;
#endif // USESTEPPER
u08 motorPulse = 255;
u08 motorPretensionNext = MOTOR_PRETENSION_NEXT;
//u08 servoSpeed = SERVO_NEXTFRAME_FAST;
u32 lampTimeout = 0;
volatile u32 optoIntTimeout;
u32 cameraBusyTimeout;
u08 sensorValue;
//u08 lastSensorValue(0);
u08 waitingFor(NONE);
//u08 stateLoop;
//u08 stateSaved;
u08 lastCommand;
u16 frameCount;
u08 filmMode = FILM_NONE;
volatile u08 shutterState = 0;

//u08 highCount;
#ifndef USESTEPPER
u16 servoCount = 0;
u16 servoThreshold = 0;
#endif // USESTEPPER

#ifdef USEFRAM
Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C();
u16 framIndex;
//volatile u32 tmrNoOptoChange;

typedef struct
{
    u08 head;
    u08 tail;
    u32 millis;
    u08 intCount;
    u08 shutterState;
    u08 waitingFor;
} IntEntry;

IntEntry intQueue[4];
volatile u08 iqHead = 0;
volatile u08 iqTail = 0;

void framWrite8(u08 value)
{
    if (framIndex >= 32767)
    {
        framIndex = 0;
    }
    fram.write8(framIndex++, value);
}

void framWrite32(u32 value)
{
    union
    {
        u32 asU32;
        char asChar[4];
    }result;
    result.asU32 = value;

    if (framIndex >= 32767)
    {
        framIndex = 0;
    }
    fram.write8(framIndex++, result.asChar[0]);
    fram.write8(framIndex++, result.asChar[1]);
    fram.write8(framIndex++, result.asChar[2]);
    fram.write8(framIndex++, result.asChar[3]);
}

u32 framRead32(u16 address)
{
    union
    {
        u32 asU32;
        char asChar[4];
    }result;

    result.asChar[0] = fram.read8(address + 0);
    result.asChar[1] = fram.read8(address + 1);
    result.asChar[2] = fram.read8(address + 2);
    result.asChar[3] = fram.read8(address + 3);

    return result.asU32;
}
//-                    u16 fValue((fram.read8(ii) << 8) | fram.read8(ii+1));
#endif // USEFRAM

#ifndef USESTEPPER
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
#endif // USESTEPPER

ISR(PCINT2_vect)
{
    intCount++;
    shutterState <<= 1;
    if (PIND & _BV(PD_SHUTTERREADY)) //  && (0 == shutterState & 0x01))
    {
        shutterState |= 1;
    }
#ifdef USEFRAM
    IntEntry entry = { iqHead, iqTail, millis(), intCount, shutterState, waitingFor };
    intQueue[iqHead++] = entry;
    iqHead %= 4;
#endif // USEFRAM
}

void setMotor(u08 set)
{
    motorPulse = set;
    analogWrite(PIN_MOTOR, motorPulse);
}

#ifndef USESTEPPER
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
#endif // USESTEPPER

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
#ifndef USESTEPPER
        setServo(SERVO_STOP);
#else
        stepperStop();
#endif // USESTEPPER
        waitingFor = FRAMESTOP;
        return 1;
    }
    return 0;
}
#define NOTRANSITION 0
#define FULLFRAME 1
#define HALFFRAME 2

u08 halfFrameTransition()
{
    sensorValue = !sensorValue;
    optoIntTimeout = millis();
    return HALFFRAME; // half frame transition
}

u08 fullFrameTransition()
{
    sensorValue = !sensorValue;
    //optoIntTimeout = 0;
    return FULLFRAME; // full frame transition
}
//
// 0 = no transition
// 1 = full frame
// 2 = half frame
//
u08 isOptoTransition()
{
    u32 now = millis();
    if (0 == optoIntTimeout)
    {
        optoIntTimeout = now;
    }
    if ((now - optoIntTimeout) > OPTOINT_TIMEOUT)
    {
        if (1 == verbose)
        {
            Serial.print("Opto int timeout ");
            Serial.print(optoIntTimeout);
            Serial.print(" vs ");
            Serial.println(now);
        }
        reset();
        return NOTRANSITION;
    }
    if (FILM_SUPER8 == filmMode)
    {
        if ((PINC & _BV(PC_OPTOINT)) && 0 == sensorValue)
        {
            return halfFrameTransition();
        }
        else if ((0 == (PINC & _BV(PC_OPTOINT))) && 0 != sensorValue)
        {
            return fullFrameTransition();
        }
    }
    else if (FILM_8MM == filmMode)
    {
        if ((PINC & _BV(PC_OPTOINT)) && 0 == sensorValue)
        {
            return fullFrameTransition();
        }
        else if ((0 == (PINC & _BV(PC_OPTOINT))) && 0 != sensorValue)
        {
            return halfFrameTransition();
        }
    }
    return 0; // no transition
}

void reset()
{
    Serial.println("Reset");
#ifndef USESTEPPER
    setServo(SERVO_STOP);
#else
    stepperInit();
#endif // USESTEPPER
    lampOff();
    //SHUTTERINT_OFF;
    CAMERA_OFF;
    SHUTTER_CLOSE;
    setMotor(MOTOR_OFF);
    //motorRewind = MOTOR_OFF;
    parameter = 0;
    waitingFor = NONE;
    verbose = 0;
    frameCount = 0;
    //optoIntTimeout = 0;
    shutterState = 0;
#ifdef USEFRAM
    framIndex = 0;
#endif // USEFRAM
}

void setup ()
{ 
    Serial.begin(57600);
    Serial.println("Init Start");
    PORTC |= _BV(PC_OPTOINT); // tie opt int sensor line hight

    analogWrite(PIN_MOTOR, MOTOR_OFF);
    //PORTB |= _BV(PC_OPTOINT); // tie led sensor line high
    //PORTB |= _BV(PB_CAMERABUSY);
    PORTC |= _BV(PC_SHUTTER);
    PORTD |= _BV(PD_CAMERAPOWER);
//    PORTD |= _BV(PD_SHUTTERREADY);
    DDRD |= _BV(PD_CAMERAPOWER);
    DDRB |= _BV(PB_LAMP);
    DDRC |= _BV(PC_SHUTTER) | 0x02;
    CAMERA_OFF;
    lampOff();
    Serial.println("Init Start 4");

#ifndef USESTEPPER
    cli();
    TCCR2B |= _BV(CS20); // no prescaler   
    TIFR2 &= ~_BV(TOV2); // clear overflow interrupt flag
    TIMSK2 |= _BV(TOIE2); // enable counter2 overflow interrupt
    OCR2A = SERVO_STOP;
    servoPulse = SERVO_STOP;
    TCNT2 = 0x00;
    DDRB |= _BV(PB_SERVO);
    sei();
#else // USESTEPPER
    Serial.println("stepperInit");
    stepperInit();
#endif // USESTEPPER
    shutterState = 0xaa;
#ifdef USEFRAM
    Serial.println("Init FRAM");
    if (fram.begin())
    {
        Serial.println("FRAM ok");
    }
    framIndex = 0;
    iqTail = 0;
    iqHead = 0;
#endif // USEFRAM
    Serial.println("Init OK");
}

#ifdef LOGSTATE
u08 lastReportedState(255);
#endif // LOGSTATE

void loop ()
{
    if (2 == verbose)
    {
        Serial.print(PINC, 2);
        Serial.print("\r\n");
        delay(100);
    }
#ifdef USESTEPPER
    stepperPoll();
#endif // USESTEPPER
    lampCheck();
#ifdef USEFRAM
#ifdef LOGSTATE
    if (lastReportedState != waitingFor)
    {
        framWrite8(FHEADER_STATE);
        framWrite8(iqHead);
        framWrite8(iqTail);
        framWrite32(millis());
        framWrite8(intCount);
        framWrite8(shutterState);
        framWrite8(waitingFor);
        lastReportedState = waitingFor;
    }
    if (iqTail != iqHead)
    {
        cli();
        IntEntry tmp = intQueue[iqTail++];
        iqTail %= 4;
        sei();
        framWrite8(FHEADER_ISR);
        framWrite8(tmp.head);
        framWrite8(tmp.tail);
        framWrite32(tmp.millis);
        framWrite8(tmp.intCount);
        framWrite8(tmp.shutterState);
        framWrite8(tmp.waitingFor);
    }
#endif // LOGSTATE
#endif // USEFRAM

    switch (waitingFor)
    {
        case NONE:
            break;

        case FRAMESTOP:
            if (2 == verbose)
            {
                Serial.println("FRAMESTOP");
            }
#ifndef USESTEPPER
            setServo(SERVO_STOP);
#else
            stepperStop();
#endif // USESTEPPER
            if (frameCount == 0)
            {
                setMotor(MOTOR_OFF);
            }
            if ('n' == lastCommand)
            {
                waitingFor = NONE;
            }
            else
            {
                //waitingFor = SHUTTEROPEN;
                cameraBusyTimeout = millis();
                waitingFor = WAITCAMERABUSY;
                if (verbose)
                {
                    Serial.print("OIC ");
                    Serial.print(millis() - optoIntTimeout, 10);
                    Serial.print("\r\n");
                }
                if ((millis() - optoIntTimeout) < 750)
                {
                    stepperDelay(1);
                }
            }
            break;

        case WAITCAMERABUSY:
            if (2 == verbose)
            {
                Serial.println("WAITCAMERABUSY");
            }
            if (!(PINB & _BV(PB_CAMERABUSY)))
            {
                if (2 == verbose)
                {
                    Serial.println("Camera Busy");
                }
                cameraBusyTimeout = millis();
                break;
            }

            if ((millis() - cameraBusyTimeout) < CAMERABUSYTIMEOUT)
            {
                if (2 == verbose)
                {
                    Serial.println("Camera Not Ready");
                }
                break;
            }

            if (verbose)
            {
                Serial.println("Camera Ready");
            }
            waitingFor = SHUTTEROPEN;
            break;
            
        case OPTOINT_HALF:
            if (2 == verbose)
            {
                Serial.println("OPTOINT_HALF");
            }
            if (HALFFRAME == isOptoTransition())
            {
#ifndef USESTEPPER
                setServo(SERVO_NEXTFRAME_SLOW);
#endif // USESTEPPER
                waitingFor = OPTOINT_FULL;
            }
            break;

        case OPTOINT_FULL:
            if (2 == verbose)
            {
                Serial.println("OPTOINT_FULL");
            }
            if (FULLFRAME == isOptoTransition())
            {
                //setMotor(MOTOR_REWIND_SLOW);
                setMotor(motorPretensionNext);
#ifndef USESTEPPER
                setServo(SERVO_REVERSE);
                waitingFor = OPTOINT_REVERSE;
#else
                waitingFor = FRAMESTOP;
#endif // USESTEPPER
            }
            break;

        case OPTOINT_REVERSE:
            if (2 == verbose)
            {
                Serial.println("OPTOINT_REVERSE");
            }
            if (HALFFRAME == isOptoTransition())
            {
                setMotor(motorPretensionNext);
#ifndef USESTEPPER
                setServo(SERVO_NEXTFRAME_VERY_SLOW);
#endif // USESTEPPER
                waitingFor = OPTOINT_FULL2;
            }
            break;

        case OPTOINT_FULL2:
            if (2 == verbose)
            {
                Serial.println("OPTOINT_FULL2");
            }
            if (FULLFRAME == isOptoTransition())
            {
                waitingFor = FRAMESTOP;
            }
            break;

        case OPTOINT_CHANGED:
            if (2 == verbose)
            {
                Serial.println("OPTOINT_CHANGED");
            }
            switch (isOptoTransition())
            {
                case 1:
                    optoIntTimeout = 0;
//                    waitingFor = REVERSESEEK;
                    waitingFor = OPTOINT_HALF;
                    setMotor(MOTOR_REWIND_SLOW);
#ifndef USESTEPPER
                    setServo(SERVO_REVERSE);
#endif // USESTEPPER
                    //DELAYEDSTATE(1000, REVERSESEEK);
                    break;
#ifndef USESTEPPER
                case 2: setServo(SERVO_NEXTFRAME_SLOW); break;
#endif // USESTEPPER
            }
            break;

        case SENSORSTART:
            if (2 == verbose)
            {
                Serial.println("SENSORSTART");
            }
            if (frameCount > 0)
            {
                setMotor(motorPretensionNext);
            }
#ifndef USESTEPPER
            setServo(SERVO_NEXTFRAME_FAST);
#else
            stepperGo();
#endif // USESTEPPER
            SENSORVALUEINIT;
            optoIntTimeout = 0;
//#ifdef USEFRAM
//            framWrite32(optoIntTimeout);
//#endif // USEFRAM
        //    if (verbose)
        //    {
        //        Serial.print("Timeout = ");
        //        Serial.println(optoIntTimeout);
        //    }
            //waitingFor = OPTOINT_CHANGED;
            waitingFor = OPTOINT_HALF;
            break;

        case SHUTTERCLOSED:
            if (2 == verbose)
            {
                Serial.println("SHUTTERCLOSED");
            }

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

//        case DELAYLOOP:
//            delay(10);
//            --stateLoop;
//            if (0 == stateLoop)
//            {
//                waitingFor = stateSaved;
//            }
//            break;

//        case EXPOSURESERIES4:
////            lampOff();
////            SHUTTERINT_OFF;
////            SHUTTER_CLOSE;
//            waitingFor = SHUTTERCLOSED;
//            break;
//
//        case EXPOSURESERIES3A:
//            if (0x01 & shutterState)
//            {
//                break;
//            }
//            waitingFor = EXPOSURESERIES4;
//            break;

        case EXPOSURESERIES3:
            if (0x01 & shutterState)
            {
                lampOn();
                SHUTTERINT_OFF;
                SHUTTER_CLOSE;
                delay(30);
                lampOff();
                waitingFor = SHUTTERCLOSED;
                //waitingFor = EXPOSURESERIES3A;
            }
            break;

        case EXPOSURESERIES2A:
            if (0x01 & shutterState)
            {
                break;
            }
            waitingFor = EXPOSURESERIES3;
            break;

        case EXPOSURESERIES2: 
            if (0x01 & shutterState)
            {
                lampOn();
                delay(10);
                lampOff();
                waitingFor = EXPOSURESERIES2A;
            }
            break;

        // next state when pulse ends
        case EXPOSURESERIES1A:
            if (0x01 & shutterState)
            {
                break;
            }
            waitingFor = EXPOSURESERIES2;
            break;

        // wait for lowgoing pulse
        case EXPOSURESERIES1: 
            if (0x01 & shutterState)
            {
                lampOn();
                delay(2);
                lampOff();
                waitingFor = EXPOSURESERIES1A;
            }
            break;

        // Trigger shutter
        case SHUTTEROPEN:
            shutterState = 0xff;
            SHUTTERINT_ON;
            SHUTTER_OPEN;
            delay(20);
            waitingFor = EXPOSURESERIES1;
//            delay(100);
//            lampOn();
//            delay(4);
//            lampOff();
//            DELAYEDSTATE(350, EXPOSURESERIES2);
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
//            Serial.print("SOFZ ");
//            Serial.println(millis());
        break;

        default:
            break;
    }

    if (!Serial.available())
    {
        return;
    }

    lastCommand = Serial.read();
    u16 ii;
    switch (lastCommand)
    {
#ifdef USEFRAM
        case 'q':
            Serial.println("Q!\r\n");
            Serial.println((int)framIndex);
            ii = 0;
            while (ii < framIndex)
            {
                u08 type(fram.read8(ii++));
                if (FHEADER_STATE == type)
                {
                    Serial.print("STA ");
                }
                else
                {
                    Serial.print("ISR ");
                }

                Serial.print((int)fram.read8(ii));       // head
                Serial.print(" ");
                Serial.print((int)fram.read8(ii + 1));       // tail
                Serial.print(" ");
                Serial.print(framRead32(ii + 2)); //  millis
                Serial.print(" ");
                Serial.print((int)fram.read8(ii + 6));       // intCount
                Serial.print(" ");
                Serial.print(fram.read8(ii + 7), 2);   // shutterState
                Serial.print(" ");
                Serial.print(fram.read8(ii + 8), 16);   // waitingFor
                Serial.print(" ");
                ii += 9;
//                ii += 5;
//                }
//                else if (FHEADER_ISR == type)
//                {
//
//                    Serial.print(framRead32(ii)); //  millis
//                    Serial.print(" ");
//                    Serial.print(fram.read8(ii + 6), 16); // waitingFor
//                    Serial.print(" ");
//                    Serial.print(fram.read8(ii + 5), 16); // shutterstate
//                    Serial.print(" ");
//                    Serial.print((int)fram.read8(ii + 4)); // intcount
//                    ii += 7;
//                }
                Serial.print("\r\n");
            }
            break;

        case 'Q':
            framIndex = 0;
            break;
#endif // USEFRAM

        case 'd':
            filmMode = FILM_8MM;
            if (verbose)
            {
                Serial.println("8mm");
            }
            break;

        case 'D':
            filmMode = FILM_SUPER8;
            if (verbose)
            {
                Serial.println("Super 8");
            }
            break;

        case 'c':
            SHUTTERINT_ON;
            CAMERA_ON;
            if (verbose)
            {
                Serial.println("Camera on");
            }
            break;

        case 'C':
            CAMERA_OFF;
            if (verbose)
            {
                Serial.println("Camera off");
            }
            break;

        case 's':
            SHUTTER_OPEN;
            delay(2);
            SHUTTER_CLOSE;
            break;

        case 'S': // triple shutter
            if (FILM_NONE == filmMode)
            {
                Serial.println("Mode?");
            }
            else
            {
                waitingFor = SENSORSTART;
            }
            break;

        case 'T': // triple
            shutterState = 0xff;
            SHUTTERINT_ON;
            SHUTTER_OPEN;
            waitingFor = EXPOSURESERIES1;
            break;

        case 'x':
            Serial.print("optoIntTimeout ");
            Serial.print((int)optoIntTimeout);
            Serial.print("\r\nsensorValue ");
            Serial.print((int)sensorValue);
            //Serial.print("\r\nlastSensorValue ");
            //Serial.print((int)lastSensorValue);
            Serial.print("\r\nwaitingFor ");
            Serial.print((int)waitingFor);
//            Serial.print("\r\nstateLoop ");
//            Serial.print((int)stateLoop);
//            Serial.print("\r\nstateSaved ");
//            Serial.print((int)stateSaved);
            Serial.print("\r\nlastCommand ");
            Serial.print(lastCommand);
            Serial.print("\r\nframeCount ");
            Serial.print((int)frameCount);
            Serial.print("\r\nfilmMode ");
            Serial.print((int)filmMode);
            Serial.print("\r\nparameter ");
            Serial.print((int)motorPretensionNext);
            Serial.print("\r\nmotorPretensionNext ");
            Serial.print((int)parameter);
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

//        case 's':
//            setServo(parameter);
//            parameter = 0;
//            break;

        case 'm':
            if (parameter > 0)
            {
                setMotor(parameter);
                parameter = 0;
            }
            break;

        case 't': // auto pretension
#ifndef USESTEPPER
            setServo(SERVO_STOP);
#else
            stepperStop();
#endif // USESTEPPER
            {
//                setMotor(0);
//                delay(500);
//                setMotor(65);
//                delay(500);
                //Serial.println("autotension");
                for (motorPretensionNext = 60;
                    motorPretensionNext > 10 && !(PINC & 0x02);
                    motorPretensionNext -= 5)
                {
                    setMotor(motorPretensionNext);
//                    Serial.print("mpn ");
//                    Serial.print(motorPretensionNext, 10);
//                    Serial.print(" PINC ");
//                    Serial.print(PINC, 2);
//                    Serial.print("\r\n");
                    delay(250);
                }
            }
            break;

        case 'u': // untension
#ifndef USESTEPPER
            setServo(SERVO_STOP);
#else
            stepperStop();
#endif // USESTEPPER
            setMotor(MOTOR_OFF);
            break;

        case 'f': // forward
            if (frameCount)
            {
                SENSORVALUEINIT;
            }
            setMotor(MOTOR_PRETENSION_FF);
#ifndef USESTEPPER
            setServo(SERVO_MIN);
#else
            stepperGo();
#endif // USESTEPPER
            if (frameCount)
            {
                waitingFor = STOPONFRAMEZERO;
            }
            break;

//        case 'F': // backward
//            setMotor(MOTOR_REWIND_SLOW);
//#ifndef USESTEPPER
//            setServo(SERVO_REVERSE);
//#endif // USESTEPPER
//            break;

        case 'r':
            SENSORVALUEINIT;
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

        case 'V': // very verbose
            verbose = 2;
            break;

        case '[':
            motorPretensionNext = parameter;
            parameter = 0;
            break;

//        case ']':
//            servoSpeed = parameter;
//            parameter = 0;
//            break;
//
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
