#include <WProgram.h>
#pragma GCC diagnostic ignored "-Wwrite-strings"
enum
{
    STOP,
    FORWARD,
    REVERSE,
    NEXT,
    PREVIOUS
};

enum
{
    CYCLE_IDLE,
    CYCLE_LAMPON,
    CYCLE_SHUTTER,
    CYCLE_LAMPOFF,
    CYCLE_NEXT
};

int camera = 12;
int motorFwd = 10;
int motorReverse = 11;
int lamp = 9;
int tagLed = 8;
int sensor = 6;
int direction = STOP;
int advanceCount = 0;
int irThreshold = 50;
int irData = 0;
bool tagMode = false;
bool tagLedState = false;
int cycleState = CYCLE_IDLE;

void lampOn()
{
    digitalWrite(lamp, LOW);
}

void lampOff()
{
    digitalWrite(lamp, HIGH);
}

void motorForward()
{
    digitalWrite(motorFwd, LOW);
    digitalWrite(motorReverse, HIGH);
}

void motorBack()
{
    digitalWrite(motorFwd, HIGH);
    digitalWrite(motorReverse, LOW);
}

void motorStop()
{
    digitalWrite(motorFwd, HIGH);
    digitalWrite(motorReverse, HIGH);
}

void shutter()
{
    digitalWrite(camera, LOW);
    delay(200);
    digitalWrite(camera, HIGH);
}

void setup ()
{
    direction = STOP;
    advanceCount = 0;
    pinMode(lamp, OUTPUT);
    pinMode(camera, OUTPUT);
    pinMode(motorFwd, OUTPUT);
    pinMode(motorReverse, OUTPUT);
    pinMode(tagLed, OUTPUT);

    digitalWrite(sensor, HIGH);
    pinMode(sensor, INPUT);
    Serial.begin(57600);
    lampOff();
    digitalWrite(camera, HIGH);
    digitalWrite(motorFwd, HIGH);
    digitalWrite(motorReverse, HIGH);
    digitalWrite(tagLed, HIGH); 
    motorStop();
}

void help()
{
        Serial.println("? - this text");
        Serial.println("a - advance 1 step");
        Serial.println("b - backstep 1 step");
        Serial.println("c - camera");
        Serial.println("C - cycle");
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
    switch (cycleState)
    {
        case CYCLE_IDLE:
            break;

        case CYCLE_LAMPON:
            lampOn();
            delay(200);
            cycleState = CYCLE_SHUTTER;
            break;

        case CYCLE_SHUTTER:
            shutter();
            delay(500);
            cycleState = CYCLE_LAMPOFF;
            break;

        case CYCLE_LAMPOFF:
            lampOff();
            cycleState = CYCLE_NEXT;
            direction = NEXT;
            advanceCount = 3;
            break;

        case CYCLE_NEXT:
            break;

    }
    if (Serial.available())
    {
        if (CYCLE_IDLE != cycleState)
        {
            cycleState = CYCLE_IDLE;
            return;
        }
        switch (Serial.read())
        {
            case '?': // help
                help();
                break;

            case 'l': // lamp on
                lampOn();
                break;

            case 'L':  // lamp off
                lampOff();
                break;

            case 'c': // camera 
                shutter();
                break;

            case 'C': // cycle 
                cycleState = CYCLE_LAMPON;
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
                digitalWrite(tagLed, HIGH);
                break;

            case 'x': // all stop
                digitalWrite(tagLed, HIGH);
                direction = STOP;
                lampOff();
                motorStop();
                digitalWrite(camera, HIGH);
                break;
        }
    }
    if (FORWARD == direction || NEXT == direction)
    {
        motorForward();
    }
    else if (REVERSE == direction || PREVIOUS == direction)
    {
        motorBack();
    }
    else
    {
        motorStop();
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
            motorStop();
            if (CYCLE_NEXT == cycleState)
            {
                cycleState = CYCLE_IDLE;
            }
        }
    }
}
