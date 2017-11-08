/*
 * Name:      ZumoRobot.ino
 * Created:   08/10/2017  13:00:00 PM
 * Authors:   André Storaug, Eirik Thue, ....
 */
 #include "ZumoMotors.h"


const int LED_PIN = 13;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_PIN, HIGH);

/*
  for (int time = 0; time <= 500; time++) {
    int speed = 100;

    motors.setLeftSpeed(speed-50);
    motors.setRightSpeed(speed+50);
    delay(2);
  }

  for (int time = 0; time <= 500; time++) {
    int speed = -100;

    motors.setLeftSpeed(speed-50);
    motors.setRightSpeed(speed+50);
    delay(2);
  }
*/





}
