// https://github.com/T-vK/ESP32-BLE-Keyboard
#include <Wire.h>
#include <BleKeyboard.h>
//#include <AsyncTimer.h>

//#define I2CSCAN
#define PAJ7620
#ifdef PAJ7620
#include "RevEng_PAJ7620.h"
#endif // PAJ7620
#ifdef APDS9960
#include "SparkFun_APDS9960.h"
#endif // ADPS9960
#include <PrintEx.h>

#define FSH(s) reinterpret_cast<const __FlashStringHelper *>(s)

using namespace ios;

BleKeyboard blek("Pacemaker", "mattd", 100);
//AsyncTimer at;

StreamEx serout = Serial;
#ifdef PAJ7620
RevEng_PAJ7620 sensor = RevEng_PAJ7620();
#endif // PAJ7620

#define I2C_SCL 22
#define I2C_SDA 21

#ifdef APDS9960
SparkFun_APDS9960 apds;
// 22 = SCL
// 21 = SDA
const unsigned int MoveTimeout = 300;
const unsigned char ProxThreshold = 35;
const unsigned short GESTURE_INT_PIN = 5;
volatile boolean hasMovement = false;
//boolean gesture = true;
#endif // ADPS9960

const char APDSINIT[] PROGMEM = "APDS9660 Init Fail";
const char APDSENABLE[] PROGMEM = "APDS9660 Enable Fail";
const char UP[] PROGMEM = "Up";
const char DOWN[] PROGMEM = "Down";
const char LEFT[] PROGMEM = "Left";
const char RIGHT[] PROGMEM = "Right";
const char NEAR[] PROGMEM = "Near";
const char FAR[] PROGMEM = "Far";

#ifdef APDS9960
void transmissionReady() {
  hasMovement = true;
}

void handleGesture()
{
    if (!apds.isGestureAvailable())
    {
        return;
    }

    switch (apds.readGesture()) {
        case DIR_UP:
            serout << FSH(UP) << endl;
            break;

        case DIR_DOWN:
            serout << FSH(DOWN) << endl;
            break;

        case DIR_LEFT:
            serout << FSH(LEFT) << endl;
            break;

        case DIR_RIGHT:
            serout << FSH(RIGHT) << endl;
            break;

        case DIR_NEAR:
            serout << FSH(NEAR) << endl;
            break;

        case DIR_FAR:
            serout << FSH(FAR) << endl;
            break;

        default:
            break;
    }
}

void initGestureSensor(void)
{
 //   attachInterrupt(digitalPinToInterrupt(GESTURE_INT_PIN), transmissionReady, FALLING);
    while (!apds.init())
    {
        serout << FSH(APDSINIT) << endl;
        delay(1000);
    }

    while (!apds.enableGestureSensor(true))
    {
        serout << FSH(APDSENABLE) << endl;
        delay(1000);
    }
}
#endif // ADPS9960
#ifdef PAJ7620
void pollSensor()
{
    Gesture gesture;                  // Gesture is an enum type from RevEng_PAJ7620.h
    gesture = sensor.readGesture();   // Read back current gesture (if any) of type Gesture
    if ((int)gesture) {
    serout << "Gesture " << (int)gesture << " connected " << (int)blek.isConnected() << endl;
    }

    switch (gesture)
    {
        case GES_FORWARD:
            serout << "GES_FORWARD" << endl;
            break;

        case GES_BACKWARD:
            serout << "GES_BACKWARD" << endl;
            break;

        case GES_LEFT:
            if (blek.isConnected())
            {
                serout << "Next Track" << endl;
                blek.write(KEY_MEDIA_NEXT_TRACK);
            }
            break;

        case GES_RIGHT:
            if (blek.isConnected())
            {
                serout << "Previous Track" << endl;
                blek.write(KEY_MEDIA_PREVIOUS_TRACK);
            }
            break;

        case GES_UP:
            if (blek.isConnected())
            {
                serout << "Play/Pause" << endl;
                blek.write(KEY_MEDIA_PLAY_PAUSE);
            }
            break;

        case GES_DOWN:
            serout << "GES_DOWN" << endl;
            break;

        case GES_CLOCKWISE:
            if (blek.isConnected())
            {
                serout << "Volume Down" << endl;
                blek.write(KEY_MEDIA_VOLUME_DOWN);
            }
            break;

        case GES_ANTICLOCKWISE:
        if (blek.isConnected())
        {
            serout << "Volume Up" << endl;
            blek.write(KEY_MEDIA_VOLUME_UP);
        }
        break;

        case GES_WAVE:
        if (blek.isConnected())
        {
            serout << "Stop" << endl;
            blek.write(KEY_MEDIA_STOP);
        }
        break;

        case GES_NONE:
            break;
    }
//   at.setTimeout(pollSensor, 100);
}
#endif // PAJ7620

#ifdef I2CSCAN
void i2cscan()
{
    byte error, address;
    int nDevices;
    serout << "Scanning..." << endl;
    nDevices = 0;
    for(address = 1; address < 127; address++ ) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0) {
            serout << "I2C device found at address  " << address << endl;
            nDevices++;
        }
        else if (error==4) {
            serout  << "Unknown error at address " << address << endl;
        }    
    }
    if (nDevices == 0) {
        serout << "No I2C devices found" << endl;
    }
    else {
        serout << "Done" << endl;
    }
}

#endif // I2CSCAN

void setup(void)
{
    Serial.begin(115200);
    blek.begin();
#ifdef I2CSCAN
//    serout << "SDA" << I2C_SDA << endl;
//    serout << "SCL" << I2C_SCL << endl;
//    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.begin();
    while (true) {
        i2cscan();
        delay(5000);
}
#endif // I2CSCAN
#ifdef APDS9960
    initGestureSensor();
#endif // ADPS9960
	serout << "void setup" << endl;
#ifdef PAJ7620
//    Wire.begin(I2C_SDA, I2C_SCL);
//    serout << "wire begin " << endl;
    Wire.begin();
//    serout << "wire began " << endl;
    if( !sensor.begin(&Wire) )           // return value of 0 == success
    {
        serout << "PAJ7620 I2C error - halting" << endl;
        while(true) {
        serout << "PAJ7620 I2C error - halting" << endl;
            delay(1000);
        }
    }
//	serout << "seems like it worked" << endl;
//    at.setTimeout(pollSensor, 100);
#endif // PAJ7620
}

void loop(void)
{
   pollSensor();
//    at.handle();
//     if (blek.isConnected())
//     {
//         blek.write(KEY_MEDIA_NEXT_TRACK);
//     }
//     else
//     {
//         serout << "Not connected" << endl;
//     }
//     delay(10000);
    //if (hasMovement && blek.isConnected())
#ifdef APDS9960
    if (hasMovement)
    { // sensed a movement
       handleGesture();
       hasMovement = false;
       // Reinitialize as workaround for sensor hanging when gesturing too fast
//       initGestureSensor();
     }
#endif // ADPS9960
}
