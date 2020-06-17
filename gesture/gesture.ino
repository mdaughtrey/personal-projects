#include <Wire.h>
//#include <SPI.h>
//#include <WiFi.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include "sensor.h"

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

extern MPU9250 IMU;
char buff[256];
const char alpha[] = "  abcdefghijklmnopqrstuvwxyz  ";

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

bool pressed = false;
//uint32_t pressedTime = 0;

float imuAXMin = 0.0;
float imuAXMax = 0.0;
float imuAYMin = 0.0;
float imuAYMax = 0.0;
float imuAZMin = 0.0;
float imuAZMax = 0.0;



typedef struct 
{
	uint8_t count;
} Context0;

typedef struct 
{
	uint8_t current;
} Context4;

typedef struct {
    Context0 ctx0;
	Context4 ctx4;
} Context;

typedef void (*ScreenFunc)(Context &);
Context context;
	

void screen0Init(Context & context)
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE); // Note: the new fonts do not draw the background colour
    context.ctx0.count = 0;
    sprintf(buff, "%04x", tft.fontsLoaded());
    tft.drawString(buff, 10, 6, 1);
    tft.drawString(buff, 10, 24, 2);
//    delay(1000);
}

void screen0Loop(Context & context)
{
    uint8_t & count(context.ctx0.count);
    sprintf(buff, "%u", count);
    tft.fillScreen(TFT_BLACK);
    tft.drawString(buff, 10, 8, count);
    count = (count + 1) % 9;
    delay(1000);
}

void screen1Init(Context & context)
{
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
}

float imuXRange[2] = {-0.4314, 1.6202};
float imuYRange[2] = {-1.4550, 0.6350};
float imuZRange[2] = {-1.9759, 0.5992};

void screen1Loop(Context & context)
{
//    tft.drawString("1", tft.width()/2, 5, 7); // Overwrite the text to clear it
    tft.setTextDatum(TL_DATUM);
    readMPU9250();
    tft.fillScreen(TFT_BLACK);
    tft.drawString(buff, 0, 16);

    imuAXMin = std::min(imuAXMin, IMU.ax); 
    imuAXMax = std::max(imuAXMax, IMU.ax); 
 
    imuAYMin = std::min(imuAYMin, IMU.ay); 
    imuAYMax = std::max(imuAYMax, IMU.ay); 
 
    imuAZMin = std::min(imuAZMin, IMU.az); 
    imuAZMax = std::max(imuAZMax, IMU.az); 
 
    tft.fillScreen(TFT_BLACK);
    snprintf(buff, sizeof(buff), "X %.4f  %.4f  %.4f", imuAXMin, IMU.ax, imuAXMax);
    tft.drawString(buff, 0, 16);
    snprintf(buff, sizeof(buff), "Y %.4f  %.4f  %.4f", imuAYMin, IMU.ay, imuAYMax);
    tft.drawString(buff, 0, 32);
    snprintf(buff, sizeof(buff), "Z %.4f  %.4f  %.4f", imuAZMin, IMU.az, imuAZMax);
    tft.drawString(buff, 0, 48);
    delay(200);
}

void screen2Init(Context & context)
{
    tft.fillScreen(TFT_BLUE);
    tft.setTextColor(TFT_YELLOW, TFT_BLUE); // Note: the new fonts do not draw the background colour
    tft.drawString("2", tft.width()/2, 5, 7); // Overwrite the text to clear it
}

void screen2Loop(Context & context)
{
    readMPU9250();
    float x = ((IMU.ax - imuXRange[0]) / (imuXRange[1] - imuXRange[0])) * (10.0 - -10.0) + -10.0;
    snprintf(buff, sizeof(buff), "X %.4f", x);
    tft.fillScreen(TFT_BLUE);
    tft.drawString(buff, 0, 16);
    float y = ((IMU.ay - imuYRange[0]) / (imuYRange[1] - imuYRange[0])) * (10.0 - -10.0) + -10.0;
    snprintf(buff, sizeof(buff), "Y %.4f", y);
    tft.drawString(buff, 0, 24);
    delay(200);
}


int16_t dotX;
int16_t dotY;

void screen3Init(Context & context)
{
    tft.fillScreen(TFT_RED);
    dotY = tft.width() / 2;
    dotX = tft.height() / 2;
}

void screen3Loop(Context & context)
{
    readMPU9250();
    float x = ((IMU.ax - imuXRange[0]) / (imuXRange[1] - imuXRange[0])) * (10.0 - -10.0) + -10.0;
    float y = ((IMU.ay - imuYRange[0]) / (imuYRange[1] - imuYRange[0])) * (10.0 - -10.0) + -10.0;
    dotX -= (int)x;
    dotY += (int)y;
    dotY = std::min(dotY, tft.width());
    dotY = std::max(dotY, (int16_t)0);

    dotX = std::min(dotX, tft.height());
    dotX = std::max(dotX, (int16_t)0);
    tft.drawPixel(dotY, dotX, TFT_WHITE);
    delay(20);
}

// Tap Detection
void screen4Init(Context & context)
{
    context.ctx4.current = sizeof(alpha)/2;
//    IMU.dmpBegin(DMP_FEATURE_TAP, 10);
//    IMU.dmpSetTap(0, 0, 100, 1,100,1000);
    tft.fillScreen(TFT_WHITE);
    tft.setTextColor(TFT_BLACK, TFT_WHITE); // Note: the new fonts do not draw the background colour
//    tft.setTextColor(TFT_YELLOW, TFT_BROWN); // Note: the new fonts do not draw the background colour
//    tft.drawString("Tap Detection", 10, 10, 2);
//    tft.setTextColor(TFT_WHITE, TFT_BROWN); // Note: the new fonts do not draw the background colour
}

void screen4Loop(Context & context)
{
    Context4 & ctx (context.ctx4);
    readMPU9250();
    int x = (int)(((IMU.ax - imuXRange[0]) / (imuXRange[1] - imuXRange[0])) * (10.0 - -10.0) + -10.0);
    int y = (int)(((IMU.ay - imuYRange[0]) / (imuYRange[1] - imuYRange[0])) * (10.0 - -10.0) + -10.0);
    sprintf(buff, "y %d", y);
    tft.drawString(buff, 0, 0);
//    if (y >= -1 || y <= 1) return;
    if (y < -1) ctx.current--;
    else if (y > 1) ctx.current++;

    if (ctx.current < 2) ctx.current = 2;
    if (ctx.current > sizeof(alpha) - 2) ctx.current = sizeof(alpha) -2;

    sprintf(buff, "c %d", ctx.current);
    tft.drawString(buff, 0, 8);

    strncpy(buff, alpha + ctx.current - 2, 5);
    tft.drawString(buff, 20, 16, 2);
    delay(100);
}


void screenLastInit(Context & context)
{
    tft.fillScreen(TFT_BROWN);
    tft.setTextColor(TFT_YELLOW, TFT_BROWN); // Note: the new fonts do not draw the background colour
    tft.drawString("X", tft.width()/2, 5, 7); // Overwrite the text to clear it
}

void screenLastLoop(Context & context)
{
}

ScreenFunc Loop[] = {screen0Loop, screen1Loop , screen2Loop, screen3Loop, screen4Loop, screenLastLoop };
const unsigned char NumLoop = sizeof(Loop)/sizeof(ScreenFunc);
ScreenFunc Init[] = {screen0Init, screen1Init , screen2Init, screen3Init, screen4Init, screenLastInit };

unsigned char screenID = 0;


void setup(void)
{
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(400000);

    Serial.begin(115200);
    tft.init();
    tft.setRotation(1);
    tft.setSwapBytes(true);

    pinMode(TP_PWR_PIN, PULLUP);
    digitalWrite(TP_PWR_PIN, HIGH);
    pinMode(TP_PIN_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    screen0Init(context);
}


// True if new button press
// False if not
bool newPress(void)
{
//     if (digitalRead(TP_PIN_PIN) == HIGH)
//     {
//         Serial.println("TP_PIN_PIN HIGH");
//         digitalWrite(LED_PIN, HIGH);
//     }
//     else
//     {
//         Serial.println("TP_PIN_PIN LOW");
//         digitalWrite(LED_PIN, LOW);
//     }
//     return false;
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

void loop()
{
    if (newPress())
    {
        screenID++;
        screenID %= NumLoop;
        Init[screenID](context);
    }
    Loop[screenID](context);
}
