/*
  iopyright (c) 2014-2015 NicoHood
  See the readme for credit to other people.

  Gamepad example
  Press a button and demonstrate Gamepad actions

  You can also use Gamepad1,2,3 and 4 as single report.
  This will use 1 endpoint for each gamepad.

  See HID Project documentation for more infos
  https://github.com/NicoHood/HID/wiki/Gamepad-API
*/
// https://github.com/NicoHood/HID/wiki/Gamepad-API

#include "HID-Project.h"
#include <AsyncTimer.h>
#include <EEPROM.h>
#include <PrintEx.h>
#include <TM1638plus.h>
#include "Stepper.h"
//#include <avr/wdt.h>

using namespace ios;

#define TM_STROBE 14
#define TM_CLOCK 15
#define TM_DIO 16

typedef struct
{
    char key;
    void (*help)();
//    char help[32];
    void (*fun)();
}Command;

typedef enum
{
    EV_BUTTONCHASER = 0x0001,
    EV_XAXIS = 0x0002,
    EV_YAXIS = 0x0004,
    EV_ZAXIS = 0x0008,
    EV_RXAXIS = 0x0010,
    EV_RYAXIS = 0x0020,
    EV_RZAXIS = 0x0040,
    EV_DPAD2 = 0x0080,
    EV_DPAD1 =0x0100 
}Event;

typedef enum
{
    UPDATE_NONE = 0,
    UPDATE_STEERING = 0x01,
    UPDATE_PEDAL0 = 0x02,
    UPDATE_PEDAL1 = 0x04,
} Update;

typedef struct
{
    uint16_t encoder0ValMax;
    uint16_t stepDelay;
    uint8_t display;
}Config;

Config config;

//const uint8_t stepStepPin = 9;
//const uint8_t stepDirPin = 10;
//const uint8_t stepResetPin = 11;
//const uint8_t stepEnablePin = 2;
//const uint8_t stepPin0 = 9;
//const uint8_t stepPin1 = 10;
//const uint8_t stepPin2 = 11;
//const uint8_t stepPin3 = 2;


const uint8_t Encoder0Pin0 = 0;
const uint8_t Encoder0Pin1 = 1;
const uint8_t Pedal0Pin = 0;
const uint8_t Pedal1Pin = 1;

volatile uint8_t isr0Count = 0;
volatile uint8_t isr1Count = 0;
volatile Update updateUSB = UPDATE_NONE;

uint16_t pedal0PinLast = 0;
uint16_t pedal1PinLast = 0;

uint16_t countLoop = 0;
uint16_t countIoCheck = 0;
uint16_t countUpdateUSB = 0;
uint32_t timeUSBWrite = 0;

volatile uint16_t encoder0Val = 0;
//uint16_t config.encoder0Max = 0xffff;
int16_t encoder0Scaled = 0;
//int16_t encoder0ScaledPrev = 0;
uint16_t xAxis = 0;
uint16_t yAxis = 0;
uint8_t zAxis = 0;

uint16_t rxAxis = 0;
uint16_t ryAxis = 0;
uint8_t rzAxis = 0;

uint8_t dpad2 = 0;
uint8_t dpad1 = 0;

uint16_t event = 0;
int16_t param;
volatile uint16_t stepCount = 0;
uint32_t buttonState = 0;
AsyncTimer at;
StreamEx serout = Serial;
Stepper stepper(9, 10, 11, 2);
TM1638plus tm (TM_STROBE, TM_CLOCK, TM_DIO, false);
void setEvent(uint8_t ev) { event |= ev; }
void resetEvent(uint8_t ev) { event &= ~ev; }
bool isEvent(uint8_t ev) { return event & ev; }

void (*hardReset) (void) = 0;

void saveConfig()
{
    uint8_t * addr(reinterpret_cast<uint8_t *>(&config));
    for (uint16_t ii = 0; ii < sizeof(config); ii++)
    {
        Serial << F("saveConfig ") << hex << *addr << endl;
        EEPROM.update(ii, (*addr++));
    }
}

void loadConfig()
{
    uint8_t * addr(reinterpret_cast<uint8_t *>(&config));
    for (uint16_t ii = 0; ii < sizeof(config); ii++)
    {
        (*addr) = EEPROM.read(ii);
        Serial << F("loadConfig ") << hex << *addr << endl;
        addr++;
    }
}

const char * pmGet(const __FlashStringHelper * str)
{
    char buffer[64];
    strcpy_P(buffer, reinterpret_cast<const char *>(str));
    serout << F("pmGet: ") << buffer << endl;
//    uint8_t n = 0;
//    while (n < 63)
//    {
//        if (char c = pgm_read_byte(reinterpret_cast<const char *>(str) + n))
//        {
//            buffer[n++] = c;
//            continue;
//        }
//        break;
//    }
//    buffer[n] = 0;
    return buffer;
}


void isrEncoder00(void)
{
    if (digitalRead(Encoder0Pin1))
    {
        if (encoder0Val)
        {
            encoder0Val--;
//            Gamepad.xAxis(encoder0Val);
//            writeReady = 1;
        }
    }
    updateUSB |= UPDATE_STEERING;
    isr0Count++;
}

void isrEncoder01(void)
{
    if (digitalRead(Encoder0Pin0))
    {
        if (encoder0Val < config.encoder0ValMax)
        {
            encoder0Val++; //  steeringInc;
//            Gamepad.xAxis(encoder0Val);
//            writeReady = 1;
        }
    }
    updateUSB |= UPDATE_STEERING;
    isr1Count++;
}



void setup() {
    Serial.begin(115200);
//    Serial.println("{State:Ready}");
//    wdt_enable(WDTO_2S);
    pinMode(Encoder0Pin0, INPUT_PULLUP);
    pinMode(Encoder0Pin1, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(Encoder0Pin0), isrEncoder00, FALLING);
    attachInterrupt(digitalPinToInterrupt(Encoder0Pin1), isrEncoder01, FALLING);
//    for (uint8_t ii = 0; ii < 17; ii++)
//    {
//        pinMode(ii, INPUT_PULLUP);
//    }
    softReset();
//    updateDisplay();
}

void pedalTest(uint8_t pin)
{
    int16_t val = analogRead(pin);
    if (pin == Pedal0Pin && val != pedal0PinLast)
    {
        pedal0PinLast = val;
        updateUSB |= UPDATE_PEDAL0;
    }
    if (pin == Pedal1Pin && val != pedal1PinLast)
    {
        pedal1PinLast = val;
        updateUSB |= UPDATE_PEDAL1;
    }
}

int16_t getPedalScaled(uint16_t val)
{
    return (val - 512) * 64;
}

int16_t getEncoder0Scaled()
{
    int16_t val = encoder0Val - config.encoder0ValMax/2;
    return val * 32;
}

bool commandB()
{
    Gamepad.press(param);
    delay(100);
    Gamepad.release(param);
    return 1;
}


void updateDisplay()
{
    char buffer[9];
    switch (config.display)
    {
//        case 0: tm.reset(); config.display = 4; break;
//        case 1: tm.displayIntNum(getEncoder0Scaled()); break;
//        case 2: tm.displayIntNum(stepCount); break;
//        case 3: tm.displayIntNum(countLoop); break;
//        case 4: tm.displayIntNum(countIoCheck); break;
//        case 5: tm.displayIntNum(countUpdateUSB); break;
//    }

    case 0: break;
    case 1:
        sprintf_P(buffer, PSTR("Enc0%04x"), getEncoder0Scaled());
        tm.displayText(buffer);
        break;
    case 2:
        sprintf_P(buffer, PSTR("Stct%04x"), stepCount);
        tm.displayText(buffer);
        break;
    case 3:
        sprintf_P(buffer, PSTR("Loop%04x"), countLoop);
        tm.displayText(buffer);
        break;
    case 4:
        sprintf_P(buffer, PSTR("IO  %04x"), countIoCheck);
        tm.displayText(buffer);
        break;
    case 5:
        sprintf_P(buffer, PSTR("USBc%04x"), countUpdateUSB);
        tm.displayText(buffer);
        break;
    case 6:
        sprintf_P(buffer, PSTR("USBt%04x"), timeUSBWrite);
        tm.displayText(buffer);
        break;
    case 7:
        sprintf_P(buffer, PSTR("UUSB%04x"), updateUSB);
        tm.displayText(buffer);
        break;
    default: tm.reset(); config.display = 0; break;
    }



 //   uint8_t val = 0xff << ((1024 + pedal1PinLast - pedal0PinLast) >> 8);
//    tm.setLEDs(~val << 8);
    at.setTimeout(updateDisplay, 100);
}

void softReset()
{
    serout << F("Soft Reset");
    stepper.init();
    //loadConfig();
    config.encoder0ValMax = 0xffff;
    config.stepDelay = 100;
    config.display = 0;
    xAxis = 0;
    yAxis = 0;
    zAxis = 0;
    rxAxis = 0;
    ryAxis = 0;
    rzAxis = 0;
    buttonState = 0;
    event = 0;
    encoder0Val = config.encoder0ValMax/2;
    tm.displayBegin();
    for (uint8_t ii = 0; ii < 3; ii++)
    {
        tm.setLED(0, 1);
        tm.displayText("Init");
        delay(100);
        tm.setLED(0, 0);
        tm.reset();
        delay(100);
    }
    Gamepad.begin();
    checkButtons();
    updateDisplay();
    ioCheck();
}

//void test()
//{
//    pinMode(Encoder0Pin0, OUTPUT);
//    pinMode(Encoder0Pin1, OUTPUT);
//    while (1) {
//        digitalWrite(Encoder0Pin0, LOW);
//        digitalWrite(Encoder0Pin1, HIGH);
//        delay(100);
//        digitalWrite(Encoder0Pin1, LOW);
//        digitalWrite(Encoder0Pin0, HIGH);
//        delay(100);
//    }
//}
//const char helpDisplay[] PROGMEM = "Set Display 0=Off 1=Opto 2=stepcount, 3=loopcount 4=I/O Check, 5=update USB";
const char helpStepEnable[] PROGMEM = "Enable stepper";
const char helpStepDisable[] PROGMEM = "Disable stepper";
//const char helpb[] PROGMEM = "Reset stepper";
//const char helpB[] PROGMEM = "Button Chaser";
const char helpc[] PROGMEM = "Clear param";
const char helpDir0[] PROGMEM = "Stepper Dir 0";
const char helpDir1[] PROGMEM = "Stepper Dir 1";
//const char helpe[] PROGMEM = "X Axis Wipe";
const char helpStepDelay[] PROGMEM = "Step Delay ms (param)";
//const char helpf[] PROGMEM = "Y Axis Wipe";
//const char helpg[] PROGMEM = "Z Axis Wipe";
const char helph[] PROGMEM = "Help";
//const char helpI[] PROGMEM = "RX Axis Wipe";
//const char helpj[] PROGMEM = "RY Axis Wipe";
//const char helpk[] PROGMEM = "RZ Axis Wipe";
const char helpLoadConfig[] PROGMEM = "Load Config";
const char helplt[] PROGMEM = "b0 Set leftmost steer";
const char helpgt[] PROGMEM = "b1 Set righttmost steer";
const char helpp[] PROGMEM = "DPad1 wipe";
const char helpP[] PROGMEM = "DPad2 wipe";
const char helpHardReset[] PROGMEM = "Hard Reset";
const char helpSteps[] PROGMEM = "Steps (param)";
const char helpS[] PROGMEM = "Save Config";
const char helpStepperReset[] PROGMEM = "Reset Stepper";
//const char helpy[] PROGMEM = "16-bit Y Axis (param)";
//const char helpY[] PROGMEM = "16-bit rY Axis (param)";
const char helpSpc[] PROGMEM = "b7 Soft Reset";

#define FSH(s) reinterpret_cast<const __FlashStringHelper *>(s)

void setEncoder0Min()
{
    encoder0Val = 0;
    config.encoder0ValMax = 0xffff;
}

void step()
{
    if (stepCount-- && config.stepDelay)
    {
        stepper.next();
        at.setTimeout(step, config.stepDelay);
    }
}


Command cmds[] = {
 //   {'b', []() { serout << FSH(helpb) << endl; }, []() { 
 //       digitalWrite(stepResetPin, 0);
 //       digitalWrite(stepResetPin, 1);
 //       }},
//    {'a', []() { serout << FSH(helpa) << endl; }  , []() { config.display = param;param = 0; }},
//    {'B', []() { serout << FSH(helpB) << endl; }  , []() { setEvent(EV_BUTTONCHASER); }},
    {'c', []() { serout << FSH(helpc) << endl; }  , [](){ param = 0; }},
    {'d', []() { serout << FSH(helpDir0) << endl; }  , [](){ stepper.dir(0); }},
    {'D', []() { serout << FSH(helpDir1) << endl; }  , [](){ stepper.dir(1); }},
//    {'E', []() { serout << FSH(helpStepEnable) << endl; }, []() { digitalWrite(stepEnablePin, 1); }},
//    {'e', []() { serout << FSH(helpStepDisable) << endl; }, []() { digitalWrite(stepEnablePin, 0); }},
//    {'e', []() { serout << FSH(helpe) << endl; }  , []() { setEvent(EV_XAXIS); }},
//    {'f', []() { serout << FSH(helpf) << endl; }  , []() { setEvent(EV_YAXIS); }},
//    {'g', []() { serout << FSH(helpf) << endl; }  , []() { setEvent(EV_ZAXIS); }},
    {'h', []() { serout << FSH(helph) << endl; }  , [](){ help(); }},
//    {'I', []() { serout << FSH(helpI) << endl; }  , []() { setEvent(EV_RXAXIS); }},
//    {'j', []() { serout << FSH(helpj) << endl; }  , []() { setEvent(EV_RYAXIS); }},
//    {'k', []() { serout << FSH(helpk) << endl; }  , []() { setEvent(EV_RZAXIS); }},
//    {'i', []() { serout << FSH(helpDisplay) << endl; }  , []() { config.display = param;param = 0; }},
    {'l', []() { serout << FSH(helpStepDelay) << endl; }  , []() { config.stepDelay = param;  param = 0;}},
    {'L', []() { serout << FSH(helpLoadConfig) << endl; }  , []() { loadConfig(); }},
    {'<', []() { serout << FSH(helplt) << endl; } , []{ setEncoder0Min(); }},
    {'>', []() { serout << FSH(helpgt) << endl; } , []{ config.encoder0ValMax = encoder0Val; }},
    {'p', []() { serout << FSH(helpp) << endl; }  , []() { setEvent(EV_DPAD1); }},
    {'P', []() { serout << FSH(helpP) << endl; }  , []() { setEvent(EV_DPAD2); }},
//    {'r', []() { serout << FSH(helpStepperReset) << endl; }, []() { 
//        digitalWrite(stepResetPin, 0);
//        digitalWrite(stepResetPin, 1);
//        }},
    {'R', []() { serout << FSH(helpHardReset) << endl; }  , []() { hardReset(); }},
    {'s', []() { serout << FSH(helpSteps) << endl; }  , []() { stepCount = param; param = 0; step(); }},
    {'S', []() { serout << FSH(helpS) << endl; }  , []() { saveConfig(); }},
//    {'y', []() { serout << FSH(helpy) << endl; }  , [](){ Gamepad.yAxis(param); }},
//    {'Y', []() { serout << FSH(helpY) << endl; }  , [](){ Gamepad.ryAxis(param); }},
    {' ', []() { serout << FSH(helpSpc) << endl; }, [](){ softReset(); return 0; }}
};

#define NUMCOMMANDS (sizeof(cmds)/sizeof(cmds[0]))

void help()
{
    for (uint8_t ii = 0; ii < NUMCOMMANDS; ii++)
    {
        serout << cmds[ii].key << ": ";
        cmds[ii].help();
    }
    char buffer[12];
    itoa(event, buffer, 2);
    serout << F("Event ") << buffer << endl;
    serout << F("encoder0Val ") << reinterpret_cast<uint16_t>(encoder0Val) << endl;
    serout << F("encoder0Scaled ") << getEncoder0Scaled() << endl;
    serout << F("isr0Count ") << reinterpret_cast<uint8_t>(isr0Count) << endl;
    serout << F("isr1Count ") << reinterpret_cast<uint8_t>(isr1Count) << endl;
    serout << F("Encoder0Pin0 ") << digitalRead(Encoder0Pin0) << endl;
    serout << F("Encoder0Pin1 ") << digitalRead(Encoder0Pin1) << endl;
    serout << F("Pedal0Val ") << analogRead(Pedal0Pin) << endl;
    serout << F("Pedal1Val ") << analogRead(Pedal1Pin) << endl;
    serout << F("Pedal0Scaled ") << getPedalScaled(Pedal0Pin) << endl;
    serout << F("Pedal1Scaled ") << getPedalScaled(Pedal1Pin) << endl;
    serout << F("param ") << param << endl;
    serout << F("Config") << endl;
    serout << F("-- encoder0ValMax ") << config.encoder0ValMax << endl;
    serout << F("-- stepDelay ") << config.stepDelay << endl;
    serout << F("-- display ") << config.display << endl;
}

void handleCommand()
{
	uint8_t lastCommand;
    lastCommand = Serial.read();
    for (uint8_t ii = 0; ii < NUMCOMMANDS; ii++)
    {
        if (lastCommand == cmds[ii].key)
        {
            cmds[ii].fun();
        }
    }

    if (lastCommand >= '0' & lastCommand <= '9')
    {
        param *= 10;
        param += lastCommand - '0';
    }
}

void checkButtons()
{
    uint16_t tmp;
    tmp = tm.readButtons();
    if (tmp & 0x01)
    {
        setEncoder0Min();
    }
    if (tmp & 0x02)
    {
        config.encoder0ValMax = encoder0Val; 
    }
    if (tmp & 1<< 6)
    {
        config.display++;
    }
    if (tmp & 1<< 7)
    {
        softReset();
    }
//    at.setTimeout(checkButtons, 50);
}

void ioCheck()
{
    int32_t ttime;
    countIoCheck++;
    checkButtons();
    pedalTest(Pedal0Pin);
    pedalTest(Pedal1Pin);
    if (updateUSB != UPDATE_NONE)
    {
        countUpdateUSB++;
        if (updateUSB & UPDATE_STEERING)
        {
            Gamepad.xAxis(getEncoder0Scaled());
        }
        if (updateUSB & UPDATE_PEDAL0)
        {
            Gamepad.rxAxis(getPedalScaled(pedal0PinLast));
        }
        if (updateUSB & UPDATE_PEDAL1)
        {
            Gamepad.ryAxis(getPedalScaled(pedal1PinLast));
        }
        ttime = millis();
        Gamepad.write();
        timeUSBWrite = millis() - ttime;
        updateUSB = UPDATE_NONE;
    }
    at.setTimeout(ioCheck, 50);
}

void loop()
{
    countLoop++;
    int16_t tmp;
    //wdt_reset();
    if (Serial.available())
    {
        handleCommand();
    }
    at.handle();
}
