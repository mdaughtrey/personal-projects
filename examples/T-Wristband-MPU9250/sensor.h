#ifndef _SENSOR_H_
#define _SENSOR_H_

//#include "MPU9250.h"
//#include "config.h"
//#include "quaternionFilters.h"


#define AHRS true         // Set to false for basic data read
#define SerialDebugMPU9250 0  // Set to true to get Serial output for debugging
#define SerialDebugCalibrate false  // Set to true to get Serial output for debugging

//MPU9250 IMU;

void setupMPU9250(); 
void readMPU9250();

#endif // _SENSOR_H_
