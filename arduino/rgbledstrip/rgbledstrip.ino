#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WS2812FX.h>
#include <ESP8266WebServer.h>
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
Adafruit_MQTT_Client * mqtt = 0;
ESP8266WebServer server(80);
std::string hostname = "none";
std::string mqtt_server = "none";
uint16_t mqtt_port = 1883;
bool webactive;

typedef Adafruit_MQTT_Subscribe Sub;
//typedef std::pair<Sub *, void (*)(Sub *) > SubPair; 
typedef std::pair<std::shared_ptr<Sub>, void (*)(Sub *) > SubPair; 

//char * topic(std::string & c)
//{
//	char buffer[128];
//	snprintf(buffer, 128, "/%s/%s", WiFi.hostname().c_str(), c);
//	return buffer;
//}

std::vector<std::string>  topics = { "led", "leds", "speed", "bright", "color" };

auto subs = {
    SubPair(std::shared_ptr<Sub>(), [] (Sub * sub) { digitalWrite(2, '0' == *(sub->lastread) ? LOW: HIGH); }),
    SubPair(std::shared_ptr<Sub>(), [] (Sub * sub) { fx.setMode(String((const char *)sub->lastread).toInt());}),
    SubPair(std::shared_ptr<Sub>(), [] (Sub * sub) { fx.setSpeed(String((const char *)sub->lastread).toInt()); }),
    SubPair(std::shared_ptr<Sub>(), [] (Sub * sub) { fx.setBrightness(String((const char *)sub->lastread).toInt()); }),
    SubPair(std::shared_ptr<Sub>(), [] (Sub * sub) { fx.setColor(std::stoi((const char *)sub->lastread)); })
};

void buildTopics()
{
	char buffer[128];
    auto itopic = topics.begin();
    for (auto && [first, second]: subs)
    {
	    snprintf(buffer, 128, "/%s/%s", WiFi.hostname().c_str(), itopic->c_str());
        itopic->assign(buffer);
        first = std::make_shared<Sub>(new Adafruit_MQTT_Subscribe(mqtt, itopic->c_str()));
        verbose("Built topic %s\r\n", itopic->c_str());
        itopic++;
    }
}

//auto subs = {
//    //SubPair(new Adafruit_MQTT_Subscribe(mqtt, topic("led")),
//    SubPair(new Adafruit_MQTT_Subscribe(mqtt, "led"),
//        [] (Sub * sub) { digitalWrite(2, '0' == *(sub->lastread) ? LOW: HIGH); }),
//    //SubPair(new Adafruit_MQTT_Subscribe(mqtt, topic("leds")),
//    SubPair(new Adafruit_MQTT_Subscribe(mqtt, "leds"),
//        [] (Sub * sub) { fx.setMode(String((const char *)sub->lastread).toInt());}),
//    SubPair(new Adafruit_MQTT_Subscribe(mqtt, "speed"),
//        [] (Sub * sub) { fx.setSpeed(String((const char *)sub->lastread).toInt()); }),
//    SubPair(new Adafruit_MQTT_Subscribe(mqtt, "bright"),
//        [] (Sub * sub) { fx.setBrightness(String((const char *)sub->lastread).toInt()); }),
//    SubPair(new Adafruit_MQTT_Subscribe(mqtt, "color"),
//        [] (Sub * sub) { fx.setColor(std::stoi((const char *)sub->lastread)); })
//};

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
    std::vector<uint8_t> serout;
    serout.push_back(WiFi.hostname().length());
    std::string tmp(WiFi.hostname().c_str());
    serout.insert(serout.end(), tmp.begin(), tmp.end());
    //serout.insert(serout.end(), WiFi.hostname.begin(), hostname.end());
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
    for (auto iter: serout)
    {
        verbose("%02x ", iter);
    }
    verbose("\r\n");
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
        verbose("%04d: %02x\r\n", 2+ii, serin.back());
    }
    ii = 0;
 	auto iter = serin.begin();
	size = *iter++;
	// hostname
    hostname.assign(iter, iter + size);
    verbose("Loaded hostname %s\r\n", hostname.c_str());
//    WiFi.hostname(hostname.c_str());
	iter += size;

	// mqtt_server
	size = *iter++;
    mqtt_server.assign(iter, iter + size);
    verbose("Loaded mqtt_server %s\r\n", mqtt_server.c_str());
	iter += size;

	// mqtt_port
	mqtt_port = (*iter++) << 8;
	mqtt_port |= *iter++;
    verbose("Loaded mqtt_port %d\r\n", mqtt_port);

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
		//hostname = server.arg("hname").c_str();
		WiFi.hostname(server.arg("hname").c_str());
		//verbose("New hostname %s\r\n", hostname.c_str());
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

//GET /setconfig?hname=%7B%7Bhostname%7D%7D&sname=%7B%7Bmqtt_server%7D%7D&mport=%7B%7Bmqtt_port%7D%7D HTTP/1.1" 404
void initWebServer()
{
    server.on("/", HTTP_GET, []() { server.send(200, "text/html", pageFixup(configpage).c_str()); });
    server.on("/setconfig", HTTP_GET, [](){ server.send(200, "text/html", handleSetConfig().c_str()); });
    server.begin();
}

void setup()
{
    Serial.begin(115200);
    pinMode(2, OUTPUT);
//    deserialize();
//    connect();

    verbose("WS2812 setup\r\n");
    fx.init();
    fx.setBrightness(100);
    fx.setSpeed(10);
    fx.setMode(FX_MODE_RAINBOW_CYCLE);
    fx.start();

    EEPROM.begin(4096);
//    verbose("Web setup\r\n");
    webactive = false;

	verbose("Setup Done\r\n");
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt->connected()) {
    return;
  }

  verbose("Connecting to MQTT...%x\r\n", mqtt);
  mqtt->connect();

//  uint8_t retries = 3;
//  while ((ret = mqtt->connect()) != 0) { // connect will return 0 for connected
//       verbose("%s\r\n", (const char * )mqtt->connectErrorString(ret));
//       verbose("Retrying MQTT connection in 5 seconds...\r\n");
//       mqtt->disconnect();
//       delay(5000);  // wait 5 seconds
//       retries--;
//       if (retries == 0) {
//		 verbose("Looks like a fail\r\n");
//		
//         // basically die and wait for WDT to reset me
//     //    while (1);
//       }
//  }
//  mqtt->setKeepAliveInterval(5);
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

void mqttConnect()
{
    int8_t ret;
    verbose("MQTT_Connect\r\n");
    mqtt = new Adafruit_MQTT_Client(&client, mqtt_server.c_str(), mqtt_port);
//    for (auto && [first,second]: subs)
//    {
//        int rc;
//        rc = mqtt->subscribe(first);
//        verbose("subscribe rc %d\r\n", rc);
//    }

    verbose("Connecting to MQTT...%x\r\n", mqtt);

    uint8_t retries = 3;
    while ((ret = mqtt->connect()) != 0) { // connect will return 0 for connected
        verbose("%s\r\n", (const char * )mqtt->connectErrorString(ret));
        verbose("Retrying MQTT connection in 5 seconds...\r\n");
        mqtt->disconnect();
        delay(3000);  // wait 5 seconds
        retries--;
        if (retries == 0) {
            verbose("Looks like a fail\r\n");
            return;
        }
    }
    mqtt->setKeepAliveInterval(5);
    verbose("MQTT Connected!\r\n");
}

typedef std::pair<std::string, void (*)() > CmdPair;
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
    {'M', CmdPair("Disconnect from MQTT", [](){ mqtt->disconnect(); })},
    {'n', CmdPair("Scan and connect", [](){  connect(); })},
    {'r', CmdPair("Reset", [](){  verbose("Resetting...\r\n"); while(1); })},
    {'w', CmdPair("Webserver on", [](){  initWebServer(); webactive = true; })},
    {'W', CmdPair("Webserver off", [](){  webactive = false; })}
};

void help()
{
    for (auto && [first, second]: cmds)
    {
        verbose("%c: %s\r\n", first, second.first.c_str());
    }

    verbose("Internal hostname %s\r\n", hostname.c_str());
    verbose("WiFi connected %d\r\n", (WiFi.isConnected()));
    verbose("Hostname: %s\r\nSSID: %s\r\nPSK: %s\r\n", WiFi.hostname().c_str(),
        WiFi.SSID().c_str(),
        WiFi.psk().c_str());
	verbose("MQTT_Server %s port %d\r\n", mqtt_server.c_str(), mqtt_port);
}

void handleCommand()
{
	uint8_t lastCommand;
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

//void mqttPing()
//{
//    if (!mqtt) return;
//	if (!mqtt->connected()) return;
//    Adafruit_MQTT_Subscribe * sub;
//    while (sub = mqtt->readSubscription(1))
//    {
//        for (auto && [first, second]: subs)
//        {
//            if (sub == first)
//            {
//                second(first);
//            }
//        }
//	}
//}

void loop()
{

	if (Serial.available())
	{
		handleCommand();
	}
//    mqttPing();
    fx.service();
    if (webactive)
    {
        server.handleClient();
    }
}
