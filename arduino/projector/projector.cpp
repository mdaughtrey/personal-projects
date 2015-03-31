#include <WProgram.h>
#pragma GCC diagnostic ignored "-Wwrite-strings"
enum
{
    STOP,
    FORWARD,
    REVERSE,
    NEXT,
    PREVIOUS,
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
int fan = 8;
int sensor = 6;
int direction = STOP;
int advanceCount = 0;
int irThreshold = 50;
unsigned char irData = 0;
int cycleState = CYCLE_IDLE;
int motorSpeed = 0;
unsigned long brakeStartTime;
int maxMotorSpeed = 0;

const int SLOW_SPEED=100;
const int SLOW_SPEED_INC=20;

const int FAST_SPEED=255;
const int FAST_SPEED_INC=1;

const int motorTable[] = { 20, 40, 60, 80, 100, 120, 140, 160 };
const int MOTOR_TABLE_SIZE = sizeof(motorTable)/sizeof(motorTable[0]);
int motorTableIndex = 0;

void lampOn()
{
    digitalWrite(lamp, HIGH);
}

void lampOff()
{
    digitalWrite(lamp, LOW);
}

void fastBack()
{
    if (motorSpeed && (motorSpeed < FAST_SPEED))
    {
        motorSpeed += FAST_SPEED_INC;
    }
    analogWrite(motorFwd, motorSpeed);
    analogWrite(motorReverse, 0);
}

void slowBack()
{
    if (motorSpeed && (motorSpeed < SLOW_SPEED))
    {
        motorSpeed += SLOW_SPEED_INC;
    }
    analogWrite(motorFwd, motorSpeed);
    analogWrite(motorReverse, 0);
}

void slowForward()
{
	if (motorTableIndex < MOTOR_TABLE_SIZE)	
	{
		motorSpeed = motorTable[motorTableIndex++];
	}
//    if (motorSpeed && (motorSpeed < SLOW_SPEED))
//    {
//        motorSpeed += SLOW_SPEED_INC;
//    }
    analogWrite(motorReverse, motorSpeed);
    analogWrite(motorFwd, 0);
}

void fastForward()
{
    if (motorSpeed && (motorSpeed < FAST_SPEED))
    {
        motorSpeed += FAST_SPEED_INC;
    }
    analogWrite(motorReverse, motorSpeed);
    analogWrite(motorFwd, 0);
}

void motorStop()
{
    motorSpeed = 0;
    motorTableIndex = 0;
    analogWrite(motorFwd, 0);
    analogWrite(motorReverse, 0);
}

void ReverseBrake()
{
    analogWrite(motorFwd, 0);
    analogWrite(motorReverse, motorSpeed);
    if (millis() - brakeStartTime > 300)
    {
        motorStop();
    }
}

void ForwardBrake()
{
    analogWrite(motorReverse, motorSpeed);
    analogWrite(motorReverse, 0);
    if (millis() - brakeStartTime > 300)
    {
        motorStop();
    }
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
    pinMode(fan, OUTPUT);

    digitalWrite(sensor, HIGH);
    pinMode(sensor, INPUT);
    Serial.begin(57600);
    lampOff();
    digitalWrite(camera, HIGH);
    digitalWrite(motorFwd, HIGH);
    digitalWrite(motorReverse, HIGH);
    digitalWrite(fan, HIGH); 
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
        Serial.println("y - fan on");
        Serial.println("Y - fan Off");
}

void loop ()
{
    switch (cycleState)
    {
        case CYCLE_IDLE:
            break;

        case CYCLE_LAMPON:
            lampOn();
            //delay(10);
            delay(20);
            lampOff();
            delay(60);
            digitalWrite(camera, HIGH);

        //    delay(2000);
        //    digitalWrite(camera, LOW);
        //    delay(130);
        //    lampOn();
        //    delay(10);
        //    lampOff();
        //    delay(60);
        //    digitalWrite(camera, HIGH);


            cycleState = CYCLE_NEXT;
            direction = NEXT;
            advanceCount = 3;
            break;

        case CYCLE_SHUTTER:
            digitalWrite(camera, LOW);
            delay(130);
            cycleState = CYCLE_LAMPON;
            break;

//        case CYCLE_LAMPOFF:
//            lampOff();
//            cycleState = CYCLE_NEXT;
//            direction = NEXT;
//            advanceCount = 3;
//            break;

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

            case '3': // triple
                digitalWrite(camera, LOW);
                delay(130);
                lampOn();
                delay(2);
                lampOff();
                delay(199);
                delay(130);
                lampOn();
                delay(20);
                lampOff();
                delay(180);
                delay(130);
                lampOn();
                delay(50);
                lampOff();
                delay(150);
                digitalWrite(camera, HIGH);
                direction = NEXT;
                advanceCount = 3;
                break;


            case 'l': // lamp on
                lampOn();
                break;

            case 'L':  // lamp off
                lampOff();
                break;

            case 'm': // lamp on
                digitalWrite(camera, LOW);
                delay(130);
                lampOn();
                delay(20);
                lampOff();
                delay(60);
                digitalWrite(camera, HIGH);
                break;

            case 'c': // camera 
                shutter();
                break;

            case 'C': // cycle 
                //cycleState = CYCLE_LAMPON;
                cycleState = CYCLE_SHUTTER;
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

            case 'y': // fan on
                digitalWrite(fan, LOW); 
                break;

            case 'Y': // fan off
                digitalWrite(fan, HIGH); 
                break;

            case 'p': // previous
                direction = PREVIOUS;
                advanceCount = 3;
                break;

            case 's': // stop
                direction = STOP;
                motorStop();
                break;

            case 'x': // all stop
                digitalWrite(fan, HIGH); 
                direction = STOP;
                lampOff();
                motorStop();
                digitalWrite(camera, HIGH);
                break;
        }
    }
    if (FORWARD == direction || NEXT == direction)
    {
        if (0 == motorSpeed)
        {
            motorSpeed = 1;
        }
        if (FORWARD == direction)
        {
            fastForward();
        }
        else
        {
            slowForward();
        }
    }
    else if (REVERSE == direction || PREVIOUS == direction)
    {
        if (0 == motorSpeed)
        {
            motorSpeed = 1;
        }
        if (REVERSE == direction)
        {
            fastBack();
        }
        else
        {
            slowBack();
        }
    }
    else
    {
        motorStop();
    }
//    else if (FORWARD_BRAKE == direction)
//    {
//        ForwardBrake();
//        if (0 == motorSpeed)
//        {
//            direction = STOP;
//            if (CYCLE_NEXT == cycleState)
//            {
//                cycleState = CYCLE_IDLE;
//            }
//        }
//    }
//    else if (REVERSE_BRAKE == direction)
//    {
//        ReverseBrake();
//        if (0 == motorSpeed)
//        {
//            direction = STOP;
//            if (CYCLE_NEXT == cycleState)
//            {
//                cycleState = CYCLE_IDLE;
//            }
//        }
//    }
//    else
//    {
//        motorStop();
//    }

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

    //if (irData == 0x0f && tagMode == true && FORWARD == direction)
//    if (irData == 0xf0 && tagMode == true && FORWARD == direction)
//    {
//        if (--advanceCount <= 0)
//        {
//            if (true == tagLedState)
//            {
//                digitalWrite(tagLed, LOW);
//            }
//            else
//            {
//                digitalWrite(tagLed, HIGH);
//            }
//            tagLedState = !tagLedState;
//            advanceCount = 3;
//        }
//        return;
//    } 
//    Serial.println(irData, HEX);

    if (irData == 0x0f && (NEXT == direction || PREVIOUS == direction))
//    if (irData == 0xf0 && (NEXT == direction || PREVIOUS == direction))
    {
        if (--advanceCount <= 0)
        {
            motorStop();
            direction = STOP;
        if (CYCLE_NEXT == cycleState)
        {
            cycleState = CYCLE_IDLE;
        }
//            if (NEXT == direction)
//            {
//                direction = FORWARD_BRAKE;
//                brakeStartTime = millis();
//                motorSpeed = 128;
//            }
//            else if (PREVIOUS == direction)
//            {
//                direction = REVERSE_BRAKE;
//                brakeStartTime = millis();
//                motorSpeed = 128;
//            }
        }
    }
}
