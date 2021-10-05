#include <stdint.h>
//#include <iostream>
//#include <string>
//#include <sstream>
#include "stepper.h"
const char help[] = "Help:\n" \
    "Some Help\r";

struct {
    uint8_t stepDelay;
    uint8_t param;
    uint8_t stopAt;
    uint8_t tension;
    uint8_t verbose;
} config;

uint8_t lastCommand;
uint8_t ledState;
int ledTime = 0;

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

void fan(uint8_t val) { digitalWrite(OutputPin0, val); }
void lamp(uint8_t val) { digitalWrite(OutputPin1, val); }
void rewindMotor(uint8_t val) { analogWrite(OutputPin3, val); }

int encoderPos = 0;
int isr1count = 0;
int isr2count = 0;

void isr1()
{
    isr1count++;
    if (digitalRead(EncoderPin1))
    {
        encoderPos++;
    }

    if (config.stopAt && (isr1count > config.stopAt))
    {
//        detachInterrupt(digitalPinToInterrupt(EncoderPin0));
        stepper::stop();
    }
}

void isr2()
{
    isr2count++;
    if (digitalRead(EncoderPin0))
    {
        encoderPos--;
    }
//    if (config.stopAt && (isr1count > config.stopAt))
//    {
//        detachInterrupt(digitalPinToInterrupt(EncoderPin0));
//        stepper::stop();
//        config.stopAt = 0;
//        isr1count %= config.stopAt;
//    }
}


void encoder_init()
{
    pinMode(EncoderPin0, INPUT_PULLUP);
    pinMode(EncoderPin1, INPUT_PULLUP);

    isr1count = 0;
    isr2count = 0;
    encoderPos = 0;
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
    Serial.println("setup");
    config.stepDelay = 2;
    stepper::init();
    buttonInit();
    encoder_init();
    outputInit();
    ledState = 0;
    pinMode(LedPin, OUTPUT);
    Serial.println("{State:Ready}");
}

void buttonPoll()
{

    if (digitalRead(ButtonPin0) != buttonState[0])
    {
        Serial.print("Button 0: "); // Reset
        buttonState[0] = !buttonState[0];
        Serial.println(buttonState[0]);
    }
    if (digitalRead(ButtonPin1) != buttonState[1]) // Rewind
    {
        Serial.print("Button 1: ");
        buttonState[1] = !buttonState[1];
        Serial.println(buttonState[1]);
    }
    if (digitalRead(ButtonPin2) != buttonState[2]) // 
    {
        Serial.print("Button 2: ");
        buttonState[2] = !buttonState[2];
        Serial.println(buttonState[2]);
    }
    if (digitalRead(ButtonPin3) != buttonState[3]) // 
    {
        Serial.print("Button 3: ");
        buttonState[3] = !buttonState[3];
        Serial.println(buttonState[3]);
    }
}

void dumpConfig()
{
    Serial.print("stepDelay: "); Serial.println(config.stepDelay);
    Serial.print("stopAt: "); Serial.println(config.stopAt);
    Serial.print("param: "); Serial.println(config.param);
    Serial.print("isr1count: "); Serial.println(isr1count);
    Serial.print("isr2count: "); Serial.println(isr2count);
    Serial.print("encoderPos: "); Serial.println(encoderPos);
    Serial.print("tension: "); Serial.println(config.tension);
}

void handleCommand()
{
    lastCommand = Serial.read();
    switch (lastCommand)
    {
        case 'v': config.verbose = 1; break;
        case 'V': config.verbose = 0; break;

        case '?': dumpConfig(); break;
        case '-': config.param = 0;; break;                     // reset input param

        case ']': config.stopAt = 1;
            attachInterrupt(digitalPinToInterrupt(EncoderPin0), isr1, FALLING);
            stepper::cw(); break;      // Jog forward

        case '[': config.stopAt = 1;
            attachInterrupt(digitalPinToInterrupt(EncoderPin0), isr1, FALLING);
            stepper::ccw(); break;     // Job back

//        case 'f': config.stopAt = 0; stepper::cw(); break;      // Continuous forward
//        case 'b': config.stopAt = 0; stepper::ccw(); break;     // Continuous back

        case 'n': //config.stopAt = 27;
            isr1count %= config.stopAt;
            attachInterrupt(digitalPinToInterrupt(EncoderPin0), isr1, FALLING);
            stepper::ccw(); break;     // Next

        case 'S':                                               // Stepper delay
            config.stepDelay = config.param;
            config.param = 0;
            break; 

        case 'A':                                               // stop At
            config.stopAt = config.param; 
            isr1count = 0;
            break;

        case 'm': // mark
            encoderPos = 0;
            break;

        case ' ':                                               // Reset
            detachInterrupt(digitalPinToInterrupt(EncoderPin0));
            stepper::stop();
            lamp(0);
            fan(0);
            rewindMotor(0);
            isr1count = 0;
            config.stopAt = 0;
            break;

        case 't':                                               // rewind tension
            config.tension = config.param;
            rewindMotor(config.param); 
            fan(1); 
            break;

        case 'T': rewindMotor(0); fan(0); break;                // tension off

        case 'l': lamp(1); break;                               // Lamp on
        case 'L': lamp(0); break;                               // Lamp off

        case 'c': fan(1); break;                                // Cooling fan on
        case 'L': fan(0); break;                                // Cooling fan off
        case 'z': zeroState = HUNT; break;
 
        default:                                                // Load parameters
            if (lastCommand >= '0' & lastCommand <= '9')
            {
                config.param *= 10;
                config.param += lastCommand - '0';
            }
            break;
    }
}

void zeroPoll()
{
    if (HUNT != zeroState) return;
    if (encoderPos > 1) stepper::ccw();
    else if (encoderPos < -1) stepper::cw();
    else zeroState = NONE;
    break;
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
    if (Serial.available())
    {
        handleCommand();
    }
    stepper::poll(config.stepDelay);
    buttonPoll();
    ledPoll();
    zeroPoll();
}
