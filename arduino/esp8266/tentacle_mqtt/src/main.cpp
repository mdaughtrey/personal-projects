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
#include <CMMC_OTA.h>
#include <cstdlib>
#include <effect.h>

#define LED_HEARTBEAT 16
#define NEO_NUMPIXELS 10
/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "Zooma223"
#define WLAN_PASS       "N0stromo"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "192.168.0.30"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "mdaughtrey"
#define AIO_KEY         "Spackle"

/************ Global State (you don't need to change this!) ******************/
CMMC_OTA ota;
// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
Adafruit_NeoPixel np(Adafruit_NeoPixel(NEO_NUMPIXELS, 14, NEO_GRB + NEO_KHZ800));
Effect effect(np, 100);
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEO_NUMPIXELS, 14, NEO_GRB + NEO_KHZ800);
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
//Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/photocell");

// Setup a feed called 'onoff' for subscribing to changes.
// Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/click");
//Adafruit_MQTT_Subscribe heartbeat = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/hb");
Adafruit_MQTT_Subscribe servo = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/servo");
Adafruit_MQTT_Subscribe fight = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/fight");
Adafruit_MQTT_Subscribe color = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/color");
Adafruit_MQTT_Subscribe subEffect = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/effect");

// Adafruit_MQTT_Subscribe red = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/red");
// Adafruit_MQTT_Subscribe green = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/green");
// Adafruit_MQTT_Subscribe blue = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/blue");

//enum class Effect { off, tiphb };

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

uint8_t gammaTable[16] = {
    0, 1, 2, 6, 12, 20, 31, 44, 60, 79, 100, 125, 153, 183, 218, 255
};

char doHeartbeat('0');
Servo shaft;

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  // mqtt.subscribe(&onoffbutton);
  //mqtt.subscribe(&heartbeat);
  mqtt.subscribe(&servo);
  mqtt.subscribe(&fight);
  mqtt.subscribe(&color);
  mqtt.subscribe(&subEffect);

  // mqtt.subscribe(&red);
  // mqtt.subscribe(&green);
  // mqtt.subscribe(&blue);

  pinMode(0, OUTPUT);
  pinMode(LED_HEARTBEAT, OUTPUT);
  shaft.attach(12);

//   ota.on_start([]() { Serial.println("OTA Start"); });
//   ota.on_end([]() { Serial.println("OTA End"); });
//     ota.on_progress([](unsigned int progress, unsigned int total) {
//         Serial.printf("_CALLBACK_ Progress: %u/%u\r\n", progress,  total);
//     });
// ota.on_error([](ota_error_t error) {
//       Serial.printf("Error[%u]: ", error);
//     });

  ota.init();
  // strip.begin();
  // strip.show();
}

uint32_t x=0;
//int effectCount = 0;
uint8_t fightState = 0;
uint32_t fightMillis;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while (true)
  {
    ota.loop();
    subscription = mqtt.readSubscription(20);
    // if (subscription == &onoffbutton) {
    // //   Serial.print(F("Got: "));
    // //   Serial.println((char *)onoffbutton.lastread);
    //   digitalWrite(0, onoffbutton.lastread[0] == '1' ? 1 : 0);
    // }
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
        // Serial.print(F("Effect "));
        // Serial.println((char *)subEffect.lastread);
        effect.select((const char *)subEffect.lastread);
    }
    else if (subscription == &servo)
    {
        shaft.write(atoi((const char *) servo.lastread));
    }
    if (subscription == &color)
    {
        char * pp;
        color.lastread[7] = 0;
        uint32 values = std::strtoul((char *)&color.lastread[1], &pp, 16);
        effect.color(
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
    // switch (effect)
    // {
    //     case Effect::tiphb: // Heartbeat at tip only
    //     {
    //         int duty = int(sin(radians(effectCount)) * 15);
    //         if (duty > 0)
    //         {
    //             analogWrite(LED_HEARTBEAT, 1023 - gammaTable[duty] * 4);
    //             for (int ii = 0; ii < NEO_NUMPIXELS; ii++)
    //             {
    //                 strip.setPixelColor(ii, strip.Color(gammaTable[duty], 0, 0));
    //             }
    //             strip.show();
    //         }
    //     }
    //     effectCount += 10;
    //     effectCount %= 360;
    //     break;
    // }
  }

  // // Now we can publish stuff!
  // Serial.print(F("\nSending photocell val "));
  // Serial.print(x);
  // Serial.print("...");
  // if (! photocell.publish(x++)) {
  //   Serial.println(F("Failed"));
  // } else {
  //   Serial.println(F("OK!"));
  // }

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
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
