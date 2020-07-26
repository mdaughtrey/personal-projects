#include <Wire.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SparkFunMPU9250-DMP.h>
//#include "sensor.h"
#define DOSCREENS
#ifdef DOSCREENS
#include "screens.h"
#endif // DOSCREENS

#define TP_PIN_PIN          33
#define I2C_SDA_PIN         21
#define I2C_SCL_PIN         22
#define IMU_INT_PIN         38
#define RTC_INT_PIN         34
#define BATT_ADC_PIN        35
#define VBUS_PIN            36
#define TP_PWR_PIN          25
#define LED_PIN             4
#define CHARGE_PIN          32

//MPU9250 IMU;
MPU9250_DMP imu;
char buff[256];
const String alpha("abcdefghijklmnopqrstuvwxyz");

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

bool pressed = false;

#ifdef DOSCREENS
const GFXfont * freeFonts[] = {&FreeSans9pt7b, &FreeSans12pt7b, &FreeSans18pt7b, &FreeSans24pt7b };
FreeFontsDemo ffDemo(&tft, &imu, freeFonts);
IMUStats imuStats(&tft, &imu);
NormIMU normImu(&tft, &imu);
Sketch sketch(&tft, &imu);
TextSelection textSelection(&tft, &imu);

Screen * screens[] = { &ffDemo, &imuStats, &normImu, &sketch, &textSelection };
//Screen * screens[] = { &textSelection };
const unsigned char NumScreens = sizeof(screens)/sizeof(Screen *);

unsigned char screenID = 0;
#endif // DOSCREENS


void setup(void)
{
    Serial.begin(115200);
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(400000);

    tft.init();
    tft.setRotation(1);
    tft.setSwapBytes(true);

//    setupMPU9250();
    pinMode(TP_PWR_PIN, PULLUP);
    digitalWrite(TP_PWR_PIN, LOW);
    delay(100);
    digitalWrite(TP_PWR_PIN, HIGH);
    pinMode(TP_PIN_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    if (INV_SUCCESS != imu.dmpBegin(DMP_FEATURE_TAP, 10))
        Serial.println("dmpBegin fails");
    // dmpSetTap parameters, in order, are:
    // x threshold: 1-1600 (0 to disable)
    // y threshold: 1-1600 (0 to disable)
    // z threshold: 1-1600 (0 to disable)
    // (Threshold units are mg/ms)
    // taps: Minimum number of taps needed for interrupt (1-4)
    // tap time: milliseconds between valid taps
    // tap time multi: max milliseconds between multi-taps
    unsigned short xThresh = 0;   // Disable x-axis tap
    unsigned short yThresh = 0;   // Disable y-axis tap
    unsigned short zThresh = 100; // Set z-axis tap thresh to 100 mg/ms
    unsigned char taps = 1;       // Set minimum taps to 1
    unsigned short tapTime = 100; // Set tap time to 100ms
    unsigned short tapMulti = 1000;// Set multi-tap time to 1s
    if(INV_SUCCESS != imu.dmpSetTap(xThresh, yThresh, zThresh, taps, tapTime, tapMulti))
    {
        Serial.println("dmpSetTap fails");
    }

//      if (imu.begin() != INV_SUCCESS)
//      {
//            tft.fillScreen(TFT_BLACK);
//            tft.setTextColor(TFT_WHITE);
//          tft.drawString("Unable to communicate with MPU-9250", 0, 0, 1);
//        while (1)
//        {
//          delay(5000);
//        }
//    }
#ifdef DOSCREENS
    screens[screenID]->in();
#endif // DOSCREENS
}


// True if new button press
// False if not
bool newPress(void)
{
    if (!pressed && digitalRead(TP_PIN_PIN) == HIGH)
    {
        Serial.print(".");
        pressed = true;
        return true;
    }
    if (pressed && digitalRead(TP_PIN_PIN) == LOW)
    {
        pressed = false;
    }
    return false;
}

void loop()
{
#ifdef DOSCREENS
    if (newPress())
    {
        screens[screenID]->out();
        screenID++;
        screenID %= NumScreens;
        screens[screenID]->in();
    }
    screens[screenID]->loop();
#endif // DOSCREENS
}
//#error you were about to integrate mpu9250_basic into gesture

