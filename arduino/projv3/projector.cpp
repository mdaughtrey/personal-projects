#include <WProgram.h>

#include <avrlibtypes.h>
#include <stepper.h>
#ifdef USEFRAM
#define LOGSTATE
#include <Adafruit_FRAM_I2C-master/Adafruit_FRAM_I2C.h>
#endif // USEFRAM
#pragma GCC diagnostic ignored "-Wwrite-strings"

// -4: PC2 Rewind
// 11: PD6 Reset
//  8: PB0 Pretension
//  4: PB4 Next
#define PC_REWIND 3
#define PD_RESET 6
#define PB_PRETENSION 0
#define PB_NEXT 4
u08 pbState = 0xff;
u08 pcState = 0xff;
u08 pdState = 0xff;

#define PB_LAMP 1
#define PIN_MOTOR 10  // PB2
//#define PB_SIGTRIPLE 5

#define PC_TENSIONSENSOR 0 // PC0
#define PC_OPTOINT 1 // PC1 PCINT9
//#define PC_SHUTTER 2

//#define PD_SHUTTERREADY 7
#define OPTOINT_TIMEOUT 10000

#define MOTOR_PRETENSION_NEXT 40 
#define MOTOR_PRETENSION_MAX 70 
#define MOTOR_PRETENSION_FF 20
#define MOTOR_REWIND_FAST 100 
#define MOTOR_REWIND_SLOW 70 
#define MOTOR_OFF 0 
#define PRETENSIONDELAY 200
#define LAMPDEFER 0
#define LAMP_TIMEOUT_MS 3000
#define NEXT_FRAME_TIMEOUT_MS 8000
#define SENSORTHRESHOLD 200
#define LAMP_OFF PORTB &= ~_BV(PB_LAMP)
#define LAMP_ON PORTB |= _BV(PB_LAMP)
//#define SHUTTER_READY PIND & _BV(PD_SHUTTERREADY)
#define SENSORVALUEINIT { sensorValue = PINC & _BV(PC_OPTOINT); }
//#define SHUTTERINT_ON { PCMSK2 |= _BV(PCINT22); PCICR |= _BV(PCIE2); }
//#define SHUTTERINT_OFF { PCMSK2 &= ~_BV(PCINT22); PCICR &= ~_BV(PCIE2); }
//#define OPTOINT_ON { PCMSK1 |= _BV(PCINT9; PCICR |= _BV(PCIE1); }
//#define OPTOINT_OFF { PCMSK1 &= ~_BV(PCINT9; PCICR &= ~_BV(PCIE1); }

#ifdef USEFRAM
#define FHEADER_ISR 0x5a
#define FHEADER_STATE 0xa5
#endif // USEFRAM

typedef enum
{ 
        NONE = 0,           
        //FRAMESTOP,          
        SENSORSTART,        
//        OPTOINT_CHANGED,    
        STOPONFRAMEZERO,    
        PRETENSION,
        OPTOINT_HALF,       
        OPTOINT_FULL,       
//        OPTOINT_REVERSE,    
//        OPTOINT_FULL2
} WaitFor;

typedef enum
{
    FILM_NONE = 0,
    FILM_8MM,
    FILM_SUPER8
} FilmMode;

void setMotor(u08 set); void lampOn();
void lampOff();
void lampCheck();
void init();

u16 parameter = 0;
volatile u08 intCount = 0;
volatile u08 lastIntCount = 0;
u08 verbose = 0;
u08 motorPulse = 255;
u08 pretension = MOTOR_PRETENSION_NEXT;
volatile u32 optoIntTimeout;
u32 cameraBusyTimeout;
u08 sensorValue;
u08 waitingFor(NONE);
u08 lastCommand;
u08 filmMode = FILM_NONE;
u08 stepperDelay = 1;

#ifdef USEFRAM
Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C();
u16 framIndex;

typedef struct
{
    u08 head;
    u08 tail;
    u32 millis;
    u08 intCount;
    u08 waitingFor;
} IntEntry;

IntEntry intQueue[4];
volatile u08 iqHead = 0;
volatile u08 iqTail = 0;

ISR(PCINT9_vect)
{
}

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
#endif // 0
void setMotor(u08 set)
{
    motorPulse = set;
    analogWrite(PIN_MOTOR, motorPulse);
}

void lampOn()
{
    LAMP_ON;
}

void lampOff()
{
    LAMP_OFF;
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
        Serial.print("{OIT:");
        Serial.print(now - optoIntTimeout, 10);
        Serial.println("}");
        init();
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

void init()
{
    u08 ii;
//    Serial.println("Reset");
    stepperInit();
    lampOff();
    setMotor(MOTOR_OFF);
    parameter = 0;
    waitingFor = NONE;
    verbose = 0;
#ifdef USEFRAM
    framIndex = 0;
#endif // USEFRAM
    stepperDelay = 1;
    for (ii = 0; ii < 5; ii++)
    {
        lampOn();
        delay(80);
        lampOff();
        delay(80);
    }
}

void setup ()
{ 
    Serial.begin(57600);
    Serial.println("Init Start");
    lastCommand = 0;
    PORTC |= _BV(PC_OPTOINT) | _BV(PC_REWIND); // tie opt int sensor line hight
    PORTB |= _BV(PB_PRETENSION) | _BV(PB_NEXT);
    //PORTD |= _BV(PD_RESET);
    PORTD |= _BV(PD_RESET);

    analogWrite(PIN_MOTOR, MOTOR_OFF);
    //PORTC |= _BV(PC_SHUTTER) | _BV(PC_TENSIONSENSOR);
    PORTC |= _BV(PC_TENSIONSENSOR);
    //DDRB |= _BV(PB_LAMP) | _BV(PB_SIGTRIPLE);
    DDRB |= _BV(PB_LAMP);
//    DDRC |= _BV(PC_SHUTTER);
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
    init();
    Serial.println("{State:Ready}");
}

#ifdef LOGSTATE
u08 lastReportedState(255);
#endif // LOGSTATE

bool buttonTest(u08 pins, u08 * state, u08 testBit)
{
    if (!(pins & _BV(testBit)))
    {
        if (*state & _BV(testBit))
        { 
            *state &= ~_BV(testBit);
            return true;
        }
        return false;
    }
    *state |= _BV(testBit); 
    return false;
}

u08 lastState = 0;
void loop ()
{
//    if(!lastState)
//    {
//        if(PINC & _BV(PC_OPTOINT)) { Serial.print("0"); lastState = 1; }
//    }
//    else
//    {
//        if(!(PINC & _BV(PC_OPTOINT))) { Serial.print("1"); lastState = 0; }
//    }
    stepperPoll(stepperDelay);

    switch (waitingFor)
    {
        case NONE:
            break;

//        case FRAMESTOP:
//            stepperStop();
//            if (1 == verbose)
//            {
//                Serial.println("FRAMESTOP");
//            }
//            if ('n' == lastCommand)
//            {
//                waitingFor = NONE;
//            }
//            break;
            
        case OPTOINT_HALF:
            if (2 == verbose)
            {
                Serial.println("OPTOINT_HALF");
            }
            if (HALFFRAME == isOptoTransition())
            {
                //OPTOINT_ON;
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
               // setMotor(pretension);
                stepperStop();
                if (1 == verbose)
                {
                    Serial.println("FRAMESTOP");
                }
                //waitingFor = FRAMESTOP;
                waitingFor = NONE;
            }
            break;

//        case OPTOINT_REVERSE:
//            if (2 == verbose)
//            {
//                Serial.println("OPTOINT_REVERSE");
//            }
//            if (HALFFRAME == isOptoTransition())
//            {
//                setMotor(pretension);
//                waitingFor = OPTOINT_FULL2;
//            }
//            break;

//        case OPTOINT_FULL2:
//            if (2 == verbose)
//            {
//                Serial.println("OPTOINT_FULL2");
//            }
//            if (FULLFRAME == isOptoTransition())
//            {
//                waitingFor = FRAMESTOP;
//            }
//            break;

//        case OPTOINT_CHANGED:
//            if (2 == verbose)
//            {
//                Serial.println("OPTOINT_CHANGED");
//            }
//            switch (isOptoTransition())
//            {
//                case 1:
//                    optoIntTimeout = 0;
//                    waitingFor = OPTOINT_HALF;
//                    setMotor(MOTOR_REWIND_SLOW);
//                    break;
//            }
//            break;

        case SENSORSTART:
            if (2 == verbose)
            {
                Serial.println("SENSORSTART");
            }
            stepperGo();
            SENSORVALUEINIT;
            optoIntTimeout = 0;
            waitingFor = OPTOINT_HALF;
            break;

        case PRETENSION:
            if (MOTOR_PRETENSION_NEXT == pretension)
            {
                stepperStop();
                pretension++;
                break;
            }
            if (pretension < 70)
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
                    waitingFor = NONE;
                    pretension /= 5;
                    pretension *= 4;
                    setMotor(pretension);
                    Serial.print("{pt:");
                    Serial.print(pretension, 10);
                    Serial.println("}");
                    break;
                }
                pretension += 4;
            }
            break;

       case STOPONFRAMEZERO:
            if (isOptoTransition())
            {
                {
                    setMotor(MOTOR_OFF);
                    waitingFor = NONE;
                }
            }
        break;

        default:
            break;
    }
    if (buttonTest(PINB, &pbState, PB_PRETENSION))
    {
        waitingFor = PRETENSION;
        pretension = MOTOR_PRETENSION_NEXT;
        return;
    }
    else
    if (buttonTest(PINB, &pbState, PB_NEXT))
    {
        lastCommand = 'n'; 
    }
    else if (buttonTest(PIND, &pdState, PD_RESET))
    {
        lastCommand = ' '; 
    }
    else if (buttonTest(PINC, &pcState, PC_REWIND))
    {
        lastCommand = 'r'; 
    }
    else if (Serial.available())
    {
        lastCommand = Serial.read();
    }
    else return;

    switch (lastCommand)
    {
        case 's':
        if (parameter > 0)
        {
            stepperDelay = parameter;
        }
        break;

        case 'd':
            filmMode = FILM_8MM;
            Serial.println("{mode:8mm}");
            break;

        case 'D':
            filmMode = FILM_SUPER8;
            Serial.println("{mode:super8}");
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
            Serial.print("\r\nstepperDelay ");
            Serial.print((int)stepperDelay);
            Serial.print("\r\nfilmMode ");
            Serial.print((int)filmMode);
            Serial.print("\r\nparameter ");
            Serial.print((int)parameter);
            Serial.print("\r\npretension ");
            Serial.print((int)pretension);
            Serial.print("\r\nSense ");
            Serial.print(PINC & _BV(PC_TENSIONSENSOR) ? 1 : 0);
            Serial.print("\r\n");
            break;

        case ' ':
            Serial.println("RESET");
            PORTD |= _BV(PD_RESET);
            DDRD |= _BV(PD_RESET);
            PORTD &= ~_BV(PD_RESET);
            break;

            break;

        case 'l':
            //LAMP_ON;
            lampOn();
            break;

        case 'L':
            //LAMP_OFF;
            lampOff();
            break;

        case 'T': // manual pretension
            pretension = parameter;
            setMotor(pretension);
            break;

        case 't': // auto pretension
            optoIntTimeout = millis();
            pretension = MOTOR_PRETENSION_NEXT;
            waitingFor = PRETENSION;
            break;


        case 'u': // untension
            stepperStop();
            setMotor(MOTOR_OFF);
            break;

        case 'f': // forward
            setMotor(MOTOR_PRETENSION_FF);
            stepperGo();
            break;

        case 'r':
            SENSORVALUEINIT;
            setMotor(MOTOR_REWIND_FAST);
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
