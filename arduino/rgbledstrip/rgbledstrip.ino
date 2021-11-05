#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "Arduino.h"

const char WIFI_SSID[] = "yfinity";
const char WIFI_PASS[] = "24HoursADay";
//const char MQTT_SERVER[] = "DESKTOP-TOJK787";
//const char MQTT_SERVER[] = "pop-os";
const char MQTT_SERVER[] = "192.168.1.27";
const int MQTT_PORT = 1883;

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT);
Adafruit_MQTT_Subscribe ledstate = Adafruit_MQTT_Subscribe(&mqtt, "/led");

uint8_t mqttConnected;

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
	mqttConnected = 0;
    Serial.begin(115200);
    pinMode(2, OUTPUT);
	verbose("Setup\r\n");
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
	mqttConnected = 1;
  verbose("MQTT Connected!\r\n");
}

void help()
{
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

void handleCommand()
{
	uint8_t lastCommand;
    lastCommand = Serial.read();
    switch (lastCommand)
    {
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
            break;

        case 'W':   // Disconnect from Wifi
            break;

        case 'm':  // Connect to MQTT
			MQTT_connect();
            break;

        case 'M':   // Disconnect from MQTT
			mqtt.disconnect();
			mqttConnected = 0;
            break;

        case 's':   // Subscribe to topic
            mqtt.subscribe(&ledstate);
            break;

        case 'S':   // Unsubscribe from topic
            mqtt.unsubscribe(&ledstate);
            break;

        case 'l': // LED on
            digitalWrite(5, LOW);
            break;

        case 'L': // LED off
            digitalWrite(5, HIGH);
            break;
    }
}

void loop()
{
	if (Serial.available())
	{
		handleCommand();
	}
	if (mqttConnected)
	{
		if (!mqtt.ping())
		{
			mqtt.disconnect();
			mqttConnected = 0;
		}
	}
}
