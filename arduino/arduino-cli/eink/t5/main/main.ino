#include <Wire.h>
#include <Arduino.h>

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
// # CLK IO18
// # DC IO17
// # CS IO5

int pins[] = {4, 16, 23, 18, 17, 5, 19};
const int d = 10;

void setup() {
    Serial.begin(115200);
    Serial.println("Hello");

    for (int ii = 0; ii < 7; ii++)
    {
        pinMode(pins[ii], OUTPUT);
    }
}

void loop() {
    for (int ii = 0; ii < 7; ii++)
    {
        digitalWrite(pins[ii], HIGH);
        delay(d);
    }
    for (int ii = 0; ii < 7; ii++)
    {
        digitalWrite(pins[ii], LOW);
        delay(d);
    }
    
}
