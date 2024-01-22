#include <stdarg.h>
#include <bitset>
#include <pico/stdlib.h>
//#include <hardware/pwm.h>
//#include "serialout.h"
#include <AsyncTimer.h>
//#include <PicoCapSensing.h>

#define FSH(s) reinterpret_cast<const __FlashStringHelper *>(s)

typedef struct
{
    char key;
    const __FlashStringHelper * help;
    //void (*help)();
    void (*fun)();
}Command;
int16_t param;

const uint8_t EncoderPin0 = 14;
const uint8_t EncoderPin1 = 15;

AsyncTimer at;
uint8_t lastCommand;
uint8_t mon = 0;
uint8_t pinpoll = 0;
int isr0count = 0;
int isr1count = 0;
uint32_t encoder00, encoder01 = 0;
uint32_t encoder10, encoder11 = 0;

Command * commandset;
void help();
extern Command commands_main[];

void handleCommand();

void isr0()
{
    isr0count++;
}

void isr1()
{
    isr1count++;
}

void readIsr0()
{
    encoder00 <<= 1;
    encoder00 |= encoder01 >> 31;
    encoder01 <<= 1 ;
    if (!digitalRead(EncoderPin0)) encoder01 |= 1;
    //if (!digitalRead(EncoderPin0)) isr0count++;
    if (pinpoll) at.setTimeout(readIsr0, 100);
}

void readIsr1()
{
    encoder10 <<= 1;
    encoder10 |= encoder11 >> 31;
    encoder11 <<= 1 ;
    if (!digitalRead(EncoderPin1)) encoder11 |= 1;
    if (pinpoll) at.setTimeout(readIsr1, 100);
}

void setup ()
{ 
    Serial.begin(115200);

    pinMode(EncoderPin0,INPUT_PULLUP);
    pinMode(EncoderPin1,INPUT_PULLUP);
//    attachInterrupt(digitalPinToInterrupt(EncoderPin0), isr1, CHANGE);
//    attachInterrupt(digitalPinToInterrupt(EncoderPin1), isr2, CHANGE);
    commandset = commands_main;
    Serial.println("{State:Ready}");
}

void dumpConfig()
{
    Serial.printf("isr0count:%d isr1count %d\r\n",isr0count,isr1count);
}

const char help_dumpconfig[] PROGMEM="dump config";
const char help_paramneg1[] PROGMEM="param *= -1";
const char help_fanon[] PROGMEM="fan on";
const char help_fanoff[] PROGMEM="fan off";
const char help_param0[] PROGMEM="param = 0";
const char help_help[] PROGMEM="help";
const char help_reset[] PROGMEM="reset config";
const char help_lampon[] PROGMEM="lamp on";
const char help_lampoff[] PROGMEM="lamp off";
const char help_nextimeout[] PROGMEM="next timeout (param)";
const char help_isr[] PROGMEM="ISR on rising/falling edge (param 0=off, 1=rising, 2=falling, 3=both)";
const char help_os[] PROGMEM="one sample)";
const char help_mon[] PROGMEM="monitoring on)";
const char help_moff[] PROGMEM="monitoring off)";
const char help_pon[] PROGMEM="polling on)";
const char help_poff[] PROGMEM="polling off)";
//const char help_threshold[] PROGMEM="touch sense threshold (param)";
const char empty[] PROGMEM="";

Command commands_main[] = {
{'?',FSH(help_dumpconfig), [](){ dumpConfig();}},
{'-',FSH(help_paramneg1), [](){ param *= -1;}},
//{'A',FSH(help_fanoff), [](){fan(0);}},
{'c',FSH(help_param0), [](){param = 0;}},
{'h',FSH(help_help), [](){ help();}},
{'m',FSH(help_mon), [](){ mon = 1;}},
{'M',FSH(help_moff), [](){ mon = 0;}},
{'p',FSH(help_pon), [](){ pinpoll = 1; at.setTimeout(readIsr0, 1); at.setTimeout(readIsr1, 1); }},
{'P',FSH(help_poff), [](){ pinpoll = 0;}},
{' ',FSH(help_reset), [](){ setup();}},
//{'L',FSH(help_lampoff), [](){ lamp(0);}},
//{'n',FSH(help_next),[](){ do_next(); }},
//{'r',FSH(help_isr), [](){ do_isr();}},
//{'t',FSH(help_threshold), [](){ touch_sense_threshold = param; param = 0;}},
{'&', FSH(empty), [](){} }
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

void loop()
{
    if (Serial.available())
    {
        handleCommand();
    }
    if (mon) {
        std::bitset<32> d3(encoder00);
        std::bitset<32> d2(encoder01);
        std::bitset<32> d1(encoder10);
        std::bitset<32> d0(encoder11);


        Serial.printf("%s %s %s %s %d %d %d %d\r", d3.to_string().c_str(),d2.to_string().c_str(),
            d1.to_string().c_str(),d0.to_string().c_str(),
            d3.count(),d2.count(),d1.count(),d0.count());

//        char buffer[33];
//        itoa(encoder00,buffer,2);
//        Serial.printf("encoder0 %s ");
//        itoa(encoder01,buffer,2);
//        Serial.printf("%s ");
//        itoa(encoder10,buffer,2);
//        Serial.printf("encoder1 %s ");
//        itoa(encoder11,buffer,2);
//        Serial.printf("%s\r");
    }
    at.handle();
}
