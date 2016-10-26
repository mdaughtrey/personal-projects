/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <ESP8266WiFi.h>
#include <Servo.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Adafruit_NeoPixel.h>
#include <ESP8266WebServer.h>
//#include <CMMC_OTA.h>
#include <cstdlib>
#include <ctype.h>
#include <effect.h>
#include <EEPROM.h>

//#define LED_HEARTBEAT 16
#define NEO_NUMPIXELS 10
/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "Zooma223"
#define WLAN_PASS       "N0stromo"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "192.168.0.30"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "tentacle"
#define AIO_KEY         "tentacle"
#define EPADDR_PASS 0
#define EPADDR_SSID 33

/************ Global State (you don't need to change this!) ******************/
// CMMC_OTA ota;
// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
//WifiServer * server(nullptr);
ESP8266WebServer * webServer(nullptr);
Adafruit_NeoPixel np(Adafruit_NeoPixel(NEO_NUMPIXELS, 14, NEO_GRB + NEO_KHZ800));
Effect effect(np, 50);
IPAddress serverAddr(192, 168, 0, 1);

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEO_NUMPIXELS, 14, NEO_GRB + NEO_KHZ800);
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
//Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/photocell");
Adafruit_MQTT_Subscribe servo = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/servo");
Adafruit_MQTT_Subscribe fight = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/fight");
Adafruit_MQTT_Subscribe color = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/color");
Adafruit_MQTT_Subscribe subEffect = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/effect");
Adafruit_MQTT_Subscribe background = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/background");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

char doHeartbeat('0');
Servo shaft;
uint8_t fightState = 0;
uint32_t fightMillis;


bool eepromToVar(uint16_t addr, char * dest)
{
  uint8_t len(EEPROM.read(addr));
  if (len > 32)
  {
      return false;
  }
  uint8_t ii;
  for (ii = 0; ii < EEPROM.read(addr); ii++)
  {
      dest[ii] = EEPROM.read(addr + ii + 1);
      if (!::isprint(dest[ii]))
      {
          return false;
      }
  }
  dest[ii] = 0;
  return true;
}

void handleRoot()
{
    webServer->send(200, "text/plain", "handleRoot");
}

void handleServo()
{
}

void handleFight()
{
    fightState = webServer->arg("state").charAt(0);
    fightMillis = millis();
    if (fightState == '0')
    {
        fightState = 0;
    }
    Serial.print("handleFight state ");
    Serial.println((int)fightState);
    webServer->send(200, "text/plain", "");
}

void handleColor()
{
}

void handleEffect()
{
}

void handleBackground()
{
}

void setup() {
  EEPROM.begin(512);
  Serial.begin(115200);
  delay(10);
  bool doAP(false);

  char ssid[16] = { 0 };
  char pass[16] = { 0 };

  do
  {
//      if (false == eepromToVar(EPADDR_SSID, ssid)
//      || false == eepromToVar(EPADDR_PASS, pass))
//      {
//          doAP = true;
//          break;
//      }

      Serial.print("Connecting to ");
      Serial.println(ssid);

      //WiFi.begin(ssid, pass);
      WiFi.begin(WLAN_SSID, WLAN_PASS);
      uint8_t retry(100);
      while (WiFi.status() != WL_CONNECTED && retry--)
      {
        delay(500);
        Serial.print(".");
      }
      Serial.println();
      break;
      if (WiFi.status() == WL_CONNECT_FAILED)
      {
          Serial.println("No AP Found");
          doAP = true;
      }
  } while (0);
  #if 0
  if (doAP)
  {
      Serial.println("Configuring as access point SSID tentacle");
      //server = new WiFiServer();1
//      WiFi.begin("tentacle", "tentacle");
      WiFi.mode(WIFI_AP);
      WiFi.softAP("tentacle");

      webServer = new ESP8266WebServer(80);
      webServer->on("/", handleRoot);
      webServer->on("/servo", handleServo);
      webServer->on("/fight", handleFight);
      webServer->on("/color", handleColor);
      webServer->on("/effect", handleEffect);
      webServer->on("/background", handleBackground);

      webServer->begin();
      Serial.print("Started HTTP Server at address 192.168.4.1");
  }
  else
  #endif
  {
      Serial.println("WiFi connected");
      Serial.println("IP address: "); Serial.println(WiFi.localIP());

      mqtt.subscribe(&servo);
      mqtt.subscribe(&fight);
      mqtt.subscribe(&color);
      mqtt.subscribe(&subEffect);
      mqtt.subscribe(&background);

      pinMode(0, OUTPUT);
      shaft.attach(12);
  }
}


void loop()
{
    #if 0
    if (webServer)
    {
        webServer->handleClient();
        return;
    }
    #endif
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while (true)
  {
    subscription = mqtt.readSubscription(20);
    if (subscription == &fight)
    {
        fightState = fight.lastread[0];
        fightMillis = millis();
        if (fightState == '0')
        {
            fightState = 0;
        }
    }
    else if (subscription == &subEffect)
    {
        effect.select((const char *)subEffect.lastread);
    }
    else if (subscription == &servo)
    {
        shaft.write(atoi((const char *) servo.lastread));
    }
    else if (subscription == &color)
    {
        char * pp;
        uint32 values = std::strtoul((char *)&color.lastread[1], &pp, 16);
        effect.color(
            (values >> 16) & 0xff, // green
            (values >> 8) & 0xff,  // red
            values & 0xff);       // blue
    }
    else if (subscription == &background)
    {
        char * pp;
        uint32 values = std::strtoul((char *)&background.lastread[1], &pp, 16);
        effect.background(
            (values >> 16) & 0xff, // green
            (values >> 8) & 0xff,  // red
            values & 0xff);       // blue
    }
    if (fightState == '0' && (millis() - fightMillis > 800))
    {
        shaft.write(180);
        fightState = '1';
        fightMillis = millis();
    }
    else if (fightState == '1' && (millis() - fightMillis > 800))
    {
        shaft.write(0);
        fightState = '0';
        fightMillis = millis();
    }
    effect.loop();
  }
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
