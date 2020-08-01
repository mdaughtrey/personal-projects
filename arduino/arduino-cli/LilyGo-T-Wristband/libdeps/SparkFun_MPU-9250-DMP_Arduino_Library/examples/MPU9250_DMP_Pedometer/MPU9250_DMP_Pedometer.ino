/************************************************************
MPU9250_DMP_Pedometer
 Pedometer example for MPU-9250 DMP Arduino Library 
Jim Lindblom @ SparkFun Electronics
original creation date: November 23, 2016
https://github.com/sparkfun/SparkFun_MPU9250_DMP_Arduino_Library

The MPU-9250's digital motion processor (DMP) can estimate
steps taken -- effecting a pedometer.

After uploading the code, try shaking the 9DoF up and
down at a "stepping speed."

Development environment specifics:
Arduino IDE 1.6.12
SparkFun 9DoF Razor IMU M0

Supported Platforms:
- ATSAMD21 (Arduino Zero, SparkFun SAMD21 Breakouts)
*************************************************************/
#include <SparkFunMPU9250-DMP.h>

#define SerialPort Serial
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

MPU9250_DMP imu;

unsigned long stepCount = 0;
unsigned long stepTime = 0;
unsigned long lastStepCount = 0;

char buff[1024];

void setup() 
{
  SerialPort.begin(115200);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(400000);

  // Call imu.begin() to verify communication and initialize
  if (imu.begin() != INV_SUCCESS)
  {
    while (1)
    {
      SerialPort.println("Unable to communicate with MPU-9250");
      SerialPort.println("Check connections, and try again.");
      SerialPort.println();
      delay(5000);
    }
  }
  
  imu.dmpBegin(DMP_FEATURE_PEDOMETER, MAX_DMP_SAMPLE_RATE, buff);
  Serial.println(buff);
  delay(10000);
  imu.dmpSetPedometerSteps(stepCount);
  imu.dmpSetPedometerTime(stepTime);

  memset(buff, 0, 1024);
}

void loop() 
{
  stepCount = imu.dmpGetPedometerSteps();
  stepTime = imu.dmpGetPedometerTime();
  
  if (stepCount != lastStepCount)
  {
    lastStepCount = stepCount;
    SerialPort.print("Walked " + String(stepCount) + 
                     " steps");
    SerialPort.println(" (" + 
              String((float)stepTime / 1000.0) + " s)");
  }
}

