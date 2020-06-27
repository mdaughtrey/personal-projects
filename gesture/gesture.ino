#include <Wire.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include "sensor.h"
#include "screens.h"

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

MPU9250 IMU;
char buff[256];
const String alpha("abcdefghijklmnopqrstuvwxyz");

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

bool pressed = false;

const GFXfont * freeFonts[] = {&FreeSans9pt7b, &FreeSans12pt7b, &FreeSans18pt7b, &FreeSans24pt7b };
FreeFontsDemo ffDemo(&tft, &IMU, freeFonts);
IMUStats imuStats(&tft, &IMU);
NormIMU normImu(&tft, &IMU);
Sketch sketch(&tft, &IMU);
TextSelection textSelection(&tft, &IMU);

Screen * screens[] = { &ffDemo, &imuStats, &normImu, &sketch, &textSelection };
const unsigned char NumScreens = sizeof(screens)/sizeof(Screen *);

unsigned char screenID = 0;

void setup(void)
{
    Serial.begin(115200);
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(400000);

    tft.init();
    tft.setRotation(1);
    tft.setSwapBytes(true);

    setupMPU9250(IMU);
    pinMode(TP_PWR_PIN, PULLUP);
    digitalWrite(TP_PWR_PIN, HIGH);
    pinMode(TP_PIN_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    screens[screenID]->in();
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
    if (newPress())
    {
        screens[screenID]->out();
        screenID++;
        screenID %= NumScreens;
        screens[screenID]->in();
    }
    screens[screenID]->loop();
}

