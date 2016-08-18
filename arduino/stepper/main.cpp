#include <WProgram.h>
#include <avrlibtypes.h>
#include <stepper.h>

#pragma GCC diagnostic ignored "-Wwrite-strings"

void setup ()
{ 
    Serial.begin(57600);
    Serial.println("Init Start");
    stepperInit();
    Serial.println("Init Done");
}

void loop ()
{
    stepperPoll();
    if (!Serial.available())
    {
        return;
    }

    switch (Serial.read())
    {
    case '-':
        stepperDelay(-1);
        break;
        
    case '=':
        stepperDelay(1);
        break;

    case ' ':
        stepperEStop();
        Serial.println("EStop");
        break;

    case 'i':
        stepperInit();
        Serial.println("Init");
        break;

    case 'q':
        coilAForward();
        break;

    case 'w':
        coilAOff();
        break;

    case 'e':
        coilABackward();
        break;

    case 'a': 
        coilBForward();
        break;

    case 's':
        coilBOff();
        break;

    case 'd':
        coilBBackward();
        break;

    case 'n':
        stepperNext();
        break;
    }
}



