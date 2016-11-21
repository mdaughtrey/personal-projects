#include <ESP8266WiFi.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WebServer.h>
#include <cstdlib>
#include <ctype.h>
#include <effect.h>

#define LED_HEARTBEAT 16
#define NEO_NUMPIXELS 10

const char SSID[] = "tentacle";
Adafruit_NeoPixel np(Adafruit_NeoPixel(NEO_NUMPIXELS, 14, NEO_GRB + NEO_KHZ800));
Effect effect(np, 50);

Servo shaft;
typedef enum
{
    FIGHT_OFF,
    FIGHT_ONE,
    FIGHT_TWO
} FightState;

FightState fightState = FIGHT_OFF;
uint32_t fightMillis;
ESP8266WebServer server(80);
extern const char * page;

void handleRoot()
{
    server.sendContent(page);
//    server.send(200, "text/plain", page);
}

void handleServo()
{
}

void handleFight()
{
    if (FIGHT_OFF == fightState)
    {
        fightState = FIGHT_ONE;
        fightMillis = millis();
        shaft.attach(12);
        shaft.write(0);
    }
    else
    {
        fightState = FIGHT_OFF;
        shaft.detach();
    }
    String jdoc = "{\"fight\":";
    jdoc += FIGHT_OFF == fightState ? '0' : '1';
    jdoc += "}";
    server.send(200, "text/plain", jdoc);
}

void handleColor()
{
    server.send(200, "text/plain", "");
    String color  = server.arg("color");
    char * pp;
    uint32 values = std::strtoul(color.c_str(), &pp, 16);
    effect.color(
        (values >> 16) & 0xff, // green
        (values >> 8) & 0xff,  // red
        values & 0xff);       // blue
}

void handleEffect()
{
    server.send(200, "text/plain", "");
    effect.select(server.arg("id").c_str());
}

void handleBackground()
{
    server.send(200, "text/plain", "");
    String color  = server.arg("color");
    char * pp;
    uint32 values = std::strtoul(color.c_str(), &pp, 16);
    effect.background(
        (values >> 16) & 0xff, // green
        (values >> 8) & 0xff,  // red
        values & 0xff);       // blue
}

void handleNotFound()
{
    server.send(200, "text/plain", "");
}

void handleGetState()
{
    String jdoc = "{\"fight\":";
    jdoc += fightState ? '1' : '0';
    jdoc += "}";
    server.send(200, "text/plain", jdoc);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Init");

    WiFi.mode(WIFI_AP);
    WiFi.softAP("tentacle");
    server.on("/", handleRoot);
    server.on("/fg", handleColor);
    server.on("/bg", handleBackground);
    server.on("/fight", handleFight);
    server.on("/effect", handleEffect);
    server.on("/getstate", handleGetState);
    server.onNotFound(handleNotFound);
    server.begin();
    effect.select("off");
}

void loop()
{
    server.handleClient();
    effect.loop();
    if (millis() - fightMillis > 800)
    {
        switch (fightState)
        {
        case FIGHT_ONE:
            shaft.write(180);
            fightMillis = millis();
            fightState = FIGHT_TWO;
            break;

        case FIGHT_TWO:
            shaft.write(0);
            fightMillis = millis();
            fightState = FIGHT_ONE;
            break;
        }
    }
}
