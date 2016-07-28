#include <WProgram.h>
#include <avrlibtypes.h>
#ifdef USEFRAM
#include <Adafruit_FRAM_I2C-master/Adafruit_FRAM_I2C.h>
#endif // USEFRAM
#pragma GCC diagnostic ignored "-Wwrite-strings"

#define PB_CAMERABUSY 0
#define PC_SHUTTER 2
#define PD_CAMERAPOWER 7
#define PD_SHUTTERREADY 6
#define PB_SERVO 3
#define PB_LAMP 1
#define PIN_MOTOR 10  // PB2
#define PC_OPTOINT 1 // PC1
#define OPTOINT_TIMEOUT 5000

#define MOTOR_PRETENSION_NEXT 40 
#define MOTOR_PRETENSION_FF 20
#define MOTOR_REWIND_FAST 100 
#define MOTOR_REWIND_SLOW 70 
#define MOTOR_OFF 0 
#define SERVO_MIN 64
#define SERVO_STOP 94
#define SERVO_MAX 128
#define SERVO_NEXTFRAME_FAST 74 
//#define SERVO_NEXTFRAME_SLOW 70 
#define SERVO_NEXTFRAME_SLOW 84 
#define SERVO_NEXTFRAME_VERY_SLOW 87
#define SERVO_REVERSE 110 

#define CAMERABUSYTIMEOUT 1000
#define LAMP_TIMEOUT_MS 3000
#define NEXT_FRAME_TIMEOUT_MS 8000
#define SERVO_TIMEOUT_MS 2000
#define SENSORTHRESHOLD 200

#define LAMP_OFF PORTB &= ~_BV(PB_LAMP)
#define LAMP_ON PORTB |= _BV(PB_LAMP)
#define SERVO_ON PORTB |= _BV(PB_SERVO)
#define SERVO_OFF PORTB &= ~_BV(PB_SERVO)
#define SHUTTER_OPEN PORTC &= ~_BV(PC_SHUTTER)
#define SHUTTER_CLOSE PORTC |= _BV(PC_SHUTTER)
#define SENSORVALUEINIT { sensorValue = PINC & _BV(PC_OPTOINT); }
#define CAMERA_ON PORTD |= _BV(PD_CAMERAPOWER);
#define CAMERA_OFF PORTD &= ~_BV(PD_CAMERAPOWER);

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
        EXPOSURESERIES3,    // 8
        EXPOSURESERIES2,    // 9
        EXPOSURESERIES1,    // a
        SHUTTEROPEN,        // b
        OPTOINT_HALF,       // c
        OPTOINT_FULL,       // d
        OPTOINT_REVERSE,    // e
        OPTOINT_FULL2       // f
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
void incServo();
void decServo();
void setServo(u08 set);
void lampOn();
void lampOff();
void lampCheck();
void reset();

u16 parameter = 0;
volatile u32 intCount = 0;
u08 verbose = 1;
u08 servoPulse = SERVO_STOP;
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
u16 servoCount = 0;
u16 servoThreshold = 0;

#ifdef USEFRAM
Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C();
u16 framIndex;
//volatile u32 tmrNoOptoChange;

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

ISR(PCINT2_vect)
{
    intCount++;
    if (PIND & _BV(PD_SHUTTERREADY)) //  && (0 == shutterState & 0x01))
    {
        shutterState |= 1;
    }
    shutterState <<= 1;
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
    optoIntTimeout = 0;
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
    setServo(SERVO_STOP);
    lampOff();
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
    framIndex = 0;
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
    PORTD |= _BV(PD_SHUTTERREADY);
    DDRD |= _BV(PD_CAMERAPOWER);
    DDRB |= _BV(PB_LAMP);
    DDRC |= _BV(PC_SHUTTER);
    CAMERA_OFF;
    lampOff();
    cli();
    
    PCMSK2 |= _BV(PCINT22);
    PCICR |= _BV(PCIE2);

    TCCR2B |= _BV(CS20); // no prescaler   
    TIFR2 &= ~_BV(TOV2); // clear overflow interrupt flag
    TIMSK2 |= _BV(TOIE2); // enable counter2 overflow interrupt
    OCR2A = SERVO_STOP;
    servoPulse = SERVO_STOP;
    TCNT2 = 0x00;
    DDRB |= _BV(PB_SERVO);
    sei();
#ifdef USEFRAM
    if (fram.begin())
    {
        Serial.println("FRAM ok");
    }
    framIndex = 0;
#endif // USEFRAM
    Serial.println("Init OK");
}

u32 countHi = 0;
u32 countLo = 0;

void loop ()
{
    lampCheck();

    switch (waitingFor)
    {
        case NONE:
            break;

        case FRAMESTOP:
#ifdef USEFRAM
#ifdef LOGSTATE
            framWrite8(0xf0 | waitingFor);
            framWrite8(shutterState);
            framWrite32(intCount);
#endif // LOGSTATE
#endif // USEFRAM
            if (2 == verbose)
            {
                Serial.println("FRAMESTOP");
            }
            setServo(SERVO_STOP);
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
                waitingFor = SHUTTEROPEN;
            }
            break;
            
        case OPTOINT_HALF:
#ifdef USEFRAM
#ifdef LOGSTATE
            framWrite8(0xf0 | waitingFor);
            framWrite8(shutterState);
            framWrite32(intCount);
#endif // LOGSTATE
#endif // USEFRAM
            if (2 == verbose)
            {
                Serial.println("OPTOINT_HALF");
            }
            if (HALFFRAME == isOptoTransition())
            {
                setServo(SERVO_NEXTFRAME_SLOW);
                waitingFor = OPTOINT_FULL;
            }
            break;

        case OPTOINT_FULL:
#ifdef USEFRAM
#ifdef LOGSTATE
            framWrite8(0xf0 | waitingFor);
            framWrite8(shutterState);
            framWrite32(intCount);
#endif // LOGSTATE
#endif // USEFRAM
            if (2 == verbose)
            {
                Serial.println("OPTOINT_FULL");
            }
            if (FULLFRAME == isOptoTransition())
            {
                setMotor(MOTOR_REWIND_SLOW);
                setServo(SERVO_REVERSE);
                waitingFor = OPTOINT_REVERSE;
            }
            break;

        case OPTOINT_REVERSE:
#ifdef USEFRAM
#ifdef LOGSTATE
            framWrite8(0xf0 | waitingFor);
            framWrite8(shutterState);
            framWrite32(intCount);
#endif // LOGSTATE
#endif // USEFRAM
            if (2 == verbose)
            {
                Serial.println("OPTOINT_REVERSE");
            }
            if (HALFFRAME == isOptoTransition())
            {
                setMotor(motorPretensionNext);
                setServo(SERVO_NEXTFRAME_VERY_SLOW);
                waitingFor = OPTOINT_FULL2;
            }
            break;

        case OPTOINT_FULL2:
#ifdef USEFRAM
            framWrite8(0xf0 | waitingFor);
            framWrite8(shutterState);
            framWrite32(intCount);
#endif // USEFRAM
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
#ifdef USEFRAM
            framWrite8(0xf0 | waitingFor);
            framWrite8(shutterState);
            framWrite32(intCount);
#endif // USEFRAM
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
                    setServo(SERVO_REVERSE);
                    //DELAYEDSTATE(1000, REVERSESEEK);
                    break;
                case 2: setServo(SERVO_NEXTFRAME_SLOW); break;
            }
//            Serial.print("OIC ");
//            Serial.println(millis());
            break;

        case SENSORSTART:
#ifdef USEFRAM
            framWrite8(0xf0 | waitingFor);
            framWrite8(shutterState);
            framWrite32(intCount);
#endif // USEFRAM
            if (2 == verbose)
            {
                Serial.println("SENSORSTART");
            }
            if (frameCount > 0)
            {
                setMotor(motorPretensionNext);
            }
            setServo(SERVO_NEXTFRAME_FAST);
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
#ifdef USEFRAM
            framWrite8(0xf0 | waitingFor);
            framWrite8(shutterState);
            framWrite32(intCount);
#endif // USEFRAM
            if (2 == verbose)
            {
                Serial.println("SHUTTERCLOSED");
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

        case EXPOSURESERIES4:
#ifdef USEFRAM
            framWrite8(0xf0 | waitingFor);
            framWrite8(shutterState);
            framWrite32(intCount);
#endif // USEFRAM
            lampOff();
            cameraBusyTimeout = millis();
            waitingFor = SHUTTERCLOSED;
            break;

        case EXPOSURESERIES3:
#ifdef USEFRAM
            framWrite8(0xf0 | waitingFor);
            framWrite8(shutterState);
            framWrite32(intCount);
#endif // USEFRAM
            if (0x10 != shutterState)
            {
                break;
            }
            shutterState = 0;
            lampOn();
            delay(60);
            lampOff();
            waitingFor = EXPOSURESERIES4;
            break;

        case EXPOSURESERIES2:
#ifdef USEFRAM
            framWrite8(0xf0 | waitingFor);
            framWrite8(shutterState);
            framWrite32(intCount);
#endif // USEFRAM
            if (0x10 != shutterState)
            {
                break;
            }
            shutterState = 0;
            lampOn();
            delay(20);
            lampOff();
            waitingFor = EXPOSURESERIES3;
            break;

        case EXPOSURESERIES1:
#ifdef USEFRAM
#ifdef LOGSTATE
            framWrite8(0xf0 | waitingFor);
            framWrite8(shutterState);
            framWrite32(intCount);
#endif // LOGSTATE
#endif // USEFRAM
            if (0x10 != shutterState)
            {
                break;
            }
            shutterState = 0;
            lampOn();
            delay(4);
            lampOff();
            waitingFor = EXPOSURESERIES2;
            break;

        case SHUTTEROPEN:
#ifdef USEFRAM
#ifdef LOGSTATE
            framWrite8(0xf0 | waitingFor);
            framWrite8(shutterState);
            framWrite32(intCount);
#endif // LOGSTATE
#endif // USEFRAM
            SHUTTER_OPEN;
            shutterState = 0;
            waitingFor = EXPOSURESERIES1;
//            delay(100);
//            lampOn();
//            delay(4);
//            lampOff();
//            DELAYEDSTATE(350, EXPOSURESERIES2);
            break;

       case STOPONFRAMEZERO:
#ifdef USEFRAM
#ifdef LOGSTATE
            framWrite8(0xf0 | waitingFor);
            framWrite8(shutterState);
            framWrite32(intCount);
#endif // LOGSTATE
#endif // USEFRAM
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
    switch (lastCommand)
    {
#ifdef USEFRAM
        case 'q':
            Serial.println((int)framIndex);
//            Serial.println((int)tmrNoOptoChange);
            for (u16 ii = 0; ii < framIndex; ii += 6)
            {
                Serial.print(fram.read8(ii), 16);
                Serial.print(" ");
                Serial.print(fram.read8(ii + 1), 16);
                Serial.print(" ");
                Serial.print(framRead32(ii + 2));
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
                SENSORVALUEINIT;
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
