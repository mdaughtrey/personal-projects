#include <stdint.h>
#include <stdarg.h>
#include "stepper.h"

typedef enum
{
    NONE = 0,
    HUNT,
    NEXT
} State;

struct {
    uint8_t stepDelay;
    uint8_t slowstepdelay;
    int16_t param;
    uint16_t encoderLimit;
    uint16_t slowEncoderThreshold;
    uint8_t tension;
    uint8_t verbose;
    int16_t stepCount;
    uint8_t rising;
    uint8_t cw;
    uint16_t encoderTO;
    uint32_t encoderTime;
    uint8_t state;
} config;

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

void fan(uint8_t val) { digitalWrite(OutputPin0, val); }
void lamp(uint8_t val) { digitalWrite(OutputPin1, val); }
void rewindMotor(uint8_t val) { analogWrite(OutputPin3, val); }
void cw() { config.cw = 1; stepper::cw(); }
void ccw() { config.cw = 0; stepper::ccw(); }
void stepdelay() { config.stepDelay = config.param; config.param = 0;}
void stepdelay(uint8_t sd) { config.stepDelay = sd; config.param = 0;}
void slowstepdelay() { config.slowstepdelay = config.param; config.param = 0;}
void slowstepdelay(uint8_t sd) { config.slowstepdelay = sd; config.param = 0;}
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

void isr1()
{
    isr1count++;
    //if (config.rising & !digitalRead(EncoderPin1))
    if (!digitalRead(EncoderPin1))
    {
        encoderPos++;
    }
    else
    {
        encoderPos--;
    }

    if (config.slowEncoderThreshold && (encoderPos > config.slowEncoderThreshold))
    {
        stepcount(config.slowstepdelay);
    }
    if (config.encoderLimit && (encoderPos > config.encoderLimit))
    {
        stepcount(0);
        stepper::stop(); 
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
    //if (config.rising & digitalRead(EncoderPin0))
    if (digitalRead(EncoderPin0))
    {
        encoderPos++;
    }
    else
    {
        encoderPos++;
    }
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
    stepdelay(2);
    slowstepdelay(2);
    stepper::init();
    buttonInit();
    encoder_init();
    outputInit();
    ledState = 0;
    stepcount(0);
    config.rising = 1;
    ccw(); 
    fan(0);
    rewindMotor(0);
    isr1count = 0;
    isr2count = 0;
    pinMode(LedPin, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(EncoderPin0), isr1, config.rising ? RISING: FALLING);
    attachInterrupt(digitalPinToInterrupt(EncoderPin1), isr2, config.rising ? RISING: FALLING);

    for (uint8_t ii = 0; ii < 11; ii++)
    {
        lamp(ii % 2);
        delay(100);
    }
    Serial.println("{State:Ready}");
}

void buttonPoll()
{

    if (digitalRead(ButtonPin0) != buttonState[0])  // Lamp
    {
        if (buttonState[0])
        {
            lamp(1);
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
    verbose(th, "stepDelay: %u\r\nslowstepdelay: %u\r\nencoderLimit: %u\r\nencoderSlowThreshold: %u\r\nstepcount: %d\r\nparam: %d\r\nisr1count: %u\r\nisr2count: %u\r\n",
        config.stepDelay, config.slowstepdelay, config.encoderLimit, config.slowEncoderThreshold, config.stepCount, config.param, isr1count, isr2count);

    verbose(th, "encoderPos: %d\r\ntension: %d\r\nrising: %d\r\nencoderTO: %d\r\nencoderTime: %d\r\ncw: %d\r\nstate: %d\r\n",
        encoderPos, config.tension, config.rising, config.encoderTO, config.encoderTime, config.cw, config.state);

    verbose(th, "verbose: %d\r\n", config.verbose);
    verbose(th, "stepper::delta %d\r\nstepper::stepIndex %d\r\nstepper::lastMove %d\r\nmillis %d\r\n\r\n",
        stepper::getdelta(), stepper::getstepindex(), stepper::getlastmove(), millis());
    verbose(th, "End Config\r\n");
}

void help()
{
    Serial.println("?: dump config");
    Serial.println("-: param *= -1");
    Serial.println("<: step clockwise");
    Serial.println(">: step counterclockwise");
    Serial.println("c: param = 0");
    Serial.println("C: coilControl");
    Serial.println("d: step delay (param)");
    Serial.println("D: slow step delay (param)");
    Serial.println("e: encoder limit (param)");
    Serial.println("E: slow encoder threshold (param)");
    Serial.println("f: fan on");
    Serial.println("F: fan off");
    Serial.println("h: help");
    Serial.println("<space>: reset config");
    Serial.println("l: lamp on");
    Serial.println("L: lamp off");
    Serial.println("m: encoder pos = 0");
    Serial.println("n: next (until encoderlimit)");
    Serial.println("o: next timeout (param)");
    Serial.println("r: ISR on rising/falling edge (param)");
    Serial.println("s: stepcount (param)");
    Serial.println("t: tension (param)");
    Serial.println("T: tension 0");
    Serial.println("v: verbose 0-2 (param)");
    Serial.println("z: hunt");
}


//void coilControl()
//{
//    lastCommand = Serial.read();
//    switch (lastCommand)
//    {
//    case '1': coilCtrl(0, 0); break;
//    case '2': coilCtrl(0, 1); break;
//    case '3': coilCtrl(1, 0); break;
//    case '4': coilCtrl(1, 1); break;
//    default: setup(); break;
//    }
//}


void handleCommand()
{
    uint8_t t;
    lastCommand = Serial.read();
    switch (lastCommand)
    {
        case '?': 
            dumpConfig(0); 
            break;

        case '-': config.param *= -1;; break;                     // reset input param
        case '<': cw(); break;
        case '>': ccw(); break;
        case '.': stepper::stop(); break;

        case 'c': config.param = 0; break;
//        case 'C': commandHandler = coilControl(); break;
        case 'd': stepdelay(); break;
        case 'D': slowstepdelay(); break;

        case 'e':                                               // Stepper delay
            config.encoderLimit = config.param;
            config.param = 0;
            break; 

        case 'E':
            config.slowEncoderThreshold = config.param;
            config.param = 0;
            break;

        case 'f': fan(1); break;                                // Cooling fan on
        case 'F': fan(0); break;                                // Cooling fan off
        case 'h': help(); break;

        case ' ': setup(); break;

        case 'l': lamp(1); fan(1); break;                               // Lamp on
        case 'L': lamp(0); break;                               // Lamp off

        case 'm': encoderPos = 0; break;

        case 'n': 
            config.state = NEXT;
            encoderStart();
            encoderPos %= config.encoderLimit;
            stepcount(-1);
            break;

        case 'o': encoderTO(); break;

        case 'r': 
            config.rising = (0 != config.param);
            config.param = 0;
            break;                 // rising/falling ISR edge
        case 's':                                               // Stepper delay
            stepcount();
            break; 

        case 't':                                               // rewind tension
            config.tension = config.param;
            config.param = 0;
            rewindMotor(config.tension); 
            fan(1); 
            break;

        case 'T': rewindMotor(0); fan(0); break;                // tension off

        case 'v': config.verbose = config.param; break;
//        case 'V': config.verbose = 0; break;
        case 'z': 
            config.state = HUNT; 
            encoderPos %= config.encoderLimit;
            stepdelay(10);
            stepcount(-1);
            break;

//         case ' ':
//             stepper::stop();
//             lamp(0);
//             fan(0);
//             rewindMotor(0);
//             isr1count = 0;
//             isr2count = 0;
//             stepcount(0);
//             config.state = NONE;
//             config.verbose = 0;
//             break;
 
        default:                                                // Load parameters
            if (lastCommand >= '0' & lastCommand <= '9')
            {
                config.param *= 10;
                config.param += lastCommand - '0';
            }
            break;
    }
    dumpConfig(1);
}

void zeroPoll()
{
    if (NONE == config.state) return;
    verbose(2, "state %u\r\n", config.state);
    if (HUNT == config.state)
    {
        if (encoderPos > 1) cw();
        else if (encoderPos < -1) ccw();
        else config.state = NONE;
        return;
    }

    if (NEXT != config.state)
    {
        return;
    }
    verbose(2, "zeroPoll elapsed %u\r\n", millis() - config.encoderTime);
    if ((millis() - config.encoderTime) > config.encoderTO)
    {
        stepcount(0);
        config.state = NONE;
        Serial.println("{NTO}");
    }
    else if (encoderPos >= config.encoderLimit)
    {
        stepcount(0);
        config.state = NONE;
        Serial.println("{HDONE}");
    }
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
    dumpConfig(3);
    if (Serial.available())
    {
        commandHandler();
//        handleCommand();
    }
    if (config.stepCount > 0)
    {
        int8_t delta = stepper::poll(config.stepDelay);
        config.stepCount -= delta;
        if (delta) dumpConfig(2);
    }
    if (config.stepCount < 0)
    {
        if (stepper::poll(config.stepDelay)) dumpConfig(2);
    }
    buttonPoll();
    ledPoll();
    zeroPoll();
}
