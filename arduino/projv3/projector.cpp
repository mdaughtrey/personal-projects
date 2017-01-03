#include <WProgram.h>
#include <avrlibtypes.h>
#include <stepper.h>
#ifdef USEFRAM
#define LOGSTATE
#include <Adafruit_FRAM_I2C-master/Adafruit_FRAM_I2C.h>
#endif // USEFRAM
#pragma GCC diagnostic ignored "-Wwrite-strings"
//#define READY2

//#define PB_CAMERABUSY 0
#define PB_SHUTTERREADY 0
#define PB_LAMP 1
#define PIN_MOTOR 10  // PB2
#define PB_SIGTRIPLE 5

#define PC_TENSIONSENSOR 0 // PC0
#define PC_OPTOINT 1 // PC1
#define PC_SHUTTER 2

#define PD_CAMERAPOWER 7
#define OPTOINT_TIMEOUT 3000

#define MOTOR_PRETENSION_NEXT 40 
#define MOTOR_PRETENSION_FF 20
#define MOTOR_REWIND_FAST 100 
#define MOTOR_REWIND_SLOW 70 
#define MOTOR_OFF 0 
#define PRETENSIONDELAY 200
#define LAMPDEFER 0

//#define CAMERABUSYTIMEOUT 1200
#define LAMP_TIMEOUT_MS 3000
#define NEXT_FRAME_TIMEOUT_MS 8000
#define SENSORTHRESHOLD 200
#define LAMP_OFF PORTB &= ~_BV(PB_LAMP)
#define LAMP_ON PORTB |= _BV(PB_LAMP)

#define SHUTTER_READY PINB & _BV(PB_SHUTTERREADY)
#define SENSORVALUEINIT { sensorValue = PINC & _BV(PC_OPTOINT); }
#define CAMERA_ON PORTD |= _BV(PD_CAMERAPOWER);
#define CAMERA_OFF PORTD &= ~_BV(PD_CAMERAPOWER);
#define SHUTTERINT_ON { PCMSK2 |= _BV(PCINT22); PCICR |= _BV(PCIE2); }
#define SHUTTERINT_OFF { PCMSK2 &= ~_BV(PCINT22); PCICR &= ~_BV(PCIE2); }
#define RESETSIG_TRIPLE { PORTB &= ~_BV(PB_SIGTRIPLE); }
#define SETSIG_TRIPLE { PORTB |= _BV(PB_SIGTRIPLE); }

#ifdef USEFRAM
#define FHEADER_ISR 0x5a
#define FHEADER_STATE 0xa5
#endif // USEFRAM

//#define DELAYEDSTATE(dd, ss) { stateLoop = dd/10; stateSaved = ss; waitingFor = DELAYLOOP; }

typedef enum
{ 
        NONE = 0,           
        FRAMESTOP,          
        SENSORSTART,        
        OPTOINT_CHANGED,    
        STOPONFRAMEZERO,    
        SHUTTERLED1,
        SHUTTERLED1A,
        SHUTTERLED2,
        SHUTTERLED2A,
        SHUTTERLED3,
        SHUTTERLED3A,
        OPTOINT_HALF,       
        OPTOINT_FULL,       
        OPTOINT_REVERSE,    
        OPTOINT_FULL2
//        WAITCAMERABUSY      
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

void lampOn();
void lampOff();
void lampCheck();
void reset();

u16 parameter = 0;
volatile u08 intCount = 0;
volatile u08 lastIntCount = 0;
u08 verbose = 0;
u08 motorPulse = 255;
u08 pretension = MOTOR_PRETENSION_NEXT;
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
//    u08 shutterState;
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


void setMotor(u08 set)
{
    motorPulse = set;
    analogWrite(PIN_MOTOR, motorPulse);
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
        stepperStop();
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
//        if (1 == verbose)
//        {
//            Serial.print("Opto int timeout ");
//            Serial.print(optoIntTimeout);
//            Serial.print(" vs ");
//            Serial.println(now);
//        }
        // JSON
        Serial.print("{OIT:");
        Serial.print(now - optoIntTimeout, 10);
        Serial.println("}");
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
    stepperInit();
    lampOff();
    //SHUTTERINT_OFF;
    CAMERA_OFF;
    //SHUTTER_CLOSE;
    setMotor(MOTOR_OFF);
    //motorRewind = MOTOR_OFF;
    parameter = 0;
    waitingFor = NONE;
    verbose = 0;
    frameCount = 0;
    //optoIntTimeout = 0;
    //shutterState = 0;
#ifdef USEFRAM
    framIndex = 0;
#endif // USEFRAM
}

u08 lampDefer = LAMPDEFER;
u08 lampDuration = 0;

void setup ()
{ 
    Serial.begin(57600);
    Serial.println("Init Start");
    PORTC |= _BV(PC_OPTOINT); // tie opt int sensor line hight

    analogWrite(PIN_MOTOR, MOTOR_OFF);
    PORTC |= _BV(PC_SHUTTER) | _BV(PC_TENSIONSENSOR);
    PORTD |= _BV(PD_CAMERAPOWER);
    DDRD |= _BV(PD_CAMERAPOWER);
    DDRB |= _BV(PB_LAMP) | _BV(PB_SIGTRIPLE);
    DDRC |= _BV(PC_SHUTTER);
    CAMERA_OFF;
    lampOff();

    stepperInit();
    //shutterState = 0xaa;
#ifdef USEFRAM
    //Serial.println("Init FRAM");
    if (fram.begin())
    {
        Serial.println("FRAM ok");
    }
    framIndex = 0;
    iqTail = 0;
    iqHead = 0;
#endif // USEFRAM
    RESETSIG_TRIPLE;
    Serial.println("{State:Ready}");
}

#ifdef LOGSTATE
u08 lastReportedState(255);
#endif // LOGSTATE

void loop ()
{
    stepperPoll();
    lampCheck();

    switch (waitingFor)
    {
        case NONE:
            break;

        case FRAMESTOP:
            if (2 == verbose)
            {
                Serial.println("FRAMESTOP");
            }
            stepperStop();
            if (frameCount == 0)
            {
                setMotor(MOTOR_OFF);
            }
            if ('n' == lastCommand)
            {
                waitingFor = NONE;
            }
//            else
//            {
//                //waitingFor = SHUTTEROPEN;
//                cameraBusyTimeout = millis();
//                waitingFor = WAITCAMERABUSY;
//                Serial.print("{OIC:");
//                Serial.print(millis() - optoIntTimeout, 10);
//                Serial.println("}");
//                if ((millis() - optoIntTimeout) < 750)
//                {
//                    stepperDelay(1);
//                }
//            }
            break;

//        case WAITCAMERABUSY:
//            if (2 == verbose)
//            {
//                Serial.println("WAITCAMERABUSY");
//            }
//            if (!(PINB & _BV(PB_CAMERABUSY)))
//            {
//                if (2 == verbose)
//                {
//                    Serial.println("Camera Busy");
//                }
//                cameraBusyTimeout = millis();
//                break;
//            }
//
//            if ((millis() - cameraBusyTimeout) < CAMERABUSYTIMEOUT)
//            {
//                if (2 == verbose)
//                {
//                    Serial.println("Camera Not Ready");
//                }
//                break;
//            }
//
//            if (verbose)
//            {
//                Serial.println("Camera Ready");
//            }
//            if (0 == frameCount)
//            {
//                Serial.println("{Remaining:0}");
////                Serial.print(frameCount, 10);
////                Serial.println("}");
//                waitingFor = NONE;
//            }
//            else
//            {
////                waitingFor = SHUTTEROPEN;
//            }
//            break;
            
        case OPTOINT_HALF:
            if (2 == verbose)
            {
                Serial.println("OPTOINT_HALF");
            }
            if (HALFFRAME == isOptoTransition())
            {
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
                setMotor(pretension);
                waitingFor = FRAMESTOP;
            }
            break;

        case OPTOINT_REVERSE:
            if (2 == verbose)
            {
                Serial.println("OPTOINT_REVERSE");
            }
            if (HALFFRAME == isOptoTransition())
            {
                setMotor(pretension);
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
                    //DELAYEDSTATE(1000, REVERSESEEK);
                    break;
            }
            break;

        case SENSORSTART:
            if (2 == verbose)
            {
                Serial.println("SENSORSTART");
            }
            if (frameCount > 0)
            {
                setMotor(pretension);
            }
            stepperGo();
            SENSORVALUEINIT;
            optoIntTimeout = 0;
            waitingFor = OPTOINT_HALF;
            break;

        case SHUTTERLED3A:
            if (SHUTTER_READY)
            {
                break;
            }
            if (verbose) { Serial.println("SL3A"); }
//            Serial.println("{TRIPLE:DONE}"); TRIPLE_COMPLETE
            SETSIG_TRIPLE;
            waitingFor = NONE;
            break;

        case SHUTTERLED3: 
            if (SHUTTER_READY)
            {
                Serial.println("{TRIPLE:DONE}");
                delay(lampDefer);
                lampOn();
                delay(30);
                lampOff();
                //waitingFor = SHUTTERLED3A;
                SETSIG_TRIPLE;
                if (verbose) { Serial.println("SETSIG_TRIPLE"); }
                waitingFor = NONE;;
            }
            break;

        case SHUTTERLED2A:
            if (SHUTTER_READY)
            {
                break;
            }
            if (verbose) { Serial.println("SL2A"); }
            waitingFor = SHUTTERLED3;
            break;

        case SHUTTERLED2: 
            if (SHUTTER_READY)
            {
                delay(lampDefer);
                lampOn();
                delay(10);
                lampOff();
                waitingFor = SHUTTERLED2A;
            }
            break;

        case SHUTTERLED1A:
            if (SHUTTER_READY)
            {
                break;
            }
            if (verbose) { Serial.println("SL1A"); }
            waitingFor = SHUTTERLED2;
            break;

        case SHUTTERLED1: 
            if (SHUTTER_READY)
            {
                delay(lampDefer);
                lampOn();
                delay(2);
                lampOff();
                waitingFor = SHUTTERLED1A;
            }
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
        case 's':
            SETSIG_TRIPLE;
            break;

        case 'S':
            RESETSIG_TRIPLE;
            break;


        case 'd':
            filmMode = FILM_8MM;
            Serial.println("{mode:8mm}");
            break;

        case 'D':
            filmMode = FILM_SUPER8;
            Serial.println("{mode:super8}");
            break;

        case 'c':
            //SHUTTERINT_ON;
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

        case 'x':
            Serial.print("optoIntTimeout ");
            Serial.print((int)optoIntTimeout);
            Serial.print("\r\nsensorValue ");
            Serial.print((int)sensorValue);
            Serial.print("\r\nwaitingFor ");
            Serial.print((int)waitingFor);
            Serial.print("\r\nlastCommand ");
            Serial.print(lastCommand);
            Serial.print("\r\nframeCount ");
            Serial.print((int)frameCount);
            Serial.print("\r\nfilmMode ");
            Serial.print((int)filmMode);
            Serial.print("\r\nparameter ");
            Serial.print((int)parameter);
            Serial.print("\r\npretension ");
            Serial.print((int)pretension);
            Serial.print("\r\nSense ");
            Serial.print(PINC & _BV(PC_TENSIONSENSOR) ? 1 : 0);
            Serial.print("\r\nLampDefer ");
            Serial.print((int)lampDefer);
            Serial.print("\r\n");
            break;

        case ' ':
            reset();
            break;

        case 'm': // lamp flash duration
            if (parameter > 0)
            {
                lampDuration = parameter;
                parameter = 0;
            }
            
            break;

        case 'a': // durations 2, 10, 30ms if (parameter > 0) //  && lampDuration > 0)
            waitingFor = SHUTTERLED1;
            lampDefer = parameter;
            parameter = 0;
            if (verbose) { Serial.println("RESETSIG_TRIPLE"); }
            RESETSIG_TRIPLE;
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

        case 't': // auto pretension
            stepperStop();
            {
                pretension = 10;
                while (pretension < 70)
                {
                    setMotor(10);
                    delay(PRETENSIONDELAY);
                    setMotor(70);
                    delay(PRETENSIONDELAY);
                    setMotor(10);
                    delay(PRETENSIONDELAY);
                    setMotor(pretension);
                    delay(PRETENSIONDELAY);
                    if (verbose)
                    {
                        Serial.print("PT ");
                        Serial.print(pretension, 10);
                        Serial.print(" Sense ");
                        Serial.print(PINC & _BV(PC_TENSIONSENSOR) ? 1 : 0);
                        Serial.print("\r\n");
                    }
                    if (PINC & _BV(PC_TENSIONSENSOR))
                    {
                        break;
                    }
                    pretension += 10;
                }
                Serial.print("{pt:");
                Serial.print(pretension, 10);
                Serial.println("}");
            }
            break;

        case 'u': // untension
            stepperStop();
            setMotor(MOTOR_OFF);
            break;

        case 'f': // forward
            if (frameCount)
            {
                SENSORVALUEINIT;
            }
            setMotor(MOTOR_PRETENSION_FF);
            stepperGo();
            if (frameCount)
            {
                waitingFor = STOPONFRAMEZERO;
            }
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
            pretension = parameter;
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
            if (verbose)
            {
                Serial.print(parameter, 10);
            }
            break;
    }
    return;
}
