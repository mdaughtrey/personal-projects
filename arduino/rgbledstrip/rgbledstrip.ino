#include <ESP8266WiFi.h>
#include <WS2812FX.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "Arduino.h"

const char WIFI_SSID[] = "yfinity";
const char WIFI_PASS[] = "24HoursADay";
//const char MQTT_SERVER[] = "DESKTOP-TOJK787";
//const char MQTT_SERVER[] = "pop-os";
const char MQTT_SERVER[] = "pop-os.local";
const int MQTT_PORT = 1883;
const uint8_t LED_COUNT = 8;
const uint8_t LED_PIN = 4;


WiFiClient client;
WS2812FX fx = WS2812FX(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT);

auto subs = {
    { Adafruit_MQTT_Subscribe(&mqtt, "/led"), [] (Adafruit_MQTT_Subscribe & sub) { digitalWrite(2, '0' == *sub.lastread ? LOW: HIGH); }},
    { Adafruit_MQTT_Subscribe(&mqtt, "/leds"), [] (Adafruit_MQTT_Subscribe & sub) { fx.setMode(String((const char *)sub.lastread).toInt()); }},
    { Adafruit_MQTT_Subscribe(&mqtt, "/speed"), [] (Adafruit_MQTT_Subscribe & sub) { fx.setSpeed(String((const char *)sub.lastread).toInt()); }},
    { Adafruit_MQTT_Subscribe(&mqtt, "/bright"), [] (Adafruit_MQTT_Subscribe & sub) { fx.setBrightness(String((const char *)sub.lastread).toInt()); }}
};


//Adafruit_MQTT_Subscribe ledstate = Adafruit_MQTT_Subscribe(&mqtt, "/led");
//Adafruit_MQTT_Subscribe leds = Adafruit_MQTT_Subscribe(&mqtt, "/leds");
//Adafruit_MQTT_Subscribe speed = Adafruit_MQTT_Subscribe(&mqtt, "/speed");
//Adafruit_MQTT_Subscribe bright = Adafruit_MQTT_Subscribe(&mqtt, "/bright");

//void verbose(uint8_t threshold, const char * fmt, ...)
// void flash2char(const __FlashStringHelper * ffmt, ...)
// {
// 	char fmt[256];
// 	memset(fmt, '\0', sizeof(fmt));
// 	strlcpy_P(fmt, (const char PROGMEM *)ffmt, sizeof(fmt));
//     va_list args;
//     va_start(args, ffmt);
// 	verbose(fmt,  args);
// }

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

void setup()
{
    Serial.begin(115200);
    pinMode(2, OUTPUT);

    fx.init();
    fx.setBrightness(100);
    fx.setSpeed(10);
    fx.setMode(FX_MODE_RAINBOW_CYCLE);
    fx.start();

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

void help()
{
    Serial.println(__cplusplus);
	verbose("c:	manual connect\r\n");
	verbose("h:	help\r\n");
	verbose("w: Connect to Wifi\r\n");
	verbose("W: Disconnect from Wifi\r\n");
	verbose("m: Connect to MQTT\r\n");
	verbose("M: Disconnect from MQTT\r\n");
	verbose("r: reset\r\n");
	verbose("s: Subscribe to topic\r\n");
	verbose("S: Unsubscribe from topic\r\n");
	verbose("l: LED on\r\n");
	verbose("L: LED off\r\n");
}

IPAddress addr;
void handleCommand()
{
	uint8_t lastCommand;
    int rc;
    lastCommand = Serial.read();
    switch (lastCommand)
    {
        case 'c': // manual connect
            rc = client.connect(MQTT_SERVER, MQTT_PORT);
            verbose("client.connect %d\r\n", rc);
            client.println("Test message from ESP8266");
            break;

		case 'h':	// help
			help();
			break;

		case 'r': // reset
			verbose("Resetting...");
			while(1);
			break;

        case 'w':  // Connect to Wifi
            WiFi.begin(WIFI_SSID, WIFI_PASS);
            while (WiFi.status() != WL_CONNECTED)
            {
                verbose("Connecting to %s\r\n", WIFI_SSID);
                delay(500);
            }
            verbose("Connected as %s\r\n", WiFi.localIP().toString().c_str());
            rc = WiFi.hostByName(MQTT_SERVER, addr);
            verbose("hostByName rc %d\r\n", rc);
            Serial.println(addr);
//            verbose("%s is %s\r\n", addr.toString().c_str());
            break;

        case 'W':   // Disconnect from Wifi
            break;

        case 'm':  // Connect to MQTT
			MQTT_connect();
            break;

        case 'M':   // Disconnect from MQTT
			mqtt.disconnect();
            break;

        case 's':   // Subscribe to topic
            rc = mqtt.subscribe(&ledstate);
            verbose("ledstate subscribe rc %d\r\n", rc);
            rc = mqtt.subscribe(&leds);
            verbose("leds subscribe rc %d\r\n", rc);
//            rc = client.connect(MQTT_SERVER, MQTT_PORT);
//            verbose("client.connect %d\r\n", rc);
            break;

        case 'S':   // Unsubscribe from topic
            rc = mqtt.unsubscribe(&ledstate);
            rc = mqtt.unsubscribe(&leds);
            verbose("unsubscribe rd %d\r\n", rc);
            break;

        case 'l': // LED on
            digitalWrite(2, LOW);
            break;

        case 'L': // LED off
            digitalWrite(2, HIGH);
            break;
    }
}

//uint32_t mqttping = 0;

void mqttPing()
{
    uint8_t mode;
	if (!mqtt.connected()) return;
//    if (millis() - mqttping > 5000)
//	{
//		if (!mqtt.ping())
//		{
//			mqtt.disconnect();
//			mqtt.connect();
//		}
//        else
//        {
//            mqttping = millis();
//        }
//    }
    Adafruit_MQTT_Subscribe * sub;
    while (sub = mqtt.readSubscription(1))
    {
        1G
        if (sub == &ledstate)
        {
            verbose("ledstate %s\r\n", (char *)ledstate.lastread);
            if ('0' == *ledstate.lastread)
            {
                digitalWrite(2, HIGH);
            }
            else
            {
                digitalWrite(2, LOW);
            }
        }
        if (sub == &leds)
        {
            mode = String((const char *)leds.lastread).toInt();
            verbose("leds %d\r\n", mode);
            fx.setMode(mode);
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
