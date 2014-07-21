#include <WProgram.h>
#include <ps2/ps2mouse.h>
#include <avrlibtypes.h>

#pragma GCC diagnostic ignored "-Wwrite-strings"

unsigned long timer; // = 0;
extern volatile s16 xPos;
extern volatile s16 yPos;
extern u08 packet0;
extern u08 packet1;
extern u08 packet2;

#ifdef SDEBUG
void showByteLog(void);
void resetByteLog(void);
#endif // SDEBUG
void debugReport(void);

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
    if (millis() - timer > 1000)
    {
            debugReport();
            timer = millis();
    }
    if (Serial.available())
    {
        switch (Serial.read())
        {
         case '0':
             xPos = 0;
             yPos = 0;
#ifdef SDEBUG
            resetByteLog();
#endif // SDEBUG
             break;

         case 'x': // reset
             Serial.print("Init");
             mouse.initialize();
             Serial.println('.');
             break;

         case 's':
#ifdef SDEBUG
             showByteLog();
#endif // SDEBUG
             break;

         case 'r': // raw report
            debugReport();
            break;
        }
    }
}
