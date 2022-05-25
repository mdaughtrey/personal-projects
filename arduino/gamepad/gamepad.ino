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
#include <avr/wdt.h>

typedef struct
{
    char key;
    char help[32];
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
void setEvent(uint8_t ev) { event |= ev; }
void resetEvent(uint8_t ev) { event &= ~ev; }
bool isEvent(uint8_t ev) { return event & ev; }


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
//    wdt_enable(WDTO_2S);
    Gamepad.begin();
}

bool commandB()
{
    Gamepad.press(param);
    delay(100);
    Gamepad.release(param);
    return 1;
}

void buttonChaser()
{
    if (!buttonState)
    {
        verbose("buttonChaser stop");
        resetEvent(EV_BUTTONCHASER);
        return;
    }
    writeReady = 1;
    Gamepad.buttons(buttonState);
    verbose("buttonChaser %04x\r", buttonState);
    buttonState <<= 1;
    at.setTimeout(buttonChaser, 100);
}

void xAxisWipe()
{
    if (xAxis > 65000)
    {
        verbose("xAxisWipe stop");
        resetEvent(EV_XAXIS);
        return;
    }
    writeReady = 1;
    Gamepad.xAxis(xAxis);
    verbose("xAxisWipe %d\r", xAxis);
    xAxis += 10;
    at.setTimeout(xAxisWipe, 100);
}

void yAxisWipe()
{
    if (yAxis > 65000)
    {
        verbose("yAxisWipe stop");
        resetEvent(EV_YAXIS);
        return;
    }
    writeReady = 1;
    Gamepad.yAxis(yAxis);
    verbose("yAxisWipe %d\r", yAxis);
    yAxis += 10;
    at.setTimeout(yAxisWipe, 100);
}

void zAxisWipe()
{
    if (zAxis > 240)
    {
        verbose("zAxisWipe stop");
        resetEvent(EV_ZAXIS);
        return;
    }
    writeReady = 1;
    Gamepad.zAxis(zAxis);
    verbose("zAxisWipe %u\r", zAxis);
    zAxis += 10;
    at.setTimeout(zAxisWipe, 100);
}

void rxAxisWipe()
{
    if (rxAxis > 240)
    {
        verbose("rxAxisWipe stop");
        resetEvent(EV_RXAXIS);
        return;
    }
    writeReady = 1;
    Gamepad.rxAxis(rxAxis);
    verbose("rxAxisWipe %u\r", rxAxis);
    rxAxis += 10;
    at.setTimeout(rxAxisWipe, 100);
}

void ryAxisWipe()
{
    if (ryAxis > 240)
    {
        verbose("ryAxisWipe stop");
        resetEvent(EV_RYAXIS);
        return;
    }
    writeReady = 1;
    Gamepad.ryAxis(ryAxis);
    verbose("ryAxisWipe %u\r", ryAxis);
    ryAxis += 10;
    at.setTimeout(ryAxisWipe, 100);
}

void rzAxisWipe()
{
    if (rzAxis > 240)
    {
        verbose("rzAxisWipe stop");
        resetEvent(EV_RZAXIS);
        return;
    }
    writeReady = 1;
    Gamepad.rzAxis(rzAxis);
    verbose("rzAxisWipe %u\r", rzAxis);
    rzAxis += 10;
    at.setTimeout(rzAxisWipe, 100);
}

void doReset()
{
    xAxis = 0;
    yAxis = 0;
    zAxis = 0;
    rxAxis = 0;
    ryAxis = 0;
    rzAxis = 0;
    buttonState = 0;
    debug = 0;
    doWrite = 0;
    event = 0;
}

Command cmds[] = {
    {'b', "Toggle button (param)", commandB},
    {'B', "Button Chaser", []() { setEvent(EV_BUTTONCHASER); return 0;}},
    {'c', "Clear param", [](){ param = 0; return 0; }},
    {'d', "Debug on", []() { debug = 1; return 0;}},
    {'D', "Debug off", []() { debug = 0; return 0;}},
    {'e', "X Axis Wipe", []() { setEvent(EV_XAXIS); return 0; }},
    {'f', "Y Axis Wipe", []() { setEvent(EV_YAXIS); return 0; }},
    {'g', "Z Axis Wipe", []() { setEvent(EV_ZAXIS); return 0; }},
    {'h', "Help", [](){ help(); return 0; }},
    {'i', "RX Axis Wipe", []() { setEvent(EV_RXAXIS); return 0; }},
    {'j', "RY Axis Wipe", []() { setEvent(EV_RYAXIS); return 0; }},
    {'k', "RZ Axis Wipe", []() { setEvent(EV_RZAXIS); return 0; }},
    {'r', "Reset", []() { while (1); }},
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
    char buffer[12];
    itoa(event, buffer, 2);
    verbose("Event %s\r\n", buffer);
    verbose("doWrite %u\r\n", doWrite);
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

void loop()
{
    //wdt_reset();
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
    if (isEvent(EV_BUTTONCHASER))
    {
        buttonState = 1;
        resetEvent(EV_BUTTONCHASER);
        verbose("buttonChaser start\r\n");
        buttonChaser();
    }
    if (isEvent(EV_XAXIS))
    {
        xAxis = 10;
        resetEvent(EV_XAXIS);
        verbose("xAxisWipe start\r\n");
        xAxisWipe();
    }
    if (isEvent(EV_YAXIS))
    {
        yAxis = 10;
        resetEvent(EV_YAXIS);
        verbose("yAxisWipe start\r\n");
        yAxisWipe();
    }
    if (isEvent(EV_ZAXIS))
    {
        zAxis = 10;
        resetEvent(EV_ZAXIS);
        verbose("zAxisWipe start\r\n");
        zAxisWipe();
    }
    if (isEvent(EV_RXAXIS))
    {
        rxAxis = 10;
        resetEvent(EV_RXAXIS);
        verbose("rxAxisWipe start\r\n");
        rxAxisWipe();
    }
    if (isEvent(EV_RYAXIS))
    {
        ryAxis = 10;
        resetEvent(EV_RYAXIS);
        verbose("ryAxisWipe start\r\n");
        ryAxisWipe();
    }
    if (isEvent(EV_RZAXIS))
    {
        rzAxis = 10;
        resetEvent(EV_RZAXIS);
        verbose("rzAxisWipe start\r\n");
        rzAxisWipe();
    }
    at.handle();
   

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
