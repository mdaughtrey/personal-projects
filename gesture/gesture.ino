#include <Wire.h>
//#include <SPI.h>
#include <WiFi.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip

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

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

bool pressed = false;
//uint32_t pressedTime = 0;

typedef void (*ScreenFunc)(void *);

void screen0(void * context)
{
    Serial.println("Screen 0");
}

void screen1(void * context)
{
    Serial.println("Screen 1");
}

void screen2(void * context)
{
    Serial.println("Screen 2");
}

void screen3(void * context)
{
    Serial.println("Screen 3");
}

void screenLast(void * context)
{
    Serial.println("Screen Last");
}

ScreenFunc Screens[] = {screen0, screen1 , screen2, screen3, screenLast };
const unsigned char NumScreens = sizeof(Screens)/sizeof(ScreenFunc);
unsigned char screenID = 0;


void setup(void)
{
    Serial.begin(115200);
    Serial.println("Setup");
    tft.init();
    tft.setRotation(1);
    tft.setSwapBytes(true);

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK); // Note: the new fonts do not draw the background colour
    tft.drawString("Setup", 0, 0);

    pinMode(TP_PIN_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
}


// True if new button press
// False if not
bool newPress(void)
{
    if (digitalRead(TP_PIN_PIN) == HIGH)
    {
        Serial.println("TP_PIN_PIN HIGH");
        digitalWrite(LED_PIN, HIGH);
    }
    else
    {
        Serial.println("TP_PIN_PIN LOW");
        digitalWrite(LED_PIN, LOW);
    }
    return false;
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
//    if (digitalRead(TP_PIN_PIN) == HIGH) {
//        if (!pressed) {
//            initial = 1;
//            targetTime = millis() + 1000;
//            tft.fillScreen(TFT_BLACK);
//            omm = 99;
//            func_select = func_select + 1 > 2 ? 0 : func_select + 1;
//            digitalWrite(LED_PIN, HIGH);
//            delay(100);
//            digitalWrite(LED_PIN, LOW);
//            pressed = true;
//            pressedTime = millis();
//        }
//k        else {
//k            if (millis() - pressedTime > 3000) {
//k                tft.fillScreen(TFT_BLACK);
//k                tft.drawString("Reset WiFi Setting",  20, tft.height() / 2 );
//k                delay(3000);
//k                wifiManager.resetSettings();
//k                wifiManager.erase(true);
//k                esp_restart();
//k            }
//k        }
//k    } else {
//k        pressed = false;
//k    }
}

void * ctx;
void loop()
{
	newPress();
//k    if (newPress())
//k    {
//k        screenID++;
//k        screenID %= NumScreens;
//k        Screens[screenID](ctx);
//k    }
}
