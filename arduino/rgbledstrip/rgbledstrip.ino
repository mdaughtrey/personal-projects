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

const char WIFI_SSID[] = "yfinity";
const char WIFI_PASS[] = "24HoursADay";
//const char MQTT_SERVER[] = "DESKTOP-TOJK787";
//const char MQTT_SERVER[] = "pop-os";
const char MQTT_SERVER[] = "pop-os.local";
const int MQTT_PORT = 1883;
const uint8_t LED_COUNT = 8;
const uint8_t LED_PIN = 4;

int16_t param;

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

#define CREDS_MAP
#ifdef CREDS_MAP
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
    for (int addr = 0; addr < serout.size(); addr++)
    {
        verbose("A %04x D %02X\r\n", addr, serout[addr]);
        EEPROM.write(2 + addr, serout[addr]);
    }
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
            verbose("first %s\r\n", first.c_str());
        }
        else if (second.empty())
        {
            second.assign(serin.begin() + ii, serin.begin() + ii + size);
            verbose("second %s\r\n", second.c_str());
            creds[first] = second;
            first.clear();
            second.clear();
        }
        ii += size;
    }
}
#endif // CREDS_MAP
#ifdef CREDS_LIST
typedef std::pair<std::string, std::string > CredPair;
typedef std::vector<CredPair> Creds;
//typedef std::vector<std::pair<std::vector<char>, std::vector<char> > > Creds;
typedef Creds::iterator CredsIter;
Creds creds;

void serialize()
{
    if (creds.empty())
    {
        return;
    }
    if (creds.size() > 4000)
    {
        verbose("Cred size %d vs max 4096\r\n", creds.size());
        return;
    }
    std::vector<uint8_t> serout;
    for (auto && [first, second]: creds)
    {
        serout.push_back((uint8_t)first.size());
        serout.insert(serout.end(), first.begin(), first.end());
        serout.push_back((uint8_t)second.size());
        serout.insert(serout.end(), second.begin(), second.end());
    }
    verbose("Serialized to %d bytes\r\n", serout.size());
    EEPROM.write(0, serout.size() >> 8);
    EEPROM.write(1, serout.size() & 0xff);
    for (int addr = 0; addr < serout.size(); addr++)
    {
        verbose("A %04x D %02X\r\n", addr, serout[addr]);
        EEPROM.write(2 + addr, serout[addr]);
    }
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
            verbose("first %s\r\n", first.c_str());
        }
        else if (second.empty())
        {
            second.assign(serin.begin() + ii, serin.begin() + ii + size);
            verbose("second %s\r\n", second.c_str());
            creds.push_back(CredPair(first, second));
            first.clear();
            second.clear();
        }
        ii += size;
    }
}
#endif // CREDS_LIST

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

    delay(2000);
	verbose("Setup %s\r\n");
    Serial.println(__cplusplus);
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

void help()
{
    Serial.println(__cplusplus);
    verbose("-: param = 0\r\n");
    verbose("a: save WPS to EEPROM\r\n");
    verbose("A: deserialize\r\n");
	verbose("c:	clear EEPROM\r\n");
	verbose("d: LED on\r\n");
	verbose("D: LED off\r\n");
    verbose("e: set FX speed\r\n");
    verbose("f: set fx mode (param)\r\n");
	verbose("h:	help\r\n");
//	verbose("w: Connect to Wifi\r\n");
	verbose("W: WPS Connect\r\n");
	verbose("m: Connect to MQTT\r\n");
	verbose("M: Disconnect from MQTT\r\n");
    verbose("l: list stored networks\r\n");
    verbose("n: scan networks\r\n");
	verbose("r: reset\r\n");
	verbose("s: Subscribe to topic\r\n");
	verbose("S: Unsubscribe from topic\r\n");
    verbose("t: save test creds to store\r\n");

    verbose("SSID: %s\r\nPSK: %s\r\n", WiFi.SSID().c_str(), WiFi.psk().c_str());
}


void handleCommand()
{
	uint8_t lastCommand;
    int rc;
    lastCommand = Serial.read();
    switch (lastCommand)
    {
        case '-': param = 0; break;

        case 'a': // save WPS to store
#ifdef CREDS_LIST            
            creds.push_back(CredPair(std::string(WiFi.SSID().c_str()),
                std::string(WiFi.psk().c_str())));
#endif // CREDS_LIST
#ifdef CREDS_MAP
            creds[WiFi.SSID().c_str()] = WiFi.psk().c_str();
#endif
            serialize();
//            EEPROM.commit();
            break;

        case 'A':
            deserialize();
            break;

        case 'c': // clear EEPROM
            EEPROM.write(0, 0);
            EEPROM.write(1, 0);
            EEPROM.commit();
            break;

        case 'e': // set Fx speed
            fx.setSpeed(param);
            break;

        case 'f':
            fx.setMode(param);
            break;

		case 'h':	// help
			help();
			break;

        case 'l': // list stored netorks
            verbose("%d Stored Networks:\r\n", creds.size());
            for (auto && [f, s]: creds)
            {
                verbose("SSID: %s ", f.c_str());
                verbose("PSK: %s\r\n", s.c_str());
            }
            break;

        case 'n': // scan networks
            rc = WiFi.scanNetworks();
            verbose("scanNetworks rc %d\r\n", rc);
            if (-1 == rc)
            {
                verbose("Scan failed\r\n");
                break;
            }
            verbose("%d networks found\r\n", rc);
            for (auto ii = 0; ii < rc; ii++)
            {
                verbose("%d: SSI: %s RSSI \r\n", ii, WiFi.SSID(ii), WiFi.RSSI(ii));
                auto iter = creds.find(WiFi.SSID(ii).c_str());
                if (iter != creds.end())
                {
                    verbose("Matched on %s/%s\r\n", iter->first.c_str(),
                        iter->second.c_str());
                }
            }
            break;

		case 'r': // reset
			verbose("Resetting...");
			while(1);
			break;

//         case 'w':  // Connect to Wifi
//             WiFi.begin(ssid.c_str(), psk.c_str());
//             while (WiFi.status() != WL_CONNECTED)
//             {
//                 verbose("Connecting to %s\r\n", WIFI_SSID);
//                 delay(500);
//             }
//             verbose("Connected as %s\r\n", WiFi.localIP().toString().c_str());
//             rc = WiFi.hostByName(MQTT_SERVER, addr);
//             verbose("hostByName rc %d\r\n", rc);
//             Serial.println(addr);
//             break;

        case 'W':   // WPS
            rc = WiFi.beginWPSConfig();
            verbose("WPSConfig rc %d, connecting to %s\r\n",
                rc, WiFi.SSID());
            if (WiFi.SSID() == "")
            {
                verbose("Timed out");
                break;
            }

            while (WiFi.psk() == "")
            {
                verbose("Waiting on PSK\r\n");
                yield();
                delay(1000);
            }

//            while (WiFi.status() != WL_CONNECTED)
//            {
//                verbose(".");
//                delay(5000);
//            }
            verbose("\r\nConnected as %s to %s, psk %s\r\n",
                WiFi.localIP().toString(), WiFi.SSID().c_str(), WiFi.psk().c_str());

//            ssid = WiFi.SSID().c_str();
//            psk = WiFi.psk().c_str();
//            bssid = WiFi.BSSIDstr().c_str();

//            verbose("SSID: %s\r\nPSK: %s\r\n, BSSID: %s\r\n",
 ////               ssid.c_str(), psk.c_str(), bssid.c_str());

            for (auto ci: creds)
            {
                if (WiFi.SSID() == ci.first.data())
                {
                    verbose("Found %s\r\n", WiFi.SSID());
                }
            }
            break;

        case 'm':  // Connect to MQTT
            verbose("MQTT_Connect\r\n");
			MQTT_connect();
            break;

        case 'M':   // Disconnect from MQTT
            verbose("MQTT_Disconnect");
			mqtt.disconnect();
            break;

        case 's':   // Subscribe to topic
            for (auto && [first,second]: subs)
            {
                rc = mqtt.subscribe(first);
                verbose("subscribe rc %d\r\n", rc);
            }
            break;

        case 'S':   // Unsubscribe from topic
            for (auto && [first, second]: subs)
            {
                rc = mqtt.unsubscribe(first);
                verbose("unsubscribe rd %d\r\n", rc);
            }
            break;

        case 'd': // LED on
            digitalWrite(2, LOW);
            break;

        case 'E': // LED off
            digitalWrite(2, HIGH);
            break;

        case 't': // store test creds
#ifdef CREDS_LIST
            creds.push_back(CredPair("TestSSID", "TestPSK"));
#endif CREDS_LIST // CREDS_LIST
#ifdef CREDS_MAP
            creds["TestSSID"] = "TestPSK";
#endif // CREDS_MAP
            serialize();
//            EEPROM.commit();
            break;

        default:                                                // Load parameters
            if (lastCommand >= '0' & lastCommand <= '9')
            {
                param *= 10;
                param += lastCommand - '0';
            }
            break;
    }
}

void mqttPing()
{
    uint8_t mode;
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
