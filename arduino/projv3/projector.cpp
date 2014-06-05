#include <WProgram.h>
#include <ps2/ps2mouse.h>

#pragma GCC diagnostic ignored "-Wwrite-strings"

#define KBDDATA 12
#define KBDCLOCK 11

PS2Mouse mouse(KBDCLOCK, KBDDATA, STREAM);


        //Serial.println("? - this text");

//void toMouse(unsigned char byte)
//{
//    Serial.print("Writing: ");
//    Serial.print(byte, 16);
//    Mouse.write(byte);
//    Serial.println("");
//}
//
//unsigned char fromMouse()
//{
//    Serial.print("Reading: ");
//    unsigned char byte = Mouse.read();
//    Serial.print(byte, 16);
//    Serial.println("");
//}

void setup ()
{
//    pinMode(lamp, OUTPUT);

 //   digitalWrite(sensor, HIGH);
    Serial.begin(57600);
    Serial.print("init");
    mouse.initialize(); 
    Serial.print("init done");
}

void loop ()
{
    int data[2];
    mouse.report(data);
    Serial.print(data[0]); // Status Byte
    Serial.print(":");
    Serial.print(data[1]); // X Movement Data
    Serial.print(",");
    Serial.print(data[2]); // Y Movement Data
    Serial.println();
}
