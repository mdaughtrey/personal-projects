#include <WProgram.h>
//#include <ps2/ps2mouse.h>
#include <avrlibtypes.h>

#pragma GCC diagnostic ignored "-Wwrite-strings"

//extern volatile s16 xPos;
//extern volatile s16 yPos;
//extern u08 packet0;
//extern u08 packet1;
//extern u08 packet2;

#define PIN_SERVO 11 
#define PIN_LAMP 9 
#define PIN_MOTOR 10
#define MOTOR
#define PIN_LASER 8
#define PIN_LEDSENSOR 2


//#ifde SDEBUG
//void showByteLog(void);
//void resetByteLog(void);
//#endif // SDEBUG
//void debugReport(void);
//
//PS2Mouse mouse;

unsigned long mouseTimer; // = 0;
u16 servoTimer = 0;
u16 servoPulse = 1500;
u16 motorPulse = 255;

void lampOn()
{
    digitalWrite(PIN_LAMP, LOW);
}

void lampOff()
{
    digitalWrite(PIN_LAMP, HIGH);
}

//void motorOn()
//{
//    digitalWrite(PIN_MOTOR, LOW);
//}
//
//void motorOff()
//{
//    digitalWrite(PIN_MOTOR, HIGH);
//}

void servoOn()
{
    //analogWrite(PIN_SERVO, 90);
    digitalWrite(PIN_SERVO,HIGH);
}

void servoOff()
{
    //analogWrite(PIN_SERVO, 0);
    digitalWrite(PIN_SERVO,LOW);
}

void laserOn()
{
		digitalWrite(PIN_LASER, HIGH);
}

void laserOff()
{
		digitalWrite(PIN_LASER, LOW);
}

void setup ()
{ 
    // todo init lamp
    pinMode(PIN_LAMP, OUTPUT);
	pinMode(PIN_LASER, OUTPUT);
    //pinMode(PIN_MOTOR, OUTPUT);
    //digitalWrite(PIN_SERVO, LOW);
    pinMode(PIN_SERVO, OUTPUT);
    //digitalWrite(PIN_MOTOR, HIGH);
    digitalWrite(PIN_LAMP, HIGH);
#ifdef MOTOR
    analogWrite(PIN_MOTOR, motorPulse);
#endif //MOTOR
    //digitalWrite(PIN_SERVO, LOW);
    Serial.begin(57600);
    mouseTimer = millis();
    servoTimer = millis();
    Serial.println("Init OK");
}

unsigned short  ledValue = 0;
void loop ()
{
    if ((millis() - servoTimer) > 100)
    {
        servoOn();
        delayMicroseconds(servoPulse);
        servoOff();
        servoTimer = millis();
		unsigned short newValue = analogRead(PIN_LEDSENSOR);
		if (newValue != ledValue)
		{
		Serial.println(newValue, 10);
		ledValue = newValue;
		}
    }
    if (millis() - mouseTimer > 100)
    {
//            debugReport();
            mouseTimer = millis();
    }


    if (!Serial.available())
    {
        return;
    }

    switch (Serial.read())
    {

        case '0':
            //xPos = 0;
            //yPos = 0;
            break;
             
        case 'x':
            servoPulse = 1500;
            lampOff();
			laserOff();
            motorPulse = 255;
            analogWrite(PIN_MOTOR, motorPulse);
            break;

        case 'l':
            lampOn();
            break;

        case 'L':
            lampOff();
            break;

		case 'e':
			laserOn();
			break;

		case 'E':
			laserOff();
			break;

        case 's':
            if (servoPulse > 1000)
            {
                servoPulse -= 10;
            }
            Serial.print("Servo ");
            Serial.println(servoPulse, 10);
            break;

        case 'S':
            if (servoPulse < 2000)
            {
                servoPulse += 10;
            }
            Serial.print("Servo ");
            Serial.println(servoPulse, 10);
            break;

        case 'M':
            if (motorPulse < 255)
            {
                motorPulse += 5;
            }
#ifdef MOTOR
            Serial.print("Motor ");
            Serial.println(motorPulse, 10);
            analogWrite(PIN_MOTOR, motorPulse);
#endif
            break;

        case 'm':
            if (motorPulse > 0)
            {
                motorPulse -= 5;
            }
#ifdef MOTOR
            Serial.print("Motor ");
            Serial.println(motorPulse,10);
            analogWrite(PIN_MOTOR, motorPulse);
#endif
            break;
    }
    return;



//    if (millis() - timer > 1000)
//    {
//            debugReport();
//            timer = millis();
//    }
//    if (Serial.available())
//    {
//        switch (Serial.read())
//        {
//         case '0':
//             xPos = 0;
//             yPos = 0;
//#ifdef SDEBUG
//            resetByteLog();
//#endif // SDEBUG
//             break;
//
//         case 'x': // reset
//             Serial.print("Init");
//             mouse.initialize();
//             Serial.println('.');
//             break;
//
//         case 's':
//#ifdef SDEBUG
//             showByteLog();
//#endif // SDEBUG
//             break;
//
//         case 'r': // raw report
//            debugReport();
//            break;
//        }
//    }
}
