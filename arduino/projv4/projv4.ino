#include "defs.h"
#include <stdarg.h>
#include <pico/stdlib.h>
#include <hardware/pwm.h>
//#include "serialout.h"
#include "stepper.h"
#include <AsyncTimer.h>

#define FSH(s) reinterpret_cast<const __FlashStringHelper *>(s)

const uint8_t stepperEnable = 20;
const uint8_t stepperDir = 19;
const uint8_t stepperPulse = 18;

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
    int16_t param;
    uint16_t encoderLimit;
    uint16_t slowEncoderThreshold;
    uint8_t tension;
    uint8_t verbose;
    uint8_t isrEdge;
    uint16_t encoderTO;
    uint32_t encoderTime;
    uint8_t state;
    uint8_t pwmslice;
    uint8_t pwmpin;
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
extern Command commands_pwm[];
//StreamEx serout = Serial;

void fan(uint8_t val) { digitalWrite(OutputPin0, val); }
void lamp(uint8_t val) { digitalWrite(OutputPin1, val); }

void rewindMotor(uint8_t val) { 
    pwm_set_chan_level(config.pwmslice, PWM_CHAN_A, val);
    pwm_set_enabled(config.pwmslice, val > 0 ? true: false);
}
//analogWrite(OutputPin3, val); }
void encoderStart() { config.encoderTime = millis(); }
void encoderTO() { config.encoderTO = config.param; config.param = 0; }

volatile uint16_t encoderPos = 0;
volatile int isr1count = 0;
volatile int isr2count = 0;
void handleCommand();
void coilControl();
//void (*commandHandler)() = handleCommand;

Stepper stepper(stepperEnable, stepperDir, stepperPulse);

//void serialout(uint8_t threshold, const char * fmt, ...)
//{
//    if (threshold > config.verbose) return;
//    char buffer[256];
//    va_list args;
//    va_start(args, fmt);
//    vsprintf(buffer, fmt, args);
//    va_end(args);
//    Serial.print(buffer);
//}

void initPWM()
{
    Serial.printf("initPWM\r\n");
    config.pwmpin = 0;
    gpio_set_function(config.pwmpin, GPIO_FUNC_PWM);
    config.pwmslice = pwm_gpio_to_slice_num(config.pwmpin);
    Serial.printf("slice is %d\r\n", config.pwmslice);
    pwm_set_wrap(config.pwmslice, 256);
    pwm_set_chan_level(config.pwmslice, PWM_CHAN_A,0);
}

void setFrameReady()
{
    frameReady = 1;
#ifndef ACCELSTEPPER
//    stepcount(0);
//    stepper::stop(); 
#endif // ACCELSTEPPER
    Serial.printf("setFrameReady\r\n");
}

void isr1()
{
    isr1count++;
    if ((config.isrEdge & OPTO_RISING) || (config.isrEdge & OPTO_FALLING))
    {
        encoderPos++;
    }

//    if (config.slowEncoderThreshold && (encoderPos >= config.slowEncoderThreshold))
//    {
//    }
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
    stepper.enable();
    stepper.m_minInterval64 = 1;
    stepper.m_maxInterval64 = 6;
    stepper.m_rampUpSteps = 10;
    stepper.m_rampDownSteps = 10;

    stepper.cw(); 
    buttonInit();
    encoder_init();
    outputInit();
    initPWM();
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
            do_next();
        }
        buttonState[0] = !buttonState[0];
    }

    if (digitalRead(ButtonPin1) != buttonState[1]) // Rewind
    {
        if (buttonState[1])
        {
            rewindMotor(192); 
            fan(1); 
        }
        buttonState[1] = !buttonState[1];
    }
    if (digitalRead(ButtonPin2) != buttonState[2]) //  tension
    {
        if (buttonState[2])
        {
            rewindMotor(110);
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
    Serial.printf("-------------------------------\r\n");
    Serial.printf("encoderLimit: %u\r\nencoderSlowThreshold: %u\r\n"
        "param: %d\r\nisr1count: %u\r\nisr2count: %u\r\n",
        config.encoderLimit,
        config.slowEncoderThreshold,
        config.param,
        isr1count,
        isr2count);

    Serial.printf("encoderPos: %d\r\ntension: %d\r\nisrEdge: %d\r\nencoderTO: %d\r\nencoderTime: %d\r\nstate: %d\r\n",
        encoderPos,
        config.tension,
        config.isrEdge,
        config.encoderTO,
        config.encoderTime,
        config.state);

    Serial.printf("verbose: %d\r\n", config.verbose);
    Serial.printf("stepper.minInterval %u stepper.maxInterval %u\r\n", stepper.m_minInterval64, stepper.m_maxInterval64);

    Serial.printf("End Config\r\n");
}

const char stepperTitle[] PROGMEM = "--------Stepper Config ---------\r\n";
void dumpStepperConfig()
{
    Serial.printf("%s",FSH(stepperTitle));
    Serial.printf("param %u\r\n", config.param);
    Serial.printf("m_rampUpSteps %u m_rampDownSteps %u\r\n", stepper.m_rampUpSteps, stepper.m_rampDownSteps);
    Serial.printf("m_minInterval64 %llu m_maxInterval64 %llu\r\n", stepper.m_minInterval64, stepper.m_maxInterval64);
    Serial.printf("m_stepCount %u m_targetSteps %u m_stepsPerMove %u m_running %u\r\n",
        stepper.m_stepCount, stepper.m_targetSteps, stepper.m_stepsPerMove, stepper.m_running);
}

void dumpPWMConfig()
{
    Serial.printf("param %u\r\n", config.param);
    Serial.printf("PWM pin %d PWM slice %d\r\n", config.pwmpin, config.pwmslice);
}

void do_next()
{
    config.state = NEXT;
    stepper.start(config.encoderLimit);
    encoderStart();
    encoderPos %= config.encoderLimit;
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
const char help_fanon[] PROGMEM="fan on";
const char help_fanoff[] PROGMEM="fan off";
const char help_param0[] PROGMEM="param = 0";
const char help_steppermenu[] PROGMEM="stepper menu";
const char help_enclimit[] PROGMEM="encoder limit (param)";
const char help_encthresh[] PROGMEM="slow encoder threshold (param)";
const char help_help[] PROGMEM="help";
const char help_reset[] PROGMEM="reset config";
const char help_lampon[] PROGMEM="lamp on";
const char help_lampoff[] PROGMEM="lamp off";
const char help_encpos0[] PROGMEM="encoder pos = 0";
const char help_next[] PROGMEM="next (until encoderlimit)";
const char help_nextimeout[] PROGMEM="next timeout (param)";
const char help_isr[] PROGMEM="ISR on rising/falling edge (param 0=off, 1=rising, 2=falling, 3=both)";
const char help_tension[] PROGMEM="tension (param)";
const char help_tension0[] PROGMEM="tension 0";
const char help_verbose[] PROGMEM="verbose 0-1 (param)";
const char help_pwmmenu[] PROGMEM="PWM menu";
const char empty[] PROGMEM="";

Command commands_main[] = {
{'?',FSH(help_dumpconfig), [](){ dumpConfig(0);}},
{'-',FSH(help_paramneg1), [](){ config.param *= -1;}},
{'a',FSH(help_fanon), [](){ fan(1);}},
{'A',FSH(help_fanoff), [](){fan(0);}},
{'c',FSH(help_param0), [](){config.param = 0;}},
{'C',FSH(help_steppermenu), [](){ commandset = commands_stepper; }},
{'e',FSH(help_enclimit), [](){
    config.encoderLimit = config.param;
    config.param = 0;}},
{'E',FSH(help_encthresh), [](){
    config.slowEncoderThreshold = config.param;
    config.param = 0; }},
{'h',FSH(help_help), [](){ help();}},
{' ',FSH(help_reset), [](){ setup();}},
{'l',FSH(help_lampon), [](){ lamp(1); fan(1);}},
{'L',FSH(help_lampoff), [](){ lamp(0);}},
{'m',FSH(help_encpos0), [](){ encoderPos = 0;}},
{'n',FSH(help_next),[](){ do_next(); }},
{'o',FSH(help_nextimeout), [](){encoderTO();}},
{'p',FSH(help_pwmmenu), [](){commandset = commands_pwm;}},
{'r',FSH(help_isr), [](){ do_isr();}},
{'t',FSH(help_tension), [](){
    config.tension = config.param;
    config.param = 0;
    rewindMotor(config.tension); 
    fan(1); }},
{'T',FSH(help_tension0), [](){ rewindMotor(0); fan(0);}},
{'v',FSH(help_verbose), [](){ config.verbose = stepper.m_verbose = config.param;}},
{'&', FSH(empty), [](){} }
};

const char help_tomain[] PROGMEM = "To main menu";
const char help_imax[] PROGMEM = "set max interval (param)";
const char help_imin[] PROGMEM = "set min interval (param)";
const char help_rup[] PROGMEM = "set ramp up steps (param)";
const char help_rdown[] PROGMEM = "set ramp down steps (param)";
const char help_sconfig[] PROGMEM = "dump stepper config";
const char help_stop[] PROGMEM = "<space> stop";
const char help_start[] PROGMEM = "start (param as steps)";
const char help_run[] PROGMEM = "run";
const char help_stepperrun[] PROGMEM = "stepper run (1 shot)";
const char help_sdisable[] PROGMEM = "disable stepper outputs";
const char help_senable[] PROGMEM = "enable stepper outputs";
const char help_pulse[] PROGMEM = "pulse (param 0=lh 1=hl)";

Command commands_stepper[] = {
    {'c',FSH(help_param0), [](){config.param = 0;}},
    {'D', FSH(help_rdown), [](){ stepper.m_rampDownSteps = config.param;} },
    {'d', FSH(help_sdisable), [](){ stepper.disable();} },
    {'e', FSH(help_senable), [](){ stepper.enable();} },
    {'h', FSH(help_help), [](){ help();}},
    {'i', FSH(help_imax), [](){ stepper.m_maxInterval64 = config.param;} },
    {'I', FSH(help_imin), [](){ stepper.m_minInterval64 = config.param;} },
    {'p', FSH(help_pulse), [](){ stepper.pulse(config.param);} },
    {'r', FSH(help_run), [](){ stepper.run();} },
    {'s', FSH(help_start), [](){ stepper.start(config.param); }},
    {'U', FSH(help_rup), [](){ stepper.m_rampUpSteps = config.param;} },
    {'x', FSH(help_tomain), [](){ commandset = commands_main;} },
    {'?', FSH(help_sconfig), [](){ dumpStepperConfig(); }},
    {' ', FSH(help_stop), [](){ setup(); }},
    {'&', FSH(empty), [](){}}
};


const char help_initpwm[] PROGMEM = "Init pwm";
const char help_wrap[] PROGMEM = "pwm cycle wrap = config.param";
const char help_level[] PROGMEM = "pwm level = config.param";
const char help_enable[] PROGMEM = "enable pwm";
const char help_disable[] PROGMEM = "disable pwm";
const char help_clkdiv[] PROGMEM = "clock div = config.param";
const char help_pwmpin[] PROGMEM = "PWM pin = config.param";


Command commands_pwm[] = {
    {'c',FSH(help_param0), [](){config.param = 0;}},
    {'d', FSH(help_disable), [](){ pwm_set_enabled(config.pwmslice, false);}},
    {'e', FSH(help_enable), [](){ pwm_set_enabled(config.pwmslice, true);}},
    {'h', FSH(help_help), [](){ help();}},
    {'p', FSH(help_pwmpin), [](){ config.pwmpin = config.param;}},
    {'i', FSH(help_initpwm), [](){ initPWM(); }},
    {'l', FSH(help_level), [](){ pwm_set_chan_level(config.pwmslice, PWM_CHAN_A, config.param);}},
//    {'v', FSH(help_clkdiv), [](){ pwm_config_set_clkdiv_int( pwm_config c = {0, hhset_enable(config.pwmslice, true);}},
    {'w', FSH(help_wrap), [](){ pwm_set_wrap(config.pwmslice, config.param);}},
    {'x', FSH(help_tomain), [](){ commandset = commands_main;} },
    {'?', FSH(help_sconfig), [](){ dumpPWMConfig(); }},
    {'&', FSH(empty), [](){}}};

const char dashes[] PROGMEM="-------------------------------------------------------\r\n";
void help()
{
    Serial.printf("%s\r\n", FSH(dashes));
    for (Command * iter = commandset; iter->key != '&'; iter++)
    {
        Serial.printf("%c: %s\r\n", iter->key, iter->help);
    }
}


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

void stepperPoll()
{
    if (NONE == config.state) return;
    if (config.verbose)
    {
        Serial.printf("state %u\r\n", config.state);
    }

    if (NEXT != config.state)
    {
        return;
    }
    if (config.verbose)
    {
        Serial.printf("stepperPoll %d\r\n", millis() - config.encoderTime);
    }
    if ((millis() - config.encoderTime) > config.encoderTO)
    {
        stepper.stop();
        config.state = NONE;
        Serial.println("{NTO}");
    }
    else if (encoderPos >= config.encoderLimit && frameReady)
    {
        stepper.stop(encoderPos);
        config.state = NONE;
        frameReady = 0;
        Serial.println("{HDONE}");
    }
    else
    {
        stepper.run();
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
    at.handle();
//    dumpConfig(3);
    if (Serial.available())
    {
        handleCommand();
    }
    buttonPoll();
    ledPoll();
    stepperPoll();
}
