#include <Adafruit_APDS9960.h>
#include <Adafruit_VL53L0X.h>
#include <ESP8266WiFi.h>
#include <ostream>
#include <StreamDev.h>
#include <StreamString.h>
// #include "mdns.h"

using namespace std;
typedef void (*DoFunc)(void);

Adafruit_APDS9960 apds;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

typedef enum {
	ASLEEP, IDLE, MOVING_UP, MOVING_DOWN, STATE_LAST
} State;

typedef enum {
    BUTTON, UP, DOWN, GESTURE, FAR, NEAR, TIMER, EVENT_LAST
} Event;

const char * EventNames[EVENT_LAST] = {"BUTTON","UP","DOWN","GESTURE","FAR","NEAR","TIMER"};


uint8_t events[EVENT_LAST] = {0};
State state = IDLE;
uint8_t resetEvent = 0;
StreamString cout(Serial);

void doIgnore(void)
{
    StreamConstPtr("ignore").sendAll(Serial);
    resetEvent = 1;
}

 
void doWakeup(void)
{
    StreamConstPtr("doWakeup").sendAll(Serial);
    //# start Wifi
    //# Get time
//    setLocalTime()
    state = IDLE;
    resetEvent = 0;
}

void doGetTime(void)
{
    StreamConstPtr("doGetTime").sendAll(Serial);
//    setLocalTime()
    state = IDLE;
    resetEvent = 0;
}
 
void doMoveUp(void)
{
    StreamConstPtr("doMoveUp").sendAll(Serial);
    state = MOVING_UP;
    resetEvent = 0;
}

void doMoveDown(void)
{
    StreamConstPtr("doMoveDown").sendAll(Serial);
    state = MOVING_DOWN;
    resetEvent = 0;
}

void doIdle(void)
{
    StreamConstPtr("doIdle").sendAll(Serial);
    state = IDLE;
    resetEvent = 0;
}

void getDistance(void)
{
    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);
    if (4 == measure.RangeStatus)
    {
        Serial.println("Lidar out of range");
        return;
    }

    Serial.print("getDistance (mm): ");
    Serial.println(measure.RangeMilliMeter);
}

void getGesture(void)
{
    if (!apds.gestureValid())
    {
        return;
    }
    
    uint8_t g = apds.readGesture();
    if(g) {
        Serial.print("getGesture ");
        Serial.println(g);
    }

    // switch (apds.readGesture())
    switch (g)
    {
        case APDS9960_DOWN:  events[DOWN] = 1; break;
        case APDS9960_UP:    events[UP] = 1; break;
        case APDS9960_LEFT:  events[GESTURE] = 1; break;
        case APDS9960_RIGHT: events[GESTURE] = 1; break;
    }
}

void getButton(void)
{
}

// # Dispatch Table

//DoFunc[EVENT_LAST][STATE_LAST] dispatch = {
DoFunc dispatch[] = {
    doWakeup, doIgnore, doIgnore, doIgnore, doIgnore, doIgnore, doGetTime,  //  ASLEEP
    doIgnore, doMoveUp, doMoveDown, doIgnore, doIgnore, doIgnore, doGetTime,  //  IDLE
    doIgnore, doIdle,   doIdle,   doIdle,     doIdle, doIdle, doIdle, doIgnore,     //  MOVING_UP
    doIdle,   doIdle,   doIdle,     doIdle, doIdle, doIdle, doIgnore};    //  MOVING_DOWN
//  BUTTON    UP        DOWN        GESTURE FAR     NEAR    TIMER

//mdns::MDns myDns(packetCallback, queryCallback, answerCallback);

void setup()
{
    Serial.begin(115200);
//    pinMode(LED_BUILTIN, OUTPUT);
    if(!apds.begin())
    {
        while (1) 
        {
         StreamConstPtr("APDS Init Fail\r\n").sendAll(Serial);
         delay(1000);
        }
    }
    else
    {
        apds.enableProximity(true);
        apds.enableGesture(true);
        StreamConstPtr("APDS Init OK\r\n").sendAll(Serial);
    }
    if(!lox.begin())
    {
        while (1) 
        {
         StreamConstPtr("Lidar Init Fail\r\n").sendAll(Serial);
         delay(1000);
        }
    }
    else
    {
        StreamConstPtr("APDS Init OK\r\n").sendAll(Serial);
    }
    doWakeup();
    delay(1000);
}

void loop()
{
//    StreamConstPtr("Loop\r\n").sendAll(Serial);
//    getDistance();
    getGesture();
//    getButton();
//
//    for (uint8_t ii = 0; ii < EVENT_LAST; ii++)
//    {
//        if (events[ii])
//        {
//            cout << EventNames[ii] << "\r\n";
//            dispatch[state][ii]();
//        }
//    }
//    delay(100);

}
