#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include <WiFiManager.h>
//#define GDB
#ifdef GDB
#include <GDBStub.h>
#endif // GDB
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "Arduino.h"
//#include <list>
#include <algorithm>
#include <cctype>
#include <vector>
#include <string>
//#include <regex>
#include <map>
#include <memory>

const uint8_t LED_COUNT = 8;
const uint8_t LED_PIN = 4;

int16_t param;
WiFiClient client;
WiFiManager wifiManager;
WiFiManagerParameter param_mqttServer("mqttserver", "MQTT Server", "", 64);
WiFiManagerParameter param_mqttPort("mqttport", "MQTT Port", "1883", 8);
//WiFiManagerParameter param_hostname("hostname", "Hostname", "rgb", 16);
std::shared_ptr<Adafruit_MQTT_Client>  mqttClient;
Adafruit_NeoPixel neop(1, LED_PIN, NEO_GRB + NEO_KHZ800);
//std::string hostname = "none";
std::string mqtt_server = "none";
uint16_t mqtt_port = 1883;

uint32_t hsvStart;
uint16_t toHSV;
uint16_t fromHSV;

typedef Adafruit_MQTT_Subscribe Sub;
typedef Adafruit_MQTT_Publish Pub;
typedef std::shared_ptr<Sub> SubPtr;
typedef std::shared_ptr<Pub> PubPtr;

typedef struct
{
    char sref[32];
    char subtopic[32];
    SubPtr sub;
    void (*subfun) (Sub*);

    char pref[32];
    char pubtopic[32];
    PubPtr pub;
    void (*pubfun) (Pub*, char *);
}PubSubTuple; 

void verbose(const char * fmt, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, 255, fmt, args);
    va_end(args);
    Serial.print(buffer);
}

PubSubTuple pubSubs[] = {
    {"led", "", SubPtr(), [] (Sub * sub) { digitalWrite(2, '0' == *(sub->lastread) ? HIGH: LOW); },
     "ledstatus", "", PubPtr(), [] (Pub * pub, char * data) {}},

     //{"ping", "", SubPtr(), [] (Sub * sub) { neop.setPixelColor(0, random(-1)); neop.show(); },
     {"ping", "", SubPtr(), [] (Sub * sub) {
         uint32_t c = std::stol((char *)sub->lastread);
         toHSV = c >> 16;
         verbose("toHSV %u fromHSV %u\r\n", toHSV, fromHSV);
//         verbose("Ping HSV %08x\r\n", neop.ColorHSV((c >>  16) & 0xffff, (c >> 8) & 0xff, c & 0xff));
//         neop.setPixelColor(0, neop.ColorHSV((c >>  16) & 0xffff, (c >> 8) & 0xff, c & 0xff));
//         neop.show();
         },
     "nul", "", PubPtr(), [] (Pub * pub, char * data) {}}
};


#define NUMSUBS (sizeof(pubSubs)/sizeof(pubSubs[0]))

void buildTopics()
{
    verbose("buildTopics\r\n");
    PubSubTuple * psp(pubSubs);
    for (uint8_t ii = 0; ii < NUMSUBS; ii++, psp++)
    {
        snprintf(psp->subtopic, 32, "/%s/%s", WiFi.hostname().c_str(), psp->sref);
        psp->sub.reset(new Adafruit_MQTT_Subscribe(mqttClient.get(), psp->subtopic));
        mqttClient->subscribe(psp->sub.get());
        verbose("Sub Topic %s\r\n", psp->subtopic);

        snprintf(psp->pubtopic, 32, "/%s/%s", WiFi.hostname().c_str(), psp->pref);
        verbose("Pub Topic %s\r\n", psp->pubtopic);
    }
}


typedef std::map<std::string, std::string> Creds;
typedef Creds::iterator CredsIter;
Creds creds;

void serialize()
{
    verbose("serialize\r\n");
    std::vector<uint8_t> serout;
//    serout.push_back(WiFi.hostname().length());
//    std::string tmp(WiFi.hostname().c_str());
//    serout.insert(serout.end(), tmp.begin(), tmp.end());
	serout.push_back(mqtt_server.size());
    serout.insert(serout.end(), mqtt_server.begin(), mqtt_server.end());
	serout.push_back(mqtt_port >> 8);
	serout.push_back(mqtt_port & 0xff);
     
    if (creds.empty())
    {
        serout.push_back(0);
    }
    else
    {
        for (auto && [f, s]: creds)
        {
            serout.push_back((uint8_t)f.size());
            serout.insert(serout.end(), f.begin(), f.end());
            serout.push_back((uint8_t)s.size());
            serout.insert(serout.end(), s.begin(), s.end());
        }
    }
    verbose("Serialized to %d bytes\r\n", serout.size());
    EEPROM.write(0, serout.size() >> 8);
    EEPROM.write(1, serout.size() & 0xff);
	int addr(2);
	for (auto iter: serout)
	{
		EEPROM.write(addr++, iter);
	}
    EEPROM.commit();
}

void deserialize()
{
    verbose("deserialize\r\n");
    std::vector<uint8_t> serin;
    std::string first("");
    std::string second("");

    int size(EEPROM.read(0) << 8 | EEPROM.read(1));
    verbose("Size %d\r\n", size);
    if (size > 4096 || size < 0)
    {
        verbose("Store is corrupt, clear and reset\r\n");
        EEPROM.write(0, 0);
        EEPROM.write(1, 0);
        EEPROM.commit();
        wifiManager.erase();
        while (1);
    }
    if (0 == size)
    {
        verbose("Cred store is empty\r\n");
        return;
    }
    int ii;
    for (ii = 0; ii < size; ii++)
    {
        serin.push_back(EEPROM.read(2+ii));
     //   verbose("%04d: %02x\r\n", 2+ii, serin.back());
    }
    ii = 0;
 	auto iter = serin.begin();
	size = *iter++;
	// hostname
//    hostname.assign(iter, iter + size);
//	iter += size;

	// mqtt_server
	size = *iter++;
    mqtt_server.assign(iter, iter + size);
	iter += size;

	// mqtt_port
	mqtt_port = (*iter++) << 8;
	mqtt_port |= *iter++;

    while (iter != serin.end())
    {
		size = *iter++;
        if (first.empty())
        {
            first.assign(iter, iter + size);
            verbose("first %s\r\n", first.c_str());
        }
        else if (second.empty())
        {
            second.assign(iter, iter + size);
            verbose("second %s\r\n", second.c_str());
            creds[first] = second;
            first.clear();
            second.clear();
        }
		iter += size;
    }
}

std::string makeHostname()
{
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char buffer[64];
    snprintf(buffer, 64, "WG%02X%02X%02X%02X%02X%02X",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return std::string(buffer);
}

void setup()
{
    delay(10000);
    Serial.begin(115200);
#ifdef GDB
    gdbstub_init();
    delay(5000);
#endif // GDB
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
    EEPROM.begin(4096);
//    deserialize();
    neop.begin();
    neop.clear();
    neop.setBrightness(255);

    neop.setPixelColor(0, neop.Color(127, 0, 0));
    neop.show();
//    connect();
}

IPAddress addr;

void scanAndConnect()
{
    verbose("scanAndConnect\r\n");
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
            WiFi.hostname(makeHostname().c_str());
//            if (!hostname.empty())
//            {
//                verbose("Setting hostname %s\r\n", hostname.c_str());
//                WiFi.hostname(hostname.c_str());
//            }
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

void doWifiManager()
{
    verbose("doWifiManager\r\n");
    wifiManager.addParameter(&param_mqttServer);
    wifiManager.addParameter(&param_mqttPort);
//    param_hostname.setValue(hostname.c_str(), hostname.length());
//    wifiManager.addParameter(&param_hostname);
    //verbose("hostname pre wifi manager is %s\r\n", hostname.c_str());
    wifiManager.autoConnect(makeHostname().c_str());
    if (WiFi.isConnected())
    {
        mqtt_server = param_mqttServer.getValue();
        std::transform(mqtt_server.begin(), mqtt_server.end(), mqtt_server.begin(),
            [](unsigned char c){ return std::tolower(c); });
//        hostname = param_hostname.getValue();
//        std::transform(hostname.begin(), hostname.end(), hostname.begin(), 
//            [](unsigned char c){ return std::tolower(c); });
        mqtt_port = std::stol(param_mqttPort.getValue());
        verbose("Connected via WiFi creds to %s\r\n", WiFi.SSID());
        serialize();
    }
}

void connect()
{
    verbose("connect\r\n");
    uint8_t rc = WL_IDLE_STATUS;
    WiFi.begin();
    uint8_t retries = 10;
    while (rc != WL_CONNECTED && rc != WL_CONNECT_FAILED && retries--)
    {
        rc = WiFi.status();
        verbose("WiFi.begin returns %d\r\n", rc);
        delay(500);
    }
    if (WiFi.isConnected())
    {
        WiFi.hostname(makeHostname().c_str());
        verbose("Connected via WiFi creds to %s\r\n", WiFi.SSID());
        return;
    }
    neop.setPixelColor(0, neop.Color(127, 127, 0));
    neop.show();
    // Try scanning other networks we know about
    scanAndConnect();
    if (WiFi.isConnected())
    {
        WiFi.hostname(makeHostname().c_str());
        verbose("Connected via stored creds to %s\r\n", WiFi.SSID());
        return;
    }
    neop.setPixelColor(0, neop.Color(127, 0, 127));
    neop.show();

    // Try WPS
    verbose("Connecting via WPS\r\n");
    rc = WiFi.beginWPSConfig();
    //verbose("WPSConfig rc %d, connecting to %s\r\n", rc, WiFi.SSID());
    if (WiFi.SSID() == "")
    {
        verbose("Timed out, enabling config portal");
        doWifiManager();
        WiFi.hostname(makeHostname().c_str());
        return;
    }
    neop.setPixelColor(0, neop.Color(0, 127, 127));
    neop.show();

    verbose("Waiting on PSK\r\n");
    unsigned long start = millis();
    while ((WiFi.psk() == "") && 10000 > (millis() - start))
    {
        yield();
    }
    if (WiFi.psk() == "")
    {
        verbose("WPS timeout out, starting WiFi manager\r\n");
        doWifiManager();
        neop.setPixelColor(0, neop.Color(0, 0, 0));
        neop.show();
    }
    verbose("Connected via WPS to %s, running config portal\r\n", WiFi.SSID());
//    WiFi.hostname(makeHostname().c_str());
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

void dumpTopics()
{
    verbose("dumpTopics:\r\n");
    for (uint8_t ii = 0; ii < NUMSUBS; ii++)
    {
        verbose("Sub topic %s \r\n", pubSubs[ii].subtopic);
        verbose("Pub topic %s \r\n", pubSubs[ii].pubtopic);
    }
}
void mqttConnect()
{
    verbose("mqttConnect\r\n");
    int8_t ret;
    mqttClient.reset(new Adafruit_MQTT_Client(&client, mqtt_server.c_str(), mqtt_port));
    buildTopics();

    uint8_t retries = 3;
    while ((ret = mqttClient->connect()) != 0) { // connect will return 0 for connected
        verbose("Failed %s\r\n", (const char * )mqttClient->connectErrorString(ret));
        verbose("Retrying MQTT connection in 5 seconds...\r\n");
        mqttClient->disconnect();
        delay(3000);  // wait 5 seconds
        retries--;
        if (retries == 0) {
            verbose("Looks like a fail\r\n");
            return;
        }
    }
    verbose("MQTT Connected!\r\n");
    mqttClient->setKeepAliveInterval(5);
}

typedef struct
{
    char key;
    char help[24];
    void (*fun)();
}Command;

Command cmds[] = {
    {'-', "Clear param", [](){ param = 0; }},
    {'a', "Save SSID/PSK to store", [](){ creds[WiFi.SSID().c_str()] = WiFi.psk().c_str(); serialize();}},
    {'A', "Load SSIDS from store", [](){ deserialize(); }},
    {'C', "Clear store", [](){ EEPROM.write(0, 0); EEPROM.write(1, 0); EEPROM.commit(); wifiManager.erase();}},
    {'d', "LED on", [](){ digitalWrite(2, LOW); }},
    {'D', "LED off", [](){ digitalWrite(2, HIGH); }},
    {'h', "Help", [](){ help(); listStoredNetworks(); }},
    {'l', "Ping", [](){ unsigned long r = random(-1);
        verbose("Ping random %08x\r\n", r);
        neop.setPixelColor(0, r);
        neop.show();
        }
    },
    {'m', "Connect to MQTT", [](){ mqttConnect(); }},
    {'M', "Disconnect from MQTT", [](){ mqttClient->disconnect(); }},
    {'n', "Scan and connect", [](){  connect(); }},
    {'r', "Reset", [](){  verbose("Resetting...\r\n"); while(1); }},
};

#define NUMCOMMANDS (sizeof(cmds)/sizeof(cmds[0]))

void help()
{
    for (uint8_t ii = 0; ii < NUMCOMMANDS; ii++)
    {
        verbose("%c: %s\r\n", cmds[ii].key, cmds[ii].help);
    }

//    verbose("Internal hostname %s\r\n", hostname.c_str());
    verbose("WiFi connected %d\r\n", (WiFi.isConnected()));
    verbose("Hostname: %s\r\nSSID: %s\r\nPSK: %s\r\n", WiFi.hostname().c_str(),
        WiFi.SSID().c_str(),
        WiFi.psk().c_str());
	verbose("MQTT_Server %s port %d\r\n", mqtt_server.c_str(), mqtt_port);
//	verbose("fxmode %u\r\n", fxMode);
    dumpTopics();
    verbose("fromHSV %u toHSV %u\r\n", fromHSV, toHSV);
}

void handleCommand()
{
	uint8_t lastCommand;
    lastCommand = Serial.read();

    for (uint8_t ii = 0; ii < NUMCOMMANDS; ii++)
    {
        if (lastCommand == cmds[ii].key)
        {
            cmds[ii].fun();
            return;
        }
    }

    if (lastCommand >= '0' & lastCommand <= '9')
    {
        param *= 10;
        param += lastCommand - '0';
    }
}

void breakit()
{
    verbose("Broke!\r\n");
}

void mqttPing()
{
    if (!WiFi.isConnected()) return;
    if (!mqttClient) 
    {
        mqttConnect();
    }
	if (!mqttClient->connected())
    {
        mqttConnect();
    }
	if (!mqttClient->connected())
    {
        return;
    }
    Adafruit_MQTT_Subscribe * sub;
    while (sub = mqttClient->readSubscription(1))
    {
        for (uint8_t ii = 0; ii < NUMSUBS; ii++)
        {
            if (sub == pubSubs[ii].sub.get())
            {
                pubSubs[ii].subfun(pubSubs[ii].sub.get());
                pubSubs[ii].pubfun(pubSubs[ii].pub.get(), (char *)sub->lastread);
            }
        }
	}
}

void ledPing()
{
    uint8_t ii;
    if (fromHSV == toHSV)
    {
        return;
    }
    if (millis() - hsvStart)
    {
        if (fromHSV < toHSV)
        {
            for (ii = 0; ii < 30 && fromHSV != toHSV; ii++)
            {
                fromHSV++;
            }
        }
        else
        {
            for (ii = 0; ii < 30 && fromHSV != toHSV; ii++)
            {
                fromHSV--;
            }
        }
        hsvStart = millis();
        neop.setPixelColor(0, neop.ColorHSV(fromHSV, 0xff, 0xff));
        neop.show();
        if (fromHSV == toHSV)
        {
            verbose("fromHSV == toHSV\r\n");
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
    ledPing();
}
