#include "Wire.h"
#include "Arduino.h"
//#include "CheapStepper.h"
#define WIFI
#ifdef WIFI
#include "ESP8266WiFi.h"
#include "DNSServer.h"
#include "ESP8266WebServer.h"
#include "WiFiManager.h"
#endif // WIFI
//#define FRAM
#ifdef FRAM
#include "Adafruit_FRAM_I2C.h"
Adafruit_FRAM_I2C fram     = Adafruit_FRAM_I2C();
#endif // FRAM
#define I2CDEV
//#define I2CDEV_FRAM
#define I2CDEV_PWM
#ifdef I2CDEV
#include "I2Cdev.h"
I2Cdev i2c = I2Cdev();
#endif // I2CDEV
//#define BRZO
#ifdef BRZO
#include "brzo_i2c.h"
#endif // BRZO
//#define PINTEST

//CheapStepper stepper (13,12,14,16);
#ifdef WIFI
ESP8266WebServer webServer(80);
#endif // WIFI
boolean cw = true;
int rpm = 12;
uint8_t buffer[8];

#ifdef WIFI
void apCallback(WiFiManager * wmgr)
{
//    webServer.begin();
}
#endif // WIFI

void handleRoot()
{
    String text;
    Serial.println("web server root");
#ifdef WIFI
//    Serial.println(webServer.args());
    String theArgs = String("Args ");
    theArgs = theArgs + "reg1: " + webServer.arg("reg0") + "\r\n";
    theArgs = theArgs + "reg1: " + webServer.arg("reg1") + "\r\n";
    Serial.println(theArgs);
#endif // WIFI
    //rpm = webServer.arg(0).toInt();
#ifdef BRZO
	brzo_i2c_start_transaction(0x5a, 400);
    buffer[0] = 100;
    buffer[1] = 100;
    brzo_i2c_write(buffer, 2, true);
//void brzo_i2c_read(uint8_t *data, uint32_t nr_of_bytes, bool repeated_start);
    //brzo_i2c_ACK_polling(2);
    Serial.println(brzo_i2c_end_transaction());
#endif // BRZO
#ifdef I2CDEV_FRAM
    Wire.begin(4,5);
//    Wire.setClock(100000);
    Wire.beginTransmission(0x50);
    Wire.write(0); // addr high
    Wire.write(0); // addr low
    Wire.write(201);
    Wire.endTransmission();

    Wire.beginTransmission(0x50);
    Wire.write(atoi(webServer.arg("reg0)")); // addr high
    Wire.write(atoi(webServer.arg("reg1)")); // addr high
    Wire.endTransmission();
//
//    Serial.print("requestFrom ");
    Wire.requestFrom(0x50, (uint8_t)1);
    text.concat(Wire.read());
    text.concat("\r\n");
//    Serial.print("buffer 0 ");
//    Serial.println(Wire.read());
#endif // I2CDEV
#ifdef I2CDEV_PWM
    Serial.println("I2CDEV_PWM");
    Wire.begin(4,5);
    Wire.beginTransmission(0x5a);
    Wire.write(0); // reg
    int reg0(atoi(webServer.arg("reg0").c_str())); 
    int reg1(atoi(webServer.arg("reg1").c_str())); 
    Serial.print("reg0: ");
    Serial.println(reg0);
    Serial.print("reg1: ");
    Serial.println(reg1);
    Wire.write(reg0);
    Wire.write(reg1);
    Wire.endTransmission();

    Wire.beginTransmission(0x5a);
    Wire.write(0); // addr high
    Wire.endTransmission();
    text = " requestfrom: " + Wire.requestFrom(0x5a, (uint8_t)2);
    text = text + Wire.read() + " " + Wire.read();
    Serial.print("text so far ");
    Serial.println(text);
//    text.concat(Wire.read());
#endif //  I2CDEV_PWM

    Serial.println("I2C done");
    Serial.println(text);
#ifdef FRAM
    uint16_t manu;
    uint16_t prod;
    fram.begin(0x50);
    fram.getDeviceID(&manu, &prod);
    Serial.print("manu ");
    Serial.println(manu);
    Serial.print("prod ");
    Serial.println(prod);
    fram.write8(0x00, 210);
    Serial.print("FRAM read ");
    Serial.println(fram.read8(0));
#endif // FRAM
    webServer.send(200, "text/html", "<HTML><BODY>Success</BODY></HTML>");
}

void saveWifiConfig()
{
    // save config
}

void setup()
{
    Serial.begin(9600);
    Serial.println("Init start");
#ifdef WIFI
    WiFiManager wifiManager;
    wifiManager.setDebugOutput(true);
    Serial.println("autoConnect start");
    wifiManager.autoConnect("strandbeast");
    Serial.println("autoConnect end");
//    wifiManager.startConfigPortal("strandbeast");
    //stepper.setRpm(24);
    //stepper.setRpm(12);

    Serial.println("Start web server");
    webServer.on("/", handleRoot);
    webServer.begin();
#endif // WIFI
//    stepper.newMoveTo(cw, 2048);
//    Serial.println("newMoveTo");
#ifdef BRZO
    brzo_i2c_setup(14, 16, 2000);
#endif // BRZO
#ifdef I2CDEV
    I2Cdev();
#endif // I2CDEV
#ifdef PINTEST
    pinMode(14, OUTPUT);
    digitalWrite(14, HIGH);
    pinMode(16, OUTPUT);
    digitalWrite(16, HIGH);
#endif // PINTEST
}

void loop()
{
#ifdef WIFI
    webServer.handleClient();
#endif // WIFI
#ifdef PINTEST
    digitalWrite(14, HIGH);
    digitalWrite(16, LOW);
    digitalWrite(14, LOW);
    digitalWrite(16, HIGH);
#endif // PINTEST
    //stepper.setRpm(rpm);
    //stepper.run();
//    if (rpm && 0 == stepper.getStepsLeft())
//    {
//        cw = !cw;
//        stepper.newMoveTo(cw, 2048);
//    }
}
