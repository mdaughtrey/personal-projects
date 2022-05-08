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

typedef struct
{
    char key;
    char help[24];
    bool (*fun)();
}Command;

int16_t param;
uint8_t lastCommand;

void verbose(const char * fmt, ...)
{
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

Command cmds[] = {
    {'b', "Toggle button (param)", commandB},
    {'c', "Clear param", [](){ param = 0; return 0; }},
    {'h', "Help", [](){ help(); return 0; }},
    {'x', "16-bit X Axis (param)", [](){ Gamepad.xAxis(param); return 1; }},
    {'y', "16-bit Y Axis (param)", [](){ Gamepad.yAxis(param); return 1; }},
    {'z', "8-bit lZ Axis (param)", [](){ Gamepad.zAxis(param); return 1; }},
    {'X', "8-bit X Axis (param)", [](){ Gamepad.rxAxis(param); return 1; }},
    {'Y', "8-bit Y Axis (param)", [](){ Gamepad.ryAxis(param); return 1; }},
    {' ', "Release all buttons", [](){  Gamepad.releaseAll(); return 1; }}
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
                Gamepad.write();
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
