#include <WProgram.h>
#include <ps2/ps2mouse.h>

#pragma GCC diagnostic ignored "-Wwrite-strings"

#define KBDDATA 7
#define KBDCLOCK 6
#define LED 13

long xx = 0;
long yy = 0;
unsigned long timer = 0;

PS2Mouse mouse;

void setup ()
{
//    digitalWrite(3, HIGH);
//    pinMode(3, OUTPUT);
//    digitalWrite(3, HIGH);
    Serial.begin(57600);
    Serial.println("OK");
    timer = millis();
    mouse.initialize();
    mouse.set_remote_mode();
}

#if 0
void rawReport(unsigned char (&data)[3])
{
    mouse.rawReport(data);
    if (data[0])
    {
        if (data[0] & (1 << 4))
        {
            xx -= ((data[1] ^ 0xff) + 1);
        }
        else
        {
            xx += data[1];
        }
        if (data[0] & (1 << 5))
        {
            yy -= ((data[2] ^ 0xff) + 1);
        }
        else
        {
            yy += data[2];
        }
    }
}
#endif // 0

void loop ()
{
    int data[3] = {0, 0, 0};
    int * pData = &data[0];

//    mouse.report(pData);
//    xx += data[1];
//    yy += data[2];
//
//    if ((millis() - timer) > 1000)
//    {
//        Serial.print(data[0], 16);
//        Serial.print(' ');
//        Serial.print(data[1]);
//        Serial.print(' ');
//        Serial.print(data[2]);
//        Serial.print(' ');
//        Serial.print(xx);
//        Serial.print(' ');
//        Serial.print(yy);
//        Serial.println(" ");
//        timer = millis();
//    }

    if (Serial.available())
    {
        switch (Serial.read())
        {
         case '0':
             xx = 0;
             yy = 0;
             break;

         case 'm':
             mouse.set_remote_mode();
             break;

         case 'x': // reset
             Serial.print("Init");
             mouse.initialize();
             Serial.println('.');
             break;

         case 'r': // raw report
             mouse.report(pData);
             xx += data[1];
             yy += data[2];
             Serial.print(data[0], 16);
             Serial.print(' ');
             Serial.print(data[1]);
             Serial.print(' ');
             Serial.print(data[2]);
             Serial.print(' ');
             Serial.print(xx);
             Serial.print(' ');
             Serial.print(yy);
             Serial.println(" ");
             break;
        }
    }
}
