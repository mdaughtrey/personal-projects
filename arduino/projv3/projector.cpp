#include <WProgram.h>
#include <avrlibtypes.h>

#pragma GCC diagnostic ignored "-Wwrite-strings"

//#define STOPONGAP
//#define SENSORINT

#define PB_SERVO 3
#define PB_LAMP 1
#define PIN_MOTOR 10  // PB2
#define PB_LASER 0
#define PIN_LEDSENSOR 2 // PC2
#define PC_LEDSENSOR 2 // PC2
#define PB_SHUTTER 4

#define MOTOR_PRETENSION_NEXT 80
#define MOTOR_PRETENSION_FF 220
#define MOTOR_REWIND 160 
#define MOTOR_OFF 255
#define SERVO_MIN 14
#define SERVO_STOP 22
#define SERVO_MAX 30
#define SERVO_NEXTFRAME_FAST 28 
#define SERVO_NEXTFRAME_SLOW 26 
#define SERVO_NEXTFRAME_REVERSE 19 

#define SENSOR_STEP 10 
// must be an even number/SENSOR_STEP
#define SENSOR_VALUE_MAX 255 - SENSOR_STEP
#define SENSOR_VALUE_MIN SENSOR_STEP
#define SENSOR_VALUE_INIT 125
#define SENSOR_VALUE_INIT_MIN SENSOR_VALUE_INIT
#define SENSOR_VALUE_INIT_MAX SENSOR_VALUE_INIT
#define LASER_TIMEOUT_MS 10000
#define LAMP_TIMEOUT_MS 3000
#define NEXT_FRAME_TIMEOUT_MS 8000
#define SERVO_TIMEOUT_MS 2000

#define LAMP_OFF PORTB |= _BV(PB_LAMP)
#define LAMP_ON PORTB &= ~_BV(PB_LAMP)
#define SERVO_ON PORTB |= _BV(PB_SERVO)
#define SERVO_OFF PORTB &= ~_BV(PB_SERVO)
#define LASER_ON PORTB |= _BV(PB_LASER);
#define LASER_OFF PORTB &= ~_BV(PB_LASER);
#define SHUTTER_OPEN PORTB &= ~_BV(PB_SHUTTER)
#define SHUTTER_CLOSE PORTB |= _BV(PB_SHUTTER)

#define DELAYEDSTATE(dd, ss) { stateLoop = dd/10; stateSaved = ss; waitingFor = DELAYLOOP; }
#ifdef SENSORINT
#define SENSORINT_ON { PCICR |= _BV(PCIE1); PCMSK1 |= _BV(PCINT10);}
#define SENSORINT_OFF { PCICR &= ~_BV(PCIE1); PCMSK1 &= ~_BV(PCINT10);}
#else // SENSORINT
#define SENSORINT_ON
#define SENSORINT_OFF
#endif // SENSORINT

typedef enum
{ 
        NONE = 0,
        LASERBLINKON,
        LASERBLINKOFF,
        FRAMESTOP,
        LOOKFORGAPEND,
        LOOKFORGAPSTART,
        LOOKFORFRAMEEND,
        DELAYLOOP,
        SENSORENABLE,
        SENSORSTART,
        SHUTTERCLOSED,
        EXPOSURESERIES5,
        EXPOSURESERIES4,
        EXPOSURESERIES3,
        EXPOSURESERIES2,
        EXPOSURESERIES1,
        EXPOSURESERIES,
        SHUTTEROPEN,
        TRIPLESTART = SHUTTEROPEN
} WaitFor;

//u08 sensorActive = 0;
u16 parameter = 0;
u08 servoPulse = SERVO_STOP;
u08 motorPulse = 255;
u08 motorPretensionNext = MOTOR_PRETENSION_NEXT;
u32 laserTimeout = 0;
u32 lampTimeout = 0;
u32 tsServoStart = 0;
u32 nextFrameTimeout = 0;
u08 sensorValue(0);
u08 waitingFor(NONE);
//u16 ledCount;
u08 stateLoop;
u08 stateSaved;

void incMotor();
void decMotor();
void setMotor(u08 set);
void incServo();
void decServo();
void setServo(u08 set);
void lampOn();
void lampOff();
void lampCheck();
u08 checkNextFrameTimeout();
void laserOn();
void laserOff();
void laserCheck();
void reset();

u08 highCount;

ISR(TIMER2_OVF_vect)
{
    SERVO_ON;
    //if (LOOKFORGAPEND != waitingFor)
    //{
    //    return;
    //}
    //if (0 == (PINC & _BV(PC_LEDSENSOR)))
    //{
    //    return;
    //}
    //--highCount;
    //if (highCount > 0)
    //{
    //    return;
    //}
    //waitingFor = SHUTTEROPEN;
    //setServo(SERVO_STOP);
    //laserOff();
}

ISR(TIMER2_COMPA_vect)
{
    SERVO_OFF;
}


#ifdef SENSORINT
u32 tsFallingSlope = 0;

ISR(PCINT1_vect)
{
    // data = 0, sensor is triggered
    // data != 0, sensor is not triggered
    u08 data = (PINC & _BV(PC_LEDSENSOR));
    if ((LOOKFORFRAMEEND == waitingFor)
        && (0 == data) 
        && (0 == tsFallingSlope))
    {
        tsFallingSlope = millis();
//        highCount = 3;
        waitingFor = LOOKFORGAPEND;
 //       SENSORINT_OFF;
    }
    else if ((LOOKFORGAPEND == waitingFor)
        && (0 != data)
        && (millis() - tsFallingSlope > 120))
    {
        waitingFor = SHUTTEROPEN;
        setServo(SERVO_STOP);
        laserOff();
        SENSORINT_OFF;
    }
}
#endif // SENSORINT

void incMotor()
{
    motorPulse += 5;
    analogWrite(PIN_MOTOR, motorPulse);
}

void decMotor()
{
    motorPulse -= 5;
    analogWrite(PIN_MOTOR, motorPulse);
}

void setMotor(u08 set)
{
    motorPulse = set;
    analogWrite(PIN_MOTOR, motorPulse);
}

void incServo()
{
    if (servoPulse >= SERVO_MAX)
    {
        return;
    }
    servoPulse++;
    OCR2A = servoPulse;
}

void decServo()
{
    if (servoPulse <= SERVO_MIN)
    {
        return;
    }
    servoPulse--;
    OCR2A = servoPulse;
}

void setServo(u08 set)
{
    if (set < SERVO_MIN || set > SERVO_MAX)
    {
        return;
    }
    servoPulse = set;
    OCR2A = servoPulse;
}

void lampOn()
{
    LAMP_ON;
    lampTimeout = millis();
}

void lampOff()
{
    LAMP_OFF;
    lampTimeout = 0;
}

void lampCheck()
{
    if (0 == lampTimeout)
    {
        return;
    }
    if ((millis() - lampTimeout) > LAMP_TIMEOUT_MS)
    {
        lampOff();
    }
}

//u08 checkServoTimeout()
//{
//    if ((millis() - tsServoStart) < SERVO_TIMEOUT_MS)
//    {
//        return 0;
//    }
//    setServo(SERVO_STOP);
//    waitingFor = NONE;
//    return 1;
//}

u08 checkNextFrameTimeout()
{
    if ((millis() - nextFrameTimeout) < NEXT_FRAME_TIMEOUT_MS)
    {
        return 0;
    }
//    nextFrameTimeout = 0;
    Serial.println("Frame Timeout");
    reset();
    return 1;
}

void laserOn()
{
    LASER_ON;
    laserTimeout = millis();
}

void laserOff()
{
    LASER_OFF;
    laserTimeout = 0;
}

void laserCheck()
{
    if (0 == laserTimeout)
    {
        return;
    }
    if ((millis() - laserTimeout) > LASER_TIMEOUT_MS)
    {
        laserOff();
    }
}

void reset()
{
    Serial.println("Reset");
    sensorValue = 0;
    setServo(SERVO_STOP);
    lampOff();
    laserOff();
    SHUTTER_CLOSE;
    setMotor(MOTOR_OFF);
    parameter = 0;
    waitingFor = NONE;
    SENSORINT_OFF;
}

void setup ()
{ 
    Serial.begin(57600);
    Serial.println("Init Start");
    PORTC |= _BV(PC_LEDSENSOR); // tie led sensor line high
    PORTB |= _BV(PB_SHUTTER);
    DDRB |= _BV(PB_LAMP) | _BV(PB_LASER) | _BV(PB_SHUTTER);
    lampOff();
    cli();
    TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20); // prescaler 1024
    TIFR2 &= ~_BV(TOV2); // clear overflow interrupt flag
    TIFR2 &= ~_BV(OCF2A); // clear compare A interrupt flag
    TIMSK2 |= _BV(OCIE2A) | _BV(TOIE2); // enable counter2 overflow interrupt
    OCR2A = SERVO_STOP;
    servoPulse = SERVO_STOP;
//    setServo(SERVO_STOP);
    TCNT2 = 0x00;
    DDRB |= _BV(PB_SERVO);
    //shifter = SHIFTER_INIT;
    // enable analog input, AVcc with external cap to AREF
    //ADMUX = _BV(MUX2) | _BV(REFS0);
    // enable, left adjusted, /128 clock divider
    //ADCSRA = _BV(ADEN) | _BV(ADLAR) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    sei();
    analogWrite(PIN_MOTOR, motorPulse);
    Serial.println("Init OK");
}

//u32 lastSample = 0;
void loop ()
{
    laserCheck();
    lampCheck();

    if (sensorValue) //  && (millis() - lastSample) > 2)
    {
        u16 value = analogRead(PIN_LEDSENSOR);
        if ((value >= 512) && sensorValue < SENSOR_VALUE_MAX)
        {
            sensorValue += SENSOR_STEP;
        }
        else if ((value < 512) && sensorValue > SENSOR_VALUE_MIN)
        {
            sensorValue -= SENSOR_STEP;
        }
    }

    switch (waitingFor)
    {
        case NONE:
            break;

        case LASERBLINKON:
            laserOn();
            DELAYEDSTATE(40, LASERBLINKOFF);
            break;

        case LASERBLINKOFF:
            laserOff();
            DELAYEDSTATE(100, LASERBLINKON);
            break;

        case FRAMESTOP:
            setServo(SERVO_STOP);
            delay(200);
            setMotor(MOTOR_OFF);
            laserOff();
            sensorValue = 0;
            DELAYEDSTATE(500, SHUTTEROPEN);
            break;

        case LOOKFORGAPEND:
            if (SENSOR_VALUE_MIN < sensorValue)
            {
                break;
            }
//            DELAYEDSTATE(500, LOOKFORFRAMEEND);
            waitingFor = FRAMESTOP;
            break;

        case LOOKFORFRAMEEND:
            if (SENSOR_VALUE_MAX > sensorValue)
            {
                break;
            }
            DELAYEDSTATE(500, LOOKFORGAPEND);
            //waitingFor = FRAMESTOP;
            break;

        case SENSORENABLE:
            break;

        case SENSORSTART:
            setMotor(motorPretensionNext);
            delay(1000);
            laserOn();
            //ledCount = 0;
            setServo(SERVO_NEXTFRAME_SLOW);
            waitingFor = LOOKFORFRAMEEND;
        //    waitingFor = LOOKFORGAPEND;
            sensorValue = SENSOR_VALUE_INIT_MAX;
            SENSORINT_ON;
            break;

        case SHUTTERCLOSED:
            SHUTTER_CLOSE;
            //DELAYEDSTATE(400, SENSORSTART);
            //tsServoStart = millis();
            waitingFor = NONE;
            break;

        case DELAYLOOP:
            delay(10);
            --stateLoop;
            if (0 == stateLoop)
            {
                waitingFor = stateSaved;
            }
            break;

        case EXPOSURESERIES5:
            lampOff();
            waitingFor = SHUTTERCLOSED;
            break;

        case EXPOSURESERIES4:
            lampOn();
            DELAYEDSTATE(100, EXPOSURESERIES5);
            break;

        case EXPOSURESERIES3:
            lampOff();
            DELAYEDSTATE(260, EXPOSURESERIES4);
            break;

        case EXPOSURESERIES2:
            lampOn();
            DELAYEDSTATE(20, EXPOSURESERIES3);
            break;

        case EXPOSURESERIES1:
            lampOn();
            delay(2);
            lampOff();
            DELAYEDSTATE(350, EXPOSURESERIES2);
            break;

        case EXPOSURESERIES:
            DELAYEDSTATE(200, EXPOSURESERIES1);
            break;

        case SHUTTEROPEN:
            SHUTTER_OPEN;
            waitingFor = EXPOSURESERIES;
            break;

        default:
            break;
    }

    if (!Serial.available())
    {
        return;
    }

    u08 input(Serial.read());
    switch (input)
    {
        case 'b':
            waitingFor = LASERBLINKON;
            break;

        case 'c':
            SHUTTER_OPEN;
            delay(20);
            SHUTTER_CLOSE;
            break;

        case 'C': // triple shutter
            nextFrameTimeout = millis();
            //waitingFor = TRIPLESTART;
            waitingFor = SENSORSTART;
            break;

        case 'x':
            reset();
            break;

        case 'l':
            //LAMP_ON;
            lampOn();
            break;

        case 'L':
            //LAMP_OFF;
            lampOff();
            break;

        case 'e':
            laserOn();
            break;

        case 'E':
            laserOff();
//            LASER_OFF;
            break;

        case 's':
            setServo(parameter);
            break;

        case 'm':
            if (parameter > 0)
            {
                setMotor(parameter);
            }
            break;

        case 't': // pretension
            setServo(SERVO_STOP);
//            if (parameter > 0)
//            {
                setMotor(motorPretensionNext);
//            }
            break;

        case 'u': // untension
            setServo(SERVO_STOP);
            setMotor(MOTOR_OFF);
            break;

        case 'f': // forward
            setMotor(MOTOR_PRETENSION_FF);
            setServo(SERVO_NEXTFRAME_SLOW);
            break;

        case 'p': // triple picture
            waitingFor = SHUTTEROPEN;
            break;

        case 'r': // rewind
            setMotor(MOTOR_REWIND);
            setServo(SERVO_STOP);
            break;

        case 'n': // next frame
            nextFrameTimeout = millis();
            waitingFor = SENSORSTART;
        //    waitingFor = LOOKFORFRAMEEND;
        //    sensorValue = SENSOR_VALUE_INIT;
            //ledCount = 0;
            break;

        case 'v': // verbose
            Serial.print("waitingFor ");
            Serial.print(waitingFor, 10);
            Serial.print(" Servo ");
            Serial.print(servoPulse, 10);
            Serial.print(" Motor ");
            Serial.print(motorPulse, 10);
//            Serial.print(" sensorActive ");
//            Serial.println(sensorActive, 10);
            break;

        case '[':
            motorPretensionNext = parameter;
            break;

        case '-':
            parameter = 0;
            break;

        default:
            if (input >= '0' && input <= '9')
            {
                parameter *= 10;
                parameter += (input - '0');
            }
            Serial.print(parameter, 10);
            break;
    }
    return;
}
