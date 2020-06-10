#include "Arduino.h"
#include <Wire.h>

#ifdef WIFI
#include "ESP8266WiFi.h"
#include "DNSServer.h"
#include "ESP8266WebServer.h"
#include "WiFiManager.h"
#endif // WIFI

#ifdef WIFI
ESP8266WebServer webServer(80);
void apCallback(WiFiManager * wmgr)
{
//    webServer.begin();
}
#endif // WIFI

void handleRoot()
{
//    int bufi0 = 0;
//    int bufi1 = 0;
    uint8_t buf0[2];
    uint8_t buf1[2];
    int rc;
//    buf0[bufi0++] = 0xb4;
    Serial.println("web server root");
#ifdef WIFI
    Serial.println(webServer.args());
    String theArgs = String("<HTML><BODY>");
    //buf0[0] = webServer.hasArg("m0a") ? atoi(webServer.arg("m0a").c_str()) : 0;
    //buf0[1] = webServer.hasArg("m0b") ? atoi(webServer.arg("m0b").c_str()) : 0;
    //buf1[0] = webServer.hasArg("m1a") ? atoi(webServer.arg("m1a").c_str()) : 0;
    //buf1[1] = webServer.hasArg("m1b") ? atoi(webServer.arg("m1b").c_str()) : 0;
    bool m0a = webServer.hasArg("m0a");
    bool m0b = webServer.hasArg("m0b");
    bool m1a = webServer.hasArg("m1a");
    bool m1b = webServer.hasArg("m1b");
    if (m0a)
    {
        theArgs += " [";
        buf0[0] =  atoi(webServer.arg("m0a").c_str());
        theArgs += String(buf0[0]);
        theArgs += "] ";
    }
    if (m0b)
    {
        theArgs += " [";
        buf0[1] =  atoi(webServer.arg("m0b").c_str());
        theArgs += String(buf0[1]);
        theArgs += "] ";
    }
    if (m1a)
    {
        theArgs += " [";
        buf1[0] =  atoi(webServer.arg("m1a").c_str());
        theArgs += String(buf1[0]);
        theArgs += "] ";
    }
    if (m1b)
    {
        theArgs += " [";
        buf1[1] =  atoi(webServer.arg("m1b").c_str());
        theArgs += String(buf1[1]);
        theArgs += "] ";
    }


//    theArgs += " [";
//    theArgs += String(buf0[0]);
//    theArgs += "][";
//    theArgs += String(buf0[1]);
//    theArgs += "][";
//    theArgs += String(buf1[0]);
//    theArgs += "][";
//    theArgs += String(buf1[1]);
//    theArgs += "] ";
//    Serial.println(theArgs.c_str());

    rc = 1;
    if (m0a & m0b)
    {
//    while (rc)
//    {
        Wire.beginTransmission(0x5a);
        Wire.write(buf0, 2);
        rc = Wire.endTransmission();
        theArgs += " rc0: ";
        theArgs += rc;
 //       if (rc) { delay(100); }
    }
//    }
//    rc = 1;
//    while (rc)
//    {
    if (m1a & m1b)
    {
        Wire.beginTransmission(0x5b);
        Wire.write(buf1, 2);
        rc = Wire.endTransmission();
        theArgs += " rc1: ";
        theArgs += rc;
//        if (rc) { delay(100); }
    }
    theArgs += "</BODY></HTML>";
    webServer.send(200, "text/html", theArgs.c_str());
#endif // WIFI
}

void saveWifiConfig()
{
    // save config
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Init start");
    Wire.begin(2,14);
#ifdef WIFI
    WiFiManager wifiManager;
    wifiManager.setDebugOutput(true);
    Serial.println("autoConnect start");
    wifiManager.autoConnect("strandbeast");
    Serial.println("autoConnect end");
//    wifiManager.startConfigPortal("strandbeast");
    webServer.on("/", handleRoot);
    webServer.begin();
#endif // WIFI
}

void loop()
{
#ifdef WIFI
    webServer.handleClient();
#endif // WIFI
}
