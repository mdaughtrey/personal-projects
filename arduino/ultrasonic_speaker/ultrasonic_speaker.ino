#include <Arduino.h>
//#include <TFT_eSPI.h>
#include <SPI.h>
//#include "WiFi.h"
#include <Wire.h>
#include <AsyncTimer.h>

//#include "driver/rtc_io.h"
#include <driver/gpio.h>
#include <math.h>
//#include "sinetable.h"

#define _TIMERINTERRUPT_LOGLEVEL_ 0

#define PIN_PWM0 12 // -> input 3/4
#define PIN_PWM1 13 // -> input 1/2
#define FSH(s) reinterpret_cast<const __FlashStringHelper *>(s)

typedef struct
{
    char key;
    const __FlashStringHelper * help;
    //void (*help)();
    void (*fun)();
}Command;

Command * commandset;
int16_t param;
float audioFrequency = 500.0f;
//uint16_t pwmDuty = 0;
//uint16_t sinetable_index = 0;
volatile bool isrKick = false;
AsyncTimer at;
//ESP32Timer timer0(1);
//hw_timer_t * timer0Cfg = NULL;
uint8_t sineTable[256];
float envelopeTable[256];
uint8_t dutyTable[256];

//constexpr float audioFrequency = 400.0f;
constexpr float MODULATION_DEPTH = 0.85f;

constexpr uint32_t AUDIO_RATE = 16000;
constexpr uint8_t PWM_BITS = 8;
constexpr uint32_t CARRIER_FREQ = 40000;

float sweepLow = 1000.0;
float sweepHigh = 2000.0;
uint8_t sweepIndex = 0;

hw_timer_t *audioTimer = nullptr;

volatile uint32_t phase = 0;
volatile uint8_t currentDuty, prevDuty = 0;

//constexpr uint32_t PHASE_INC = (uint32_t)(audioFrequency * 4294967296.0 / AUDIO_RATE);
volatile uint32_t phaseIncrement = (uint32_t)(audioFrequency * 4294967296.0 / AUDIO_RATE);

void initSineTable()
{
    for (int i = 0; i < 256; i++)
    {
        float angle = (2.0f * PI * i) / 256.0f;
        sineTable[i] = (uint8_t)(127.5f + 127.5f * sinf(angle));
        float audio = ((float)sineTable[i] - 128.0f) / 128.0f;
        envelopeTable[i] = sqrt(1.0f + MODULATION_DEPTH * audio);
        dutyTable[i] = (uint8_t)(envelopeTable[i] * 127.0f);
    }
}

void IRAM_ATTR onAudioTick()
{
    phase += phaseIncrement;
//    uint8_t index = phase >> 24;
//    uint8_t duty = dutyTable[index];
    currentDuty = dutyTable[phase >> 24];

//    ledcWrite(PIN_PWM0, duty);
//    ledcWrite(PIN_PWM1, duty);
}

void help();


// void cmd_carrier_on()
// {
//     printf("Carrier On\r\n");
//     ledcChangeFrequency(PIN_PWM0, 40000, PWM_BITS);
//     ledcChangeFrequency(PIN_PWM1, 40000, PWM_BITS);
// }
// 
// void cmd_carrier_off()
// {
//     printf("Carrier Off\r\n");
//     timerEnd(audioTimer);
//     ledcChangeFrequency(PIN_PWM0, 0, PWM_BITS);
//     ledcChangeFrequency(PIN_PWM1, 0, PWM_BITS);
// }

void cmd_frequency()
{
    audioFrequency = (float)param;

}

void sweep_callback()
{
    sweepIndex++;
    audioFrequency = sweepLow + (sweepHigh * ((float)sineTable[sweepIndex])/256.0f);
    at.setTimeout(sweep_callback, 1/audioFrequency);
}

void cmd_sweep()
{
    sweepIndex = 0;
    audioFrequency = sweepLow;
    at.setTimeout(sweep_callback, 1000/audioFrequency);
}

//extern Command commands_stepper[];
extern Command commands_main[];
// extern Command commands_pwm[];

// const char help_dumpconfig[] PROGMEM="dump config";
// const char help_paramneg1[] PROGMEM="param *= -1";
// const char help_fanon[] PROGMEM="fan on";
// const char help_fanoff[] PROGMEM="fan off";
const char help_param0[] PROGMEM="param = 0";
// const char help_steppermenu[] PROGMEM="stepper menu";
// const char help_enclimit[] PROGMEM="encoder limit (param)";
const char help_carrier_on[] PROGMEM="40kHz Carrier On";
const char help_carrier_off[] PROGMEM="40kHz Carrier Off";
const char help_frequency[] PROGMEM="Set Frequency Hz (param)";
const char help_help[] PROGMEM="help";
const char help_sweep[] PROGMEM="Frequency Sweep";
// const char help_reset[] PROGMEM="reset config";
// const char help_lampon[] PROGMEM="lamp on";
// const char help_lampoff[] PROGMEM="lamp off";
// const char help_encpos0[] PROGMEM="encoder pos = 0";
// const char help_next[] PROGMEM="next (until encoderlimit)";
// const char help_nextimeout[] PROGMEM="next timeout (param)";
// const char help_isr[] PROGMEM="ISR on rising/falling edge (param 0=off, 1=rising, 2=falling, 3=both)";
// const char help_tension[] PROGMEM="tension (param)";
// const char help_tension0[] PROGMEM="tension 0";
// const char help_verbose[] PROGMEM="verbose 0-1 (param)";
// const char help_pwmmenu[] PROGMEM="PWM menu";
const char empty[] PROGMEM="";

Command commands_main[] = {
// {'?',FSH(help_dumpconfig), [](){}},
// {'-',FSH(help_paramneg1), [](){}},
// {'a',FSH(help_fanon), [](){}},
// {'A',FSH(help_fanoff), [](){}},
{'c',FSH(help_param0), [](){param = 0;}},
// {'C',FSH(help_steppermenu), [](){ }},
// {'e',FSH(help_enclimit), [](){}},
// {'E',FSH(help_encthresh), [](){}},
{'f',FSH(help_frequency), [](){ cmd_frequency();}},
{'h',FSH(help_help), [](){ help();}},
{'s',FSH(help_sweep), [](){ cmd_sweep();}},
// {' ',FSH(help_reset), [](){ setup();}},
// {'l',FSH(help_lampon), [](){}},
// {'L',FSH(help_lampoff), [](){}},
// {'m',FSH(help_encpos0), [](){}},
// {'n',FSH(help_next),[](){}},
// {'o',FSH(help_nextimeout), [](){}},
//{'r',FSH(help_carrier_on), []{cmd_carrier_on();}},
//{'R',FSH(help_carrier_off), []{cmd_carrier_off();}},
// {'r',FSH(help_isr), [](){}},
// {'t',FSH(help_tension), [](){}},
// {'T',FSH(help_tension0), [](){}},
// {'v',FSH(help_verbose), [](){}},
 {'&', FSH(empty), [](){} }
};

//! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms)
{
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

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

    if (lastCommand >= '0' & lastCommand <= '9')
    {
        param *= 10;
        param += lastCommand - '0';
    }
}

void IRAM_ATTR timerISR0()
{

    isrKick = true;
//    pwm0->setPWM_Int(PIN_PWM0, 40000.0, sinetable[sinetable_index]);
//    pwm1->setPWM_Int(PIN_PWM1, 40000.0, sinetable[sinetable_index]);
//    sinetable_index++;
//    sinetable_index %= NUM_SINETABLE;
}

void setup()
{
    delay(1000);
		initSineTable();
    Serial.begin(115200);

//    Serial.println("Start");
    commandset = commands_main;

    ledcAttach(PIN_PWM0, 40000, PWM_BITS);
    ledcAttach(PIN_PWM1, 40000, PWM_BITS);
    ledcOutputInvert(PIN_PWM1, true);

//    ledcWrite(PIN_PWM0, 128);
//    ledcWrite(PIN_PWM1, 128);

    audioTimer = timerBegin(1e6);
    timerAttachInterrupt(audioTimer, &onAudioTick);
//    timerAlarm(audioTimer, 1e6/frequency, true, 0);
    timerAlarm(audioTimer, 1e6/AUDIO_RATE, true, 0);

//    pinMode(PIN_PWM0, OUTPUT); 
//    pinMode(PIN_PWM1, OUTPUT); 
//    GPIO.func_out_sel_cfg[PIN_PWM1].inv_sel = 1;
//    pwm0->setPWM();
//    pwm1->setPWM();
//    timer0.attachInterruptInterval(1000, timerISR0);
//    timer0Cfg = timerBegin(0, 80, true);
//    timerAttachInterrupt(timer0Cfg, &timerISR0, true);
//    timerAlarmWrite(timer0Cfg, 125, true);
//    timerAlarmEnable(timer0Cfg);
    
}

void loop()
{
    if (currentDuty != prevDuty)
    {
        ledcWrite(PIN_PWM0, currentDuty);
        ledcWrite(PIN_PWM1, currentDuty);
        prevDuty = currentDuty;
    }
    at.handle();
    if (Serial.available())
    {
        handleCommand();
    }

//    if (isrKick)
//    {
//        pwm0->setPWM_Int(PIN_PWM0, 40000.0, sinetable[sinetable_index]);
//        pwm1->setPWM_Int(PIN_PWM1, 40000.0, sinetable[sinetable_index]);
//        sinetable_index++;
//        sinetable_index %= NUM_SINETABLE;
//        isrKick = false;
//    }
}
