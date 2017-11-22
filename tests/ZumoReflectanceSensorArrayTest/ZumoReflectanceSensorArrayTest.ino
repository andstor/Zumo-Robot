/*
Test for the ZumoReflectanceSensorArray
*/
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>

ZumoReflectanceSensorArray reflectanceSensors;

// Define an array for holding sensor values.
#define NUM_SENSORS 6
unsigned int sensorValues[NUM_SENSORS];

void setup()
{
  Serial.begin(9600);
  reflectanceSensors.init();

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);        // turn on LED to indicate we are in calibration mode

}

void loop()
{
  // Get calibrated sensor data
  reflectanceSensors.read(sensorValues); 
  
  for (byte i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print(' ');
  }
  Serial.println("    ");
  
  delay(250);
}
