#include <WProgram.h>

enum
{
    STOP,
    FORWARD,
    REVERSE,
    NEXT,
    PREVIOUS
};

int lamp = 13;
int camera = 12;
int motorFwd = 11;
int motorReverse = 10;
int direction = STOP;
int advanceCount = 0;

void setup ()
{
    direction = STOP;
    advanceCount = 0;
    pinMode(lamp, OUTPUT);
    pinMode(camera, OUTPUT);
    pinMode(motorFwd, OUTPUT);
    pinMode(motorReverse, OUTPUT);
    Serial.begin(57600);
    digitalWrite(lamp, HIGH);
    digitalWrite(camera, HIGH);
    digitalWrite(motorFwd, HIGH);
    digitalWrite(motorReverse, HIGH);
}

void loop ()
{
    if (Serial.available())
    {
        switch (Serial.read())
        {
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

            case 'n': // next
                direction = FORWARD;
                advanceCount = 3;
                break;

            case 'r': // reverse
                direction = REVERSE;
                break;

            case 'p': // previous
                direction = REVERSE;
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
        else if (REVERSE == direction || PREV == direction)
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
}
