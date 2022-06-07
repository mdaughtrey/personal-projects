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
//#include <avr/wdt.h>

typedef struct
{
    char key;
    char help[32];
    bool (*fun)();
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

const uint8_t Encoder0Pin0 = 0;
const uint8_t Encoder0Pin1 = 1;
const uint8_t Pedal0Pin = 0;
const uint8_t Pedal1Pin = 1;

volatile uint8_t isr0Count = 0;
volatile uint8_t isr1Count = 0;
uint8_t steeringInc = 24;

volatile uint16_t encoder0Val = 0;
uint16_t encoder0Min = 0;
uint16_t encoder0Max = 0xffff;
int16_t encoder0Scaled = 0;
int16_t encoder0ScaledPrev = 0;
uint16_t xAxis = 0;
uint16_t yAxis = 0;
uint8_t zAxis = 0;

uint16_t rxAxis = 0;
uint16_t ryAxis = 0;
uint8_t rzAxis = 0;

uint8_t dpad2 = 0;
uint8_t dpad1 = 0;

//uint8_t debug = 0;
uint8_t doWrite = 1;
uint16_t event = 0;
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
//    if (!debug)
//    {
//        return;
//    }
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, 255, fmt, args);
    va_end(args);
    Serial.print(buffer);
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
    isr0Count++;
}

void isrEncoder01(void)
{
    if (digitalRead(Encoder0Pin0))
    {
        if (encoder0Val < 65535)
        {
            encoder0Val++; //  steeringInc;
//            Gamepad.xAxis(encoder0Val);
//            writeReady = 1;
        }
    }
    isr1Count++;
}

void setup() {
    Serial.begin(115200);
//    Serial.println("{State:Ready}");
//    wdt_enable(WDTO_2S);
    Gamepad.begin();

    pinMode(Encoder0Pin0, INPUT_PULLUP);
    pinMode(Encoder0Pin1, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(Encoder0Pin0), isrEncoder00, FALLING);
    attachInterrupt(digitalPinToInterrupt(Encoder0Pin1), isrEncoder01, FALLING);
//    for (uint8_t ii = 0; ii < 17; ii++)
//    {
//        pinMode(ii, INPUT_PULLUP);
//    }
}

int16_t getEncoder0Scaled()
{
    return (encoder0Val - 1000) * 32;
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
        return;
    }
    writeReady = 1;
    Gamepad.xAxis(xAxis);
    verbose("xAxisWipe %d\r", xAxis);
    xAxis += 1000;
    at.setTimeout(xAxisWipe, 100);
}

void yAxisWipe()
{
    if (yAxis > 65000)
    {
        verbose("yAxisWipe stop");
        return;
    }
    writeReady = 1;
    Gamepad.yAxis(yAxis);
    verbose("yAxisWipe %d\r", yAxis);
    yAxis += 1000;
    at.setTimeout(yAxisWipe, 100);
}

void zAxisWipe()
{
    if (zAxis > 240)
    {
        verbose("zAxisWipe stop");
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
    if (rxAxis > 65000)
    {
        verbose("rxAxisWipe stop");
        return;
    }
    writeReady = 1;
    Gamepad.rxAxis(rxAxis);
    verbose("rxAxisWipe %u\r", rxAxis);
    rxAxis += 1000;
    at.setTimeout(rxAxisWipe, 100);
}

void ryAxisWipe()
{
    if (ryAxis > 65000)
    {
        verbose("ryAxisWipe stop");
        return;
    }
    writeReady = 1;
    Gamepad.ryAxis(ryAxis);
    verbose("ryAxisWipe %u\r", ryAxis);
    ryAxis += 1000;
    at.setTimeout(ryAxisWipe, 100);
}

void rzAxisWipe()
{
    if (rzAxis > 240)
    {
        verbose("rzAxisWipe stop");
        return;
    }
    writeReady = 1;
    Gamepad.rzAxis(rzAxis);
    verbose("rzAxisWipe %u\r", rzAxis);
    rzAxis += 10;
    at.setTimeout(rzAxisWipe, 100);
}

void dpad2Wipe()
{
    if (dpad2 > GAMEPAD_DPAD_UP_LEFT)
    {
        verbose("dpad2Wipe stop");
        return;
    }
    writeReady = 1;
    Gamepad.dPad2(dpad2);
    verbose("dpad2 %u\r", dpad2);
    dpad2++;
    at.setTimeout(dpad2Wipe, 1000);
}

void dpad1Wipe()
{
    if (dpad1 > GAMEPAD_DPAD_UP_LEFT)
    {
        verbose("dpad1Wipe stop");
        return;
    }
    writeReady = 1;
    Gamepad.dPad1(dpad1);
    verbose("dpad1 %u\r", dpad1);
    dpad1++;
    at.setTimeout(dpad1Wipe, 1000);
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
//    debug = 0;
    doWrite = 1;
    event = 0;
    encoder0Val = 32767;
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

Command cmds[] = {
    {'b', "Toggle button (param)", commandB},
    {'B', "Button Chaser", []() { setEvent(EV_BUTTONCHASER); return 0;}},
    {'c', "Clear param", [](){ param = 0; return 0; }},
//    {'d', "Debug on", []() { debug = 1; return 0;}},
//    {'D', "Debug off", []() { debug = 0; return 0;}},
    {'e', "X Axis Wipe", []() { setEvent(EV_XAXIS); return 0; }},
    {'f', "Y Axis Wipe", []() { setEvent(EV_YAXIS); return 0; }},
    {'g', "Z Axis Wipe", []() { setEvent(EV_ZAXIS); return 0; }},
    {'h', "Help", [](){ help(); return 0; }},
    {'i', "Steering Inc", []() { steeringInc = param; return 0; }},
    {'I', "RX Axis Wipe", []() { setEvent(EV_RXAXIS); return 0; }},
    {'j', "RY Axis Wipe", []() { setEvent(EV_RYAXIS); return 0; }},
    {'k', "RZ Axis Wipe", []() { setEvent(EV_RZAXIS); return 0; }},
    {'s', "Set leftmost steer", []{ encoder0Val = 0; return 0; }},
    {'S', "Set righttmost steer", []{ encoder0Max = encoder0Val; }},
    {'p', "DPad1 wipe", []() { setEvent(EV_DPAD1); return 0; }},
    {'P', "DPad2 wipe", []() { setEvent(EV_DPAD2); return 0; }},
    {'r', "Reset", []() { while (1); }},
//    {'w', "USB Write on", []() { doWrite = 1; return 0; }},
//    {'t', "Test", []() { test(); return 0; }},
    {'w', "USB Write off", []() { doWrite = 0; return 0; }},
//    {'x', "16-bit X Axis (param)", [](){ Gamepad.xAxis(param); return 1; }},
//    {'y', "16-bit Y Axis (param)", [](){ Gamepad.yAxis(param); return 1; }},
//    {'z', "8-bit lZ Axis (param)", [](){ Gamepad.zAxis(param); return 1; }},
//    {'X', "8-bit X Axis (param)", [](){ Gamepad.rxAxis(param); return 1; }},
//    {'Y', "8-bit Y Axis (param)", [](){ Gamepad.ryAxis(param); return 1; }},
    {'z', "Set center steering position", [](){ encoder0Val = 32767; }} ,
    {' ', "Soft Reset", [](){ doReset(); return 0; }}
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
    verbose("encoder0Val %u\r\n", encoder0Val);
    verbose("encoder0Scaled %d\r\n", getEncoder0Scaled());
    verbose("isr0Count %u\r\n", isr0Count);
    verbose("isr1Count %u\r\n", isr1Count);
    verbose("Encoder0Pin0 %u\r\n", digitalRead(Encoder0Pin0));
    verbose("Encoder0Pin1 %u\r\n", digitalRead(Encoder0Pin1));
    verbose("Pedal0Val %u\r\n", analogRead(Pedal0Pin));
    verbose("Pedal1Val %u\r\n", analogRead(Pedal1Pin));
//    verbose("Pins");
//    for (uint8_t ii = 0; ii < 17; ii++)
//    {
//        verbose("%u", digitalRead(ii));
//    }
//    verbose("\r\n");
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
    int16_t tmp;
    //wdt_reset();
    if (Serial.available())
    {
        handleCommand();
    }
    if ((tmp = getEncoder0Scaled()) != encoder0ScaledPrev)
    {
        encoder0ScaledPrev = tmp;
        Gamepad.xAxis(tmp);
        writeReady = 1;
    }
    Gamepad.zAxis((analogRead(Pedal0Pin)-128) >> 2);
    Gamepad.rzAxis((analogRead(Pedal1Pin)-128) >> 2);
    writeReady = 1;
    doWrite = 1;
//    if (writeReady)
//    {
//        if (doWrite)
//        {
            Gamepad.write();
//        }
//        writeReady = 0;
//    }
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
    if (isEvent(EV_DPAD2))
    {
        dpad2 = GAMEPAD_DPAD_UP;
        resetEvent(EV_DPAD2);
        verbose("dpad2Wipe start\r\n");
        dpad2Wipe();
    }
    if (isEvent(EV_DPAD1))
    {
        dpad1 = GAMEPAD_DPAD_UP;
        resetEvent(EV_DPAD1);
        verbose("dpad1Wipe start\r\n");
        dpad1Wipe();
    }
//    verbose("%u:%u\r\n", digitalRead(Encoder0Pin0), digitalRead(Encoder0Pin1));
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
