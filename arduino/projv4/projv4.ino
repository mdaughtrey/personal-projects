#define ACCELSTEPPER
#include <stdint.h>
#include <stdarg.h>
#ifndef ACCELSTEPPER
#include "stepper.h"
#else
#include "AccelStepper.h"
#endif // ACCELSTEPPER
#include <AsyncTimer.h>

#define FSH(s) reinterpret_cast<const __FlashStringHelper *>(s)

#ifdef ACCELSTEPPER
const int stepperPin4 = 18;
const int stepperPin3 = 19;
const int stepperPin2 = 20;
const int stepperPin1 = 21;
#endif //  ACCELSTEPPER

typedef enum
{
    NONE = 0,
    HUNT,
    NEXT
} State;

typedef enum
{
    OPTO_NONE = 0,
    OPTO_RISING,
    OPTO_FALLING,
    OPTO_BOTH
} OptoEdge;

typedef struct
{
    char key;
    const __FlashStringHelper * help;
    //void (*help)();
    void (*fun)();
}Command;

struct {
#ifdef ACCELSTEPPER
    float fastStepDelay;
    float slowStepDelay;
    float currStepDelay;
#else //  ACCELSTEPPER
    uint8_t fastStepDelay;
    uint8_t slowStepDelay;
    uint8_t currStepDelay;
#endif // ACCELSTEPPER
    int16_t param;
    uint16_t encoderLimit;
    uint16_t slowEncoderThreshold;
    uint8_t tension;
    uint8_t verbose;
    int16_t stepCount;
    uint8_t isrEdge;
    uint8_t cw;
    uint16_t encoderTO;
    uint32_t encoderTime;
    uint8_t state;
    uint8_t stepperRun;
} config;

AsyncTimer at;

uint8_t frameReady = 0;
uint8_t lastCommand;
uint8_t ledState;
uint32_t ledTime = 0;

const uint8_t ButtonPin0 = 2;
const uint8_t ButtonPin1 = 3;
const uint8_t ButtonPin2 = 4;
const uint8_t ButtonPin3 = 5;

const uint8_t EncoderPin0 = 14;
const uint8_t EncoderPin1 = 15;

const uint8_t OutputPin0 = 10; // fan
const uint8_t OutputPin1 = 11; // lamp
const uint8_t OutputPin2 = 12;
const uint8_t OutputPin3 = 13; // rewindMotor

const uint8_t LedPin = 25;
Command * commandset;
void help();
extern Command commands_stepper[];
extern Command commands_main[];
//StreamEx serout = Serial;

void fan(uint8_t val) { digitalWrite(OutputPin0, val); }
void lamp(uint8_t val) { digitalWrite(OutputPin1, val); }
void rewindMotor(uint8_t val) { analogWrite(OutputPin3, val); }
#ifdef ACCELSTEPPER
void fastStepDelay() { config.fastStepDelay = config.param * 1.0; config.param = 0;}
void fastStepDelay(uint8_t sd) { config.fastStepDelay = sd * 1.0; config.param = 0;}
void stepFast() { if (config.fastStepDelay) config.currStepDelay = config.fastStepDelay; }
void stepSlow() { if (config.slowStepDelay) config.currStepDelay = config.slowStepDelay; }
#else // ACCELSTEPPER
void cw() { config.cw = 1; stepper::cw(); }
void ccw() { config.cw = 0; stepper::ccw(); }
void fastStepDelay() { config.fastStepDelay = config.param; config.param = 0;}
void fastStepDelay(uint8_t sd) { config.fastStepDelay = sd; config.param = 0;}
void stepFast() { if (config.fastStepDelay) config.currStepDelay = config.fastStepDelay; }
void stepSlow() { if (config.slowStepDelay) config.currStepDelay = config.slowStepDelay; }
void slowStepDelay() { config.slowStepDelay = config.param; config.param = 0;}
void slowStepDelay(uint8_t sd) { config.slowStepDelay = sd; config.param = 0;}
#endif // ACCELSTEPPER
void stepcount() { config.stepCount = config.param; config.param = 0; }
void stepcount(int16_t sc) { config.stepCount = sc; config.param = 0; }
void encoderStart() { config.encoderTime = millis(); }
void encoderTO() { config.encoderTO = config.param; config.param = 0; }

volatile int encoderPos = 0;
volatile int isr1count = 0;
volatile int isr2count = 0;
void handleCommand();
void coilControl();
void (*commandHandler)() = handleCommand;

#ifdef ACCELSTEPPER
//AccelStepper stepper(AccelStepper::FULL4WIRE, stepperPin1, stepperPin3, stepperPin2, stepperPin4);
AccelStepper stepper(AccelStepper::FULL4WIRE, stepperPin3, stepperPin1, stepperPin4, stepperPin2);
//AccelStepper stepper(AccelStepper::FULL4WIRE, stepperPin1, stepperPin2, stepperPin3, stepperPin4);
//AccelStepper stepper(AccelStepper::FULL4WIRE, stepperPin4, stepperPin3, stepperPin2, stepperPin1);
//AccelStepper stepper(AccelStepper::FULL4WIRE, stepperPin2, stepperPin4, stepperPin1, stepperPin3);
#endif //  ACCELSTEPPER

void verbose(uint8_t threshold, const char * fmt, ...)
{
    if (threshold > config.verbose) return;
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    Serial.print(buffer);
}

void setFrameReady()
{
    frameReady = 1;
#ifndef ACCELSTEPPER
    stepcount(0);
//    stepper::stop(); 
#endif // ACCELSTEPPER
    verbose(1, "setFrameReady\r\n");
}

void isr1()
{
    isr1count++;
    if ((config.isrEdge & OPTO_RISING) || (config.isrEdge & OPTO_FALLING))
    {
        encoderPos++;
    }

    if (config.slowEncoderThreshold && (encoderPos >= config.slowEncoderThreshold))
    {
#ifndef ACCELSTEPPER
        stepSlow();
#endif //  ACCELSTEPPER
    }
    if (config.encoderLimit && (encoderPos >= config.encoderLimit))
    {
        at.setTimeout(setFrameReady, 10);
    }
//    if (config.verbose)
//    {
//        Serial.println("ISR1: ");
//        dumpConfig();
//    }
}

void isr2()
{
    isr2count++;
//    if (config.isrEdge & digitalRead(EncoderPin0))
//    {
//        encoderPos++;
//    }
//    else
//    {
//        encoderPos++;
//    }
//    if (config.verbose)
//    {
//        Serial.println("ISR2: ");
//        dumpConfig();
//    }
}


void encoder_init()
{
    pinMode(EncoderPin0, INPUT_PULLUP);
    pinMode(EncoderPin1, INPUT_PULLUP);

    encoderPos = 0;
    config.encoderTO = 4000;
}

uint8_t buttonState[4] = { 0 };

void buttonInit()
{
    pinMode(ButtonPin0, INPUT_PULLUP);
    pinMode(ButtonPin1, INPUT_PULLUP);
    pinMode(ButtonPin2, INPUT_PULLUP);
    pinMode(ButtonPin3, INPUT_PULLUP);
}

void outputInit()
{
    pinMode(OutputPin0, OUTPUT);
    pinMode(OutputPin1, OUTPUT);
    pinMode(OutputPin2, OUTPUT);
    pinMode(OutputPin3, OUTPUT);
}

void setup ()
{ 
    Serial.begin(115200);
    memset(&config, sizeof(config), 0);
    commandHandler = handleCommand;
#ifndef ACCELSTEPPER
    fastStepDelay(2);
//    slowstepdelay(2);
    stepper::init();
    stepFast();
    stepcount(0);
    cw(); 
#else
    config.stepperRun = 0;
    fastStepDelay(200);
    stepper.setMaxSpeed(200.0);
    stepper.setSpeed(0.0);
#endif // ACCELSTEPPER
    buttonInit();
    encoder_init();
    outputInit();
    ledState = 0;
    config.isrEdge = OPTO_BOTH;
    fan(0);
    rewindMotor(0);
    isr1count = 0;
    isr2count = 0;
    pinMode(LedPin, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(EncoderPin0), isr1, CHANGE);
    attachInterrupt(digitalPinToInterrupt(EncoderPin1), isr2, CHANGE);
    commandset = commands_main;

    for (uint8_t ii = 0; ii < 11; ii++)
    {
        lamp(ii % 2);
        delay(100);
    }
    Serial.println("{State:Ready}");
}

void buttonPoll()
{

    if (digitalRead(ButtonPin0) != buttonState[0])  // Next
    {
        if (buttonState[0])
        {
            //lamp(1);
            config.state = NEXT;
            encoderStart();
            encoderPos %= config.encoderLimit;
#ifndef ACCELSTEPPER
            stepcount(-1);
            stepFast();
#endif //  ACCELSTEPPER
        }
        buttonState[0] = !buttonState[0];
    }

    if (digitalRead(ButtonPin1) != buttonState[1]) // Rewind
    {
        if (buttonState[1])
        {
            rewindMotor(99); 
            fan(1); 
        }
        buttonState[1] = !buttonState[1];
    }
    if (digitalRead(ButtonPin2) != buttonState[2]) //  tension
    {
        if (buttonState[2])
        {
            rewindMotor(33);
        }
        buttonState[2] = !buttonState[2];
    } 
    if (digitalRead(ButtonPin3) != buttonState[3]) //  Reset
    {
        if (buttonState[3])
        {
            setup();
        }
        buttonState[3] = !buttonState[3];
    }
}

void dumpConfig(uint8_t th)
{
    verbose(th, "-------------------------------\r\n");
#ifdef ACCELSTEPPER
    verbose(th, "stepDelay: %f\r\nslowstepdelay: %u\r\ncurrStepDelay: %u\r\nencoderLimit: %u\r\nencoderSlowThreshold: %u\r\n"
#else // ACCELSTEPPER
    verbose(th, "stepDelay: %u\r\nslowstepdelay: %u\r\ncurrStepDelay: %u\r\nencoderLimit: %u\r\nencoderSlowThreshold: %u\r\n"
#endif // ACCELSTEPPER
        "stepcount: %d\r\nparam: %d\r\nisr1count: %u\r\nisr2count: %u\r\n",
        config.fastStepDelay,
        config.slowStepDelay,
        config.currStepDelay,
        config.encoderLimit,
        config.slowEncoderThreshold,
        config.stepCount,
        config.param,
        isr1count,
        isr2count);

    verbose(th, "encoderPos: %d\r\ntension: %d\r\nisrEdge: %d\r\nencoderTO: %d\r\nencoderTime: %d\r\ncw: %d\r\nstate: %d\r\n",
        encoderPos,
        config.tension,
        config.isrEdge,
        config.encoderTO,
        config.encoderTime,
        config.cw,
        config.state);

    verbose(th, "verbose: %d\r\n", config.verbose);
#ifndef ACCELSTEPPER
    verbose(th, "stepper::delta %d\r\nstepper::stepIndex %d\r\nstepper::lastMove %d\r\nmillis %d\r\n\r\n",
        stepper::getdelta(), stepper::getstepindex(), stepper::getlastmove(), millis());
#endif // ACCELSTEPPER

    verbose(th, "End Config\r\n");
}

void dumpStepperConfig()
{
    verbose(0, "stepper.maxSpeed %f\r\nstepper.speed %f\r\nstepper.targetPosition %d\r\nstepper.currentPosition %d\r\n",
        stepper.maxSpeed(),
        stepper.speed(),
        stepper.targetPosition(),
        stepper.currentPosition());
    verbose(0, "stepper.distanceToGo %d\r\n stepper.isRunning %d\r\n, config.stepperRun %d\r\n",
        stepper.distanceToGo(),
        stepper.isRunning(),
        config.stepperRun);
    verbose(0, "config.param %d, as float %f\r\n", config.param, config.param * 1.0);
    verbose(0, "End Config\r\n");
}

void do_next()
{
    config.state = NEXT;
    encoderStart();
    encoderPos %= config.encoderLimit;
#ifdef ACCELSTEPPER
    //stepper.setSpeed(config.fastStepDelay);
    stepper.setSpeed(10.0);
#else // ACCELSTEPPER
    stepcount(-1);
#endif // ACCELSTEPPER
       //            stepFast();
}

void do_isr()
{
    config.isrEdge = config.param % 4;
    config.param = 0;
    switch (config.isrEdge)
    {
        case OPTO_RISING:
            attachInterrupt(digitalPinToInterrupt(EncoderPin0), isr1, RISING);
            attachInterrupt(digitalPinToInterrupt(EncoderPin1), isr2, RISING);
            break;

        case OPTO_FALLING:
            attachInterrupt(digitalPinToInterrupt(EncoderPin0), isr1, FALLING);
            attachInterrupt(digitalPinToInterrupt(EncoderPin1), isr2, FALLING);
            break;

        case OPTO_BOTH:
            attachInterrupt(digitalPinToInterrupt(EncoderPin0), isr1, CHANGE);
            attachInterrupt(digitalPinToInterrupt(EncoderPin1), isr2, CHANGE);
            break;

        default:
            detachInterrupt(digitalPinToInterrupt(EncoderPin0));
            detachInterrupt(digitalPinToInterrupt(EncoderPin1));
            break;
    }
}

const char help_dumpconfig[] PROGMEM="dump config";
const char help_paramneg1[] PROGMEM="param *= -1";
#ifndef ACCELSTEPPER
const char help_stepcw[] PROGMEM="step clockwise";
const char help_stepccw[] PROGMEM="step counterclockwise";
#endif // ACCELSTEPPER
const char help_fanon[] PROGMEM="fan on";
const char help_fanoff[] PROGMEM="fan off";
const char help_param0[] PROGMEM="param = 0";
const char help_steppermenu[] PROGMEM="stepper menu";
#ifndef ACCELSTEPPER
const char help_stepdelay[] PROGMEM="step delay (param)";
const char help_slowdelay[] PROGMEM="slow step delay (param)";
#endif // ACCELSTEPPER
const char help_enclimit[] PROGMEM="encoder limit (param)";
const char help_encthresh[] PROGMEM="slow encoder threshold (param)";
#ifndef ACCELSTEPPER
const char help_stepfast[] PROGMEM="step fast";
const char help_stepslow[] PROGMEM="step slow";
#endif // ACCELSTEPPER
const char help_help[] PROGMEM="help";
const char help_reset[] PROGMEM="reset config";
const char help_lampon[] PROGMEM="lamp on";
const char help_lampoff[] PROGMEM="lamp off";
const char help_encpos0[] PROGMEM="encoder pos = 0";
const char help_next[] PROGMEM="next (until encoderlimit)";
const char help_nextimeout[] PROGMEM="next timeout (param)";
const char help_isr[] PROGMEM="ISR on rising/falling edge (param 0=off, 1=rising, 2=falling, 3=both)";
#ifndef ACCELSTEPPER
const char help_stepcount[] PROGMEM="stepcount (param)";
#endif // ACCELSTEPPER
const char help_tension[] PROGMEM="tension (param)";
const char help_tension0[] PROGMEM="tension 0";
const char help_verbose[] PROGMEM="verbose 0-2 (param)";
const char empty[] PROGMEM="";

Command commands_main[] = {
{'?',FSH(help_dumpconfig), [](){ dumpConfig(0);}},
{'-',FSH(help_paramneg1), [](){ config.param *= -1;}},
#ifndef ACCELSTEPPER
{'<',FSH(help_stepcw), [](){ cw();}},
{'>',FSH(help_stepccw), [](){ccw();}},
#endif // ACCELSTEPPER
{'a',FSH(help_fanon), [](){ fan(1);}},
{'A',FSH(help_fanoff), [](){fan(0);}},
{'c',FSH(help_param0), [](){config.param = 0;}},
{'C',FSH(help_steppermenu), [](){ commandset = commands_stepper; }},
#ifndef ACCELSTEPPER
{'d',FSH(help_stepdelay), [](){fastStepDelay();}},
{'D',FSH(help_slowdelay), [](){slowStepDelay();}},
#endif // ACCELSTEPPER
{'e',FSH(help_enclimit), [](){
    config.encoderLimit = config.param;
    config.param = 0;}},
{'E',FSH(help_encthresh), [](){
    config.slowEncoderThreshold = config.param;
    config.param = 0; }},
#ifndef ACCELSTEPPER
{'f',FSH(help_stepfast), [](){stepFast();}},
{'F',FSH(help_stepslow), [](){stepSlow();}},
#endif // ACCELSTEPPER
{'h',FSH(help_help), [](){ help();}},
{' ',FSH(help_reset), [](){}},
{'l',FSH(help_lampon), [](){ lamp(1); fan(1);}},
{'L',FSH(help_lampoff), [](){ lamp(0);}},
{'m',FSH(help_encpos0), [](){ encoderPos = 0;}},
{'n',FSH(help_next),[](){ do_next(); }},
{'o',FSH(help_nextimeout), [](){encoderTO();}},
{'r',FSH(help_isr), [](){ do_isr();}},
#ifndef ACCELSTEPPER
{'s',FSH(help_stepcount), [](){stepcount();}},
#endif // ACCELSTEPPER
{'t',FSH(help_tension), [](){
    config.tension = config.param;
    config.param = 0;
    rewindMotor(config.tension); 
    fan(1); }},
{'T',FSH(help_tension0), [](){ rewindMotor(0); fan(0);}},
{'v',FSH(help_verbose), [](){}},
{'&', FSH(empty), [](){} }
};

const char help_tomain[] PROGMEM = "To main menu";
const char help_smax[] PROGMEM = "set max speed (param)";
const char help_speed[] PROGMEM = "set speed (param)";
const char help_sconfig[] PROGMEM = "dump stepper config";
const char help_accel[] PROGMEM = "set acceleration (param)";
const char help_stop[] PROGMEM = "<space> stop";
const char help_run[] PROGMEM = "run";
const char help_runspeed[] PROGMEM = "runSpeed";
const char help_setcurrent[] PROGMEM = "set current position (param)";
const char help_moveto[] PROGMEM = "move to target position (param)";
const char help_move[] PROGMEM = "move to relative position (param)";
const char help_r2position[] PROGMEM = "run to target position";
const char help_rs2position[] PROGMEM = "runSpeed to target position";
const char help_stepperrun[] PROGMEM = "stepper run = config";

Command commands_stepper[] = {
    {'a', FSH(help_accel), [](){ stepper.setAcceleration(config.param * 1.0); }},
    {'c',FSH(help_param0), [](){config.param = 0;}},
    {'C', FSH(help_setcurrent), [](){ stepper.setCurrentPosition(config.param * 1.0); }},
    {'h' ,FSH(help_help), [](){ help();}},
    {'m', FSH(help_moveto), [](){ stepper.moveTo(config.param * 1.0); }},
    {'M', FSH(help_move), [](){ stepper.move(config.param * 1.0); }},
    {'p', FSH(help_r2position), [](){ stepper.runToPosition(); }},
    {'P', FSH(help_rs2position), [](){ stepper.runSpeedToPosition(); }},
    {'r', FSH(help_run), [](){ stepper.run(); }},
    {'R', FSH(help_runspeed), [](){ stepper.runSpeed(); }},
    {'s', FSH(help_speed), [](){ stepper.setSpeed(config.param * 1.0); }},
    {'S', FSH(help_stepperrun), [](){ config.stepperRun = config.param; }},
    {'x', FSH(help_tomain), [](){ commandset = commands_main;} },
    {'X', FSH(help_smax), [](){ stepper.setMaxSpeed(config.param * 1.0); }},
    {'?', FSH(help_sconfig), [](){ dumpStepperConfig(); }},
    {' ', FSH(help_stop), [](){ stepper.setSpeed(0.0); }},
    {'&', FSH(empty), [](){}}
};

const char dashes[] PROGMEM="-------------------------------------------------------\r\n";
void help()
{
    Serial.printf("%s\r\n", FSH(dashes));
    for (Command * iter = commandset; iter->key != '&'; iter++)
    {
        Serial.printf("%c: %s\r\n", iter->key, iter->help);
    }
}


#ifndef ACCELSTEPPER
void coilControl()
{
    verbose(1, "Coil Control");
    lastCommand = Serial.read();
    switch (lastCommand)
    {
    case '1': stepper::coilCtrl(0, 0); break;
    case '2': stepper::coilCtrl(0, 1); break;
    case '3': stepper::coilCtrl(1, 0); break;
    case '4': stepper::coilCtrl(1, 1); break;
    default: setup(); break;
    }
}
#endif // ACCELSTEPPER

void handleCommand()
{
	uint8_t lastCommand;
    lastCommand = Serial.read();
    for (Command * iter = commandset; iter->key != '&'; iter++)
    {
        if (iter->key == lastCommand)
        {
            iter->fun();
            return;
        }
    }

//    for (uint8_t ii = 0; ii < NUMCOMMANDS; ii++)
//    {
//        if (lastCommand == mainCommands[ii].key)
//        {
//            mainCommands[ii].fun();
//        }
//    }

    if (lastCommand >= '0' & lastCommand <= '9')
    {
        config.param *= 10;
        config.param += lastCommand - '0';
    }
}

//void handleCommand()
//{
//    uint8_t t;
//    lastCommand = Serial.read();
//    switch (lastCommand)
//    {
//        case '?': 
//            dumpConfig(0); 
//            break;
//
//        case '-': config.param *= -1;; break;                     // reset input param
//#ifndef ACCELSTEPPER
//        case '<': cw(); break;
//        case '>': ccw(); break;
//        case '.': stepper::stop(); break;
//#endif // ACCELSTEPPER
//
//        case 'a': fan(1); break;                                // Cooling fan on
//        case 'A': fan(0); break;                                // Cooling fan off
//        case 'c': config.param = 0; break;
//#ifndef ACCELSTEPPER
//        case 'C': commandHandler = coilControl; break;
//        case 'D': slowStepDelay(); break;
//#endif // ACCELSTEPPER
//        case 'd': fastStepDelay(); break;
//
//        case 'e':                                               // Stepper delay
//            config.encoderLimit = config.param;
//            config.param = 0;
//            break; 
//
//        case 'E':
//            config.slowEncoderThreshold = config.param;
//            config.param = 0;
//            break;
//
//#ifndef ACCELSTEPPER
//        case 'f': stepFast(); break;
//        case 'F': stepSlow(); break;
//#endif // ACCELSTEPPER
//
//        case 'h': help(); break;
//
//#ifdef ACCELSTEPPER
//        case 'i': stepper.setPinsInverted(true, true, true, true, true); break;
//#endif // ACCELSTEPPER
//
//        case ' ': setup(); break;
//
//        case 'l': lamp(1); fan(1); break;                               // Lamp on
//        case 'L': lamp(0); break;                               // Lamp off
//
//        case 'm': encoderPos = 0; break;
//
//        case 'n': 
//            config.state = NEXT;
//            encoderStart();
//            encoderPos %= config.encoderLimit;
//#ifdef ACCELSTEPPER
//            //stepper.setSpeed(config.fastStepDelay);
//            stepper.setSpeed(10.0);
//#else // ACCELSTEPPER
//            stepcount(-1);
//#endif // ACCELSTEPPER
////            stepFast();
//            break;
//
//        case 'o': encoderTO(); break;
//
//        case 'r': 
//            config.isrEdge = config.param % 4;
//            config.param = 0;
//            switch (config.isrEdge)
//            {
//                case OPTO_RISING:
//                    attachInterrupt(digitalPinToInterrupt(EncoderPin0), isr1, RISING);
//                    attachInterrupt(digitalPinToInterrupt(EncoderPin1), isr2, RISING);
//                    break;
//
//                case OPTO_FALLING:
//                    attachInterrupt(digitalPinToInterrupt(EncoderPin0), isr1, FALLING);
//                    attachInterrupt(digitalPinToInterrupt(EncoderPin1), isr2, FALLING);
//                    break;
//
//                case OPTO_BOTH:
//                    attachInterrupt(digitalPinToInterrupt(EncoderPin0), isr1, CHANGE);
//                    attachInterrupt(digitalPinToInterrupt(EncoderPin1), isr2, CHANGE);
//                    break;
//
//                default:
//                    detachInterrupt(digitalPinToInterrupt(EncoderPin0));
//                    detachInterrupt(digitalPinToInterrupt(EncoderPin1));
//                    break;
//            }
//            break;                 
//
//#ifndef ACCELSTEPPER
//        case 's':                                               // Stepper delay
//            stepcount();
//            break; 
//#endif // ACCELSTEPPER
//
//        case 't':                                               // rewind tension
//            config.tension = config.param;
//            config.param = 0;
//            rewindMotor(config.tension); 
//            fan(1); 
//            break;
//
//        case 'T': rewindMotor(0); fan(0); break;                // tension off
//
//        case 'v': config.verbose = config.param; break;
////        case 'V': config.verbose = 0; break;
////        case 'z': 
////            config.state = HUNT; 
////            encoderPos %= config.encoderLimit;
////            stepSlow(); 
//////            stepdelay(10);
////            stepcount(-1);
////            break;
//
////         case ' ':
////             stepper::stop();
////             lamp(0);
////             fan(0);
////             rewindMotor(0);
////             isr1count = 0;
////             isr2count = 0;
////             stepcount(0);
////             config.state = NONE;
////             config.verbose = 0;
////             break;
// 
//        default:                                                // Load parameters
//            if (lastCommand >= '0' & lastCommand <= '9')
//            {
//                config.param *= 10;
//                config.param += lastCommand - '0';
//            }
//            break;
//    }
//    dumpConfig(1);
//}

void zeroPoll()
{
#ifdef ACCELSTEPPER
    if (config.stepperRun) 
    {
        stepper.runSpeed();
        return;
    }
#endif //  ACCELSTEPPER
    if (NONE == config.state) return;
    verbose(2, "state %u\r\n", config.state);

    if (NEXT != config.state)
    {
        return;
    }
    verbose(2, "zeroPoll %d\r\n", millis() - config.encoderTime);
    if ((millis() - config.encoderTime) > config.encoderTO)
    {
        stepcount(0);
#ifdef ACCELSTEPPER
        stepper.stop();
        stepper.setSpeed(0.0);
#else // ACCELSTEPPER
        stepper::stop();
#endif //  ACCELSTEPPER
        config.state = NONE;
        Serial.println("{NTO}");
    }
    else if (encoderPos >= config.encoderLimit && frameReady)
    {
        stepcount(0);
#ifdef ACCELSTEPPER
        stepper.stop();
#else // ACCELSTEPPER
        stepper::stop();
#endif //  ACCELSTEPPER
        config.state = NONE;
        frameReady = 0;
        Serial.println("{HDONE}");
    }
#ifdef ACCELSTEPPER
    else
    {
        stepper.runSpeed();
    }
#endif //  ACCELSTEPPER
}

void ledPoll()
{
    if ((millis() - ledTime) > 1000)
    {
        ledState = !ledState;
        digitalWrite(LedPin, ledState);
        ledTime = millis();
    }
}

void loop ()
{
    at.handle();
    dumpConfig(3);
    if (Serial.available())
    {
        commandHandler();
//        handleCommand();
    }
#ifndef ACCELSTEPPER
    if (config.stepCount > 0)
    {
        int8_t delta = stepper::poll(config.currStepDelay);
        config.stepCount -= delta;
        if (delta) dumpConfig(2);
    }
    if (config.stepCount < 0)
    {
        if (stepper::poll(config.currStepDelay)) dumpConfig(2);
    }
#endif // ACCELSTEPPER
    buttonPoll();
    ledPoll();
    zeroPoll();
}
