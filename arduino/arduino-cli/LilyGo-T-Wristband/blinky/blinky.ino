#include <Wire.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

#define LED_PIN             4


TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

void setup() {
    pinMode(LED_PIN, OUTPUT);
    tft.init();
    tft.setRotation(1);
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_RED);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Do up your zipper." ,10, tft.height() / 2);
}

void loop() {
digitalWrite(LED_PIN, HIGH);
delay(1000);
digitalWrite(LED_PIN, LOW);
delay(1000);
}
