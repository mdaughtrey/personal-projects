#include <WProgram.h>

enum
{
    STOP,
    FORWARD,
    REVERSE,
    NEXT,
    PREVIOUS
};
#define false 0
#define true 1

int camera = 12;
int motorFwd = 11;
int motorReverse = 10;
int lamp = 9;
int tagLed = 8;
int sensor = 6;
int direction = STOP;
int advanceCount = 0;
int irThreshold = 50;
int irData = 0;
bool tagMode = false;
bool tagLedState = false;

void setup ()
{
    direction = STOP;
    advanceCount = 0;
//    pinMode(led, OUTPUT);
    pinMode(lamp, OUTPUT);
    pinMode(camera, OUTPUT);
    pinMode(motorFwd, OUTPUT);
    pinMode(motorReverse, OUTPUT);
    pinMode(tagLed, OUTPUT);

    digitalWrite(sensor, HIGH);
    pinMode(sensor, INPUT);
    Serial.begin(57600);
    digitalWrite(lamp, HIGH);
    digitalWrite(camera, HIGH);
    digitalWrite(motorFwd, HIGH);
    digitalWrite(motorReverse, HIGH);
    digitalWrite(tagLed, HIGH); 
}

void help()
{
        Serial.println("? - this text");
        Serial.println("a - advance 1 step");
        Serial.println("b - backstep 1 step");
        Serial.println("c - camera");
        Serial.println("f - forward freerunning");
        Serial.println("l - lamp on");
        Serial.println("L - lamp off");
        Serial.println("n - next 3 steps");
        Serial.println("p - previous 3 stops");
        Serial.println("r - reverse freerunning");
        Serial.println("s - motor stop");
        Serial.println("t - tag mode on");
        Serial.println("T - tag mode Off");
}

void loop ()
{
    if (Serial.available())
    {
        switch (Serial.read())
        {
            case '?': // help
                help();
                break;


            case 'l': // lamp on
                digitalWrite(lamp, LOW);
                break;

            case 'L':  // lamp off
                digitalWrite(lamp, HIGH);
                break;

            case 'c': // camera 
                digitalWrite(camera, LOW);
                delay(200);
                digitalWrite(camera, HIGH);
                break;

            case 'f': // forward
                direction = FORWARD;
                break;

            case 'a': // advance
                direction = NEXT;
                advanceCount = 1;
                break;

            case 'b': // backstep
                direction = PREVIOUS;
                advanceCount = 1;
                break;

            case 'n': // next
                direction = NEXT;
                advanceCount = 3;
                break;

            case 'r': // reverse
                direction = REVERSE;
                break;

            case 't': // tag mode on
                tagMode = true;
                advanceCount = 3;
                break;

            case 'T': // tag mode off
                tagMode = false;
                break;

            case 'p': // previous
                direction = PREVIOUS;
                advanceCount = 3;
                break;


            case 's': // stop
                direction = STOP;
                break;

            case 'x': // all stop
                direction = STOP;
                digitalWrite(lamp, HIGH);
                digitalWrite(camera, HIGH);
                digitalWrite(motorFwd, HIGH);
                digitalWrite(motorReverse, HIGH);
                break;
        }
        if (FORWARD == direction || NEXT == direction)
        {
            digitalWrite(motorFwd, LOW);
            digitalWrite(motorReverse, HIGH);
        }
        else if (REVERSE == direction || PREVIOUS == direction)
        {
            digitalWrite(motorFwd, HIGH);
            digitalWrite(motorReverse, LOW);
        }
        else
        {
            digitalWrite(motorFwd, HIGH);
            digitalWrite(motorReverse, HIGH);
        }
    }

    if (digitalRead(sensor))
    {
        if (irThreshold)
        {
            irThreshold--;
        }
    }
    else
    {
        if (irThreshold < 100)
        {
            irThreshold++;
        }
    }
    
    if (irThreshold == 0)
    {
        irData <<= 1;
    }
    else if (irThreshold == 100)
    {
        irData <<= 1;
        irData |= 1;
    }

    if (irData == 0x0f && tagMode == true && FORWARD == direction)
    {
        if (--advanceCount <= 0)
        {
            if (true == tagLedState)
            {
                digitalWrite(tagLed, LOW);
            }
            else
            {
                digitalWrite(tagLed, HIGH);
            }
            tagLedState = !tagLedState;
            advanceCount = 3;
        }
        return;
    } 

    if (irData == 0x0f && (NEXT == direction || PREVIOUS == direction))
    {
        if (--advanceCount <= 0)
        {
            direction = STOP;
            // motor off
            digitalWrite(motorFwd, HIGH);
            digitalWrite(motorReverse, HIGH);
        }
    }
}
