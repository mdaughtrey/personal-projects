#include <SparkFun_APDS9960.h>

SparkFun_APDS9960 gestureSensor;
const unsigned int MoveTimeout = 300;
const unsigned char ProxThreshold = 35;
const unsigned short GESTURE_INT_PIN = 1;
//unsigned int moveTime = 0;
volatile boolean hasMovement = false;
//boolean gesture = true;
const unsigned char LCDL = 15;
const unsigned char LCDR = 14;

/**
   The main business logic. Translates gesturkjes to keyboard events.
*/
void handleGesture() {
  if (gestureSensor.isGestureAvailable()) {
//   Serial.println("ga");
//      moveTime = millis()/1000;
    switch (gestureSensor.readGesture()) {
      case DIR_UP:
//        Serial.println("UP");
//      gesture = true;
        digitalWrite(LCDR, HIGH);
        digitalWrite(LCDL, HIGH);
        break;
      case DIR_DOWN:
//      Serial.println("DOWN");
//      gesture = false;
//      digitalWrite(16, LOW);// turn off
        digitalWrite(LCDR, LOW);
        digitalWrite(LCDL, LOW);
        break;
      case DIR_LEFT:
        digitalWrite(LCDL, LOW);
        digitalWrite(LCDR, HIGH);
//      Serial.println("LEFT");
        break;
      case DIR_RIGHT:
        digitalWrite(LCDR, LOW);
        digitalWrite(LCDL, HIGH);
//      Serial.println("RIGHT");
        break;
      case DIR_NEAR:
//      Serial.println("NEAR");
        digitalWrite(LCDR, LOW);
        digitalWrite(LCDL, LOW);
        break;
      case DIR_FAR:
      //Serial.println("FAR");
        digitalWrite(LCDR, HIGH);
        digitalWrite(LCDL, HIGH);
        break;
      default:
//      Serial.println("default");
        break;
    }
  }
//  Serial.println("xga");
}

/**
   Gets called whenever the gesture module signals there are new data to be sent
*/
void transmissionReady() {
  hasMovement = true;
}

/**
   Initializes gesture sensor
  @return whether gesture sensor initialization was successful
*/
boolean initializeGestureSensor() {
  if(!gestureSensor.init())
  {
    //Serial.println("init fail");
  }
  if(!gestureSensor.enableGestureSensor())
  {
    //Serial.println("enable fail");
  }
  return gestureSensor.init() && gestureSensor.enableGestureSensor(true);
}

void setup() {
//    delay(10000);
//    Serial.begin(9600);
//    Serial.println("init");

  attachInterrupt(digitalPinToInterrupt(GESTURE_INT_PIN), transmissionReady, FALLING);
  boolean initializationSuccessful = initializeGestureSensor();
  // If ADPS sensor failed to initialize, block and send error messages via Serial
  if (!initializationSuccessful) {
    while (!initializationSuccessful) {
//      Serial.println("Error while initializing the gesture sensor");
      delay(10000);
    }
  }
  pinMode(LCDR, OUTPUT);
  pinMode(LCDL, OUTPUT);
  digitalWrite(LCDR, HIGH);
  digitalWrite(LCDL, HIGH);
}

//unsigned char prox = 0;
void loop() {
//    Serial.println("Hello");
//    delay(2000);
//    return;
//  Serial.println(millis(), 10);
  if (hasMovement)
  { // sensed a movement
    handleGesture();
    hasMovement = false;
    // Reinitialize as workaround for sensor hanging when gesturing too fast
    initializeGestureSensor();
  }
  
//  if (gesture) digitalWrite(16, digitalRead(15));
}
