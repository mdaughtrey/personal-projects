#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WS2812FX.h>
#include <ESP8266WebServer.h>
#ifdef GDB
#include <GDBStub.h>
#endif // GDB
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "Arduino.h"
#include <list>
#include <vector>
#include <string>
#include <regex>
#include <map>
#include <memory>

const char * configpage="<!DOCTYPE html><html><head><title>{{hostname}} Configuration</title></head><body> <h2>{{hostname}} Configuration</h2><form action=\"/setconfig\"><table><tr><td><label for=\"hname\">Hostname:</label></td><td><input type=\"text\" id=\"hname\" name=\"hname\" value=\"{{hostname}}\"><br></td></tr><tr><td><label for=\"sname\">MQTT Server:</label></td><td><input type=\"text\" id=\"sname\" name=\"sname\" value=\"{{mqtt_server}}\"><br></td></tr><tr><td><label for=\"mport\">MQTT Port:</label></td><td><input type=\"numeric\" id=\"mport\" name=\"mport\" value=\"{{mqtt_port}}\"><br></td></tr></table><input type=\"submit\" value=\"Submit\"></form><p>The device will restart on submit.</p></body></html>";
const uint8_t LED_COUNT = 8;
const uint8_t LED_PIN = 4;

int16_t param;
WiFiClient client;
WS2812FX fx = WS2812FX(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);
std::shared_ptr<Adafruit_MQTT_Client>  mqttClient;
ESP8266WebServer server(80);
std::string hostname = "none";
std::string mqtt_server = "none";
uint16_t mqtt_port = 1883;
uint8_t fxMode = 0;
uint8_t fxRed = 0;
uint8_t fxGreen = 0;
uint8_t fxBlue = 0;
bool webactive;

typedef Adafruit_MQTT_Subscribe Sub;
typedef Adafruit_MQTT_Publish Pub;
typedef std::shared_ptr<Sub> SubPtr;
typedef std::shared_ptr<Pub> PubPtr;
//typedef std::pair<SubPtr, void (*)(Sub *) > SubPair; 
typedef std::tuple<std::string, SubPtr, void (*)(Sub *) > SubTuple;
typedef std::tuple<std::string, SubPtr, void (*)(Sub *),
    PubPtr, void (*)(Pub *, char *) > PubSubTuple;

void verbose(const char * fmt, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, 255, fmt, args);
    va_end(args);
    Serial.print(buffer);
}

enum {
    TOPIC = 0,
    SUB,
    SUBFUN,
    PUB,
    PUBFUN
};

std::vector<PubSubTuple> pubSubs = {
    PubSubTuple(std::string("led"),
        SubPtr(),
        [] (Sub * sub) { digitalWrite(2, '0' == *(sub->lastread) ? LOW: HIGH); },
        PubPtr(),
        [] (Pub * pub, char * data) {}),

    PubSubTuple(std::string("fx"),
        SubPtr(),
        [] (Sub * sub) { fx.setMode(String((const char *)sub->lastread).toInt());},
        PubPtr(),
        [] (Pub * pub, char * data) {}),

    PubSubTuple(std::string("speed"),
        SubPtr(),
        [] (Sub * sub) { fx.setSpeed(String((const char *)sub->lastread).toInt()); },
        PubPtr(),
        [] (Pub * pub, char * data) {}),

    PubSubTuple(std::string("bright"),
        SubPtr(),
        [] (Sub * sub) { fx.setBrightness(String((const char *)sub->lastread).toInt()); },
        PubPtr(),
        [] (Pub * pub, char * data) {}),

    PubSubTuple(std::string("color"),
        SubPtr(),
        [] (Sub * sub) { 
            verbose("setColor %s\r\n", (const char *)sub->lastread);
            verbose("setColor %lu\r\n", std::stol((const char *)sub->lastread + 1, nullptr, 16)); 
            uint32_t val = std::stol((const char *)sub->lastread + 1, nullptr, 16);
            fx.setColor(val << 16, val << 24, val & 0xff);},
         PubPtr(),
         [] (Pub * pub, char * data) {}),

    PubSubTuple(std::string("fxprev"),
        SubPtr(),
        [] (Sub * sub) { fx.setMode(fx.getMode() + fx.getModeCount() - 1 % fx.getModeCount());},
        PubPtr(),
        [] (Pub * pub, char * data) {}),

    PubSubTuple(std::string("fxnext"),
        SubPtr(),
        [] (Sub * sub) { fx.setMode(fx.getMode() + 1 % fx.getModeCount());},
        PubPtr(),
        [] (Pub * pub, char * data) {}),

    PubSubTuple(std::string("green"),
        SubPtr(),
        [] (Sub * sub) { verbose("Red %s\r\n", sub->lastread);
            fx.setColor((*fx.getColors(0)) & 0x0000ffff | 
                std::stoi((char *)sub->lastread) << 16);
            verbose("getColor %x\r\n", *fx.getColors(0)); }, 
        PubPtr(),
        [] (Pub * pub, char * data) {}),

    PubSubTuple(std::string("red"),
        SubPtr(),
        [] (Sub * sub) { verbose("Green %s\r\n", sub->lastread); 
            fx.setColor((*fx.getColors(0)) & 0x00ff00ff | 
                std::stoi((char *)sub->lastread) << 8); 
            verbose("getColor %x\r\n", *fx.getColors(0)); }, 
        PubPtr(),
        [] (Pub * pub, char * data) {}),

    PubSubTuple(std::string("blue"),
        SubPtr(),
        [] (Sub * sub) { verbose("Blue %s\r\n", sub->lastread); 
            fx.setColor((*fx.getColors(0)) & 0x00ffff00 | 
                std::stoi((char *)sub->lastread)), 
            verbose("getColor %x\r\n", *fx.getColors(0)); }, 
        PubPtr(),
        [] (Pub * pub, char * data) {}),
};

//std::vector<SubTuple> subs = {
//    SubTuple(std::string("led"), SubPtr(), [] (Sub * sub) { digitalWrite(2, '0' == *(sub->lastread) ? LOW: HIGH); }),
//    SubTuple(std::string("leds"), SubPtr(), [] (Sub * sub) { fx.setMode(String((const char *)sub->lastread).toInt());}),
//    SubTuple(std::string("speed"), SubPtr(), [] (Sub * sub) { fx.setSpeed(String((const char *)sub->lastread).toInt()); }),
//    SubTuple(std::string("bright"), SubPtr(), [] (Sub * sub) { fx.setBrightness(String((const char *)sub->lastread).toInt()); }),
//    SubTuple(std::string("color"), SubPtr(), [] (Sub * sub) { 
//        verbose("setColor %s\r\n", (const char *)sub->lastread);
//        verbose("setColor %lu\r\n", std::stol((const char *)sub->lastread + 1, nullptr, 16)); 
//        uint32_t val = std::stol((const char *)sub->lastread + 1, nullptr, 16);
//        fx.setColor(val << 16, val << 24, val & 0xff);})
//};


void buildTopics()
{
    char buffer[128];
    for (auto iter = pubSubs.begin(); iter != pubSubs.end(); iter++)
    {
        snprintf(buffer, 128, "/%s/%s", WiFi.hostname().c_str(),
            std::get<TOPIC>(*iter).c_str());
        std::get<TOPIC>(*iter).assign(buffer);
        std::get<SUB>(*iter).reset(new Adafruit_MQTT_Subscribe(mqttClient.get(),
            std::get<TOPIC>(*iter).c_str()));
        mqttClient->subscribe(std::get<SUB>(*iter).get());
        std::get<PUB>(*iter).reset(new Adafruit_MQTT_Publish(mqttClient.get(),
            std::get<TOPIC>(*iter).c_str()));
        verbose("Topic %s\r\n", std::get<TOPIC>(*iter).c_str());
    }
}

//void buildTopics0()
//{
//    char buffer[128];
//    for (auto iter = subs.begin(); iter != subs.end(); iter++)
//    {
//        snprintf(buffer, 128, "/%s/%s", WiFi.hostname().c_str(), std::get<0>(*iter).c_str());
//        verbose("Buffer topic is %s\r\n", buffer);
//        std::get<0>(*iter).assign(buffer);
//        verbose("stack buffer address %x subs buffer address %x\r\n", buffer, std::get<0>(*iter).data());
//        std::get<1>(*iter).reset(new Adafruit_MQTT_Subscribe(mqttClient.get(), std::get<0>(*iter).c_str()));
//        mqttClient->subscribe(std::get<1>(*iter).get());
//        verbose("subs topic is %s\r\n", std::get<0>(*iter).c_str());
//    }
//    verbose("subs0 topic is %s\r\n", std::get<0>(subs[0]).c_str());
//}


typedef std::map<std::string, std::string> Creds;
typedef Creds::iterator CredsIter;
Creds creds;

void serialize()
{
    std::vector<uint8_t> serout;
    serout.push_back(WiFi.hostname().length());
    std::string tmp(WiFi.hostname().c_str());
    serout.insert(serout.end(), tmp.begin(), tmp.end());
	serout.push_back(mqtt_server.size());
    serout.insert(serout.end(), mqtt_server.begin(), mqtt_server.end());
	serout.push_back(mqtt_port >> 8);
	serout.push_back(mqtt_port & 0xff);
	serout.push_back(fxMode >> 8);
	serout.push_back(fxMode & 0xff);
     
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
//    for (auto iter: serout)
//    {
//        verbose("%02x ", iter);
//    }
//    verbose("\r\n");
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
    std::vector<uint8_t> serin;
    std::string first("");
    std::string second("");

	hostname = "";
	mqtt_server = "";

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
     //   verbose("%04d: %02x\r\n", 2+ii, serin.back());
    }
    ii = 0;
 	auto iter = serin.begin();
	size = *iter++;
	// hostname
    hostname.assign(iter, iter + size);
	iter += size;

	// mqtt_server
	size = *iter++;
    mqtt_server.assign(iter, iter + size);
	iter += size;

	// mqtt_port
	mqtt_port = (*iter++) << 8;
	mqtt_port |= *iter++;

	// fxMode
	fxMode = (*iter++) << 8;
	fxMode |= *iter++;

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


void findAndReplaceAll(std::string & data, std::string toSearch, std::string replaceStr)
{
    // Get the first occurrence
    size_t pos = data.find(toSearch);
    // Repeat till end is reached
    while( pos != std::string::npos)
    {
        // Replace this occurrence of Sub String
        data.replace(pos, toSearch.size(), replaceStr);
        // Get the next occurrence from the current position
        pos =data.find(toSearch, pos + replaceStr.size());
    }
}

std::string pageFixup(std::string html)
{
     //findAndReplaceAll(html, "{{hostname}}", hostname);
     findAndReplaceAll(html, "{{hostname}}", WiFi.hostname().c_str());
     findAndReplaceAll(html, "{{mqtt_server}}", mqtt_server);
     findAndReplaceAll(html, "{{mqtt_port}}", std::to_string(mqtt_port));
    return html;
}

std::string handleSetConfig()
{
	for (auto ii = 0; ii < server.args(); ii++)
	{
		verbose("%s = %s\r\n", server.arg(ii), server.argName(ii));
	}
	if (server.hasArg("hname"))
	{
		WiFi.hostname(server.arg("hname").c_str());
		verbose("New hostname %s\r\n", WiFi.hostname().c_str());
	}
	if (server.hasArg("sname"))
	{
		mqtt_server = server.arg("sname").c_str();
		verbose("New MQTT server %s\r\n", mqtt_server.c_str());
	}
	if (server.hasArg("mport"))
	{
		mqtt_port = server.arg("mport").toInt();
		verbose("New MQTT port %d\r\n", mqtt_port);
	}
	return "<html><body>Restarting...</body></html>";
}

void initWebServer()
{
    server.on("/", HTTP_GET, []() { server.send(200, "text/html", pageFixup(configpage).c_str()); });
    server.on("/setconfig", HTTP_GET, [](){ server.send(200, "text/html", handleSetConfig().c_str()); });
    server.begin();
}

void setup()
{
    Serial.begin(115200);
#ifdef GDB
    gdbstub_init();
    delay(5000);
#endif // GDB
    pinMode(2, OUTPUT);
    verbose("WS2812 setup\r\n");
    fx.init();
    fx.setBrightness(10);
    fx.setSpeed(100);
    fx.setMode(0);
    fx.setColor(0);
    fx.start();

    EEPROM.begin(4096);
    webactive = false;

	verbose("Setup Done\r\n");

    deserialize();
    fx.setMode(fxMode);
    connect();
    mqttConnect();
    if (WiFi.hostname().substring(0, 3) == "ESP")
    {
        verbose("I am %s\r\n", WiFi.hostname().c_str());
        initWebServer();
        webactive = true;
    }
}

IPAddress addr;

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
            if (!hostname.empty())
            {
                verbose("Setting hostname %s\r\n", hostname.c_str());
                WiFi.hostname(hostname.c_str());
            }
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

    verbose("\r\nConnected as %s/%s to %s, psk %s\r\n",
        WiFi.hostname().c_str(), WiFi.localIP().toString(),
        WiFi.SSID().c_str(), WiFi.psk().c_str());
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
    for (auto iter: pubSubs)
    {
        verbose("Topic %s \r\n", std::get<TOPIC>(iter).c_str());
    }
}
void mqttConnect()
{
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

typedef std::pair<const char *, void (*)() > CmdPair;
typedef std::map<char, CmdPair> Cmds;
typedef Cmds::iterator CmdsIter;

Cmds cmds = {
    {'-', CmdPair("Clear param", [](){ param = 0; })},
    {'a', CmdPair("Save SSID/PSK to store", [](){ creds[WiFi.SSID().c_str()] = WiFi.psk().c_str(); serialize();})},
    {'A', CmdPair("Load SSIDS from store", [](){ deserialize(); })},
    {'C', CmdPair("Clear store", [](){ EEPROM.write(0, 0); EEPROM.write(1, 0); EEPROM.commit();})},
    {'d', CmdPair("LED on", [](){ digitalWrite(2, LOW); })},
    {'D', CmdPair("LED off", [](){ digitalWrite(2, HIGH); })},
    {'e', CmdPair("Set FX Speed", [](){ fx.setSpeed(param); })},
    {'f', CmdPair("Set FX Mode", [](){ fx.setMode(param); })},
    {'h', CmdPair("Help", [](){ help(); listStoredNetworks(); })},
    {'m', CmdPair("Connect to MQTT", [](){ mqttConnect(); })},
    {'M', CmdPair("Disconnect from MQTT", [](){ mqttClient->disconnect(); })},
    {'n', CmdPair("Scan and connect", [](){  connect(); })},
    {'r', CmdPair("Reset", [](){  verbose("Resetting...\r\n"); while(1); })},
    {'w', CmdPair("Webserver on", [](){  initWebServer(); webactive = true; })},
    {'W', CmdPair("Webserver off", [](){  webactive = false; })}
};

void help()
{
    for (auto && [first, second]: cmds)
    {
        verbose("%c: %s\r\n", first, second.first);
    }

    verbose("Internal hostname %s\r\n", hostname.c_str());
    verbose("WiFi connected %d\r\n", (WiFi.isConnected()));
    verbose("Hostname: %s\r\nSSID: %s\r\nPSK: %s\r\n", WiFi.hostname().c_str(),
        WiFi.SSID().c_str(),
        WiFi.psk().c_str());
	verbose("MQTT_Server %s port %d\r\n", mqtt_server.c_str(), mqtt_port);
	verbose("fxmode %u\r\n", fxMode);
    dumpTopics();
}

void handleCommand()
{
	uint8_t lastCommand;
    lastCommand = Serial.read();
    CmdsIter iter = cmds.find(lastCommand);
    if (iter != cmds.end())
    {
        verbose("%s\r\n", iter->second.first);
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
    if (!mqttClient) return;
	if (!mqttClient->connected()) return;
    Adafruit_MQTT_Subscribe * sub;
    while (sub = mqttClient->readSubscription(1))
    {
        for (auto iter: pubSubs)
        {
            if (sub == std::get<SUB>(iter).get())
            {
                std::get<SUBFUN>(iter)(std::get<SUB>(iter).get());
                std::get<PUBFUN>(iter)(std::get<PUB>(iter).get(), (char *)sub->lastread);
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
    if (webactive)
    {
        server.handleClient();
    }
}
