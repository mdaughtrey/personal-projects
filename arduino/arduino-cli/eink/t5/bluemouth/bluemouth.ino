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
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define EPD 1
#ifdef EPD
#include <GxEPD.h>
#include "SD.h"
#include "SPI.h"
#include <GxGDEH0213B72/GxGDEH0213B72.h>  // 2.13" b/w new panel

#include "bitmap.h"
#include "mouth1.h"
#include "toothysmile.h"
#include "hulk.h"

#define GxGDEH0213B72_X_PIXELS 128
#define GxGDEH0213B72_Y_PIXELS 250

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>


#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

#define SPI_MOSI 23
#define SPI_MISO -1
#define SPI_CLK 18

#define ELINK_SS 5
#define ELINK_BUSY 4
#define ELINK_RESET 16
#define ELINK_DC 17

#define SDCARD_SS 13
#define SDCARD_CLK 14
#define SDCARD_MOSI 15
#define SDCARD_MISO 2

#define BUTTON_PIN 39

typedef struct 
{
    const char * name;
    const unsigned char * data;
    const int width;
    const int height;
} Resource;

Resource resources[] = {{"mouth1", mouth1_data, mouth1_width, mouth1_height},
	{"bm", bm, bmwidth, bmheight},
	{"toothysmile", toothysmile_data,  toothysmile_width, toothysmile_height},
	{"hulk", hulk_data,  hulk_width, hulk_height}
    };
const int NumResources = sizeof(resources) / sizeof(resources[0]);

GxIO_Class io(SPI, /*CS=5*/ ELINK_SS, /*DC=*/ ELINK_DC, /*RST=*/ ELINK_RESET);
GxEPD_Class display(io, /*RST=*/ ELINK_RESET, /*BUSY=*/ ELINK_BUSY);

SPIClass sdSPI(VSPI);


const char *skuNum = "About Fucking Time";
bool sdOK = false;
int startX = 40, startY = 10;
#endif // EPD

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
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
	if (rxValue.back() == '\n') rxValue.pop_back();
	if (rxValue.back() == '\r') rxValue.pop_back();

      if (rxValue.length() > 0) {
	if (rxValue == "resources" )
	{
		pTxCharacteristic->setValue("res,mouth1,bm,toothysmile,hulk");
		pTxCharacteristic->notify();
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
		    display.fillScreen(GxEPD_WHITE);
		    display.drawBitmap(resources[ii].data, 0, 0,  resources[ii].width, resources[ii].height, GxEPD_BLACK);
		    display.update();
		}
	}

//	if (rxValue == "led0") {
//	    display.fillScreen(GxEPD_WHITE);
//	    display.drawBitmap(bm, 0, 0,  bmwidth, bmheight, GxEPD_BLACK);
//	    display.update();
//	}
//	if (rxValue == "led1") {
//	    display.fillScreen(GxEPD_WHITE);
//	    display.drawBitmap(mouth1_data, 0, 0,  mouth1_width, mouth1_height, GxEPD_BLACK);
//	    display.update();
//	}

//        Serial.println("*********");
//        Serial.print("Received Value: ");
//        for (int i = 0; i < rxValue.length(); i++)
//          Serial.print(rxValue[i]);
//
//        Serial.println();
//        Serial.println("*********");
      }
    }
};


void setup() {
  Serial.begin(115200);

  // Create the BLE Device
  BLEDevice::init("UART Service");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

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

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");

#ifdef EPD
    SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, ELINK_SS);
    display.init(); // enable diagnostic output on Serial

    display.setRotation(1);
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(0, 0);

    sdSPI.begin(SDCARD_CLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_SS);

    if (!SD.begin(SDCARD_SS, sdSPI)) {
        sdOK = false;
    } else {
        sdOK = true;
    }

    display.fillScreen(GxEPD_WHITE);

    display.drawBitmap(bm, 0, 0,  bmwidth, bmheight, GxEPD_BLACK);

    //display.setCursor(display.width()  - display.width() / 2, display.height() - 35);
//    display.setCursor(10, display.height() - 35);

//   display.println(skuNum);

//    display.setTextColor(GxEPD_BLACK);

    display.update();

    // goto sleep
//    esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, LOW);

 //   esp_deep_sleep_start();
#endif // EPD
}

void loop() {

//    if (deviceConnected) {
//        pTxCharacteristic->setValue(&txValue, 1);
//        pTxCharacteristic->notify();
//        txValue++;
//		delay(10); // bluetooth stack will go into congestion, if too many packets are sent
//	}

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
}
