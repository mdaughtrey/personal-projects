/*
  Copyright (c) 2014-2015 NicoHood
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

typedef struct
{
    char key;
    char help[24];
    bool (*fun)();
}Command;

typedef enum
{
    EV_BUTTONCHASER = 1,
    EV_XAXIS = 2,
    EV_YAXIS = 4,
    EV_ZAXIS = 8,
    EV_RXAXIS = 16,
    EV_RYAXIS = 32,
    EV_RZAXIS = 64
}Event;

uint16_t xAxis = 0;
uint16_t yAxis = 0;
uint8_t zAxis = 0;

uint8_t rxAxis = 0;
uint8_t ryAxis = 0;
uint8_t rzAxis = 0;

uint8_t debug = 0;
uint8_t doWrite = 0;
uint8_t event = 0;
int16_t param;
uint8_t lastCommand;
uint32_t buttonState = 0;
uint8_t writeReady = 0;
AsyncTimer at;
//uat.setTimeout(setFrameReady, 10);
void setEvent(uint8_t ev} { event |= ev; }
void resetEvent(uint8_t ev} { event &= ~ev; }
void isEvent(uint8 ev) { event &= ev; }


void verbose(const char * fmt, ...)
{
    if (!debug)
    {
        return;
    }
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, 255, fmt, args);
    va_end(args);
    Serial.print(buffer);
}

void setup() {
    Serial.begin(115200);
    Serial.println("{State:Ready}");
    Gamepad.begin();
}

bool commandB()
{
    Gamepad.press(param);
    delay(100);
    Gamepad.release(param);
    return 1;
}

bool buttonChaser()
{
    buttonState <<= 1;
    if (~buttonState)
    {
        resetEvent(EV_BUTTONCHASER);
    }
    if (isEvent(EV_BUTTONCHASER))
    {
        writeReady = 1;
        Gamepad.buttons(buttonState);
        verbose("buttonChaser %04x\r", buttonState);
        at.setTimeout(buttonChaser, 100);
    }
}

bool xAxisWipe()
{
    xAxis++;
    if (!xAxis)
    {
        resetEvent(EV_BUTTONCHASER);
    }
    if (isEvent(EV_XAXIS))
    {
        writeReady = 1;
        Gamepad.xAxis(xAxis);
        verbose("xAxisWipe %u\r", xAxis);
        at.setTimeout(xAxisWipe, 10);
    }
}

bool yAxisWipe()
{
    yAxis++;
    if (!yAxis)
    {
        resetEvent(EV_YAXIS);
    }
    if (isEvent(EV_YAXIS))
    {
        writeReady = 1;
        Gamepad.yAxis(yAxis);
        verbose("yAxisWipe %u\r", yAxis);
        at.setTimeout(yAxisWipe, 10);
    }
}

bool zAxisWipe()
{
    zAxis++;
    if (!zAxis)
    {
        resetEvent(EV_ZAXIS);
    }
    if (isEvent(EV_ZAXIS))
    {
        writeReady = 1;
        Gamepad.zAxis(zAxis);
        verbose("zAxisWipe %u\r", zAxis);
        at.setTimeout(zAxisWipe, 10);
    }
}

bool rxAxisWipe()
{
    rxAxis++;
    if (!rxAxis)
    {
        resetEvent(EV_RXAXIS);
    }
    if (isEvent(EV_RXAXIS))
    {
        writeReady = 1;
        Gamepad.rxAxis(rxAxis);
        verbose("rxAxisWipe %u\r", rxAxis);
        at.setTimeout(rxAxisWipe, 10);
    }
}

bool ryAxisWipe()
{
    ryAxis++;
    if (ryAxis)
    {
        writeReady = 1;
        Gamepad.ryAxis(ryAxis);
        verbose("ryAxisWipe %u\r", ryAxis);
        at.setTimeout(ryAxisWipe, 10);
    }
}

bool rzAxisWipe()
{
    rzAxis++;
    if (rzAxis)
    {
        writeReady = 1;
        Gamepad.rzAxis(rzAxis);
        verbose("rzAxisWipe %u\r", rzAxis);
        at.setTimeout(rzAxisWipe, 10);
    }
}

void doReset()
{
    xAxis = 0;
    yAxis = 0;
    zAxis = 0;
    rxAxis = 0;
    ryAxis = 0;
    rzAxis = 0;
    debug = 0;
    doWrite = 0;
    event = 0;
}

Command cmds[] = {
    {'b', "Toggle button (param)", commandB},
    {'B', "Button Chaser", []() { event |= EV_BUTTONCHASER; return 0;}},
    {'c', "Clear param", [](){ param = 0; return 0; }},
    {'d', "Debug on", []() { debug = 1; return 0;}},
    {'D', "Debug off", []() { debug = 0; return 0;}},
    {'e', "X Axis Wipe", []() { event |= EV_XAXIS; return 0; }},
    {'f', "Y Axis Wipe", []() { event |= EV_YAXIS; return 0; }},
    {'g', "Z Axis Wipe", []() { event |= EV_ZAXIS; return 0; }},
    {'h', "Help", [](){ help(); return 0; }},
    {'i', "RX Axis Wipe", []() { event |= EV_RXAXIS; return 0; }},
    {'j', "RY Axis Wipe", []() { event |= EV_RYAXIS; return 0; }},
    {'k', "RZ Axis Wipe", []() { event |= EV_RZAXIS; return 0; }},
    {'w', "USB Write on", []() { doWrite = 1; return 0; }},
    {'W', "USB Write off", []() { doWrite = 0; return 0; }},
    {'x', "16-bit X Axis (param)", [](){ Gamepad.xAxis(param); return 1; }},
    {'y', "16-bit Y Axis (param)", [](){ Gamepad.yAxis(param); return 1; }},
    {'z', "8-bit lZ Axis (param)", [](){ Gamepad.zAxis(param); return 1; }},
    {'X', "8-bit X Axis (param)", [](){ Gamepad.rxAxis(param); return 1; }},
    {'Y', "8-bit Y Axis (param)", [](){ Gamepad.ryAxis(param); return 1; }},
    {' ', "Reset", [](){ doReset(); return 0; }}
};

#define NUMCOMMANDS (sizeof(cmds)/sizeof(cmds[0]))

void help()
{
    for (uint8_t ii = 0; ii < NUMCOMMANDS; ii++)
    {
        verbose("%c: %s\r\n", cmds[ii].key, cmds[ii].help);
    }
}

void handleCommand()
{
	uint8_t lastCommand;
    lastCommand = Serial.read();

    for (uint8_t ii = 0; ii < NUMCOMMANDS; ii++)
    {
        if (lastCommand == cmds[ii].key)
        {
            if (cmds[ii].fun())
            {
                writeReady = 1;
            }
            return;
        }
    }

    if (lastCommand >= '0' & lastCommand <= '9')
    {
        param *= 10;
        param += lastCommand - '0';
    }
}

void loop() {
    if (Serial.available())
    {
        handleCommand();
    }
    if (writeReady)
    {
        if (doWrite)
        {
            Gamepad.write();
        }
        writeReady = 0;
    }
    if (event & EV_BUTTONCHASER)
    {
        event &= ~EV_BUTTONCHASER;
        buttonChaser();
    }
    if (event & EV_XAXIS)
    {
        event &= ~EV_XAXIS;
        xAxisWipe();
    }
    if (event & EV_YAXIS)
    {
        event &= ~EV_YAXIS;
        yAxisWipe();
    }
    if (event & EV_ZAXIS)
    {
        event &= ~EV_ZAXIS;
        zAxisWipe();
    }
    if (event & EV_RXAXIS)
    {
        event &= ~EV_RXAXIS;
        rxAxisWipe();
    }
    if (event & EV_RYAXIS)
    {
        event &= ~EV_RYAXIS;
        ryAxisWipe();
    }
    if (event & EV_RZAXIS)
    {
        event &= ~EV_RZAXIS;
        rzAxisWipe();
    }
    if (event)
    {
        at.handle();
    }

//  if (!digitalRead(pinButton)) {
//    digitalWrite(pinLed, HIGH);
//
//    // Press button 1-32
//    static uint8_t count = 0;
//    count++;
//    if (count == 33) {
//      Gamepad.releaseAll();
//      count = 0;
//    }
//    else
//      Gamepad.press(count);
//
//    // Move x/y Axis to a new position (16bit)
//    Gamepad.xAxis(random(0xFFFF));
//    Gamepad.yAxis(random(0xFFFF));
//
//    // Go through all dPad positions
//    // values: 0-8 (0==centered)
//    static uint8_t dpad1 = GAMEPAD_DPAD_CENTERED;
//    Gamepad.dPad1(dpad1++);
//    if (dpad1 > GAMEPAD_DPAD_UP_LEFT)
//      dpad1 = GAMEPAD_DPAD_CENTERED;
//
//    static int8_t dpad2 = GAMEPAD_DPAD_CENTERED;
//    Gamepad.dPad2(dpad2--);
//    if (dpad2 < GAMEPAD_DPAD_CENTERED)
//      dpad2 = GAMEPAD_DPAD_UP_LEFT;
//
//    // Functions above only set the values.
//    // This writes the report to the host.
//    Gamepad.write();
//
//    // Simple debounce
//    delay(300);
//    digitalWrite(pinLed, LOW);
//  }
}
