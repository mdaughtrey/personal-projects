#include <WProgram.h>
#include <ps2/ps2mouse.h>

#pragma GCC diagnostic ignored "-Wwrite-strings"

long xx = 0;
long yy = 0;
unsigned long timer = 0;
extern int xPos;
extern int yPos;
extern int intCount;
void showByteLog(void);
void resetByteLog(void);

PS2Mouse mouse;

void setup ()
{
    Serial.begin(57600);
    timer = millis();
    Serial.println("OK0");
    mouse.initialize();
    Serial.println("OK1");
}

void loop ()
{
    int data[3] = {0, 0, 0};
    int * pData = &data[0];

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

         case 'l':
             showByteLog();
             break;

         case 'e':
            mouse.set_remote_mode();
            break;

         case 's':
            Serial.print('s');
            mouse.set_stream_mode();
            Serial.println(' ');
            break;

         case 'i':
//            mouse.getIntCount();
            Serial.println(intCount);
            break;

         case 'R':
            resetByteLog();
            break;

         case 'r': // raw report
            Serial.print("X");
            Serial.print(xPos);
            Serial.print(" Y");
            Serial.println(yPos);
            break;
        }
    }
}
