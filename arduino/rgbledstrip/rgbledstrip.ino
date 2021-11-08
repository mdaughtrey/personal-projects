#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WS2812FX.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "Arduino.h"
#include <list>
#include <vector>
#include <string>
#include <map>

//const char MQTT_SERVER[] = "DESKTOP-TOJK787";
//const char MQTT_SERVER[] = "pop-os";
const char MQTT_SERVER[] = "mqtt.local";
const int MQTT_PORT = 1883;
const uint8_t LED_COUNT = 8;
const uint8_t LED_PIN = 4;

int16_t param;
//bool textInput;
//std::string textParam;
//std::string ssid;
//std::string psk;

WiFiClient client;
WS2812FX fx = WS2812FX(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT);

typedef Adafruit_MQTT_Subscribe Sub;
typedef std::pair<Sub *, void (*)(Sub *) > SubPair; 

auto subs = {
    SubPair(new Adafruit_MQTT_Subscribe(&mqtt, "/led"),
        [] (Sub * sub) { digitalWrite(2, '0' == *(sub->lastread) ? LOW: HIGH); }),
    SubPair(new Adafruit_MQTT_Subscribe(&mqtt, "/leds"),
        [] (Sub * sub) { fx.setMode(String((const char *)sub->lastread).toInt());}),
    SubPair(new Adafruit_MQTT_Subscribe(&mqtt, "/speed"),
        [] (Sub * sub) { fx.setSpeed(String((const char *)sub->lastread).toInt()); }),
    SubPair(new Adafruit_MQTT_Subscribe(&mqtt, "/bright"),
        [] (Sub * sub) { fx.setBrightness(String((const char *)sub->lastread).toInt()); })
};

void verbose(const char * fmt, ...)
{
//    if (threshold > config.verbose) return;
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    Serial.print(buffer);
}

//typedef std::pair<std::string, std::string > CredPair;
typedef std::map<std::string, std::string> Creds;
//typedef std::vector<std::pair<std::vector<char>, std::vector<char> > > Creds;
typedef Creds::iterator CredsIter;
Creds creds;

void serialize()
{
    if (creds.empty())
    {
        return;
    }
    int size = 0;
    for (const auto & ii: creds)
    {
        size += ii.first.length() + ii.second.length();
    }

    if (size > 4000)
    {
        verbose("Cred size %d vs max 4096\r\n", size);
        return;
    }
    std::vector<uint8_t> serout;
    for (auto && [f, s]: creds)
    {
        serout.push_back((uint8_t)f.size());
        serout.insert(serout.end(), f.begin(), f.end());
        serout.push_back((uint8_t)s.size());
        serout.insert(serout.end(), s.begin(), s.end());
    }
    verbose("Serialized to %d bytes\r\n", serout.size());
    EEPROM.write(0, serout.size() >> 8);
    EEPROM.write(1, serout.size() & 0xff);
//    for (int addr = 0; addr < serout.size(); addr++)
//    {
//        verbose("A %04x D %02X\r\n", addr, serout[addr]);
//        EEPROM.write(2 + addr, serout[addr]);
//    }
    EEPROM.commit();
}

void deserialize()
{
    std::vector<uint8_t> serin;
    std::string first("");
    std::string second("");

    int size(EEPROM.read(0) << 8 | EEPROM.read(1));
    verbose("Size %d\r\n", size);
    if (0 == size)
    {
        verbose("Cred store is empty\r\n");
        return;
    }
    int ii;
    for (ii = 0; ii < size; ii++)
    {
        serin.push_back(EEPROM.read(2+ii));
    }
    ii = 0;
    while (size = serin[ii])
    {
        ii++;
        if (first.empty())
        {
            first.assign(serin.begin() + ii, serin.begin() + ii + size);
//            verbose("first %s\r\n", first.c_str());
        }
        else if (second.empty())
        {
            second.assign(serin.begin() + ii, serin.begin() + ii + size);
//            verbose("second %s\r\n", second.c_str());
            creds[first] = second;
            first.clear();
            second.clear();
        }
        ii += size;
    }
}

void setup()
{
    Serial.begin(115200);
    //Serial.setDebugOutput(true);
    pinMode(2, OUTPUT);

    fx.init();
    fx.setBrightness(100);
    fx.setSpeed(10);
    fx.setMode(FX_MODE_RAINBOW_CYCLE);
    fx.start();

    EEPROM.begin(4096);
//    textParam.clear();
//    textInput = false;

    delay(2000);
	verbose("Setup %s\r\n");
 //   Serial.println(__cplusplus);
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  verbose("Connecting to MQTT...\r\n");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       verbose("%s\r\n", (const char * )mqtt.connectErrorString(ret));
       verbose("Retrying MQTT connection in 5 seconds...\r\n");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
		 verbose("Looks like a fail\r\n");
		
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  mqtt.setKeepAliveInterval(5);
  verbose("MQTT Connected!\r\n");
}

IPAddress addr;
// std::string ssid;
// std::string psk;
// std::string bssid;


void scanAndConnect()
{
    int8_t rc;
    rc = WiFi.scanNetworks();
    verbose("scanNetworks rc %d\r\n", rc);
    if (-1 == rc)
    {
        verbose("Scan failed\r\n");
        return;
    }
    verbose("%d networks found\r\n", rc);
    for (auto ii = 0; ii < rc; ii++)
    {
        verbose("%d: SSI: %s RSSI \r\n", ii, WiFi.SSID(ii), WiFi.RSSI(ii));
        auto iter = creds.find(WiFi.SSID(ii).c_str());
        if (iter != creds.end())
        {
            verbose("Matched on %s/%s, connecting\r\n", iter->first.c_str(),
                    iter->second.c_str());
            WiFi.begin(iter->first.c_str(), iter->second.c_str());
            while (WiFi.status() != WL_CONNECTED)
            {
                yield();
            }
            verbose("Connected as %s\r\n", WiFi.localIP().toString().c_str());
            return;
        }
    }
}

void connect()
{
    int8_t rc;
    scanAndConnect();
    if (WiFi.isConnected())
    {
        verbose("Connected to %s\r\n", WiFi.SSID());
        return;
    }

    verbose("Connecting via WPS\r\n");
    rc = WiFi.beginWPSConfig();
    verbose("WPSConfig rc %d, connecting to %s\r\n", rc, WiFi.SSID());
    if (WiFi.SSID() == "")
    {
        verbose("Timed out");
        return;
    }

    verbose("Waiting on PSK\r\n");
    while (WiFi.psk() == "")
    {
        yield();
    }

    verbose("\r\nConnected as %s to %s, psk %s\r\n",
        WiFi.localIP().toString(), WiFi.SSID().c_str(), WiFi.psk().c_str());
}

void listStoredNetworks()
{
    verbose("%d Stored Networks:\r\n", creds.size());
    for (auto && [f, s]: creds)
    {
        verbose("SSID: %s ", f.c_str());
        verbose("PSK: %s\r\n", s.c_str());
    }
}

void mqttConnect()
{
    verbose("MQTT_Connect\r\n");
    MQTT_connect();
    for (auto && [first,second]: subs)
    {
        int rc;
        rc = mqtt.subscribe(first);
        verbose("subscribe rc %d\r\n", rc);
    }
}

typedef std::pair<std::string, void (*)() > CmdPair;
typedef std::map<char, CmdPair> Cmds;
typedef Cmds::iterator CmdsIter;

Cmds cmds = {
    {'-', CmdPair("Clear param", [](){ param = 0; })},
    {'a', CmdPair("Save SSID/PSK to store", [](){ creds[WiFi.SSID().c_str()] = WiFi.psk().c_str(); serialize();})},
    {'A', CmdPair("Load SSIDS from store", [](){ deserialize(); })},
    {'c', CmdPair("Clear store", [](){ EEPROM.write(0, 0); EEPROM.write(1, 0); EEPROM.commit();})},
    {'d', CmdPair("LED on", [](){ digitalWrite(2, LOW); })},
    {'D', CmdPair("LED off", [](){ digitalWrite(2, HIGH); })},
    {'e', CmdPair("Set FX Speed", [](){ fx.setSpeed(param); })},
    {'f', CmdPair("Set FX Mode", [](){ fx.setMode(param); })},
    {'h', CmdPair("Help", [](){ help(); })},
    {'l', CmdPair("List stored networks", [](){ listStoredNetworks(); })},
    {'m', CmdPair("Connect to MQTT", [](){ mqttConnect(); })},
    {'M', CmdPair("Disconnect from MQTT", [](){ mqtt.disconnect(); })},
    {'n', CmdPair("Scan and connect", [](){  connect(); })},
    {'r', CmdPair("Reset", [](){  while(1); })}
};

void help()
{
    for (auto && [first, second]: cmds)
    {
        verbose("%c: %s\r\n", first, second.first.c_str());
    }

    if (WiFi.isConnected())
    {
        verbose("SSID: %s\r\nPSK: %s\r\n", WiFi.SSID().c_str(), WiFi.psk().c_str());
    }
}

void handleCommand()
{
	uint8_t lastCommand;
    int rc;
    lastCommand = Serial.read();
    CmdsIter iter = cmds.find(lastCommand);
    if (iter != cmds.end())
    {
        iter->second.second();
        return;
    }

    if (lastCommand >= '0' & lastCommand <= '9')
    {
        param *= 10;
        param += lastCommand - '0';
    }
}

void mqttPing()
{
	if (!mqtt.connected()) return;
    Adafruit_MQTT_Subscribe * sub;
    while (sub = mqtt.readSubscription(1))
    {
        for (auto && [first, second]: subs)
        {
            if (sub == first)
            {
                second(first);
            }
        }
	}
}

void loop()
{

	if (Serial.available())
	{
		handleCommand();
	}
    mqttPing();
    fx.service();
}
