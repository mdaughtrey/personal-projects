// #include <WProgram.h>
// 
// #include <avrlibtypes.h>
// #include <stepper.h>
// #pragma GCC diagnostic ignored "-Wwrite-strings"

// -4: PC2 Rewind
// 11: PD6 Reset
//  8: PB0 Pretension
//  4: PB4 Next
#define PC_REWIND 3
#define PD_RESET 6
#define PB_PRETENSION 0
#define PB_NEXT 4
uint8_t pbState = 0xff;
uint8_t pcState = 0xff;
uint8_t pdState = 0xff;

#define PB_LAMP 1
#define PIN_MOTOR 10  // PB2
//#define PB_SIGTRIPLE 5

#define PC_TENSIONSENSOR 0 // PC0
#define PC_OPTOINT 1 // PC1 PCINT9
//#define PC_SHUTTER 2

//#define PD_SHUTTERREADY 7
#define OPTOINT_TIMEOUT 10000

#define MOTOR_PRETENSION_NEXT 40 
#define MOTOR_PRETENSION_MAX 70 
#define MOTOR_PRETENSION_FF 20
#define MOTOR_REWIND_FAST 100 
#define MOTOR_REWIND_SLOW 70 
#define MOTOR_OFF 0 
#define PRETENSIONDELAY 200
#define LAMPDEFER 0
#define LAMP_TIMEOUT_MS 3000
#define NEXT_FRAME_TIMEOUT_MS 8000
#define SENSORTHRESHOLD 200
#define LAMP_OFF PORTB &= ~_BV(PB_LAMP)
#define LAMP_ON PORTB |= _BV(PB_LAMP)
//#define SHUTTER_READY PIND & _BV(PD_SHUTTERREADY)
#define SENSORVALUEINIT { sensorValue = PINC & _BV(PC_OPTOINT); }
//#define SHUTTERINT_ON { PCMSK2 |= _BV(PCINT22); PCICR |= _BV(PCIE2); }
//#define SHUTTERINT_OFF { PCMSK2 &= ~_BV(PCINT22); PCICR &= ~_BV(PCIE2); }
//#define OPTOINT_ON { PCMSK1 |= _BV(PCINT9; PCICR |= _BV(PCIE1); }
//#define OPTOINT_OFF { PCMSK1 &= ~_BV(PCINT9; PCICR &= ~_BV(PCIE1); }

typedef enum
{ 
        NONE = 0,           
        //FRAMESTOP,          
        SENSORSTART,        
//        OPTOINT_CHANGED,    
        STOPONFRAMEZERO,    
        PRETENSION,
        OPTOINT_HALF,       
        OPTOINT_FULL,       
//        OPTOINT_REVERSE,    
//        OPTOINT_FULL2
} WaitFor;

typedef enum
{
    FILM_NONE = 0,
    FILM_8MM,
    FILM_SUPER8
} FilmMode;

void setMotor(uint8_t set); void lampOn();
void lampOff();
void lampCheck();
void p4_reinit();

uint16_t parameter = 0;
volatile uint8_t intCount = 0;
volatile uint8_t lastIntCount = 0;
uint8_t verbose = 0;
uint8_t motorPulse = 255;
uint8_t pretension = MOTOR_PRETENSION_NEXT;
volatile uint32_t optoIntTimeout;
uint32_t cameraBusyTimeout;
uint8_t sensorValue;
uint8_t waitingFor(NONE);
uint8_t lastCommand;
uint8_t filmMode = FILM_NONE;
uint8_t stepperDelay = 1;

void setMotor(uint8_t set)
{
    motorPulse = set;
//    analogWrite(PIN_MOTOR, motorPulse);
}

void lampOn()
{
//    LAMP_ON;
}

void lampOff()
{
//    LAMP_OFF;
}

#define NOTRANSITION 0
#define FULLFRAME 1
#define HALFFRAME 2

uint8_t halfFrameTransition()
{
    sensorValue = !sensorValue;
    optoIntTimeout = millis();
    return HALFFRAME; // half frame transition
}

uint8_t fullFrameTransition()
{
    sensorValue = !sensorValue;
    //optoIntTimeout = 0;
    return FULLFRAME; // full frame transition
}
//
// 0 = no transition
// 1 = full frame
// 2 = half frame
//
uint8_t isOptoTransition()
{
//    uint32_t now = millis();
//    if (0 == optoIntTimeout)
//    {
//        optoIntTimeout = now;
//    }
//    if ((now - optoIntTimeout) > OPTOINT_TIMEOUT)
//    {
//        Serial.print("{OIT:");
//        Serial.print(now - optoIntTimeout, 10);
//        Serial.println("}");
//        p4_reinit();
//        return NOTRANSITION;
//    }
//    if (FILM_SUPER8 == filmMode)
//    {
//        if ((PINC & _BV(PC_OPTOINT)) && 0 == sensorValue)
//        {
//            return halfFrameTransition();
//        }
//        else if ((0 == (PINC & _BV(PC_OPTOINT))) && 0 != sensorValue)
//        {
//            return fullFrameTransition();
//        }
//    }
//    else if (FILM_8MM == filmMode)
//    {
//        if ((PINC & _BV(PC_OPTOINT)) && 0 == sensorValue)
//        {
//            return fullFrameTransition();
//        }
//        else if ((0 == (PINC & _BV(PC_OPTOINT))) && 0 != sensorValue)
//        {
//            return halfFrameTransition();
//        }
//    }
    return 0; // no transition
}

void p4_reinit()
{
//    uint8_t ii;
////    Serial.println("Reset");
//    stepperInit();
//    lampOff();
//    setMotor(MOTOR_OFF);
//    parameter = 0;
//    waitingFor = NONE;
//    verbose = 0;
//    stepperDelay = 1;
//    for (ii = 0; ii < 5; ii++)
//    {
//        lampOn();
//        delay(80);
//        lampOff();
//        delay(80);
//    }
}

void setup ()
{ 
    Serial.begin(115200);
    Serial.println("setup");
//    Serial.begin(57600);
//    Serial.println("Init Start");
//    lastCommand = 0;
//    PORTC |= _BV(PC_OPTOINT) | _BV(PC_REWIND); // tie opt int sensor line hight
//    PORTB |= _BV(PB_PRETENSION) | _BV(PB_NEXT);
//    //PORTD |= _BV(PD_RESET);
//    PORTD |= _BV(PD_RESET);
//
//    analogWrite(PIN_MOTOR, MOTOR_OFF);
//    //PORTC |= _BV(PC_SHUTTER) | _BV(PC_TENSIONSENSOR);
//    PORTC |= _BV(PC_TENSIONSENSOR);
//    //DDRB |= _BV(PB_LAMP) | _BV(PB_SIGTRIPLE);
//    DDRB |= _BV(PB_LAMP);
////    DDRC |= _BV(PC_SHUTTER);
//    p4_reinit();
//    Serial.println("{State:Ready}");
}

#ifdef LOGSTATE
uint8_t lastReportedState(255);
#endif // LOGSTATE

bool buttonTest(uint8_t pins, uint8_t * state, uint8_t testBit)
{
//    if (!(pins & _BV(testBit)))
//    {
//        if (*state & _BV(testBit))
//        { 
//            *state &= ~_BV(testBit);
//            return true;
//        }
//        return false;
//    }
//    *state |= _BV(testBit); 
    return false;
}

uint8_t lastState = 0;
void loop ()
{
	Serial.println("FRAMESTOP");
	delay(1000);
//  //    if(!lastState)
//  //    {
//  //        if(PINC & _BV(PC_OPTOINT)) { Serial.print("0"); lastState = 1; }
//  //    }
//  //    else
//  //    {
//  //        if(!(PINC & _BV(PC_OPTOINT))) { Serial.print("1"); lastState = 0; }
//  //    }
//      stepperPoll(stepperDelay);
//  
//      switch (waitingFor)
//      {
//          case NONE:
//              break;
//  
//  //        case FRAMESTOP:
//  //            stepperStop();
//  //            if (1 == verbose)
//  //            {
//  //                Serial.println("FRAMESTOP");
//  //            }
//  //            if ('n' == lastCommand)
//  //            {
//  //                waitingFor = NONE;
//  //            }
//  //            break;
//              
//          case OPTOINT_HALF:
//              if (2 == verbose)
//              {
//                  Serial.println("OPTOINT_HALF");
//              }
//              if (HALFFRAME == isOptoTransition())
//              {
//                  //OPTOINT_ON;
//                  waitingFor = OPTOINT_FULL;
//              }
//              break;
//  
//          case OPTOINT_FULL:
//              if (2 == verbose)
//              {
//                  Serial.println("OPTOINT_FULL");
//              }
//              if (FULLFRAME == isOptoTransition())
//              {
//                 // setMotor(pretension);
//                  stepperStop();
//                  if (1 == verbose)
//                  {
//                      Serial.println("FRAMESTOP");
//                  }
//                  //waitingFor = FRAMESTOP;
//                  waitingFor = NONE;
//              }
//              break;
//  
//  //        case OPTOINT_REVERSE:
//  //            if (2 == verbose)
//  //            {
//  //                Serial.println("OPTOINT_REVERSE");
//  //            }
//  //            if (HALFFRAME == isOptoTransition())
//  //            {
//  //                setMotor(pretension);
//  //                waitingFor = OPTOINT_FULL2;
//  //            }
//  //            break;
//  
//  //        case OPTOINT_FULL2:
//  //            if (2 == verbose)
//  //            {
//  //                Serial.println("OPTOINT_FULL2");
//  //            }
//  //            if (FULLFRAME == isOptoTransition())
//  //            {
//  //                waitingFor = FRAMESTOP;
//  //            }
//  //            break;
//  
//  //        case OPTOINT_CHANGED:
//  //            if (2 == verbose)
//  //            {
//  //                Serial.println("OPTOINT_CHANGED");
//  //            }
//  //            switch (isOptoTransition())
//  //            {
//  //                case 1:
//  //                    optoIntTimeout = 0;
//  //                    waitingFor = OPTOINT_HALF;
//  //                    setMotor(MOTOR_REWIND_SLOW);
//  //                    break;
//  //            }
//  //            break;
//  
//          case SENSORSTART:
//              if (2 == verbose)
//              {
//                  Serial.println("SENSORSTART");
//              }
//              stepperGo();
//              SENSORVALUEINIT;
//              optoIntTimeout = 0;
//              waitingFor = OPTOINT_HALF;
//              break;
//  
//          case PRETENSION:
//              if (MOTOR_PRETENSION_NEXT == pretension)
//              {
//                  stepperStop();
//                  pretension++;
//                  break;
//              }
//              if (pretension < 70)
//              {
//                  setMotor(10);
//                  delay(PRETENSIONDELAY);
//                  setMotor(70);
//                  delay(PRETENSIONDELAY);
//                  setMotor(10);
//                  delay(PRETENSIONDELAY);
//                  setMotor(pretension);
//                  delay(PRETENSIONDELAY);
//                  if (verbose)
//                  {
//                      Serial.print("PT ");
//                      Serial.print(pretension, 10);
//                      Serial.print(" Sense ");
//                      Serial.print(PINC & _BV(PC_TENSIONSENSOR) ? 1 : 0);
//                      Serial.print("\r\n");
//                  }
//                  if (PINC & _BV(PC_TENSIONSENSOR))
//                  {
//                      waitingFor = NONE;
//                      pretension /= 5;
//                      pretension *= 4;
//                      setMotor(pretension);
//                      Serial.print("{pt:");
//                      Serial.print(pretension, 10);
//                      Serial.println("}");
//                      break;
//                  }
//                  pretension += 4;
//              }
//              break;
//  
//         case STOPONFRAMEZERO:
//              if (isOptoTransition())
//              {
//                  {
//                      setMotor(MOTOR_OFF);
//                      waitingFor = NONE;
//                  }
//              }
//          break;
//  
//          default:
//              break;
//      }
//      if (buttonTest(PINB, &pbState, PB_PRETENSION))
//      {
//          waitingFor = PRETENSION;
//          pretension = MOTOR_PRETENSION_NEXT;
//          return;
//      }
//      else
//      if (buttonTest(PINB, &pbState, PB_NEXT))
//      {
//          lastCommand = 'n'; 
//      }
//      else if (buttonTest(PIND, &pdState, PD_RESET))
//      {
//          lastCommand = ' '; 
//      }
//      else if (buttonTest(PINC, &pcState, PC_REWIND))
//      {
//          lastCommand = 'r'; 
//      }
//      else if (Serial.available())
//      {
//          lastCommand = Serial.read();
//      }
//      else return;
//  
//      switch (lastCommand)
//      {
//          case 's':
//          if (parameter > 0)
//          {
//              stepperDelay = parameter;
//          }
//          break;
//  
//          case 'd':
//              filmMode = FILM_8MM;
//              Serial.println("{mode:8mm}");
//              break;
//  
//          case 'D':
//              filmMode = FILM_SUPER8;
//              Serial.println("{mode:super8}");
//              break;
//  
//          case 'x':
//              Serial.print("optoIntTimeout ");
//              Serial.print((int)optoIntTimeout);
//              Serial.print("\r\nsensorValue ");
//              Serial.print((int)sensorValue);
//              Serial.print("\r\nwaitingFor ");
//              Serial.print((int)waitingFor);
//              Serial.print("\r\nlastCommand ");
//              Serial.print(lastCommand);
//              Serial.print("\r\nstepperDelay ");
//              Serial.print((int)stepperDelay);
//              Serial.print("\r\nfilmMode ");
//              Serial.print((int)filmMode);
//              Serial.print("\r\nparameter ");
//              Serial.print((int)parameter);
//              Serial.print("\r\npretension ");
//              Serial.print((int)pretension);
//              Serial.print("\r\nSense ");
//              Serial.print(PINC & _BV(PC_TENSIONSENSOR) ? 1 : 0);
//              Serial.print("\r\n");
//              break;
//  
//          case ' ':
//              Serial.println("RESET");
//              PORTD |= _BV(PD_RESET);
//              DDRD |= _BV(PD_RESET);
//              PORTD &= ~_BV(PD_RESET);
//              break;
//  
//              break;
//  
//          case 'l':
//              //LAMP_ON;
//              lampOn();
//              break;
//  
//          case 'L':
//              //LAMP_OFF;
//              lampOff();
//              break;
//  
//          case 'T': // manual pretension
//              pretension = parameter;
//              setMotor(pretension);
//              break;
//  
//          case 't': // auto pretension
//              optoIntTimeout = millis();
//              pretension = MOTOR_PRETENSION_NEXT;
//              waitingFor = PRETENSION;
//              break;
//  
//  
//          case 'u': // untension
//              stepperStop();
//              setMotor(MOTOR_OFF);
//              break;
//  
//          case 'f': // forward
//              setMotor(MOTOR_PRETENSION_FF);
//              stepperGo();
//              break;
//  
//          case 'r':
//              SENSORVALUEINIT;
//              setMotor(MOTOR_REWIND_FAST);
//              break;
//  
//          case 'n': // next frame
//              waitingFor = SENSORSTART;
//              break;
//  
//          case 'v': // verbose
//              verbose = 1;
//              break;
//  
//          case 'V': // very verbose
//              verbose = 2;
//              break;
//  
//          case '[':
//              pretension = parameter;
//              parameter = 0;
//              break;
//  
//          case '-':
//              parameter = 0;
//              break;
//  
//          default:
//              if (lastCommand >= '0' && lastCommand <= '9')
//              {
//                  parameter *= 10;
//                  parameter += (lastCommand - '0');
//              }
//              if (verbose)
//              {
//                  Serial.print(parameter, 10);
//              }
//              break;
//      }
    return;
}
