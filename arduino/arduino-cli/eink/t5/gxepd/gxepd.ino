#define ENABLE_GxEPD2_GFX 0
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <SPI.h>
//#include <GDBStub.h>

// # plain
// # b red/black/white
// # c yellow/black/white
// # d plain flexible
// 
// #/ Good Display GDEH0213B72
// #/ HINK-E0213A22
// #/ IL3897 / ssd16xx SSD1675
// #/ waveshare_epaper_gdeh0213b72
// #/ https://v4.cecdn.yun300.cn/100001_1909185148/GDEH0213B72.pdf
// # Busy IO4
// # Reset IO16
// # MOSI IO23
#define MOSI 23
// # CLK IO18
#define SCK 18
#define MISO -1
#define SS -1
// # DC IO17
// # CS IO5

//SPIClass hSPI;
//GxEPD2_BW<GxEPD2_213_B72, GxEPD2_213_B72::HEIGHT> display(GxEPD2_213_B72(/*CS=5*/ 5, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEH0213B72

//    GxEPD2_213_B72(SPIClass & spi, int8_t cs, int8_t dc, int8_t rst, int8_t busy);

const char HelloWorld[] = "Merediths Tits!";
void hello()
{
GxEPD2_BW<GxEPD2_213_B72, GxEPD2_213_B72::HEIGHT> display(GxEPD2_213_B72(/*CS=5*/ 5, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEH0213B72
    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display.setRotation(1);
    Serial.println("setRotation");
    display.setFont(&FreeMonoBold9pt7b);
    Serial.println("setFont");
    display.setTextColor(GxEPD_BLACK);
    Serial.println("getTextColor");
    display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
    Serial.println("getTextBounds");

    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    uint16_t y = ((display.height() - tbh) / 2) - tby;
    display.setFullWindow();
    Serial.println("setFullWindow");
    display.firstPage();
    Serial.println("firstPage");
}


void setup() {
    SPI.begin();
    Serial.begin(115200);
    Serial.println("Sleeping");
    delay(1000);
    hello();
    Serial.println("I said HELLO");
}

void loop() {
}

