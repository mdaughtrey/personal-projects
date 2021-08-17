#include <Adafruit_APDS9960.h>
#include <ostream>
#include <StreamDev.h>
#include <StreamString.h>

using namespace std;
typedef void (*DoFunc)(void);

Adafruit_APDS9960 apds;

typedef enum {
	ASLEEP, IDLE, MOVING_UP, MOVING_DOWN, STATE_LAST
} State;

typedef enum {
    BUTTON, UP, DOWN, GESTURE, FAR, NEAR, TIMER, EVENT_LAST
} Event;


Event events[EVENT_LAST] = {0};
State state = IDLE;
uint8_t resetEvent = 0;
StreamString cout(Serial);

void ignore(State state)
{
    StreamConstPtr("ignore").sendAll(Serial);
    resetEvent = 1;
}

 
void doWakeup(State state)
{
    StreamConstPtr("doWakeup").sendAll(Serial);
    //# start Wifi
    //# Get time
//    setLocalTime()
    state = IDLE;
    resetEvent = 0;
}

void doGetTime(State state)
{
    StreamConstPtr("doGetTime").sendAll(Serial);
//    setLocalTime()
    state = IDLE;
    resetEvent = 0;
}
 
void doMoveUp(State state)
{
    StreamConstPtr("doMoveUp").sendAll(Serial);
    state = MOVING_UP;
    resetEvent = 0;
}

void doMoveDown(State state)
{
    StreamConstPtr("doMoveDown").sendAll(Serial);
    state = MOVING_DOWN;
    resetEvent = 0;
}

void doIdle(State state)
{
    StreamConstPtr("doIdle").sendAll(Serial);
    state = IDLE;
    resetEvent = 0;
}

void getDistance(void)
{
}

void getGesture(void)
{
    uint8_t gesture = apds.readGesture();
    if(gesture == APDS9960_DOWN) events[DOWN] = 1;
    if(gesture == APDS9960_UP) events[UP] = 1;
    if(gesture == APDS9960_LEFT) events[LEFT] = 1;
    if(gesture == APDS9960_RIGHT) events[RIGHT] = 1;
}

void getButton(void)
{
}

// # Dispatch Table

/DoFunc[EVENT_LAST][STATE_LAST] dispatch = {
DoFunc dispatch[] = {
    doWakeup, ignore,   ignore,     ignore, ignore, ignore, doGetTime,  //  ASLEEP
    ignore,   doMoveUp, doMoveDown, ignore, ignore, ignore, doGetTime,  //  IDLE
    doIdle,   doIdle,   doIdle,     doIdle, doIdle, doIdle, ignore,     //  MOVING_UP
    doIdle,   doIdle,   doIdle,     doIdle, doIdle, doIdle, ignore};    //  MOVING_DOWN
//  BUTTON    UP        DOWN        GESTURE FAR     NEAR    TIMER


// dispatch=(
//     (doWakeup, ignore,   ignore,     ignore, ignore, ignore, doGetTime),  # ASLEEP
//     (ignore,   doMoveUp, doMoveDown, ignore, ignore, ignore, doGetTime),  # IDLE
//     (doIdle,   doIdle,   doIdle,     doIdle, doIdle, doIdle, ignore),     # MOVING_UP
//     (doIdle,   doIdle,   doIdle,     doIdle, doIdle, doIdle, ignore))     # MOVING_DOWN
// #    BUTTON    UP        DOWN        GESTURE FAR     NEAR    TIMER



void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    if(!apds.begin())
    {
        Serial << "APDS Init Failes";	
    }
    else
    {
//        apds.enableProximity(true);
        apds.enableGesture(true);
        Serial << "APDS Init OK";
    }
}

void loop()
{
}
