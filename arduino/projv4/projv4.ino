#include "defs.h"
#include <stdarg.h>
#include <pico/stdlib.h>
#include <hardware/pwm.h>
#include <bitset>
//#include "serialout.h"
#include "stepper.h"
#include <AsyncTimer.h>

#define FSH(s) reinterpret_cast<const __FlashStringHelper *>(s)

const uint8_t stepperEnable = 20;
const uint8_t stepperDir = 19;
const uint8_t stepperPulse = 18;

typedef enum
{
    HUNT_NONE = 0,
    HUNT_GAP,
    HUNT_FRAME
} HuntState;

typedef enum
{
    FILM_NONE = 0,
    FILM_GAP,
    FILM_FRAME
} FilmState;

//typedef enum
//{
//    OPTO_NONE = 0,
//    OPTO_RISING,
//    OPTO_FALLING,
//    OPTO_BOTH
//} OptoEdge;

typedef struct
{
    char key;
    const __FlashStringHelper * help;
    //void (*help)();
    void (*fun)();
}Command;

struct {
    int16_t param;
#ifdef OPTO_ENCODER
    uint16_t encoderLimit;
    uint16_t slowEncoderThreshold;
#endif // OPTO_ENCODER
    uint8_t tension;
    uint8_t verbose;
    uint8_t isrEdge;
#ifdef OPTO_ENCODER
    uint16_t encoderTO;
    uint32_t encoderTime;
#endif // OPTO_ENCODER
    uint8_t pwmslice;
    uint8_t pwmpin;
    uint8_t pwmchan;
    HuntState huntState;
    FilmState filmState;
    uint8_t pinpoll;
} config;

AsyncTimer at;

uint8_t frameReady = 0;
uint8_t lastCommand;
uint8_t ledState;
uint32_t ledTime = 0;
uint8_t mon = 0;

const uint8_t ButtonPin0 = 2;
const uint8_t ButtonPin1 = 3;
const uint8_t ButtonPin2 = 4;
const uint8_t ButtonPin3 = 5;

#ifdef OPTO_ENCODER
const uint8_t EncoderPin0 = 14;
const uint8_t EncoderPin1 = 15;
#endif // OPTO_ENCODER

const uint8_t OutputPin0 = 10; // fan
const uint8_t OutputPin1 = 11; // lamp
const uint8_t OutputPin2 = 12;
const uint8_t OutputPin3 = 13; // setTension

const uint8_t LedPin = 25;
Command * commandset;
void help();
extern Command commands_stepper[];
extern Command commands_main[];
extern Command commands_pwm[];
//StreamEx serout = Serial;

void fan(uint8_t val) { digitalWrite(OutputPin0, val); }
void lamp(uint8_t val) { digitalWrite(OutputPin1, val); }

void setTension(uint8_t val) { 
    pwm_set_chan_level(config.pwmslice, PWM_CHAN_A, val);
    pwm_set_enabled(config.pwmslice, val > 0 ? true: false);
}

#ifdef OPTO_ENCODER
void encoderStart() { config.encoderTime = millis(); }
void encoderTO() { config.encoderTO = config.param; config.param = 0; }

volatile uint16_t encoderPos = 0;
#endif // OPTO_ENCODER
//volatile int isr1count = 0;
//volatile int isr2count = 0;

uint32_t encoder00, encoder01 = 0;
uint32_t encoder10, encoder11 = 0;
void handleCommand(uint8_t command);
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
    config.pwmpin = 0;
    Serial.printf("initPWM\r\n");
    gpio_set_function(config.pwmpin, GPIO_FUNC_PWM);
    config.pwmslice = pwm_gpio_to_slice_num(config.pwmpin);
    config.pwmchan = pwm_gpio_to_channel(config.pwmpin);
    Serial.printf("slice is %d\r\n", config.pwmslice);
    pwm_set_wrap(config.pwmslice, 255);
    pwm_set_chan_level(config.pwmslice, config.pwmchan,0);
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

#if 0
void isr1()
{
#ifdef OPTO_ENCODER
    isr1count++;
//    if ((config.isrEdge & OPTO_RISING) || (config.isrEdge & OPTO_FALLING))
//    {
//        encoderPos++;
//    }
//
//    if (config.encoderLimit && (encoderPos >= config.encoderLimit))
//    {
//        at.setTimeout(setFrameReady, 10);
//    }
#endif // OPTO_ENCODER
}

void isr2()
{
#ifdef OPTO_ENCODER
    isr2count++;
#endif // OPTO_ENCODER
}
#endif // 0

// 1 == gap 0 == frame
void readFilmSensor()
{
    encoder00 <<= 1;
    encoder00 |= encoder01 >> 31;
    encoder01 <<= 1 ;
    encoder01 |= digitalRead(EncoderPin0);

    encoder10 <<= 1;
    encoder10 |= encoder11 >> 31;
    encoder11 <<= 1 ;
    encoder11 |= digitalRead(EncoderPin1);

    if (config.pinpoll && HUNT_NONE != config.huntState) at.setTimeout(readFilmSensor, config.pinpoll);
    if (0 == encoder01 || 0 == encoder11) config.filmState = FILM_FRAME;
    if (0xffffffff == encoder01 || 0xffffffff == encoder11) config.filmState = FILM_GAP;
}


#ifdef OPTO_ENCODER
void encoder_init()
{
    pinMode(EncoderPin0, INPUT_PULLUP);
    pinMode(EncoderPin1, INPUT_PULLUP);

    encoderPos = 0;
    config.encoderTO = 4000;
}
#endif // OPTO_ENCODER

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
    config.filmState = FILM_NONE;
    config.pinpoll = 1;

    stepper.cw(); 
    buttonInit();
#ifdef OPTO_ENCODER
    encoder_init();
#endif // OPTO_ENCODER
    outputInit();
    initPWM();
    ledState = 0;
//    config.isrEdge = OPTO_BOTH;
    fan(0);
    setTension(0);
//    isr1count = 0;
//    isr2count = 0;
    pinMode(LedPin, OUTPUT);

//#ifdef OPTO_ENCODER
//    attachInterrupt(digitalPinToInterrupt(EncoderPin0), isr1, RISING);
//    attachInterrupt(digitalPinToInterrupt(EncoderPin1), isr2, RISING);
//#endif // OPTO_ENCODER
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
            setTension(192); 
            fan(1); 
        }
        buttonState[1] = !buttonState[1];
    }
    if (digitalRead(ButtonPin2) != buttonState[2]) //  tension
    {
        if (buttonState[2])
        {
            setTension(110);
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
#ifdef OPTO_ENCODER
    Serial.printf("encoderLimit: %u\r\nencoderSlowThreshold: %u\r\nparam %u\r\nconfig.pinpoll %u\r\ntension %u \r\n",
        config.encoderLimit,
        config.slowEncoderThreshold,
        config.param,
        config.pinpoll,
        config.tension);

    Serial.printf("encoderTO: %d\r\nencoderTime: %d\r\nhuntState: %d\r\nfilmState: %d\r\n",
        config.encoderTO,
        config.encoderTime,
        config.huntState,
        config.filmState);
#endif // OPTO_ENCODER

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
    config.huntState = HUNT_GAP;
//#ifdef OPTO_ENCODER
//    stepper.start(config.encoderLimit);
    encoderStart();
    stepper.start();
    at.setTimeout(readFilmSensor, config.pinpoll);
//    encoderPos %= config.encoderLimit;
//#endif // OPTO_ENCODER
}

//void do_isr()
//{
//    config.isrEdge = config.param % 4;
//    config.param = 0;
//#ifdef OPTO_ENCODER
//    switch (config.isrEdge)
//    {
//        case OPTO_RISING:
//            attachInterrupt(digitalPinToInterrupt(EncoderPin0), isr1, RISING);
//            attachInterrupt(digitalPinToInterrupt(EncoderPin1), isr2, RISING);
//            break;
//
//        case OPTO_FALLING:
//            attachInterrupt(digitalPinToInterrupt(EncoderPin0), isr1, FALLING);
//            attachInterrupt(digitalPinToInterrupt(EncoderPin1), isr2, FALLING);
//            break;
//
//        case OPTO_BOTH:
//            attachInterrupt(digitalPinToInterrupt(EncoderPin0), isr1, CHANGE);
//            attachInterrupt(digitalPinToInterrupt(EncoderPin1), isr2, CHANGE);
//            break;
//
//        default:
//            detachInterrupt(digitalPinToInterrupt(EncoderPin0));
//            detachInterrupt(digitalPinToInterrupt(EncoderPin1));
//            break;
//    }
//#endif // OPTO_ENCODER
//}

void initialize()
{
    //const char initstr[] = "kcv50tl30ecE8000oCc500Ic2000ic25D25Ux";
    const char initstr[] = "kcv50tl30ecE8000oCc500Ic2000icDUx";
    for (int ii = 0; ii < strlen(initstr); ii++)
    {
        handleCommand(initstr[ii]);
    }
}

void autotension()
{
    uint8_t low = 10;
    uint8_t high = 150;
    uint8_t mid = (low + (high - low))/2;
    uint8_t breaker = 20;

    setTension(0);
    delay(1000);
//    int ii;
//    for (ii=low; (ii < high) && digitalRead(EncoderPin0); ii++)
//    {
//        setTension(ii);
//        delay(100);
//        config.tension = ii;
//    }
//    return;
    while ((low <= high) && breaker--)
    {
        mid = (low + (high - low)/2);
        if (config.verbose)
            Serial.printf("%u: %u < %u > %u %u %u\r\n", breaker,low, mid, high,digitalRead(EncoderPin0), digitalRead(EncoderPin1));
        setTension(mid);
        delay(1000);

        if (digitalRead(EncoderPin0) & digitalRead(EncoderPin1))
        {
            low = mid + 1;
        }
        else
        {
            high = mid -1;
        }
    }
    config.tension = mid;
}

const char help_dumpconfig[] PROGMEM="dump config";
const char help_paramneg1[] PROGMEM="param *= -1";
const char help_fanon[] PROGMEM="fan on";
const char help_fanoff[] PROGMEM="fan off";
const char help_param0[] PROGMEM="param = 0";
const char help_steppermenu[] PROGMEM="stepper menu";
#ifdef OPTO_ENCODER
const char help_enclimit[] PROGMEM="encoder limit (param)";
const char help_encthresh[] PROGMEM="slow encoder threshold (param)";
#endif // OPTO_ENCODER
const char help_help[] PROGMEM="help";
const char help_reset[] PROGMEM="reset config";
const char help_lampon[] PROGMEM="lamp on";
const char help_lampoff[] PROGMEM="lamp off";
#ifdef OPTO_ENCODER
const char help_encpos0[] PROGMEM="encoder pos = 0";
const char help_next[] PROGMEM="next (until encoderlimit)";
const char help_nextimeout[] PROGMEM="next timeout (param)";
#endif // OPTO_ENCODER
const char help_isr[] PROGMEM="ISR on rising/falling edge (param 0=off, 1=rising, 2=falling, 3=both)";
const char help_tension[] PROGMEM="tension (param)";
const char help_tension0[] PROGMEM="tension 0";
const char help_verbose[] PROGMEM="verbose 0-1 (param)";
const char help_pwmmenu[] PROGMEM="PWM menu";
const char help_init[] PROGMEM="Initialize with string";
const char empty[] PROGMEM="";
const char help_mon[] PROGMEM="monitoring on";
const char help_moff[] PROGMEM="monitoring off";
const char help_pon[] PROGMEM="set pin polling (ms,param))";
const char help_poff[] PROGMEM="polling off";
const char help_autotension[] PROGMEM="Autotension";

Command commands_main[] = {
{'?',FSH(help_dumpconfig), [](){ dumpConfig(0);}},
{'-',FSH(help_paramneg1), [](){ config.param *= -1;}},
{'a',FSH(help_fanon), [](){ fan(1);}},
{'A',FSH(help_fanoff), [](){fan(0);}},
{'b',FSH(help_autotension), [](){ autotension(); }},
{'c',FSH(help_param0), [](){config.param = 0;}},
#ifdef SIMPLEINTERVAL
{'C',FSH(help_steppermenu), [](){ commandset = commands_stepper; }},
#endif //  SIMPLEINTERVAL
#ifdef OPTO_ENCODER
{'e',FSH(help_enclimit), [](){
    config.encoderLimit = config.param;
    config.param = 0;}},
{'E',FSH(help_encthresh), [](){
    config.slowEncoderThreshold = config.param;
    config.param = 0; }},
#endif // OPTO_ENCODER
{'h',FSH(help_help), [](){ help();}},
{'i',FSH(help_init), [](){ initialize(); }},
{' ',FSH(help_reset), [](){ setup();}},
{'l',FSH(help_lampon), [](){ lamp(1); fan(1);}},
{'L',FSH(help_lampoff), [](){ lamp(0);}},
#ifdef OPTO_ENCODER
{'m',FSH(help_encpos0), [](){ encoderPos = 0;}},
{'n',FSH(help_next),[](){ do_next(); }},
{'o',FSH(help_nextimeout), [](){encoderTO();}},
#endif // OPTO_ENCODER
{'p',FSH(help_pwmmenu), [](){commandset = commands_pwm;}},
//{'r',FSH(help_isr), [](){ do_isr();}},
{'t',FSH(help_tension), [](){
    config.tension = config.param;
    config.param = 0;
    setTension(config.tension); 
    fan(1); }},
{'T',FSH(help_tension0), [](){ setTension(0); fan(0);}},
{'v',FSH(help_verbose), [](){ config.verbose = stepper.m_verbose = config.param;}},
{'w',FSH(help_mon), [](){ mon = 1; at.setTimeout(monitor,100);}},
{'W',FSH(help_moff), [](){ mon = 0;}},
{'x',FSH(help_pon), [](){ config.pinpoll = config.param; at.setTimeout(readFilmSensor, config.pinpoll); config.param = 0; }},
{'X',FSH(help_poff), [](){ config.pinpoll = 0;}},
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
#ifdef OPTO_ENCODER
    {'r', FSH(help_run), [](){ stepper.run(encoderPos);} },
#endif //  OPTO_ENCODER
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

void handleCommand(uint8_t command)
{
//	uint8_t lastCommand;
//    lastCommand = Serial.read();
    for (Command * iter = commandset; iter->key != '&'; iter++)
    {
        if (iter->key == command)
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

    if (command >= '0' & command <= '9')
    {
        config.param *= 10;
        config.param += command - '0';
    }
}

void stepperPoll()
{
    if (HUNT_NONE == config.huntState) return;
//    if (config.verbose)
//    {
//        Serial.printf("huntState %u filmState %u encoder01 %s encoder11 %s\r\n",
//            config.huntState,config.filmState,std::bitset<32>(encoder01).to_string().c_str(), std::bitset<32>(encoder11).to_string().c_str());
//    }

//    if (NEXT != config.state)
//    {
//        return;
//    }
    if ((millis() - config.encoderTime) > config.encoderTO)
    {
        stepper.stop();
        config.huntState = HUNT_NONE;
        Serial.println("{NTO}");
    }

    if (HUNT_GAP == config.huntState && FILM_GAP == config.filmState)
    {
        config.huntState = HUNT_FRAME;
    } 
    else if (HUNT_FRAME == config.huntState && FILM_FRAME == config.filmState)
    {
        stepper.stop();
        config.huntState = HUNT_NONE;
        Serial.println("{HDONE}");
    }
    else
    {
        stepper.run();
    }
// #ifdef OPTO_ENCODER
//     if (config.verbose)
//     {
//         Serial.printf("stepperPoll %d\r\n", millis() - config.encoderTime);
//     }
//     else if (encoderPos >= config.encoderLimit && frameReady)
//     {
//         stepper.stop(encoderPos);
//         config.state = NONE;
//         frameReady = 0;
//         Serial.println("{HDONE}");
//     }
// #endif // OPTO_ENCODER
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

void monitor(void)
{
    if (!mon) return;

//    std::bitset<32> d3(encoder00);
//    std::bitset<32> d2(encoder01);
//    std::bitset<32> d1(encoder10);
//    std::bitset<32> d0(encoder11);
//    if (d0.count() == 0 | d0.count() == 32) Serial.printf("%u ", d0.count());
//    Serial.printf("%d %d %d %d\r\n", d3.count(),d2.count(),d1.count(),d0.count());
    Serial.printf("huntState %u filmState %u encoder01 %s encoder11 %s p14 %u p15 %u\r\n",
            config.huntState,config.filmState,std::bitset<32>(encoder01).to_string().c_str(), std::bitset<32>(encoder11).to_string().c_str(),
            digitalRead(EncoderPin0), digitalRead(EncoderPin1));
    if (mon) at.setTimeout(monitor, 100);
} 

void loop ()
{
    at.handle();
//    dumpConfig(3);
    if (Serial.available())
    {
        handleCommand(Serial.read());
    }
    buttonPoll();
    ledPoll();
    stepperPoll();

}
