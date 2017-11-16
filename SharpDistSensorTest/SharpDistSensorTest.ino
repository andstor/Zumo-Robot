/*
  SharpDistSensorBasic.ino
  Source: https://github.com/DrGFreeman/SharpDistSensor

  MIT License

  Copyright (c) 2017 Julien de la Bruere-Terreault <drgfreeman@tuta.io>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

/*
  This example shows how to use the SharpDistSensor library to continuously
  read the sensor and display the analog value and the corrseponding distance
  in mm.

  The library default values corresponding to the Sharp GP2Y0A60SZLF 5V sensor
  are used.

  See the library README for how to use pre-defined sensor models or custom
  fit functions.
*/

#include <SharpDistSensor.h>

// Analog pin to which the sensor is connected
const byte sensorPin1 = A2;
const byte sensorPin2 = A1;

// Window size of the median filter (odd number, 1 = no filtering)
const byte mediumFilterWindowSize = 5;

// Create an object instance of the SharpDistSensor class
SharpDistSensor sensor1(sensorPin1, mediumFilterWindowSize);
SharpDistSensor sensor2(sensorPin2, mediumFilterWindowSize);



void setup() {
  Serial.begin(9600);
}

void loop() {
  // Get distance from sensor
  unsigned int distance1 = sensor1.getDist();
  unsigned int distance2 = sensor2.getDist();

  // Print distance to Serial
  Serial.print("1: ");
  Serial.print(distance1);
  Serial.print("      2: ");
  Serial.println(distance2);

 
  // Wait some time
  delay(50);
}
