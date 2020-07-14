#include <WiFi.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

void setup(void)
{
    Serial.begin(115200);
    tft.init();
    tft.setRotation(1);
    tft.setSwapBytes(true);

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK); // Note: the new fonts do not draw the background colour
    tft.drawString("Setup", 0, 0);
}


void loop()
{
    Serial.println("Hello!");
    delay(1000);
}
