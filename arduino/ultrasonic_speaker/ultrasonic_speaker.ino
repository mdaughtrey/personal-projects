// 
// Name: "ESP32_FastPWM"
//   Author: Khoi Hoang <khoih.prog@gmail.com>
//   Maintainer: Khoi Hoang <khoih.prog@gmail.com>
//   Sentence: This library enables you to use Hardware-based PWM channels on ESP32, ESP32_S2, ESP32_S3 or ESP32_C3-based boards to create and output PWM to pins.
//   Paragraph: The most important feature is they're purely hardware-based PWM channels, supporting very high PWM frequencies. Therefore, their executions are not blocked by bad-behaving functions or tasks. This important feature is absolutely necessary for mission-critical tasks. These hardware-based PWMs, still work even if other software functions are blocking. Moreover, they are much more precise (certainly depending on clock frequency accuracy) than other software-based PWM using ISR, millis() or micros(). That's necessary if you need to control devices requiring high precision. New efficient setPWM_manual function to facilitate waveform creation using PWM
//   Website: https://github.com/khoih-prog/ESP32_FastPWM
//   License: MIT
//   Category: Device Control
//   Architecture: esp32
//   Types: Contributed
//   Versions: [1.0.0, 1.0.1, 1.1.0]
//   Provides includes: ESP32_FastPWM.h

#define _PWM_LOG_LEVEL 0
#include <TFT_eSPI.h>
#include <SPI.h>
//#include "WiFi.h"
#include <Wire.h>
#include "ESP32_FastPWM.h"
#include <AsyncTimer.h>
//#include "Button2.h"
//#include <esp_adc_cal.h>
//#include "bmp.h"

//#include "driver/rtc_io.h"
#include "driver/gpio.h"
#include "sinetable.h"

#define _TIMERINTERRUPT_LOGLEVEL_ 0
#include "ESP32_New_TimerInterrupt.h"

// #define ADC_EN              14  //ADC_EN is the ADC detection enable port
// #define ADC_PIN             34
// #define BUTTON_1            35
// #define BUTTON_2            0
#define PIN_PWM0 12
#define PIN_PWM1 17
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
ESP32_FAST_PWM * pwm0;
ESP32_FAST_PWM * pwm1;
uint16_t pwmDuty = 0;
uint16_t sinetable_index = 0;
volatile bool isrKick = false;
//AsyncTimer at;
//ESP32Timer timer0(1);
hw_timer_t * timer0Cfg = NULL;

void help();
//extern Command commands_stepper[];
 extern Command commands_main[];
// extern Command commands_pwm[];

// const char help_dumpconfig[] PROGMEM="dump config";
// const char help_paramneg1[] PROGMEM="param *= -1";
// const char help_fanon[] PROGMEM="fan on";
// const char help_fanoff[] PROGMEM="fan off";
// const char help_param0[] PROGMEM="param = 0";
// const char help_steppermenu[] PROGMEM="stepper menu";
// const char help_enclimit[] PROGMEM="encoder limit (param)";
// const char help_encthresh[] PROGMEM="slow encoder threshold (param)";
const char help_help[] PROGMEM="help";
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
// const char empty[] PROGMEM="";

Command commands_main[] = {
// {'?',FSH(help_dumpconfig), [](){}},
// {'-',FSH(help_paramneg1), [](){}},
// {'a',FSH(help_fanon), [](){}},
// {'A',FSH(help_fanoff), [](){}},
// {'c',FSH(help_param0), [](){param = 0;}},
// {'C',FSH(help_steppermenu), [](){ }},
// {'e',FSH(help_enclimit), [](){}},
// {'E',FSH(help_encthresh), [](){}},
{'h',FSH(help_help), [](){ help();}}
// {' ',FSH(help_reset), [](){ setup();}},
// {'l',FSH(help_lampon), [](){}},
// {'L',FSH(help_lampoff), [](){}},
// {'m',FSH(help_encpos0), [](){}},
// {'n',FSH(help_next),[](){}},
// {'o',FSH(help_nextimeout), [](){}},
// {'p',FSH(help_pwmmenu), [](){}},
// {'r',FSH(help_isr), [](){}},
// {'t',FSH(help_tension), [](){}},
// {'T',FSH(help_tension0), [](){}},
// {'v',FSH(help_verbose), [](){}},
// {'&', FSH(empty), [](){} }
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

// at.setTimeout(setFrameReady, 10);

void setup()
{
    Serial.begin(115200);
    Serial.println("Start");
    espDelay(5000);
    commandset = commands_main;
    pwm0 = new ESP32_FAST_PWM(PIN_PWM0, 40000.0, 50.0, 10);
    pwm1 = new ESP32_FAST_PWM(PIN_PWM1, 40000.0, 50.0, 10);
    GPIO.func_out_sel_cfg[PIN_PWM1].inv_sel = 1;
    pwm0->setPWM();
    pwm1->setPWM();
//    at.setTimeout(setFrameReady, 10);
//    timer0.attachInterruptInterval(1000, timerISR0);
    timer0Cfg = timerBegin(0, 80, true);
    timerAttachInterrupt(timer0Cfg, &timerISR0, true);
    timerAlarmWrite(timer0Cfg, 125, true);
    timerAlarmEnable(timer0Cfg);
    
}

void loop()
{
    //at.handle();
    if (Serial.available())
    {
        handleCommand();
    }
    if (isrKick)
    {
        pwm0->setPWM_Int(PIN_PWM0, 40000.0, sinetable[sinetable_index]);
        pwm1->setPWM_Int(PIN_PWM1, 40000.0, sinetable[sinetable_index]);
        sinetable_index++;
        sinetable_index %= NUM_SINETABLE;
        isrKick = false;
    }

}
