#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
    Serial.begin(115200);
    while (!lox.begin())
    {
        Serial.println("VL53L0X Init fails");
        delay(2000);
    }
    Serial.println("setup complete");
}

void loop() {
    VL53L0X_RangingMeasurementData_t measure;
    Serial.print("Reading...");
    lox.rangingTest(&measure, false);
    if (measure.RangeStatus != 4)
    {
        Serial.print(measure.RangeMilliMeter);
        Serial.println("mm");
    }
    else
    {
        Serial.println("Out of range");
    }
    delay(100);
}

