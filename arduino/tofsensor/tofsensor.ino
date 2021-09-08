#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
    Serial.begin(115200);
    delay(1000);
    pinMode(25, OUTPUT);
    while (!lox.begin())
    {
        Serial.println("VL53L0X Init fails");
        delay(2000);
    }
    Serial.println("setup complete");
}

void loop() {
    Serial.println("Sure, whatever.");
        digitalWrite(25, HIGH);
        delay(500);
        digitalWrite(25, LOW);
        delay(500);
}

