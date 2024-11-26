/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE" 
   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   In this example rxValue is the data received (only accessible inside that function).
   And txValue is the data to be sent, in this example just a byte incremented every second. 
*/
#define BLE 
#ifdef BLE
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#endif // BLE

#define EPD 1
#ifdef EPD
#include "DEV_Config.h"
#include "Debug.h"
#include "EPD_2in9d.h"
//#include <stdlib.h>

#include "Grin1.h"
#include "Gritted1.h"
#include "overbite.h"
#include "snarl.h"
#include "underbite.h"
#include "hackaday.h"
#include "1010logo.h"
#include "pumpkin.h"
#include "ghost.h"
#endif // EPD

typedef struct 
{
    const char * name;
    const unsigned char * data;
} Resource;

Resource resources[] = {{"Grin1", Grin1_data},
    {"Gritted1", Gritted1_data},
    {"Overbite", overbite_data},
    {"Snarl", snarl_data},
    {"Underbite", underbite_data},
    {"Hackaday", hackaday_data},
    {"1010logo", x1010logo_data},
    {"Ghost", ghost_data},
    {"Pumpkin", pumpkin_data},
    };
const int NumResources = sizeof(resources) / sizeof(resources[0]);

#ifdef BLE
BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    Serial.println("Connected");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    Serial.println("Disconnected");
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
    Serial.print("Received: ");
    Serial.println(rxValue.c_str());
    if (rxValue.back() == '\n') rxValue.pop_back();
    if (rxValue.back() == '\r') rxValue.pop_back();

      if (rxValue.length() > 0) {
    if (rxValue == "list" )
    {
        Serial.println("Sending list");
        pTxCharacteristic->setValue("list,Grin1,Gritted1,Overbite,Snarl,Underbite,Hackaday,1010logo,Ghost,Pumpkin");
        pTxCharacteristic->notify();
        Serial.println("Notified list");
        return;
    }
    for (int ii = 0; ii < NumResources; ii++)
    {
        char buffer[128];
        sprintf(buffer, "Testing [%s] against [%s]", rxValue.c_str(), resources[ii].name);
        Serial.println(buffer);
        if (rxValue == resources[ii].name)
        {
            Serial.println("Found a thing");
    		EPD_2IN9D_Init();
//		    EPD_2IN9D_Clear();
             EPD_2IN9D_Display(resources[ii].data);
    EPD_2IN9D_Sleep();
//            display.fillScreen(GxEPD_WHITE);
//            display.drawImage((const uint8_t *)resources[ii].data,
//            0, 0, GxEPD_WIDTH, GxEPD_HEIGHT);
            break;
//            void drawImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w, int16_t h, bool invert = false, bool mirror_y = false, bool pgm = false);
//            display.drawBitmap(resources[ii].data, 0, 0,  GxEPD_WIDTH, GxEPD_HEIGHT, GxEPD_BLACK);
//            display.update();
        }
    }
      }
    }
};
#endif // BLE


void setup() {
  Serial.begin(115200);
  Serial.println("Init");
#ifdef BLE
  // Create the BLE Device
  BLEDevice::init("BlueMouth Service");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  Serial.println("BLE Server");

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);
  Serial.println("BLE Service");

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
          CHARACTERISTIC_UUID_TX,
          BLECharacteristic::PROPERTY_NOTIFY
          );

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
          CHARACTERISTIC_UUID_RX,
          BLECharacteristic::PROPERTY_WRITE
          );

  pRxCharacteristic->setCallbacks(new MyCallbacks());
  Serial.println("BLE Characteristic");

  Serial.println("BLE Service Start");
  // Start the service
  pService->start();

  // Start advertising
  Serial.println("BLE Advertising Start");
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
#endif // BLE
#ifdef EPD
 DEV_Module_Init();
    EPD_2IN9D_Init();
    EPD_2IN9D_Clear();
    EPD_2IN9D_Display(Gritted1_data);
    EPD_2IN9D_Sleep();
#endif // EPD

}

void loop() {
#ifdef BLE
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
#endif // BLE
}
